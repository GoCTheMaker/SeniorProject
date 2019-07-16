/****************************************************
*gps.c
*This file contains functions to facilitate communications
* with GPS device for data storage
****************************************************/

//Includes
#include "globals.h"
#include "main.h"
#include <string.h>
#include "gps.h"

//Software init

//---------------------------------------------------
void GPS_GPSEnable(void)
{
	GPS_EN_GPIO_Port->ODR &= ~GPS_EN_Pin;	//Disable EN pin, Logic low 
	return;
}
//---------------------------------------------------
void GPS_GPSDisable(void)
{
	GPS_EN_GPIO_Port->ODR |= GPS_EN_Pin; //Enable EN pin, Logic high
	return;
}
//---------------------------------------------------
void GPS_GPSCSLow(void)
{
	//To be used when SPI coms initialized with gps
	GPS_CS_GPIO_Port->ODR &= ~GPS_CS_Pin;
	return;
}
//---------------------------------------------------
void GPS_GPSCSHigh(void)
{
	//End coms with GPS chip
	GPS_CS_GPIO_Port->ODR |= GPS_CS_Pin;
	return;
}
//---------------------------------------------------
void GPS_SendByte(char byte)
{
	//Send data and wait for busy to clear
	LL_SPI_TransmitData8(SPI1, byte);
	while(SPI1->SR & 0x08);
	return;
}
//---------------------------------------------------
void GPS_SendData(char * data, int size)
{
	int i;
	char dataToTx;
	for(i = 0; i < size; i++)
	{
		dataToTx = *data;
		//GPS_SendByte(* data);
		GPS_SendByte( dataToTx );
		data++;
	}
	
	return; //No err	
}
//---------------------------------------------------
void GPS_ClockInByte(char * byte)
{
	GPS_SendByte(IDLE_LINE);
	*byte = LL_SPI_ReceiveData8(SPI1);
	return;
}
//---------------------------------------------------
int GPS_GetDataWord(char * data, int len)
{
	int i;
	char * ptr = data;//This way the data pointer will not be affected
	
	for(i = 0; i<len; i++)
	{
		GPS_ClockInByte(ptr);
		
		if(*ptr == IDLE_LINE)
		{
			return NO_DATA;
		}
		else if(*ptr == LINE_FEED)
		{
			return GOOD_WORD;			
		}
		else if(*ptr == 0)
		{
			return ERROR;
		}	
		
		ptr++;//Next byte
	}
	return ERROR; //Avoids mem faults if something goes wrong
}
//---------------------------------------------------
//int GPS_SendCommand
//---------------------------------------------------
//---------------------------------------------------
void GPS_TestCommand(void)
{
	unsigned int len = 20;
	char test[50];
	unsigned int a, b;
	//Sync 1
	test[0] = (char)0xB5;
	//Sync 2
	test[1] = (char)0x62;
	//Class
	test[2] = (char)0x01;
	//ID
	test[3] = (char)0x09;
	//Len, little endian
	test[4] = (char)(len & 0xFF);
	test[5] = (char)((len & 0xFF00)>>8);
	//Payload
	test[0] = (char)0xB5;
	
	
		
}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
/***EOF***/
