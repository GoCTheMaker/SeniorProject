/****************************************************
*gps.c
*This file contains functions to facilitate communications
* with GPS device for data storage
****************************************************/

//Includes
#include "globals.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>
#include "gps.h"
#include "timers.h"
#include "comport.h"
#include "flash.h"

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
/* Unused functions
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

struct UBXTXInfo GPS_UBX_sendMSG(struct UBXmessage msg, char payload[50] ){
	int i = 0;
	char msgBuffer[50];
	char checksumBuffer[ msg.length + 4];
	struct UBXTXInfo returnInfo;
	
	char wwww;
		
	// Second, build the buffer for the checksum
	// The checksum is built off of the Message Class,
	// Message ID, Length, and Payload, in that order. 
	// Therefore, we will build a string of this data. 
	
	checksumBuffer[0] = msg.msgClass;
	checksumBuffer[1] = msg.msgID;
	checksumBuffer[3] = (msg.length & 0x00FF);				// Concat lower byte of length, little endian
	checksumBuffer[2] = (msg.length & 0xFF00) >> 8;		// Concat upper byte of length
	for( i=0; i<msg.length; i++){
		checksumBuffer[i+4] = payload[i];
	}
	
	// Calculate the RFC 1145 checksum:
	// see pg 144 of the Receiver Description
	for(i = 0; i < msg.length+4; i++){
		returnInfo.checksumA = returnInfo.checksumA + checksumBuffer[i];
		returnInfo.checksumB = returnInfo.checksumB + returnInfo.checksumA;
	}
	
	returnInfo.checksumA = 0x05;
	returnInfo.checksumB = 0x3F;
	
	LL_SPI_TransmitData8(SPI1, 0x00);
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, 0x00);
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, 0x00);
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, 0x00);
	while( (SPI1->SR & 0x80) == 0x80);	
	
	
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, SYNC_CHAR_1);
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, SYNC_CHAR_2);
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, msg.msgClass);
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, msg.msgID);
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, (msg.length & 0x00FF));		// little endian
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, (msg.length & 0xFF00) >> 8) ;
	while( (SPI1->SR & 0x80) == 0x80);
	for( i=0; i<msg.length; i++){
		LL_SPI_TransmitData8( SPI1, payload[i]);
	while( (SPI1->SR & 0x80) == 0x80);
	}
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, returnInfo.checksumA);
	while( (SPI1->SR & 0x80) == 0x80);
	LL_SPI_TransmitData8(SPI1, returnInfo.checksumB);
	while( (SPI1->SR & 0x80) == 0x80);
	
	returnInfo.status = 1;
	return returnInfo;

}
//---------------------------------------------------
int GPS_UBX_requestMSG(struct UBXmessage msg){
	
	uint8_t header1 = 0xFF;
	uint8_t header2, rxClass, rxID; 
	uint16_t rxLength;
	uint8_t rxChecksumA = 0;
	uint8_t rxChecksumB = 0;
	int tempLength = msg.length;
	//memset(rxPayload, 0, sizeof rxPayload);
	
	
	// Declare the info struct:
	struct UBXTXInfo txInfo; // needed to compare checksums but not currently used
	
	// Set some flags:
	int i = 0;
		
	msg.length = 0;
	
	GPS_GPSCSLow();
	// Fix this later if needed
	//txInfo = GPS_UBX_sendMSG( msg, payload );
	GPS_GPSCSHigh();
	msg.length = tempLength;
	
	
 	GPS_GPSCSLow();
	TIM2_delay(500);
	
	while( header1 == 0xFF ){
		LL_SPI_TransmitData8(SPI1, 0x00);
		header1 = LL_SPI_ReceiveData8(SPI1);
		while( (SPI1->SR & 0x80) == 0x80);
	};
	
	LL_SPI_TransmitData8(SPI1, 0x00); 
	header2 = LL_SPI_ReceiveData8(SPI1);
	while( (SPI1->SR & 0x80) == 0x80);
	
 	LL_SPI_TransmitData8(SPI1, 0x00);
	rxClass = LL_SPI_ReceiveData8(SPI1);
	while( (SPI1->SR & 0x80) == 0x80);
	
	LL_SPI_TransmitData8(SPI1, 0x00);
	rxID = LL_SPI_ReceiveData8(SPI1);
	while( (SPI1->SR & 0x80) == 0x80);
	
	LL_SPI_TransmitData8(SPI1, 0x00);
	rxLength = LL_SPI_ReceiveData8(SPI1);
	while( (SPI1->SR & 0x80) == 0x80);
	
	LL_SPI_TransmitData8(SPI1, 0x00);
	rxLength |= ( LL_SPI_ReceiveData8(SPI1) << 8 ) ;
	while( (SPI1->SR & 0x80) == 0x80);
	
	for(i=0; i < msg.length; i++){
		LL_SPI_TransmitData8(SPI1, 0x00);
		 msg.payloadLoc[i] = LL_SPI_ReceiveData8(SPI1);
	}
	
	LL_SPI_TransmitData8(SPI1, 0x00);
	rxChecksumA = LL_SPI_ReceiveData8(SPI1);
	while( (SPI1->SR & 0x80) == 0x80);
	
	LL_SPI_TransmitData8(SPI1, 0x00);
	rxChecksumB = LL_SPI_ReceiveData8(SPI1);
	while( (SPI1->SR & 0x80) == 0x80);
	
	GPS_GPSCSHigh();
	
	// TODO: implement proper checksumming & error checking
 	if( header1 == 0xB5 && header2 == 0x62
	 && rxClass == msg.msgClass && rxID == msg.msgID){
		 msg.status = 1;
	 }else{
		 msg.status = 0;
	 }
}
//---------------------------------------------------
void clearGPSBuffer(void){
	uint8_t rxData = 0x00;
	uint8_t FFcount = 0;
	uint8_t count = 0;
	
	while( ( FFcount < 5 ) && ( count < BFR_CLR_TIMEOUT ) ){
		LL_SPI_TransmitData8(SPI1, 0x00);
		rxData = LL_SPI_ReceiveData8(SPI1);
		if ( rxData == 0xFF ){
			FFcount++;
		}else{
			FFcount = 0;
		}
		count++;
	}
}
*/
//---------------------------------------------------
struct GPS_POS GPS_getNMEA(void){
	// End goal here is to only parse RMC data. However, it currently just prints whatever
	// Sentance it reads. 

	char rxData = 0;
	int state = 1;
	int RMC_timeFlag = 0;
	char rxBuffer[GPS_RX_BUFF_SIZE];
	memset( rxBuffer, 0, GPS_RX_BUFF_SIZE );
	int index = 0;
	int accI = 0;
	int strI = 0;
	char* charTok;
	char accTemp[ACC_TEMP_SIZE];
	char checksum[2];
	uint8_t msgComplete = 0;
	static struct GPS_POS position;
		memset(&position, 0, sizeof(position));
	position.acc = 99999;
	int i, count = 0, nextIndex = 0, lastIndex = 0;


		
		//  NMEA MESSAGE PARSER
		// 	STATE MACHINE
		//		1 - idle. waiting for '$' start character 
		//		2 - reserved
		//		3 - start char detected. keep reading until the '*' checksum deliminator and store to array.
		//		4 - UBX Parse
		//		5 - we got the full message. let's parse it based on the ',' field deliminator.

	while( msgComplete == 0 ){
		switch( state ){
			case 1:
				// ---- IDLE STATE ---
				// Continue to clock in characters until we read the start:
				LL_SPI_TransmitData8( SPI1, 0x00 );
				while( (SPI1->SR & 0x80) == 0x80);
				rxData = LL_SPI_ReceiveData8( SPI1 );
				if( rxData == '$' ){
					// NMEA start byte. Go to NMEA parser. 
					state = 3;
					index = 0;
				}
				if( rxData == (char)0xB5 ){
					// UBX start byte. Go to NMEA parser. 
					// Future enhancement. 
					index = 0;
					state = 4;
				}
			break;
		// State 2 is reserved. 
			case 3:
				
				// ---- NMEA READ STATE ---
				LL_SPI_TransmitData8( SPI1, 0x00 );
				while( (SPI1->SR & 0x80) == 0x80);
				rxData = LL_SPI_ReceiveData8( SPI1 );
				rxBuffer[index] = rxData;
				
				// Check if consecutive data is valid: 
				if( rxData == (char)0xFF ){
					// if not, go back to state 1:
					state = 1;
				}else if( rxData != (char)0xFF ); //LL_USART_TransmitData8( USART1, rxData );
				// IF it is:
				if( rxBuffer[index] == '*' ){
					LL_SPI_TransmitData8( SPI1, 0x00 );
					while( (SPI1->SR & 0x80) == 0x80);
					checksum[0] = LL_SPI_ReceiveData8( SPI1 );
					LL_SPI_TransmitData8( SPI1, 0x00 );
					while( (SPI1->SR & 0x80) == 0x80);
					checksum[1] = LL_SPI_ReceiveData8( SPI1 );
					rxBuffer[ index + 1 ] = 0x0D;
					rxBuffer[ index + 2 ] = 0x0A;
					state = 5;
				}
				if( index >= GPS_RX_BUFF_SIZE){
					// Overflow, start over I guess
					state = 1;
				}
				index++;
				break;
			case 4:
				// ---- UBX READ STATE ---
				LL_SPI_TransmitData8( SPI1, 0x00 );
				while( (SPI1->SR & 0x80) == 0x80);
				rxData = LL_SPI_ReceiveData8( SPI1 );
				rxBuffer[index] = rxData;
				
				// Check if consecutive data is valid: 
				if( rxData == (char)0xFF ){
					// if not, go back to state 1:
					state = 1;
				}else if( rxData != (char)0xFF ){
				// IF it is:
					LL_USART_TransmitData8( USART1, rxData );
				}
				if( index >= GPS_RX_BUFF_SIZE){
					// Overflow, start over I guess
					state = 1;
				}
				index++;
				break;
			case 5:
				// ---- NMEA PARSE STATE ---
				// We successfully read a full sentence! Theoretically.
			
				// For debugging, send the string to UART before it's messed with
				CC_SendData( rxBuffer, sizeof(rxBuffer) );
 				
			// Parse PUBX,00 to read time, fix & accuracy data:
				if(rxBuffer[0] == 'P' && rxBuffer[1] == 'U' && rxBuffer[2] == 'B'){
					
					// strtok! 
 					charTok = strtok( rxBuffer, ",");
					for( strI=0; strI<PUBX_POS_NUM_FIELDS-1; strI++){
						charTok = strtok( NULL, ",");
						
						// The required fields are 2, 3, 4, 5, 6, 9.
						// Subtract 1 to adjust and strtok!
						switch( strI ){
							case 2:
								// Latitude 
								strncpy(position.lat, charTok, 10);
							break;
							case 3:
								// N/S Indicator 
								position.NS = charTok[0];
							break;
							case 4:
								// Longitude
								strncpy(position.longt, charTok, 10);
							break;
							case 5:
								// E/W Indicator
								position.EW = charTok[0];
							break;
							case 8:
								// Horizontal Accuracy
								position.acc = atoi(charTok);
								strI=PUBX_POS_NUM_FIELDS;
							break;
						};
					}					
				}
				// Parse RMC to get date:
				if(rxBuffer[0] == 'G' && rxBuffer[1] == 'N' && rxBuffer[2] == 'R' && rxBuffer[3] == 'M'){
					for(i=0; i<strlen(rxBuffer);i++)
					{
						if(rxBuffer[i] == ',')
						{
							count++;
							lastIndex = nextIndex;
							nextIndex = i;
						}
						if( (count == 2) && (RMC_timeFlag == 0) ){
							strncpy( position.time, &rxBuffer[lastIndex + 1], 4);
							RMC_timeFlag = 1;
						}
						if(count == 10)
						{
							strncpy( position.date, &rxBuffer[lastIndex + 1], 6);
							break;
						}

					}//for rxbuffer
				}//if rmc			
			msgComplete = 1;
			break;
		}
	}
	
	return position;
} // GPS_getNMEA_RMC 

//---------------------------------------------------
int GPS_UBX_enablePUBX_Position(void){
	
	char payload[16] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF1, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x05, 0x3F};
	int i = 0;
	
	for(i=0;i<16; i++){
		while( (SPI1->SR & 0x80) == 0x80);				// Wait until bus is calm
		LL_SPI_TransmitData8( SPI1, payload[i] );	// Send character
	}
	return 0;
}
//---------------------------------------------------
int GPS_subroutine(void){
	static struct GPS_POS position;
		memset(&position, 0, sizeof(position));
		position.acc = 99999;
	LL_RTC_TimeTypeDef time;
	LL_RTC_DateTypeDef date;
	
	
	GPS_GPSEnable();
	GPS_GPSCSHigh();
	TIM2_delay(150);
	GPS_GPSCSLow();
	TIM2_delay(500);
	GPS_UBX_enablePUBX_Position();
	
	TIM2_initDelay_inline( GPS_TIMEOUT );
	while( !(TIM2->SR & TIM_SR_UIF) &&  (position.acc > GPS_ACC_REQ) ){
		position = GPS_getNMEA();
	}
	
	if(position.acc > GPS_ACC_REQ){
		memset(position.lat, '0', sizeof(position.lat));
		memset(position.longt, '0', sizeof(position.longt));
	}
	
	time.Hours   = (position.time[0]-'0')*10+(position.time[1]-'0');
	time.Minutes = (position.time[2]-'0')*10+(position.time[3]-'0');
	date.Day     = (position.date[0]-'0')*10+(position.date[1]-'0');
	date.Month   = (position.date[2]-'0')*10+(position.date[3]-'0');
	date.Year   = (position.date[4]-'0')*10+(position.date[5]-'0');
	// thank you to Michael Keith and Tom Craver for this beaut:
	date.WeekDay = (date.Day += date.Month < 3 ? date.Year--:date.Year-2,23*date.Month/9+date.Day+4+date.Year/4-date.Year/100+date.Year/400)%7;
	
	if( (date.Year != ',') && (date.Year > 0) && (date.Year < 100) ){
		RTC_setTimeDate(time, date);
	}else{
		RTC_getTimeDate( &time, &date );
		position.time[0] = (time.Hours / 10)+'0';  position.time[1] = (time.Hours % 10)+'0';
		position.time[2] = (time.Hours / 10)+'0';  position.time[3] = (time.Hours % 10)+'0';
		position.date[0] = (date.Day   / 10)+'0';  position.date[1] = (date.Day   % 10)+'0';
		position.date[2] = (date.Month / 10)+'0';  position.date[3] = (date.Month % 10)+'0';
		position.date[4] = (date.Year  / 10)+'0';  position.date[5] = (date.Year  % 10)+'0';		
	}
	
#ifdef _PRODUCTION__	
	FLASH_saveFix(position);
#endif
	
	
	return 0;
}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
/***EOF***/
