/*
 * U-Boot board functions for CompuLab CL-SOM-iMX7 module
 *
 * (C) Copyright 2017 CompuLab, Ltd. http://www.compulab.com
 *
 * Author: Uri Mashiach <uri.mashiach@compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <mmc.h>
#include <phy.h>
#include <netdev.h>
#include <fsl_esdhc.h>
#include <pca953x.h>
#include <power/pmic.h>
#include <power/pfuze3000_pmic.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/arch-mx7/mx7-pins.h>
#include <asm/arch-mx7/sys_proto.h>
#include <asm/arch-mx7/clock.h>
#include <asm/setup.h>
#include <eeprom_layout.h>
#include "../common/eeprom.h"
#include "common.h"

DECLARE_GLOBAL_DATA_PTR;

static int nand_enabled = 0;

static uchar cl_som_am57x_eeprom_buf[CONFIG_SYS_EEPROM_SIZE];
static uchar sb_som_am57x_eeprom_buf[CONFIG_SYS_EEPROM_SIZE];
static struct eeprom_layout cl_som_am57x_layout;
static struct eeprom_layout sb_som_am57x_layout;

#ifdef CONFIG_SYS_I2C_MXC

/* Baseboard I2C bus is initialized flag */
int cl_som_imx7_base_i2c_init;

static struct i2c_pads_info cl_som_imx7_i2c_pad_info4 = {
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

/* Environment variable: base board I2C bus enable */
#define CL_SOM_IMX7_ENV_BASE_I2C "baseboard_i2c_enable"

/*
 * cl_som_imx7_setup_i2c() - I2C  pinmux configuration.
 */
static int cl_som_imx7_setup_i2c1(void)
{
	int ret;
	char *base_i2c = getenv(CL_SOM_IMX7_ENV_BASE_I2C);

	if (base_i2c && (!strcmp(base_i2c, "yes") ||
			 !strcmp(base_i2c, "true") ||
			 !strcmp(base_i2c, "1"))) {
		ret = setup_i2c(1, CONFIG_SYS_I2C_SPEED, 0x7f, &cl_som_imx7_i2c_pad_info4);
		if (ret)
			return 0;
		return 1;
	}

	return 0;
}
#else /* !CONFIG_SYS_I2C_MXC */
static int cl_som_imx7_setup_i2c1(void) { return 0; }
#endif /* CONFIG_SYS_I2C_MXC */

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();

	return 0;
}

#ifdef CONFIG_FSL_ESDHC

#define CL_SOM_IMX7_GPIO_USDHC3_PWR	IMX_GPIO_NR(6, 11)

static struct fsl_esdhc_cfg cl_som_imx7_usdhc_cfg[3] = {
	{USDHC1_BASE_ADDR, 0, 4},
	{USDHC3_BASE_ADDR},
};

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
			cl_som_imx7_usdhc1_pads_set();
			gpio_request(CL_SOM_IMX7_GPIO_USDHC1_CD, "usdhc1_cd");
			cl_som_imx7_usdhc_cfg[0].sdhc_clk =
				mxc_get_clock(MXC_ESDHC_CLK);
			break;
		case 1:
			if (nand_enabled)
				return 0; /* nand enabled configuration */
			/* emmc enabled configuration */
			cl_som_imx7_usdhc3_emmc_pads_set();
			gpio_request(CL_SOM_IMX7_GPIO_USDHC3_PWR, "usdhc3_pwr");
			gpio_direction_output(CL_SOM_IMX7_GPIO_USDHC3_PWR, 0);
			udelay(500);
			gpio_direction_output(CL_SOM_IMX7_GPIO_USDHC3_PWR, 1);
			cl_som_imx7_usdhc_cfg[1].sdhc_clk =
				mxc_get_clock(MXC_ESDHC3_CLK);
			break;
		default:
			printf("Warning: you configured more USDHC controllers "
				"(%d) than supported by the board\n", i + 1);
			return -EINVAL;
		}

		ret = fsl_esdhc_initialize(bis, &cl_som_imx7_usdhc_cfg[i]);
		if (ret)
			return ret;
	}

	return 0;
}
#endif /* CONFIG_FSL_ESDHC */

#ifdef CONFIG_NAND_MXS

#define CL_SOM_IMX7_NAND_ENABLE		IMX_GPIO_NR(6, 13)

static void get_nand_enable_state(void) {
	cl_som_imx7_nand_enable_pads_set();
	gpio_direction_input(CL_SOM_IMX7_NAND_ENABLE);
	mdelay(1);
	nand_enabled = gpio_get_value(CL_SOM_IMX7_NAND_ENABLE);
}

static void cl_som_imx7_setup_gpmi_nand(void)
{
	get_nand_enable_state();
	/* nand enabled configuration */
	cl_som_imx7_gpmi_nand_pads_set();
	set_clk_nand();
}
#else /* !CONFIG_NAND_MXS */
static void cl_som_imx7_setup_gpmi_nand(void) {}
#endif /* CONFIG_NAND_MXS */

#ifdef CONFIG_FEC_MXC

#define CL_SOM_IMX7_ETH1_PHY_NRST	IMX_GPIO_NR(1, 4)

/*
 * cl_som_imx7_rgmii_rework() - Ethernet PHY configuration.
 */
static void cl_som_imx7_rgmii_rework(struct phy_device *phydev)
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
}

int board_phy_config(struct phy_device *phydev)
{
	cl_som_imx7_rgmii_rework(phydev);

	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}

/*
 * cl_som_imx7_handle_mac_address() - set Ethernet MAC address environment.
 *
 * @env_var: MAC address environment variable
 * @eeprom_bus: I2C bus of the environment EEPROM
 * @eeprom_field_mac_addr: EEPROM field name of the MAC address 
 *
 * @return: 0 on success, < 0 on failure
 */
static int cl_som_imx7_handle_mac_address(char *env_var, uint eeprom_bus,
					  char *eeprom_field_mac_addr)
{
	int ret;
	unsigned char enetaddr[6];

	ret = eth_getenv_enetaddr(env_var, enetaddr);
	if (ret)
		return 0;

	ret = cl_eeprom_read_mac_addr(enetaddr, eeprom_bus);
	if (ret)
		return ret;

	ret = cl_som_am57x_layout.read(&cl_som_am57x_layout,
				      eeprom_field_mac_addr, enetaddr,
				      sizeof(enetaddr));
	if (ret)
		return ret;

	ret = is_valid_ethaddr(enetaddr);
	if (!ret)
		return -1;

	return eth_setenv_enetaddr(env_var, enetaddr);
}

#define CL_SOM_IMX7_FEC_DEV_ID_PRI 0
#define CL_SOM_IMX7_FEC_DEV_ID_SEC 1
#define CL_SOM_IMX7_PCA953X_PHY_RST_SEC 4
#define CL_SOM_IMX7_FEC_PHYADDR_PRI CONFIG_FEC_MXC_PHYADDR
#define CL_SOM_IMX7_FEC_PHYADDR_SEC 1

/*
 * cl_som_imx7_eth_init_pri() - primary Ethernet interface initialization
 *
 * @bis: board information
 *
 * @return: 0 on success, < 0 on failure
 */
static int cl_som_imx7_eth_init_pri(bd_t *bis)
{
	/* set Ethernet MAC address environment */
	cl_som_imx7_handle_mac_address("ethaddr", CONFIG_SYS_I2C_EEPROM_BUS,
				       "1st MAC Address");
	/* Ethernet interface pinmux configuration  */
	cl_som_imx7_phy1_rst_pads_set();
	cl_som_imx7_fec1_pads_set();
	/* PHY reset */
	gpio_request(CL_SOM_IMX7_ETH1_PHY_NRST, "eth1_phy_nrst");
	gpio_direction_output(CL_SOM_IMX7_ETH1_PHY_NRST, 0);
	mdelay(10);
	gpio_set_value(CL_SOM_IMX7_ETH1_PHY_NRST, 1);

	return fecmxc_initialize_multi(bis, CL_SOM_IMX7_FEC_DEV_ID_PRI,
				       CL_SOM_IMX7_FEC_PHYADDR_PRI,
				       IMX_FEC_BASE, IMX_FEC_BASE);
}

/*
 * cl_som_imx7_eth_init_pri() - secondary Ethernet interface initialization
 *
 * @bis: board information
 *
 * @return: 0 on success, < 0 on failure
 */
static int cl_som_imx7_eth_init_sec(bd_t *bis)
{
	int ret;
	u32 cpurev = get_cpu_rev();

	/* i.MX7Solo include 1 Ethernet interface */
	if (((cpurev & 0xFF000) >> 12) == MXC_CPU_MX7S)
		return 0;

	/* set Ethernet MAC address environment */
	cl_som_imx7_handle_mac_address("eth1addr", CONFIG_SYS_I2C_EEPROM_BUS,
				       "2nd MAC Address");
	/* Ethernet interface pinmux configuration  */
	cl_som_imx7_fec2_pads_set();
	/* PHY reset */
	ret = i2c_set_bus_num(SYS_I2C_BUS_SOM);
	if (ret != 0) {
		puts ("Failed to select the SOM I2C bus\n");
		return -1;
	}
	ret = pca953x_set_dir(CONFIG_SYS_I2C_PCA953X_ADDR,
			      1 << CL_SOM_IMX7_PCA953X_PHY_RST_SEC,
			      PCA953X_DIR_OUT <<
			      CL_SOM_IMX7_PCA953X_PHY_RST_SEC);
	ret |= pca953x_set_val(CONFIG_SYS_I2C_PCA953X_ADDR,
			       1 << CL_SOM_IMX7_PCA953X_PHY_RST_SEC, 0);
	mdelay(10);
	ret |= pca953x_set_val(CONFIG_SYS_I2C_PCA953X_ADDR,
			       1 << CL_SOM_IMX7_PCA953X_PHY_RST_SEC,
			       1 << CL_SOM_IMX7_PCA953X_PHY_RST_SEC);
	if (ret != 0) {
		puts ("Failed to reset the secondary Ethernet PHY\n");
		return -1;
	}
	/* MAC initialization with the primary MDIO bus */
	return fecmxc_initialize_multi(bis, CL_SOM_IMX7_FEC_DEV_ID_SEC,
				       CL_SOM_IMX7_FEC_PHYADDR_SEC,
				       ENET2_IPS_BASE_ADDR, IMX_FEC_BASE);
}

int board_eth_init(bd_t *bis)
{
	int ret;

	ret = cl_som_imx7_eth_init_pri(bis);
	if (ret)
		puts ("Ethernet initialization failure: primary interface\n");
	ret |= cl_som_imx7_eth_init_sec(bis);
	if (ret)
		puts ("Ethernet initialization failure: secondary interface\n");

	return ret;
}

/*
 * cl_som_imx7_setup_fec() - Ethernet MAC 1 clock configuration.
 * - ENET1 reference clock mode select.
 * - ENET1_TX_CLK output driver is disabled when configured for ALT1.
 */
static void cl_som_imx7_setup_fec(void)
{
	struct iomuxc_gpr_base_regs *const iomuxc_gpr_regs
		= (struct iomuxc_gpr_base_regs *)IOMUXC_GPR_BASE_ADDR;

	/* Use 125M anatop REF_CLK1 for ENET1, clear gpr1[13], gpr1[17]
	   Use 125M anatop REF_CLK2 for ENET2, clear gpr1[14], gpr1[18] */
	clrsetbits_le32(&iomuxc_gpr_regs->gpr[1],
			(IOMUXC_GPR_GPR1_GPR_ENET1_TX_CLK_SEL_MASK |
			 IOMUXC_GPR_GPR1_GPR_ENET2_TX_CLK_SEL_MASK |
			 IOMUXC_GPR_GPR1_GPR_ENET1_CLK_DIR_MASK |
			 IOMUXC_GPR_GPR1_GPR_ENET2_CLK_DIR_MASK), 0);

	set_clk_enet(ENET_125MHz);
}
#else /* !CONFIG_FEC_MXC */
static void cl_som_imx7_setup_fec(void) {}
#endif /* CONFIG_FEC_MXC */

#ifdef CONFIG_SPI

static void cl_som_imx7_spi_init(void)
{
	cl_som_imx7_espi1_pads_set();
}
#else /* !CONFIG_SPI */
static void cl_som_imx7_spi_init(void) {}
#endif /* CONFIG_SPI */

#define CL_SOM_IMX7_PRODUCT_NAME_SIZE 16

typedef struct {
       char name[CL_SOM_IMX7_PRODUCT_NAME_SIZE];
       char fdt_file[25];
} cl_som_imx7_base_board_param;

static cl_som_imx7_base_board_param cl_som_imx7_base_board_param_db[] = {
       {"SB-SOM", "imx7d-sbc-imx7.dtb"},
       {"SB-IOT", "imx7d-sbc-iot-imx7.dtb"},
       {"SBC-IOT", "imx7d-sbc-iot-imx7.dtb"},
       {"IOTG", "imx7d-sbc-iot-imx7.dtb"},
       {"other", "imx7d-cl-som-imx7.dtb"},
};

/* CL-SOM-iMX7 base board ID */
cl_som_imx7_base cl_som_imx7_base_id = CL_SOM_IMX7_OTHER;

/*
 * cl_som_imx7_get_baseboard_id() - determine baseboard ID.
 * Baseboard ID determined by the base board's
 * EEPROM filed "product name".
 */
static void cl_som_imx7_get_baseboard_id(void)
{
	int ret, i;
	char prod_name_base[CL_SOM_IMX7_PRODUCT_NAME_SIZE];

	if (sb_som_am57x_layout.data != sb_som_am57x_eeprom_buf)
		return;

	ret = sb_som_am57x_layout.read(&sb_som_am57x_layout, "Product Name",
				       (uchar*) prod_name_base,
				       CL_SOM_IMX7_PRODUCT_NAME_SIZE);

	if (ret) {
		prod_name_base[0] = 0;
		printf("Failed getting base board name\n");
	}

	for (i = 0; i < CL_SOM_IMX7_OTHER; i++) {
		if (!strncmp(prod_name_base,
			     cl_som_imx7_base_board_param_db[i].name,
			     CL_SOM_IMX7_PRODUCT_NAME_SIZE)) {
			cl_som_imx7_base_id = i;
			break;
		}
	}
}

#define CL_SOM_IMX7_FDT_FILE_NAME "fdtfile"
#define CL_SOM_IMX7_BOARD_ID_DEF CL_SOM_IMX7_OTHER

/*
 * cl_som_imx7_update_dtb_name() - update device tree blob file name.
 * Device tree file name determined base board ID
 */
static void cl_som_imx7_update_dtb_name(void)
{
	char *fdt_file = getenv(CL_SOM_IMX7_FDT_FILE_NAME);

	if (fdt_file) /* Device tree blob file name was set */
		return;

	if (cl_som_imx7_base_id < CL_SOM_IMX7_OTHER)
		setenv(CL_SOM_IMX7_FDT_FILE_NAME,
		       cl_som_imx7_base_board_param_db[cl_som_imx7_base_id].
		       fdt_file);
	else
		setenv(CL_SOM_IMX7_FDT_FILE_NAME,
		       cl_som_imx7_base_board_param_db
		       [CL_SOM_IMX7_BOARD_ID_DEF].fdt_file);
}

#define CL_SOM_IMX7_ENV_SERIAL "serial#"
#define CL_SOM_IMX7_SERIAL_BUF_BIN_SIZE 12
#define CL_SOM_IMX7_SERIAL_BUF_SIZE (2*CL_SOM_IMX7_SERIAL_BUF_BIN_SIZE+1)

/*
 * cl_som_imx7_set_serial_env() - set environment variable serial#.
 * The serial number is read from the EEPROM of the module or the mainboard.
 *
 */
static void cl_som_imx7_set_serial_env(void)
{
	char *serial_ptr = getenv(CL_SOM_IMX7_ENV_SERIAL);
	uchar serial_buf_bin[CL_SOM_IMX7_SERIAL_BUF_BIN_SIZE];
	char serial_buf[CL_SOM_IMX7_SERIAL_BUF_SIZE];
	int leading_zero = 1;
	int i;

	if (serial_ptr) /* Serial number was set */
		return;

	/* Read serial number from the EEPROM */
	switch (cl_som_imx7_base_id) {
	case CL_SOM_IMX7_SB_SOM:
		cl_som_am57x_layout.read(&cl_som_am57x_layout, "Serial Number",
					 (uchar*) &serial_buf_bin,
					 CL_SOM_IMX7_SERIAL_BUF_BIN_SIZE);
		break;
	case CL_SOM_IMX7_SB_IOT:
	case CL_SOM_IMX7_SBC_IOT:
	case CL_SOM_IMX7_IOTG:
		if (sb_som_am57x_layout.data != sb_som_am57x_eeprom_buf)
			break;
		sb_som_am57x_layout.read(&sb_som_am57x_layout, "Serial Number",
					 (uchar*) &serial_buf_bin,
					 CL_SOM_IMX7_SERIAL_BUF_BIN_SIZE);
		break;
	default:
		return;
	}

	memset(serial_buf, 0, CL_SOM_IMX7_SERIAL_BUF_SIZE);
	serial_ptr = serial_buf;
	/* Convert binary data to string */
	for (i = 0; i < CL_SOM_IMX7_SERIAL_BUF_BIN_SIZE; i++) {
		if (leading_zero && !serial_buf_bin[i])
			continue;
		if (leading_zero) {
			sprintf(serial_ptr, "%x", serial_buf_bin[i]);
			serial_ptr += strlen(serial_ptr);
			leading_zero = 0;
		}
		else {
			sprintf(serial_ptr, "%02x", serial_buf_bin[i]);
			serial_ptr += 2;
		}
	}
	setenv(CL_SOM_IMX7_ENV_SERIAL, serial_buf);
}

int board_early_init_f(void)
{
	cl_som_imx7_uart1_pads_set();
	cl_som_imx7_usb_otg1_pads_set();
	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;
	cl_som_imx7_setup_i2c0();
	cl_som_imx7_setup_gpmi_nand();
	cl_som_imx7_setup_fec();
	cl_som_imx7_spi_init();

	return 0;
}

#ifdef CONFIG_POWER
int power_init_board(void)
{
	struct pmic *p;
	int ret;
	unsigned int reg, rev_id;

	ret = power_pfuze3000_init(CL_SOM_IMX7_I2C_BUS_PMIC);
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
#endif /* CONFIG_POWER */

/*
 * cl_som_imx7_setup_wdog() - watchdog configuration.
 * - Output WDOG_B signal to reset external pmic.
 * - Suspend the watchdog timer during low-power modes.
 */
void cl_som_imx7_setup_wdog(void)
{
	struct wdog_regs *wdog = (struct wdog_regs *)WDOG1_BASE_ADDR;
	cl_som_imx7_wdog_pads_set();
	set_wdog_reset(wdog);
	/*
	* Do not assert internal WDOG_RESET_B_DEB(controlled by bit 4),
	* since we use PMIC_PWRON to reset the board.
	*/
	clrsetbits_le16(&wdog->wcr, 0, 0x10);
}

int board_late_init(void)
{
	int ret;

#ifdef CONFIG_VIDEO_MXS
	ret = enable_display();
	if (ret < 0)
		printf("Display enable failure\n");
#endif /* CONFIG_VIDEO_MXS */

	cl_som_imx7_setup_wdog();

	return 0;
}

int checkboard(void)
{
	int ret;
	char *mode;

	if (IS_ENABLED(CONFIG_ARMV7_BOOT_SEC_DEFAULT))
		mode = "secure";
	else
		mode = "non-secure";

	printf("Board: CL-SOM-iMX7 in %s mode\n", mode);

	setenv("board_name", "CL-SOM-iMX7");
	cl_som_imx7_base_i2c_init = cl_som_imx7_setup_i2c1();
	ret = cl_eeprom_layout_setup(&cl_som_am57x_layout,
				     cl_som_am57x_eeprom_buf,
				     LAYOUT_VERSION_AUTODETECT,
				     CONFIG_SYS_I2C_EEPROM_BUS,
				     CONFIG_SYS_I2C_EEPROM_ADDR);
	if (ret)
		printf("Module EEPROM layout initialization failure\n");
	if (cl_som_imx7_base_i2c_init)
		ret = cl_eeprom_layout_setup(&sb_som_am57x_layout,
					     sb_som_am57x_eeprom_buf,
					     LAYOUT_VERSION_AUTODETECT,
					     CL_SOM_IMX7_I2C_BUS_EXT,
					     CL_SOM_IMX7_I2C_EEPROM_EXT);
	if (ret)
		printf("Base board EEPROM layout initialization failure\n");
	cl_som_imx7_get_baseboard_id();
	cl_som_imx7_update_dtb_name();
	cl_som_imx7_set_serial_env();

	return 0;
}

#ifdef CONFIG_OF_BOARD_SETUP
#include <malloc.h>
#include "../common/common.h"

int fdt_board_adjust(void)
{
	int ret = 0;
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

#ifdef CONFIG_VIDEO
	/* Update display timing parameters */
	ret = fdt_board_adjust_display();
#endif /* CONFIG_VIDEO */

	return ret;
}
#endif /* CONFIG_OF_BOARD_SETUP */
