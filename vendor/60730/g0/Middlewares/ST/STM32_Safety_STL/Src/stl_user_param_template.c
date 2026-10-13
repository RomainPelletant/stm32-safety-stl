/*
  ******************************************************************************
  * @file    stl_user_param_template.c
  * @author  MCD Application Team
  * @brief   STL User parameters
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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>


/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
/******************************************************************************/
/*       CUSTOMIZABLE PART: you can add missing STM32G0 devices below         */
/******************************************************************************/
/* FLASH configuration */
#define STL_ROM_START_ADDR (0x08000000UL) /* customizable */
#if defined (STM32G071xx)
#define STL_ROM_END_ADDR   (0x0801FFFFUL) /* customizable */ /* 128 Kbytes */
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
/* User shall locate the buffer in RAM */
/* The RAM backup buffer is placed in "backup_buffer_section". */
/* "backup_buffer_section" section is defined in scatter file */


/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
uint32_t STL_RomStart = STL_ROM_START_ADDR;
uint32_t STL_RomEnd = STL_ROM_END_ADDR;
#ifdef STL_DISABLE_RAM_BCKUP_BUF
uint32_t *STL_pRamTmBckUpBuf = NULL; /* no Backup Buffer */
#else
#if defined ( __ICCARM__ )
uint32_t STL_aRamTmBckUpBuf[STL_TM_RAM_BCKP_BUF_SZ] @ "backup_buffer_section";
#elif defined ( __CC_ARM ) || defined(__GNUC__) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
uint32_t STL_aRamTmBckUpBuf[STL_TM_RAM_BCKP_BUF_SZ] __attribute__((section("backup_buffer_section")));
#endif /*  __ICCARM__ */
uint32_t *STL_pRamTmBckUpBuf = (uint32_t *)STL_aRamTmBckUpBuf;
#endif /* STL_DISABLE_RAM_BCKUP_BUF */

/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

