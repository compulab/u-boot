/*
 * Board functions for Compulab CM-FX6 board
 *
 * Copyright (C) 2014, Compulab Ltd - http://compulab.co.il/
 *
 * Author: Nikita Kiryanov <nikita@compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <fsl_esdhc.h>
#include <miiphy.h>
#include <netdev.h>
#include <errno.h>
#include <usb.h>
#include <fdt_support.h>
#include <sata.h>
#include <splash.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mxc_hdmi.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/sata.h>
#include <asm/imx-common/video.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <dm/platform_data/serial_mxc.h>
#include "common.h"
#include "../common/eeprom.h"
#include "../common/common.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SPLASH_SCREEN
static struct splash_location cm_fx6_splash_locations[] = {
	{
		.name = "sf",
		.storage = SPLASH_STORAGE_SF,
		.flags = SPLASH_STORAGE_RAW,
		.offset = 0x100000,
	},
	{
		.name = "mmc_fs",
		.storage = SPLASH_STORAGE_MMC,
		.flags = SPLASH_STORAGE_FS,
		.devpart = "2:1",
	},
	{
		.name = "usb_fs",
		.storage = SPLASH_STORAGE_USB,
		.flags = SPLASH_STORAGE_FS,
		.devpart = "0:1",
	},
	{
		.name = "sata_fs",
		.storage = SPLASH_STORAGE_SATA,
		.flags = SPLASH_STORAGE_FS,
		.devpart = "0:1",
	},
};

int splash_screen_prepare(void)
{
	return splash_source_load(cm_fx6_splash_locations,
				  ARRAY_SIZE(cm_fx6_splash_locations));
}
#endif

#ifdef CONFIG_IMX_HDMI
static void cm_fx6_enable_hdmi(struct display_info_t const *dev)
{
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	imx_setup_hdmi();
	setbits_le32(&mxc_ccm->CCGR3, MXC_CCM_CCGR3_IPU1_IPU_DI0_MASK);
	imx_enable_hdmi_phy();
}

static struct display_info_t preset_hdmi_1024X768 = {
	.bus	= -1,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.enable	= cm_fx6_enable_hdmi,
	.mode	= {
		.name           = "HDMI",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 40385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED,
	}
};

static iomux_v3_cfg_t const rgb_pads[] = {
	IOMUX_PADS(PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DI0_PIN15__IPU1_DI0_PIN15 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DI0_PIN2__IPU1_DI0_PIN02 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DI0_PIN3__IPU1_DI0_PIN03 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DI0_PIN4__IPU1_DI0_PIN04 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT0__IPU1_DISP0_DATA00 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT1__IPU1_DISP0_DATA01 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT2__IPU1_DISP0_DATA02 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT3__IPU1_DISP0_DATA03 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT4__IPU1_DISP0_DATA04 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT5__IPU1_DISP0_DATA05 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT6__IPU1_DISP0_DATA06 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT7__IPU1_DISP0_DATA07 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT8__IPU1_DISP0_DATA08 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT9__IPU1_DISP0_DATA09 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT10__IPU1_DISP0_DATA10 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT11__IPU1_DISP0_DATA11 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT12__IPU1_DISP0_DATA12 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT13__IPU1_DISP0_DATA13 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT14__IPU1_DISP0_DATA14 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT15__IPU1_DISP0_DATA15 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT16__IPU1_DISP0_DATA16 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT17__IPU1_DISP0_DATA17 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT18__IPU1_DISP0_DATA18 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT19__IPU1_DISP0_DATA19 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT20__IPU1_DISP0_DATA20 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT21__IPU1_DISP0_DATA21 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT22__IPU1_DISP0_DATA22 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_DISP0_DAT23__IPU1_DISP0_DATA23 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

static void enable_rgb(struct display_info_t const *dev)
{
	imx_iomux_v3_setup_multiple_pads(rgb_pads, ARRAY_SIZE(rgb_pads));
}

static struct display_info_t preset_lcd_800X480 = {
	.bus	= 0,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= NULL,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "LCD",
		.refresh        = 60,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = 29850,
		.left_margin    = 89,
		.right_margin   = 164,
		.upper_margin   = 23,
		.lower_margin   = 10,
		.hsync_len      = 10,
		.vsync_len      = 10,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
	}
};

static void cm_fx6_setup_display(void)
{
	struct iomuxc *const iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;

	enable_ipu_clock();
	clrbits_le32(&iomuxc_regs->gpr[3], MXC_CCM_CCGR3_IPU1_IPU_DI0_MASK);
}

int board_video_skip(void)
{
	int ret;
	struct display_info_t *preset;
	char const *panel = getenv("displaytype");

	if (!panel) /* Also accept panel for backward compatibility */
		panel = getenv("panel");

	if (!panel)
		return -ENOENT;

	if (!strcmp(panel, "HDMI"))
		preset = &preset_hdmi_1024X768;
	else if (!strcmp(panel, "LCD"))
		preset = &preset_lcd_800X480;
	else
		return -EINVAL;

	ret = ipuv3_fb_init(&preset->mode, 0, preset->pixfmt);
	if (ret) {
		printf("Can't init display %s: %d\n", preset->mode.name, ret);
		return ret;
	}

	preset->enable(preset);
	printf("Display: %s (%ux%u)\n", preset->mode.name, preset->mode.xres,
	       preset->mode.yres);

	return 0;
}
#else
static inline void cm_fx6_setup_display(void) {}
#endif /* CONFIG_VIDEO_IPUV3 */

#ifdef CONFIG_DWC_AHSATA
static int cm_fx6_issd_gpios[] = {
	/* The order of the GPIOs in the array is important! */
	CM_FX6_SATA_LDO_EN,
	CM_FX6_SATA_PHY_SLP,
	CM_FX6_SATA_NRSTDLY,
	CM_FX6_SATA_PWREN,
	CM_FX6_SATA_NSTANDBY1,
	CM_FX6_SATA_NSTANDBY2,
};

static void cm_fx6_sata_power(int on)
{
	int i;

	if (!on) { /* tell the iSSD that the power will be removed */
		gpio_direction_output(CM_FX6_SATA_PWLOSS_INT, 1);
		mdelay(10);
	}

	for (i = 0; i < ARRAY_SIZE(cm_fx6_issd_gpios); i++) {
		gpio_direction_output(cm_fx6_issd_gpios[i], on);
		udelay(100);
	}

	if (!on) /* for compatibility lower the power loss interrupt */
		gpio_direction_output(CM_FX6_SATA_PWLOSS_INT, 0);
}

static iomux_v3_cfg_t const sata_pads[] = {
	/* SATA PWR */
	IOMUX_PADS(PAD_ENET_TX_EN__GPIO1_IO28 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_A22__GPIO2_IO16    | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_D20__GPIO3_IO20    | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_A25__GPIO5_IO02    | MUX_PAD_CTRL(NO_PAD_CTRL)),
	/* SATA CTRL */
	IOMUX_PADS(PAD_ENET_TXD0__GPIO1_IO30  | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_D23__GPIO3_IO23    | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_D29__GPIO3_IO29    | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_A23__GPIO6_IO06    | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_BCLK__GPIO6_IO31   | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

static int cm_fx6_setup_issd(void)
{
	int ret, i;

	SETUP_IOMUX_PADS(sata_pads);

	for (i = 0; i < ARRAY_SIZE(cm_fx6_issd_gpios); i++) {
		ret = gpio_request(cm_fx6_issd_gpios[i], "sata");
		if (ret)
			return ret;
	}

	ret = gpio_request(CM_FX6_SATA_PWLOSS_INT, "sata_pwloss_int");
	if (ret)
		return ret;

	return 0;
}

#define CM_FX6_SATA_INIT_RETRIES	10
int sata_initialize(void)
{
	int err, i;

	/* Make sure this gpio has logical 0 value */
	gpio_direction_output(CM_FX6_SATA_PWLOSS_INT, 0);
	udelay(100);
	cm_fx6_sata_power(1);

	for (i = 0; i < CM_FX6_SATA_INIT_RETRIES; i++) {
		err = setup_sata();
		if (err) {
			printf("SATA setup failed: %d\n", err);
			return err;
		}

		udelay(100);

		err = __sata_initialize();
		if (!err)
			break;

		/* There is no device on the SATA port */
		if (sata_port_status(0, 0) == 0)
			break;

		/* There's a device, but link not established. Retry */
	}

	return err;
}

int sata_stop(void)
{
	__sata_stop();
	cm_fx6_sata_power(0);
	mdelay(250);

	return 0;
}
#else
static int cm_fx6_setup_issd(void) { return 0; }
#endif

#ifdef CONFIG_SYS_I2C_MXC
#define I2C_PAD_CTRL	(PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED | \
			PAD_CTL_DSE_40ohm | PAD_CTL_HYS | \
			PAD_CTL_ODE | PAD_CTL_SRE_FAST)

I2C_PADS(i2c0_pads,
	 PAD_EIM_D21__I2C1_SCL | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 PAD_EIM_D21__GPIO3_IO21 | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 IMX_GPIO_NR(3, 21),
	 PAD_EIM_D28__I2C1_SDA | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 PAD_EIM_D28__GPIO3_IO28 | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 IMX_GPIO_NR(3, 28));

I2C_PADS(i2c1_pads,
	 PAD_KEY_COL3__I2C2_SCL | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 PAD_KEY_COL3__GPIO4_IO12 | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 IMX_GPIO_NR(4, 12),
	 PAD_KEY_ROW3__I2C2_SDA | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 PAD_KEY_ROW3__GPIO4_IO13 | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 IMX_GPIO_NR(4, 13));

I2C_PADS(i2c2_pads,
	 PAD_GPIO_3__I2C3_SCL | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 PAD_GPIO_3__GPIO1_IO03 | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 IMX_GPIO_NR(1, 3),
	 PAD_GPIO_6__I2C3_SDA | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 PAD_GPIO_6__GPIO1_IO06 | MUX_PAD_CTRL(I2C_PAD_CTRL),
	 IMX_GPIO_NR(1, 6));


static int cm_fx6_setup_one_i2c(int busnum, struct i2c_pads_info *pads)
{
	int ret;

	ret = setup_i2c(busnum, CONFIG_SYS_I2C_SPEED, 0x7f, pads);
	if (ret)
		printf("Warning: I2C%d setup failed: %d\n", busnum, ret);

	return ret;
}

static int cm_fx6_setup_i2c(void)
{
	int ret = 0, err;

	/* i2c<x>_pads are wierd macro variables; we can't use an array */
	err = cm_fx6_setup_one_i2c(0, I2C_PADS_INFO(i2c0_pads));
	if (err)
		ret = err;
	err = cm_fx6_setup_one_i2c(1, I2C_PADS_INFO(i2c1_pads));
	if (err)
		ret = err;
	err = cm_fx6_setup_one_i2c(2, I2C_PADS_INFO(i2c2_pads));
	if (err)
		ret = err;

	return ret;
}
#else
static int cm_fx6_setup_i2c(void) { return 0; }
#endif

#ifdef CONFIG_USB_EHCI_MX6
#define WEAK_PULLDOWN	(PAD_CTL_PUS_100K_DOWN |		\
			PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |	\
			PAD_CTL_HYS | PAD_CTL_SRE_SLOW)
#define MX6_USBNC_BASEADDR	0x2184800
#define USBNC_USB_H1_PWR_POL	(1 << 9)

static int cm_fx6_setup_usb_host(void)
{
	int err;

	SETUP_IOMUX_PAD(PAD_SD3_RST__GPIO7_IO08 | MUX_PAD_CTRL(NO_PAD_CTRL));
	err = gpio_request(CM_FX6_USB_HUB_RST, "usb hub rst");
	if (err)
		return err;

	SETUP_IOMUX_PAD(PAD_GPIO_0__USB_H1_PWR | MUX_PAD_CTRL(NO_PAD_CTRL));
	err = gpio_request(CM_FX6_USB_HB1_PWR, "usb h1 pwr");
	if (err)
		return err;

	gpio_direction_output(CM_FX6_USB_HB1_PWR, 1);

	return 0;
}

static int cm_fx6_setup_usb_otg(void)
{
	int err;
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;

	err = gpio_request(SB_FX6_USB_OTG_PWR, "usb-pwr");
	if (err) {
		printf("USB OTG pwr gpio request failed: %d\n", err);
		return err;
	}

	SETUP_IOMUX_PAD(PAD_EIM_D22__GPIO3_IO22 | MUX_PAD_CTRL(NO_PAD_CTRL));
	SETUP_IOMUX_PAD(PAD_ENET_RX_ER__USB_OTG_ID |
						MUX_PAD_CTRL(WEAK_PULLDOWN));
	clrbits_le32(&iomux->gpr[1], IOMUXC_GPR1_OTG_ID_MASK);
	/* disable ext. charger detect, or it'll affect signal quality at dp. */
	return gpio_direction_output(SB_FX6_USB_OTG_PWR, 0);
}

int board_usb_phy_mode(int port)
{
	return USB_INIT_HOST;
}

int board_ehci_hcd_init(int port)
{
	int ret;
	u32 *usbnc_usb_uh1_ctrl = (u32 *)(MX6_USBNC_BASEADDR + 4);

	/* Only 1 host controller in use. port 0 is OTG & needs no attention */
	if (port != 1)
		return 0;

	/* Set PWR polarity to match power switch's enable polarity */
	setbits_le32(usbnc_usb_uh1_ctrl, USBNC_USB_H1_PWR_POL);
	ret = gpio_direction_output(CM_FX6_USB_HUB_RST, 0);
	if (ret)
		return ret;

	udelay(10);
	ret = gpio_direction_output(CM_FX6_USB_HUB_RST, 1);
	if (ret)
		return ret;

	mdelay(1);

	return 0;
}

int board_ehci_power(int port, int on)
{
	if (port == 0)
		return gpio_direction_output(SB_FX6_USB_OTG_PWR, on);

	return 0;
}
#else
static int cm_fx6_setup_usb_otg(void) { return 0; }
static int cm_fx6_setup_usb_host(void) { return 0; }
#endif

#ifdef CONFIG_FEC_MXC
#define ENET_PAD_CTRL_22K	(PAD_CTL_PUS_22K_UP | PAD_CTL_SPEED_MED | \
				 PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define ENET_PAD_CTRL		(PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED | \
				 PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

static int mx6_rgmii_rework(struct phy_device *phydev)
{
	unsigned short val;

	/* Ar8031 phy SmartEEE feature cause link status generates glitch,
	 * which cause ethernet link down/up issue, so disable SmartEEE
	 */
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x3);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, 0x805d);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x4003);
	val = phy_read(phydev, MDIO_DEVAD_NONE, 0xe);
	val &= ~(0x1 << 8);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, val);

	/* To enable AR8031 ouput a 125MHz clk from CLK_25M */
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x7);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, 0x8016);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x4007);

	val = phy_read(phydev, MDIO_DEVAD_NONE, 0xe);
	val &= 0xffe3;
	val |= 0x18;
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, val);

	/* introduce tx clock delay */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x5);
	val = phy_read(phydev, MDIO_DEVAD_NONE, 0x1e);
	val |= 0x0100;
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, val);

	/* introduce rx clock delay */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x0);
	val = phy_read(phydev, MDIO_DEVAD_NONE, 0x1e);
	val |= 0x08000;
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, val);

	return 0;
}

extern int phy_status;
int board_phy_config(struct phy_device *phydev)
{
	if (phy_status) {
        setenv("ext_phy" , "yes");
        return 0;
    }

	mx6_rgmii_rework(phydev);

	if (phydev->drv->config)
		return phydev->drv->config(phydev);

	return 0;
}

#ifdef CL_SOM_IMX6
static iomux_v3_cfg_t const heartbeat_pads[] = {
	IOMUX_PADS(PAD_EIM_EB3__GPIO2_IO31 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};
void cm_fx6_hb_led(void) {
	SETUP_IOMUX_PADS(heartbeat_pads);

	gpio_request(IMX_GPIO_NR(2, 31),"heart beat");
	gpio_direction_output(IMX_GPIO_NR(2, 31), 1);

};
#else
void cm_fx6_hb_led(void) {};
#endif

static iomux_v3_cfg_t const enet_pads[] = {
	IOMUX_PADS(PAD_ENET_REF_CLK__ENET_TX_CLK  |
						MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_ENET_MDIO__ENET_MDIO | MUX_PAD_CTRL(ENET_PAD_CTRL_22K)),
	IOMUX_PADS(PAD_ENET_MDC__ENET_MDC   | MUX_PAD_CTRL(ENET_PAD_CTRL)),

	IOMUX_PADS(PAD_RGMII_TX_CTL__RGMII_TX_CTL |
						MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_TXC__RGMII_TXC | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_TD0__RGMII_TD0 | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_TD1__RGMII_TD1 | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_TD2__RGMII_TD2 | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_TD3__RGMII_TD3 | MUX_PAD_CTRL(ENET_PAD_CTRL)),

	IOMUX_PADS(PAD_RGMII_RXC__RGMII_RXC | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_RD0__RGMII_RD0 | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_RD1__RGMII_RD1 | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_RD2__RGMII_RD2 | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_RD3__RGMII_RD3 | MUX_PAD_CTRL(ENET_PAD_CTRL)),
	IOMUX_PADS(PAD_RGMII_RX_CTL__RGMII_RX_CTL |
						MUX_PAD_CTRL(ENET_PAD_CTRL)),

#ifdef CL_SOM_IMX6
	IOMUX_PADS(PAD_EIM_A19__GPIO2_IO19 | MUX_PAD_CTRL(NO_PAD_CTRL)),
#else
	IOMUX_PADS(PAD_SD4_DAT0__GPIO2_IO08 | MUX_PAD_CTRL(0x84)),
#endif
};

static int handle_mac_address(char *env_var, uint eeprom_bus)
{
	unsigned char enetaddr[6];
	int rc;

	rc = eth_getenv_enetaddr(env_var, enetaddr);
	if (rc)
		return 0;

	rc = cl_eeprom_read_mac_addr(enetaddr, eeprom_bus);
	if (rc)
		return rc;

	if (!is_valid_ethaddr(enetaddr))
		return -1;

	return eth_setenv_enetaddr(env_var, enetaddr);
}

#define SB_FX6_I2C_EEPROM_BUS	0
#define NO_MAC_ADDR		"No MAC address found for %s\n"
int board_eth_init(bd_t *bis)
{
	if (handle_mac_address("ethaddr", CONFIG_SYS_I2C_EEPROM_BUS))
		printf(NO_MAC_ADDR, "primary NIC");

	gpio_request(IMX_GPIO_NR(1, 2),"i2cmux-switch");
	gpio_direction_output(IMX_GPIO_NR(1, 2), 0);

	if (handle_mac_address("eth1addr", SB_FX6_I2C_EEPROM_BUS))
		printf(NO_MAC_ADDR, "secondary NIC");

	SETUP_IOMUX_PADS(enet_pads);

	/* phy reset */
	gpio_request(PHY_ENET_NRST, "enet_nrst");
	gpio_direction_output(PHY_ENET_NRST, 0);
	udelay(1000);
	gpio_set_value(PHY_ENET_NRST, 1);
	udelay(1000);

	if (is_mx6dqp()) {
		/*
		 * select ENET MAC0 TX clock from PLL
		 */
		imx_iomux_set_gpr_register(5, 9, 1, 1);
		enable_fec_anatop_clock(ENET_125MHZ);
	}

	enable_enet_clk(1);
	return cpu_eth_init(bis);
}
#endif

#ifndef CONFIG_SPL_BUILD
static int nand_enabled = 0;
#endif

#ifdef CONFIG_NAND_MXS
static iomux_v3_cfg_t const nand_pads[] = {
	IOMUX_PADS(PAD_NANDF_CLE__NAND_CLE     | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_ALE__NAND_ALE     | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_CS0__NAND_CE0_B   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_RB0__NAND_READY_B | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_D0__NAND_DATA00   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_D1__NAND_DATA01   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_D2__NAND_DATA02   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_D3__NAND_DATA03   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_D4__NAND_DATA04   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_D5__NAND_DATA05   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_D6__NAND_DATA06   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_D7__NAND_DATA07   | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_SD4_CMD__NAND_RE_B      | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_SD4_CLK__NAND_WE_B      | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_WP_B__NAND_WP_B   | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

#ifdef CL_SOM_IMX6
#define NAND_ENABLE    IMX_GPIO_NR(6, 9)
static iomux_v3_cfg_t const nand_enable_pads[] = {
	IOMUX_PADS(PAD_NANDF_WP_B__GPIO6_IO09  | MUX_PAD_CTRL(PAD_CTL_PUS_100K_DOWN)),
};

static void get_nand_enable_state(void) {
	imx_iomux_v3_setup_multiple_pads(
	       nand_enable_pads, ARRAY_SIZE(nand_enable_pads));
	gpio_request(NAND_ENABLE,"NAND_ENABLE");
	gpio_direction_input(NAND_ENABLE);
	mdelay(1);
	nand_enabled = gpio_get_value(NAND_ENABLE);
	gpio_free(NAND_ENABLE);
}
#else
static void get_nand_enable_state(void) {
	nand_enabled = 1
}
#endif

static void cm_fx6_setup_gpmi_nand(void)
{
	get_nand_enable_state();

	SETUP_IOMUX_PADS(nand_pads);
	/* Enable clock roots */
	enable_usdhc_clk(1, 3);
	enable_usdhc_clk(1, 4);

	setup_gpmi_io_clk(MXC_CCM_CS2CDR_ENFC_CLK_PODF(0xf) |
			  MXC_CCM_CS2CDR_ENFC_CLK_PRED(1)   |
			  MXC_CCM_CS2CDR_ENFC_CLK_SEL(0));
}
#else
static void cm_fx6_setup_gpmi_nand(void) {}
#endif

#ifdef CONFIG_FSL_ESDHC
static struct fsl_esdhc_cfg usdhc_cfg[CONFIG_SYS_FSL_USDHC_NUM] = {
	{USDHC1_BASE_ADDR},
	{USDHC2_BASE_ADDR},
	{USDHC3_BASE_ADDR},
	{USDHC4_BASE_ADDR},
};

static enum mxc_clock usdhc_clk[CONFIG_SYS_FSL_USDHC_NUM] = {
	MXC_ESDHC_CLK,
	MXC_ESDHC2_CLK,
	MXC_ESDHC3_CLK,
	MXC_ESDHC4_CLK,
};

static iomux_v3_cfg_t const usdhc_wifi_pads[] = {
	IOMUX_PADS(PAD_NANDF_CS3__GPIO6_IO16	| MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_DA1__GPIO3_IO01	| MUX_PAD_CTRL(NO_PAD_CTRL)),
};

static void usdhc_wifi_mmc_init(void) {

	SETUP_IOMUX_PADS(usdhc_wifi_pads);

	gpio_request(IMX_GPIO_NR(6, 16),"6_16");
	gpio_request(IMX_GPIO_NR(3, 1),"3_01");

	gpio_direction_output(IMX_GPIO_NR(6, 16), 1);
	gpio_direction_output(IMX_GPIO_NR(3, 1), 1);
}

int board_mmc_init(bd_t *bis)
{
	int i;

	usdhc_wifi_mmc_init();
	cm_fx6_set_usdhc_iomux();
	for (i = 0; i < (CONFIG_SYS_FSL_USDHC_NUM - nand_enabled) ; i++) {
		usdhc_cfg[i].sdhc_clk = mxc_get_clock(usdhc_clk[i]);
		usdhc_cfg[i].max_bus_width = 4;
		fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
		enable_usdhc_clk(1, i);
	}

	return 0;
}
#endif

#ifdef CONFIG_MXC_SPI
int cm_fx6_setup_ecspi(void)
{
	cm_fx6_set_ecspi_iomux();
	return gpio_request(CM_FX6_ECSPI_BUS0_CS0, "ecspi_bus0_cs0");
}
#else
int cm_fx6_setup_ecspi(void) { return 0; }
#endif

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, bd_t *bd)
{
	u32 baseboard_rev;
	int nodeoffset;
	const char *usdhc3_path = "/soc/aips-bus@02100000/usdhc@02198000/";
	const char *usdhc4_path = "/soc/aips-bus@02100000/usdhc@0219c000/";
	const char *gpmi_nand = "/soc/gpmi-nand@00112000";
        const char *rtc_nand = "/soc/aips-bus@02100000/i2c@021a8000/rtc_nand@69";
        const char *rtc_emmc = "/soc/aips-bus@02100000/i2c@021a8000/rtc_emmc@69";
	char *baseboard_name;
	uint8_t enetaddr[6];

	/* MAC addr */
	if (eth_getenv_enetaddr("ethaddr", enetaddr)) {
		fdt_find_and_setprop(blob,
				     "/soc/aips-bus@02100000/ethernet@02188000",
				     "local-mac-address", enetaddr, 6, 1);
	}

	if (eth_getenv_enetaddr("eth1addr", enetaddr)) {
		fdt_find_and_setprop(blob, "/eth@pcie", "local-mac-address",
				     enetaddr, 6, 1);
	}

	baseboard_name = cl_eeprom_get_product_name(0);
	baseboard_rev = cl_eeprom_get_board_rev(0);
	if (!strncmp("SB-FX6m", baseboard_name, 7) && baseboard_rev <= 120) {
		fdt_shrink_to_minimum(blob); /* Make room for new properties */
		nodeoffset = fdt_path_offset(blob, usdhc3_path);
		fdt_delprop(blob, nodeoffset, "cd-gpios");
		fdt_find_and_setprop(blob, usdhc3_path, "non-removable",
				     NULL, 0, 1);
		fdt_find_and_setprop(blob, usdhc3_path, "keep-power-in-suspend",
				     NULL, 0, 1);
	}

	if (!nand_enabled) {
		/* nand disable */
		fdt_status_disabled_by_alias(blob, gpmi_nand);
		/* Disable an rtc that makes use of an emmc pin for irq */
		fdt_status_disabled_by_alias(blob, rtc_emmc);
	} else {
		/* emmc disable */
		fdt_status_disabled_by_alias(blob, usdhc4_path);
		/* Disable an rtc that makes use of a nand pin for irq */
		fdt_status_disabled_by_alias(blob, rtc_nand);
	}

	if (nand_enabled) {
		/* nand enable */
		fdt_status_okay_by_alias(blob, gpmi_nand);
		/* Enable an rtc that makes use of an emmc pin for irq */
		fdt_status_okay_by_alias(blob, rtc_emmc);
	} else {
		/* emmc enable */
		fdt_status_okay_by_alias(blob, usdhc4_path);
		/* Enable an rtc that makes use of a nand pin for irq */
		fdt_status_okay_by_alias(blob, rtc_nand);
	}

	return 0;
}
#endif

#define CPU_TYPE "cpu_type"
static int cm_fx6_cpu_type(void)
{
	switch (get_cpu_type_ext()) {
		case MXC_CPU_MX6SOLO:
			return setenv(CPU_TYPE, "MX6SOLO");
		case MXC_CPU_MX6D:
			return setenv(CPU_TYPE, "MX6D");
		case MXC_CPU_MX6Q:
			return setenv(CPU_TYPE, "MX6Q");
		case MXC_CPU_MX6DP:
			return setenv(CPU_TYPE, "MX6DP");
		case MXC_CPU_MX6QP:
			return setenv(CPU_TYPE, "MX6QP");
		default:
			return setenv(CPU_TYPE, "unknown");
	}
	return -1;
}

int board_init(void)
{
	int ret;

	cm_fx6_hb_led();

	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;
	cm_fx6_setup_gpmi_nand();

	ret = cm_fx6_setup_ecspi();
	if (ret)
		printf("Warning: ECSPI setup failed: %d\n", ret);

	ret = cm_fx6_setup_usb_otg();
	if (ret)
		printf("Warning: USB OTG setup failed: %d\n", ret);

	ret = cm_fx6_setup_usb_host();
	if (ret)
		printf("Warning: USB host setup failed: %d\n", ret);

	/*
	 * cm-fx6 may have iSSD not assembled and in this case it has
	 * bypasses for a (m)SATA socket on the baseboard. The socketed
	 * device is not controlled by those GPIOs. So just print a warning
	 * if the setup fails.
	 */
	ret = cm_fx6_setup_issd();
	if (ret)
		printf("Warning: iSSD setup failed: %d\n", ret);

	/* Warn on failure but do not abort boot */
	ret = cm_fx6_setup_i2c();
	if (ret)
		printf("Warning: I2C setup failed: %d\n", ret);

	cm_fx6_setup_display();

	return 0;
}

int board_late_init(void)
{
	cm_fx6_cpu_type();

#ifdef CONFIG_ATP_ENV
	setenv("atp" , "yes");
#endif

	return 0;
}

int checkboard(void)
{
	puts("Board: "CONFIG_BOARD_NAME"\n");
	return 0;
}

int misc_init_r(void)
{
	cl_print_pcb_info();

	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;

	switch (gd->ram_size) {
	case 0x10000000: /* DDR_16BIT_256MB */
		gd->bd->bi_dram[0].size = 0x10000000;
		gd->bd->bi_dram[1].size = 0;
		break;
	case 0x20000000: /* DDR_32BIT_512MB */
		gd->bd->bi_dram[0].size = 0x20000000;
		gd->bd->bi_dram[1].size = 0;
		break;
	case 0x40000000:
		if (is_cpu_type(MXC_CPU_MX6SOLO)) { /* DDR_32BIT_1GB */
			gd->bd->bi_dram[0].size = 0x20000000;
			gd->bd->bi_dram[1].size = 0x20000000;
		} else { /* DDR_64BIT_1GB */
			gd->bd->bi_dram[0].size = 0x40000000;
			gd->bd->bi_dram[1].size = 0;
		}
		break;
	case 0x80000000: /* DDR_64BIT_2GB */
		gd->bd->bi_dram[0].size = 0x40000000;
		gd->bd->bi_dram[1].size = 0x40000000;
		break;
	case 0xEFF00000: /* DDR_64BIT_4GB */
		gd->bd->bi_dram[0].size = 0x70000000;
		gd->bd->bi_dram[1].size = 0x7FF00000;
		break;
	default:
	        gd->bd->bi_dram[0].size = 0xDEADBEEF;
		break;
	}
}

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();
	switch (gd->ram_size) {
	case 0x10000000:
	case 0x20000000:
	case 0x40000000:
	case 0x80000000:
		break;
	case 0xF0000000:
		gd->ram_size -= 0x100000;
		break;
	default:
		printf("ERROR: Unsupported DRAM size 0x%lx\n", gd->ram_size);
		return -1;
	}

	return 0;
}

u32 get_board_rev(void)
{
	return cl_eeprom_get_board_rev(CONFIG_SYS_I2C_EEPROM_BUS);
}

static struct mxc_serial_platdata cm_fx6_mxc_serial_plat = {
	.reg = (struct mxc_uart *)UART4_BASE,
};

U_BOOT_DEVICE(cm_fx6_serial) = {
	.name	= "serial_mxc",
	.platdata = &cm_fx6_mxc_serial_plat,
};
