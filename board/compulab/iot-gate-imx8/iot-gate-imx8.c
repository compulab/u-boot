/*
 * Copyright 2020 CompuLab
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm-generic/gpio.h>
#include <fsl_esdhc.h>
#include <mmc.h>
#include <asm/arch/imx8mm_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <asm/arch/clock.h>
#include <spl.h>
#include <asm/mach-imx/dma.h>
#include <power/pmic.h>
#include <power/bd71837.h>
#include <usb.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/mach-imx/video.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_OF_BOARD_SETUP
#define IOT_GATE_IMX8_DTB_UART1_MODE_GPIO_RS232 {0x29000000,		\
0x1a000000, 0x01000000}
#define IOT_GATE_IMX8_DTB_PINCTRL_UART1 {				\
/* MX8MM_IOMUXC_SAI2_RXC_UART1_DCE_RX */				\
0xb4010000, 0x1c040000, 0xf4040000, 0x04000000,	0x03000000, 0x40010000,	\
/* MX8MM_IOMUXC_SAI2_RXFS_UART1_DCE_TX */				\
0xb0010000, 0x18040000, 0x00000000, 0x04000000,	0x00000000, 0x40010000,	\
/* MX8MM_IOMUXC_SAI2_TXFS_UART1_DCE_CTS_B */				\
0xbc010000, 0x24040000, 0x00000000, 0x04000000,	0x00000000, 0x40010000,	\
/* MX8MM_IOMUXC_SAI2_RXD0_UART1_DTE_RTS_B */				\
0xb8010000, 0x20040000, 0xf0040000, 0x04000000,	0x02000000, 0x40010000	\
}
#define IOT_GATE_IMX8_DTB_PATH_UART1_MODE "/regulator-uart1-mode"
#define IOT_GATE_IMX8_DTB_PATH_UART1 "/soc@0/bus@30800000/serial@30860000"
#define IOT_GATE_IMX8_DTB_PATH_PINCTRL_UART1 \
"/soc@0/bus@30000000/pinctrl@30330000/sb-iotgimx8/uart1grp"
/*
 * iot_gate_imx8_update_uart1_node() - update the device tree node uart1
 * The node is updated according to the requested UART mode: RS485 or  RS232.
 *
 * @blob: ptr to device tree
 */
static void iot_gate_imx8_update_uart1_node(void *blob)
{
	int ret, nodeoffset;
	char *env_uart_mode = env_get("uart_mode");
	u32 prop_data_gpio[] = IOT_GATE_IMX8_DTB_UART1_MODE_GPIO_RS232;
	u32 prop_data_pinctrl_uart1[] = IOT_GATE_IMX8_DTB_PINCTRL_UART1;

	if (env_uart_mode == NULL || strcmp(env_uart_mode, "rs232"))
		return;

	/* Update node regulator-uart1-mode */
	do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_UART1_MODE, "gpio",
			 prop_data_gpio, sizeof(prop_data_gpio), 0);
	do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_UART1_MODE, "enable-active-low",
			 NULL, 0, 1);
	nodeoffset = fdt_path_offset(blob, IOT_GATE_IMX8_DTB_PATH_UART1_MODE);
	if (nodeoffset < 0) {
		printf("%s: failed to locate the offset of the node uart1 mode "
		       "regulator\n", __func__);
		return;
	}
	ret = fdt_delprop(blob, nodeoffset, "enable-active-high");
	if (ret)
		printf("%s: failed to delete property enable-active-high\n",
		       __func__);

	/* Update node uart1 */
	nodeoffset = fdt_path_offset(blob, IOT_GATE_IMX8_DTB_PATH_UART1);
	if (nodeoffset < 0) {
		printf("%s: failed to locate the offset of node uart1\n",
		       __func__);
		return;
	}
	ret = fdt_delprop(blob, nodeoffset, "linux,rs485-enabled-at-boot-time");
	if (ret)
		printf("%s: failed to delete property "
		       "linux,rs485-enabled-at-boot-time\n", __func__);
	ret = fdt_delprop(blob, nodeoffset, "rts-gpios");
	if (ret)
		printf("%s: failed to delete property rts-gpios\n", __func__);
	ret = fdt_delprop(blob, nodeoffset, "rs485-rts-active-high");
	if (ret)
		printf("%s: failed to delete property rs485-rts-active-high\n", __func__);
	do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_UART1, "fsl,uart-has-rtscts",
			 NULL, 0, 1);

	/* Update node pinctrl_uart1 */
	do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_PINCTRL_UART1,
			 "fsl,pins", prop_data_pinctrl_uart1, 96, 0);
}

/*
 * sub_ft_board_setup() - IOT-GATE-iMX8 implementation of ft_board_setup
 *
 * @blob: ptr to device tree
 * @bd: board information record
 */
int sub_ft_board_setup(void *blob, struct bd_info *bd)
{
	int ret=0;

	iot_gate_imx8_update_uart1_node(blob);/* Update	dtb node uart1 */

	return ret;
}
#endif /* CONFIG_OF_BOARD_SETUP */
