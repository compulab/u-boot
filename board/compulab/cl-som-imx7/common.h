/*
 * SPL/U-Boot common header file for CompuLab CL-SOM-iMX7 module
 *
 * (C) Copyright 2017 CompuLab, Ltd. http://www.compulab.com
 *
 * Author: Uri Mashiach <uri.mashiach@compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#define PADS_SET_PROT(pads_array) void cl_som_imx7_##pads_array##_set(void);

#ifdef CONFIG_FSL_ESDHC
#define CL_SOM_IMX7_GPIO_USDHC1_CD	IMX_GPIO_NR(5, 0)
PADS_SET_PROT(usdhc1_pads)
#endif /* CONFIG_FSL_ESDHC */
PADS_SET_PROT(uart1_pads)
#ifdef CONFIG_SPI
PADS_SET_PROT(espi1_pads)
#endif /* CONFIG_SPI */

PADS_SET_PROT(wdog_pads)

#ifndef CONFIG_SPL_BUILD
#ifdef CONFIG_FSL_ESDHC
PADS_SET_PROT(usdhc3_emmc_pads)
#endif /* CONFIG_FSL_ESDHC */
#ifdef CONFIG_FEC_MXC
PADS_SET_PROT(phy1_rst_pads)
PADS_SET_PROT(fec1_pads)
PADS_SET_PROT(fec2_pads)
#endif /* CONFIG_FEC_MXC */
PADS_SET_PROT(usb_otg1_pads)
PADS_SET_PROT(wdog_pads)
#ifdef CONFIG_NAND_MXS
PADS_SET_PROT(gpmi_nand_pads)
PADS_SET_PROT(nand_enable_pads)
#endif /* CONFIG_NAND_MXS */
#ifdef CONFIG_SYS_I2C_MXC
extern int cl_som_imx7_base_i2c_init;
#endif /* CONFIG_SYS_I2C_MXC */

/* CL-SOM-iMX7 base board ID */
typedef enum {
       CL_SOM_IMX7_SB_SOM,
       CL_SOM_IMX7_SB_IOT,
       CL_SOM_IMX7_SBC_IOT,
       CL_SOM_IMX7_IOTG,
       CL_SOM_IMX7_OTHER,
} cl_som_imx7_base;

extern cl_som_imx7_base cl_som_imx7_base_id;

#ifdef CONFIG_VIDEO_MXS
int enable_display(void);
int fdt_board_adjust_display(void);
PADS_SET_PROT(lcd_pads)
PADS_SET_PROT(lcd_control_pads)
#endif /* CONFIG_VIDEO_MXS */

#endif /* !CONFIG_SPL_BUILD */

void cl_som_imx7_setup_i2c0(void);
#define CL_SOM_IMX7_I2C_BUS_PMIC SYS_I2C_BUS_SOM

#if defined(CONFIG_SYS_I2C_MXC) || defined(CONFIG_SPL_I2C_SUPPORT)
#define I2C_PAD_CTRL		(PAD_CTL_DSE_3P3V_32OHM | PAD_CTL_SRE_SLOW | \
				PAD_CTL_HYS)
#endif /* CONFIG_SYS_I2C_MXC || CONFIG_SPL_I2C_SUPPORT */
