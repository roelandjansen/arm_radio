/*******************************************************************************

                   main.h module of the program ARM_Radio
						                          
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
 
#define true  1
#define false 0 
#define __FPU_PRESENT 1

typedef uint8_t    u8;
typedef enum {AM=0, LSB=1, USB=2, CW=3} Mode;
typedef enum {Fast, Slow}   Agctype;
typedef enum {Narrow, Wide} Bwidth;
typedef struct 
        {
          char   name[16];
          float  freq;
          Mode   mode;
          Bwidth bw;
        } Presets;
        
#define MAXPRESETS   14 

#define myPI         3.14159265358979f
#define TWOPI        (2.f*myPI)
#define CWPITCH      650.f
#define SAMPLINGRATE 1785714.286f
#define RATEOUT      (SAMPLINGRATE/64)
#define BSIZE        512
#define FFTLEN       BSIZE*2
#define NUMFIRCOEFS  64
#define DIRECTFFT    0
#define INVERSEFFT   1
#define NOREVERSE    1

#define hSTATION       (GUI_ID_USER + 0x02)
#define hVOL           (GUI_ID_USER + 0x05)
#define hFRADIO        (GUI_ID_USER + 0x06)
#define hFPLUS         (GUI_ID_USER + 0x03)
#define hFMINUS        (GUI_ID_USER + 0x04)
#define hMUTE          (GUI_ID_USER + 0x09)
#define hAM            (GUI_ID_USER + 0x0A)
#define hLSB           (GUI_ID_USER + 0x0B)
#define hUSB           (GUI_ID_USER + 0x0C)
#define hCW            (GUI_ID_USER + 0x0D)
#define hNARROW        (GUI_ID_USER + 0x0E) 
#define hWIDE          (GUI_ID_USER + 0x0F)
#define hFAST          (GUI_ID_USER + 0x12)
#define hSLOW          (GUI_ID_USER + 0x13)
#define hFREQ          (GUI_ID_USER + 0x14) 
 
#define AIRCR_VECTKEY_MASK ((uint32_t)0x05FA0000)
#define PriorityGroup_4    ((uint32_t)0x300) // 4 bits for pre-emption priority
                                             // 0 bits for subpriority

// Includes ------------------------------------------------------------------
// some of these include files have ad-hoc modifications for this project
#include "misc.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_ll_fmc.h"
#include "stm32f4xx_hal_def.h"
#include "core_cm4.h"
#include "stm32f429i_discovery.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_flash_ex.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_sdram.h"
#include <WM.h>
#include <BUTTON.h>
#include <arm_math.h> 
#include <arm_const_structs.h>
#include "SDR_math.h"
#include "Board_LED.h"
#include "Board_Buttons.h"
#include "GUI.h"
#include "DIALOG.h"
#include "Driver_I2C.h"

#ifdef   INMAIN
  #include "FIRcoefs.h"
#endif

#include "Globals.h"

#ifndef TSC_I2C_PORT
 #define TSC_I2C_PORT    3              // I2C Port number
#endif

#define DAC_DHR12R2_ADDRESS  0x40007414
#define TSC_I2C_ADDR    0x41            // 7-bit I2C address
#define PF8_HI          GPIOF->BSRRL = 256   
#define PF8_LO          GPIOF->BSRRH = 256   

/* I2C Driver */
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(TSC_I2C_PORT);
#define ptrI2C           (&I2C_Driver_(TSC_I2C_PORT))

extern uint32_t os_time;
extern volatile GUI_TIMER_TIME OS_TimeMS;

extern void SystemClock_Config(void);
extern void Error_Handler(void);
extern void SysTick_Handler(void);

extern uint32_t bouncing(void);
extern void Load_Presets(void);
extern void Tune_Preset(u8 Idx);
extern void SetMode(WM_HWIN ptr, Mode newmode);
extern void SetBW(WM_HWIN ptr, Bwidth newbw);
extern void SetAGC(WM_HWIN ptr, Agctype newagc);
extern void ChangeColor(WM_HWIN ptr, int handle, int color);
extern void SetFstep(int idx);
extern void FplusClicked(void);
extern void FminusClicked(void);
extern WM_HWIN CreateSDR_MainForm(void);
extern int32_t Touch_Initialize(void);
extern void BSP_LCD_DisplayStringAtLineMode
	          (uint16_t Line, uint8_t *ptr, Text_AlignModeTypdef mode);
extern void Set_NVIC_PriorityGroupConfig(uint32_t PriorityGroup);
extern void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);
extern void SDR_ADC_Init(ADC_TypeDef* ADCx, SDR_ADC_InitTypeDef* ADC_InitStruct);

extern void SDR_InitGPIO(void);
extern void SDR_InitEXTI(void);
extern void SDR_InitDAC(void);
extern void SDR_InitADC(void);
extern void SDR_StartADC(void);

#endif /* __MAIN_H */
