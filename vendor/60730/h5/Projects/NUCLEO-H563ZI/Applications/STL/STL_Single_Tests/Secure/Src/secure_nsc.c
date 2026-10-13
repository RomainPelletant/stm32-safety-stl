/**
  ******************************************************************************
  * @file    Secure/Src/secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if defined ( __ICCARM__ )
#define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

/* Toolchain-dependent definitions of TEST_ROM_END_ADDR to define the end of the program in flash */
/* For Keil MDK: */
#if defined (__CC_ARM) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/* Load$$LR$$LR_IROM1$$Limit is linker-defined symbol following the last byte of the used part of flash memory. */
extern void *Load$$LR$$LR_IROM1$$Limit;
#define TEST_ROM_TZ_S_END_ADDR ( (uint32_t) &Load$$LR$$LR_IROM1$$Limit - 1)
#endif /* _CC_ARM */

/* For GCC (STM32CubeIDE): */
#if (defined (__GNUC__) && !defined (__CC_ARM)  && !defined (__ARMCC_VERSION) )
/* symbol _edata_load is defined in linker-script. Its address is the last word of the used part of flash memory. */
extern void *_edata_load;
#define TEST_ROM_TZ_S_END_ADDR ( (uint32_t)&_edata_load + 3)
#endif /* __GNUC__ */

/* For IAR EWARM: */
#if defined (__ICCARM__)
/* All flash content has been placed in FIXED_ORDER_ROM. Its size is added to flash base address to calculate the address of its end.
   In linker-script, block FIXED_ORDER_ROM has been defined to contain entire content of flash.
   It is placed at the ROM origin and its length is used to calculate the address of its last byte. */
#pragma section = "FIXED_ORDER_ROM"
#define TEST_ROM_TZ_S_END_ADDR ( (uint32_t) __section_begin("FIXED_ORDER_ROM") + __section_size("FIXED_ORDER_ROM") -1)
#endif /* __ICCARM__ */


CMSE_NS_ENTRY int Get_Test_Rom_TZ_S_End_Addr(void)
{
  return TEST_ROM_TZ_S_END_ADDR;
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_Init(void)
{
  return (STL_SCH_Init());
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_RunCpuTM1L(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_RunCpuTM1L(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_RunCpuTM7(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_RunCpuTM7(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_RunCpuTMCB(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_RunCpuTMCB(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_InitFlash(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_InitFlash(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_ConfigureFlash(STL_TmStatus_t *pSingleTmStatus, STL_MemConfig_t *pFlashConfig)
{
  return (STL_SCH_ConfigureFlash(pSingleTmStatus, pFlashConfig));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_RunFlashTM(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_RunFlashTM(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_ResetFlash(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_ResetFlash(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_DeInitFlash(STL_TmStatus_t *pSingleTmStatus)
{
return (STL_SCH_DeInitFlash(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_InitRam(STL_TmStatus_t *pSingleTmStatus)
{
return (STL_SCH_InitRam(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_ConfigureRam(STL_TmStatus_t *pSingleTmStatus, STL_MemConfig_t *pRamConfig)
{
  return (STL_SCH_ConfigureRam(pSingleTmStatus, pRamConfig));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_RunRamTM(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_RunRamTM(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_ResetRam(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_ResetRam(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_DeInitRam(STL_TmStatus_t *pSingleTmStatus)
{
  return (STL_SCH_DeInitRam(pSingleTmStatus));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_StartArtifFailing(const STL_ArtifFailingConfig_t *pArtifFailingConfig)
{
  return (STL_SCH_StartArtifFailing(pArtifFailingConfig));
}

CMSE_NS_ENTRY STL_Status_t SECURE_STL_SCH_StopArtifFailing(void)
{
  return (STL_SCH_StopArtifFailing());
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
