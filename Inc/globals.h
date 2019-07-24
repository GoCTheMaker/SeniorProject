/****************************
*File to contain all global vars for the program
*Globals intended to handle passing between ISR's
****************************/

#ifndef GLOBALS_H_
#define GLOBALS_H_

//Macros
//----------------------------------------------
//General
#define YES 1
#define NO	0
#define TRUE 	1
#define FALSE	0

//Serial buffer size
#define BUFFER_SIZE 64
#define EOC	'?'	//End of command 



//Globals
//----------------------------------------------
//Com port buffer handling
extern char ComRxBuff[BUFFER_SIZE];	//Ring bufer for raw input
extern int ComRxWriteIndex;	//Current write index of buffer
extern int ComRxReadTo;
extern int ComRxReadFrom;

//Xbee port buffer handling
extern char XbeeRxBuff[BUFFER_SIZE];	//Ring bufer for raw input
extern int XbeeRxWriteIndex;	//Current write index of buffer
extern int XbeeRxReadTo;
extern int XbeeRxReadFrom;
extern int XbeeRxBufferActive;
extern int XbeeDongleConnected;

extern int checkSchedule;


#endif //GLOBALS_H_