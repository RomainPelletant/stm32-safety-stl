/**
  ******************************************************************************
  * @file    Secure_nsclib/secure_nsc.h
  * @author  MCD Application Team
  * @brief   Header for secure non-secure callable APIs list
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SECURE_NSC_H
#define SECURE_NSC_H

/* Includes ------------------------------------------------------------------*/
#include "stl_user_api.h"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int Get_Test_Rom_TZ_S_End_Addr(void);
STL_Status_t SECURE_STL_SCH_Init(void);
STL_Status_t SECURE_STL_SCH_RunCpuTM1L(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_RunCpuTM7(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_RunCpuTMCB(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_InitFlash(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_ConfigureFlash(STL_TmStatus_t *pSingleTmStatus, STL_MemConfig_t *pFlashConfig);
STL_Status_t SECURE_STL_SCH_RunFlashTM(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_ResetFlash(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_DeInitFlash(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_InitRam(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_ConfigureRam(STL_TmStatus_t *pSingleTmStatus, STL_MemConfig_t *pRamConfig);
STL_Status_t SECURE_STL_SCH_RunRamTM(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_ResetRam(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_DeInitRam(STL_TmStatus_t *pSingleTmStatus);
STL_Status_t SECURE_STL_SCH_StartArtifFailing(const STL_ArtifFailingConfig_t *pArtifFailingConfig);
STL_Status_t SECURE_STL_SCH_StopArtifFailing(void);

#endif /* SECURE_NSC_H */

