/*
 * Copyright (C) 2015 Compulab, Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <miiphy.h>
#include <cpsw.h>
#include <asm/gpio.h>
#include <asm/arch/sys_proto.h>
#include <asm/emif.h>
#include <power/pmic.h>
#include <power/tps65218.h>
#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

/* setup board specific PMIC */
int power_init_board(void)
{
	struct pmic *p;
	uchar tps_status = 0;

	power_tps65218_init(I2C_PMIC);
	p = pmic_get("TPS65218_PMIC");
	if (p && !pmic_probe(p)) {
		puts("PMIC:  TPS65218\n");
		/* We don't care if fseal is locked, but we do need it set */
		tps65218_lock_fseal();
		tps65218_reg_read(TPS65218_STATUS, &tps_status);
		if (!(tps_status & TPS65218_FSEAL))
			printf("WARNING: RTC not backed by battery!\n");
	}

	return 0;
}

int board_init(void)
{
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
	gpmc_init();
	set_i2c_pin_mux();
	i2c_init(CONFIG_SYS_OMAP24_I2C_SPEED, CONFIG_SYS_OMAP24_I2C_SLAVE);
	i2c_probe(TPS65218_CHIP_PM);

	return 0;
}

#ifdef CONFIG_DRIVER_TI_CPSW

static void cpsw_control(int enabled)
{
	return;
}

/* Common PHY address for all of the configurations */
#define PHY_ADDR_COMM 0x00
#define OUI_PHY_ATHEROS 0x1374
#define OUI_PHY_REALTEK 0x0732
#define PHY_PRI_ADDR_ATHEROS 0
#define PHY_SEC_ADDR_ATHEROS 1
#define PHY_PRI_ADDR_REALTEK 4
#define PHY_SEC_ADDR_REALTEK 5
#define PHY_INIT_DLY 30

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_addr	= PHY_PRI_ADDR_ATHEROS,
		.phy_if		= PHY_INTERFACE_MODE_RGMII,
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,
		.phy_addr	= PHY_SEC_ADDR_ATHEROS,
		.phy_if		= PHY_INTERFACE_MODE_RGMII,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 2,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
	.active_slave		= 0,
};

#define GPIO_PHY1_RST		170
#define GPIO_PHY2_RST		168

int board_phy_config(struct phy_device *phydev)
{
	static int stage = 0;
	unsigned short phyid1, phyid2;
	unsigned int oui;

	if (stage == 0) {
		phydev->addr = PHY_ADDR_COMM;

		phyid1 = phy_read(phydev, MDIO_DEVAD_NONE, MII_PHYSID1);
		phyid2 = phy_read(phydev, MDIO_DEVAD_NONE, MII_PHYSID2);
		oui = phyid2 | phyid1 << 16;
		if (oui == 0xffff) {
			printf("%s: PHY not detected\n", __func__);
			return -1;
		}
		oui >>= 10;
		switch (oui) {
		case OUI_PHY_ATHEROS:
			cpsw_slaves[0].phy_addr = PHY_PRI_ADDR_ATHEROS;
			cpsw_slaves[1].phy_addr = PHY_SEC_ADDR_ATHEROS;
			break;
		case OUI_PHY_REALTEK:
			cpsw_slaves[0].phy_addr = PHY_PRI_ADDR_REALTEK;
			cpsw_slaves[1].phy_addr = PHY_SEC_ADDR_REALTEK;
			break;
		default:
			printf("%s: PHY not detected\n", __func__);
			return -1;
		}
		stage = 1;
		return 0;
	}

	if (phydev->drv->config)
		return phydev->drv->config(phydev);

	return 0;
}


static void board_phy_init(void)
{
	set_mdio_pin_mux();
	writel(0x40003, 0x44e10a74); /* Mux pin as clkout2 */
	writel(0x10006, 0x44df4108); /* Select EXTDEV as clock source */
	writel(0x4, 0x44df2e60); /* Set EXTDEV as MNbypass */

	/* For revision A */
	writel(0x2000009, 0x44df2e6c);
	writel(0x38a, 0x44df2e70);

	mdelay(10);

	gpio_request(GPIO_PHY1_RST, "phy1_rst");
	gpio_request(GPIO_PHY2_RST, "phy2_rst");
	gpio_direction_output(GPIO_PHY1_RST, 0);
	gpio_direction_output(GPIO_PHY2_RST, 0);
	mdelay(PHY_INIT_DLY);

	gpio_set_value(GPIO_PHY1_RST, 1);
	gpio_set_value(GPIO_PHY2_RST, 1);
	mdelay(PHY_INIT_DLY);

}

static int board_phy_reg_set(void)
{
	const char *devname;
	unsigned short val;
	int rv;

	devname = miiphy_get_current_dev();

	switch (cpsw_slaves[0].phy_addr) {
	case PHY_PRI_ADDR_ATHEROS:
		/* Enable TX clock delay */
		rv = miiphy_write(devname, PHY_PRI_ADDR_ATHEROS, 0x1d, 0x5);
		rv |= miiphy_read(devname, PHY_PRI_ADDR_ATHEROS, 0x1e, &val);
		val |= 0x0100;
		rv |= miiphy_write(devname, PHY_PRI_ADDR_ATHEROS, 0x1e, val);
		rv |= miiphy_write(devname, PHY_SEC_ADDR_ATHEROS, 0x1d, 0x5);
		rv |= miiphy_read(devname, PHY_SEC_ADDR_ATHEROS, 0x1e, &val);
		val |= 0x0100;
		rv |= miiphy_write(devname, PHY_SEC_ADDR_ATHEROS, 0x1e, val);
		break;
	case PHY_PRI_ADDR_REALTEK:
		/* Enable TX & RX clock delay */
		rv = miiphy_write(devname, PHY_PRI_ADDR_REALTEK, 0x1f, 0x07);
		rv |= miiphy_write(devname, PHY_PRI_ADDR_REALTEK, 0x1e, 0xa4);
		rv |= miiphy_read(devname, PHY_PRI_ADDR_REALTEK, 0x1c, &val);
		val |= (0x1 << 13) | (0x1 << 12) | (0x1 << 11);
		rv |= miiphy_write(devname, PHY_PRI_ADDR_REALTEK, 0x1c, val);
		rv |= miiphy_write(devname, PHY_SEC_ADDR_REALTEK, 0x1f, 0x07);
		rv |= miiphy_write(devname, PHY_SEC_ADDR_REALTEK, 0x1e, 0xa4);
		rv |= miiphy_read(devname, PHY_SEC_ADDR_REALTEK, 0x1c, &val);
		val |= (0x1 << 13) | (0x1 << 12) | (0x1 << 11);
		rv |= miiphy_write(devname, PHY_SEC_ADDR_REALTEK, 0x1c, val);
		/* LED settings */
		rv |= miiphy_write(devname, PHY_SEC_ADDR_REALTEK, 0x1e, 0x2c);
		rv |= miiphy_read(devname, PHY_SEC_ADDR_REALTEK, 0x1c, &val);
		val &= 0xf8ff;
		rv |= miiphy_write(devname, PHY_SEC_ADDR_REALTEK, 0x1c, val);
		/* Set page 0 */
		rv |= miiphy_write(devname, PHY_PRI_ADDR_REALTEK, 0x1f, 0x00);
		rv |= miiphy_write(devname, PHY_SEC_ADDR_REALTEK, 0x1f, 0x00);
		break;
	default:
		printf("%s: PHY initialization skipped!\n", __func__);
		return -1;
	}

	if (rv)
		return -1;
	return 0;
}

int board_eth_init(bd_t *bis)
{
	int rv;

	set_rgmii_pin_mux();
	writel(RGMII_MODE_ENABLE | RGMII_INT_DELAY, &cdev->miisel);
	board_phy_init();

	/* PHY type detection */
	rv = cpsw_register(&cpsw_data);
	if (rv < 0)
		printf("Error %d detecting PHY type\n", rv);
	rv = board_phy_reg_set();
	if (rv < 0)
		printf("Error %d setting PHY registers\n", rv);
	/* PHY initialization */
	rv = cpsw_register(&cpsw_data);
	if (rv < 0)
		printf("Error %d registering CPSW switch\n", rv);

	return rv;
}

#ifdef CONFIG_OF_BOARD_SETUP
#include "fdt_support.h"

#define FLIP_32B(val) ((val>>24)&0xff) | ((val<<8)&0xff0000) | \
	((val>>8)&0xff00) | ((val<<24)&0xff000000)
#define FDT_PHYADDR_PRI "/ocp/ethernet@4a100000/slave@4a100200"
#define FDT_PHYADDR_SEC "/ocp/ethernet@4a100000/slave@4a100300"

int ft_board_setup(void *blob, bd_t *bd)
{
	u32 phy_id[2];

	if (cpsw_slaves[0].phy_addr != PHY_PRI_ADDR_REALTEK)
		return 0;/* Not a Realtek PHY */

	/* Resize FDT to be on the safe side */
	fdt_shrink_to_minimum(blob);

	/* Update primary Ethernet interface PHY ID */
	phy_id[0] = fdt_getprop_u32_default(blob, FDT_PHYADDR_PRI, "phy_id", 0);
	phy_id[0] = FLIP_32B(phy_id[0]);
	phy_id[1] = FLIP_32B(PHY_PRI_ADDR_REALTEK);
	do_fixup_by_path(blob, FDT_PHYADDR_PRI, "phy_id", phy_id,
			 sizeof(phy_id), 0);
	/* Update secondary Ethernet interface PHY ID */
	phy_id[0] = fdt_getprop_u32_default(blob, FDT_PHYADDR_SEC, "phy_id", 0);
	phy_id[0] = FLIP_32B(phy_id[0]);
	phy_id[1] = FLIP_32B(PHY_SEC_ADDR_REALTEK);
	do_fixup_by_path(blob, FDT_PHYADDR_SEC, "phy_id", phy_id,
			 sizeof(phy_id), 0);

	return 0;
}
#endif /* CONFIG_OF_BOARD_SETUP */

#endif
