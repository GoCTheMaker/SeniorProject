/****************************
*Header file for xbee function calls 
* and macros
****************************/

#ifndef XBEE_H_
#define XBEE_H_

//Macros
//----------------------------------------------
//Xbee Config Commands
#define DL_LEN	6
#define PROGRAM_MODE_CMD	"+++"
#define CR	'\r'
#define PROGRAM_MODE_RESP	"OK\r"

#define SET_DL	"ATDL"
#define SET_DH	"ATDH"

#define GET_SH	"ATSH"
#define GET_SL	"ATSL"


#define SET_CT	"ATCT"
#define SET_GT	"ATGT"
#define DEF_CT	"5"
#define	DEF_GT	"1F4"

#define SET_NVM	"ATWR"
#define CONFIRM_SET	"ATAC"

//Xbee GUI Commands
#define GETFIXES "DTA?"

#define GETFIXES_N 1

//File Defines
#define DELIM ','
#define END_TRANS '\t'

//Prototypes
//----------------------------------------------
int XB_XbeeSubroutine();
void XB_EnableXbee();
void XB_DisableXbee();
int XB_SendByte(char byte);
int XB_SendData(char * string, int size);
int XB_GetResponse(char * resp, int *len);
int XB_ConfigureDevice(char * sn, int len);
int XB_GetConfig();
int XB_GetSN();
int XB_CheckForDongle();
int XB_ParseCommand();
int XB_ExecuteCommand(int command);
int XB_TransmitFixes();

#endif //XBEE_H_