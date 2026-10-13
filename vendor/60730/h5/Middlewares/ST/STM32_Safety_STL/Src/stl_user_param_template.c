/**
  ******************************************************************************
  * @file    stl_user_param_template.c
  * @author  MCD Application Team
  * @brief   STL User parameters
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/******************************************************************************/
/*       CUSTOMIZABLE PART: you can add missing STM32H5 devices below         */
/******************************************************************************/
/* FLASH configuration */
#define STL_ROM_NO_TZ_START_ADDR (0x08000000UL) /* customizable */ /* No TZ */
#if defined (STM32H563xx)
#define STL_ROM_TZ_S_START_ADDR  (0x0C000000UL) /* customizable */ /* TZ, Secure Flash */
#define STL_ROM_TZ_NS_START_ADDR (0x08100000UL) /* customizable */ /* TZ, Non-Secure Flash */
#define STL_ROM_NO_TZ_END_ADDR   (0x081FFFFFUL) /* customizable */ /* no TZ: 2 Mbytes */
#define STL_ROM_TZ_S_END_ADDR    (0x0C0FFFFFUL) /* customizable */ /* TZ, Secure: 1 Mbytes */
#define STL_ROM_TZ_NS_END_ADDR   (0x081FFFFFUL) /* customizable */ /* TZ, Non-Secure: 1 Mbytes */
/* For all devices not supporting TrustZone, addresses related to TrustZone should be initialized to NULL value */
#elif defined (STM32H503xx)
#define STL_ROM_NO_TZ_END_ADDR   (0x0801FFFFUL) /* customizable */ /* no TZ: 128 Kbytes */
#define STL_ROM_TZ_S_END_ADDR     NULL
#define STL_ROM_TZ_NS_END_ADDR    NULL
#define STL_ROM_TZ_S_START_ADDR   NULL
#define STL_ROM_TZ_NS_START_ADDR  NULL
#else
#error "please add your device ROM end address"
#endif

/******************************************************************************/
/*       !!!!!!!   NON CUSTOMIZABLE PART: DON'T MODIFY CODE BELOW   !!!!!     */
/******************************************************************************/

/* TM RAM Backup Buffer configuration */
/* the size is fixed to 8 * 32-bit words due to STL design constraint */
/* !! don't change this value !! */
#define STL_TM_RAM_BCKP_BUF_SZ (8UL)

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
uint32_t STL_RomNoTzStart = STL_ROM_NO_TZ_START_ADDR;
uint32_t STL_RomNoTzEnd = STL_ROM_NO_TZ_END_ADDR;
uint32_t STL_RomTzSStart = STL_ROM_TZ_S_START_ADDR;
uint32_t STL_RomTzSEnd = STL_ROM_TZ_S_END_ADDR;
uint32_t STL_RomTzNsStart = STL_ROM_TZ_NS_START_ADDR;
uint32_t STL_RomTzNsEnd = STL_ROM_TZ_NS_END_ADDR;

/* TM RAM Backup Buffer configuration */
/* User shall locate the buffer in Secure RAM */
/* The RAM backup buffer is placed in the "backup_buffer_section" section. */
/* The "backup_buffer_section" section is defined in scatter file */
#if defined ( __ICCARM__ )
uint32_t STL_aRamTmBckUpBuf[STL_TM_RAM_BCKP_BUF_SZ] @ "backup_buffer_section";
#elif defined ( __CC_ARM ) || defined(__GNUC__) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
uint32_t STL_aRamTmBckUpBuf[STL_TM_RAM_BCKP_BUF_SZ] __attribute__((section("backup_buffer_section")));
#else
#error "toolchain not supported"
#endif /*  __ICCARM__, __CC_ARM, __GNUC__ */

/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

