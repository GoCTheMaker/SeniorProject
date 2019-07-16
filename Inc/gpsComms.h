#ifndef __GPSCOMMS_
#define __GPSCOMMS_
//------------------------
#include <stdint.h>
#include <string.h>
#include "stm32l0xx.h"                  // Device header
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_spi.h"


// --------------
#define SYNC_CHAR_1 0xB5
#define SYNC_CHAR_2 0x62
// --------------

#define CLASS_NAV	0x01
#define CLASS_RXM	0x02
#define CLASS_INF	0x04
#define CLASS_ACK	0x05
#define CLASS_CFG 0x06
#define CLASS_UPD 0x09
#define CLASS_MON 0x0A
#define CLASS_AUD 0x0B
#define CLASS_TIM 0x0D
#define CLASS_ESF 0x10
#define CLASS_MGA 0x13
#define CLASS_LOG 0x21
#define CLASS_SEC 0x27
#define CLASS_HNR 0x28

// -------------------

#define BFR_CLR_TIMEOUT 30

// -------------------

struct UBXmessage{
	uint8_t msgClass;
	uint8_t msgID;
	uint16_t length;
	char* payloadLoc;
	uint8_t checksumA;
	uint8_t checksumB;
	uint8_t status;
};

struct UBXTXInfo{
	uint8_t status;
	uint8_t checksumA;
	uint8_t checksumB;
};

// -------------------
	// Set the CS Pin Active (0) or Inactive (1)
void enableGPSCS(void);
void disableGPSCS(void);

/*
*		payloadLoc : pointer to element 0 of array of length @length
*		checksumLoc : pointer to element 0 of array of length 2
*/
struct UBXTXInfo sendMSG(struct UBXmessage msg);
int requestMSG(struct UBXmessage msg);

void clearGPSBuffer(void);

//---------------#define CLASS_NAV---------
#endif
