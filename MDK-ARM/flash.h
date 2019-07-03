/****************************
*Header file for flash function calls 
* and macros
****************************/

#ifndef FLASH_H_
#define FLASH_H_

//Macros
//----------------------------------------------
#define UNLOCK_KEY_1 0x89ABCDEF
#define UNLOCK_KEY_2 0x02030405

//Prototypes
//----------------------------------------------
int FLASH_Unlock();
int FLASH_Lock();
int FLASH_WriteByte(unsigned char byte, unsigned long addr);
int FLASH_WriteData(unsigned char * str, int len, unsigned long addr);
int FLASH_ReadData(unsigned char * str, int len, unsigned long addr);


#endif //FLASH_H_