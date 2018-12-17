/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * SPL/U-Boot common header file for CompuLab CL-SOM-iMX7 module
 *
 * (C) Copyright 2017 CompuLab, Ltd. http://www.compulab.com
 *
 * Author: Uri Mashiach <uri.mashiach@compulab.co.il>
 */

#define PADS_SET_PROT(pads_array) void cl_som_imx7_##pads_array##_set(void)

#ifdef CONFIG_FSL_ESDHC
#define CL_SOM_IMX7_GPIO_USDHC1_CD	IMX_GPIO_NR(5, 0)
PADS_SET_PROT(usdhc1_pads);
#endif /* CONFIG_FSL_ESDHC */
PADS_SET_PROT(uart1_pads);
#ifdef CONFIG_SPI
PADS_SET_PROT(espi1_pads);
#endif /* CONFIG_SPI */

#ifndef CONFIG_SPL_BUILD
#ifdef CONFIG_FSL_ESDHC
PADS_SET_PROT(usdhc3_emmc_pads);
#endif /* CONFIG_FSL_ESDHC */
#ifdef CONFIG_FEC_MXC
PADS_SET_PROT(phy1_rst_pads);
PADS_SET_PROT(fec1_pads);
#endif /* CONFIG_FEC_MXC */
PADS_SET_PROT(usb_otg1_pads);
PADS_SET_PROT(wdog_pads);
#ifdef CONFIG_NAND_MXS
PADS_SET_PROT(gpmi_nand_pads);
PADS_SET_PROT(nand_enable_pads);
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

#endif /* !CONFIG_SPL_BUILD */
