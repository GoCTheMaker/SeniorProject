#include "commPeriph.h"
#include "stm32l0xx.h"                  // Device header
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_spi.h"

// GPS Communication Functions
inline void setCS_gps(int state){
	if( !state ) {
		GPIOA->ODR |= LL_GPIO_PIN_8;
	};
	if( state ) {
		GPIOA->ODR &= ~LL_GPIO_PIN_8;
	};
}
void transmit8SPI_gps(uint8_t data){
		setCS_gps(1);
		LL_SPI_TransmitData8(SPI1, data);
		setCS_gps(0);
}
void transmitSPI_gps(char *dataAdr, uint16_t length);
uint8_t receiveSPI_gps(void);

