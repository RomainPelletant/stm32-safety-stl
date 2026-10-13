/*
 * Copyright (c) 2026 Sixtaupi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>

#include <zephyr/sys/atomic.h>

#include <safety/providers/stm32_stl.h>

#include <stl_user_api.h>

typedef STL_Status_t (*stm32_stl_run_fn_t)(STL_TmStatus_t *status);

struct stm32_stl_cpu_config {
	stm32_stl_run_fn_t run;
};

enum stm32_stl_init_state {
	STM32_STL_UNINITIALIZED,
	STM32_STL_INITIALIZING,
	STM32_STL_INITIALIZED,
};

static atomic_t init_state;

/* X-CUBE-STL-G0 RAM TM section size used by the supplied integration. */
#define STM32_STL_RAM_SECTION_SIZE 128U
#define STM32_STL_FLASH_SECTION_SIZE 1024U

/* Configured by the customizable X-CUBE-STL user-parameter translation unit. */
extern uint32_t STL_RomStart;
extern uint32_t STL_RomEnd;

_Static_assert((int)SAFETY_STM32_STL_CPU_TEST_COUNT == (int)STL_CPU_TM_MAX,
	       "STL CPU test identifiers no longer match the vendor API");

static int stm32_stl_init_library(void)
{
	STL_Status_t status;

	if (atomic_get(&init_state) == STM32_STL_INITIALIZED) {
		return 0;
	}
	if (!atomic_cas(&init_state, STM32_STL_UNINITIALIZED,
			STM32_STL_INITIALIZING)) {
		return -EBUSY;
	}

	status = STL_SCH_Init();
	if (status != STL_OK) {
		atomic_set(&init_state, STM32_STL_UNINITIALIZED);
		return -EIO;
	}

	atomic_set(&init_state, STM32_STL_INITIALIZED);
	return 0;
}

static int stm32_stl_init(const struct safety_test *test)
{
	if (test == NULL) {
		return -EINVAL;
	}

	return stm32_stl_init_library();
}

static int stm32_stl_map_result(STL_TmStatus_t stl_result,
				enum safety_test_result *result)
{
	switch (stl_result) {
	case STL_PASSED:
		*result = SAFETY_TEST_PASSED;
		return 0;
	case STL_PARTIAL_PASSED:
		*result = SAFETY_TEST_IN_PROGRESS;
		return 0;
	case STL_FAILED:
		*result = SAFETY_TEST_FAILED;
		return 0;
	case STL_NOT_TESTED:
		*result = SAFETY_TEST_NOT_RUN;
		return 0;
	case STL_ERROR:
		*result = SAFETY_TEST_ERROR;
		return 0;
	default:
		*result = SAFETY_TEST_ERROR;
		return -EPROTO;
	}
}

int safety_stm32_stl_ram_run_complete(
	const struct safety_stm32_stl_ram_config *config,
	enum safety_test_result *result)
{
	STL_MemSubset_t subset;
	STL_MemConfig_t ram_config;
	STL_TmStatus_t stl_result = STL_ERROR;
	uint32_t current_addr;
	uint32_t region_size;
	uint32_t remaining_sections;
	uint32_t chunk_sections;
	STL_Status_t status;
	int ret;

	if ((config == NULL) || (result == NULL) ||
	    (config->start_addr > config->end_addr) ||
	    (config->atomic_sections == 0U)) {
		return -EINVAL;
	}

	region_size = config->end_addr - config->start_addr + 1U;
	if (((config->start_addr % sizeof(uint32_t)) != 0U) ||
	    ((region_size % STM32_STL_RAM_SECTION_SIZE) != 0U)) {
		return -EINVAL;
	}

	*result = SAFETY_TEST_ERROR;
	ret = stm32_stl_init_library();
	if (ret != 0) {
		return ret;
	}

	/*
	 * Configure one atomic chunk at a time to reproduce the supplied,
	 * validated X-CUBE-STL-G0 integration sequence. This also gives every
	 * STL invocation an exact subset matching NumSectionsAtomic.
	 */
	current_addr = config->start_addr;
	remaining_sections = region_size / STM32_STL_RAM_SECTION_SIZE;

	while (remaining_sections != 0U) {
		chunk_sections = remaining_sections;
		if (chunk_sections > config->atomic_sections) {
			chunk_sections = config->atomic_sections;
		}

		status = STL_SCH_InitRam(&stl_result);
		if ((status != STL_OK) || (stl_result != STL_NOT_TESTED)) {
			return -EIO;
		}

		subset = (STL_MemSubset_t) {
			.StartAddr = current_addr,
			.EndAddr = current_addr +
				(chunk_sections * STM32_STL_RAM_SECTION_SIZE) - 1U,
			.pNext = NULL,
		};
		ram_config = (STL_MemConfig_t) {
			.pSubset = &subset,
			.NumSectionsAtomic = chunk_sections,
		};

		status = STL_SCH_ConfigureRam(&stl_result, &ram_config);
		if ((status != STL_OK) || (stl_result != STL_NOT_TESTED)) {
			return -EIO;
		}

		status = STL_SCH_RunRamTM(&stl_result);
		if (status != STL_OK) {
			*result = SAFETY_TEST_ERROR;
			return -EIO;
		}

		ret = stm32_stl_map_result(stl_result, result);
		if (ret != 0) {
			return ret;
		}
		if (*result != SAFETY_TEST_PASSED) {
			return 0;
		}

		remaining_sections -= chunk_sections;
		current_addr = subset.EndAddr + 1U;
	}

	return 0;
}

int safety_stm32_stl_flash_run_complete(
	const struct safety_stm32_stl_flash_config *config,
	enum safety_test_result *result)
{
	STL_MemSubset_t subset;
	STL_MemConfig_t flash_config;
	STL_TmStatus_t stl_result = STL_ERROR;
	uint32_t region_size;
	uint32_t sections;
	uint32_t maximum_runs;
	uint32_t reference_table_size;
	uint32_t reference_table_start;
	STL_Status_t status;
	int ret;

	if ((config == NULL) || (result == NULL) ||
	    (config->start_addr > config->end_addr) ||
	    (config->atomic_sections == 0U)) {
		return -EINVAL;
	}

	region_size = config->end_addr - config->start_addr + 1U;
	reference_table_size = (STL_RomEnd - STL_RomStart + 1U) /
			       (STM32_STL_FLASH_SECTION_SIZE / sizeof(uint32_t));
	reference_table_start = STL_RomEnd - reference_table_size + 1U;

	if ((config->start_addr < STL_RomStart) ||
	    (config->end_addr >= reference_table_start) ||
	    (((config->start_addr - STL_RomStart) %
	      STM32_STL_FLASH_SECTION_SIZE) != 0U) ||
	    ((region_size % sizeof(uint32_t)) != 0U)) {
		return -EINVAL;
	}

	*result = SAFETY_TEST_ERROR;
	ret = stm32_stl_init_library();
	if (ret != 0) {
		return ret;
	}

	status = STL_SCH_InitFlash(&stl_result);
	if ((status != STL_OK) || (stl_result != STL_NOT_TESTED)) {
		return -EIO;
	}

	subset = (STL_MemSubset_t) {
		.StartAddr = config->start_addr,
		.EndAddr = config->end_addr,
		.pNext = NULL,
	};
	flash_config = (STL_MemConfig_t) {
		.pSubset = &subset,
		.NumSectionsAtomic = config->atomic_sections,
	};

	status = STL_SCH_ConfigureFlash(&stl_result, &flash_config);
	if ((status != STL_OK) || (stl_result != STL_NOT_TESTED)) {
		return -EIO;
	}

	sections = region_size / STM32_STL_FLASH_SECTION_SIZE;
	if ((region_size % STM32_STL_FLASH_SECTION_SIZE) != 0U) {
		sections++;
	}
	maximum_runs = sections / config->atomic_sections;
	if ((sections % config->atomic_sections) != 0U) {
		maximum_runs++;
	}

	for (uint32_t run = 0U; run < maximum_runs; run++) {
		status = STL_SCH_RunFlashTM(&stl_result);
		if (status != STL_OK) {
			*result = SAFETY_TEST_ERROR;
			return -EIO;
		}

		ret = stm32_stl_map_result(stl_result, result);
		if (ret != 0) {
			return ret;
		}
		if (*result != SAFETY_TEST_IN_PROGRESS) {
			return 0;
		}
	}

	*result = SAFETY_TEST_ERROR;
	return -EIO;
}

static int stm32_stl_ram_run(const struct safety_test *test,
			     enum safety_test_result *result)
{
	if ((test == NULL) || (test->config == NULL)) {
		return -EINVAL;
	}

	return safety_stm32_stl_ram_run_complete(test->config, result);
}

static int stm32_stl_flash_run(const struct safety_test *test,
			       enum safety_test_result *result)
{
	if ((test == NULL) || (test->config == NULL)) {
		return -EINVAL;
	}

	return safety_stm32_stl_flash_run_complete(test->config, result);
}

const struct safety_test_api safety_stm32_stl_ram_api = {
	.init = stm32_stl_init,
	.run = stm32_stl_ram_run,
	.reset = NULL,
	.deinit = NULL,
};

const struct safety_test_api safety_stm32_stl_flash_api = {
	.init = stm32_stl_init,
	.run = stm32_stl_flash_run,
	.reset = NULL,
	.deinit = NULL,
};

static int stm32_stl_run(const struct safety_test *test,
			 enum safety_test_result *result)
{
	const struct stm32_stl_cpu_config *config;
	STL_TmStatus_t stl_result = STL_NOT_TESTED;
	STL_Status_t status;

	if ((test == NULL) || (test->config == NULL) || (result == NULL)) {
		return -EINVAL;
	}
	if (atomic_get(&init_state) != STM32_STL_INITIALIZED) {
		*result = SAFETY_TEST_ERROR;
		return -EACCES;
	}

	config = test->config;
	status = config->run(&stl_result);
	if (status != STL_OK) {
		/* STL documents the test-module status as irrelevant after STL_KO. */
		*result = SAFETY_TEST_ERROR;
		return -EIO;
	}

	return stm32_stl_map_result(stl_result, result);
}

static const struct safety_test_api stm32_stl_cpu_api = {
	.init = stm32_stl_init,
	.run = stm32_stl_run,
	.reset = NULL,
	.deinit = NULL,
};

static const struct stm32_stl_cpu_config cpu_configs[] = {
	[SAFETY_STM32_STL_CPU_TM1] = { .run = STL_SCH_RunCpuTM1 },
	[SAFETY_STM32_STL_CPU_TM1L] = { .run = STL_SCH_RunCpuTM1L },
	[SAFETY_STM32_STL_CPU_TM2] = { .run = STL_SCH_RunCpuTM2 },
	[SAFETY_STM32_STL_CPU_TM3] = { .run = STL_SCH_RunCpuTM3 },
	[SAFETY_STM32_STL_CPU_TM4] = { .run = STL_SCH_RunCpuTM4 },
	[SAFETY_STM32_STL_CPU_TM5] = { .run = STL_SCH_RunCpuTM5 },
	[SAFETY_STM32_STL_CPU_TM6] = { .run = STL_SCH_RunCpuTM6 },
	[SAFETY_STM32_STL_CPU_TM7] = { .run = STL_SCH_RunCpuTM7 },
	[SAFETY_STM32_STL_CPU_TM7L] = { .run = STL_SCH_RunCpuTM7L },
	[SAFETY_STM32_STL_CPU_TM8] = { .run = STL_SCH_RunCpuTM8 },
};

const struct safety_provider safety_stm32_stl_provider = {
	.vendor = "STMicroelectronics",
	.package = "X-CUBE-STL-G0",
	.version = "STL library 2.0.0",
	.target = "STM32G071xx",
};

#define STM32_STL_CPU_TEST(test_id, test_name)                            \
	[test_id] = {                                                     \
		.name = test_name,                                         \
		.provider = &safety_stm32_stl_provider,                     \
		.provider_test_id = test_id,                                \
		.api = &stm32_stl_cpu_api,                                  \
		.capabilities = 0U,                                         \
		.wcet_us = 0U,                                              \
		.max_irq_mask_us = 0U,                                      \
		.config = &cpu_configs[test_id],                             \
		.data = NULL,                                                \
	}

const struct safety_test
	safety_stm32_stl_cpu_tests[SAFETY_STM32_STL_CPU_TEST_COUNT] = {
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM1, "CPU TM1"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM1L, "CPU TM1L"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM2, "CPU TM2"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM3, "CPU TM3"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM4, "CPU TM4"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM5, "CPU TM5"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM6, "CPU TM6"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM7, "CPU TM7"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM7L, "CPU TM7L"),
	STM32_STL_CPU_TEST(SAFETY_STM32_STL_CPU_TM8, "CPU TM8"),
};
