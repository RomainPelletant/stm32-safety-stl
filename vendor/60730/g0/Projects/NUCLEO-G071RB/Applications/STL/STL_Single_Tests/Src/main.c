/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   main body test scenario
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
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define FLASH_SECTION_SIZE 1024U
#define RAM_SECTION_SIZE 128U
#ifdef STL_DISABLE_RAM_BCKUP_BUF
#define RAM_BACKUP_BUFFER_SIZE 0
#else
#define RAM_BACKUP_BUFFER_SIZE 32
#endif
#define TEST_RAM_START_ADDR (0x20000000U + RAM_BACKUP_BUFFER_SIZE)
#define TEST_ROM_START_ADDR 0x08000000U
/* Change this value to reflect actual usage of RAM by the application or device RAM size */
#define TEST_RAM_SECTION_NB_RUN 8    /* only 8 RAM sections are tested during runtime. */

/* During initial full test, the entire RAM is tested: */
#define TEST_RAM_END_ADDR_FULL 0x20008fff
#warning "please check if end address of RAM matches with your product"

/* Toolchain-dependent definitions of TEST_FLASH_END_OF_LAST_SECTION to define the end of the program in flash */
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
/* All flash content has been placed in FIXED_ORDER_ROM. Its size is added to flash base address to calculate the address of its end. */
/* In linker-script, block FIXED_ORDER_ROM has been defined to contain all content of flash. */
/* It is placed at the ROM origin and its length is used to calculate the address of its last byte. */
#pragma section = "FIXED_ORDER_ROM"
#define TEST_ROM_END_ADDR ( (uint32_t) TEST_ROM_START_ADDR + __section_size("FIXED_ORDER_ROM") -1)
#endif /* __ICCARM__ */

/* Value of the system clock frequency at run time in Hz */
#define SYSTCLK_AT_RUN (uint32_t)(64000000uL)
/* Value of the Internal LSI oscillator in Hz */
#define LSI_Freq    ((uint32_t)32000uL)
/* CLK frequency above this limit considered as harmonics at case of HSE */
#define CLK_LimitHigh(fcy) ((uint32_t)(((fcy)/LSI_Freq)*8u*5u)/4u) /* (Value + 25%) */
/* CLK frequency below this limit considered as sub-harmonics at case of HSE */
#define CLK_LimitLow(fcy) ((uint32_t)(((fcy)/LSI_Freq)*8u*3u)/4u)  /* (Value - 25%) */
/* if SYSCK is derived from HSI, the upper fixed clock limits can be set more severe 
   e.g., ...*8u*6u)/5 resp ...*8u*4u)/5 (~ value +/- 20%) but such a severe restriction
   can require including some adaptive flow to compensate possible temperature drift
   of HSI by making acceptable window defined by these limits variable in time */ 

/* uncomment next lines when simulate artificial failing of the modules */
/* #define ARTI_FAILING_CPU_TM   */
/* #define ARTI_FAILING_FLASH_TM */
/* #define ARTI_FAILING_RAM_TM   */
/* #define ARTI_FAILING_CLK_TM   */


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

/* next variables has to be static due to separation of configuration and run procedures */
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
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 64000000
  *            HCLK(Hz)                       = 64000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            Flash Latency(WS)              = 2
  *            PLLM                           = 1
  *            PLLN                           = 8
  *            PLLP                           = 2
  *            PLLQ                           = 2
  *            PLLR                           = 2
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable HSI oscillator, select it as PLL source and finally activate the PLL */
  /* Note> When switching to HSE select HSI as system clock source anyway at firs here 
     to allow modification of the PLL configuration */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv         = RCC_HSI_DIV1;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM       = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN       = 8;
  RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ       = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR       = RCC_PLLR_DIV2;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    /* Initialization Error */
    FailSafe_Handler(HAL_ERR_CODE);
  }

  /* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
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
  TIM_HandleTypeDef  tim_capture_handle;
  TIM_IC_InitTypeDef tim_input_config;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  
  __PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  /* Configure LSI as RTC clock soucre */
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    return (STL_KO);
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    return (STL_KO);
  }
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
  
  /* TIM16 Peripheral clock enable */
   __TIM16_CLK_ENABLE();
  
  /* Configure the NVIC for TIM16 */
  HAL_NVIC_SetPriority(TIM16_IRQn, 3u, 0u);
  
  /* Enable the TIM16 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM16_IRQn);

  /* TIM16 configuration: Input Capture mode ---------------------
  The Rising edge is used as active edge, ICC input divided by 8
  The TIM16 CCR1 is used to compute the frequency value. 
  ------------------------------------------------------------ */
  tim_capture_handle.Instance = TIM16;
  tim_capture_handle.Init.Prescaler         = 0u; 
  tim_capture_handle.Init.CounterMode       = TIM_COUNTERMODE_UP;  
  tim_capture_handle.Init.Period            = 0xFFFF; 
  tim_capture_handle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;     
  tim_capture_handle.Init.RepetitionCounter = 0u; 
  tim_capture_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; 
  
  
  /* define internal HAL driver status here as handle structure is defined locally */
  __HAL_RESET_HANDLE_STATE(&tim_capture_handle);
  if(HAL_TIM_IC_Init(&tim_capture_handle) != HAL_OK)
  {
    /* Initialization Error */
    return (STL_KO);
  }

  /* Configure the TIM16 Input Capture of channel 1 */
  tim_input_config.ICPolarity  = TIM_ICPOLARITY_RISING;
  tim_input_config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  tim_input_config.ICPrescaler = TIM_ICPSC_DIV8;
  tim_input_config.ICFilter    = 0u;
  if(HAL_TIM_IC_ConfigChannel(&tim_capture_handle, &tim_input_config, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Initialization Error */
    return (STL_KO);
  }
  
  HAL_TIMEx_TISelection(&tim_capture_handle, TIM_TIM16_TI1_LSI, TIM_CHANNEL_1);
  
  /* Reset the flags */
  tim_capture_handle.Instance->SR = 0u;
  LSIPeriodFlag = 0u;
  
  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&tim_capture_handle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Initialization Error */
    return (STL_KO);
  }

  return (STL_OK);
}
/*----------------------------------------------------------------------------*/
/**
  * @brief  Verification result of the cross check frequencies process
  * @param  : None
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
      *clk_sts = STL_FAILED;	/* Sub-harmonics: HSE -25% below expected */
    }
    if (PeriodValue > CLK_LimitHigh(SYSTCLK_AT_RUN))
    {
      *clk_sts = STL_FAILED;	/* Harmonics: HSE +25% above expected */
    }
  #else
    *clk_sts = STL_FAILED;
  #endif /* ARTI_FAILING_CLK_TM */
    
    /* clear flag here to ensure refreshed LSI measurement result will be taken at next check */
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
void My_TIM16_IRQHandler(void)
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
      /* take correct measurement only */
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
        /* ignore computation in case of IC overflow */
        TIM16->SR &= (uint16_t)(~TIM_SR_CC1OF);
      }
    }
    /* ignore computation in case data is not required */
  }
}
/*----------------------------------------------------------------------------*/
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  Code of the failure detected
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
      BSP_LED_On(LED4);
      HAL_Delay(100);
      BSP_LED_Off(LED4);
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
     This function executes all the provided CPU, flash, RAM, and clock test modules at this order. Test 
     of both the memories is performed in one shot in their entirety here. The function should be called
     prior to the main application entry, but can also be re-called by the application at any later point.
     Configuration structures for handling tests of the memories are declared locally, therefore they
     cannot be referenced outside of the function which is supposed to be executed as a whole.
  */

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


  /*********************/
  /* FLASH Test Module */
  /*********************/

#ifdef ARTI_FAILING_FLASH_TM
    /* forced STL_FAILED status simulates the TM configuration error and
       STL_NOT_TESTED status simulates TM run error */
    ArtifFailing.FlashTmStatus = STL_FAILED;
  STL_SCH_StartArtifFailing(&ArtifFailing);
#endif /* ARTI_FAILING_FLASH_TM */

  /* The following configuration tests the entire program in the flash, in one shot */
  STL_MemSubset_t FlashSubsetFullTest = {
      .StartAddr = TEST_ROM_START_ADDR,
      .EndAddr   = TEST_ROM_END_ADDR,
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
  else if (StlFlashStatus != STL_PASSED){
    FailSafe_Handler(TMF_ERR_CODE);
  }
  StlFlashStatus = STL_NOT_TESTED;

#ifdef ARTI_FAILING_FLASH_TM
  STL_SCH_StopArtifFailing();
#endif /* ARTI_FAILING_FLASH_TM */


  /********************/
  /* RAM Test module  */
  /********************/

#ifdef ARTI_FAILING_RAM_TM
    /* forced STL_FAILED status simulates the TM configuration error and
       STL_NOT_TESTED status simulates TM run error */
    ArtifFailing.RamTmStatus = STL_FAILED;
  STL_SCH_StartArtifFailing(&ArtifFailing);
#endif /* ARTI_FAILING_RAM_TM */

  /* The following configuration tests the entire user-defined range of RAM at once */
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

#ifdef ARTI_FAILING_RAM_TM
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
      /* test completed successfuly - to be restarted */
      StlFlashStatus= STL_NOT_TESTED;
      #if defined(STL_EVAL_MODE)
        BSP_LED_Toggle(LED4);
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
    /* Init & configure Flash TM */
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
        /* BSP_LED_Toggle(LED4); */
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
  * @brief  main body
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32 HAL library initialization:
  - Configure the Flash prefetch
  - Configure the Systick to generate an interrupt each 1 msec
  - Set NVIC Group Priority to 3
  - Low Level Initialization
  */
  HAL_Init();

  /* Configure the System clock to have a frequency of 64 MHz */
  SystemClock_Config();

#if defined(STL_EVAL_MODE)
  BSP_LED_Init(LED4);
#endif  /* STL_EVAL_MODE */

  /* Initialize STL_scheduler, clock test and set up flash & RAM global config structures: */
  STL_Init();

  /* Before main application loop, run tests of CPU, entire flash area, entire RAM area, and clock: */
  STL_RunFullTest();

      
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
    HAL_Delay(5); 
    /* the delay simulates application task execution here, 5 ms is rather
       applied here to make the demo LED blinking visible. */
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
