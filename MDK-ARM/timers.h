
#ifndef TIMERS_H__
#define TIMERS_H__

#include <stdint.h>
#include "gps.h"
#include "stm32l0xx_ll_rtc.h"

#define RTC_ASYNC_PRE 128		// RTC Asynchronos Prescaler
#define RTC_SYNC_PRE  256		// RTC Synchronos Prescaler		



void initTIM2(void);
void tim2Delay(uint16_t delay);

void RTC_initAlarm(void);
void RTC_setTimeDate(LL_RTC_TimeTypeDef time, LL_RTC_DateTypeDef date);
void RTC_getTimeDate(LL_RTC_TimeTypeDef* time, LL_RTC_DateTypeDef* date);


#endif // TIMERS_H__