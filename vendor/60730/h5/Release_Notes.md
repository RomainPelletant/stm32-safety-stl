---
pagetitle: Release Notes for X-CUBE-CLASSB-H5 Package
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# <small>Release Notes for</small> <mark>X-CUBE-CLASSB-H5</mark>
# <small>Expansion package for STM32Cube</small>
Copyright &copy; 2023 STMicroelectronics\

[![ST logo](./_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>

# Purpose

The X-CUBE-CLASSB-H5 **Self-Test Library (STL)** is an application-independent software test library released by ST to implement a relevant subset of safety mechanisms required by the safety concepts applicable to microcontrollers of the STM32H5 Series.

The STL is compilation tool chain-agnostic, so it can be compiled by any standard C-compiler. STL is an autonomous software, which executes on application demand selected tests to detect HW issues, and reports the outcomes to the application. STL is delivered partly in object code (for the library itself) and partly in source code for the user interfaces definition and user parameters settings. 

The X-CUBE-CLASSB-H5 is built on top of STM32Cube software technology.

The X-CUBE-CLASSB-H5 software comes with example implementations running on NUCLEO-H563ZI.

X-CUBE-CLASSB-H5 software features:

- STL tests of the CPU Arm® core.
- STL tests of the Flash memory.
- STL tests of the RAM.

The figure below shows the overall architecture.

<center>
![architecture](_htmresc/STM32Cube_2020_safety_STL.png)
</center>

Here is the list of references to user documents:

-   [UM3065](https://www.st.com/resource/en/user_manual/dm00906547-.pdf) : Getting started with STM32CubeH5 for STM32H5  Series
-   [UM3132](https://www.st.com/resource/en/user_manual/dm00948527.pdf) : Description of STM32H5 HAL and low-layer drivers

:::

::: {.col-sm-12 .col-lg-8}
# Update History
::: {.collapse}
<input type="checkbox" id="collapse-section0" checked aria-hidden="true">
<label for="collapse-section0" aria-hidden="true">__V4.0.0 / 13-December-2023__</label>
<div>

## Main Changes

First release of Self-Test Library for STM32H5 series

## Contents

**Projects** information

The X-CUBE-CLASSB-H5 package:


  - comes with a set of Applications running on STMicroelectronics board, STM32H563ZI-Nucleo
  - offers full scope of Applications ported on 3 toolchains


  Name          Version        Release note
  ---------     --------       --------------------------------------------------------------------------------
  Projects      V4.0.0         [release note](Projects\Release_Notes.html)

## Components
<small>The components flagged by "[]{.icon-st-update}" have changed since the
previous release. "[]{.icon-st-add}" are new.</small>

**Drivers**

  Name                                    Version        Release note
  --------------------------------------- -------------  ------------------------------------------------------------------------------------------------------------------------------------------------
  Cortex®-M CMSIS                         V5.6.0_cm33    [release note](Drivers/CMSIS/docs/General/html/index.html)
  STM32H5xx CMSIS                         V1.0.0         [release note](Drivers/CMSIS/Device/ST/STM32H5xx/Release_Notes.html)
  BSP STM32H5xx_Nucleo                    V1.0.0         [release note](Drivers/BSP/STM32H5xx_Nucleo/Release_Notes.html)
  STM32H5 HAL/LL APIs                     V1.0.0         [release note](Drivers/STM32H5xx_HAL_Driver/Release_Notes.html)
													    
**Middlewares**

  Name                                    Version        Release note
  --------------------------------------- -------------  ------------------------------------------------------------------------------------------------------------------------------------------------
  STM32 safety (source code and library)  V4.0.0.        [release note](Middlewares/ST/STM32_Safety_STL/Release_Notes.html)

## Documents
<small>The components flagged by "[]{.icon-st-update}" have changed since the
previous release. "[]{.icon-st-add}" are new.</small>

  Name                                      Document
  ------------------------------            -----------------------------------------------------------------------------------------------------------------------
  UM3267                                    STL user guide
  
\

## Known Limitations

None


## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V9.40.1 + ST-Link
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.38 + ST-Link
- STM32CubeIDE V1.13.2 using GCC11.1


## Supported Devices and boards

-   NUCLEO-H563ZI REV B (MB1404 B-02)


</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on STM32H5xx, visit: [www.st.com/stm32h5](http://www.st.com/stm32h5); 
[www.st.com/STM32](https://www.st.com/en/microcontrollers-microprocessors.html)

This release note uses up to date web standards and, for this reason, should not be opened with Internet Explorer
but preferably with popular browsers such as Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>

