---
pagetitle: Release Notes for STM32H5 Safety Self Test Library (STL)
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# <small>Release Notes for</small> <mark>Safety Self Test Library (STL)</mark>
Copyright &copy; 2023 STMicroelectronics\

[![ST logo](./_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>


# Purpose

The STL is the main component of the X-CUBE-CLASSB-H5 expansion package for STM32Cube.

STL is an application-independent software test library released by ST to implement a relevant subset of safety mechanisms applicable to generic parts of Arm® Cortex® M33 CPU included in the STM32H5 Series microcontrollers required by the safety standard targeting inhouse appliances.
The STL is HAL/LL independent and compilation tool chain-agnostic, so it can be compiled by any standard C-compiler.
STL is an autonomous software, which executes on application demand selected tests to detect hardware issues,
and reports the outcomes to the application.

STL is delivered partly in object code (for the library itself) and partly in source code for the user interfaces
definition and user parameters settings.

__Architecture Overview__

The STL tests the Arm® Cortex® M33 CPU core, the Flash memory and the RAM.

As shown in Figure 1. STL architecture, a system architecture with an end-user application integrating the STL is composed of:

- User application (in light blue)

- User parameters (in light blue)

- STL scheduler (in light yellow): directly accessible by the User application via User APIs (not going through HAL/LL)

- STL internal test modules (in light yellow): called by the STL scheduler (not seen from the User application).

The figure below shows the STL architecture:

<center>
![STL Architecture](_htmresc/STL_Architecture_CM33.PNG)\
</center>

The STL reference document is:

-   UM3267 : STM32H5 Series UL/CSA/IEC 60730-1/60335-1 Class B self-test library user guide

:::

::: {.col-sm-12 .col-lg-8}
# Update History
::: {.collapse}
<input type="checkbox" id="collapse-section0" checked aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__V4.0.0 / 13-12-2023__</label>
<div>

## Main Changes

**First release of a new STL delivered at object format**


## Integrity

- SHA256 function (specified in FIPS-180 publications) is used to check STL delivery integrity

- SHA256 expected values:

The STL library file, STL_Lib.a, could have a different version than the other files delivered in source, that have all the same version.

<small>The components flagged by "[]{.icon-st-update}" have changed since the
previous release. "[]{.icon-st-add}" are new.</small>

__STL library file version: v4.0.0__

File name                                SHA256
---------------------------------------- ----------------------------------------------------------------
*\*./Lib/STL_Lib.a*                      08d70bf9b866cb0f7df453a0b068824d1244082a928041ec95a2b9b482c7d6c7

  
## Known Limitations

- None

## Backward Compatibility

- Not applicable as 1st version


## Development Toolchains and Compilers for the library

- IAR Embedded Workbench for ARM toolchain V9.30.1 - more information in User Guide


</div>
:::


:::
:::

<footer class="sticky">
For complete documentation on **STM32H5xx**, visit: [[www.st.com/stm32h5](http://www.st.com/stm32h5)]
visit: [www.st.com/STM32](https://www.st.com/en/microcontrollers-microprocessors.html)

This release note uses up to date web standards and, for this reason, should not be opened with Internet Explorer
but preferably with popular browsers such as Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>

