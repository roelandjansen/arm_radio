/* -----------------------------------------------------------------------------
 * Copyright (c) 2013 - 2015 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software. Permission is granted to anyone to use this 
 * software for any purpose, including commercial applications, and to alter 
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be 
 *    appreciated but is not required. 
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be 
 *    misrepresented as being the original software. 
 * 
 * 3. This notice may not be removed or altered from any source distribution.
 *   
 *
 * $Date:        22. January 2015
 * $Revision:    V2.02
 *
 * Driver:       Driver_SPI1, Driver_SPI2, Driver_SPI3,
 *               Driver_SPI4, Driver_SPI5, Driver_SPI6
 * Configured:   via RTE_Device.h configuration file
 * Project:      SPI Driver for ST STM32F4xx
 * ----------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting               Value     SPI Interface
 *   ---------------------               -----     -------------
 *   Connect to hardware via Driver_SPI# = 1       use SPI1
 *   Connect to hardware via Driver_SPI# = 2       use SPI2
 *   Connect to hardware via Driver_SPI# = 3       use SPI3
 *   Connect to hardware via Driver_SPI# = 4       use SPI4
 *   Connect to hardware via Driver_SPI# = 5       use SPI5
 *   Connect to hardware via Driver_SPI# = 6       use SPI6
 * -------------------------------------------------------------------- */

/* History:
 *  Version 2.02
 *    Corrected spi->info->mode handling
 *  Version 2.01
 *    STM32CubeMX generated code can also be used to configure the driver.
 *  Version 2.00
 *    Updated to CMSIS Driver API V2.00
 *    Added SPI4 and SPI6
 *  Version 1.04
 *    Added SPI5
 *  Version 1.03
 *    Event send_data_event added to capabilities
 *    SPI IRQ handling corrected
 *  Version 1.02
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.01
 *    Corrections for configuration without DMA
 *  Version 1.00
 *    Initial release
 */

/* STM32CubeMX configuration:
 *
 * Pinout tab:
 *   - Select SPIx peripheral and enable mode
 * Clock Configuration tab:
 *   - Configure clock (APB2 is used for SPIx)
 * Configuration tab:
 *   - Select SPIx under Connectivity section which opens SPIx Configuration window:
 *       - Parameter Settings tab: settings are unused by this driver
 *       - NVIC Settings: enable SPIx global interrupt
 *       - GPIO Settings: configure as needed
 *       - DMA Settings:  to enable DMA transfers you need to
 *           - add SPIx_RX and SPIx_TX DMA Request
 *           - select Normal DMA mode (for RX and TX)
 *           - deselect Use Fifo option (for RX and TX)
 *           - go to NVIC Settings tab and enable RX and TX stream global interrupt
 */

#include "my_hal.h"
#include "SPI_STM32F4xx.h"

#define ARM_SPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,02)

// Driver Version
static const ARM_DRIVER_VERSION DriverVersion = { ARM_SPI_API_VERSION, ARM_SPI_DRV_VERSION };

// Driver Capabilities
static const ARM_SPI_CAPABILITIES DriverCapabilities = {
  0,  /* Simplex Mode (Master and Slave) */
  1,  /* TI Synchronous Serial Interface */
  0,  /* Microwire Interface */
  1   /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};


#ifdef MX_SPI1

#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
extern SPI_HandleTypeDef hspi1;
#endif

// SPI1 Run-Time Information
static SPI_INFO          SPI1_Info = {0};
static SPI_TRANSFER_INFO SPI1_TransfefInfo = {0};


#ifdef MX_SPI1_MOSI_Pin
  static SPI_PIN SPI1_mosi = {MX_SPI1_MOSI_GPIOx, MX_SPI1_MOSI_GPIO_Pin, MX_SPI1_MOSI_GPIO_AF};
#endif
#ifdef MX_SPI1_MISO_Pin
  static SPI_PIN SPI1_miso = {MX_SPI1_MISO_GPIOx, MX_SPI1_MISO_GPIO_Pin, MX_SPI1_MISO_GPIO_AF};
#endif
#ifdef MX_SPI1_SCK_Pin
  static SPI_PIN SPI1_sck  = {MX_SPI1_SCK_GPIOx,  MX_SPI1_SCK_GPIO_Pin,  MX_SPI1_SCK_GPIO_AF};
#endif
#ifdef MX_SPI1_NSS_Pin
  static SPI_PIN SPI1_nss  = {MX_SPI1_NSS_GPIOx,  MX_SPI1_NSS_GPIO_Pin,  MX_SPI1_NSS_GPIO_AF};
#endif

#ifdef MX_SPI1_RX_DMA_Instance
  void SPI1_RX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef  hdma_spi1_rx;
  static SPI_DMA SPI1_DMA_Rx = {
    &hdma_spi1_rx,
    SPI1_RX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI1_RX_DMA_Instance,
    MX_SPI1_RX_DMA_Channel,
    MX_SPI1_RX_DMA_Priority,
    MX_SPI1_RX_DMA_IRQn
#endif
  };
#endif
#ifdef MX_SPI1_TX_DMA_Instance
  void SPI1_TX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi1_tx;
  static SPI_DMA SPI1_DMA_Tx = {
    &hdma_spi1_tx,
    SPI1_TX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI1_TX_DMA_Instance,
    MX_SPI1_TX_DMA_Channel,
    MX_SPI1_TX_DMA_Priority,
    MX_SPI1_TX_DMA_IRQn
#endif
  };
#endif

// SPI1 Resources
static const SPI_RESOURCES SPI1_Resources = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hspi1,
#endif
  SPI1,
  HAL_RCC_GetPCLK2Freq,

  // PINS
  {
#ifdef MX_SPI1_MOSI_Pin
    &SPI1_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI1_MISO_Pin
    &SPI1_miso,
#else
    NULL,
#endif
#ifdef MX_SPI1_SCK_Pin
    &SPI1_sck,
#else
    NULL,
#endif
#ifdef MX_SPI1_NSS_Pin 
    &SPI1_nss,
#else
    NULL,
#endif
  },

  SPI1_IRQn,

#ifdef MX_SPI1_RX_DMA_Instance
  &SPI1_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI1_TX_DMA_Instance
  &SPI1_DMA_Tx,
#else
  NULL,
#endif

  &SPI1_Info,
  &SPI1_TransfefInfo
};
#endif /* MX_SPI1 */

#ifdef MX_SPI2

#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
extern SPI_HandleTypeDef hspi2;
#endif

// SPI2 Run-Time Information
static SPI_INFO          SPI2_Info = {0};
static SPI_TRANSFER_INFO SPI2_TransfefInfo = {0};


#ifdef MX_SPI2_MOSI_Pin
  static SPI_PIN SPI2_mosi = {MX_SPI2_MOSI_GPIOx, MX_SPI2_MOSI_GPIO_Pin, MX_SPI2_MOSI_GPIO_AF};
#endif
#ifdef MX_SPI2_MISO_Pin
  static SPI_PIN SPI2_miso = {MX_SPI2_MISO_GPIOx, MX_SPI2_MISO_GPIO_Pin, MX_SPI2_MISO_GPIO_AF};
#endif
#ifdef MX_SPI2_SCK_Pin
  static SPI_PIN SPI2_sck  = {MX_SPI2_SCK_GPIOx,  MX_SPI2_SCK_GPIO_Pin,  MX_SPI2_SCK_GPIO_AF};
#endif
#ifdef MX_SPI2_NSS_Pin
  static SPI_PIN SPI2_nss  = {MX_SPI2_NSS_GPIOx,  MX_SPI2_NSS_GPIO_Pin,  MX_SPI2_NSS_GPIO_AF};
#endif

#ifdef MX_SPI2_RX_DMA_Instance
  void SPI2_RX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi2_rx;
  static SPI_DMA SPI2_DMA_Rx = {
    &hdma_spi2_rx,
    SPI2_RX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI2_RX_DMA_Instance,
    MX_SPI2_RX_DMA_Channel,
    MX_SPI2_RX_DMA_Priority,
    MX_SPI2_RX_DMA_IRQn
#endif
  };
#endif 
#ifdef MX_SPI2_TX_DMA_Instance
  void SPI2_TX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi2_tx;
  static SPI_DMA SPI2_DMA_Tx = {
    &hdma_spi2_tx,
    SPI2_TX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI2_TX_DMA_Instance,
    MX_SPI2_TX_DMA_Channel,
    MX_SPI2_TX_DMA_Priority,
    MX_SPI2_TX_DMA_IRQn
#endif
  };
#endif

// SPI2 Resources
static const SPI_RESOURCES SPI2_Resources = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hspi2,
#endif
  SPI2,
  HAL_RCC_GetPCLK2Freq,

  // PINS
  {
#ifdef MX_SPI2_MOSI_Pin
    &SPI2_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI2_MISO_Pin
    &SPI2_miso,
#else
    NULL,
#endif
#ifdef MX_SPI2_SCK_Pin
    &SPI2_sck,
#else
    NULL,
#endif
#ifdef MX_SPI2_NSS_Pin 
    &SPI2_nss,
#else
    NULL,
#endif
  },

  SPI2_IRQn,

#ifdef MX_SPI2_RX_DMA_Instance
  &SPI2_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI2_TX_DMA_Instance
  &SPI2_DMA_Tx,
#else
  NULL,
#endif

  &SPI2_Info,
  &SPI2_TransfefInfo
};
#endif /* MX_SPI2 */

#ifdef MX_SPI3

#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
extern SPI_HandleTypeDef hspi3;
#endif

// SPI3 Run-Time Information
static SPI_INFO          SPI3_Info = {0};
static SPI_TRANSFER_INFO SPI3_TransfefInfo = {0};


#ifdef MX_SPI3_MOSI_Pin
  static SPI_PIN SPI3_mosi = {MX_SPI3_MOSI_GPIOx, MX_SPI3_MOSI_GPIO_Pin, MX_SPI3_MOSI_GPIO_AF};
#endif
#ifdef MX_SPI3_MISO_Pin
  static SPI_PIN SPI3_miso = {MX_SPI3_MISO_GPIOx, MX_SPI3_MISO_GPIO_Pin, MX_SPI3_MISO_GPIO_AF};
#endif
#ifdef MX_SPI3_SCK_Pin
  static SPI_PIN SPI3_sck  = {MX_SPI3_SCK_GPIOx,  MX_SPI3_SCK_GPIO_Pin,  MX_SPI3_SCK_GPIO_AF};
#endif
#ifdef MX_SPI3_NSS_Pin
  static SPI_PIN SPI3_nss  = {MX_SPI3_NSS_GPIOx,  MX_SPI3_NSS_GPIO_Pin,  MX_SPI3_NSS_GPIO_AF};
#endif

#ifdef MX_SPI3_RX_DMA_Instance
  void SPI3_RX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi3_rx;
  static SPI_DMA SPI3_DMA_Rx = {
    &hdma_spi3_rx,
    SPI3_RX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI3_RX_DMA_Instance,
    MX_SPI3_RX_DMA_Channel,
    MX_SPI3_RX_DMA_Priority,
    MX_SPI3_RX_DMA_IRQn
#endif
  };
#endif 
#ifdef MX_SPI3_TX_DMA_Instance
  void SPI3_TX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi3_tx;
  static SPI_DMA SPI3_DMA_Tx = {
    &hdma_spi3_tx,
    SPI3_TX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI3_TX_DMA_Instance,
    MX_SPI3_TX_DMA_Channel,
    MX_SPI3_TX_DMA_Priority,
    MX_SPI3_TX_DMA_IRQn
#endif
  };
#endif

// SPI3 Resources
static const SPI_RESOURCES SPI3_Resources = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hspi3,
#endif
  SPI3,
  HAL_RCC_GetPCLK2Freq,

  // PINS
  {
#ifdef MX_SPI3_MOSI_Pin
    &SPI3_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI3_MISO_Pin
    &SPI3_miso,
#else
    NULL,
#endif
#ifdef MX_SPI3_SCK_Pin
    &SPI3_sck,
#else
    NULL,
#endif
#ifdef MX_SPI3_NSS_Pin 
    &SPI3_nss,
#else
    NULL,
#endif
  },

  SPI3_IRQn,

#ifdef MX_SPI3_RX_DMA_Instance
  &SPI3_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI3_TX_DMA_Instance
  &SPI3_DMA_Tx,
#else
  NULL,
#endif

  &SPI3_Info,
  &SPI3_TransfefInfo
};
#endif /* MX_SPI3 */

#ifdef MX_SPI4

#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
extern SPI_HandleTypeDef hspi4;
#endif

// SPI4 Run-Time Information
static SPI_INFO          SPI4_Info = {0};
static SPI_TRANSFER_INFO SPI4_TransfefInfo = {0};


#ifdef MX_SPI4_MOSI_Pin
  static SPI_PIN SPI4_mosi = {MX_SPI4_MOSI_GPIOx, MX_SPI4_MOSI_GPIO_Pin, MX_SPI4_MOSI_GPIO_AF};
#endif
#ifdef MX_SPI4_MISO_Pin
  static SPI_PIN SPI4_miso = {MX_SPI4_MISO_GPIOx, MX_SPI4_MISO_GPIO_Pin, MX_SPI4_MISO_GPIO_AF};
#endif
#ifdef MX_SPI4_SCK_Pin
  static SPI_PIN SPI4_sck  = {MX_SPI4_SCK_GPIOx,  MX_SPI4_SCK_GPIO_Pin,  MX_SPI4_SCK_GPIO_AF};
#endif
#ifdef MX_SPI4_NSS_Pin
  static SPI_PIN SPI4_nss  = {MX_SPI4_NSS_GPIOx,  MX_SPI4_NSS_GPIO_Pin,  MX_SPI4_NSS_GPIO_AF};
#endif

#ifdef MX_SPI4_RX_DMA_Instance
  void SPI4_RX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi4_rx;
  static SPI_DMA SPI4_DMA_Rx = {
    &hdma_spi4_rx,
    SPI4_RX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI4_RX_DMA_Instance,
    MX_SPI4_RX_DMA_Channel,
    MX_SPI4_RX_DMA_Priority,
    MX_SPI4_RX_DMA_IRQn
#endif
  };
#endif 
#ifdef MX_SPI4_TX_DMA_Instance
  void SPI4_TX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi4_tx;
  static SPI_DMA SPI4_DMA_Tx = {
    &hdma_spi4_tx,
    SPI4_TX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI4_TX_DMA_Instance,
    MX_SPI4_TX_DMA_Channel,
    MX_SPI4_TX_DMA_Priority,
    MX_SPI4_TX_DMA_IRQn
#endif
  };
#endif

// SPI4 Resources
static const SPI_RESOURCES SPI4_Resources = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hspi4,
#endif
  SPI4,
  HAL_RCC_GetPCLK2Freq,

  // PINS
  {
#ifdef MX_SPI4_MOSI_Pin
    &SPI4_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI4_MISO_Pin
    &SPI4_miso,
#else
    NULL,
#endif
#ifdef MX_SPI4_SCK_Pin
    &SPI4_sck,
#else
    NULL,
#endif
#ifdef MX_SPI4_NSS_Pin 
    &SPI4_nss,
#else
    NULL,
#endif
  },

  SPI4_IRQn,

#ifdef MX_SPI4_RX_DMA_Instance
  &SPI4_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI4_TX_DMA_Instance
  &SPI4_DMA_Tx,
#else
  NULL,
#endif

  &SPI4_Info,
  &SPI4_TransfefInfo
};
#endif /* MX_SPI4 */

#ifdef MX_SPI5

#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
extern SPI_HandleTypeDef hspi5;
#endif

// SPI5 Run-Time Information
static SPI_INFO          SPI5_Info = {0};
static SPI_TRANSFER_INFO SPI5_TransfefInfo = {0};


#ifdef MX_SPI5_MOSI_Pin
  static SPI_PIN SPI5_mosi = {MX_SPI5_MOSI_GPIOx, MX_SPI5_MOSI_GPIO_Pin, MX_SPI5_MOSI_GPIO_AF};
#endif
#ifdef MX_SPI5_MISO_Pin
  static SPI_PIN SPI5_miso = {MX_SPI5_MISO_GPIOx, MX_SPI5_MISO_GPIO_Pin, MX_SPI5_MISO_GPIO_AF};
#endif
#ifdef MX_SPI5_SCK_Pin
  static SPI_PIN SPI5_sck  = {MX_SPI5_SCK_GPIOx,  MX_SPI5_SCK_GPIO_Pin,  MX_SPI5_SCK_GPIO_AF};
#endif
#ifdef MX_SPI5_NSS_Pin
  static SPI_PIN SPI5_nss  = {MX_SPI5_NSS_GPIOx,  MX_SPI5_NSS_GPIO_Pin,  MX_SPI5_NSS_GPIO_AF};
#endif

#ifdef MX_SPI5_RX_DMA_Instance
  void SPI5_RX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi5_rx;
  static SPI_DMA SPI5_DMA_Rx = {
    &hdma_spi5_rx,
    SPI5_RX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI5_RX_DMA_Instance,
    MX_SPI5_RX_DMA_Channel,
    MX_SPI5_RX_DMA_Priority,
    MX_SPI5_RX_DMA_IRQn
#endif
  };
#endif 
#ifdef MX_SPI5_TX_DMA_Instance
  void SPI5_TX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi5_tx;
  static SPI_DMA SPI5_DMA_Tx = {
    &hdma_spi5_tx,
    SPI5_TX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI5_TX_DMA_Instance,
    MX_SPI5_TX_DMA_Channel,
    MX_SPI5_TX_DMA_Priority,
    MX_SPI5_TX_DMA_IRQn
#endif
  };
#endif

// SPI5 Resources
static const SPI_RESOURCES SPI5_Resources = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hspi5,
#endif
  SPI5,
  HAL_RCC_GetPCLK2Freq,

  // PINS
  {
#ifdef MX_SPI5_MOSI_Pin
    &SPI5_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI5_MISO_Pin
    &SPI5_miso,
#else
    NULL,
#endif
#ifdef MX_SPI5_SCK_Pin
    &SPI5_sck,
#else
    NULL,
#endif
#ifdef MX_SPI5_NSS_Pin 
    &SPI5_nss,
#else
    NULL,
#endif
  },

  SPI5_IRQn,

#ifdef MX_SPI5_RX_DMA_Instance
  &SPI5_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI5_TX_DMA_Instance
  &SPI5_DMA_Tx,
#else
  NULL,
#endif

  &SPI5_Info,
  &SPI5_TransfefInfo
};
#endif /* MX_SPI5 */

#ifdef MX_SPI6

#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
extern SPI_HandleTypeDef hspi6;
#endif

// SPI6 Run-Time Information
static SPI_INFO          SPI6_Info = {0};
static SPI_TRANSFER_INFO SPI6_TransfefInfo = {0};


#ifdef MX_SPI6_MOSI_Pin
  static SPI_PIN SPI6_mosi = {MX_SPI6_MOSI_GPIOx, MX_SPI6_MOSI_GPIO_Pin, MX_SPI6_MOSI_GPIO_AF};
#endif
#ifdef MX_SPI6_MISO_Pin
  static SPI_PIN SPI6_miso = {MX_SPI6_MISO_GPIOx, MX_SPI6_MISO_GPIO_Pin, MX_SPI6_MISO_GPIO_AF};
#endif
#ifdef MX_SPI6_SCK_Pin
  static SPI_PIN SPI6_sck  = {MX_SPI6_SCK_GPIOx,  MX_SPI6_SCK_GPIO_Pin,  MX_SPI6_SCK_GPIO_AF};
#endif
#ifdef MX_SPI6_NSS_Pin
  static SPI_PIN SPI6_nss  = {MX_SPI6_NSS_GPIOx,  MX_SPI6_NSS_GPIO_Pin,  MX_SPI6_NSS_GPIO_AF};
#endif

#ifdef MX_SPI6_RX_DMA_Instance
  void SPI6_RX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi6_rx;
  static SPI_DMA SPI6_DMA_Rx = {
    &hdma_spi6_rx,
    SPI6_RX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI6_RX_DMA_Instance,
    MX_SPI6_RX_DMA_Channel,
    MX_SPI6_RX_DMA_Priority,
    MX_SPI6_RX_DMA_IRQn
#endif
  };
#endif
#ifdef MX_SPI6_TX_DMA_Instance
  void SPI6_TX_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static
#else
  extern
#endif
  DMA_HandleTypeDef hdma_spi6_tx;
  static SPI_DMA SPI6_DMA_Tx = {
    &hdma_spi6_tx,
    SPI6_TX_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SPI6_TX_DMA_Instance,
    MX_SPI6_TX_DMA_Channel,
    MX_SPI6_TX_DMA_Priority,
    MX_SPI6_TX_DMA_IRQn
#endif
  };
#endif

// SPI6 Resources
static const SPI_RESOURCES SPI6_Resources = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hspi6,
#endif
  SPI6,
  HAL_RCC_GetPCLK2Freq,

  // PINS
  {
#ifdef MX_SPI6_MOSI_Pin
    &SPI6_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI6_MISO_Pin
    &SPI6_miso,
#else
    NULL,
#endif
#ifdef MX_SPI6_SCK_Pin
    &SPI6_sck,
#else
    NULL,
#endif
#ifdef MX_SPI6_NSS_Pin 
    &SPI6_nss,
#else
    NULL,
#endif
  },

  SPI6_IRQn,

#ifdef MX_SPI6_RX_DMA_Instance
  &SPI6_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI6_TX_DMA_Instance
  &SPI6_DMA_Tx,
#else
  NULL,
#endif

  &SPI6_Info,
  &SPI6_TransfefInfo
};
#endif /* MX_SPI6 */

// Function prototypes
void SPI_IRQHandler (const SPI_RESOURCES *spi);
#ifdef __SPI_DMA_TX
void SPI_TX_DMA_Complete(const SPI_RESOURCES *spi);
#endif
#ifdef __SPI_DMA_RX
void SPI_RX_DMA_Complete(const SPI_RESOURCES *spi);
#endif

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
/**
  \fn          void Enable_GPIO_Clock (GPIO_TypeDef *port)
  \brief       Enable GPIO clock
*/
static void Enable_GPIO_Clock (GPIO_TypeDef *GPIOx) {
#ifdef GPIOA
  if (GPIOx == GPIOA) __GPIOA_CLK_ENABLE();
#endif 
#ifdef GPIOB
  if (GPIOx == GPIOB) __GPIOB_CLK_ENABLE();
#endif
#ifdef GPIOC
  if (GPIOx == GPIOC) __GPIOC_CLK_ENABLE();
#endif
#ifdef GPIOD
  if (GPIOx == GPIOD) __GPIOD_CLK_ENABLE();
#endif
#ifdef GPIOE
  if (GPIOx == GPIOE) __GPIOE_CLK_ENABLE();
#endif
#ifdef GPIOF
  if (GPIOx == GPIOF) __GPIOF_CLK_ENABLE();
#endif
#ifdef GPIOG
  if (GPIOx == GPIOG) __GPIOG_CLK_ENABLE();
#endif
#ifdef GPIOH
  if (GPIOx == GPIOH) __GPIOH_CLK_ENABLE();
#endif
#ifdef GPIOI
  if (GPIOx == GPIOI) __GPIOI_CLK_ENABLE();
#endif
}
#endif

/**
  \fn          ARM_DRIVER_VERSION SPIX_GetVersion (void)
  \brief       Get SPI driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION SPIX_GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_SPI_CAPABILITIES SPI_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_SPI_CAPABILITIES
*/
static ARM_SPI_CAPABILITIES SPIX_GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          int32_t SPI_Initialize (ARM_SPI_SignalEvent_t cb_event, const SPI_RESOURCES *spi)
  \brief       Initialize SPI Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SPI_SignalEvent
  \param[in]   spi       Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Initialize (ARM_SPI_SignalEvent_t cb_event, const SPI_RESOURCES *spi) {
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  GPIO_InitTypeDef GPIO_InitStruct;
#endif

  if (spi->info->state & SPI_INITIALIZED) { return ARM_DRIVER_OK; }
  if (spi->info->state & SPI_POWERED)     { return ARM_DRIVER_ERROR; }

  // Initialize SPI Run-Time Resources
  spi->info->cb_event = cb_event;
  spi->info->status.busy       = 0U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Clear transfer information
  memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));
  
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  // Configure SPI SCK pin
  Enable_GPIO_Clock (spi->io.sck->port);
  GPIO_InitStruct.Pin       = spi->io.sck->pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = spi->io.sck->af;
  HAL_GPIO_Init(spi->io.sck->port, &GPIO_InitStruct);

  // Configure SPI MOSI pin
  Enable_GPIO_Clock (spi->io.mosi->port);
  GPIO_InitStruct.Pin       = spi->io.mosi->pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = spi->io.mosi->af;
  HAL_GPIO_Init(spi->io.mosi->port, &GPIO_InitStruct);

  // Configure SPI MISO pin
  Enable_GPIO_Clock (spi->io.miso->port);
  GPIO_InitStruct.Pin       = spi->io.miso->pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = spi->io.miso->af;
  HAL_GPIO_Init(spi->io.miso->port, &GPIO_InitStruct);

#ifdef __SPI_DMA
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {

    if (spi->rx_dma != NULL) {
      // Initialize SPI RX DMA Resources
      spi->rx_dma->hdma->Instance             = spi->rx_dma->stream;
      spi->rx_dma->hdma->Init.Channel         = spi->rx_dma->channel;
      spi->rx_dma->hdma->Init.Direction       = DMA_PERIPH_TO_MEMORY;
      spi->rx_dma->hdma->Init.Mode            = DMA_NORMAL;
      spi->rx_dma->hdma->Init.Priority        = spi->rx_dma->priority;
      spi->rx_dma->hdma->Init.FIFOMode        = DMA_FIFOMODE_DISABLE;
      spi->rx_dma->hdma->Init.FIFOThreshold   = DMA_FIFO_THRESHOLD_FULL;
      spi->rx_dma->hdma->Init.MemBurst        = DMA_MBURST_SINGLE;
      spi->rx_dma->hdma->Init.PeriphBurst     = DMA_PBURST_SINGLE;
      spi->rx_dma->hdma->Parent               = NULL;
      spi->rx_dma->hdma->XferCpltCallback     = spi->rx_dma->cb_complete;
      spi->rx_dma->hdma->XferHalfCpltCallback = NULL;
      spi->rx_dma->hdma->XferM1CpltCallback   = NULL;
      spi->rx_dma->hdma->XferErrorCallback    = NULL;

      // Enable DMA IRQ in NVIC
      HAL_NVIC_EnableIRQ (spi->rx_dma->irq_num);
    }

    if (spi->tx_dma != NULL) {
      // Initialize SPI TX DMA Resources
      spi->tx_dma->hdma->Instance             = spi->tx_dma->stream;
      spi->tx_dma->hdma->Init.Channel         = spi->tx_dma->channel;
      spi->tx_dma->hdma->Init.Direction       = DMA_MEMORY_TO_PERIPH;
      spi->tx_dma->hdma->Init.Mode            = DMA_NORMAL;
      spi->tx_dma->hdma->Init.Priority        = spi->tx_dma->priority;
      spi->tx_dma->hdma->Init.FIFOMode        = DMA_FIFOMODE_DISABLE;
      spi->tx_dma->hdma->Init.FIFOThreshold   = DMA_FIFO_THRESHOLD_FULL;
      spi->tx_dma->hdma->Init.MemBurst        = DMA_MBURST_SINGLE;
      spi->tx_dma->hdma->Init.PeriphBurst     = DMA_PBURST_SINGLE;
      spi->tx_dma->hdma->Parent               = NULL;
      spi->tx_dma->hdma->XferCpltCallback     = spi->tx_dma->cb_complete;
      spi->tx_dma->hdma->XferHalfCpltCallback = NULL;
      spi->tx_dma->hdma->XferM1CpltCallback   = NULL;
      spi->tx_dma->hdma->XferErrorCallback    = NULL;

      // Enable DMA IRQ in NVIC
      HAL_NVIC_EnableIRQ (spi->tx_dma->irq_num);
    }

    // Enable DMA clock
    if ((spi->reg == SPI2) || (spi->reg == SPI3)) {
      // DMA1 used for SPI2 and SPI3
      __DMA1_CLK_ENABLE();
    } else {
      // DMA2 used for SPI1, SPI4, SPI5 and SPI6
      __DMA2_CLK_ENABLE();
    }
  }
#endif
#else
  spi->h->Instance = spi->reg;
  HAL_SPI_MspInit (spi->h);

  if (spi->rx_dma != NULL) {
    spi->rx_dma->hdma->XferCpltCallback = spi->rx_dma->cb_complete;
  }
  if (spi->tx_dma != NULL) {
    spi->tx_dma->hdma->XferCpltCallback = spi->tx_dma->cb_complete;
  }
#endif
  

  spi->info->state = SPI_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Uninitialize (const SPI_RESOURCES *spi)
  \brief       De-initialize SPI Interface.
  \param[in]   spi  Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Uninitialize (const SPI_RESOURCES *spi) {

  if ((spi->info->state & SPI_INITIALIZED) == 0U) { return ARM_DRIVER_OK; }
  if ( spi->info->state & SPI_POWERED)            { return ARM_DRIVER_ERROR; }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  // Unconfigure SPI pins
  if (spi->io.mosi != NULL) { HAL_GPIO_DeInit(spi->io.mosi->port, spi->io.mosi->pin); }
  if (spi->io.miso != NULL) { HAL_GPIO_DeInit(spi->io.miso->port, spi->io.miso->pin); }
  if (spi->io.sck  != NULL) { HAL_GPIO_DeInit(spi->io.sck->port,  spi->io.sck->pin); }
  if (spi->io.nss  != NULL) { HAL_GPIO_DeInit(spi->io.nss->port,  spi->io.nss->pin); }

#ifdef __SPI_DMA
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    if (spi->rx_dma != NULL) {
      // Disable DMA IRQ in NVIC
      HAL_NVIC_DisableIRQ (spi->rx_dma->irq_num);
      // Deinitialize DMA
      HAL_DMA_DeInit (spi->rx_dma->hdma);
    }

    if (spi->tx_dma != NULL) {
      // Disable DMA IRQ in NVIC
      HAL_NVIC_DisableIRQ (spi->tx_dma->irq_num);
      // Deinitialize DMA
      HAL_DMA_DeInit (spi->tx_dma->hdma);
    }
  }
#endif
#else
  HAL_SPI_MspDeInit (spi->h);
#endif

  // Clear SPI state
  spi->info->state = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_PowerControl (ARM_POWER_STATE state, const SPI_RESOURCES *spi)
  \brief       Control SPI Interface Power.
  \param[in]   state  Power state
  \param[in]   spi    Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_PowerControl (ARM_POWER_STATE state, const SPI_RESOURCES *spi) {

  if ((spi->info->state & SPI_INITIALIZED) == 0U) { return ARM_DRIVER_ERROR; }
  if ( spi->info->status.busy)                    { return ARM_DRIVER_ERROR_BUSY; }

  switch (state) {
    case ARM_POWER_OFF:
      // Check if SPI is already powered off
      if ((spi->info->state & SPI_POWERED) == 0U) { return ARM_DRIVER_OK; }

      // Disable SPI Interrupts
      spi->reg->CR2 = 0U;

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
      HAL_NVIC_DisableIRQ (spi->irq_num);
#endif

      // Disable SPI peripheral
      spi->reg->CR1 = 0U;

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
      // Disable SPI clock
      __SPIx_CLK_DISABLE(spi->reg);
#endif

      // Clear powered flag
      spi->info->state = SPI_INITIALIZED;
      break;

    case ARM_POWER_FULL:
      // Check if SPI already powered
      if (spi->info->state & SPI_POWERED) { return ARM_DRIVER_OK; }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
      // Enable SPI clock
      __SPIx_CLK_ENABLE(spi->reg);
#endif

      // Disable SPI interrupts
      spi->reg->CR2 = 0U;

      // Ready for operation - set powered flag
      spi->info->state |= SPI_POWERED;

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
      HAL_NVIC_ClearPendingIRQ (spi->irq_num);
      HAL_NVIC_EnableIRQ (spi->irq_num);
#endif
      break;

    case ARM_POWER_LOW:
    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Send (const void *data, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start sending data to SPI transmitter.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter
  \param[in]   num   Number of data items to send
  \param[in]   spi   Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Send (const void *data, uint32_t num, const SPI_RESOURCES *spi) {

  if ((data == NULL) || (num == 0U))             { return ARM_DRIVER_ERROR_PARAMETER; }
  if ((spi->info->state & SPI_CONFIGURED) == 0U) { return ARM_DRIVER_ERROR; }
  if ( spi->info->status.busy)                   { return ARM_DRIVER_ERROR_BUSY; }

  // Check if transmit pin available
  if ((((spi->io.mosi != NULL) && ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_MASTER)) ||
       ((spi->io.miso != NULL) && ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_SLAVE ))) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  // Update SPI statuses
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Save transfer info
  spi->xfer->rx_buf = NULL;
  spi->xfer->tx_buf = (uint8_t *)data;
  spi->xfer->num    = num;
  spi->xfer->rx_cnt = 0U;
  spi->xfer->tx_cnt = 0U;

#ifdef __SPI_DMA_RX
  if (spi->rx_dma != NULL) {
    // DMA mode
    // Prepare DMA to receive dummy RX data
    spi->rx_dma->hdma->Init.PeriphInc             = DMA_PINC_DISABLE;
    spi->rx_dma->hdma->Init.MemInc                = DMA_MINC_DISABLE; 
    if (spi->reg->CR1 & SPI_CR1_DFF) {
      // 16 - bit data frame
      spi->rx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      spi->rx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
    } else {
      //  8 - bit data frame
      spi->rx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      spi->rx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_BYTE;
    }
    // Initialize and start SPI RX DMA Stream
    if (HAL_DMA_Init     (spi->rx_dma->hdma) != HAL_OK) { return ARM_DRIVER_ERROR; }
    if (HAL_DMA_Start_IT (spi->rx_dma->hdma, (uint32_t)(&spi->reg->DR), (uint32_t)(&spi->xfer->dump_val), num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }

    // RX Buffer DMA enable
    spi->reg->CR2 |= SPI_CR2_RXDMAEN;
  } else
#endif
  {
    // Interrupt mode
    // RX Buffer not empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_RXNEIE;
  }

#ifdef __SPI_DMA_TX
  if (spi->tx_dma != NULL) {
    // Prepare DMA to send TX data
    spi->tx_dma->hdma->Init.PeriphInc             = DMA_PINC_DISABLE;
    spi->tx_dma->hdma->Init.MemInc                = DMA_MINC_ENABLE; 
    if (spi->reg->CR1 & SPI_CR1_DFF) {
      // 16 - bit data frame
      spi->tx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      spi->tx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
    } else {
      //  8 - bit data frame
      spi->tx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      spi->tx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_BYTE;
    }

    // Initialize and start SPI TX DMA Stream
    if (HAL_DMA_Init     (spi->tx_dma->hdma) != HAL_OK) { return ARM_DRIVER_ERROR; }
    if (HAL_DMA_Start_IT (spi->tx_dma->hdma, (uint32_t)spi->xfer->tx_buf, (uint32_t)(&spi->reg->DR), num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }

    // TX Buffer DMA enable
    spi->reg->CR2 |= SPI_CR2_TXDMAEN;
  } else
#endif
  {
    // Interrupt mode
    // TX Buffer empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_TXEIE;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Receive (void *data, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start receiving data from SPI receiver.
  \param[out]  data  Pointer to buffer for data to receive from SPI receiver
  \param[in]   num   Number of data items to receive
  \param[in]   spi   Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Receive (void *data, uint32_t num, const SPI_RESOURCES *spi) {

  if ((data == NULL) || (num == 0U))             { return ARM_DRIVER_ERROR_PARAMETER; }
  if ((spi->info->state & SPI_CONFIGURED) == 0U) { return ARM_DRIVER_ERROR; }
  if ( spi->info->status.busy)                   { return ARM_DRIVER_ERROR_BUSY; }

  // Check if receive pin available
  if ((((spi->io.miso != NULL) && ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_MASTER)) ||
       ((spi->io.mosi != NULL) && ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_SLAVE ))) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  // Update SPI statuses
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Save transfer info
  spi->xfer->rx_buf = (uint8_t *)data;
  spi->xfer->tx_buf = NULL;
  spi->xfer->num    = num;
  spi->xfer->rx_cnt = 0U;
  spi->xfer->tx_cnt = 0U;

#ifdef __SPI_DMA_RX
  // DMA mode
  if (spi->rx_dma != NULL) {
    // Prepare DMA to receive RX data
    spi->rx_dma->hdma->Init.PeriphInc             = DMA_PINC_DISABLE;
    spi->rx_dma->hdma->Init.MemInc                = DMA_MINC_ENABLE; 
    if (spi->reg->CR1 & SPI_CR1_DFF) {
      // 16 - bit data frame
      spi->rx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      spi->rx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
    } else {
      //  8 - bit data frame
      spi->rx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      spi->rx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_BYTE;
    }
    // Initialize and start SPI RX DMA Stream
    if (HAL_DMA_Init     (spi->rx_dma->hdma) != HAL_OK) { return ARM_DRIVER_ERROR; }
    if (HAL_DMA_Start_IT (spi->rx_dma->hdma, (uint32_t)(&spi->reg->DR), (uint32_t)spi->xfer->rx_buf, num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }

    // RX Buffer DMA enable
    spi->reg->CR2 |= SPI_CR2_RXDMAEN;
  } else
#endif
  {
    // Interrupt mode
    // RX Buffer not empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_RXNEIE;
  }

#ifdef __SPI_DMA_TX
  // DMA mode
  if (spi->tx_dma != NULL) {
    // Prepare DMA to send dummy TX data
    spi->tx_dma->hdma->Init.PeriphInc             = DMA_PINC_DISABLE;
    spi->tx_dma->hdma->Init.MemInc                = DMA_MINC_DISABLE; 
    if (spi->reg->CR1 & SPI_CR1_DFF) {
      // 16 - bit data frame
      spi->tx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      spi->tx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
    } else {
      //  8 - bit data frame
      spi->tx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      spi->tx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_BYTE;
    }

    // Initialize and start SPI TX DMA Stream
    if (HAL_DMA_Init     (spi->tx_dma->hdma) != HAL_OK) { return ARM_DRIVER_ERROR; }
    if (HAL_DMA_Start_IT (spi->tx_dma->hdma, (uint32_t)&spi->xfer->def_val, (uint32_t)(&spi->reg->DR), num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }

    // TX Buffer DMA enable
    spi->reg->CR2 |= SPI_CR2_TXDMAEN;
  } else
#endif
  {
    // Interrupt mode
    // TX buffer empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_TXEIE;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Transfer (const void *data_out, void *data_in, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start sending/receiving data to/from SPI transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   spi       Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Transfer (const void *data_out, void *data_in, uint32_t num, const SPI_RESOURCES *spi) {

  if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) { return ARM_DRIVER_ERROR_PARAMETER; }
  if ((spi->info->state & SPI_CONFIGURED) == 0U)              { return ARM_DRIVER_ERROR; }
  if ( spi->info->status.busy)                                { return ARM_DRIVER_ERROR_BUSY; }

  // Check if receive and transmit pins available
  if ((spi->io.miso == NULL) || (spi->io.mosi == NULL)) {
    return ARM_DRIVER_ERROR;
  }

  // Update SPI statuses
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Save transfer info
  spi->xfer->rx_buf = (uint8_t *)data_in;
  spi->xfer->tx_buf = (uint8_t *)data_out;
  spi->xfer->num    = num;
  spi->xfer->rx_cnt = 0U;
  spi->xfer->tx_cnt = 0U;

#ifdef __SPI_DMA
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    // DMA mode

    if (spi->rx_dma != NULL) {
      // Prepare DMA to receive RX data
      spi->rx_dma->hdma->Init.PeriphInc             = DMA_PINC_DISABLE;
      spi->rx_dma->hdma->Init.MemInc                = DMA_MINC_ENABLE; 
      if (spi->reg->CR1 & SPI_CR1_DFF) {
        // 16 - bit data frame
        spi->rx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        spi->rx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
      } else {
       //  8 - bit data frame
         spi->rx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        spi->rx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_BYTE;
      }
      // Initialize and start SPI RX DMA Stream
      if (HAL_DMA_Init     (spi->rx_dma->hdma) != HAL_OK) { return ARM_DRIVER_ERROR; }
      if (HAL_DMA_Start_IT (spi->rx_dma->hdma, (uint32_t)(&spi->reg->DR), (uint32_t)spi->xfer->rx_buf, num) != HAL_OK) {
        return ARM_DRIVER_ERROR;
      }

      // RX Buffer DMA enable
      spi->reg->CR2 |= SPI_CR2_RXDMAEN;
    }

    if (spi->tx_dma != NULL) {
      // Prepare DMA to send TX data
      spi->tx_dma->hdma->Init.PeriphInc             = DMA_PINC_DISABLE;
      spi->tx_dma->hdma->Init.MemInc                = DMA_MINC_ENABLE;
      if (spi->reg->CR1 & SPI_CR1_DFF) {
        // 16 - bit data frame
        spi->tx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        spi->tx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
      } else {
        //  8 - bit data frame
        spi->tx_dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        spi->tx_dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_BYTE;
      }

      // Initialize and start SPI TX DMA Stream
      if (HAL_DMA_Init     (spi->tx_dma->hdma) != HAL_OK) { return ARM_DRIVER_ERROR; }
      if (HAL_DMA_Start_IT (spi->tx_dma->hdma, (uint32_t)spi->xfer->tx_buf, (uint32_t)(&spi->reg->DR), num) != HAL_OK) {
        return ARM_DRIVER_ERROR;
      }

      // TX Buffer DMA enable
      spi->reg->CR2 |= SPI_CR2_TXDMAEN;
    }
  } else
#endif
  {
    // Interrupt mode
    // TX Buffer empty and RX Buffer not empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_RXNEIE | SPI_CR2_TXEIE;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t SPI_GetDataCount (const SPI_RESOURCES *spi)
  \brief       Get transferred data count.
  \param[in]   spi  Pointer to SPI resources
  \return      number of data items transferred
*/
static uint32_t SPI_GetDataCount (const SPI_RESOURCES *spi) {
  return (spi->xfer->rx_cnt);
}

/**
  \fn          int32_t SPI_Control (uint32_t control, uint32_t arg, const SPI_RESOURCES *spi)
  \brief       Control SPI Interface.
  \param[in]   control  operation
  \param[in]   arg      argument of operation (optional)
  \param[in]   spi      Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Control (uint32_t control, uint32_t arg, const SPI_RESOURCES *spi) {
  uint32_t           mode, val, pclk;
  uint32_t           cr1, cr2;
  GPIO_InitTypeDef   GPIO_InitStruct;

  mode  = 0U;
  val   = 0U;
  cr1   = 0U;
  cr2   = 0U;

  if ((spi->info->state & SPI_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  if ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_ABORT_TRANSFER) {
    // Send abort
    if (spi->tx_dma != NULL) {
      // DMA mode
      // TX buffer DMA disable
      spi->reg->CR2 &= ~SPI_CR2_TXDMAEN;

      // Abort TX DMA transfer
      HAL_DMA_Abort (spi->tx_dma->hdma);
    } else {
      // Interrupt mode
      // Disable TX buffer empty interrupt
      spi->reg->CR2 &= ~SPI_CR2_TXEIE;
    }

    // Receive abort
    if (spi->rx_dma != NULL) {
      // DMA mode
      // RX buffer DMA disable
      spi->reg->CR2 &= ~SPI_CR2_RXDMAEN;

      // Abort RX DMA transfer
      HAL_DMA_Abort (spi->rx_dma->hdma);
    } else {
      // Interrupt mode
      // Disable RX buffer not empty interrupt
      spi->reg->CR2 &= ~SPI_CR2_RXNEIE;
    }

    memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));
    spi->info->status.busy = 0U;
    return ARM_DRIVER_OK;
  }

  // Check for busy flag
  if (spi->info->status.busy) { return ARM_DRIVER_ERROR_BUSY; }

  switch (control & ARM_SPI_CONTROL_Msk) {
    case ARM_SPI_MODE_INACTIVE:
      mode |= ARM_SPI_MODE_INACTIVE;
      break;

    case ARM_SPI_MODE_MASTER:
      mode |= ARM_SPI_MODE_MASTER;

      // Master enabled
      cr1 |= SPI_CR1_MSTR;
      break;

    case ARM_SPI_MODE_SLAVE:
      mode |= ARM_SPI_MODE_SLAVE;
      break;

    case ARM_SPI_MODE_MASTER_SIMPLEX:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_MODE_SLAVE_SIMPLEX:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_SET_BUS_SPEED:
      // Set SPI Bus Speed 
      pclk = spi->periph_clock();
      for (val = 1U; val < 8U; val++) {
        if (arg >= (pclk >> val)) { break; }
      }
      // Disable SPI, update prescaler and enable SPI
      spi->reg->CR1 &= ~SPI_CR1_SPE;
      spi->reg->CR1 |= (val - 1U) << 3U;
      spi->reg->CR1 |=  SPI_CR1_SPE;
      return ARM_DRIVER_OK;

    case ARM_SPI_GET_BUS_SPEED:
      // Return current bus speed
      return (spi->periph_clock() >> (((spi->reg->CR1 & SPI_CR1_BR) >> 3U) + 1U));

    case ARM_SPI_SET_DEFAULT_TX_VALUE:
      spi->xfer->def_val = (uint16_t)(arg & 0xFFFFU);
      return ARM_DRIVER_OK;

    case ARM_SPI_CONTROL_SS:
      val = (spi->info->mode & ARM_SPI_CONTROL_Msk); 
      // Master modes
      if (val == ARM_SPI_MODE_MASTER) {
        val = spi->info->mode & ARM_SPI_SS_MASTER_MODE_Msk;
        // Check if NSS pin is available and
        // software slave select master is selected
        if ((spi->io.nss != NULL) && (val == ARM_SPI_SS_MASTER_SW)) {
          // Set/Clear NSS pin
          if (arg == ARM_SPI_SS_INACTIVE)
            HAL_GPIO_WritePin (spi->io.nss->port, spi->io.nss->pin, GPIO_PIN_SET);
          else
            HAL_GPIO_WritePin (spi->io.nss->port, spi->io.nss->pin, GPIO_PIN_RESET);
        } else return ARM_DRIVER_ERROR;
        return ARM_DRIVER_OK;
      }
      // Slave modes
      else if (val == ARM_SPI_MODE_SLAVE) {
        val = spi->info->mode & ARM_SPI_SS_SLAVE_MODE_Msk;
        // Check if slave select slave mode is selected
        if (val == ARM_SPI_SS_MASTER_SW) {
          if (arg == ARM_SPI_SS_ACTIVE) {
            spi->reg->CR1 |= SPI_CR1_SSI;
          }
          else {
            spi->reg->CR1 &= ~SPI_CR1_SSI;
          }
          return ARM_DRIVER_OK;
        } else { return ARM_DRIVER_ERROR; }
      } else { return ARM_DRIVER_ERROR; }

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  // Frame format:
  switch (control & ARM_SPI_FRAME_FORMAT_Msk) {
    case ARM_SPI_CPOL0_CPHA0:
      break;
    case ARM_SPI_CPOL0_CPHA1:
     cr1 |= SPI_CR1_CPHA;
      break;
    case ARM_SPI_CPOL1_CPHA0:
      cr1 |= SPI_CR1_CPOL;
      break;
    case ARM_SPI_CPOL1_CPHA1:
      cr1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
      break;
    case ARM_SPI_TI_SSI:
      cr1 |= SPI_CR2_FRF;
      break;
    case ARM_SPI_MICROWIRE:
      return ARM_SPI_ERROR_FRAME_FORMAT;
    default: return ARM_SPI_ERROR_FRAME_FORMAT;
  }

  // Data Bits
  switch (control & ARM_SPI_DATA_BITS_Msk) {
    case ARM_SPI_DATA_BITS(8U):
      break;
    case ARM_SPI_DATA_BITS(16U):
      cr1 |= SPI_CR1_DFF;
      break;
    default: return ARM_SPI_ERROR_DATA_BITS;
  }

  // Bit order
  if ((control & ARM_SPI_BIT_ORDER_Msk) == ARM_SPI_LSB_MSB) {
    cr1 |= SPI_CR1_LSBFIRST;
  }

  // Slave select master modes
  if (mode == ARM_SPI_MODE_MASTER) {
    switch (control & ARM_SPI_SS_MASTER_MODE_Msk) {
      case ARM_SPI_SS_MASTER_UNUSED:
        if (spi->io.nss != NULL) {
          // Unconfigure NSS pin
          HAL_GPIO_DeInit (spi->io.nss->port, spi->io.nss->pin);
        }
        // Software slave management
        // Internal NSS always active, IO value is ignored
        cr1 |= SPI_CR1_SSM | SPI_CR1_SSI;
        mode |= ARM_SPI_SS_MASTER_UNUSED;
        break;

      case ARM_SPI_SS_MASTER_HW_INPUT:
        if (spi->io.nss) {
          // Configure NSS pin
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
          Enable_GPIO_Clock (spi->io.nss->port);
#endif
          GPIO_InitStruct.Pin       = spi->io.nss->pin;
          GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
          GPIO_InitStruct.Pull      = GPIO_NOPULL;
          GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
          GPIO_InitStruct.Alternate = spi->io.nss->af;
          HAL_GPIO_Init(spi->io.nss->port, &GPIO_InitStruct);
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        mode |= ARM_SPI_SS_MASTER_HW_INPUT;
        break;

      case ARM_SPI_SS_MASTER_SW:
        if (spi->io.nss) {
          // Configure NSS pin as GPIO output
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
          Enable_GPIO_Clock (spi->io.nss->port);
#endif
          GPIO_InitStruct.Pin       = spi->io.nss->pin;
          GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Pull      = GPIO_NOPULL;
          GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
          HAL_GPIO_Init(spi->io.nss->port, &GPIO_InitStruct);

          // Software slave management
          cr1 |= SPI_CR1_SSM | SPI_CR1_SSI;

          mode |= ARM_SPI_SS_MASTER_SW;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_MASTER_HW_OUTPUT:
        if (spi->io.nss) {
          // Configure NSS pin - SPI NSS alternative function
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
          Enable_GPIO_Clock (spi->io.nss->port);
#endif
          GPIO_InitStruct.Pin       = spi->io.nss->pin;
          GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
          GPIO_InitStruct.Pull      = GPIO_NOPULL;
          GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
          GPIO_InitStruct.Alternate = spi->io.nss->af;
          HAL_GPIO_Init(spi->io.nss->port, &GPIO_InitStruct);

          // Slave select output enable
          cr2 |= SPI_CR2_SSOE;

          mode |= ARM_SPI_SS_MASTER_HW_OUTPUT;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;
        default: return ARM_SPI_ERROR_SS_MODE;
    }
  }

  // Slave select slave modes
  if (mode ==  ARM_SPI_MODE_SLAVE) {
    switch (control & ARM_SPI_SS_SLAVE_MODE_Msk) {
      case ARM_SPI_SS_SLAVE_HW:
        if (spi->io.nss) {
          // Configure NSS pin - SPI NSS alternative function
          GPIO_InitStruct.Pin       = spi->io.nss->pin;
          GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
          GPIO_InitStruct.Pull      = GPIO_NOPULL;
          GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
          GPIO_InitStruct.Alternate = spi->io.nss->af;
          HAL_GPIO_Init(spi->io.nss->port, &GPIO_InitStruct);

          mode |= ARM_SPI_SS_SLAVE_HW;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_SLAVE_SW:
        if (spi->io.nss) {
          // Unconfigure NSS pin
          HAL_GPIO_DeInit (spi->io.nss->port, spi->io.nss->pin);
        }
        // Enable software slave management
        cr1 |= SPI_CR1_SSM;
        mode |= ARM_SPI_SS_SLAVE_SW;
        break;
      default: return ARM_SPI_ERROR_SS_MODE;
    }
  }

  // Set SPI Bus Speed 
  pclk = spi->periph_clock();
  for (val = 1U; val < 8U; val++) {
    if (arg >= (pclk >> val)) break;
  }
  // Save prescaler value
  cr1 |= (val - 1U) << 3U;

  spi->info->mode = mode;

  // Configure registers
  spi->reg->CR1 &= ~SPI_CR1_SPE;
  spi->reg->CR2  = cr2 | SPI_CR2_ERRIE;
  spi->reg->CR1  = cr1;

  if ((mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_INACTIVE) {
    spi->info->state &= ~SPI_CONFIGURED;
  } else {
    spi->info->state |=  SPI_CONFIGURED;
  }

  // Enable SPI
  spi->reg->CR1 |= SPI_CR1_SPE;

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_SPI_STATUS SPI_GetStatus (const SPI_RESOURCES *spi)
  \brief       Get SPI status.
  \param[in]   spi  Pointer to SPI resources
  \return      SPI status \ref ARM_SPI_STATUS
*/
static ARM_SPI_STATUS SPI_GetStatus (const SPI_RESOURCES *spi) {
  return (spi->info->status);
}

/*SPI IRQ Handler */
void SPI_IRQHandler (const SPI_RESOURCES *spi) {
  uint16_t data, sr;
  uint32_t event;

  // Save status register
  sr = spi->reg->SR;

  event = 0U;

  if ((sr & SPI_SR_OVR) != 0U) {
    // Overrrun flag is set
    spi->info->status.data_lost = 1U;
    event |=ARM_SPI_EVENT_DATA_LOST;
  }
  if ((sr & SPI_SR_UDR) != 0U) {
    // Underrun flag is set
    spi->info->status.data_lost = 1U;
    event |= ARM_SPI_EVENT_DATA_LOST;
  }
  if ((sr & SPI_SR_MODF) != 0U) {
    // Mode fault flag is set
    spi->info->status.mode_fault = 1U;
    event |= ARM_SPI_EVENT_MODE_FAULT;
  }

  if (((sr & SPI_SR_TXE) != 0U) && ((spi->reg->CR2 & SPI_CR2_TXEIE) != 0U)) {
    if (spi->xfer->num) {
      if (spi->xfer->tx_buf != NULL) {
        // Send buffered data
        data = *(spi->xfer->tx_buf++);
        if (spi->reg->CR1 & SPI_CR1_DFF) {
          // 16-bit data frame format
          data |= *(spi->xfer->tx_buf++) << 8U;
        }
      } else {
        // Send default transfer value
        data = spi->xfer->def_val;
      }

      // Send data
      spi->reg->DR = data;

      spi->xfer->tx_cnt++;

      if (spi->xfer->tx_cnt == spi->xfer->num) {
        // All data sent, disable TX Buffer Empty Interrupt
        spi->reg->CR2 &= ~SPI_CR2_TXEIE;
      }
    } else {
      // Unexpected transfer, data lost
      event |= ARM_SPI_EVENT_DATA_LOST;
    }
  }

  if (((sr & SPI_SR_RXNE) != 0U) && ((spi->reg->CR2 & SPI_CR2_RXNEIE) != 0U)) {
    // Receive Buffer Not Empty
    data = spi->reg->DR;

    if (spi->xfer->num != 0U) {
      if ((spi->xfer->rx_buf) != NULL) {
        // Put data into buffer
        *(spi->xfer->rx_buf++) = (uint8_t)data;
        if (spi->reg->CR1 & SPI_CR1_DFF) {
          *(spi->xfer->rx_buf++) = (uint8_t)(data >> 8U);
        }
      }
      spi->xfer->rx_cnt++;

      if (spi->xfer->rx_cnt == spi->xfer->num) {

        // Disable RX Buffer Not Empty Interrupt
        spi->reg->CR2 &= ~SPI_CR2_RXNEIE;
        HAL_NVIC_ClearPendingIRQ (spi->irq_num);

        spi->xfer->num = 0U;
        // Clear busy flag
        spi->info->status.busy = 0U;

        // Transfer completed
        event |= ARM_SPI_EVENT_TRANSFER_COMPLETE;
      }
    }
    else {
      // Unexpected transfer, data lost
      event |= ARM_SPI_EVENT_DATA_LOST;
    }
  }

  // Send event
  if ((event != 0U) && ((spi->info->cb_event != NULL))) {
    spi->info->cb_event(event);
  }
}

#ifdef __SPI_DMA_TX
void SPI_TX_DMA_Complete(const SPI_RESOURCES *spi) {

  if ((__HAL_DMA_GET_COUNTER(spi->tx_dma->hdma) != 0) && (spi->xfer->num != 0)) {
    // TX DMA Complete caused by transfer abort
    return;
  }

  spi->xfer->tx_cnt = spi->xfer->num;
}
#endif

#ifdef __SPI_DMA_RX
void SPI_RX_DMA_Complete(const SPI_RESOURCES *spi) {

  if ((__HAL_DMA_GET_COUNTER(spi->rx_dma->hdma) != 0) && (spi->xfer->num != 0)) {
    // RX DMA Complete caused by transfer abort
    return;
  }

  spi->xfer->tx_cnt = spi->xfer->num;

  spi->info->status.busy = 0U;
  if (spi->info->cb_event != NULL) {
    spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
  }
}
#endif

// SPI1
#ifdef MX_SPI1
static int32_t        SPI1_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI1_Resources); }
static int32_t        SPI1_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI1_Resources); }
static int32_t        SPI1_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI1_Resources); }
static int32_t        SPI1_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI1_Resources); }
static int32_t        SPI1_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI1_Resources); }
static int32_t        SPI1_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI1_Resources); }
static uint32_t       SPI1_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI1_Resources); }
static int32_t        SPI1_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI1_Resources); }
static ARM_SPI_STATUS SPI1_GetStatus           (void)                                              { return SPI_GetStatus (&SPI1_Resources); }
       void           SPI1_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI1_Resources); }

#ifdef MX_SPI1_TX_DMA_Instance
      void            SPI1_TX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_TX_DMA_Complete (&SPI1_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI1_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI1_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
}
#endif
#endif
#ifdef MX_SPI1_RX_DMA_Instance
      void            SPI1_RX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_RX_DMA_Complete(&SPI1_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI1_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI1_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
}
#endif
#endif

ARM_DRIVER_SPI Driver_SPI1 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI1_Initialize,
  SPI1_Uninitialize,
  SPI1_PowerControl,
  SPI1_Send,
  SPI1_Receive,
  SPI1_Transfer,
  SPI1_GetDataCount,
  SPI1_Control,
  SPI1_GetStatus
};
#endif

// SPI2
#ifdef MX_SPI2
static int32_t        SPI2_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI2_Resources); }
static int32_t        SPI2_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI2_Resources); }
static int32_t        SPI2_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI2_Resources); }
static int32_t        SPI2_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI2_Resources); }
static int32_t        SPI2_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI2_Resources); }
static int32_t        SPI2_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI2_Resources); }
static uint32_t       SPI2_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI2_Resources); }
static int32_t        SPI2_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI2_Resources); }
static ARM_SPI_STATUS SPI2_GetStatus           (void)                                              { return SPI_GetStatus (&SPI2_Resources); }
       void           SPI2_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI2_Resources); }

#ifdef MX_SPI2_TX_DMA_Instance
      void            SPI2_TX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_TX_DMA_Complete (&SPI2_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI2_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI2_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
}
#endif
#endif
#ifdef MX_SPI2_RX_DMA_Instance
      void            SPI2_RX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_RX_DMA_Complete(&SPI2_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI2_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI2_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi2_rx);
}
#endif
#endif

ARM_DRIVER_SPI Driver_SPI2 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI2_Initialize,
  SPI2_Uninitialize,
  SPI2_PowerControl,
  SPI2_Send,
  SPI2_Receive,
  SPI2_Transfer,
  SPI2_GetDataCount,
  SPI2_Control,
  SPI2_GetStatus
};
#endif

// SPI3
#ifdef MX_SPI3
static int32_t        SPI3_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI3_Resources); }
static int32_t        SPI3_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI3_Resources); }
static int32_t        SPI3_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI3_Resources); }
static int32_t        SPI3_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI3_Resources); }
static int32_t        SPI3_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI3_Resources); }
static int32_t        SPI3_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI3_Resources); }
static uint32_t       SPI3_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI3_Resources); }
static int32_t        SPI3_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI3_Resources); }
static ARM_SPI_STATUS SPI3_GetStatus           (void)                                              { return SPI_GetStatus (&SPI3_Resources); }
       void           SPI3_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI3_Resources); }

#ifdef MX_SPI3_TX_DMA_Instance
      void            SPI3_TX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_TX_DMA_Complete (&SPI3_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI3_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI3_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi3_tx);
}
#endif
#endif
#ifdef MX_SPI3_RX_DMA_Instance
      void            SPI3_RX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_RX_DMA_Complete(&SPI3_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI3_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI3_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi3_rx);
}
#endif
#endif

ARM_DRIVER_SPI Driver_SPI3 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI3_Initialize,
  SPI3_Uninitialize,
  SPI3_PowerControl,
  SPI3_Send,
  SPI3_Receive,
  SPI3_Transfer,
  SPI3_GetDataCount,
  SPI3_Control,
  SPI3_GetStatus
};
#endif

// SPI4
#ifdef MX_SPI4
static int32_t        SPI4_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI4_Resources); }
static int32_t        SPI4_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI4_Resources); }
static int32_t        SPI4_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI4_Resources); }
static int32_t        SPI4_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI4_Resources); }
static int32_t        SPI4_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI4_Resources); }
static int32_t        SPI4_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI4_Resources); }
static uint32_t       SPI4_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI4_Resources); }
static int32_t        SPI4_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI4_Resources); }
static ARM_SPI_STATUS SPI4_GetStatus           (void)                                              { return SPI_GetStatus (&SPI4_Resources); }
       void           SPI4_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI4_Resources); }

#ifdef MX_SPI4_TX_DMA_Instance
      void            SPI4_TX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_TX_DMA_Complete (&SPI4_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI4_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI4_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi4_tx);
}
#endif
#endif
#ifdef MX_SPI4_RX_DMA_Instance
      void            SPI4_RX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_RX_DMA_Complete(&SPI4_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI4_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI4_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi4_rx);
}
#endif
#endif

ARM_DRIVER_SPI Driver_SPI4 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI4_Initialize,
  SPI4_Uninitialize,
  SPI4_PowerControl,
  SPI4_Send,
  SPI4_Receive,
  SPI4_Transfer,
  SPI4_GetDataCount,
  SPI4_Control,
  SPI4_GetStatus
};
#endif

// SPI5
#ifdef MX_SPI5
static int32_t        SPI5_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI5_Resources); }
static int32_t        SPI5_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI5_Resources); }
static int32_t        SPI5_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI5_Resources); }
static int32_t        SPI5_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI5_Resources); }
static int32_t        SPI5_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI5_Resources); }
static int32_t        SPI5_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI5_Resources); }
static uint32_t       SPI5_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI5_Resources); }
static int32_t        SPI5_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI5_Resources); }
static ARM_SPI_STATUS SPI5_GetStatus           (void)                                              { return SPI_GetStatus (&SPI5_Resources); }
       void           SPI5_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI5_Resources); }

#ifdef MX_SPI5_TX_DMA_Instance
      void            SPI5_TX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_TX_DMA_Complete (&SPI5_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI5_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI5_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi5_tx);
}
#endif
#endif
#ifdef MX_SPI5_RX_DMA_Instance
      void            SPI5_RX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_RX_DMA_Complete(&SPI5_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI5_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI5_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi5_rx);
}
#endif
#endif

ARM_DRIVER_SPI Driver_SPI5 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI5_Initialize,
  SPI5_Uninitialize,
  SPI5_PowerControl,
  SPI5_Send,
  SPI5_Receive,
  SPI5_Transfer,
  SPI5_GetDataCount,
  SPI5_Control,
  SPI5_GetStatus
};
#endif

// SPI6
#ifdef MX_SPI6
static int32_t        SPI6_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI6_Resources); }
static int32_t        SPI6_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI6_Resources); }
static int32_t        SPI6_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI6_Resources); }
static int32_t        SPI6_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI6_Resources); }
static int32_t        SPI6_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI6_Resources); }
static int32_t        SPI6_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI6_Resources); }
static uint32_t       SPI6_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI6_Resources); }
static int32_t        SPI6_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI6_Resources); }
static ARM_SPI_STATUS SPI6_GetStatus           (void)                                              { return SPI_GetStatus (&SPI6_Resources); }
       void           SPI6_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI6_Resources); }

#ifdef MX_SPI6_TX_DMA_Instance
      void            SPI6_TX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_TX_DMA_Complete (&SPI6_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI6_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI6_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi6_tx);
}
#endif
#endif
#ifdef MX_SPI6_RX_DMA_Instance
      void            SPI6_RX_DMA_Complete     (DMA_HandleTypeDef *hdma)                           {        SPI_RX_DMA_Complete(&SPI6_Resources); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SPI6_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SPI6_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_spi6_rx);
}
#endif
#endif

ARM_DRIVER_SPI Driver_SPI6 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI6_Initialize,
  SPI6_Uninitialize,
  SPI6_PowerControl,
  SPI6_Send,
  SPI6_Receive,
  SPI6_Transfer,
  SPI6_GetDataCount,
  SPI6_Control,
  SPI6_GetStatus
};
#endif
