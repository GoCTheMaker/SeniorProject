/****************************
*Header file for gps function calls 
* and macros
****************************/

#ifndef GPS_H_
#define GPS_H_

#include <stdint.h>
#include "stm32l0xx_ll_rtc.h"

//Macros
//----------------------------------------------
#define GOOD_WORD 0
#define NO_DATA -2
#define ERROR	-1

#define IDLE_LINE (char)0xFF
#define LINE_FEED (char)10
	
//#define TESTCOMMAND "$PUBX,41,4,0007,0003,19200,0*25"
//#define TESTCOMMAND "$EIGNQ,RMC*24"
#define TESTCOMMAND "PUBX,40,RMC,1,1,1,1,1,0*46"


#define SYNC_CHAR_1 0xB5
#define SYNC_CHAR_2 0x62
#define BFR_CLR_TIMEOUT 40

//Data Word Struct
//----------------------------------------------
/*struct gpsword
{
	char id[2];
	char format[3];
	char data[50];	
};*/


struct UBXmessage{
	uint8_t msgClass;
	uint8_t msgID;
	uint16_t length;
	char payloadLoc[50];
	uint8_t checksumA;
	uint8_t checksumB;
	uint8_t status;
};

struct UBXTXInfo{
	uint8_t status;
	uint8_t checksumA;
	uint8_t checksumB;
};

struct NMEA_RMC{
	LL_RTC_TimeTypeDef time;	// Time struct, see above
	char 		status;			// Data validity status. 
											// V = No pos/Limits Exceeded/DeadReckoning&limits exceeded 
											// A = Dead Reckoning fix/RKT Float/RTK Fixed/ 2D/3D Fix/ Combined GNSS/dead reckoning fix
	float 	lat;				// Latitude
	char 		NS;					// North/South Indicator
	float 	longt;			// Longitude
	char 		EW;					// East/West Indicator
	float 	speed;			// speed in knots
	float 	cog;				// Course Over Ground (heading) (in degrees)
	float		mv;					// "Magnetic Variation Value"
	char		mvEW;				// "Magnetic Variation E/R Indicator"
	char		posMode;		// Mode Indicator
	char 		navStatus;	// Navigation status indicator. Should always be 'V'. I think.
	uint8_t	checksum;		// Checksum. Hex. 
};

//Prototypes
//----------------------------------------------
void GPS_GPSEnable(void);
void GPS_GPSDisable(void);
void GPS_GPSCSLow(void);
void GPS_GPSCSHigh(void);
void GPS_SendByte(char byte);
void GPS_SendData(char * data, int size);
void GPS_ClockInByte(char * byte);
int GPS_GetDataWord(char * data, int len);
//int GPS_SendDataWord(struct gpsword

struct UBXTXInfo GPS_UBX_sendMSG(struct UBXmessage msg, char payload[50]);
int GPS_UBX_requestMSG(struct UBXmessage msg);

void clearGPSBuffer(void);

int GPS_UBX_enablePUBX_Position(void);

struct NMEA_RMC GPS_getNMEA_RMC(void);


#endif //GPS_H_