/**
  ******************************************************************************
  * @file           : main.c
  * @author  MCD Application Team
  * @brief   main body test scenario
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

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* RAM & FLASH Tests definitions */
#define FLASH_SECTION_SIZE 1024U
#define RAM_SECTION_SIZE 128U
#ifdef STL_DISABLE_RAM_BCKUP_BUF
#define RAM_BACKUP_BUFFER_SIZE 0
#else
#define RAM_BACKUP_BUFFER_SIZE 32
#endif

/* If the addresses match your device, you may remove the following warning or comment it out. */
#warning "Please make sure that the following boundary addresses match the application and the device:"
#define TEST_RAM_START_ADDR (0x20000000U + RAM_BACKUP_BUFFER_SIZE)
#define TEST_ROM_START_ADDR 0x08000000U
/* Change this value to reflect actual usage of RAM by the application or device RAM size */
#define TEST_RAM_SECTION_NB_RUN 8    /* only 8 RAM sections are tested during runtime. */

/* During initial full test, the entire 640 kB area of RAM is tested */
#define TEST_RAM_END_ADDR_FULL  0x2009FFFFU

/* Toolchain-dependent definitions of TEST_ROM_END_ADDR to define the end of the program in flash */
/* For Keil MDK: */
#if defined (__CC_ARM) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/* Load$$LR$$LR_IROM1$$Limit is linker-defined symbol following the last byte of the used part of flash memory. */
extern void *Load$$LR$$LR_IROM1$$Limit;
#define TEST_ROM_END_ADDR ( (uint32_t) &Load$$LR$$LR_IROM1$$Limit - 1)
#endif /* _CC_ARM */

/* For GCC (STM32CubeIDE): */
#if (defined (__GNUC__) && !defined (__CC_ARM)  && !defined (__ARMCC_VERSION) )
/* symbol _edata_load is defined in linker-script. Its address is the last word of the used part of flash memory. */
extern void *_edata_load;
#define TEST_ROM_END_ADDR ( (uint32_t)&_edata_load + 3)
#endif /* __GNUC__ */

/* For IAR EWARM: */
#if defined (__ICCARM__)
/* All flash content has been placed in FIXED_ORDER_ROM. Its size is added to flash base address to calculate the address of its end.
   In linker-script, block FIXED_ORDER_ROM has been defined to contain entire content of flash.
   It is placed at the ROM origin and its length is used to calculate the address of its last byte. */
#pragma section = "FIXED_ORDER_ROM"
#define TEST_ROM_END_ADDR ( (uint32_t) TEST_ROM_START_ADDR + __section_size("FIXED_ORDER_ROM") -1)
#endif /* __ICCARM__ */

/* Value of the system clock frequency at run time in Hz */
#define SYSTCLK_AT_RUN (uint32_t)(250000000uL)
/* Value of the Internal LSI oscillator in Hz */
#define LSI_Freq    ((uint32_t)32000uL)
/* CLK frequency above this limit considered as harmonics in case of HSE */
#define CLK_LimitHigh(fcy) ((uint32_t)(((fcy)/LSI_Freq)*4u*5u)/4u) /* (Value + 25%) */
/* CLK frequency below this limit considered as sub-harmonics in case of HSE */
#define CLK_LimitLow(fcy) ((uint32_t)(((fcy)/LSI_Freq)*4u*3u)/4u)  /* (Value - 25%) */
/* if SYSCK is derived from HSI, the upper fixed clock limits can be set more severe
   e.g., ...*2u*6u)/5 resp. ...*2u*4u)/5 (~ value +/- 20%) but such a severe restriction
   can require including some adaptive flow to compensate possible temperature drift
   of HSI by making acceptable window defined by these limits variable in time */

/* uncomment following lines when simulating artificial failing of particular modules
   during execution of either full or run test */
/* #define ARTI_FAILING_CPU_TM */
/* #define ARTI_FAILING_FLASH_TM */
#define ARTI_FAILING_RAM_TM
/* #define ARTI_FAILING_CLK_TM  */
/* #define ARTI_FAILING_DURING_FULL_TEST */

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* variables to keep user configuration and status of all the test modules
   - can be handled as local and/or changed dynamically inside the STL_RunStep procedure
   - set of global variables is applied here to keep overall overview when debug STL */
STL_TmStatus_t StlCpuTm1LStatus;
STL_TmStatus_t StlCpuTm7Status;
STL_TmStatus_t StlCpuTmCBStatus;
STL_TmStatus_t StlFlashStatus;
STL_TmStatus_t StlRamStatus;
STL_TmStatus_t StlClkStatus;

/* following variables have to be static due to separation of configuration and run procedures */
/* Flash & RAM configurations: */
STL_MemSubset_t FlashSubsetRunTime;
STL_MemSubset_t RamSubsetRunTime;
STL_MemConfig_t FlashConfigRunTime;
STL_MemConfig_t RamConfigRunTime;

/* internal variables for clock cross check measurement */
volatile uint16_t tmpCC1_last;
volatile uint32_t PeriodValue;
volatile uint32_t PeriodValueInv;
volatile uint32_t LSIPeriodFlag;

/* structure to handle artificial failing */
#if defined(ARTI_FAILING_CPU_TM) || defined(ARTI_FAILING_FLASH_TM) || defined(ARTI_FAILING_RAM_TM)
STL_ArtifFailingConfig_t ArtifFailing = {{ STL_PASSED, STL_PASSED, STL_PASSED }, STL_PASSED, STL_PASSED };
#endif /* ARTI_FAILING */

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static STL_Status_t STL_InitClock_Xcross_Measurement(void);
static STL_Status_t STL_RunClockTest(STL_TmStatus_t *clk_sts);

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 250000000  (CPU Clock)
  *            HCLK(Hz)                       = 250000000  (Bus matrix and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1 (APB1 Clock  250MHz)
  *            APB2 Prescaler                 = 1 (APB2 Clock  250MHz)
  *            APB3 Prescaler                 = 1 (APB3 Clock  250MHz)
  *            HSI Frequency(Hz)              = 64000000
  *            PLL_M                          = 16
  *            PLL_N                          = 125
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 125;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    FailSafe_Handler(HAL_ERR_CODE);
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    FailSafe_Handler(HAL_ERR_CODE);
  }
}

/*----------------------------------------------------------------------------*/
/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static STL_Status_t STL_InitClock_Xcross_Measurement(void)
{
  TIM_HandleTypeDef htim16;
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* TIMx Peripheral clock enable */
  __TIM16_CLK_ENABLE();

  /* Configure the NVIC for TIM16 */
  HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM16_IRQn);

  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 0;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    return (STL_KO);
  }
  if (HAL_TIM_IC_Init(&htim16) != HAL_OK)
  {
    return (STL_KO);
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV4;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim16, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    return (STL_KO);
  }
  if (HAL_TIMEx_TISelection(&htim16, TIM_TIM16_TI1_LSI, TIM_CHANNEL_1) != HAL_OK)
  {
    return (STL_KO);
  }
  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&htim16, TIM_CHANNEL_1) != HAL_OK)
  {
    return (STL_KO);
  }
  return (STL_OK);
}

/*----------------------------------------------------------------------------*/
/**
  * @brief  Verification result of the cross check frequencies process
  * @param  : clk_sts  Pointer to Test Module status to be written to.
  * @retval : None
  */
STL_Status_t STL_RunClockTest(STL_TmStatus_t *clk_sts)
{
  STL_Status_t tst_res;

  *clk_sts = STL_PARTIAL_PASSED;
  /* next line can produce compilation warning due to accessing of two volatile
     variables making single result value integrity pair. The pair is changed
     simultaneously exclusively at TIM16 IRQ and its integrity is verified here
     before the clock measurement is compared if it fits within expected range */
  if (((PeriodValue ^ PeriodValueInv) == 0xFFFFFFFFuL)\
  &&   (LSIPeriodFlag == 0xAAAA5555u) )
  {
  #ifndef ARTI_FAILING_CLK_TM
    *clk_sts = STL_PASSED;
    if (PeriodValue < CLK_LimitLow(SYSTCLK_AT_RUN))
    {
      *clk_sts = STL_FAILED;    /* Sub-harmonics: HSE -25% below expected */
    }
    if (PeriodValue > CLK_LimitHigh(SYSTCLK_AT_RUN))
    {
      *clk_sts = STL_FAILED;    /* Harmonics: HSE +25% above expected */
    }
  #else
    *clk_sts = STL_FAILED;
  #endif /* ARTI_FAILING_CLK_TM */

    /* clear flag here to ensure the latest LSI measurement result will be taken into account at next check */
    LSIPeriodFlag = 0u;
    tst_res = STL_OK;
  }
  else
  {
    tst_res = STL_KO; /* Clock measurement flow error */
  }
  return(tst_res);
}

/*----------------------------------------------------------------------------*/
/**
  * @brief TIM16 IC interrupt handler
  * @param None
  * @retval None
  */
void TIM16_IRQHandler(void)
{
  uint16_t tmpCC1_last_cpy;

  if ((TIM16->SR & TIM_SR_CC1IF) != 0u )
  {
    /* store previous captured value */
    tmpCC1_last_cpy = tmpCC1_last;
    /* get currently captured value */
    tmpCC1_last = (uint16_t)(TIM16->CCR1);
    /* The CC1IF flag is already cleared here by reading CCR1 register */

    /* overwrite results only in case the data is required */
    if (LSIPeriodFlag == 0u)
    {
      /* Skip over the measurement at case of timer over capture */
      if ((TIM16->SR & TIM_SR_CC1OF) == 0u)
      {
        /* Compute period length and double store its value secured at integrity pair  */
        PeriodValue = ((uint32_t)(tmpCC1_last) - (uint32_t)(tmpCC1_last_cpy)) & 0xFFFFuL;
        PeriodValueInv = ~PeriodValue;
        /* Set Flag confirming secured storage tested at main loop */
        LSIPeriodFlag = 0xAAAA5555uL;
      }
      else
      {
        /* ignore computation in case of IC overcapture */
        TIM16->SR &= (uint16_t)(~TIM_SR_CC1OF);
      }
    }
    /* ignore computation in case data is not required */
  }
}

/*----------------------------------------------------------------------------*/
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  fail_code Code of the failure detected
  * @retval None
  */
void FailSafe_Handler(uint32_t fail_code)
{
  /* Here, user has to add an action to setup & keep the application in safe state.
     The fail_code input parameter together with TM dedicated user status value
     can be used to identify detected issue and adopt specific action */
  while (1)
  {
    #if defined(STL_EVAL_MODE)
      /* LED2 slow flashing loop */
      BSP_LED_On(LED2);
      HAL_Delay(100);
      BSP_LED_Off(LED2);
      if(fail_code < DEF_PROG_OFFSET)
      {
        HAL_Delay(1900);
      }
      else
      {
         HAL_Delay(3900);
      }
    #endif  /* STL_EVAL_MODE */
  }
}

/*----------------------------------------------------------------------------*/

/**
  * @brief Single test sequence configuration
  * @param  None
  * @retval None
  */
void STL_Init(void)
{
  /* FLASH test Config
     *****************
     In this example, single continuous area (subset) is tested under unique configuration setting.
     User can apply wider set of subsets tested sequentially to cover separated memory areas
     as well as different configurations to be applied for selected subset(s) - see UM */
  FlashSubsetRunTime.StartAddr = TEST_ROM_START_ADDR;
  FlashSubsetRunTime.EndAddr = TEST_ROM_END_ADDR;
  FlashSubsetRunTime.CrcTableIdx = STL_CRC_TABLE_NO_TZ;
  FlashSubsetRunTime.pNext = NULL;
  FlashConfigRunTime.pSubset = &FlashSubsetRunTime;
  FlashConfigRunTime.NumSectionsAtomic = 1; /* split test into sections of 1kB */

   /* RAM test Config
     ***************
     In this example, single continuous area (subset) is tested under unique configuration setting.
     User can apply wider set of subsets tested sequentially to cover separated memory areas
     as well as different configurations to be applied for selected subset(s) - see UM */
  RamSubsetRunTime.StartAddr = TEST_RAM_START_ADDR;
  RamSubsetRunTime.EndAddr = TEST_RAM_START_ADDR + TEST_RAM_SECTION_NB_RUN * RAM_SECTION_SIZE - 1;
  RamSubsetRunTime.pNext = NULL;
  RamConfigRunTime.pSubset = &RamSubsetRunTime;
  RamConfigRunTime.NumSectionsAtomic = 1; /* split test into sections of 128 bytes */
  /* RamConfig.NumSectionsAtomic = (2 * TEST_RAM_SECTION_NB);  */  /* - use this setting for one shot */


  /* test status initialization */
  StlCpuTm1LStatus = STL_NOT_TESTED;
  StlCpuTm7Status = STL_NOT_TESTED;
  StlCpuTmCBStatus = STL_NOT_TESTED;
  StlFlashStatus= STL_NOT_TESTED;
  StlRamStatus= STL_NOT_TESTED;

  if (STL_SCH_Init() != STL_OK)
  {
    FailSafe_Handler(SCH_ERR_CODE + DEF_PROG_OFFSET);
  }

  /* Clock test Config */
  if(STL_InitClock_Xcross_Measurement() != STL_OK)
  {
    FailSafe_Handler(CXM_ERR_CODE + DEF_PROG_OFFSET);
  }
  /* Wait for two subsequent LSI periods measurements to perform initial crosscheck computation */
  LSIPeriodFlag = 0u;
  while (LSIPeriodFlag == 0u)
  { }
  LSIPeriodFlag = 0u;
  while (LSIPeriodFlag == 0u)
  { }
}

/*----------------------------------------------------------------------------*/
/**
  * @brief Full CPU, FLASH, RAM, and clock test sequence to be run at application startup
  * @param  None
  * @retval None
  */
void STL_RunFullTest(void)
{
  /* Example of complete device self-test
     ************************************
     This function executes all the provided CPU, flash, RAM, and clock test modules in this order. Test
     of both the memories is performed in one shot in their entirety here. The function should be called
     prior to the main application entry, but can also be re-called by the application at any later point.
     Configuration structures for handling tests of the memories are declared locally, therefore they
     cannot be referenced outside of the function which is supposed to be executed as a whole.
  */

  /********************/
  /* CPU Test modules */
  /********************/

#if defined(ARTI_FAILING_CPU_TM) && defined(ARTI_FAILING_DURING_FULL_TEST)
  /* Artificial failing feature -
     when activated, it forces the STL outputs to predefined values */
  ArtifFailing.aCpuTmStatus[0] = STL_PASSED;
  ArtifFailing.aCpuTmStatus[1] = STL_PASSED;
  ArtifFailing.aCpuTmStatus[2] = STL_FAILED;
  STL_SCH_StartArtifFailing(&ArtifFailing);
#endif /* ARTI_FAILING_CPU_TM */

  /* CPU TM1L */
  if (STL_SCH_RunCpuTM1L(&StlCpuTm1LStatus) != STL_OK)
  {
    FailSafe_Handler(TM1L_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlCpuTm1LStatus != STL_PASSED)
  {
    FailSafe_Handler(TM1L_ERR_CODE);
  }
  /* CPU TM7 */
  if (STL_SCH_RunCpuTM7(&StlCpuTm7Status) != STL_OK)
  {
    FailSafe_Handler(TM7_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlCpuTm7Status != STL_PASSED)
  {
    FailSafe_Handler(TM7_ERR_CODE);
  }
  /* CPU TMCB */
  if (STL_SCH_RunCpuTMCB(&StlCpuTmCBStatus) != STL_OK)
  {
    FailSafe_Handler(TMCB_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlCpuTmCBStatus != STL_PASSED)
  {
    FailSafe_Handler(TMCB_ERR_CODE);
  }

#if defined(ARTI_FAILING_CPU_TM) && defined(ARTI_FAILING_DURING_FULL_TEST)
  STL_SCH_StopArtifFailing();
#endif /* ARTI_FAILING_CPU_TM */


  /*********************/
  /* FLASH Test Module */
  /*********************/

#if defined(ARTI_FAILING_FLASH_TM) && defined(ARTI_FAILING_DURING_FULL_TEST)
    /* forced STL_FAILED status simulates the TM configuration error and
       STL_NOT_TESTED status simulates TM run error */
    ArtifFailing.FlashTmStatus = STL_FAILED;
  STL_SCH_StartArtifFailing(&ArtifFailing);
#endif /* ARTI_FAILING_FLASH_TM */

  /* The following configuration tests the entire program in the flash, in one shot */
  STL_MemSubset_t FlashSubsetFullTest = {
      .StartAddr = TEST_ROM_START_ADDR,
      .EndAddr   = TEST_ROM_END_ADDR,
      .CrcTableIdx = STL_CRC_TABLE_NO_TZ,
      .pNext     = NULL
  };
  STL_MemConfig_t FlashConfigFullTest = {
      .NumSectionsAtomic = UINT32_MAX,
      .pSubset           = &FlashSubsetFullTest
  };
  if (STL_SCH_InitFlash(&StlFlashStatus) != STL_OK)
  {
    FailSafe_Handler(TMF_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (STL_SCH_ConfigureFlash(&StlFlashStatus, &FlashConfigFullTest) != STL_OK)
  {
    FailSafe_Handler(TMF_ERR_CODE + DEF_PROG_OFFSET);
  }
  else if (StlFlashStatus != STL_NOT_TESTED)
  {
    FailSafe_Handler(TMF_ERR_CODE);
  }
  if (STL_SCH_RunFlashTM(&StlFlashStatus) != STL_OK)
  {
    FailSafe_Handler(TMF_ERR_CODE + DEF_PROG_OFFSET);
  }
  else if (StlFlashStatus != STL_PASSED)
  {
    FailSafe_Handler(TMF_ERR_CODE);
  }
  StlFlashStatus = STL_NOT_TESTED;

#if defined(ARTI_FAILING_FLASH_TM) && defined(ARTI_FAILING_DURING_FULL_TEST)
  STL_SCH_StopArtifFailing();
#endif /* ARTI_FAILING_FLASH_TM */


  /********************/
  /* RAM Test module  */
  /********************/

#if defined(ARTI_FAILING_RAM_TM) && defined(ARTI_FAILING_DURING_FULL_TEST)
    /* forced STL_FAILED status simulates the TM configuration error and
       STL_NOT_TESTED status simulates TM run error */
    ArtifFailing.RamTmStatus = STL_NOT_TESTED;
  STL_SCH_StartArtifFailing(&ArtifFailing);
#endif /* ARTI_FAILING_RAM_TM */

  /* The following configuration tests the entire user-defined range of the RAM used by the program */
  /* RAM subset: */
  STL_MemSubset_t RamSubsetFullTest = {
      .StartAddr = TEST_RAM_START_ADDR,
      .EndAddr   = TEST_RAM_END_ADDR_FULL,
      .pNext     = NULL
  };
  STL_MemConfig_t RamConfigFullTest = {
      .NumSectionsAtomic = UINT32_MAX,
      .pSubset = &RamSubsetFullTest
  };

  if (STL_SCH_InitRam(&StlRamStatus) != STL_OK)
  {
    FailSafe_Handler(TMR_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (STL_SCH_ConfigureRam(&StlRamStatus, &RamConfigFullTest) != STL_OK)
  {
    FailSafe_Handler(TMR_ERR_CODE + DEF_PROG_OFFSET);
  }
  else if (StlRamStatus != STL_NOT_TESTED)
  {
    FailSafe_Handler(TMR_ERR_CODE);
  }
  if (STL_SCH_RunRamTM(&StlRamStatus) != STL_OK)
  {
    FailSafe_Handler(TMR_ERR_CODE + DEF_PROG_OFFSET);
  }
  else if (StlRamStatus != STL_PASSED)
  {
    FailSafe_Handler(TMR_ERR_CODE);
  }
  StlRamStatus = STL_NOT_TESTED;

#if defined(ARTI_FAILING_RAM_TM) && defined(ARTI_FAILING_DURING_FULL_TEST)
  STL_SCH_StopArtifFailing();
#endif /* ARTI_FAILING_RAM_TM */

  /************/
  /* Clock TM */
  /************/
  StlClkStatus = STL_NOT_TESTED;
  if (STL_RunClockTest(&StlClkStatus) != STL_OK)
  {
    FailSafe_Handler(CLK_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlClkStatus != STL_PASSED)
  {
    FailSafe_Handler(CLK_ERR_CODE);
  }
}

/*----------------------------------------------------------------------------*/
/**
  * @brief Single partial step of the test sequence
  * @param  None
  * @retval None
  */
void STL_RunStep(void)
{
  /* Example of the test sequence execution
     **************************************
     In this example, a unique sequence performing all the tests in one shot is applied.
     Note it is fully upon user to define order and repetition frequency of all the API calls.
     The same sequence of the calls can be repeated regularly or few different sequences can be
     applied at different time slots. All the tests can be repeated without any limitation
     except for clock test which can not be called earlier than 8 LSI periods to prepare new clock
     computation result in between two comparisons (otherwise the clock API call fails). */

  /********************/
  /* CPU Test modules */
  /********************/

#ifdef ARTI_FAILING_CPU_TM
  /* Artificial failing feature -
     when activated, it forces the STL outputs to predefined values */
  ArtifFailing.aCpuTmStatus[0] = STL_PASSED;
  ArtifFailing.aCpuTmStatus[1] = STL_PASSED;
  ArtifFailing.aCpuTmStatus[2] = STL_FAILED;
  STL_SCH_StartArtifFailing(&ArtifFailing);
#endif /* ARTI_FAILING_CPU_TM */

  /* CPU TM1L */
  if (STL_SCH_RunCpuTM1L(&StlCpuTm1LStatus) != STL_OK)
  {
    FailSafe_Handler(TM1L_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlCpuTm1LStatus != STL_PASSED)
  {
    FailSafe_Handler(TM1L_ERR_CODE);
  }
  /* CPU TM7 */
  if (STL_SCH_RunCpuTM7(&StlCpuTm7Status) != STL_OK)
  {
    FailSafe_Handler(TM7_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlCpuTm7Status != STL_PASSED)
  {
    FailSafe_Handler(TM7_ERR_CODE);
  }
  /* CPU TMCB */
  if (STL_SCH_RunCpuTMCB(&StlCpuTmCBStatus) != STL_OK)
  {
    FailSafe_Handler(TMCB_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlCpuTmCBStatus != STL_PASSED)
  {
    FailSafe_Handler(TMCB_ERR_CODE);
  }

#ifdef ARTI_FAILING_CPU_TM
  STL_SCH_StopArtifFailing();
#endif /* ARTI_FAILING_CPU_TM */


  /************/
  /* Flash TM */
  /************/

#ifdef ARTI_FAILING_FLASH_TM
    /* forced STL_FAILED status simulates the TM configuration error and
       STL_NOT_TESTED status simulates TM run error */
    ArtifFailing.FlashTmStatus = STL_FAILED;
  STL_SCH_StartArtifFailing(&ArtifFailing);
#endif /* ARTI_FAILING_FLASH_TM */

  if (StlFlashStatus == STL_NOT_TESTED)
  {
    /* Init & configure Flash TM */
    if (STL_SCH_InitFlash(&StlFlashStatus) != STL_OK)
    {
      FailSafe_Handler(TMF_ERR_CODE + DEF_PROG_OFFSET);
    }
    if (STL_SCH_ConfigureFlash(&StlFlashStatus, &FlashConfigRunTime) != STL_OK)
    {
      FailSafe_Handler(TMF_ERR_CODE + DEF_PROG_OFFSET);
    }
    else if (StlFlashStatus != STL_NOT_TESTED)
    {
      FailSafe_Handler(TMF_ERR_CODE);
    }
  }
  /* run partial atomic test */
  if (STL_SCH_RunFlashTM(&StlFlashStatus) != STL_OK)
  {
    FailSafe_Handler(TMF_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlFlashStatus != STL_PARTIAL_PASSED)
  {
    if (StlFlashStatus == STL_PASSED)
    {
      /* test completed successfully - to be restarted */
      StlFlashStatus= STL_NOT_TESTED;
      #if defined(STL_EVAL_MODE)
        BSP_LED_Toggle(LED2);
      #endif  /* STL_EVAL_MODE */
    }
    else
    {
      FailSafe_Handler(TMF_ERR_CODE);
    }
  }

#ifdef ARTI_FAILING_FLASH_TM
  STL_SCH_StopArtifFailing();
#endif /* ARTI_FAILING_FLASH_TM */


  /**********/
  /* RAM TM */
  /**********/

#ifdef ARTI_FAILING_RAM_TM
    /* forced STL_FAILED status simulates the TM configuration error and
       STL_NOT_TESTED status simulates TM run error */
    ArtifFailing.RamTmStatus = STL_NOT_TESTED;
    STL_SCH_StartArtifFailing(&ArtifFailing);
#endif /* ARTI_FAILING_RAM_TM */

  if (StlRamStatus == STL_NOT_TESTED)
  {
    /* Initialize & configure Flash TM */
    if (STL_SCH_InitRam(&StlRamStatus) != STL_OK)
    {
      FailSafe_Handler(TMR_ERR_CODE + DEF_PROG_OFFSET);
    }
    if (STL_SCH_ConfigureRam(&StlRamStatus, &RamConfigRunTime) != STL_OK)
    {
      FailSafe_Handler(TMR_ERR_CODE + DEF_PROG_OFFSET);
    }
    else if (StlRamStatus != STL_NOT_TESTED)
    {
      FailSafe_Handler(TMR_ERR_CODE);
    }
  }
  /* run partial atomic test */
  if (STL_SCH_RunRamTM(&StlRamStatus) != STL_OK)
  {
    FailSafe_Handler(TMR_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlRamStatus != STL_PARTIAL_PASSED)
  {
    if (StlRamStatus == STL_PASSED)
    {
      /* test completed successfully - to be restarted */
      StlRamStatus= STL_NOT_TESTED;
      #if defined(STL_EVAL_MODE)
        /* BSP_LED_Toggle(LED2); */
      #endif  /* STL_EVAL_MODE */
    }
    else
    {
      FailSafe_Handler(TMR_ERR_CODE);
    }
  }

#ifdef ARTI_FAILING_RAM_TM
  STL_SCH_StopArtifFailing();
#endif /* ARTI_FAILING_RAM_TM */

  /************/
  /* Clock TM */
  /************/
  /* When artificial failing is activated for clock module, the failure status is
     forced by the API module itself available as open source code */
  StlClkStatus = STL_NOT_TESTED;
  if (STL_RunClockTest(&StlClkStatus) != STL_OK)
  {
    FailSafe_Handler(CLK_ERR_CODE + DEF_PROG_OFFSET);
  }
  if (StlClkStatus != STL_PASSED)
  {
    FailSafe_Handler(CLK_ERR_CODE);
  }
}

/*----------------------------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* STM32H5xx HAL library initialization:
  - Configure the Flash prefetch
  - Configure the Systick to generate an interrupt every 1 msec
  - Set NVIC Group Priority to 4
  - Low Level Initialization
  */
  HAL_Init();

  /* Configure the System clock to have a frequency of 250 MHz */
  SystemClock_Config();

#if defined(STL_EVAL_MODE)
  BSP_LED_Init(LED2);
#endif  /* STL_EVAL_MODE */

  /* Initialize STL_scheduler, clock test and set up flash & RAM global config structures: */
  STL_Init();

  /* Before main application loop, run tests of CPU, entire flash area, entire RAM area, and clock: */
  STL_RunFullTest();
  HAL_Delay(10);


/* Main infinite loop */
  while (1)
  {
    /* STL is executed step by step here. In each iteration, one section of
       RAM and flash, as well as CPU and clock are tested. There is no need
       to run all test modules together. The sequence of calling the test modules can
       be modified or split and run as separate segments and so tailored to a particular
       application needs. Memory areas under test can be changed dynamically, too. */
    STL_RunStep();

    /* application code continues here as a part of main loop */
    HAL_Delay(2);
    /* the delay simulates application task execution here, 2 ms is rather
       applied here to make the demo LED blinking visible. */
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
