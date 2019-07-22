/****************************
*Header file for flash function calls 
* and macros
****************************/

#ifndef FLASH_H_
#define FLASH_H_

#include "gps.h"

//Macros
//----------------------------------------------
#define UNLOCK_KEY_1 0x89ABCDEF
#define UNLOCK_KEY_2 0x02030405
#define PRGKEY1 0x8C9DAEBF
#define PRGKET2	0x13141516

#define PAGE_SZE 128 // in bytes
#define DATA_LENGTH 




// End of memory is 0x0800 3FFF

//Prototypes
//----------------------------------------------
int FLASH_Unlock();
int FLASH_Lock();
int FLASH_WriteByte(unsigned char byte, unsigned int addr);
int FLASH_WriteData(unsigned char * str, int len, unsigned int addr);

int FLASH_unlockPrgm(void);
int FLASH_ReadData(unsigned char * str, int len, unsigned int addr);
int FLASH_erasePage(unsigned int addr);


int FLASH_writeWord(unsigned int data, unsigned int addr);

int FLASH_saveFix(struct GPS_POS position);

#endif //FLASH_H_