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
int XbeeRxBufferActive;	//Flag set to signify the existence of new data in the Xbee buffer
int XbeeDongleConnected;
//---------------------------------------------------
int XB_XbeeSubroutine()
{
	
	//Enable XBee
	XB_EnableXbee();
	//Start Timeout timer
	
	
	//Check for 
	//While not timed out 
	if(!XB_CheckForDongle())
	{
		XbeeDongleConnected = YES;
		//Set connected timeout
		while(XbeeDongleConnected)
		{
			XB_ExecuteCommand(XB_ParseCommand());			
		}
	}
		
}
//---------------------------------------------------
void XB_EnableXbee()
{
	XBEE_EN_GPIO_Port->ODR &= ~XBEE_EN_Pin;
}
//---------------------------------------------------
void XB_DisableXbee()
{
	XBEE_EN_GPIO_Port->ODR |= XBEE_EN_Pin;
}
//---------------------------------------------------
int XB_SendByte(char byte)
{
	LL_USART_TransmitData8(USART4, byte);
	while(!((USART4->ISR & LL_USART_ISR_TC) == LL_USART_ISR_TC));
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
	
	XbeeRxBufferActive = 0; //Clear buffer flag for safety
	
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
int XB_CheckForDongle()
{
	int i;
	if(XbeeRxBufferActive)
	{
		//Check for correct character
		for(i = XbeeRxReadFrom;i != XbeeRxWriteIndex; i = (i + 1) % BUFFER_SIZE)
		{
			//If proper handshake char from dongle read
			if(XbeeRxBuff[i] == EOC)
			{
				XbeeRxReadFrom = (i + 1) % BUFFER_SIZE;
				//Respond with handshake				
				XB_SendByte(EOC);
				return 0;				
			}
		}
		return -2; //No correct data on line 
		//for(chars in buffer)
		//if char is EOC 
		//send resp
		//return 
		//end of for
		//return not correct response 
	}
	else
	{
		return -1;//No activity on rx line
	}
}
//---------------------------------------------------
int XB_ParseCommand()
{
	int i, currWriteIndex = XbeeRxWriteIndex; //Need temp index so global won't be 
																					//iterated during function call. makes errors
	char temp[50];
	
	//Check for end of command first in buffer
	//Ideally, this sort of buffering should allow for multiple 
	//	commands in the buffer at once 
	for(i=XbeeRxReadFrom; i!=currWriteIndex; i = (i+1)%BUFFER_SIZE)
	{
		if(XbeeRxBuff[i] == EOC)
		{
			XbeeRxReadTo = i + 1;
			break;
		}
	}	
	if(i == currWriteIndex) return NO; //No full command found
	
	memset(temp, 0, sizeof(temp));//clears string 
	i = 0;
	//If command found, read command to local string
	for(; XbeeRxReadFrom!=XbeeRxReadTo; XbeeRxReadFrom=(XbeeRxReadFrom + 1)%BUFFER_SIZE)
	{
		temp[i++] = XbeeRxBuff[XbeeRxReadFrom];
	}
	//Check command against known strings 	
	// Return case for switch case statement
	if(!strcmp(temp, GETFIXES)) 
		return GETFIXES_N;
	//else if(!strcmp(temp, )) return ;
	
	else return NO;	//Default for unknown command
}
//---------------------------------------------------
int XB_ExecuteCommand(int command)
{
	switch(command)
	{
		//Commands run here
		//-------------------------------------------------------------
		case GETFIXES_N:
			return XB_TransmitFixes();
			break;
		//-------------------------------------------------------------
		default:	//Unprogrammed response should be 0 but this is safer
			return -1;//No command executed
			break;
	
	}
}
//---------------------------------------------------
int XB_TransmitFixes()
{
	//Currently a test function
	char test[] = "this,is\nbullshit,data";
	XB_SendData(test, strlen(test));
	XB_SendByte(EOC);
	XbeeDongleConnected = NO;
	return 0;//Holder	
}
//---------------------------------------------------
/*--EOF--*/
