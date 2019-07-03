/****************************************************
*xbee.c
*This file contains functions to facilitate communications
* with a researchers computer via the xbee device onboard
****************************************************/

//Includes
#include "comport.h"
#include "globals.h"
#include "main.h"
#include "flash.h"
#include <string.h>
#include "xbee.h"

//Software init
char XbeeRxBuff[BUFFER_SIZE];	//Ring bufer for raw input
int XbeeRxWriteIndex;	//Current write index of buffer
int XbeeRxReadTo;
int XbeeRxReadFrom;

//---------------------------------------------------
int XB_SendByte(char byte)
{
	LL_LPUART_TransmitData8(LPUART1, byte);
	while(!((LPUART1->ISR & LL_LPUART_ISR_TC) == LL_LPUART_ISR_TC));
	return 0;
}
//---------------------------------------------------	
int XB_SendData(char * string, int size)
{
	int i;
	for(i=0; i<size; i++)
	{
		XB_SendByte(* string);
		string++;
	}
	return 0;
}
//---------------------------------------------------
int XB_GetResponse(char * resp, int *len)
{
	int i;
	//Check for data in buffer (Wait for resp from device)
	while(XbeeRxBuff[XbeeRxWriteIndex-1] != CR) //Waiting for end of second transmission
	{
		
	}		//Really dirty way of doing this, @TODO fix this
	XbeeRxReadTo = XbeeRxWriteIndex - 1;
	
	//Get data from buffer
	for(i = 0; XbeeRxReadFrom != XbeeRxReadTo; XbeeRxReadFrom = (XbeeRxReadFrom+1)%BUFFER_SIZE)
	{
		resp[i++] = XbeeRxBuff[XbeeRxReadFrom];
	}	
	
	*len = strlen(resp);
	
	//Skip /r to keep String handling simple 
	XbeeRxWriteIndex = (XbeeRxWriteIndex + 1) % BUFFER_SIZE;
	XbeeRxReadFrom = (XbeeRxReadFrom + 2) % BUFFER_SIZE;
	return 0;
}
//---------------------------------------------------
int XB_ConfigureDevice(char * sn, int len)
{
	char str[10];
	char dl[10], dh[10];
	int i, temp;
	
	memset(dl, 0, sizeof(dl));
	memset(dh, 0, sizeof(dh));
	
	memcpy(dh, sn, DL_LEN);
	memcpy(dl, sn+DL_LEN, len - DL_LEN);
	
	//CC_SendData(dl, strlen(dl));
	//CC_SendData(dh, strlen(dh));
	
		
	
	//Clear tempstring
	memset(str, 0, sizeof(str));
	
	//Enter command mode
	XB_SendData(PROGRAM_MODE_CMD, 3);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	
	//Setting the parameters
	//Just in case, update timing params
	XB_SendData(SET_CT, strlen(SET_CT));
	XB_SendData(DEF_CT, strlen(DEF_CT));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);
	
	XB_SendData(SET_GT, strlen(SET_GT));
	XB_SendData(DEF_GT, strlen(DEF_GT));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);
	
	//DL 
	XB_SendData(SET_DL, strlen(SET_DL));
	XB_SendData(dl, strlen(dl));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	//DH
	XB_SendData(SET_DH, strlen(SET_DL));
	XB_SendData(dh, strlen(dh));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	//Confirm with AC
	XB_SendData(CONFIRM_SET, strlen(SET_DL));	
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	//Write to NVM
	XB_SendData(SET_NVM, strlen(SET_NVM));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	//Get Config and ECHO
	XB_SendData(SET_DH, strlen(SET_DH));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	XB_SendData(SET_DL, strlen(SET_DL));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	return 0;
}
//---------------------------------------------------
int XB_GetConfig()
{
	char str[50];
	int temp;
	memset(str, 0, sizeof(str));
	
	//Enter command mode
	XB_SendData(PROGRAM_MODE_CMD, 3);	
	XB_GetResponse(str, &temp);	
	
	CC_SendData(str, temp);	
	
	//Get Config and ECHO
	XB_SendData(SET_DH, strlen(SET_DH));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	XB_SendData(SET_DL, strlen(SET_DL));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	return 0;
}
//---------------------------------------------------
int XB_GetSN()
{
	char str[50];
	int temp;
	
	memset(str, 0, sizeof(str));
	
	//Enter command mode
	XB_SendData(PROGRAM_MODE_CMD, 3);	
	XB_GetResponse(str, &temp);	
	
	CC_SendData(str, temp);	
	
	//Get Config and ECHO
	XB_SendData(GET_SH, strlen(GET_SH));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	XB_SendData(GET_SL, strlen(GET_SL));
	XB_SendByte(CR);
	
	XB_GetResponse(str, &temp);	
	CC_SendData(str, temp);	
	
	return 0;	
}
//---------------------------------------------------