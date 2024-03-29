/*
 * SPL specific code for Compulab CM-FX6 board
 *
 * Copyright (C) 2014, Compulab Ltd - http://compulab.co.il/
 *
 * Author: Nikita Kiryanov <nikita@compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <spl.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/crm_regs.h>
#include <asm/imx-common/iomux-v3.h>
#include <fsl_esdhc.h>
#include "common.h"

DECLARE_GLOBAL_DATA_PTR;

enum ddr_config {
	DDR_16BIT_256MB,
	DDR_32BIT_512MB,
	DDR_32BIT_1GB,
	DDR_64BIT_1GB,
	DDR_64BIT_2GB,
	DDR_64BIT_4GB,
	DDR_UNKNOWN,
};

/*
 * Below DRAM_RESET[DDR_SEL] = 0 which is incorrect according to
 * Freescale QRM, but this is exactly the value used by the automatic
 * calibration script and it works also in all our tests, so we leave
 * it as is at this point.
 */
#define SOM_IMX6_QDP_DDR_IOMUX_CFG \
	.dram_sdqs0	= 0x00000038, \
	.dram_sdqs1	= 0x00000038, \
	.dram_sdqs2	= 0x00000038, \
	.dram_sdqs3	= 0x00000038, \
	.dram_sdqs4	= 0x00000038, \
	.dram_sdqs5	= 0x00000038, \
	.dram_sdqs6	= 0x00000038, \
	.dram_sdqs7	= 0x00000038, \
	.dram_addr02	= 0x00018200, \
	.dram_addr03	= 0x00008000, \
	.dram_addr04	= 0x00018200, \
	.dram_addr05	= 0x00018200, \
	.dram_addr06	= 0x00018200, \
	.dram_addr07	= 0x00018200, \
	.dram_addr08	= 0x00018200, \
	.dram_addr09	= 0x00018200, \
	.dram_dqm0	= 0x00000038, \
	.dram_dqm1	= 0x00000038, \
	.dram_dqm2	= 0x00000038, \
	.dram_dqm3	= 0x00000038, \
	.dram_dqm4	= 0x00000038, \
	.dram_dqm5	= 0x00000038, \
	.dram_dqm6	= 0x00000038, \
	.dram_dqm7	= 0x00000038, \
	.dram_cas	= 0x00000038, \
	.dram_ras	= 0x00000038, \
	.dram_sdclk_0	= 0x00000038, \
	.dram_sdclk_1	= 0x00000038, \
	.dram_sdcke0	= 0x00003000, \
	.dram_sdcke1	= 0x00003000, \
	.dram_reset	= 0x00000038, \
	.dram_sdba2	= 0x00000000, \
	.dram_sdodt0	= 0x00000038, \
	.dram_sdodt1	= 0x00000038,

#define SOM_IMX6_QDP_GPR_IOMUX_CFG \
	.grp_b0ds	= 0x00000038, \
	.grp_b1ds	= 0x00000038, \
	.grp_b2ds	= 0x00000038, \
	.grp_b3ds	= 0x00000038, \
	.grp_b4ds	= 0x00000038, \
	.grp_b5ds	= 0x00000038, \
	.grp_b6ds	= 0x00000038, \
	.grp_b7ds	= 0x00000038, \
	.grp_addds	= 0x00000038, \
	.grp_ddrmode_ctl = 0x00020000, \
	.grp_ddrpke	= 0x00000000, \
	.grp_ddrmode	= 0x00020000, \
	.grp_ctlds	= 0x00000038, \
	.grp_ddr_type	= 0x000C0000,

#define CM_FX6_DDR_IOMUX_CFG \
	.dram_sdqs0	= 0x00000038, \
	.dram_sdqs1	= 0x00000038, \
	.dram_sdqs2	= 0x00000038, \
	.dram_sdqs3	= 0x00000038, \
	.dram_sdqs4	= 0x00000038, \
	.dram_sdqs5	= 0x00000038, \
	.dram_sdqs6	= 0x00000038, \
	.dram_sdqs7	= 0x00000038, \
	.dram_dqm0	= 0x00000038, \
	.dram_dqm1	= 0x00000038, \
	.dram_dqm2	= 0x00000038, \
	.dram_dqm3	= 0x00000038, \
	.dram_dqm4	= 0x00000038, \
	.dram_dqm5	= 0x00000038, \
	.dram_dqm6	= 0x00000038, \
	.dram_dqm7	= 0x00000038, \
	.dram_cas	= 0x00000038, \
	.dram_ras	= 0x00000038, \
	.dram_sdclk_0	= 0x00000038, \
	.dram_sdclk_1	= 0x00000038, \
	.dram_sdcke0	= 0x00003000, \
	.dram_sdcke1	= 0x00003000, \
	.dram_reset	= 0x00000038, \
	.dram_sdba2	= 0x00000000, \
	.dram_sdodt0	= 0x00000038, \
	.dram_sdodt1	= 0x00000038,

#define CM_FX6_GPR_IOMUX_CFG \
	.grp_b0ds	= 0x00000038, \
	.grp_b1ds	= 0x00000038, \
	.grp_b2ds	= 0x00000038, \
	.grp_b3ds	= 0x00000038, \
	.grp_b4ds	= 0x00000038, \
	.grp_b5ds	= 0x00000038, \
	.grp_b6ds	= 0x00000038, \
	.grp_b7ds	= 0x00000038, \
	.grp_addds	= 0x00000038, \
	.grp_ddrmode_ctl = 0x00020000, \
	.grp_ddrpke	= 0x00000000, \
	.grp_ddrmode	= 0x00020000, \
	.grp_ctlds	= 0x00000038, \
	.grp_ddr_type	= 0x000C0000,


static struct mx6sdl_iomux_ddr_regs ddr_iomux_s = { CM_FX6_DDR_IOMUX_CFG };
static struct mx6sdl_iomux_grp_regs grp_iomux_s = { CM_FX6_GPR_IOMUX_CFG };
static struct mx6dq_iomux_ddr_regs ddr_iomux_q = { CM_FX6_DDR_IOMUX_CFG };
static struct mx6dq_iomux_grp_regs grp_iomux_q = { CM_FX6_GPR_IOMUX_CFG };
static struct mx6dq_iomux_ddr_regs qp_ddr_iomux_q = { SOM_IMX6_QDP_DDR_IOMUX_CFG };
static struct mx6dq_iomux_grp_regs qp_grp_iomux_q = { SOM_IMX6_QDP_GPR_IOMUX_CFG };

static void spl_mx6s_dram_init(enum ddr_config dram_config, bool reset)
{
	volatile struct mmdc_p_regs *mmdc_p0;
	volatile struct mmdc_p_regs *mmdc_p1;
	mmdc_p0 = (struct mmdc_p_regs *)MX6_MMDC_P0_MDCTL;
	mmdc_p1 = (struct mmdc_p_regs *)MX6_MMDC_P1_MDCTL;

	if (reset)
		mmdc_p0->mdmisc = 2;

	mmdc_p0->mdscr  = 0x00008000;
	while (!(mmdc_p0->mdscr & (1 << 14)))
		;

	mmdc_p0->mpzqhwctrl  = 0xA1390003;
	/* Wait for ZQ_HW_FOR to finish the calibration on both MMDCs */
	while (mmdc_p0->mpzqhwctrl & 0x00010000)
		;

	mmdc_p0->mpwldectrl0 = 0x005B0061;
	mmdc_p0->mpwldectrl1 = 0x004F0055;
	mmdc_p0->mpdgctrl0   = 0x0314030C;
	mmdc_p0->mpdgctrl1   = 0x025C0268;
	mmdc_p0->mprddlctl   = 0x42464646;
	mmdc_p0->mpwrdlctl   = 0x36322C34;
	mmdc_p0->mprddqby0dl = 0x33333333;
	mmdc_p0->mprddqby1dl = 0x33333333;
	mmdc_p0->mprddqby2dl = 0x33333333;
	mmdc_p0->mprddqby3dl = 0x33333333;
	mmdc_p1->mprddqby0dl = 0x33333333;
	mmdc_p1->mprddqby1dl = 0x33333333;
	mmdc_p1->mprddqby2dl = 0x33333333;
	mmdc_p1->mprddqby3dl = 0x33333333;
	mmdc_p0->mpmur0	     = 0x00000800;
	mmdc_p0->mdpdc  = 0x0002002D;
	mmdc_p0->mdotc  = 0x1B444040;
	mmdc_p0->mdcfg0 = 0x676B5335;
	mmdc_p0->mdcfg1 = 0xB68E8F64;
	mmdc_p0->mdcfg2 = 0x01FF00DB;
	mmdc_p0->mdmisc = 0x00091740;
	mmdc_p0->mdrwd  = 0x000026D2;
	mmdc_p0->mdor   = 0x006B1023;

	switch (dram_config) {
	case DDR_16BIT_256MB:
		mmdc_p0->mdctl = 0x83180000;
		break;
	case DDR_32BIT_512MB:
		mmdc_p0->mdctl = 0x83190000;
		break;
	case DDR_32BIT_1GB:
		mmdc_p0->mdctl = 0xC3190000;
		break;
	default:
		puts("Tried to setup invalid DDR configuration\n");
		hang();
	}

	mmdc_p0->mdscr = 0x00088032;
	mmdc_p0->mdscr = 0x0008803A;
	mmdc_p0->mdscr = 0x00008033;
	mmdc_p0->mdscr = 0x0000803B;
	mmdc_p0->mdscr = 0x00068031;
	mmdc_p0->mdscr = 0x00068039;
	mmdc_p0->mdscr = 0x09408030;
	mmdc_p0->mdscr = 0x09408038;
	mmdc_p0->mdscr = 0x04008040;
	mmdc_p0->mdscr = 0x04008048;
	mmdc_p0->mdref = 0x00007800;
	mmdc_p0->mpodtctrl = 0x00022227;
	mmdc_p0->mdpdc = 0x000255ED;
	mmdc_p0->mapsr = 0x00001006;
	mmdc_p0->mdscr = 0x00000000;
	udelay(100);
}

#define F1200MHZ
static void spl_mx6q_dram_init(enum ddr_config dram_config, bool reset)
{
	volatile struct mmdc_p_regs *mmdc_p0;
	volatile struct mmdc_p_regs *mmdc_p1;
	mmdc_p0 = (struct mmdc_p_regs *)MX6_MMDC_P0_MDCTL;
	mmdc_p1 = (struct mmdc_p_regs *)MX6_MMDC_P1_MDCTL;

	if (reset)
		mmdc_p0->mdmisc = 2;

	mmdc_p0->mdscr  = 0x00008000;
	while (!(mmdc_p0->mdscr & (1 << 14)))
		;

	mmdc_p0->mpzqhwctrl  = 0xA1390003;
	/* Wait for ZQ_HW_FOR to finish the calibration on both MMDCs */
	while (mmdc_p0->mpzqhwctrl & 0x00010000)
		;

#define FERROR
#ifdef F0800MHZ
	mmdc_p0->mpwldectrl0 = 0x0071007B;
	mmdc_p0->mpwldectrl1 = 0x0076006C;
	mmdc_p1->mpwldectrl0 = 0x003D0056;
	mmdc_p1->mpwldectrl1 = 0x00110022;
	mmdc_p0->mpdgctrl0   = 0x04100410;
	mmdc_p0->mpdgctrl1   = 0x03560356;
	mmdc_p1->mpdgctrl0   = 0x03540350;
	mmdc_p1->mpdgctrl1   = 0x03480310;
	mmdc_p0->mprddlctl   = 0x4C423E44;
	mmdc_p1->mprddlctl   = 0x3C3E3A48;
	mmdc_p0->mpwrdlctl   = 0x3A3A423C;
	mmdc_p1->mpwrdlctl   = 0x4A364640;
#undef FERROR
#endif
#ifdef F1000MHZ
	mmdc_p0->mpwldectrl0 = 0x006F0076;
	mmdc_p0->mpwldectrl1 = 0x0074006B;
	mmdc_p1->mpwldectrl0 = 0x003D0053;
	mmdc_p1->mpwldectrl1 = 0x00120021;
	mmdc_p0->mpdgctrl0   = 0x040C0418;
	mmdc_p0->mpdgctrl1   = 0x034A034A;
	mmdc_p1->mpdgctrl0   = 0x034A034A;
	mmdc_p1->mpdgctrl1   = 0x033C0320;
	mmdc_p0->mprddlctl   = 0x483C3A40;
	mmdc_p1->mprddlctl   = 0x383C3844;
	mmdc_p0->mpwrdlctl   = 0x3838423C;
	mmdc_p1->mpwrdlctl   = 0x4634463E;
#undef FERROR
#endif
#ifdef F1200MHZ
	mmdc_p0->mpwldectrl0 = 0x006D0077;
	mmdc_p0->mpwldectrl1 = 0x00710068;
	mmdc_p1->mpwldectrl0 = 0x003B0052;
	mmdc_p1->mpwldectrl1 = 0x00100021;
	mmdc_p0->mpdgctrl0   = 0x04040414;
	mmdc_p0->mpdgctrl1   = 0x03680368;
	mmdc_p1->mpdgctrl0   = 0x03380368;
	mmdc_p1->mpdgctrl1   = 0x03300314;
	mmdc_p0->mprddlctl   = 0x463A3842;
	mmdc_p1->mprddlctl   = 0x363C3842;
	mmdc_p0->mpwrdlctl   = 0x3838423C;
	mmdc_p1->mpwrdlctl   = 0x4834443E;
#undef FERROR
#endif
#ifdef FERROR
#error Undefined MMDC frequency
#endif
	mmdc_p0->mprddqby0dl = 0x33333333;
	mmdc_p0->mprddqby1dl = 0x33333333;
	mmdc_p0->mprddqby2dl = 0x33333333;
	mmdc_p0->mprddqby3dl = 0x33333333;
	mmdc_p1->mprddqby0dl = 0x33333333;
	mmdc_p1->mprddqby1dl = 0x33333333;
	mmdc_p1->mprddqby2dl = 0x33333333;
	mmdc_p1->mprddqby3dl = 0x33333333;
	mmdc_p0->mpmur0	     = 0x00000800;
	mmdc_p1->mpmur0	     = 0x00000800;
	/* MMDC init: in DDR3, 64-bit mode, only MMDC0 is initiated: */
	mmdc_p0->mdpdc  = 0x00020036;
	mmdc_p0->mdotc  = 0x09444040;
	mmdc_p0->mdcfg0 = 0x8A8F79A5;
	mmdc_p0->mdcfg1 = 0xFF738F64;
	mmdc_p0->mdcfg2 = 0x01FF00DD;
	mmdc_p0->mdmisc = 0x00091740;
	mmdc_p0->mdrwd  = 0x000026d2;
	mmdc_p0->mdor   = 0x008F1023;

	switch (dram_config) {
	case DDR_16BIT_256MB:
		mmdc_p0->mdctl = 0x83180000;
		break;
	case DDR_32BIT_512MB:
		mmdc_p0->mdctl = 0x83190000;
		break;
	case DDR_64BIT_1GB:
		mmdc_p0->mdctl = 0x831A0000;
		break;
	case DDR_64BIT_2GB:
		mmdc_p0->mdctl = 0xC31A0000;
		break;
	case DDR_64BIT_4GB:
		mmdc_p0->mdctl = 0xC41A0000;
		break;
	default:
		puts("Tried to setup invalid DDR configuration\n");
		hang();
	}

	mmdc_p0->mdscr = 0x00088032;
	mmdc_p0->mdscr = 0x0008803A;
	mmdc_p0->mdscr = 0x00008033;
	mmdc_p0->mdscr = 0x0000803B;
	mmdc_p0->mdscr = 0x00068031;
	mmdc_p0->mdscr = 0x00068039;
	mmdc_p0->mdscr = 0x09408030;
	mmdc_p0->mdscr = 0x09408038;
	mmdc_p0->mdscr = 0x04008040;
	mmdc_p0->mdscr = 0x04008048;
	mmdc_p0->mdref = 0x00007800;
	mmdc_p0->mpodtctrl = 0x00022227;
	mmdc_p1->mpodtctrl = 0x00022227;
	mmdc_p0->mdpdc = 0x000255F6;
	mmdc_p0->mapsr = 0x00001006;
	mmdc_p0->mdscr = 0x00000000;
	udelay(100);
}
static void spl_mx6dqp_dram_init(enum ddr_config dram_config, bool reset)
{
	volatile struct mmdc_p_regs *mmdc_p0;
	volatile struct mmdc_p_regs *mmdc_p1;
	mmdc_p0 = (struct mmdc_p_regs *)MX6_MMDC_P0_MDCTL;
	mmdc_p1 = (struct mmdc_p_regs *)MX6_MMDC_P1_MDCTL;

	volatile struct mx6dqp_noc_sched_regs *noc_sched;
	noc_sched = (struct mx6dqp_noc_sched_regs *)MX6DQP_NOC_SCHED_BASE;

	if (reset)
		mmdc_p0->mdmisc = 2;

	mmdc_p0->mdscr  = 0x00008000;
	while (!(mmdc_p0->mdscr & (1 << 14)))
		;

	mmdc_p0->mpzqhwctrl  = 0xA1390003;
	/* Wait for ZQ_HW_FOR to finish the calibration on both MMDCs */
	while (mmdc_p0->mpzqhwctrl & 0x00010000)
		;

	mmdc_p0->mpwldectrl0 = 0x00590059;
	mmdc_p0->mpwldectrl1 = 0x0055004f;
	mmdc_p1->mpwldectrl0 = 0x002b003e;
	mmdc_p1->mpwldectrl1 = 0x000b0015;
	mmdc_p0->mpdgctrl0   = 0x035c036c;
	mmdc_p0->mpdgctrl1   = 0x033c0338;
	mmdc_p1->mpdgctrl0   = 0x03300334;
	mmdc_p1->mpdgctrl1   = 0x03100268;
	mmdc_p0->mprddlctl   = 0x44363c42;
	mmdc_p1->mprddlctl   = 0x3e3a3640;
	mmdc_p0->mpwrdlctl   = 0x3a3a3a3c;
	mmdc_p1->mpwrdlctl   = 0x44384442;
	mmdc_p0->mprddqby0dl = 0x33333333;
	mmdc_p0->mprddqby1dl = 0x33333333;
	mmdc_p0->mprddqby2dl = 0x33333333;
	mmdc_p0->mprddqby3dl = 0x33333333;
	mmdc_p1->mprddqby0dl = 0x33333333;
	mmdc_p1->mprddqby1dl = 0x33333333;
	mmdc_p1->mprddqby2dl = 0x33333333;
	mmdc_p1->mprddqby3dl = 0x33333333;
	mmdc_p0->mpdccr      = 0x24912489;
	mmdc_p1->mpdccr      = 0x24914452;
	mmdc_p0->mpmur0	     = 0x00000800;
	mmdc_p1->mpmur0	     = 0x00000800;
	/* MMDC init: in DDR3, 64-bit mode, only MMDC0 is initiated: */
	mmdc_p0->mdpdc  = 0x00020036;
	mmdc_p0->mdotc  = 0x09444040;
	mmdc_p0->mdcfg0 = 0x8A8F79A5;
	mmdc_p0->mdcfg1 = 0xFF738F64;
	mmdc_p0->mdcfg2 = 0x01FF00DD;
	mmdc_p0->mdmisc = 0x00091740;
	mmdc_p0->mdscr  = 0x00008000;
	while (!(mmdc_p0->mdscr & (1 << 14)))
	          ;
	mmdc_p0->mdrwd  = 0x000026d2;
	mmdc_p0->mdor   = 0x008F1023;
	//       	mmdc_p0->mdasp  = 0x00000027; /* this causes incorrect DRAM size detection */
        mmdc_p0->maarcr = 0x14420000;

	switch (dram_config) {
	case DDR_16BIT_256MB:
		mmdc_p0->mdctl = 0x83180000;
		break;
	case DDR_32BIT_512MB:
		mmdc_p0->mdctl = 0x83190000;
		break;
	case DDR_64BIT_1GB:
		mmdc_p0->mdctl = 0x831A0000;
		break;
	case DDR_64BIT_2GB:
		mmdc_p0->mdctl = 0xC31A0000;
		break;
	case DDR_64BIT_4GB:
		mmdc_p0->mdctl = 0xC41A0000;
		break;
	default:
		puts("Tried to setup invalid DDR configuration\n");
		hang();
	}

	mmdc_p0->mppdcmpr2 = 0x00400c58;

	/* NoC DDR configuration register set 0x00BB0000 0x00BB003F */
	noc_sched->ddrconf = 0x00000000;
	noc_sched->ddrtiming = 0x2891E41A;
	noc_sched->activate = 0x00000564;
	noc_sched->rlat = 0x00000040;
	noc_sched->ipu1 = 0x00000020;
	noc_sched->ipu2 = 0x00000020;

	mmdc_p0->mdscr = 0x00088032;
	mmdc_p0->mdscr = 0x00008033;
	mmdc_p0->mdscr = 0x00068031;
	mmdc_p0->mdscr = 0x09408030;
	mmdc_p0->mdscr = 0x04008040;

	mmdc_p0->mdscr = 0x0008803A;
	mmdc_p0->mdscr = 0x0000803B;
	mmdc_p0->mdscr = 0x00068039;
	mmdc_p0->mdscr = 0x09408038;
	mmdc_p0->mdscr = 0x04008048;

	mmdc_p0->mdref = 0x00007800;
	mmdc_p0->mpodtctrl = 0x00022227;
	mmdc_p1->mpodtctrl = 0x00022227;
	mmdc_p0->mdpdc = 0x000255F6;
	mmdc_p0->mapsr = 0x00001006;
	mmdc_p0->mdscr = 0x00000000;
	udelay(100);
}

static int cm_fx6_spl_dram_init(void)
{
	unsigned long bank1_size, bank2_size;

	switch (get_cpu_type()) {
	case MXC_CPU_MX6SOLO:
		mx6sdl_dram_iocfg(64, &ddr_iomux_s, &grp_iomux_s);

		spl_mx6s_dram_init(DDR_32BIT_1GB, false);
		bank1_size = get_ram_size((long int *)PHYS_SDRAM_1, 0x80000000);
		bank2_size = get_ram_size((long int *)PHYS_SDRAM_2, 0x80000000);
		if (bank1_size == 0x20000000) {
			if (bank2_size == 0x20000000)
				return 0;

			spl_mx6s_dram_init(DDR_32BIT_512MB, true);
			return 0;
		}

		spl_mx6s_dram_init(DDR_16BIT_256MB, true);
		bank1_size = get_ram_size((long int *)PHYS_SDRAM_1, 0x80000000);
		if (bank1_size == 0x10000000)
			return 0;

		break;
	case MXC_CPU_MX6D:
	case MXC_CPU_MX6Q:
		if (is_mx6dqp()) {
			puts("DualPlus/QuadPlus CPU detected\n");
			mx6dqp_dram_iocfg(64, &qp_ddr_iomux_q, &qp_grp_iomux_q);
			spl_mx6dqp_dram_init(DDR_64BIT_4GB, false);
			bank1_size = get_ram_size((long int *)PHYS_SDRAM_1, 0x80000000);
			if (bank1_size == 0x80000000)
				return 0;

			if (bank1_size == 0x40000000) {
				bank2_size = get_ram_size((long int *)PHYS_SDRAM_2,
								0x80000000);
				if (bank2_size == 0x40000000) {
				/* Don't do a full reset here */
				    spl_mx6dqp_dram_init(DDR_64BIT_2GB, false);
				} else {
				    spl_mx6dqp_dram_init(DDR_64BIT_1GB, true);
				}

				return 0;
			}

			spl_mx6dqp_dram_init(DDR_32BIT_512MB, true);
			bank1_size = get_ram_size((long int *)PHYS_SDRAM_1, 0x80000000);
			if (bank1_size == 0x20000000)
				return 0;

			spl_mx6dqp_dram_init(DDR_16BIT_256MB, true);
			bank1_size = get_ram_size((long int *)PHYS_SDRAM_1, 0x80000000);
			if (bank1_size == 0x10000000)
				return 0;
	        } else { //non QuadPlus or DualPlus SoC
			puts("Dual/Quad CPU detected\n");
			mx6dq_dram_iocfg(64, &ddr_iomux_q, &grp_iomux_q);
			spl_mx6q_dram_init(DDR_64BIT_4GB, false);
			bank1_size = get_ram_size((long int *)PHYS_SDRAM_1, 0x80000000);
			if (bank1_size == 0x80000000)
				return 0;

			if (bank1_size == 0x40000000) {
				bank2_size = get_ram_size((long int *)PHYS_SDRAM_2,
								0x80000000);
				if (bank2_size == 0x40000000) {
				/* Don't do a full reset here */
				    spl_mx6q_dram_init(DDR_64BIT_2GB, false);
				} else {
				    spl_mx6q_dram_init(DDR_64BIT_1GB, true);
				}

				return 0;
			}

			spl_mx6q_dram_init(DDR_32BIT_512MB, true);
			bank1_size = get_ram_size((long int *)PHYS_SDRAM_1, 0x80000000);
			if (bank1_size == 0x20000000)
				return 0;

			spl_mx6q_dram_init(DDR_16BIT_256MB, true);
			bank1_size = get_ram_size((long int *)PHYS_SDRAM_1, 0x80000000);
			if (bank1_size == 0x10000000)
				return 0;

                }
		break;
	}

	return -1;
}

static iomux_v3_cfg_t const uart4_pads[] = {
	IOMUX_PADS(PAD_KEY_COL0__UART4_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
	IOMUX_PADS(PAD_KEY_ROW0__UART4_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
};

static void cm_fx6_setup_uart(void)
{
	SETUP_IOMUX_PADS(uart4_pads);
	enable_uart_clk(1);
}

#ifdef CONFIG_SPL_SPI_SUPPORT
static void cm_fx6_setup_ecspi(void)
{
	cm_fx6_set_ecspi_iomux();
	enable_cspi_clock(1, 0);
}
#else
static void cm_fx6_setup_ecspi(void) { }
#endif

void board_init_f(ulong dummy)
{
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	/*
	 * We don't use DMA in SPL, but we do need it in U-Boot. U-Boot
	 * initializes DMA very early (before all board code), so the only
	 * opportunity we have to initialize APBHDMA clocks is in SPL.
	 */
	setbits_le32(&mxc_ccm->CCGR0, MXC_CCM_CCGR0_APBHDMA_MASK);
	enable_usdhc_clk(1, 2);

	arch_cpu_init();
	timer_init();
	cm_fx6_setup_ecspi();
	cm_fx6_setup_uart();
	get_clocks();
	preloader_console_init();
	gpio_direction_output(CM_FX6_GREEN_LED, 1);
	if (cm_fx6_spl_dram_init()) {
		puts("!!!ERROR!!! DRAM detection failed!!!\n");
		hang();
	}

	memset(__bss_start, 0, __bss_end - __bss_start);
	board_init_r(NULL, 0);
}

void board_boot_order(u32 *spl_boot_list)
{
	spl_boot_list[0] = spl_boot_device();
	switch (spl_boot_list[0]) {
	case BOOT_DEVICE_SPI:
		spl_boot_list[1] = BOOT_DEVICE_MMC1;
		break;
	case BOOT_DEVICE_MMC1:
		spl_boot_list[1] = BOOT_DEVICE_SPI;
		break;
	}
}

#ifdef CONFIG_SPL_MMC_SUPPORT
static struct fsl_esdhc_cfg usdhc_cfg = {
	.esdhc_base = USDHC3_BASE_ADDR,
	.max_bus_width = 4,
};

int board_mmc_init(bd_t *bis)
{
	cm_fx6_set_usdhc_iomux();

	usdhc_cfg.sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);

	return fsl_esdhc_initialize(bis, &usdhc_cfg);
}
#endif
