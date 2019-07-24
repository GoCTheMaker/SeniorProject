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
#define DATA_LENGTH 32
#define DATA_START_ADDR 	0x08003700
#define END_OF_MEMORY			0x0800FFFF

#define NEXTSTARTADDR_ADDR  0x08080014


// We can fit 4 fixes per page
// @ 1 word per fix






// End of memory is 0x0800 3FFF

//Prototypes
//----------------------------------------------
int FLASH_Unlock();
int FLASH_Lock();
int EEPROM_WriteByte(unsigned char byte, unsigned int addr);
int EEPROM_WriteData(unsigned char * str, int len, unsigned int addr);

int FLASH_unlockPrgm(void);
int FLASH_ReadData(unsigned char * str, int len, unsigned int addr);
int FLASH_erasePage(unsigned int addr);


int FLASH_writeWord(uint32_t data, unsigned int addr);

int FLASH_initDataStorage(void);
int FLASH_saveFix(struct GPS_POS position);

int FLASH_readFixes(unsigned char *rtn);

#endif //FLASH_H_