/****************************
*Header file for gps function calls 
* and macros
****************************/

#ifndef GPS_H_
#define GPS_H_

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

//Data Word Struct
//----------------------------------------------
/*struct gpsword
{
	char id[2];
	char format[3];
	char data[50];	
};*/

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



#endif //GPS_H_