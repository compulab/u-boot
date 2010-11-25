/*
 * cm_t3517.c - board file for CompuLab CM-T3517 module.
 *
 * Author: Igor Grinberg <grinberg@compulab.co.il>
 *
 * Based on logicpd/am3517evm/am3517evm.c
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
#include <netdev.h>
#include <net.h>

#include <asm/io.h>
#include <asm/arch/mem.h>
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/emac_defs.h>
#include <asm/arch/gpio.h>
#include <i2c.h>
#include <asm/mach-types.h>
#include "cm_t3517.h"

DECLARE_GLOBAL_DATA_PTR;

#define CM_T3517_DEBUG	0

#if CM_T3517_DEBUG
#define pr_dbg printf
#else
#define pr_dbg(fmt, ...)
#endif

/* CM-T3517 EPROM layout */
#define CM_T3517_EPROM_I2C_ADDR	0x50
/* Binary Block */
#define PCBREV		0
#define PCBPRO		2
#define MAC1		4
#define MAC2		10
#define DATE		16
#define SERIAL		20
#define MAC3		32
#define MAC4		38
#define LAYOUT		44
/* ASCII Block */
#define NAME		128
#define OPTIONS1	144
#define OPTIONS2	160
#define OPTIONS3	176

/* GPIOs */
#define CM_T3517_LED_GPIO	186
#define SB_T35_ETH_RST_GPIO	164

#define mdelay(n) ({unsigned long msec=(n); while (msec--) udelay(1000);})

#define AM3517_IP_SW_RESET	0x48002598
#define CPGMACSS_SW_RST		(1 << 1)
#define CONTROL_EFUSE_EMAC_LSB  0x48002380
#define CONTROL_EFUSE_EMAC_MSB  0x48002384

static u32 gpmc_nand_config[GPMC_MAX_REG] = {
	SMNAND_GPMC_CONFIG1,
	SMNAND_GPMC_CONFIG2,
	SMNAND_GPMC_CONFIG3,
	SMNAND_GPMC_CONFIG4,
	SMNAND_GPMC_CONFIG5,
	SMNAND_GPMC_CONFIG6,
        0,
};

extern struct gpmc *gpmc_cfg;

/*
 * Routine: board_init
 * Description: Early hardware init.
 */
int board_init(void)
{
	gpmc_init(); /* in SRAM or SDRAM, finish GPMC */

	enable_gpmc_cs_config(gpmc_nand_config, &gpmc_cfg->cs[0],
			      CONFIG_SYS_NAND_BASE, GPMC_SIZE_16M);

	/* board id for Linux */
	gd->bd->bi_arch_number = MACH_TYPE_CM_T3517;
	/* boot param addr */
	gd->bd->bi_boot_params = (OMAP34XX_SDRC_CS0 + 0x100);

	/* turn the Green LED on */
	if (!omap_request_gpio(CM_T3517_LED_GPIO)) {
		omap_set_gpio_direction(CM_T3517_LED_GPIO, 0);
		omap_set_gpio_dataout(CM_T3517_LED_GPIO, 1);
	}

	return 0;
}

static u32 cm_t3517_rev;

static u32 cm_t3517_board_rev(void)
{
	u8 tmp[4] = { 0xff, 0xff, 0xff, 0xff };
	u32 rev = 0xffffffff;
	int err = 0;

#ifdef CONFIG_DRIVER_OMAP34XX_I2C
	err = i2c_read(CM_T3517_EPROM_I2C_ADDR, PCBREV, 1, tmp, 4);
#endif
	if (!err)
		rev = (tmp[1] << 24) | (tmp[0] << 16) | (tmp[3] << 8) | tmp[2];

	return rev;
}

/*
 * get_board_rev() - setup to pass kernel board revision information
 */
u32 get_board_rev(void)
{
	return cm_t3517_rev;
}

void pcb_info(void)
{
	uint rev = cm_t3517_rev >> 16;
	uint rev_major = rev / 100;
	uint rev_minor = rev - (100 * rev_major);

	if ((rev_minor / 10) * 10 == rev_minor)
		rev_minor = rev_minor / 10;

	printf("PCB:   %u.%u\n", rev_major, rev_minor);
}

/*
 * Routine: misc_init_r
 * Description: Init i2c, ethernet, etc... (done here so udelay works)
 */
int misc_init_r(void)
{
#ifdef CONFIG_DRIVER_OMAP34XX_I2C
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif
	cm_t3517_rev = cm_t3517_board_rev();

	pcb_info();
	dieid_num_r();

	return 0;
}

/*
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers specific to the
 *		hardware. Many pins need to be moved from protect to primary
 *		mode.
 */
void set_muxconf_regs(void)
{
	MUX_CM_T3517();
}

static u32 gpmc_net_config[GPMC_MAX_REG] = {
	NET_GPMC_CONFIG1,
	NET_GPMC_CONFIG2,
	NET_GPMC_CONFIG3,
	NET_GPMC_CONFIG4,
	NET_GPMC_CONFIG5,
	NET_GPMC_CONFIG6,
	0
};

static void setup_net_chip_gmpc(void)
{
	struct ctrl *ctrl_base = (struct ctrl *)OMAP34XX_CTRL_BASE;
	enable_gpmc_cs_config(gpmc_net_config, &gpmc_cfg->cs[4],
			      CONFIG_SMC911X_BASE, GPMC_SIZE_16M);

	/* Enable off mode for NWE in PADCONF_GPMC_NWE register */
	writew(readw(&ctrl_base->gpmc_nwe) | 0x0E00, &ctrl_base->gpmc_nwe);
	/* Enable off mode for NOE in PADCONF_GPMC_NADV_ALE register */
	writew(readw(&ctrl_base->gpmc_noe) | 0x0E00, &ctrl_base->gpmc_noe);
	/* Enable off mode for ALE in PADCONF_GPMC_NADV_ALE register */
	writew(readw(&ctrl_base->gpmc_nadv_ale) | 0x0E00,
		&ctrl_base->gpmc_nadv_ale);
}

static void reset_net_chip(void)
{
	omap_request_gpio(SB_T35_ETH_RST_GPIO);

	/* set reset gpio as output */
	omap_set_gpio_direction(SB_T35_ETH_RST_GPIO, 0);

	/* send the pulse */
	omap_set_gpio_dataout(SB_T35_ETH_RST_GPIO, 1);
	mdelay(1);
	omap_set_gpio_dataout(SB_T35_ETH_RST_GPIO, 0);
	mdelay(35);
	omap_set_gpio_dataout(SB_T35_ETH_RST_GPIO, 1);
	mdelay(10);
}

int am3517_get_efuse_enetaddr(u8 *enetaddr)
{
	u32 lsb = __raw_readl(CONTROL_EFUSE_EMAC_LSB);
	u32 msb = __raw_readl(CONTROL_EFUSE_EMAC_MSB);

	enetaddr[0] = (u8)((msb >> 16) & 0xff);
	enetaddr[1] = (u8)((msb >> 8 ) & 0xff);
	enetaddr[2] = (u8)(msb & 0xff);
	enetaddr[3] = (u8)((lsb >> 16) & 0xff);
	enetaddr[4] = (u8)((lsb >> 8 ) & 0xff);
	enetaddr[5] = (u8)(lsb & 0xff);

	return is_valid_ether_addr(enetaddr);
}

int cm_t3517_get_eprom_enetaddr(u8 *enetaddr)
{
	int ret;

#ifdef CONFIG_DRIVER_OMAP34XX_I2C
	ret = i2c_read(CM_T3517_EPROM_I2C_ADDR, MAC1, 1, enetaddr, 6);
	if (ret)
		return 0;
#endif

	return is_valid_ether_addr(enetaddr);
}

int cm_t3517_get_env_enetaddr(u8 *enetaddr)
{
	int ret;

	ret = eth_getenv_enetaddr("ethaddr", enetaddr);
	if (ret)
		return ret;

	return eth_getenv_enetaddr("eth1addr", enetaddr);
}

void cm_t3517_setup_enetaddr(u8 *enetaddr)
{
	int ret;

	/* check the environment */
	ret = cm_t3517_get_env_enetaddr(enetaddr);
	if (ret)
		return;

	/* check eprom */
	ret = cm_t3517_get_eprom_enetaddr(enetaddr);
	if (!ret)	/* finally check efuse */
		ret = am3517_get_efuse_enetaddr(enetaddr);

	if (!ret) {
		printf("CM-T3517: No valid MAC address found\n");
		return;
	}

	pr_dbg("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", enetaddr[0], enetaddr[1],
	       enetaddr[2], enetaddr[3], enetaddr[4], enetaddr[5]);

	/* set the ethaddr variable with the detected addr */
	eth_setenv_enetaddr("ethaddr", enetaddr);
}

extern void davinci_eth_set_mac_addr(const u_int8_t *addr);

int board_eth_init(bd_t *bis)
{
	u8 enetaddr[6];
	int count = 0, ret;
	u32 reset;

	memset(enetaddr, 0, 6);
	cm_t3517_setup_enetaddr(enetaddr);

#if defined(CONFIG_DRIVER_TI_EMAC)
	/*ensure that the module is out of reset*/
	reset = readl(AM3517_IP_SW_RESET);
	reset &= ~CPGMACSS_SW_RST;
	writel(reset, AM3517_IP_SW_RESET);

	davinci_eth_set_mac_addr(enetaddr);
	ret = davinci_emac_initialize();
	count += ret == 1 ? 1 : 0;
#endif

#ifdef CONFIG_SMC911X
	setup_net_chip_gmpc();
	reset_net_chip();
	ret = smc911x_initialize(0, CONFIG_SMC911X_BASE);
	count += ret == 1 ? 1 : 0;
#endif

	return count;
}
