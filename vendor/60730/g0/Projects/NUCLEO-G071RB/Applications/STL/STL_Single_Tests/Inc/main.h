/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief    This file contains the common defines of the application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "stm32g0xx_nucleo.h"
#include "stl_user_api.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_it.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void FailSafe_Handler(uint32_t fail_code);
void STL_Init(void);
void STL_RunFullTest(void);
void STL_RunStep(void);
void My_TIM16_IRQHandler(void);

#endif /* __MAIN_H */

