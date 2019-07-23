/****************************************************
*flash.c
*This file contains functions to facilitate storing and
* reading stored data to and from the onboard flash memory
****************************************************/

//Includes
#include "comport.h"
#include "globals.h"
#include "main.h"
#include "flash.h"
#include <string.h>
//Software init



//---------------------------------------------------
int FLASH_Unlock()
{
	/* Unlocking FLASH_PECR register access */
	if((FLASH->PECR & FLASH_PECR_PELOCK) != RESET)
	{  
		//FLASH->PEKEYR = FLASH_PEKEYR_PEKEYR_Msk;
		FLASH->PEKEYR = UNLOCK_KEY_1;
		FLASH->PEKEYR = UNLOCK_KEY_2;
		return 0; //No err
	}
	return -1; //Not reset	 
}
//---------------------------------------------------
int FLASH_Lock()
{
	while ((FLASH->SR & FLASH_SR_BSY) != 0) //Wait for not busy
	{
	 /* For robust implementation, add here time-out management */
	}
	FLASH->PECR |= FLASH_PECR_PELOCK; //Lock flash write
	
	return 0;
}
//---------------------------------------------------
int EEPROM_WriteByte(unsigned char byte, unsigned int addr)
{
	/*
	if((FLASH->PECR & FLASH_PECR_PELOCK) == RESET)
	{
		return -1; //Write protected
	}*/
	
	unsigned char * ptr = (unsigned char *) addr;
	*ptr = byte;
	return 0; //No err	
}
//---------------------------------------------------
int EEPROM_WriteData(unsigned char * str, int len, unsigned int addr)
{
	int i;
	for(i=0; i<len; i++)
	{
		if(EEPROM_WriteByte(*(str), addr))
		{
			return -1; //Error
		}
		str++;
		addr++;
	}
	return 0; //No err
}
//---------------------------------------------------
int FLASH_ReadData(unsigned char * str, int len, unsigned int addr)
{
	int i;
	unsigned char * ptr = (unsigned char *) addr;
	
	for(i=0; i<len; i++)
	{
		*str =  *ptr;
		str++;
		ptr++;
	}
	return 0;
}
//---------------------------------------------------
int FLASH_unlockPrgm(void){
	/* Unlocking FLASH_PECR register access */
	if((FLASH->PECR & FLASH_PECR_PRGLOCK) != RESET)
	{ 
		
		FLASH->PRGKEYR = PRGKEY1;
		FLASH->PRGKEYR = PRGKET2;
		FLASH->PECR &= ~FLASH_PECR_PRGLOCK;
		return 0; //No err
	}
	return -1; //Not reset
}
//---------------------------------------------------
int FLASH_writeWord( uint32_t data, unsigned int addr){
	__disable_irq();
	while ((FLASH->SR & FLASH_SR_BSY) != 0);
	*(__IO uint32_t*)(addr) = data;
	while ((FLASH->SR & FLASH_SR_BSY) != 0)
	{
		/* For robust implementation, add here time-out management */
	}
	__enable_irq();
	if ((FLASH->SR & FLASH_SR_EOP) != 0)
	{
		FLASH->SR = FLASH_SR_EOP;
		return 0;
	}else{
		// Error
		return -1;
	}
}
//---------------------------------------------------
int FLASH_erasePage(unsigned int addr){
	
	FLASH->PECR |= FLASH_PECR_ERASE | FLASH_PECR_PROG; 
	*(__IO uint32_t *)addr = (uint32_t)0;
	while ((FLASH->SR & FLASH_SR_BSY) != 0);
	if ((FLASH->SR & FLASH_SR_EOP) != 0)
	{
		FLASH->SR = FLASH_SR_EOP;
	}else{
		return -1;
	}
	FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_PROG);
	return 0;
}
//---------------------------------------------------
int FLASH_initDataStorage(void){
	uint32_t addr = NEXTSTARTADDR_ADDR;
	uint32_t data = DATA_START_ADDR;
	uint8_t state = 0;
	
	FLASH_Unlock();
	EEPROM_WriteData((unsigned char *)&data, 4, addr);
	
	FLASH_unlockPrgm();
	FLASH_erasePage( DATA_START_ADDR );
	FLASH_Lock();
	
	return state;	
}
//---------------------------------------------------
int FLASH_saveFix( struct GPS_POS position){
	
	uint32_t startAddr;
	uint32_t nextStartAddr;
	
	char str1[4], str2[4], str3[4], str4[4];
	char str5[4], str6[4], str7[4], str8[4];
	
	strcpy( str1, position.time);				// All of time into str1
	strncpy(str2, position.date, 4);		// First 4 of date into str2 (4/6)
	strncpy(str3, position.date+4, 2);	// Rest of date into str3 (6/6)
	strncpy(str3, position.lat, 2);			// First 2 of lat into str3 (2/10)
	strncpy(str4, position.lat+2, 4);		// Next 4 of lat (6/10)
	strncpy(str5, position.lat+6, 4);		// Final 4 of lat
	str6[0] = position.NS;							// Copy NS
	strncpy(str6, position.longt, 3);		// Copy first 3 of longt into str6 (3/10)
	strncpy(str7, position.longt+3, 4);	// Next 4 of longt (7/10)
	strncpy(str8, position.longt+7, 3);	// Final 3 of longt (10/10)
	str8[3] = position.EW;
	
	
	FLASH_ReadData( &startAddr, 4, NEXTSTARTADDR_ADDR);
	nextStartAddr = startAddr + DATA_LENGTH;
	
	if( startAddr >= END_OF_MEMORY) return -1;
	
	FLASH_Unlock();
	EEPROM_WriteByte( nextStartAddr, NEXTSTARTADDR_ADDR);
	FLASH_unlockPrgm();
	if( (startAddr % 128) != 0){
			FLASH_erasePage( startAddr );
	}
	FLASH_writeWord( &str1[0], startAddr);
	FLASH_writeWord( &str2[0], startAddr+4);
	FLASH_writeWord( &str3[0], startAddr+8);
	FLASH_writeWord( &str4[0], startAddr+12);
	FLASH_writeWord( &str5[0], startAddr+16);
	FLASH_writeWord( &str6[0], startAddr+20);
	FLASH_writeWord( &str7[0], startAddr+24);
	FLASH_writeWord( &str8[0], startAddr+28);
	
	return 0;
}
//---------------------------------------------------