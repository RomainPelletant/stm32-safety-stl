/*
  ******************************************************************************
  * @file    stl_stm32_hw_config.h
  * @author  MCD Application Team
  * @brief   Header file of STM32 HW configuration
  *          In order to be CMSIS agnostic, HW resources used by STL are re-defined here
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
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
  volatile uint32_t DR;          /*!< CRC Data register,                           Address offset: 0x00 */
  volatile uint32_t IDR;         /*!< CRC Independent data register,               Address offset: 0x04 */
  volatile uint32_t CR;          /*!< CRC Control register,                        Address offset: 0x08 */
  uint32_t          RESERVED2;   /*!< Reserved,                                                    0x0C */
  volatile uint32_t INIT;        /*!< Initial CRC value register,                  Address offset: 0x10 */
  volatile uint32_t POL;         /*!< CRC polynomial register,                     Address offset: 0x14 */
           uint32_t RESERVED3[246]; /*!< Reserved,                                                      */
  volatile uint32_t HWCFGR;         /*!< CRC IP HWCFGR register,                   Address offset: 0x3F0 */
  volatile uint32_t VERR;           /*!< CRC IP version register,                  Address offset: 0x3F4 */
  volatile uint32_t PIDR;           /*!< CRC IP type identification register,      Address offset: 0x3F8 */
  volatile uint32_t SIDR;           /*!< CRC IP map Size ID register,              Address offset: 0x3FC */
}  STL_Wrapper_CRC_TypeDef;

/* Exported constants --------------------------------------------------------*/
/* RCC, CRC, SBS, FLASH peripherals */
/* Secure base address */
#define STL_WRP_PERIPH_BASE_S                      (0x50000000UL)
#define STL_WRP_AHB1PERIPH_BASE_S                  (STL_WRP_PERIPH_BASE_S + 0x20000UL)
#define STL_WRP_APB2PERIPH_BASE_S                  (STL_WRP_PERIPH_BASE_S + 0x10000UL)
#define STL_WRP_AHB3PERIPH_BASE_S                  (STL_WRP_PERIPH_BASE_S + 0x04020000UL)
#define STL_WRP_APB3PERIPH_BASE_S                  (STL_WRP_PERIPH_BASE_S + 0x04000000UL)
#define STL_WRP_CRC_BASE_S                         (STL_WRP_AHB1PERIPH_BASE_S + 0x3000UL)
#define STL_WRP_RCC_BASE_S                         (STL_WRP_AHB3PERIPH_BASE_S + 0x0C00UL)
#define STL_WRP_SBS_BASE_S                         (STL_WRP_APB3PERIPH_BASE_S + 0x0400UL)
/* non-Secure base address */
#define STL_WRP_PERIPH_BASE_NS                     (0x40000000UL)
#define STL_WRP_AHB1PERIPH_BASE_NS                 (STL_WRP_PERIPH_BASE_NS + 0x20000UL)
#define STL_WRP_APB2PERIPH_BASE_NS                 (STL_WRP_PERIPH_BASE_NS + 0x10000UL)
#define STL_WRP_AHB3PERIPH_BASE_NS                 (STL_WRP_PERIPH_BASE_NS + 0x04020000UL)
#define STL_WRP_APB3PERIPH_BASE_NS                 (STL_WRP_PERIPH_BASE_NS + 0x04000000UL)
#define STL_WRP_CRC_BASE_NS                        (STL_WRP_AHB1PERIPH_BASE_NS + 0x3000UL)
#define STL_WRP_RCC_BASE_NS                        (STL_WRP_AHB3PERIPH_BASE_NS + 0x0C00UL)
#define STL_WRP_SBS_BASE_NS                        (STL_WRP_APB3PERIPH_BASE_NS + 0x0400UL)

#if defined (__ARM_FEATURE_CMSE) && !defined (TRUSTZONE_DISABLED_COMPILER_SPECIFIC) /* TZ enabled */
#if (__ARM_FEATURE_CMSE == 3UL) /* Secure */
#define STL_WRP_PERIPH_BASE                        STL_WRP_PERIPH_BASE_S
#define STL_WRP_AHB1PERIPH_BASE                    STL_WRP_AHB1PERIPH_BASE_S
#define STL_WRP_APB2PERIPH_BASE                    STL_WRP_APB2PERIPH_BASE_S
#define STL_WRP_AHB3PERIPH_BASE                    STL_WRP_AHB3PERIPH_BASE_S
#define STL_WRP_APB3PERIPH_BASE                    STL_WRP_APB3PERIPH_BASE_S
#define STL_WRP_CRC_BASE                           STL_WRP_CRC_BASE_S
#define STL_WRP_RCC_BASE                           STL_WRP_RCC_BASE_S
#define STL_WRP_SBS_BASE                           STL_WRP_SBS_BASE_S
#else /* non-Secure */
#error "When TZ is enabled, the STL shall be compiled with Secure enabled"
#endif /* __ARM_FEATURE_CMSE == 3U */
#else /* TZ not enabled */
#define STL_WRP_PERIPH_BASE                        STL_WRP_PERIPH_BASE_NS
#define STL_WRP_AHB1PERIPH_BASE                    STL_WRP_AHB1PERIPH_BASE_NS
#define STL_WRP_APB2PERIPH_BASE                    STL_WRP_APB2PERIPH_BASE_NS
#define STL_WRP_AHB3PERIPH_BASE                    STL_WRP_AHB3PERIPH_BASE_NS
#define STL_WRP_APB3PERIPH_BASE                    STL_WRP_APB3PERIPH_BASE_NS
#define STL_WRP_CRC_BASE                           STL_WRP_CRC_BASE_NS
#define STL_WRP_RCC_BASE                           STL_WRP_RCC_BASE_NS
#define STL_WRP_SBS_BASE                           STL_WRP_SBS_BASE_NS
#endif /* __ARM_FEATURE_CMSE */

#define STL_WRP_RCC_AHB1ENR_OFFSET                 (0x88UL)
#define STL_WRP_RCC_AHB1ENR_CRCEN_Pos              (12UL)
#define STL_WRP_RCC_AHB1ENR_CRCEN_Msk              (0x1UL << STL_WRP_RCC_AHB1ENR_CRCEN_Pos)   /*!< 0x00001000 */
#define STL_WRP_RCC_AHB1ENR_CRCEN                  STL_WRP_RCC_AHB1ENR_CRCEN_Msk

#define STL_WRP_RCC_APB3ENR_SBSEN_OFFSET           (0xA8UL)
#define STL_WRP_RCC_APB3ENR_SBSEN_Pos              (1UL)
#define STL_WRP_RCC_APB3ENR_SBSEN_Msk              (0x1UL << STL_WRP_RCC_APB3ENR_SBSEN_Pos )  /*!< 0x00000002 */
#define STL_WRP_RCC_APB3ENR_SBSEN                  STL_WRP_RCC_APB3ENR_SBSEN_Msk

#define STL_WRP_SBS_FPUIMR_OFFSET               (0x104UL)
#define STL_WRP_SBS_FPUIMR_FPU_IE_Pos           (0UL)
#define STL_WRP_SBS_FPUIMR_FPU_IE_Msk           (0x3FUL << STL_WRP_SBS_FPUIMR_FPU_IE_Pos)           /*!< 0x0000003F - */
#define STL_WRP_SBS_FPUIMR_FPU_IE_5             (0x20UL << STL_WRP_SBS_FPUIMR_FPU_IE_Pos)           /*!< 0x00000020 - Inexact Interrupt enable (interrupt disabled at reset) */

/* other internal definitions in stl_tm.h */

/* CRC type and default values */
#define STL_WRP_CRC_S                              ((STL_Wrapper_CRC_TypeDef *) STL_WRP_CRC_BASE_S)
#define STL_WRP_CRC_NS                             ((STL_Wrapper_CRC_TypeDef *) STL_WRP_CRC_BASE_NS)
#if defined (__ARM_FEATURE_CMSE) && !defined (TRUSTZONE_DISABLED_COMPILER_SPECIFIC) /* TZ enabled */
#if (__ARM_FEATURE_CMSE == 3U) /* Secure */
#define STL_WRP_CRC                                STL_WRP_CRC_S
#else /* non-Secure */
#error "STL shall be compiled with TZ & Secure enabled"
#endif /* __ARM_FEATURE_CMSE == 3U */
#else /* TZ not enabled */
#define STL_WRP_CRC                                STL_WRP_CRC_NS
#endif /* __ARM_FEATURE_CMSE */
#define STL_WRP_CRC_IDR_DEFAULT_VALUE              0x00000000UL
#define STL_WRP_CRC_CR_OUTPUT_DATA_REVERSE_NONE    0x00000000UL
#define STL_WRP_CRC_CR_OUTPUT_DATA_REVERSE         0x00000080UL
#define STL_WRP_CRC_CR_INPUT_DATA_REVERSE_NONE     0x00000000UL
#define STL_WRP_CRC_CR_INPUT_DATA_REVERSE_WORD     0x00000060UL
#define STL_WRP_CRC_CR_INPUT_DATA_REVERSE_HALFWORD 0x00000040UL
#define STL_WRP_CRC_CR_INPUT_DATA_REVERSE_BYTE     0x00000020UL
#define STL_WRP_CRC_CR_POLYLENGTH_32B              0x00000000UL
#define STL_WRP_CRC_CR_DR_RESET                    0x00000001UL
#define STL_WRP_CRC_INIT_DEFAULT_VALUE             0xFFFFFFFFUL
#define STL_WRP_CRC_POL_DEFAULT_CRC32_POLY         0x04C11DB7UL

#define SAU_RNR (0xE000EDD8UL)

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* STL_STM32_HW_CONFIG_H */
