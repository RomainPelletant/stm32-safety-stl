
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

---
pagetitle: STL_Single_tests Application Description
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="./_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12}

## <b>STL_Single_tests Application Description</b>

This application demonstrates STL implementation when TrustZone security is both activated (Option bit TZEN=1) and not activated (Option bit TZEN=0).

The Test Modules executed in this application are launched consecutively by the application with the use of single tests APIs.

The Flash subsets consist of 1 subset of several sections in 1 pass.

The RAM subsets consist of 1 subset of several sections in 1 pass (To cover discontinuous adressing RAM areas, at least one subset per discontinuous RAM area must be defined - refer to STL_Mul_Sub_Tests application)

Please pay attention to the following STL specific items when porting this application on a different target:

  - check ROM memory configuration in stl_user_param_template.c according to the device
  - check "backup_buffer_section" section is defined in the scatter file
  - check Flash & RAM configurations in the application main.c

The iplementation project is composed of three sub-projects:

Project xxxxx_NO_TZ is aplicable  when TrustZone security is not activated.

There are two subprojects when TrustZone security is activated:

  - one for the secure application part (xxxxx_TZ_S)
  - one for the non-secure application part (xxxxx_TZ_NS).

Please remember that on system with security enabled:

- the system always boots in secure and the secure application is responsible for
launching the non-secure application.
- the SystemInit() function in secure application sets up the SAU/IDAU, FPU and
secure/non-secure interrupts allocations defined in partition_stm32h563xx.h file.

This project shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories
into two halves:

 - The first half for the secure application and
 - The second half for the non-secure application.

User Option Bytes configuration:
Please note the internal Flash is fully secure by default in TZEN=B4 and User Option Bytes
SECWM1_PSTRT/SECWM1_PEND and SECWM2_PSTRT/SECWM2_PEND should be set according to the application
configuration. Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:

    - TZEN=B4
    - SECWM1_PSTRT=0x0  SECWM1_PEND=0x7F  meaning all 128 sectors of Bank1 set as secure
    - SECWM2_PSTRT=0x7F SECWM2_PEND=0x0   meaning all 128 sectors of Bank2 set as non_secure

All these projects are targeted to run on STM32H5xx device on boards from STMicroelectronics.

These examples configure the maximum system clock frequency at 250 Mhz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, STL

### <b>Directory contents</b>

 File                                                Content
 -------------------------------------------         ---------------------------
 Secure/Src/main.c                                   Secure Main program
 Secure/Src/secure_nsc.c                             Secure Non-secure callable functions
 Secure/Src/stm32h5xx_hal_msp.c                      Secure HAL MSP module
 Secure/Src/stm32h5xx_it.c                           Secure Interrupt handlers
 Secure/Src/system_stm32h5xx_s.c                     Secure STM32H5xx system clock configuration file
 Secure/Inc/main.h                                   Secure Main program header file
 Secure/Inc/partition_stm32h575xx.h                  STM32H5 Device System Configuration file
 Secure/Inc/stm32h5xx_hal_conf.h                     Secure HAL Configuration file
 Secure/Inc/stm32h5xx_it.h                           Secure Interrupt handlers header file
 Secure/Inc/stm32h5xx_nucleo_conf.h                  Secure Nucleo board Configuration file
 Secure_nsclib/secure_nsc.h                          Secure Non-Secure Callable (NSC) module header file
 NonSecure/Src/main.c                                Non-secure Main program
 NonSecure/Src/stm32h5xx_hal_msp.c                   Non-secure HAL MSP module
 NonSecure/Src/stm32h5xx_it.c                        Non-secure Interrupt handlers
 NonSecure/Src/system_stm32h5xx_ns.c                 Non-secure STM32H5xx system clock configuration file
 NonSecure/Inc/main.h                                Non-secure Main program header file
 NonSecure/Inc/stm32h5xx_hal_conf.h                  Non-secure HAL Configuration file
 NonSecure/Inc/stm32h5xx_it.h                        Non-secure Interrupt handlers header file
 NonSecure/Inc/stm32h5xx_nucleo_conf.h               Non-secure Nucleo board Configuration file
 NoTrustZone/Src/main.c                              Main program when Trust Zone is not enabled 
 NoTrustZone/Src/stm32h5xx_hal_msp.c                 HAL MSP module when Trust Zone is not enabled 
 NoTrustZone/Src/stm32h5xx_it.c                      Interrupt handlers when Trust Zone is not enabled 
 NoTrustZone/Src/system_stm32h5xx_s.c                STM32H5xx system clock configuration file when Trust Zone is not enabled 

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5xx devices.
  - This example has been tested with NUCLEO-H563ZI-Qboard and can be easily tailored to any other supported devices and development boards.
  - User must assure proper setings of option bytes corresponding to the selected project (TZEN and SECWM) and associated scatter files (with STM32CubeProgrammer tool).

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


<b>Test compilation and load when TrustZone is not enabled</b>

 Make sure that the CPU is configured with the security not enabled (TZEN=0) (option byte)

1. Open Multi-projects workspace at your tool chain 
2. Select No_TZ project as active
3. Compile the project (it compiles and execute the post-build command)
3. Load the image into the target
4. Execute the code and observe LED2 blinking to see the STL status:
    - LED2 toggling regularly: test is running OK
    - LED2 slowly flashes: test ERROR:
        - STL Tests FAILED if LED2 flashes once per 2 sec
        - STL Defense Programming if LED2 flashes once per 4 sec


<b>Test compilation and laod when TrustZone is enabled</b>

 Make sure that the system is configured with the security enable (TZEN=1) (option byte)
 At principle, compilation of the secure project must precede compilation of the nonsecure one. Order of load is not important but the project firstly loaded
 should perform full chip erase while second one should not perform any erase. To be able to debug both the secure and nonsecure code, 
 loading additional debug symbols of the complementary  project into the debugger is required.
 -In IAR, this is done by adding extra image to Download or for Debug info only in Project Options -> Debugger -> Images.
 -In Keil, a Debugger initialization file is created and selected in Project Options -> Debug
 -In STM32CubeIDE this is done by changing Debug or Run configuration, as explained in the following instructions:

For IAR:

   - Open your toolchain
   - Open Multi-projects workspace file Project.eww
   - Set the "xxxxx_S" as active application (Set as Active)
   - Rebuild xxxxx_S project
   - Set the "xxxxx_NS" as active application (Set as Active)
   - Rebuild xxxxx_NS project
   - Flash the non-secure binary with Project->Download->Download active application
     (this shall download the \\EWARM\\TZ_NS\\Exe\\xxxxx_NS.out to flash memory)
   - Set the "xxxxx_S" as active application (Set as Active)
   - Flash the secure binary with Project->Download->Download active application
     (this shall download the \\EWARM\\TZ_S\\Exe\\xxxxx_S.out to flash memory)
   - Run the example and observe LED2 blinking to see the STL status:


For MDK-ARM:

   - Open your toolchain
   - Open Multi-projects workspace file Project.uvmpw
   - Select the xxxxx_S project as Active Project (Set as Active Project)
   - Build xxxxx_S project
   - Select the xxxxx_NS project as Active Project (Set as Active Project)
   - Build xxxxx_NS project
   - Load the non-secure binary (F8)
     (this shall download the \\MDK-ARM\\TZ_NS\\Exe\\Project.axf to flash memory)
   - Select the xxxxx_S project as Active Project (Set as Active Project)
   - Load the secure binary (F8)
     (this shall download the \\MDK-ARM\\TZ_S\\Exe\\Project.axf to flash memory)
   - Run the example and observe LED2 blinking to see the STL status:


For STM32CubeIDE:

   - Open STM32CubeIDE
   - File > Import. Point to the STM32CubeIDE folder of the example project. Click Finish.
   - Build configuration: Set the same active build configuration: Debug (default) or Release for both projects Secure & NonSecure
   - Select and build the xxxxx_NS project, this will automatically trigger build of xxxxx_S project
   - Select the xxxxx_S project and select "Debug configuration" or "Run configuration" in function of the active build configuration
   - Double click on “STM32 Cortex-M C/C++ Application”
   - Select  “Startup” >  “Add” >
   - Select the xxxxx_NS project
   - Click Debug/Run to debug/run the example and observe LED2 blinking to see the STL status:

    - LED2 toggling regularly: test is running OK
    - LED2 slowly flashes: test ERROR:
        - STL Tests FAILED if LED2 flashes once per 2 sec
        - STL Defense Programming if LED2 flashes once per 4 sec

<b>information</b>

A warning related to the overwriting of the elf file may be displayed after link. This warning is normal and is highlighting that, after
the link, the Elf file is modified by the post-build script.

:::

:::
:::
:::


:::
:::

