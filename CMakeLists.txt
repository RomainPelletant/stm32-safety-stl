# Copyright (c) 2026 Sixtaupi
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_SAFETY_STM32_STL_G0)
	set(STM32_STL_G0_DIR ${CMAKE_CURRENT_LIST_DIR}/vendor/61508/g0)

	# Provider API consumed by applications and by the adapter itself.
	zephyr_include_directories(include)

	zephyr_library()
	zephyr_library_include_directories(
		${STM32_STL_G0_DIR}/Inc
	)
	zephyr_library_compile_definitions(STM32G071xx)
	zephyr_library_sources(
		src/safety_stm32_stl.c
		${STM32_STL_G0_DIR}/Src/stl_util.c
		${STM32_STL_G0_DIR}/Src/stl_user_param_template.c
	)
	zephyr_library_sources_ifdef(CONFIG_SAFETY_BOOT_TESTS src/startup.c)

	zephyr_linker_sources(RAM_SECTIONS SORT_KEY 0000_stm32_stl_backup
		src/ram-sections.ld)

	zephyr_library_import(stm32_safety_stl
		"${STM32_STL_G0_DIR}/Lib/STL_Lib.a")
endif()
