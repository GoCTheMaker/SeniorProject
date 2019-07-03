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
int FLASH_WriteByte(unsigned char byte, unsigned long addr)
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
int FLASH_WriteData(unsigned char * str, int len, unsigned long addr)
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
int FLASH_ReadData(unsigned char * str, int len, unsigned long addr)
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