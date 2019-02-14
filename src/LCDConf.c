/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2014  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.24 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration

              Display:        STM32F429I-Discovery
              Controller:     ILI9341

              Display driver: GUIDRV_Lin
              Operation mode: 24bpp, 18-bit RGB interface with SPI

---------------------------END-OF-HEADER------------------------------
*/

#ifdef _RTE_
#include "RTE_Components.h"
#endif
#include "GUI.h"
#include "GUI_Private.h"
#include "GUIDRV_Lin.h"
#include "LCD_X.h"

#include "my_hal.h"

/*********************************************************************
*
*       Supported color modes
*
**********************************************************************
*/
//      Color mode               Conversion    Driver (default orientation)
//      -------------------------------------------------------------------
#define COLOR_MODE_ARGB8888 0 // GUICC_M8888I  GUIDRV_LIN_32
#define COLOR_MODE_RGB888   1 // GUICC_M888    GUIDRV_LIN_24
#define COLOR_MODE_RGB565   2 // GUICC_M565    GUIDRV_LIN_16
#define COLOR_MODE_ARGB1555 3 // GUICC_M1555I  GUIDRV_LIN_16
#define COLOR_MODE_ARGB4444 4 // GUICC_M4444I  GUIDRV_LIN_16
#define COLOR_MODE_L8       5 // GUICC_8666    GUIDRV_LIN_8
#define COLOR_MODE_AL44     6 // GUICC_1616I   GUIDRV_LIN_8
#define COLOR_MODE_AL88     7 // GUICC_88666I  GUIDRV_LIN_16

/*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/

/*********************************************************************
*
*       Common
*/

//
// Physical display size
//
#define XSIZE_PHYS 240
#define YSIZE_PHYS 320

//
// Buffers / VScreens
//
#define NUM_BUFFERS   1
#define NUM_VSCREENS  1

//
// Redefine number of Layers. Must be equal or less than in GUIConf.h!
//
#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS 1

//
// Display orientation
//
//#define DISPLAY_ORIENTATION  0
//#define DISPLAY_ORIENTATION               (GUI_MIRROR_X | GUI_MIRROR_Y)
//#define DISPLAY_ORIENTATION (GUI_SWAP_XY | GUI_MIRROR_Y)
#define DISPLAY_ORIENTATION (GUI_SWAP_XY | GUI_MIRROR_X)

//
// Touch screen
//
#ifdef RTE_Graphics_Touchscreen
#define USE_TOUCH   1
#else
//#define USE_TOUCH   0
#define USE_TOUCH   1
#endif
//
// Touch screen calibration
#define TOUCH_X_MIN 0x0F00
#define TOUCH_X_MAX 0x0120
#define TOUCH_Y_MIN 0x01A0
#define TOUCH_Y_MAX 0x0F00

//
// Video RAM Address
//
#define VRAM_ADDR 0xD0000000

/*********************************************************************
*
*       Layer 0
*/

//
// Layer size
//
#define XSIZE_0 240
#define YSIZE_0 320

//
// Color mode
//
#define COLOR_MODE_0 COLOR_MODE_RGB888

/*********************************************************************
*
*       Layer 1
*/

//
// Layer size
//
#define XSIZE_1 240
#define YSIZE_1 320

//
// Color mode
//
#define COLOR_MODE_1 COLOR_MODE_RGB888

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   NUM_BUFFERS
  #define NUM_BUFFERS 1
#else
  #if (NUM_BUFFERS <= 0)
    #error At least one buffer needs to be defined!
  #endif
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif
#if (GUI_NUM_LAYERS < 1)
  #error At least one layer needs to be defined!
#else
  #if (GUI_NUM_LAYERS > 2)
    #error Maximum two layers are supported by this configuration!
  #endif
#endif
#ifndef   DISPLAY_ORIENTATION
  #define DISPLAY_ORIENTATION  0
#endif

#if ((DISPLAY_ORIENTATION & GUI_SWAP_XY) != 0)
#define WIDTH       YSIZE_PHYS  /* Screen Width (in pixels)         */
#define HEIGHT      XSIZE_PHYS  /* Screen Hight (in pixels)         */
#else
#define WIDTH       XSIZE_PHYS  /* Screen Width (in pixels)         */
#define HEIGHT      YSIZE_PHYS  /* Screen Hight (in pixels)         */
#endif

#if ((DISPLAY_ORIENTATION & GUI_SWAP_XY) != 0)
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_X) != 0)
    #define TOUCH_TOP    TOUCH_X_MAX
    #define TOUCH_BOTTOM TOUCH_X_MIN
  #else
    #define TOUCH_TOP    TOUCH_X_MIN
    #define TOUCH_BOTTOM TOUCH_X_MAX
  #endif
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_Y) != 0)
    #define TOUCH_LEFT   TOUCH_Y_MAX
    #define TOUCH_RIGHT  TOUCH_Y_MIN
  #else
    #define TOUCH_LEFT   TOUCH_Y_MIN
    #define TOUCH_RIGHT  TOUCH_Y_MAX
  #endif
#else
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_X) != 0)
    #define TOUCH_LEFT   TOUCH_X_MAX
    #define TOUCH_RIGHT  TOUCH_X_MIN
  #else
    #define TOUCH_LEFT   TOUCH_X_MIN
    #define TOUCH_RIGHT  TOUCH_X_MAX
  #endif
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_Y) != 0)
    #define TOUCH_TOP    TOUCH_Y_MAX
    #define TOUCH_BOTTOM TOUCH_Y_MIN
  #else
    #define TOUCH_TOP    TOUCH_Y_MIN
    #define TOUCH_BOTTOM TOUCH_Y_MAX
  #endif
#endif

/*********************************************************************
*
*       Automatic selection of driver and color conversion
*
**********************************************************************
*/

#if   (COLOR_MODE_0 == COLOR_MODE_ARGB8888)
  #define COLOR_CONVERSION_0 GUICC_M8888I
#elif (COLOR_MODE_0 == COLOR_MODE_RGB888)
  #define COLOR_CONVERSION_0 GUICC_M888
#elif (COLOR_MODE_0 == COLOR_MODE_RGB565)
  #define COLOR_CONVERSION_0 GUICC_M565
#elif (COLOR_MODE_0 == COLOR_MODE_ARGB1555)
  #define COLOR_CONVERSION_0 GUICC_M1555I
#elif (COLOR_MODE_0 == COLOR_MODE_ARGB4444)
  #define COLOR_CONVERSION_0 GUICC_M4444I
#elif (COLOR_MODE_0 == COLOR_MODE_L8)
  #define COLOR_CONVERSION_0 GUICC_8666
#elif (COLOR_MODE_0 == COLOR_MODE_AL44)
  #define COLOR_CONVERSION_0 GUICC_1616I
#elif (COLOR_MODE_0 == COLOR_MODE_AL88)
  #define COLOR_CONVERSION_0 GUICC_88666I
#else
  #error Unsupported layer 0 color mode!
#endif

#if   (COLOR_MODE_0 == COLOR_MODE_ARGB8888)
  #define PIXEL_BITS_0 32
  #if   (DISPLAY_ORIENTATION == (0))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_32
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OX_32
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OY_32
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OXY_32
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OS_32
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_X))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OSX_32
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OSY_32
  #else
    #error Unsupported display orientation!
  #endif
#elif (COLOR_MODE_0 == COLOR_MODE_RGB888)
  #define PIXEL_BITS_0 24
  #if   (DISPLAY_ORIENTATION == (0))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_24
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OX_24
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OY_24
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OXY_24
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OS_24
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_X))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OSX_24
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OSY_24
  #else
    #error Unsupported display orientation!
  #endif
#elif (COLOR_MODE_0 == COLOR_MODE_RGB565)   \
   || (COLOR_MODE_0 == COLOR_MODE_ARGB1555) \
   || (COLOR_MODE_0 == COLOR_MODE_ARGB4444) \
   || (COLOR_MODE_0 == COLOR_MODE_AL88)
  #define PIXEL_BITS_0 16
  #if   (DISPLAY_ORIENTATION == (0))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_16
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OX_16
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OY_16
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OXY_16
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OS_16
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_X))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OSX_16
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OSY_16
  #else
    #error Unsupported display orientation!
  #endif
#elif (COLOR_MODE_0 == COLOR_MODE_L8)   \
   || (COLOR_MODE_0 == COLOR_MODE_AL44)
  #define PIXEL_BITS_0 8
  #if   (DISPLAY_ORIENTATION == (0))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_8
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OX_8
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_0 GUIDRV_LIN_OXY_8
  #else
    #error Unsupported display orientation!
  #endif
#endif

#define PIXEL_BYTES_0 ((PIXEL_BITS_0 + 7)/8)

#if (GUI_NUM_LAYERS > 1)

#if   (COLOR_MODE_1 == COLOR_MODE_ARGB8888)
  #define COLOR_CONVERSION_1 GUICC_M8888I
#elif (COLOR_MODE_1 == COLOR_MODE_RGB888)
  #define COLOR_CONVERSION_1 GUICC_M888
#elif (COLOR_MODE_1 == COLOR_MODE_RGB565)
  #define COLOR_CONVERSION_1 GUICC_M565
#elif (COLOR_MODE_1 == COLOR_MODE_ARGB1555)
  #define COLOR_CONVERSION_1 GUICC_M1555I
#elif (COLOR_MODE_1 == COLOR_MODE_ARGB4444)
  #define COLOR_CONVERSION_1 GUICC_M4444I
#elif (COLOR_MODE_1 == COLOR_MODE_L8)
  #define COLOR_CONVERSION_1 GUICC_8666
#elif (COLOR_MODE_1 == COLOR_MODE_AL44)
  #define COLOR_CONVERSION_1 GUICC_1616I
#elif (COLOR_MODE_1 == COLOR_MODE_AL88)
  #define COLOR_CONVERSION_1 GUICC_88666I
#else
  #error Unsupported layer 1 color mode!
#endif

#if   (COLOR_MODE_1 == COLOR_MODE_ARGB8888)
  #define PIXEL_BITS_1 32
  #if   (DISPLAY_ORIENTATION == (0))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_32
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OX_32
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OY_32
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OXY_32
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OS_32
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_X))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OSX_32
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OSY_32
  #else
    #error Unsupported display orientation!
  #endif
#elif (COLOR_MODE_1 == COLOR_MODE_RGB888)
  #define PIXEL_BITS_1 24
  #if   (DISPLAY_ORIENTATION == (0))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_24
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OX_24
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OY_24
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OXY_24
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OS_24
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_X))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OSX_24
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OSY_24
  #else
    #error Unsupported display orientation!
  #endif
#elif (COLOR_MODE_1 == COLOR_MODE_RGB565)   \
   || (COLOR_MODE_1 == COLOR_MODE_ARGB1555) \
   || (COLOR_MODE_1 == COLOR_MODE_ARGB4444) \
   || (COLOR_MODE_1 == COLOR_MODE_AL88)
  #define PIXEL_BITS_1 16
  #if   (DISPLAY_ORIENTATION == (0))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_16
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OX_16
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OY_16
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OXY_16
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OS_16
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_X))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OSX_16
  #elif (DISPLAY_ORIENTATION == (GUI_SWAP_XY | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OSY_16
  #else
    #error Unsupported display orientation!
  #endif
#elif (COLOR_MODE_1 == COLOR_MODE_L8)   \
   || (COLOR_MODE_1 == COLOR_MODE_AL44)
  #define PIXEL_BITS_1 8
  #if   (DISPLAY_ORIENTATION == (0))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_8
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OX_8
  #elif (DISPLAY_ORIENTATION == (GUI_MIRROR_X | GUI_MIRROR_Y))
    #define DISPLAY_DRIVER_1 GUIDRV_LIN_OXY_8
  #else
    #error Unsupported display orientation!
  #endif
#endif

#define PIXEL_BYTES_1 ((PIXEL_BITS_1 + 7)/8)

#endif

/*********************************************************************
*
*       ILI934 Registers
*
*********************************************************************
*/
#define ILI9341_SLEEP_OUT       0x11    // Sleep out register
#define ILI9341_GAMMA           0x26    // Gamma register
#define ILI9341_DISPLAY_OFF     0x28    // Display off register
#define ILI9341_DISPLAY_ON      0x29    // Display on register
#define ILI9341_COLUMN_ADDR     0x2A    // Column address register
#define ILI9341_PAGE_ADDR       0x2B    // Page address register
#define ILI9341_GRAM            0x2C    // GRAM register
#define ILI9341_MAC             0x36    // Memory Access Control register
#define ILI9341_PIXEL_FORMAT    0x3A    // Pixel Format register
#define ILI9341_WDB             0x51    // Write Brightness Display register
#define ILI9341_WCD             0x53    // Write Control Display register
#define ILI9341_RGB_INTERFACE   0xB0    // RGB Interface Signal Control
#define ILI9341_FRC             0xB1    // Frame Rate Control register
#define ILI9341_BPC             0xB5    // Blanking Porch Control register
#define ILI9341_DFC             0xB6    // Display Function Control register
#define ILI9341_POWER1          0xC0    // Power Control 1 register
#define ILI9341_POWER2          0xC1    // Power Control 2 register
#define ILI9341_VCOM1           0xC5    // VCOM Control 1 register
#define ILI9341_VCOM2           0xC7    // VCOM Control 2 register
#define ILI9341_POWERA          0xCB    // Power control A register
#define ILI9341_POWERB          0xCF    // Power control B register
#define ILI9341_PGAMMA          0xE0    // Positive Gamma Correction register
#define ILI9341_NGAMMA          0xE1    // Negative Gamma Correction register
#define ILI9341_DTCA            0xE8    // Driver timing control A
#define ILI9341_DTCB            0xEA    // Driver timing control B
#define ILI9341_POWER_SEQ       0xED    // Power on sequence register
#define ILI9341_3GAMMA_EN       0xF2    // 3 Gamma enable register
#define ILI9341_INTERFACE       0xF6    // Interface control register
#define ILI9341_PRC             0xF7    // Pump ratio control register

#define ILI9341_WriteReg(reg)   LCD_X_Write0_8(reg)
#define ILI9341_WriteData(data) LCD_X_Write1_8(data)

/*********************************************************************
*
*       Private constants and variables
*
*********************************************************************
*/
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#if (GUI_NUM_LAYERS == 2)
#define VRAM_SIZE \
 ((XSIZE_0 * YSIZE_0 * PIXEL_BYTES_0 * NUM_VSCREENS * NUM_BUFFERS) + \
  (XSIZE_1 * YSIZE_1 * PIXEL_BYTES_1 * NUM_VSCREENS * NUM_BUFFERS))
#else
#define VRAM_SIZE \
  (XSIZE_0 * YSIZE_0 * PIXEL_BYTES_0 * NUM_VSCREENS * NUM_BUFFERS)
#endif

static uint8_t _VRAM[VRAM_SIZE] __attribute__((at(VRAM_ADDR)));

static const uint32_t _VRAMAddr[GUI_NUM_LAYERS] = {
  VRAM_ADDR,
#if (GUI_NUM_LAYERS > 1)
  VRAM_ADDR + XSIZE_0 * YSIZE_0 * NUM_VSCREENS * NUM_BUFFERS
#endif
};

static const uint8_t _ColorMode[GUI_NUM_LAYERS] = {
  COLOR_MODE_0,
#if (GUI_NUM_LAYERS > 1)
  COLOR_MODE_1
#endif
};

static const uint8_t _BytesPerPixel[GUI_NUM_LAYERS] = {
  PIXEL_BYTES_0,
#if (GUI_NUM_LAYERS > 1)
  PIXEL_BYTES_1
#endif
};

static int _xSize[GUI_NUM_LAYERS];
static int _ySize[GUI_NUM_LAYERS];

#if (DISPLAY_ORIENTATION == 0)
static int _BufferIndex  [GUI_NUM_LAYERS];
#endif
static int _PendingBuffer[GUI_NUM_LAYERS];

static uint32_t _CLUT[256];

// Buffers for DMA2D color conversion (hardware does not support overlapping regions)
static uint32_t _BufferDMA[MAX(XSIZE_PHYS,YSIZE_PHYS)];
static uint32_t _BufferFG [MAX(XSIZE_PHYS,YSIZE_PHYS)];
static uint32_t _BufferBG [MAX(XSIZE_PHYS,YSIZE_PHYS)];

static LTDC_HandleTypeDef  LTDC_Handle;
static DMA2D_HandleTypeDef DMA2D_Handle;

/*********************************************************************
*
*       Private code
*
**********************************************************************
*/

/*********************************************************************
*
*       _DMA_Copy
*/
#if (DISPLAY_ORIENTATION == 0)
static void _DMA_Copy(uint32_t SrcAddress,     uint32_t DstAddress,
                      uint32_t Width,          uint32_t Height,
                      uint32_t InputOffset,    uint32_t OutputOffset,
                      uint32_t InputColorMode, uint32_t OutputColorMode) {

  DMA2D_Handle.Init.Mode = (InputColorMode != OutputColorMode) ?
                            DMA2D_M2M_PFC : DMA2D_M2M;
  DMA2D_Handle.Init.ColorMode = OutputColorMode;
  DMA2D_Handle.Init.OutputOffset = OutputOffset;
  HAL_DMA2D_Init(&DMA2D_Handle);

  DMA2D_Handle.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  DMA2D_Handle.LayerCfg[1].InputAlpha = 0;
  DMA2D_Handle.LayerCfg[1].InputColorMode = InputColorMode;
  DMA2D_Handle.LayerCfg[1].InputOffset = InputOffset;
  HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 1);

  HAL_DMA2D_Start_IT(&DMA2D_Handle, SrcAddress, DstAddress, Width, Height);
  while (HAL_DMA2D_GetState(&DMA2D_Handle) == HAL_DMA2D_STATE_BUSY);
}
#endif

/*********************************************************************
*
*       _DMA_Fill
*/
#if (DISPLAY_ORIENTATION == 0)
static void _DMA_Fill(uint32_t DstAddress,
                      uint32_t Width,      uint32_t Height,
                      uint32_t LineOffset,
                      uint32_t ColorMode,  uint32_t Color) {

  DMA2D_Handle.Init.Mode = DMA2D_R2M;
  DMA2D_Handle.Init.ColorMode = ColorMode;
  DMA2D_Handle.Init.OutputOffset = LineOffset;
  HAL_DMA2D_Init(&DMA2D_Handle);

  HAL_DMA2D_Start_IT(&DMA2D_Handle, Color, DstAddress, Width, Height);
  while (HAL_DMA2D_GetState(&DMA2D_Handle) == HAL_DMA2D_STATE_BUSY);
}
#endif

/*********************************************************************
*
*       _DMA_ConvertColors
*/
#ifdef DMA2D_COLOR_CONVERISON
static void _DMA_ConvertColors(uint32_t SrcAddress,     uint32_t DstAddress,
                               uint32_t NumItems,
                               uint32_t InputColorMode, uint32_t OutputColorMode) {

  if (OutputColorMode == InputColorMode) return;

  DMA2D_Handle.Init.Mode = DMA2D_M2M_PFC;
  DMA2D_Handle.Init.ColorMode = OutputColorMode;
  DMA2D_Handle.Init.OutputOffset = 0;
  HAL_DMA2D_Init(&DMA2D_Handle);

  DMA2D_Handle.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  DMA2D_Handle.LayerCfg[1].InputAlpha = 0;
  DMA2D_Handle.LayerCfg[1].InputColorMode = InputColorMode;
  DMA2D_Handle.LayerCfg[1].InputOffset = 0;
  HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 1);

  HAL_DMA2D_Start_IT(&DMA2D_Handle, SrcAddress, DstAddress, NumItems, 1);
  while (HAL_DMA2D_GetState(&DMA2D_Handle) == HAL_DMA2D_STATE_BUSY);
}
#endif

/*********************************************************************
*
*       _DMA_AlphaBlending
*/
static void _DMA_AlphaBlending(uint32_t * pFG, uint32_t * pBG, uint32_t * pDst,
                               uint32_t NumItems) {

  DMA2D_Handle.Init.Mode = DMA2D_M2M_BLEND;
  DMA2D_Handle.Init.ColorMode = COLOR_MODE_ARGB8888;
  DMA2D_Handle.Init.OutputOffset = 0;
  HAL_DMA2D_Init(&DMA2D_Handle);

  DMA2D_Handle.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  DMA2D_Handle.LayerCfg[1].InputAlpha = 0;
  DMA2D_Handle.LayerCfg[1].InputColorMode = COLOR_MODE_ARGB8888;
  DMA2D_Handle.LayerCfg[1].InputOffset = 0;
  HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 1);

  DMA2D_Handle.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  DMA2D_Handle.LayerCfg[0].InputAlpha = 0;
  DMA2D_Handle.LayerCfg[0].InputColorMode = COLOR_MODE_ARGB8888;
  DMA2D_Handle.LayerCfg[0].InputOffset = 0;
  HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 0);

  HAL_DMA2D_BlendingStart_IT(&DMA2D_Handle, (uint32_t)pFG, (uint32_t)pBG, (uint32_t)pDst, NumItems, 1);
  while (HAL_DMA2D_GetState(&DMA2D_Handle) == HAL_DMA2D_STATE_BUSY);
}

/*********************************************************************
*
*       _DMA_MixColors
*/
static void _DMA_MixColors(uint32_t * pFG, uint32_t * pBG, uint32_t * pDst,
                           uint8_t Intens, uint32_t NumItems) {

  DMA2D_Handle.Init.Mode = DMA2D_M2M_BLEND;
  DMA2D_Handle.Init.ColorMode = COLOR_MODE_ARGB8888;
  DMA2D_Handle.Init.OutputOffset = 0;
  HAL_DMA2D_Init(&DMA2D_Handle);

  DMA2D_Handle.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
  DMA2D_Handle.LayerCfg[1].InputAlpha = Intens;
  DMA2D_Handle.LayerCfg[1].InputColorMode = COLOR_MODE_ARGB8888;
  DMA2D_Handle.LayerCfg[1].InputOffset = 0;
  HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 1);

  DMA2D_Handle.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  DMA2D_Handle.LayerCfg[0].InputAlpha = 255 - Intens;
  DMA2D_Handle.LayerCfg[0].InputColorMode = COLOR_MODE_ARGB8888;
  DMA2D_Handle.LayerCfg[0].InputOffset = 0;
  HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 0);

  HAL_DMA2D_BlendingStart_IT(&DMA2D_Handle, (uint32_t)pFG, (uint32_t)pBG, (uint32_t)pDst, NumItems, 1);
  while (HAL_DMA2D_GetState(&DMA2D_Handle) == HAL_DMA2D_STATE_BUSY);
}

/*********************************************************************
*
*       _DMA_LoadCLUT
*/
static void _DMA_LoadCLUT(uint32_t * pCLUT, uint8_t NumItems) {
  DMA2D_CLUTCfgTypeDef CLUTCfg;

  CLUTCfg.CLUTColorMode = DMA2D_CCM_ARGB8888;
  CLUTCfg.pCLUT = pCLUT;
  CLUTCfg.Size = NumItems - 1;
  HAL_DMA2D_ConfigCLUT(&DMA2D_Handle, CLUTCfg, 1);
  HAL_DMA2D_EnableCLUT(&DMA2D_Handle, 1);
}

/*********************************************************************
*
*       _GetBufferSize
*/
#if (DISPLAY_ORIENTATION == 0)
static uint32_t _GetBufferSize(int LayerIndex) {
  return (_xSize[LayerIndex] * _ySize[LayerIndex] * _BytesPerPixel[LayerIndex]);
}
#endif

/*********************************************************************
*
*       _CopyBuffer
*/
#if (DISPLAY_ORIENTATION == 0)
static void _CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  uint32_t BufferSize;
  uint32_t SrcAddr, DstAddr;

  BufferSize = _GetBufferSize(LayerIndex);
  SrcAddr = _VRAMAddr[LayerIndex] + BufferSize * IndexSrc;
  DstAddr = _VRAMAddr[LayerIndex] + BufferSize * IndexDst;
  _DMA_Copy(SrcAddr, DstAddr, _xSize[LayerIndex], _ySize[LayerIndex], 0, 0, _ColorMode[LayerIndex], _ColorMode[LayerIndex]);

  // After this all drawing operations are routed to new Buffer[IndexDst]!
  _BufferIndex[LayerIndex] = IndexDst;
}
#endif

/*********************************************************************
*
*       _CopyRect
*/
#if (DISPLAY_ORIENTATION == 0)
static void _CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) {
  uint32_t BufferSize;
  uint32_t Offset;
  uint32_t SrcAddr, DstAddr;

  BufferSize = _GetBufferSize(LayerIndex);
  Offset = _VRAMAddr[LayerIndex] + BufferSize * _BufferIndex[LayerIndex];
  SrcAddr = Offset + (y0 * _xSize[LayerIndex] + x0) * _BytesPerPixel[LayerIndex];
  DstAddr = Offset + (y1 * _xSize[LayerIndex] + x1) * _BytesPerPixel[LayerIndex];
  Offset = _xSize[LayerIndex] - xSize;
  _DMA_Copy(SrcAddr, DstAddr, xSize, ySize, Offset, Offset, _ColorMode[LayerIndex], _ColorMode[LayerIndex]);
}
#endif

/*********************************************************************
*
*       _FillRect
*/
#if (DISPLAY_ORIENTATION == 0)
static void _FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) {
  uint32_t BufferSize;
  uint32_t Offset;
  uint32_t DstAddr;
  uint32_t xSize, ySize;

  if (GUI_GetDrawMode() == GUI_DM_XOR) {
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))_FillRect);
  } else {
    BufferSize = _GetBufferSize(LayerIndex);
    Offset = _VRAMAddr[LayerIndex] + BufferSize * _BufferIndex[LayerIndex];
    DstAddr = Offset + (y0 * _xSize[LayerIndex] + x0) * _BytesPerPixel[LayerIndex];
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    Offset = _xSize[LayerIndex] - xSize;
    _DMA_Fill(DstAddr, xSize, ySize, Offset, _ColorMode[LayerIndex], PixelIndex);
  }
}
#endif

/*********************************************************************
*
*       _DrawBmp8bpp
*/
#if (DISPLAY_ORIENTATION == 0)
static void _DrawBmp8bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine, const LCD_PIXELINDEX * pTrans) {
  uint32_t BufferSize;
  uint32_t Offset;
  uint32_t DstAddr;
  uint32_t InputOffset, OutputOffset;

  BufferSize = _GetBufferSize(LayerIndex);
  Offset = _VRAMAddr[LayerIndex] + BufferSize * _BufferIndex[LayerIndex];
  DstAddr = Offset + (y * _xSize[LayerIndex] + x) * _BytesPerPixel[LayerIndex];
  InputOffset  = BytesPerLine - xSize;
  OutputOffset = _xSize[LayerIndex] - xSize;
  _DMA_Copy((uint32_t)p, DstAddr, xSize, ySize, InputOffset, OutputOffset, COLOR_MODE_L8, _ColorMode[LayerIndex]);
}
#endif

/*********************************************************************
*
*       _DrawBmp16bpp
*/
#if (DISPLAY_ORIENTATION == 0)
static void _DrawBmp16bpp(int LayerIndex, int x, int y, U16 const * p, int xSize, int ySize, int BytesPerLine) {
  uint32_t BufferSize;
  uint32_t Offset;
  uint32_t DstAddr;
  uint32_t InputOffset, OutputOffset;

  BufferSize = _GetBufferSize(LayerIndex);
  Offset = _VRAMAddr[LayerIndex] + BufferSize * _BufferIndex[LayerIndex];
  DstAddr = Offset + (y * _xSize[LayerIndex] + x) * _BytesPerPixel[LayerIndex];
  InputOffset  = (BytesPerLine / 2) - xSize;
  OutputOffset = _xSize[LayerIndex] - xSize;
  _DMA_Copy((uint32_t)p, DstAddr, xSize, ySize, InputOffset, OutputOffset, _ColorMode[LayerIndex], _ColorMode[LayerIndex]);
}
#endif

/*********************************************************************
*
*       _InvertAlpha
*
* Purpose:
*   Color format of DMA2D is different to emWin color format. This routine
*   inverts alpha that it is compatible to emWin and vice versa.
*   Changes are done in the destination memory location.
*/
static void _InvertAlpha(LCD_COLOR * pColorSrc, LCD_COLOR * pColorDst, U32 NumItems) {
  U32 Color;

  do {
    Color = *pColorSrc++;
    *pColorDst++ = Color ^ 0xFF000000;  // Invert alpha
  } while (--NumItems);
}

/*********************************************************************
*
*       _InvertAlpha_SwapRB
*
* Purpose:
*   Color format of DMA2D is different to emWin color format. This routine
*   swaps red and blue and inverts alpha that it is compatible to emWin
*   and vice versa.
*/
static void _InvertAlpha_SwapRB(LCD_COLOR * pColorSrc, LCD_COLOR * pColorDst, U32 NumItems) {
  U32 Color;

  do {
    Color = *pColorSrc++;
    *pColorDst++ = ((Color & 0x000000FF) << 16)         // Swap red <-> blue
                 |  (Color & 0x0000FF00)                // Green
                 | ((Color & 0x00FF0000) >> 16)         // Swap red <-> blue
                 | ((Color & 0xFF000000) ^ 0xFF000000); // Invert alpha
  } while (--NumItems);
}

/*********************************************************************
*
*       _DMA_Index2ColorBulk
*
* Purpose:
*   This routine is used by the emWin color conversion routines to use DMA2D for
*   color conversion. It converts the given index values to 32 bit colors.
*   Because emWin uses ABGR internally and 0x00 and 0xFF for opaque and fully
*   transparent the color array needs to be converted after DMA2D has been used.
*/
#ifdef DMA2D_COLOR_CONVERISON
static void _DMA_Index2ColorBulk(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U32 ColorMode) {

  // Use DMA2D for the conversion
  _DMA_ConvertColors((uint32_t)pIndex, (uint32_t)_BufferDMA, NumItems, ColorMode, COLOR_MODE_ARGB8888);

  // Convert colors from ARGB to ABGR and invert alpha values
  _InvertAlpha_SwapRB((LCD_COLOR *)_BufferDMA, pColor, NumItems);
}
#endif

/*********************************************************************
*
*       _DMA_Color2IndexBulk
*
* Purpose:
*   This routine is used by the emWin color conversion routines to use DMA2D for
*   color conversion. It converts the given 32 bit color array to index values.
*   Because emWin uses ABGR internally and 0x00 and 0xFF for opaque and fully
*   transparent the given color array needs to be converted before DMA2D can be used.
*/
#ifdef DMA2D_COLOR_CONVERISON
static void _DMA_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U32 ColorMode) {

  // Convert colors from ABGR to ARGB and invert alpha values
  _InvertAlpha_SwapRB(pColor, (LCD_COLOR *)_BufferDMA, NumItems);

  // Use DMA2D for the conversion
  _DMA_ConvertColors((uint32_t)_BufferDMA, (uint32_t)pIndex, NumItems, COLOR_MODE_ARGB8888, ColorMode);
}
#endif

/*********************************************************************
*
*       _Color2IndexBulk
*/
#ifdef DMA2D_COLOR_CONVERISON
static void _Color2IndexBulk_M8888I(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Color2IndexBulk(pColor, pIndex, NumItems, COLOR_MODE_ARGB8888);
}
static void _Color2IndexBulk_M888  (LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Color2IndexBulk(pColor, pIndex, NumItems, COLOR_MODE_RGB888);
}
static void _Color2IndexBulk_M565  (LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Color2IndexBulk(pColor, pIndex, NumItems, COLOR_MODE_RGB565);
}
static void _Color2IndexBulk_M1555I(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Color2IndexBulk(pColor, pIndex, NumItems, COLOR_MODE_ARGB1555);
}
static void _Color2IndexBulk_M4444I(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Color2IndexBulk(pColor, pIndex, NumItems, COLOR_MODE_ARGB4444);
}
#endif

/*********************************************************************
*
*       _Index2ColorBulk
*/
#ifdef DMA2D_COLOR_CONVERISON
static void _Index2ColorBulk_M8888I(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Index2ColorBulk(pIndex, pColor, NumItems, COLOR_MODE_ARGB8888);
}
static void _Index2ColorBulk_M888  (void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Index2ColorBulk(pIndex, pColor, NumItems, COLOR_MODE_RGB888);
}
static void _Index2ColorBulk_M565  (void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Index2ColorBulk(pIndex, pColor, NumItems, COLOR_MODE_RGB565);
}
static void _Index2ColorBulk_M1555I(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Index2ColorBulk(pIndex, pColor, NumItems, COLOR_MODE_ARGB1555);
}
static void _Index2ColorBulk_M4444I(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) {
  _DMA_Index2ColorBulk(pIndex, pColor, NumItems, COLOR_MODE_ARGB4444);
}
#endif

/*********************************************************************
*
*       _AlphaBlending
*/
static void _AlphaBlending(LCD_COLOR * pColorFG, LCD_COLOR * pColorBG, LCD_COLOR * pColorDst, U32 NumItems) {

  // Invert input alpha values
  _InvertAlpha(pColorFG, (LCD_COLOR *)_BufferFG, NumItems);
  _InvertAlpha(pColorBG, (LCD_COLOR *)_BufferBG, NumItems);

  // Use DMA2D for mixing
  _DMA_AlphaBlending(_BufferFG, _BufferBG, _BufferDMA, NumItems);

  // Invert output alpha values
  _InvertAlpha((LCD_COLOR *)_BufferDMA, pColorDst, NumItems);
}

/*********************************************************************
*
*       _MixColors
*
* Purpose:
*   Function for mixing up 2 colors with the given intensity.
*   If the background color is completely transparent the
*   foreground color should be used unchanged.
*/
static LCD_COLOR _MixColors(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens) {
  uint32_t ColorFG, ColorBG, ColorDst;

  if ((BkColor & 0xFF000000) == 0xFF000000) {
    return Color;
  }
  ColorFG = Color   ^ 0xFF000000;
  ColorBG = BkColor ^ 0xFF000000;

  _DMA_MixColors(&ColorFG, &ColorBG, &ColorDst, Intens, 1);

  return (ColorDst ^ 0xFF000000);
}

/*********************************************************************
*
*       _MixColorsBulk
*/
static void _MixColorsBulk(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens) {
  int  y;

  for (y = 0; y < ySize; y++) {

    // Invert input alpha values
    _InvertAlpha(pFG, (LCD_COLOR *)_BufferFG, xSize);
    _InvertAlpha(pBG, (LCD_COLOR *)_BufferBG, xSize);

    // Mix Colors
    _DMA_MixColors(_BufferFG, _BufferBG, _BufferDMA, Intens, xSize);

    // Invert output alpha values
    _InvertAlpha((LCD_COLOR *)_BufferDMA, pDst, xSize);

    pFG  += xSize + OffFG;
    pBG  += xSize + OffBG;
    pDst += xSize + OffDest;
  }
}

/*********************************************************************
*
*       _GetpPalConvTable
*
* Purpose:
*   The emWin function LCD_GetpPalConvTable() normally translates the given colors into
*   index values for the display controller. In case of index based bitmaps without
*   transparent pixels we load the palette only to the DMA2D CLUT registers to be
*   translated (converted) during the process of drawing via DMA2D.
*/
static LCD_PIXELINDEX * _GetpPalConvTable(const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int LayerIndex) {
  void (* pFunc)(void);
  int Default = 0;

  // Check if we have a non transparent device independent bitmap
  if (pBitmap->BitsPerPixel == 8) {
    pFunc = LCD_GetDevFunc(LayerIndex, LCD_DEVFUNC_DRAWBMP_8BPP);
    if (pFunc) {
      if (pBitmap->pPal) {
        if (pBitmap->pPal->HasTrans) {
          Default = 1;
        }
      } else {
        Default = 1;
      }
    } else {
      Default = 1;
    }
  } else {
    Default = 1;
  }

  // Default palette management for other cases
  if (Default) {
    return LCD_GetpPalConvTable(pLogPal);
  }

  // Convert palette colors from ARGB to ABGR
  _InvertAlpha_SwapRB((LCD_COLOR *)pLogPal->pPalEntries, (LCD_COLOR *)_BufferDMA, pLogPal->NumEntries);

  // Load CLUT using DMA2D
  _DMA_LoadCLUT(_BufferDMA, pLogPal->NumEntries);

  return ((LCD_PIXELINDEX *)_BufferDMA);
}

/*********************************************************************
*
*       _InitIL9341
*
* Purpose:
*   Initializes the IL9341 controller
*/
static void _InitIL9341(void) {

  /* Configure LCD */
  ILI9341_WriteReg (0xCA);
  ILI9341_WriteData(0xC3);
  ILI9341_WriteData(0x08);
  ILI9341_WriteData(0x50);
  ILI9341_WriteReg (ILI9341_POWERB);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0xC1);
  ILI9341_WriteData(0x30);
  ILI9341_WriteReg (ILI9341_POWER_SEQ);
  ILI9341_WriteData(0x64);
  ILI9341_WriteData(0x03);
  ILI9341_WriteData(0x12);
  ILI9341_WriteData(0x81);
  ILI9341_WriteReg (ILI9341_DTCA);
  ILI9341_WriteData(0x85);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x78);
  ILI9341_WriteReg (ILI9341_POWERA);
  ILI9341_WriteData(0x39);
  ILI9341_WriteData(0x2C);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x34);
  ILI9341_WriteData(0x02);
  ILI9341_WriteReg (ILI9341_PRC);
  ILI9341_WriteData(0x20);
  ILI9341_WriteReg (ILI9341_DTCB);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x00);
  ILI9341_WriteReg (ILI9341_FRC);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x1B);
  ILI9341_WriteReg (ILI9341_DFC);
  ILI9341_WriteData(0x0A);
  ILI9341_WriteData(0xA2);
  ILI9341_WriteReg (ILI9341_POWER1);
  ILI9341_WriteData(0x10);
  ILI9341_WriteReg (ILI9341_POWER2);
  ILI9341_WriteData(0x10);
  ILI9341_WriteReg (ILI9341_VCOM1);
  ILI9341_WriteData(0x45);
  ILI9341_WriteData(0x15);
  ILI9341_WriteReg (ILI9341_VCOM2);
  ILI9341_WriteData(0x90);
  ILI9341_WriteReg(ILI9341_MAC);
  ILI9341_WriteData(0xC8);
  ILI9341_WriteReg (ILI9341_3GAMMA_EN);
  ILI9341_WriteData(0x00);
  ILI9341_WriteReg (ILI9341_RGB_INTERFACE);
  ILI9341_WriteData(0xC2);
  ILI9341_WriteReg (ILI9341_DFC);
  ILI9341_WriteData(0x0A);
  ILI9341_WriteData(0xA7);
  ILI9341_WriteData(0x27);
  ILI9341_WriteData(0x04);

  /* Column address set */
  ILI9341_WriteReg (ILI9341_COLUMN_ADDR);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0xEF);

  /* Page address set */
  ILI9341_WriteReg (ILI9341_PAGE_ADDR);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x01);
  ILI9341_WriteData(0x3F);
  ILI9341_WriteReg (ILI9341_INTERFACE);
  ILI9341_WriteData(0x01);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x06);

  ILI9341_WriteReg (ILI9341_GRAM);
  GUI_Delay(200);

  ILI9341_WriteReg (ILI9341_GAMMA);
  ILI9341_WriteData(0x01);

  ILI9341_WriteReg (ILI9341_PGAMMA);
  ILI9341_WriteData(0x0F);
  ILI9341_WriteData(0x29);
  ILI9341_WriteData(0x24);
  ILI9341_WriteData(0x0C);
  ILI9341_WriteData(0x0E);
  ILI9341_WriteData(0x09);
  ILI9341_WriteData(0x4E);
  ILI9341_WriteData(0x78);
  ILI9341_WriteData(0x3C);
  ILI9341_WriteData(0x09);
  ILI9341_WriteData(0x13);
  ILI9341_WriteData(0x05);
  ILI9341_WriteData(0x17);
  ILI9341_WriteData(0x11);
  ILI9341_WriteData(0x00);
  ILI9341_WriteReg (ILI9341_NGAMMA);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x16);
  ILI9341_WriteData(0x1B);
  ILI9341_WriteData(0x04);
  ILI9341_WriteData(0x11);
  ILI9341_WriteData(0x07);
  ILI9341_WriteData(0x31);
  ILI9341_WriteData(0x33);
  ILI9341_WriteData(0x42);
  ILI9341_WriteData(0x05);
  ILI9341_WriteData(0x0C);
  ILI9341_WriteData(0x0A);
  ILI9341_WriteData(0x28);
  ILI9341_WriteData(0x2F);
  ILI9341_WriteData(0x0F);

  ILI9341_WriteReg (ILI9341_SLEEP_OUT);
  GUI_Delay(200);

  ILI9341_WriteReg (ILI9341_DISPLAY_ON);
  ILI9341_WriteReg (ILI9341_GRAM);
}

/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Initializes the display controller
*/
static void _InitController(int LayerIndex) {
#ifndef WIN32
  static int Initialized = 0;
  LTDC_LayerCfgTypeDef LayerCfg;
  int xSize, ySize;
  int BitsPerPixel;
  int i;

  if (Initialized == 0) { 
    LCD_X_Init();

    _InitIL9341();

    // Enable the LTDC and DMA2D Clock
    __LTDC_CLK_ENABLE();
    __DMA2D_CLK_ENABLE();

    /* DMA2D configuration */
    DMA2D_Handle.Instance = DMA2D;
    DMA2D_Handle.XferCpltCallback  = NULL;
    DMA2D_Handle.XferErrorCallback = NULL;

    // LTDC configuration
    LTDC_Handle.Instance = LTDC;
      
    /* Timing configuration (Typical configuration from ILI9341 datasheet)
         HSYNC   = 10  (9+1)
         VSYNC   = 2   (1+1)
         HBP     = 20  (29-10+1)
         VBP     = 2   (3-2+1)
         ActiveW = 240 (269-20-10+1)
         ActiveH = 320 (323-2-2+1)
         HFP     = 10  (279-240-20-10+1)
         VFP     = 4   (327-320-2-2+1)
     */
    // Configure horizontal synchronization width
    LTDC_Handle.Init.HorizontalSync = 9;
    // Configure vertical synchronization height
    LTDC_Handle.Init.VerticalSync = 1;
    // Configure accumulated horizontal back porch
    LTDC_Handle.Init.AccumulatedHBP = 29;
    // Configure accumulated vertical back porch
    LTDC_Handle.Init.AccumulatedVBP = 3;
    // Configure accumulated active width
    LTDC_Handle.Init.AccumulatedActiveW = 269;
    // Configure accumulated active height
    LTDC_Handle.Init.AccumulatedActiveH = 323;
    // Configure total width
    LTDC_Handle.Init.TotalWidth = 279;
    // Configure total height
    LTDC_Handle.Init.TotalHeigh = 327;

    // Configure R,G,B component values for LCD background color
    LTDC_Handle.Init.Backcolor.Red   = 0;
    LTDC_Handle.Init.Backcolor.Blue  = 0;
    LTDC_Handle.Init.Backcolor.Green = 0;

    // Polarity
    LTDC_Handle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    LTDC_Handle.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    LTDC_Handle.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    LTDC_Handle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
      
    HAL_LTDC_Init(&LTDC_Handle); 

    HAL_NVIC_EnableIRQ(LTDC_IRQn);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);

    Initialized = 1;
  }

#if (DISPLAY_ORIENTATION == 0)
  _BufferIndex  [LayerIndex] = 0;
#endif
  _PendingBuffer[LayerIndex] = -1;

  // Layer configuration
  if (LCD_GetSwapXYEx(LayerIndex)) {
    xSize = LCD_GetYSizeEx(LayerIndex);
    ySize = LCD_GetXSizeEx(LayerIndex);
  } else {
    xSize = LCD_GetXSizeEx(LayerIndex);
    ySize = LCD_GetYSizeEx(LayerIndex);
  }
  LayerCfg.WindowX0 = 0;
  LayerCfg.WindowX1 = xSize - 1;
  LayerCfg.WindowY0 = 0;
  LayerCfg.WindowY1 = ySize - 1;
  LayerCfg.PixelFormat = _ColorMode[LayerIndex];
  LayerCfg.Alpha  = 255;
  LayerCfg.Alpha0 = 0;
  LayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  LayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  LayerCfg.FBStartAdress = _VRAMAddr[LayerIndex];
  LayerCfg.ImageWidth  = xSize;
  LayerCfg.ImageHeight = ySize;
  LayerCfg.Backcolor.Red   = 0;
  LayerCfg.Backcolor.Green = 0;
  LayerCfg.Backcolor.Blue  = 0;
  HAL_LTDC_ConfigLayer(&LTDC_Handle, &LayerCfg, LayerIndex);

  // Configure and Enable LUT
  BitsPerPixel = LCD_GetBitsPerPixelEx(LayerIndex);
  if (BitsPerPixel <= 8) {
    // Enable usage of LUT for all modes with <= 8bpp
    HAL_LTDC_EnableCLUT(&LTDC_Handle, LayerIndex);
  } else {
    // Optional LUT configuration for AL88 mode (16bpp)
    if (_ColorMode[LayerIndex] == COLOR_MODE_AL88) {
      for (i = 0; i < 256; i++) {
        _CLUT[i] = LCD_API_ColorConv_8666.pfIndex2Color(i);
      }
      HAL_LTDC_ConfigCLUT(&LTDC_Handle, _CLUT, 256, LayerIndex);
      HAL_LTDC_EnableCLUT(&LTDC_Handle, LayerIndex);
    }
  }
#endif  /* WIN32 */
}

/*********************************************************************
*
*       _DisplayOn
*/
static void _DisplayOn(void) {
  ILI9341_WriteReg (ILI9341_DISPLAY_ON);
}

/*********************************************************************
*
*       _DisplayOff
*/
static void _DisplayOff(void) {
  ILI9341_WriteReg (ILI9341_DISPLAY_OFF);
}

/*********************************************************************
*
*       _SetVRAMAddr
*
* Purpose:
*   Should set the frame buffer base address
*/
static void _SetVRAMAddr(int LayerIndex, void * pVRAM) {
}

/*********************************************************************
*
*       _SetOrg
*
* Purpose:
*   Should set the origin of the display typically by modifying the
*   frame buffer base address register
*/
static void _SetOrg(int LayerIndex, int xPos, int yPos) {
  uint32_t Address;

  Address = _VRAMAddr[LayerIndex] + (xPos + yPos * _xSize[LayerIndex]) * _BytesPerPixel[LayerIndex];
  HAL_LTDC_SetAddress(&LTDC_Handle, Address, LayerIndex);
}

/*********************************************************************
*
*       _SetLUTEntry
*
* Purpose:
*   Should set the desired LUT entry
*/
static void _SetLUTEntry(int LayerIndex, LCD_COLOR Color, U8 Pos) {
  _CLUT[Pos] = ((Color & 0xFF0000) >> 16) |
                (Color & 0x00FF00)        |
               ((Color & 0x0000FF) << 16);
  HAL_LTDC_ConfigCLUT(&LTDC_Handle, _CLUT, 256, LayerIndex);
}

/*********************************************************************
*
*       _SetVis
*/
static void _SetVis(int LayerIndex, int OnOff) {
  if (OnOff) {
    __HAL_LTDC_LAYER_ENABLE (&LTDC_Handle, LayerIndex);
  } else {
    __HAL_LTDC_LAYER_DISABLE(&LTDC_Handle, LayerIndex);
  }
}

/*********************************************************************
*
*       _SetLayerPos
*/
static void _SetLayerPos(int LayerIndex, int xPos, int yPos) {
  HAL_LTDC_SetWindowPosition(&LTDC_Handle, xPos, yPos, LayerIndex);
}

/*********************************************************************
*
*       _SetLayerSize
*/
static void _SetLayerSize(int LayerIndex, int xSize, int ySize) {
  HAL_LTDC_SetWindowSize(&LTDC_Handle, xSize, ySize, LayerIndex);
  _xSize[LayerIndex] = xSize;
  _ySize[LayerIndex] = ySize;
}

/*********************************************************************
*
*       _SetLayerAlpha
*/
static void _SetLayerAlpha(int LayerIndex, int Alpha) {
  HAL_LTDC_SetAlpha(&LTDC_Handle, Alpha, LayerIndex);
}

/*********************************************************************
*
*       _SetChromaMode
*/
static void _SetChromaMode(int LayerIndex, int ChromaMode) {
  if (ChromaMode) {
    HAL_LTDC_EnableColorKeying (&LTDC_Handle, LayerIndex);
  } else {
    HAL_LTDC_DisableColorKeying(&LTDC_Handle, LayerIndex);
  }
}

/*********************************************************************
*
*       _SetChroma
*/
static void _SetChroma(int LayerIndex, LCD_COLOR ChromaMin, LCD_COLOR ChromaMax) {
  uint32_t RGB_Value;

  RGB_Value = ((ChromaMin & 0xFF0000) >> 16) |
               (ChromaMin & 0x00FF00)        |
              ((ChromaMin & 0x0000FF) << 16);
  HAL_LTDC_ConfigColorKeying(&LTDC_Handle, RGB_Value, LayerIndex);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
void DMA2D_IRQHandler(void) {
  HAL_NVIC_ClearPendingIRQ(DMA2D_IRQn);
  HAL_DMA2D_IRQHandler(&DMA2D_Handle);
}

void LTDC_IRQHandler(void) {
  HAL_NVIC_ClearPendingIRQ(LTDC_IRQn);
  HAL_LTDC_IRQHandler(&LTDC_Handle);
}

/*********************************************************************
*
*       HAL_LTDC_LineEvenCallback
*
* Purpose:
*   LTDC Line Event callback
*/
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {
  uint32_t Address;
  int      i;

  for (i = 0; i < GUI_NUM_LAYERS; i++) {
    if (_PendingBuffer[i] != -1) {
      Address = _VRAMAddr[i] + _xSize[i] * _ySize[i] * _BytesPerPixel[i] * _PendingBuffer[i];
      HAL_LTDC_SetAddress(&LTDC_Handle, Address, i);
      GUI_MULTIBUF_ConfirmEx(i, _PendingBuffer[i]);
      _PendingBuffer[i] = -1;
    }
  }
}

/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
  int i;

  #if (NUM_BUFFERS > 1)
    for (i = 0; i < GUI_NUM_LAYERS; i++) {
      GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS);
    }
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  //
  // Set size of 1st layer
  //
  if (LCD_GetSwapXYEx(0)) {
    LCD_SetSizeEx (0, YSIZE_0, XSIZE_0);
    LCD_SetVSizeEx(0, YSIZE_0, XSIZE_0 * NUM_VSCREENS);
  } else {
    LCD_SetSizeEx (0, XSIZE_0, YSIZE_0);
    LCD_SetVSizeEx(0, XSIZE_0, YSIZE_0 * NUM_VSCREENS);
  }
  #if (GUI_NUM_LAYERS > 1)
    //
    // Set display driver and color conversion for 2nd layer
    //
    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 0);
    //
    // Set size of 2nd layer
    //
    if (LCD_GetSwapXYEx(1)) {
      LCD_SetSizeEx (1, YSIZE_1, XSIZE_1);
      LCD_SetVSizeEx(1, YSIZE_1 * NUM_VSCREENS, XSIZE_1);
    } else {
      LCD_SetSizeEx (1, XSIZE_1, YSIZE_1);
      LCD_SetVSizeEx(1, XSIZE_1, YSIZE_1 * NUM_VSCREENS);
    }
  #endif
  //
  // Set VRAM address
  //
  for (i = 0; i < GUI_NUM_LAYERS; i++) {
    LCD_SetVRAMAddrEx(i, (void *)(_VRAMAddr[i]));
  }
  //
  // Set up custom drawing functions
  //
  #if (DISPLAY_ORIENTATION == 0)
    for (i = 0; i < GUI_NUM_LAYERS; i++) {
      LCD_SetDevFunc(i, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))_CopyBuffer);
      LCD_SetDevFunc(i, LCD_DEVFUNC_COPYRECT, (void(*)(void))_CopyRect);
      if (_ColorMode[i] < COLOR_MODE_L8) {
        LCD_SetDevFunc(i, LCD_DEVFUNC_FILLRECT, (void(*)(void))_FillRect);
        LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_8BPP, (void(*)(void))_DrawBmp8bpp);
      }
      if (_ColorMode[i] == COLOR_MODE_RGB565) {
        LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))_DrawBmp16bpp);
      }
    }
  #endif
  //
  // Set up custom alpha blending function
  //
  GUI_SetFuncAlphaBlending(_AlphaBlending);
  //
  // Set up custom function for translating a bitmap palette into index values.
  // Required to load a bitmap palette into DMA2D CLUT in case of a 8bpp indexed bitmap
  //
  GUI_SetFuncGetpPalConvTable(_GetpPalConvTable);
  //
  // Set up custom color conversion using DMA2D (only direct color modes are supported)
  //
  #ifdef DMA2D_COLOR_CONVERISON
    GUICC_M1555I_SetCustColorConv(_Color2IndexBulk_M1555I, _Index2ColorBulk_M1555I);
    GUICC_M565_SetCustColorConv  (_Color2IndexBulk_M565,   _Index2ColorBulk_M565);
    GUICC_M4444I_SetCustColorConv(_Color2IndexBulk_M4444I, _Index2ColorBulk_M4444I);
    GUICC_M888_SetCustColorConv  (_Color2IndexBulk_M888,   _Index2ColorBulk_M888);
    GUICC_M8888I_SetCustColorConv(_Color2IndexBulk_M8888I, _Index2ColorBulk_M8888I);
  #endif
  //
  // Set up a custom function for mixing up single pixel colors
  //
  GUI_SetFuncMixColors(_MixColors);
  //
  // Set up a custom function for mixing up multiple pixel colors
  //
  GUI_SetFuncMixColorsBulk(_MixColorsBulk);

  #if (USE_TOUCH == 1)
    //
    // Set orientation of touch screen
    //
    GUI_TOUCH_SetOrientation(DISPLAY_ORIENTATION);
    //
    // Calibrate touch screen
    //
    GUI_TOUCH_Calibrate(GUI_COORD_X, 0, WIDTH  - 1, TOUCH_LEFT, TOUCH_RIGHT);
    GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, HEIGHT - 1, TOUCH_TOP,  TOUCH_BOTTOM);
  #endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r = 0;

  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    _InitController(LayerIndex);
    break;
  }
  case LCD_X_ON: {
    //
    // Required if the display controller should support switching on
    //
    _DisplayOn();
    break;
  }
  case LCD_X_OFF: {
    //
    // Required if the display controller should support switching off
    //
    _DisplayOff();
    break;
  }
  case LCD_X_SHOWBUFFER: {
    //
    // Required if multiple buffers are used. The 'Index' element of p contains the buffer index.
    //
    LCD_X_SHOWBUFFER_INFO * p;
    p = (LCD_X_SHOWBUFFER_INFO *)pData;
    _PendingBuffer[LayerIndex] = p->Index;
    break;
  }
  case LCD_X_SETVRAMADDR: {
    //
    // Required for setting the address of the video RAM for drivers
    // with memory mapped video RAM which is passed in the 'pVRAM' element of p
    //
    LCD_X_SETVRAMADDR_INFO * p;
    p = (LCD_X_SETVRAMADDR_INFO *)pData;
    _SetVRAMAddr(LayerIndex, p->pVRAM);
    break;
  }
  case LCD_X_SETORG: {
    //
    // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
    //
    LCD_X_SETORG_INFO * p;
    p = (LCD_X_SETORG_INFO *)pData;
    _SetOrg(LayerIndex, p->xPos, p->yPos);
    break;
  }
  case LCD_X_SETLUTENTRY: {
    //
    // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
    //
    LCD_X_SETLUTENTRY_INFO * p;
    p = (LCD_X_SETLUTENTRY_INFO *)pData;
    _SetLUTEntry(LayerIndex, p->Color, p->Pos);
    break;
  }
  case LCD_X_SETVIS: {
    //
    // Required for setting the layer visibility which is passed in the 'OnOff' element of p
    //
    LCD_X_SETVIS_INFO * p;
    p = (LCD_X_SETVIS_INFO *)pData;
    _SetVis(LayerIndex, p->OnOff);
    break;
  }
  case LCD_X_SETPOS: {
    //
    // Required for setting the layer position which is passed in the 'xPos' and 'yPos' element of p
    //
    LCD_X_SETPOS_INFO * p;
    p = (LCD_X_SETPOS_INFO *)pData;
    _SetLayerPos(LayerIndex, p->xPos, p->yPos);
    break;
  }
  case LCD_X_SETSIZE: {
    //
    // Required for setting the layer size which is passed in the 'xSize' and 'ySize' element of p
    //
    LCD_X_SETSIZE_INFO * p;
    p = (LCD_X_SETSIZE_INFO *)pData;
    _SetLayerSize(LayerIndex, p->xSize, p->ySize);
    break;
  }
  case LCD_X_SETALPHA: {
    //
    // Required for setting the alpha value which is passed in the 'Alpha' element of p
    //
    LCD_X_SETALPHA_INFO * p;
    p = (LCD_X_SETALPHA_INFO *)pData;
    _SetLayerAlpha(LayerIndex, p->Alpha);
    break;
  }
  case LCD_X_SETCHROMAMODE: {
    //
    // Required for setting the chroma mode which is passed in the 'ChromaMode' element of p
    //
    LCD_X_SETCHROMAMODE_INFO * p;
    p = (LCD_X_SETCHROMAMODE_INFO *)pData;
    _SetChromaMode(LayerIndex, p->ChromaMode);
    break;
  }
  case LCD_X_SETCHROMA: {
    //
    // Required for setting the chroma value which is passed in the 'ChromaMin' and 'ChromaMax' element of p
    //
    LCD_X_SETCHROMA_INFO * p;
    p = (LCD_X_SETCHROMA_INFO *)pData;
    _SetChroma(LayerIndex, p->ChromaMin, p->ChromaMax);
    break;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
