/****************************
*Header file for comport function calls 
* and macros
****************************/

#ifndef COMPORT_H_
#define COMPORT_H_

//Macros
//----------------------------------------------
//Commands sent by com link
#define HANDSHAKE "ACK?"
#define	TIMESET		"TST?"
#define PROGRAM		"PGM?"
#define GETPGM		"GPG?"
#define TIMESTAMP "TMP?"
#define CONFIGXBEE	"CFG?"
#define GETXBCFG	"GFG?"
#define GETSN			"GSN?"
#define ERSFLASH	"ERS?"

#define	TIMESET_N	1
#define	PROGRAM_N	2
#define TIMESTAMP_N 3
#define GETPGM_N	4
#define CONFIGXBEE_N 5
#define GETXBCFG_N	6
#define GETSN_N	7
#define ERSFLASH_N 8

//Program data
#define MAX_PGM_LEN 50

//Prototypes
//----------------------------------------------
int CC_ComPortPresent();
int CC_SendByte(char byte);
int CC_SendData(char * string, int size);
int CC_CheckForHandshake();
int CC_ParseCommand();
int CC_ExecuteCommand(int command);
int CC_SetRTC();
int CC_SendTimestamp();
int CC_SetProgram();
int CC_ReadProgram();
int CC_ConfigureXbee();



#endif //COMPORT_H_