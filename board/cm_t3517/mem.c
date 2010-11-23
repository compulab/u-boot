/*
 * mem.c - DDR2 SDRAM init for CompuLab's CM-T3517 module.
 *
 * Author: Igor Grinberg <grinberg@compulab.co.il>
 *
 * Copyright (C) 2010
 * CompuLab, Ltd. - www.compulab.co.il
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <common.h>
#include <asm/io.h>


DECLARE_GLOBAL_DATA_PTR;

static emif4_t *emif4_base = (emif4_t *)OMAP34XX_SDRC_BASE;

/**
 * mem_init() - Initialize memory subsystem
 */
void mem_init(void)
{
	/* DRAM has been already initialized by x-loader */
}

/*
 * CM-X300 can have at most two DDR2 x16 chips connected on CS0.
 * If only one chip is assembled, then 16-31 data lines are not used.
 * Therefore the EMIF4 controller must be setup in narrow mode.
 * Here we assume that x-loader has configured the EMIF4 correctly,
 * so we just read the configuration.
 */
static inline uint get_sdram_size(void)
{
	u32 narrow = (readl(&emif4_base->sdram_config) >> 14) && 0x3;

	return narrow ? PHYS_SDRAM_1_SIZE / 2 : PHYS_SDRAM_1_SIZE;
}

/* CM-X300 modules can come either with 64MB or 128MB RAM */
int dram_init(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = get_sdram_size();

	return 0;
}


/**
 * is_mem_sdr() - return 1 if mem type in use is SDR
 */
u32 is_mem_sdr(void)
{
	return 0;
}

/**
 * get_sdr_cs_offset() - get offset of cs from cs0 start
 */
u32 get_sdr_cs_offset(u32 cs)
{
	return 0;
}

/**
 * get_sdr_cs_size() - get size of chip select 0/1
 */
u32 get_sdr_cs_size(u32 cs)
{
	return get_sdram_size();
}
