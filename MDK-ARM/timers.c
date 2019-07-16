#include "main.h"
#include "timers.h"
#include "stm32l0xx_ll_tim.h"

int RTC_Flag = 0;

void initTIM2(void){
	
	// Enable TIM2 in the RCC:
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	// Prescaler is the core clock (262144 Hz) by 1000:
	TIM2->PSC = 262144 / 1000;
}

void tim2Delay(uint16_t delay){
	// Delay is in ms
	
	// Clear the update event flag 
	TIM2->SR = 0;
	
	// Move the delay into the ARR:
	TIM2->ARR = delay;
	
	// Start the timer:
	TIM2->CR1 |= TIM_CR1_CEN;
	
	// Wait until it's done:
	while (!(TIM2->SR & TIM_SR_UIF));
	
	return;
}

void RTC_initAlarm(void){
	LL_RTC_DisableWriteProtection(RTC);
	
	LL_RTC_ALMA_Disable(RTC);
	
	while((RTC->ISR & RTC_ISR_ALRAWF) != RTC_ISR_ALRAWF);

	RTC->CR |= RTC_CR_ALRAIE;
	
	LL_RTC_ALMA_SetSecond(RTC, 0x00);
	LL_RTC_ALMA_SetMask( RTC, RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4);
	
	LL_RTC_ALMA_Enable(RTC);
	
	LL_RTC_EnableWriteProtection(RTC);
		
}

void RTC_setTimeDate(LL_RTC_TimeTypeDef time, LL_RTC_DateTypeDef date){
	// CC_SetRTC() should be modified to return the appropriate datas
	// in the above typedefs
	
	LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &time);
	LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &date);
	
}

void RTC_getTimeDate(LL_RTC_TimeTypeDef* time, LL_RTC_DateTypeDef* date){
	// CC_SetRTC() should be modified to return the appropriate datas
	// in the above typedefs
	
	(*time).Hours = LL_RTC_TIME_GetHour(RTC);
	(*time).Minutes = LL_RTC_TIME_GetMinute(RTC);
	(*time).Seconds = LL_RTC_TIME_GetSecond(RTC);
	(*time).TimeFormat = LL_RTC_TIME_GetFormat(RTC);
	
	(*date).Day = LL_RTC_DATE_GetDay(RTC);
	(*date).Month = LL_RTC_DATE_GetMonth(RTC);
	(*date).Year = LL_RTC_DATE_GetYear(RTC);
	(*date).WeekDay = LL_RTC_DATE_GetWeekDay(RTC);
	
}
