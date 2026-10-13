/**
  ******************************************************************************
  * @file    stl_user_api.h
  * @author  MCD Application Team
  * @brief   Header file of STL User API.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STL_USER_API_H
#define STL_USER_API_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define STL_OK_DEF              0x00006679UL /* non-trivial & enumerated pattern */
#define STL_KO_DEF              0x0000aab5UL /* non-trivial & enumerated pattern */
#define STL_PASSED_DEF          0x00001fe1UL /* non-trivial & enumerated pattern */
#define STL_PARTIAL_PASSED_DEF  0x00014b55UL /* non-trivial & enumerated pattern */
#define STL_FAILED_DEF          0x0000d32dUL /* non-trivial & enumerated pattern */
#define STL_NOT_TESTED_DEF      0x0001ffffUL /* non-trivial & enumerated pattern */
#define STL_ERROR_DEF           0x0001a976UL /* non-trivial & enumerated pattern */
#define STL_TEST_ENABLE_DEF     0x0003d7a2UL /* non-trivial & enumerated pattern */
#define STL_TEST_DISABLE_DEF    0x0006d5f9UL /* non-trivial & enumerated pattern */
#define STL_CRC_TABLE_NO_TZ_DEF 0x0048cbf9UL /* non-trivial & enumerated pattern */
#define STL_CRC_TABLE_TZ_S_DEF  0x00309562UL /* non-trivial & enumerated pattern */
#define STL_CRC_TABLE_TZ_NS_DEF 0x005291acUL /* non-trivial & enumerated pattern */

/* user defined codes to identify Fail Safe routine entry caller, a wider set
   can be defined by user for more detailed identification of the problem */
#define HAL_ERR_CODE     0
#define SCH_ERR_CODE     1
#define CXM_ERR_CODE     2
#define TM1L_ERR_CODE    10
#define TM7_ERR_CODE     11
#define TMCB_ERR_CODE    12
#define TMF_ERR_CODE     13
#define TMR_ERR_CODE     14
#define CLK_ERR_CODE     15
/* when a module returns STL_KO status, the module error idetification code is passed
with defense programming offset */
#define DEF_PROG_OFFSET  100
/* Exported types ------------------------------------------------------------*/
typedef enum
{
  STL_OK = STL_OK_DEF, /* Scheduler function successfully executed */
  STL_KO = STL_KO_DEF  /* Scheduler function unsuccessfully executed (defensive programming error,
                          checksum error). In this case the STL_TmStatus_t values are not relevant */
} STL_Status_t;        /* Type for the status return value of the STL function execution */

typedef enum
{
  STL_PASSED = STL_PASSED_DEF,                 /* Test passed. For Flash/RAM, test is passed and end
                                                  of configuration is also reached */
  STL_PARTIAL_PASSED = STL_PARTIAL_PASSED_DEF, /* Used only for RAM and Flash testing. Test passed, but
                                                  end of Flash/RAM configuration not yet reached */
  STL_FAILED = STL_FAILED_DEF,                 /* Hardware error detection by Test Module */
  STL_NOT_TESTED = STL_NOT_TESTED_DEF,         /* Initial value after a SW init, SW config, SW reset,
                                                  SW de-init or value when Test Module not executed */
  STL_ERROR = STL_ERROR_DEF                    /* Test Module unsuccessfully executed (defensive
                                                  programing check failed) */
} STL_TmStatus_t;                              /* Type for the result of a Test Module */

typedef enum
{
  STL_TEST_ENABLE = STL_TEST_ENABLE_DEF,       /* Used to add a Test Module in the list to run */
  STL_TEST_DISABLE = STL_TEST_DISABLE_DEF      /* Used to remove a Test Module from the list to run*/
} STL_TmEnable_t;                              /* Type used/checked only for Multiple testing, to add/remove
                                                  a test module in/from the test list */

typedef enum
{
  STL_CPU_TM1L_IDX,                     /* CPU ARM Core Test Module 1L index */
  STL_CPU_TM7_IDX,                      /* CPU ARM Core Test Module 7 index */
  STL_CPU_TMCB_IDX,                       /* CPU ARM Core Test Module 11 index */
  STL_CPU_TM_MAX                        /* Number of CPU ARM Core Test Modules */
} STL_CpuTmxIndex_t;             /* Type for index of CPU ARM Core Test Modules */

typedef enum
{
  STL_CRC_TABLE_NO_TZ = STL_CRC_TABLE_NO_TZ_DEF,   /* CRC Table idx when Trust Zone is disabled */
  STL_CRC_TABLE_TZ_S = STL_CRC_TABLE_TZ_S_DEF,     /* CRC Table idx when Trust Zone is enabled and subset is located in Secure binary */
  STL_CRC_TABLE_TZ_NS = STL_CRC_TABLE_TZ_NS_DEF    /* CRC Table idx when Trust Zone is enabled and subset is located in non-Secure binary */
} STL_CrcTableIdx_t;

typedef struct
{
  STL_TmStatus_t aCpuTmStatus[STL_CPU_TM_MAX]; /* Array for CPU ARM Core Test Module test status */
  STL_TmStatus_t FlashTmStatus;                /* Flash Test Module status */
  STL_TmStatus_t RamTmStatus;                  /* RAM Test Module status */
} STL_TmListStatus_t;                          /* Type used for Test Modules status */

typedef struct
{
  STL_TmEnable_t aCpuTmEnable[STL_CPU_TM_MAX]; /* Array for CPU ARM Core Test Module test list */
  STL_TmEnable_t FlashTmEnable;                /* Flash Test Module enablement information */
  STL_TmEnable_t RamTmEnable;                  /* RAM Test Module enablement information */
} STL_TmListEnable_t;                          /* Type used to define STL internal tests list */

typedef struct STL_MemSubset_struct
{
  uint32_t StartAddr;                          /* Start address of Flash or RAM  memory subset */
  uint32_t EndAddr;                            /* End address of Flash or RAM memory subset */
  STL_CrcTableIdx_t CrcTableIdx;               /* Index of the CRC table (used by Flash TM) */
  struct STL_MemSubset_struct *pNext;          /* Pointer to the next Flash or RAM memory subset -
                                                  to be set to NULL for the last subset  */
} STL_MemSubset_t;                             /* Type used to define Flash or RAM subsets to test */

typedef struct
{
  STL_MemSubset_t *pSubset;                    /* Pointer to the Flash or RAM subsets to test */
  uint32_t NumSectionsAtomic;                  /* Number of Flash or RAM sections to be tested during
                                                  an atomic test */
} STL_MemConfig_t;                             /* Type used to fully define Flash or RAM test configuration */

typedef struct
{
  STL_TmStatus_t aCpuTmStatus[STL_CPU_TM_MAX]; /* Array of forced status value for CPU Test Modules */
  STL_TmStatus_t FlashTmStatus;                /* Forced status value for Flash Test Module */
  STL_TmStatus_t RamTmStatus;                  /* Forced status value for RAM Test Module */
} STL_ArtifFailingConfig_t;                    /* Type used to force Test Modules status to a specific value
                                                  for each STL Test Module */

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* User API functions prototypes */
/* All */
STL_Status_t STL_SCH_Init(void);                                  /* To initalise the scheduler */

/* CPU */
STL_Status_t STL_SCH_RunCpuTM1L(STL_TmStatus_t *pSingleTmStatus);  /* To run CPU Test Module 1L - only for single test */
STL_Status_t STL_SCH_RunCpuTM7(STL_TmStatus_t *pSingleTmStatus);  /* To run CPU Test Module 7 - only for single test */
STL_Status_t STL_SCH_RunCpuTMCB(STL_TmStatus_t *pSingleTmStatus);  /* To run CPU Test Module Class B - only for single test */

/* Flash */
STL_Status_t STL_SCH_InitFlash(STL_TmStatus_t *pSingleTmStatus);  /* To initialise Flash test - only for single test */
STL_Status_t STL_SCH_ConfigureFlash(STL_TmStatus_t *pSingleTmStatus, STL_MemConfig_t *pFlashConfig);
/* To configure Flash subsets - only for single test */
STL_Status_t STL_SCH_RunFlashTM(STL_TmStatus_t *pSingleTmStatus); /* To run Flash test - only for single test */
STL_Status_t STL_SCH_ResetFlash(STL_TmStatus_t *pSingleTmStatus); /* To reset Flash test - for single or multiple test*/
STL_Status_t STL_SCH_DeInitFlash(STL_TmStatus_t *pSingleTmStatus);/* To de-initialise Flash test - only for single test */

/* Ram */
STL_Status_t STL_SCH_InitRam(STL_TmStatus_t *pSingleTmStatus);    /* To initialise Ram test - only for single test */
STL_Status_t STL_SCH_ConfigureRam(STL_TmStatus_t *pSingleTmStatus, STL_MemConfig_t *pRamConfig);
/* To configure Ram subsets - only for single test */
STL_Status_t STL_SCH_RunRamTM(STL_TmStatus_t *pSingleTmStatus);   /* To run Ram test - only for single test */
STL_Status_t STL_SCH_ResetRam(STL_TmStatus_t *pSingleTmStatus);   /* To reset Ram test - for single or multiple test */
STL_Status_t STL_SCH_DeInitRam(STL_TmStatus_t *pSingleTmStatus);  /* To de-initialise Ram test - only for single test */

/* artificial failing */
STL_Status_t STL_SCH_StartArtifFailing(const STL_ArtifFailingConfig_t *pArtifFailingConfig);
/* To set artificial failing configuration and start
   artificial failing feature - for single and multiple tests */
STL_Status_t STL_SCH_StopArtifFailing(void);
/* To stop artificial failing */

#endif /* STL_USER_API_H */
