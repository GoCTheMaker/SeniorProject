#ifndef __COMMPERIPH_
#define __COMMPERIPH_
//--------------------------------
#include <stdint.h>
#include "stm32l0xx.h"                  // Device header
#include "stm32l0xx_ll_gpio.h"

// ---  SPI BASE FUNCTIONS --- \\
// GPS Communication Functions
	// Set the CS Pin Active (1) or Inactive (0)
void setCS_gps(int state);
	// Transmit an 8 bit char to the GPS Unit and stop
void transmit8SPI_gps(uint8_t data);
	// Transmit an arbitrary length data to the GPS unit
	// *dataAdr is a pointer to the data you wish to send
	// length is the length of the data in chars
void transmitSPI_gps(char *dataAdr, uint16_t length);
	// Receive 8 bits of 
uint8_t receiveSPI_gps(void);


// ---  UART BASE FUNCTIONS --- \\

// Transmit 8 bits on UART
void transmit8UART_PC(uint8_t data);
void transmit8UART_XB(uint8_t data);

// Transmit arbitrary amount of data over UART
void transmitUART_PC(char *dataAdr, uint16_t length);
void transmitUART_XB(char *dataAdr, uint16_t length);

//--------------------------------
#endif
