/* Copyright (c) 2026 Sixtaupi */
/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/linker/sections.h>

SECTION_PROLOGUE(backup_buffer_section, (NOLOAD), SUBALIGN(4))
{
	__stm32_stl_ram_backup_start = .;
	KEEP(*(backup_buffer_section))
	/*
	 * STL consumes 32 bytes, while its March C sections are 128 bytes.
	 * Reserve the complete first section so the remaining SRAM is exactly
	 * divisible into vendor test sections and no tail is silently skipped.
	 */
	. = ALIGN(128);
	__stm32_stl_ram_backup_end = .;
} GROUP_DATA_LINK_IN(RAMABLE_REGION, RAMABLE_REGION)
