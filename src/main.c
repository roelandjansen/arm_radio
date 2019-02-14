/*******************************************************************************

                   main.c module of the program ARM_Radio
                                                  
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
#define  INMAIN

// Includes ------------------------------------------------------------------
#include "main.h"

//----------------------------------------------------------------------------
int main(void)
{
  HAL_DeInit();
  HAL_Init();
// Configure the System clock for a frequency of 200 MHz 
  SystemClock_Config();

// Init some display stuff  
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0, (uint32_t) LCD_FRAME_BUFFER);
  BSP_LCD_SetLayerVisible(0, ENABLE);
  BSP_LCD_SelectLayer(0);

  GUI_Init();
  Touch_Initialize();

  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();

// Create now the user interface, by loading the panel built 
// using the GUIBuilder utility  
  hWin = CreateSDR_MainForm();

// Set now the default values for some variables
  cwpitch = CWPITCH;
  os_time = 0;
  meanavg = 0.f;
  Qfactor = 0.987f;         // Q factor for the CW peak filter
  Muted   = false;
  AMindex  = LSBindex = 1;
  USBindex = CWindex  = 1;
  bw[AM]   = bw[LSB]  = Wide;
  bw[USB]  = bw[CW]   = Wide;
  agc[AM]  = agc[LSB] = Slow;
  agc[USB] = Slow;
  agc[CW]  = Fast;
  AGC_decay[Fast] = 0.9995f;
  AGC_decay[Slow] = 0.99995f;
  Hangcount[Fast] = 2;
  Hangcount[Slow] = 30;
  AgcThreshold    = 1.92e-4f;
  SDR_compute_IIR_parms();  // compute the IIR parms for the CW peak filter

// init the decimating FIR control blocks   
  arc = arm_fir_decimate_init_f32(&SfirR, NUMFIRCOEFS, 4, FIRcoefs, FIRstate1R, BSIZE*4); 
  while(arc != ARM_MATH_SUCCESS);   // spin loop if error
  arc = arm_fir_decimate_init_f32(&SfirI, NUMFIRCOEFS, 4, FIRcoefs, FIRstate1I, BSIZE*4); 
  while(arc != ARM_MATH_SUCCESS);   // spin loop if error

// initialize the NVIC for PriorityGroup 4
  Set_NVIC_PriorityGroupConfig(PriorityGroup_4);

// initialize the various components of the application
  Load_Presets();
  Tune_Preset(1);      // Set the initial tuning to Preset 1 
  SDR_InitGPIO();
  SDR_InitEXTI();
  SDR_InitADC();
  SDR_InitDAC();
  SDR_StartADC();      // Start now the data acquisition/processing cycle

// Infinite loop in the main task, which has a idle priority
  while (true)
  {
// compute the smoothed average of the samples of the ADC, to be subtracted
// as offset, when converting from short word to floating point    
        meanavg = (meanavg * 49.f + mean)/50.f;

    GUI_TOUCH_Exec();   // execute Touchscreen support
    GUI_Exec();         // execute all GUI jobs
    GUI_Delay(10);        // no need to hurry...
  }
}
//-----------------------------------------------------------------------------
