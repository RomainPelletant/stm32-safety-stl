---
pagetitle: STL_Single_tests Application Description
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="../../../../_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12}

## <b>STL_Single_tests Application Description</b>

This application demonstrates possible execution of all tests included at the STL at both startup and run time condition.

The Test Modules executed in this application are launched consecutively by the application with the use of single tests APIs.

The Flash subsets consist of 1 subset of several sections tested by one shot overall test at startup and split into series of single atomic steps at each partial run time test.

The RAM subsets consist of 1 subset of several sections tested by one shot at startup overall test and split into series of single atomic steps at each partial run time step test. To cover discontinuous adressing RAM areas, at least one subset per discontinuous RAM area must be defined.

Please pay attention to the following STL specific items when porting this application on a different target:

  - check ROM memory configuration in stl_user_param_template.c according to the device
  - check "backup_buffer_section" section is defined in the scatter file
  - check Flash & RAM configurations in the application main.c

For more details, refer to Safety documents.

### <b>Keywords</b>

System, STL

### <b>Directory contents</b>

 File                                                Content
 -------------------------------------------------   ------------------------
 STL_Single_Tests/Src/system_stm32g0xx.c             STM32G0xx system source file
 STL_Single_Tests/Src/main.c                         main application file
 STL_Single_Tests/Src/stm32g0xx_hal_msp.c            HAL MSP module
 STL_Single_Tests/Src/stm32g0xx_it.c                 STM32 interrupt handlers
 STL_Single_Tests/Inc/main.h                         header for main.c
 STL_Single_Tests/Inc/stm32g0xx_hal_conf.h           HAL configuration file
 STL_Single_Tests/Inc/stm32g0xx_it.h                 header for stm32g0xx_it.c

### <b>Hardware and Software environment</b>

  - This example runs on STM32G0xx devices.
  - This example has been tested with STM32G071RB-Nucleo Rev C board and can be
    easily tailored to any other supported devices and development boards.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

<b>Install ST CRC tool</b>

ST provides a CRC tool used for Flash testing, but user can use his own CRC tools.
ST CRC tool install procedure:

- Select on ST web-site STM32CubeProgrammer
- Install the package.

The easiest is to add the tool path inside host environment variable (need computer administration right).
If not, you need to update the post build project command line with the tool path:

  With IAR IDE:

    In Project, Option, Build Actions:
    The current post-build command line is:
      STM32_Programmer_CLI.exe -sl "path/of/Project.out" FlashStart@ FlashEnd@ FlashSectionSize

  With MDK-ARM KEIL IDE:

    In Project, Option, User:
    The current After Build/Rebuild command line is:
      STM32_Programmer_CLI.exe  -sl  "Path\Project.axf" FlashStart@ FlashEnd@ FlashSectionSize


  With ST32CubeIDE:
  
    In Project, Settings, Build steps:
	The current Pre-Build steps command line is:
      rm -f ${BuildArtifactFileBaseName}.elf
    The current Post-Build steps command line is:
      arm-none-eabi-objcopy -O binary "${BuildArtifactFileBaseName}.elf" "${BuildArtifactFileBaseName}.bin" && arm-none-eabi-size "${BuildArtifactFileName}" && echo  "\"STM32_Programmer_CLI.exe\" -sl \"${CWD}\\${BuildArtifactFileName}\" FlashStart@ FlashEnd@ FlashSectionSize " > call_prg.bat && "${CWD}\call_prg.bat"

  => adapt the STM32_Programmer_CLI.exe path


<b>Test execution</b>

1. Compile the project
It compiles and execute the post-build command

2. load the image into the target

3. Execute the code
- LED4 toggles regularly : all the tests are executed correctly (each toggle signalizes flash test completion)
- LED4 single short flash appears once per 2 seconds: one of the tests FAILED (FailSafe_Handler keeps fail code)
- LED4 single short flash appears once per 4 seconds: one of the tests FAILED by defense programming 

<b>information</b>

A warning related to the overwriting of the elf file may be displayed after link. This warning is normal and is highlighting that, after
the link, the Elf file is modified by the post-build script.

:::

:::