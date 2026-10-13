/* Copyright (c) 2026 Sixtaupi */
/* SPDX-License-Identifier: Apache-2.0 */

#include <safety/providers/stm32_stl.h>
#include <safety/startup.h>

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM1)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm1_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM1],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM1L)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm1l_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM1L],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM2)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm2_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM2],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM3)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm3_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM3],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM4)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm4_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM4],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM5)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm5_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM5],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM6)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm6_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM6],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM7)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm7_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM7],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM7L)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm7l_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM7L],
	SAFETY_POLICY_RELAXED);
#endif

#if defined(CONFIG_SAFETY_BOOT_STM32_STL_CPU_TM8)
SAFETY_BOOT_TEST_DEFINE(stm32_stl_cpu_tm8_boot,
	&safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TM8],
	SAFETY_POLICY_RELAXED);
#endif
