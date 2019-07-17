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
}
//---------------------------------------------------
int FLASH_WriteByte(unsigned char byte, unsigned int addr)
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
int FLASH_WriteData(unsigned char * str, int len, unsigned int addr)
{
	int i;
	for(i=0; i<len; i++)
	{
		if(FLASH_WriteByte(*(str), addr))
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
int FLASH_writeWord( unsigned int data, unsigned int addr){
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
		return 1;
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
		return 1;
	}
	FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_PROG);
	return 0;
}
//---------------------------------------------------
int FLASH_saveFix(struct GPS_POS position){
	
}
//---------------------------------------------------