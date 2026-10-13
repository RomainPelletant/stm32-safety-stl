/* Copyright (c) 2026 Sixtaupi */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef SAFETY_PROVIDERS_STM32_STL_H_
#define SAFETY_PROVIDERS_STM32_STL_H_

#include <stddef.h>
#include <stdint.h>

#include <safety/provider.h>
#include <safety/test.h>

#ifdef __cplusplus
extern "C" {
#endif

/** IDs preserve the STL_CpuTmxIndex_t values from X-CUBE-STL-G0. */
enum safety_stm32_stl_cpu_test_id {
	SAFETY_STM32_STL_CPU_TM1 = 0,
	SAFETY_STM32_STL_CPU_TM1L,
	SAFETY_STM32_STL_CPU_TM2,
	SAFETY_STM32_STL_CPU_TM3,
	SAFETY_STM32_STL_CPU_TM4,
	SAFETY_STM32_STL_CPU_TM5,
	SAFETY_STM32_STL_CPU_TM6,
	SAFETY_STM32_STL_CPU_TM7,
	SAFETY_STM32_STL_CPU_TM7L,
	SAFETY_STM32_STL_CPU_TM8,
	SAFETY_STM32_STL_CPU_TEST_COUNT,
};

enum safety_stm32_stl_memory_test_id {
	SAFETY_STM32_STL_FLASH_TM = 0x100U,
	SAFETY_STM32_STL_RAM_TM,
};

extern const struct safety_provider safety_stm32_stl_provider;
extern const struct safety_test
	safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TEST_COUNT];
extern const struct safety_test_api safety_stm32_stl_ram_api;
extern const struct safety_test_api safety_stm32_stl_flash_api;

/*
 * Linker bounds of the first 128-byte reserved section. It contains the
 * vendor-required 32-byte backup buffer plus alignment padding.
 */
extern uint8_t __stm32_stl_ram_backup_start[];
extern uint8_t __stm32_stl_ram_backup_end[];

/** Product-owned RAM region passed to the X-CUBE-STL-G0 RAM diagnostic. */
struct safety_stm32_stl_ram_config {
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t atomic_sections;
};

/** Product-owned Flash range passed to the X-CUBE-STL-G0 diagnostic. */
struct safety_stm32_stl_flash_config {
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t atomic_sections;
};

/** Define a configurable ST RAM diagnostic usable with safety_execute(). */
#define SAFETY_STM32_STL_RAM_TEST_INITIALIZER(test_name, config_ptr)          \
	{                                                                      \
		.name = (test_name),                                              \
		.provider = &safety_stm32_stl_provider,                           \
		.provider_test_id = SAFETY_STM32_STL_RAM_TM,                      \
		.api = &safety_stm32_stl_ram_api,                                 \
		.capabilities = 0U,                                               \
		.wcet_us = 0U,                                                    \
		.max_irq_mask_us = 0U,                                            \
		.config = (config_ptr),                                           \
		.data = NULL,                                                     \
	}

/** Define a configurable ST Flash diagnostic usable with safety_execute(). */
#define SAFETY_STM32_STL_FLASH_TEST_INITIALIZER(test_name, config_ptr)        \
	{                                                                      \
		.name = (test_name),                                              \
		.provider = &safety_stm32_stl_provider,                           \
		.provider_test_id = SAFETY_STM32_STL_FLASH_TM,                    \
		.api = &safety_stm32_stl_flash_api,                               \
		.capabilities = 0U,                                               \
		.wcet_us = 0U,                                                    \
		.max_irq_mask_us = 0U,                                            \
		.config = (config_ptr),                                           \
		.data = NULL,                                                     \
	}

/**
 * Run a complete ST RAM diagnostic cycle on one inclusive address range.
 *
 * The caller owns selection of a safe/testable range. It must exclude the
 * STL backup buffer and every region that cannot tolerate the vendor
 * diagnostic's temporary destructive accesses. The range is split into
 * independent chunks of at most atomic_sections 128-byte STL sections. The
 * last chunk is shortened when necessary.
 */
int safety_stm32_stl_ram_run_complete(
	const struct safety_stm32_stl_ram_config *config,
	enum safety_test_result *result);

/**
 * Run a complete ST Flash diagnostic cycle on one inclusive address range.
 *
 * An ST Flash section is 1024 bytes. start_addr must be section-aligned and
 * end_addr + 1 must be word-aligned. The X-CUBE-STL reference CRC table must
 * already be present at the end of the physical Flash, as required by the
 * vendor integration procedure. That table must not be part of the range.
 */
int safety_stm32_stl_flash_run_complete(
	const struct safety_stm32_stl_flash_config *config,
	enum safety_test_result *result);

#ifdef __cplusplus
}
#endif

#endif /* SAFETY_PROVIDERS_STM32_STL_H_ */
