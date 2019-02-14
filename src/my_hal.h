/*******************************************************************************

                   my_hal.h module of the program ARM_Radio
						                          
						                          Copyright 2015 by Alberto I2PHD, June 2015
						                                      
    This file is part of ARM_Radio.

    ARM_Radio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ARM_Radio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ARM_Radio.  It is contained in the file Copying.txt in the
    same ZIP file where this file was extracted from.				                                      
						                                      
*******************************************************************************/

#ifndef MY_HALH
 #define MY_HALH
 
#include "stm32f4xx_ll_fmc.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_sdram.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_ltdc.h"
#include "stm32f4xx_hal_dma2d.h"
#include "stm32f429i_discovery_lcd.h"

#define HAL_ADC_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA2D_MODULE_ENABLED
#define HAL_LTDC_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_SDRAM_MODULE_ENABLED
#define MX_I2C3

#endif
