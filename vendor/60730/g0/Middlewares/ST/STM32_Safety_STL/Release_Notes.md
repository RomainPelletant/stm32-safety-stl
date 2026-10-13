---
pagetitle: Release Notes for STM32G0 Safety Self Test Library component (STL)
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# <small>Release Notes for</small> <mark>Safety Self Test Library (STL)</mark>
Copyright &copy; 2022 STMicroelectronics\

[![ST logo](./_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>


# Purpose

The STL is the main component of the X-CUBE-CLASSB-G0 **Self-Test Library (STL)** Expansion package for STM32Cube.

The **STL** is an application-independent software test library released by ST to implement a relevant subset of safety mechanisms
applicable to generic parts of the STM32G0 Series microcontrollers required by the safety standard targeting inhouse appliances.
The STL is compilation tool chain-agnostic, so it can be compiled by any standard C-compiler. STL is an autonomous software which
executes on application demand selected tests to detect HW issues and reports the outcomes to the application. STL is delivered
partly in object code (for the library itself) and partly in source code for the user interfaces definition and user parameters settings.
The X-CUBE-CLASSB-G0 is built on top of STM32Cube software technology and folow principles derived from another ST safety library
dedicated for industrial applieances (X-CUBE-STL).

__Architecture Overview__

The STL tests the Arm® Cortex® M0+ CPU core, the Flash memory and the RAM.

As shown in Figure 1. STL architecture, a system architecture with an end-user application integrating the STL is composed of:

- User application (in light blue)

- User parameters (in light blue)

- STL scheduler (in light yellow): directly accessible by the User application via User APIs (not going through HAL/LL)

- STL internal test modules (in light yellow): called by the STL scheduler (not seen from the User application).


The figure below shows the STL architecture:

<center>
![STL Architecture](_htmresc/STL_Architecture.PNG)\
</center>

Here is the list of references to user documents:

- [UM3083](???.pdf):STM32G0 Series UL/CSA/IEC 60730-1/60335-1 Class B self-test library user guide
:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section2" checked aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">__V4.0.0 / 23-December-2022__</label>
<div>

## Main Changes

**First release**

## Integrity

- SHA256 function (specified in FIPS-180 publications) is used to check STL delivery integrity. It expected value is
  
<small>The components flagged by "[]{.icon-st-update}" have changed since the
previous release. "[]{.icon-st-add}" are new.</small>

__STL library file version: v4.0.0__

File name                                SHA256
---------------------------------------- ----------------------------------------------------------------
*\*./Lib/STL_Lib.a*                      e423d8af443e412608e0b50b14446a93f152d1dfb79fc698192db17e95f30c81



## Known Limitations

- None

## Backward Compatibility

- Not applicable


## Development Toolchains and Compilers for the library

- IAR Embedded Workbench for ARM toolchain V9.20.1 - more information in User Guide


</div>
:::


:::
:::

<footer class="sticky">
For complete documentation on **STM32G0xx**, visit: [www.st.com/stm32g0](http://www.st.com/stm32g0)
visit: [www.st.com/STM32](https://www.st.com/en/microcontrollers-microprocessors.html)

This release note uses up to date web standards and, for this reason, should not be opened with Internet Explorer
but preferably with popular browsers such as Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>

