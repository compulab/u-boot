/*
 * Copyright (C) 2015 CompuLab, Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx7-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <common.h>
#include <fsl_esdhc.h>
#include <mmc.h>
#include <miiphy.h>
#include <netdev.h>
#include <power/pmic.h>
#include <power/pfuze3000_pmic.h>
#include "../../freescale/common/pfuze.h"
#include <i2c.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/arch/crm_regs.h>
#include <usb.h>
#include <usb/ehci-ci.h>
#include "../common/eeprom.h"

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL		(PAD_CTL_DSE_3P3V_49OHM | \
				PAD_CTL_PUS_PU100KOHM | PAD_CTL_HYS)

#define GPIO_PAD_CTRL		(PAD_CTL_PUS_PU5KOHM | PAD_CTL_PUE | \
				PAD_CTL_SRE_SLOW)

#ifdef CONFIG_SYS_I2C_MXC

#define I2C_PAD_CTRL		(PAD_CTL_DSE_3P3V_32OHM | PAD_CTL_SRE_SLOW | \
				PAD_CTL_HYS)
/* I2C2 */
static struct i2c_pads_info i2c_pad_info2 = {
	.scl = {
		.i2c_mode = MX7D_PAD_GPIO1_IO06__I2C2_SCL |
			MUX_PAD_CTRL(I2C_PAD_CTRL),
		.gpio_mode = MX7D_PAD_GPIO1_IO06__GPIO1_IO6 |
			MUX_PAD_CTRL(I2C_PAD_CTRL),
		.gp = IMX_GPIO_NR(1, 6),
	},
	.sda = {
		.i2c_mode = MX7D_PAD_GPIO1_IO07__I2C2_SDA |
			MUX_PAD_CTRL(I2C_PAD_CTRL),
		.gpio_mode = MX7D_PAD_GPIO1_IO07__GPIO1_IO7 |
			MUX_PAD_CTRL(I2C_PAD_CTRL),
		.gp = IMX_GPIO_NR(1, 7),
	},
};

static struct i2c_pads_info i2c_pad_info4 = {
       .scl = {
               .i2c_mode = MX7D_PAD_GPIO1_IO10__I2C4_SCL |
                       MUX_PAD_CTRL(I2C_PAD_CTRL),
               .gpio_mode = MX7D_PAD_GPIO1_IO10__GPIO1_IO10 |
                       MUX_PAD_CTRL(I2C_PAD_CTRL),
               .gp = IMX_GPIO_NR(1, 10),
       },
       .sda = {
               .i2c_mode = MX7D_PAD_GPIO1_IO11__I2C4_SDA |
                       MUX_PAD_CTRL(I2C_PAD_CTRL),
               .gpio_mode = MX7D_PAD_GPIO1_IO11__GPIO1_IO11 |
                       MUX_PAD_CTRL(I2C_PAD_CTRL),
               .gp = IMX_GPIO_NR(1, 11),
       },
};
#endif

static int nand_enabled = 0;

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_SIZE; /* FIXME: only hardcoded value */

	return 0;
}

static iomux_v3_cfg_t const uart1_pads[] = {
	MX7D_PAD_UART1_TX_DATA__UART1_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX7D_PAD_UART1_RX_DATA__UART1_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

static iomux_v3_cfg_t const usb_otg1_pads[] = {
	MX7D_PAD_GPIO1_IO05__USB_OTG1_PWR | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_otg(void)
{
	imx_iomux_v3_setup_multiple_pads(usb_otg1_pads, ARRAY_SIZE(usb_otg1_pads));
}

#if defined(CONFIG_STATUS_LED) && defined(STATUS_LED_BOOT)
static iomux_v3_cfg_t const status_led_pads[] = {
	MX7D_PAD_SAI1_TX_SYNC__GPIO6_IO14 | MUX_PAD_CTRL(GPIO_PAD_CTRL),
};

static void setup_iomux_led(void)
{
	imx_iomux_v3_setup_multiple_pads(status_led_pads, ARRAY_SIZE(status_led_pads));
}


static void setup_led(void)
{
	setup_iomux_led();
	status_led_init();
	status_led_set(STATUS_LED_BOOT, STATUS_LED_ON);
}
#else
static void setup_led(void) {}
#endif

#ifdef CONFIG_FSL_ESDHC

#define USDHC_PAD_CTRL		(PAD_CTL_DSE_3P3V_32OHM | PAD_CTL_SRE_SLOW | \
				PAD_CTL_HYS | PAD_CTL_PUE | \
				PAD_CTL_PUS_PU47KOHM)

static iomux_v3_cfg_t const usdhc1_pads[] = {
	MX7D_PAD_SD1_CLK__SD1_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_CMD__SD1_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA0__SD1_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA1__SD1_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA2__SD1_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA3__SD1_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	MX7D_PAD_SD1_CD_B__GPIO5_IO0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc3_emmc_pads[] = {
	MX7D_PAD_SD3_CLK__SD3_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_CMD__SD3_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA4__SD3_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA5__SD3_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA6__SD3_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA7__SD3_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_STROBE__SD3_STROBE	 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	MX7D_PAD_SD3_RESET_B__GPIO6_IO11 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

#define USDHC1_CD_GPIO		IMX_GPIO_NR(5, 0)
#define USDHC3_PWR_GPIO		IMX_GPIO_NR(6, 11)

static struct fsl_esdhc_cfg usdhc_cfg[3] = {
	{USDHC1_BASE_ADDR, 0, 4},
	{USDHC3_BASE_ADDR},
};

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC1_BASE_ADDR:
		ret = !gpio_get_value(USDHC1_CD_GPIO);
		break;
	case USDHC3_BASE_ADDR:
		ret = 1; /* Assume uSDHC3 emmc is always present */
		break;
	}

	return ret;
}

int board_mmc_init(bd_t *bis)
{
	int i, ret;
	/*
	 * According to the board_mmc_init() the following map is done:
	 * (U-boot device node)    (Physical Port)
	 * mmc0                    USDHC1
	 * mmc2                    USDHC3 (eMMC)
	 */
	for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
		switch (i) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc1_pads, ARRAY_SIZE(usdhc1_pads));
			gpio_request(USDHC1_CD_GPIO, "usdhc1_cd");
			usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
			break;
		case 1:
			if (nand_enabled) {
			/* nand enabled configuration */
				return 0;
			}
			/* emmc enabled configuration */
			imx_iomux_v3_setup_multiple_pads(
				usdhc3_emmc_pads, ARRAY_SIZE(usdhc3_emmc_pads));
			gpio_request(USDHC3_PWR_GPIO, "usdhc3_pwr");
			gpio_direction_output(USDHC3_PWR_GPIO, 0);
			udelay(500);
			gpio_direction_output(USDHC3_PWR_GPIO, 1);
			usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
			break;
		default:
			printf("Warning: you configured more USDHC controllers"
				"(%d) than supported by the board\n", i + 1);
			return -EINVAL;
			}

			ret = fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
			if (ret)
				return ret;
	}

	return 0;
}
#endif

#ifdef CONFIG_NAND_MXS

#define GPMI_PAD_CTRL		(PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_PUE | \
				PAD_CTL_PUS_PU100KOHM | PAD_CTL_SRE_SLOW | \
				PAD_CTL_HYS)

static iomux_v3_cfg_t const gpmi_nand_pads[] = {
	MX7D_PAD_SD3_CLK__NAND_CLE | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_CMD__NAND_ALE | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_DATA0__NAND_DATA00 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_DATA1__NAND_DATA01 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_DATA2__NAND_DATA02 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_DATA3__NAND_DATA03 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_DATA4__NAND_DATA04 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_DATA5__NAND_DATA05 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_DATA6__NAND_DATA06 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_DATA7__NAND_DATA07 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_STROBE__NAND_RE_B	 | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SD3_RESET_B__NAND_WE_B | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SAI1_TX_BCLK__NAND_CE0_B | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SAI1_TX_DATA__NAND_READY_B | MUX_PAD_CTRL(GPMI_PAD_CTRL),
	MX7D_PAD_SAI1_MCLK__NAND_WP_B | MUX_PAD_CTRL(GPMI_PAD_CTRL),
};

#define NAND_ENABLE	IMX_GPIO_NR(6, 13)

static iomux_v3_cfg_t const nand_enable_pads[] = {
	MX7D_PAD_SAI1_TX_BCLK__GPIO6_IO13 | MUX_PAD_CTRL(0),
};

static void get_nand_enable_state(void) {
	imx_iomux_v3_setup_multiple_pads(nand_enable_pads,
					 ARRAY_SIZE(nand_enable_pads));
	gpio_direction_input(NAND_ENABLE);
	mdelay(1);
	nand_enabled = gpio_get_value(NAND_ENABLE);
}

static void setup_gpmi_nand(void)
{
	get_nand_enable_state();

	/* nand enabled configuration */
	imx_iomux_v3_setup_multiple_pads(
		gpmi_nand_pads, ARRAY_SIZE(gpmi_nand_pads));

	set_clk_nand();
}
#else
static void setup_gpmi_nand(void) {}
#endif

#ifdef CONFIG_FEC_MXC

#define ENET_PAD_CTRL		(PAD_CTL_PUS_PD100KOHM | PAD_CTL_DSE_3P3V_49OHM)
#define ENET_PAD_CTRL_MII	(PAD_CTL_PUS_PU5KOHM)

#define CL_SOM_IFX7_ETH1_PHY_NRST	IMX_GPIO_NR(1, 4)

static int mx7_rgmii_rework(struct phy_device *phydev)
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

	return 0;
}

int board_phy_config(struct phy_device *phydev)
{
	mx7_rgmii_rework(phydev);

	if (phydev->drv->config)
		phydev->drv->config(phydev);
	return 0;
}

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

static iomux_v3_cfg_t const fec1_pads[] = {
	MX7D_PAD_ENET1_RGMII_RX_CTL__ENET1_RGMII_RX_CTL | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_RD0__ENET1_RGMII_RD0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_RD1__ENET1_RGMII_RD1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_RD2__ENET1_RGMII_RD2 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_RD3__ENET1_RGMII_RD3 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_RXC__ENET1_RGMII_RXC | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TX_CTL__ENET1_RGMII_TX_CTL | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TD0__ENET1_RGMII_TD0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TD1__ENET1_RGMII_TD1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TD2__ENET1_RGMII_TD2 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TD3__ENET1_RGMII_TD3 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_ENET1_RGMII_TXC__ENET1_RGMII_TXC | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_SD2_CD_B__ENET1_MDIO | MUX_PAD_CTRL(ENET_PAD_CTRL_MII),
	MX7D_PAD_SD2_WP__ENET1_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL_MII),
};

static iomux_v3_cfg_t const phy1_rst_pads[] = {
	/* PHY1 RST */
	MX7D_PAD_GPIO1_IO04__GPIO1_IO4	| MUX_PAD_CTRL(GPIO_PAD_CTRL),
};

static void setup_iomux_fec(void)
{
	imx_iomux_v3_setup_multiple_pads(phy1_rst_pads, ARRAY_SIZE(phy1_rst_pads));
	imx_iomux_v3_setup_multiple_pads(fec1_pads, ARRAY_SIZE(fec1_pads));
}

int board_eth_init(bd_t *bis)
{
	if (handle_mac_address("ethaddr", CONFIG_SYS_I2C_EEPROM_BUS))
		printf("No primary MAC address found\n");

	setup_iomux_fec();

	/* phy reset */
	gpio_request(CL_SOM_IFX7_ETH1_PHY_NRST, "eth1_phy_nrst");
	gpio_direction_output(CL_SOM_IFX7_ETH1_PHY_NRST, 0);
	mdelay(10);
	gpio_set_value(CL_SOM_IFX7_ETH1_PHY_NRST, 1);

	return fecmxc_initialize_multi(bis, 0,
				       CONFIG_FEC_MXC_PHYADDR, IMX_FEC_BASE);
}

static void setup_fec(void)
{
	struct iomuxc_gpr_base_regs *const iomuxc_gpr_regs
		= (struct iomuxc_gpr_base_regs *) IOMUXC_GPR_BASE_ADDR;

	/* Use 125M anatop REF_CLK1 for ENET1, clear gpr1[13], gpr1[17]*/
	clrsetbits_le32(&iomuxc_gpr_regs->gpr[1],
		(IOMUXC_GPR_GPR1_GPR_ENET1_TX_CLK_SEL_MASK |
		 IOMUXC_GPR_GPR1_GPR_ENET1_CLK_DIR_MASK), 0);

	set_clk_enet(ENET_125MHz);
}
#else
static void setup_fec(void) {}
#endif

#ifdef CONFIG_SPI

#define SPI_PAD_CTRL	(PAD_CTL_HYS | PAD_CTL_SRE_SLOW | \
			PAD_CTL_DSE_3P3V_32OHM)

static iomux_v3_cfg_t const espi1_pads[] = {
	MX7D_PAD_ECSPI1_SCLK__ECSPI1_SCLK | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX7D_PAD_ECSPI1_MISO__ECSPI1_MISO | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX7D_PAD_ECSPI1_MOSI__ECSPI1_MOSI | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX7D_PAD_ECSPI1_SS0__GPIO4_IO19 | MUX_PAD_CTRL(GPIO_PAD_CTRL),
};

static void board_spi_init(void)
{
	imx_iomux_v3_setup_multiple_pads(espi1_pads, ARRAY_SIZE(espi1_pads));
}

int board_spi_cs_gpio(unsigned bus, unsigned cs)
{
	return IMX_GPIO_NR(4, 19);
}
#else
static void board_spi_init(void) {}
#endif

static iomux_v3_cfg_t const wdog_pads[] = {
	MX7D_PAD_GPIO1_IO00__WDOG1_WDOG_B | MUX_PAD_CTRL(NO_PAD_CTRL),
};

void setup_wdog(void)
{
	struct wdog_regs *wdog = (struct wdog_regs *)WDOG1_BASE_ADDR;

	imx_iomux_v3_setup_multiple_pads(wdog_pads, ARRAY_SIZE(wdog_pads));

	set_wdog_reset(wdog);

	/*
	 * Do not assert internal WDOG_RESET_B_DEB(controlled by bit 4),
	 * since we use PMIC_PWRON to reset the board.
	 */
	clrsetbits_le16(&wdog->wcr, 0, 0x10);
}

int board_early_init_f(void)
{
	setup_iomux_uart();

	setup_iomux_otg();

	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_SYS_I2C_MXC
	setup_i2c(0, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info2);
	setup_i2c(1, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info4);
#endif
	setup_gpmi_nand();

	setup_fec();

	board_spi_init();

	setup_led();

	return 0;
}

#ifdef CONFIG_POWER
#define I2C_PMIC	0
int power_init_board(void)
{
	struct pmic *p;
	int ret;
	unsigned int reg, rev_id;

	ret = power_pfuze3000_init(I2C_PMIC);
	if (ret)
		return ret;

	p = pmic_get("PFUZE3000");
	ret = pmic_probe(p);
	if (ret)
		return ret;

	pmic_reg_read(p, PFUZE3000_DEVICEID, &reg);
	pmic_reg_read(p, PFUZE3000_REVID, &rev_id);
	printf("PMIC: PFUZE3000 DEV_ID=0x%x REV_ID=0x%x\n", reg, rev_id);

	/* disable Low Power Mode during standby mode */
	pmic_reg_write(p, PFUZE3000_LDOGCTL, 0x1);

	return 0;
}
#endif

int board_late_init(void)
{
	setenv("board_name", "CL-SOM-iMX7");

	setup_wdog();

	return 0;
}

int checkboard(void)
{
	char *mode;

	if (IS_ENABLED(CONFIG_ARMV7_BOOT_SEC_DEFAULT))
		mode = "secure";
	else
		mode = "non-secure";

	printf("Board: CL-SOM-iMX7 in %s mode\n", mode);

	return 0;
}

#ifdef CONFIG_OF_BOARD_SETUP
#include <malloc.h>
#include "../common/common.h"

int fdt_board_adjust(void)
{
	u32 cpurev = get_cpu_rev();

	/* Disable features not supported by i.MX7Solo */
	if (((cpurev & 0xFF000) >> 12) == MXC_CPU_MX7S) {
		/* FEC2 with PHY */
		fdt_node_disable("/soc/aips-bus@30800000/ethernet@30bf0000");
		fdt_node_disable("/soc/aips-bus@30800000/ethernet@30be0000/mdio/ethernet-phy@1");
		/* PCIe */
		fdt_node_disable("/soc/pcie@0x33800000");
		/* USB Host HSIC */
		fdt_node_disable("/soc/aips-bus@30800000/usb@30b20000");
	}

	/* Main storage setup */
	if (nand_enabled) {
		/* Enable GPMI and disable eMMC */
		fdt_node_enable("/soc/gpmi-nand@33002000");
		fdt_node_disable("/soc/aips-bus@30800000/usdhc@30b60000");
	} else {
		/* Enable eMMC and disable GPMI */
		fdt_node_enable("/soc/aips-bus@30800000/usdhc@30b60000");
		fdt_node_disable("/soc/gpmi-nand@33002000");
	}

	return 0;
}
#endif

