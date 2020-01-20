#ifndef _CURRENT_H_
#define _CURRENT_H_


#include "stm32f4xx.h"

extern vs16 ADC2ConvertedValue[4];
extern vs16 ADC3ConvertedValue[4];

void current_Init(void);//电机电流采集

#endif
