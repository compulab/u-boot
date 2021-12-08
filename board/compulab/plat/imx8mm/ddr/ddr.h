/*
 * Copyright 2017 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __DDR_H__
#define __DDR_H__

extern struct dram_timing_info ucm_dram_timing_ff020008;
extern struct dram_timing_info ucm_dram_timing_ff000110;
extern struct dram_timing_info ucm_dram_timing_01061010;
extern struct dram_timing_info ucm_dram_timing_01050008;

void spl_dram_init(void);

#endif
