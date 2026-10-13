/*
  ******************************************************************************
  * @file    stl_stm32_hw_config.h
  * @author  MCD Application Team
  * @brief   Header file of STM32 HW configuration
  *          In order to be CMSIS agnostic, HW resources used by STL are re-defined here
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STL_STM32_HW_CONFIG_H
#define STL_STM32_HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  volatile uint32_t DR;             /*!< CRC Data register,                         Address offset: 0x00 */
  volatile uint32_t IDR;            /*!< CRC Independent data register,             Address offset: 0x04 */
  volatile uint32_t CR;             /*!< CRC Control register,                      Address offset: 0x08 */
  uint32_t RESERVED1;               /*!< Reserved,                                                  0x0C */
  volatile uint32_t INIT;           /*!< Initial CRC value register,                Address offset: 0x10 */
  volatile uint32_t POL;            /*!< CRC polynomial register,                   Address offset: 0x14 */
  /* end of struct not redefined as not used in STL */
} STL_Wrapper_CRC_TypeDef;

typedef struct
{
  volatile uint32_t CR;          /*!< RCC Clock Sources Control Register,                                     Address offset: 0x00 */
  volatile uint32_t ICSCR;       /*!< RCC Internal Clock Sources Calibration Register,                        Address offset: 0x04 */
  volatile uint32_t CFGR;        /*!< RCC Regulated Domain Clocks Configuration Register,                     Address offset: 0x08 */
  volatile uint32_t PLLCFGR;     /*!< RCC System PLL configuration Register,                                  Address offset: 0x0C */
  uint32_t RESERVED0;            /*!< Reserved,                                                               Address offset: 0x10 */
  uint32_t RESERVED1;            /*!< Reserved,                                                               Address offset: 0x14 */
  volatile uint32_t CIER;        /*!< RCC Clock Interrupt Enable Register,                                    Address offset: 0x18 */
  volatile uint32_t CIFR;        /*!< RCC Clock Interrupt Flag Register,                                      Address offset: 0x1C */
  volatile uint32_t CICR;        /*!< RCC Clock Interrupt Clear Register,                                     Address offset: 0x20 */
  volatile uint32_t IOPRSTR;     /*!< RCC IO port reset register,                                             Address offset: 0x24 */
  volatile uint32_t AHBRSTR;     /*!< RCC AHB peripherals reset register,                                     Address offset: 0x28 */
  volatile uint32_t APBRSTR1;    /*!< RCC APB peripherals reset register 1,                                   Address offset: 0x2C */
  volatile uint32_t APBRSTR2;    /*!< RCC APB peripherals reset register 2,                                   Address offset: 0x30 */
  volatile uint32_t IOPENR;      /*!< RCC IO port enable register,                                            Address offset: 0x34 */
  volatile uint32_t AHBENR;      /*!< RCC AHB peripherals clock enable register,                              Address offset: 0x38 */
  volatile uint32_t APBENR1;     /*!< RCC APB peripherals clock enable register1,                             Address offset: 0x3C */
  volatile uint32_t APBENR2;     /*!< RCC APB peripherals clock enable register2,                             Address offset: 0x40 */
  volatile uint32_t IOPSMENR;    /*!< RCC IO port clocks enable in sleep mode register,                       Address offset: 0x44 */
  volatile uint32_t AHBSMENR;    /*!< RCC AHB peripheral clocks enable in sleep mode register,                Address offset: 0x48 */
  volatile uint32_t APBSMENR1;   /*!< RCC APB peripheral clocks enable in sleep mode register1,               Address offset: 0x4C */
  volatile uint32_t APBSMENR2;   /*!< RCC APB peripheral clocks enable in sleep mode register2,               Address offset: 0x50 */
  volatile uint32_t CCIPR;       /*!< RCC Peripherals Independent Clocks Configuration Register,              Address offset: 0x54 */
  uint32_t RESERVED2;            /*!< Reserved,                                                               Address offset: 0x58 */
  volatile uint32_t BDCR;        /*!< RCC Backup Domain Control Register,                                     Address offset: 0x5C */
  volatile uint32_t CSR;         /*!< RCC Unregulated Domain Clock Control and Status Register,               Address offset: 0x60 */
} STL_Wrapper_RCC_TypeDef;

/* Exported constants --------------------------------------------------------*/
#define STL_WRP_PERIPH_BASE                        (0x40000000UL)
#define STL_WRP_AHBPERIPH_BASE                     (STL_WRP_PERIPH_BASE + 0x00020000UL)
#define STL_WRP_CRC_BASE                           (STL_WRP_AHBPERIPH_BASE + 0x00003000UL)
#define STL_WRP_RCC_BASE                           (STL_WRP_AHBPERIPH_BASE + 0x00001000UL)

#define STL_WRP_CRC                                ((STL_Wrapper_CRC_TypeDef *) STL_WRP_CRC_BASE)
#define STL_WRP_RCC                                ((STL_Wrapper_RCC_TypeDef *) STL_WRP_RCC_BASE)

#define STL_WRP_CRC_IDR_DEFAULT_VALUE              0x00000000UL
#define STL_WRP_CRC_CR_OUTPUT_DATA_REVERSE_NONE    0x00000000UL
#define STL_WRP_CRC_CR_OUTPUT_DATA_REVERSE         0x00000080UL
#define STL_WRP_CRC_CR_INPUT_DATA_REVERSE_NONE     0x00000000UL
#define STL_WRP_CRC_CR_INPUT_DATA_REVERSE_WORD     0x00000060UL
#define STL_WRP_CRC_CR_INPUT_DATA_REVERSE_BYTE     0x00000020UL
#define STL_WRP_CRC_CR_POLYLENGTH_32B              0x00000000UL
#define STL_WRP_CRC_CR_DR_RESET                    0x00000001UL
#define STL_WRP_CRC_INIT_DEFAULT_VALUE             0xFFFFFFFFUL
#define STL_WRP_CRC_POL_DEFAULT_CRC32_POLY         0x04C11DB7UL

#define STL_WRP_RCC_AHBENR_CRCEN_Pos               (12UL)
#define STL_WRP_RCC_AHBENR_CRCEN_Msk               (0x1UL << STL_WRP_RCC_AHBENR_CRCEN_Pos)
#define STL_WRP_RCC_AHBENR_CRCEN                   STL_WRP_RCC_AHBENR_CRCEN_Msk           /*!< CRC clock enable */

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* STL_STM32_HW_CONFIG_H */
