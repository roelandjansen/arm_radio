#ifndef PERIPHS_H
  #define PERIPHS_H

#include "stm32f4xx_adc.h"  
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


void SDR_GPIO_Init(GPIO_TypeDef* GPIOx, SDR_GPIO_InitTypeDef* GPIO_InitStruct);
void SDR_GPIO_StructInit(SDR_GPIO_InitTypeDef* GPIO_InitStruct);

#endif
