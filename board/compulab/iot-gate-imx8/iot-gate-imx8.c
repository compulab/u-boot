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

/* IOT_GATE-iMX8 extension boards ID */
typedef enum {
	IOT_GATE_EXT_EMPTY, /* No extension */
	IOT_GATE_EXT_CAN,   /* CAN bus */
	IOT_GATE_EXT_IED,   /* Bridge */
	IOT_GATE_EXT_POE,   /* POE */
	IOT_GATE_EXT_POEV2, /* POEv2 */
} iot_gate_imx8_ext;

/* Sevice tree blob per extention board */
#define IOT_GATE_IMX8_DTB_NAME_SIZE 25
static char iot_gate_imx8_dtb[][IOT_GATE_IMX8_DTB_NAME_SIZE] = {
	"sb-iotgimx8.dtb",
	"sb-iotgimx8-can.dtb",
	"sb-iotgimx8-ied.dtb",
	"sb-iotgimx8-poed.dtb",
	"sb-iotgimx8-poedv2.dtb",
};

#define IOT_GATE_IMX8_EXT_I2C 3 /* I2C ID of the extension board */
#define IOT_GATE_IMX8_EXT_I2C_ADDR_EEPROM 0x54 /* I2C address of the EEPROM */
/* I2C address of the EEPROM in the POE extension */
#define IOT_GATE_IMX8_EXT_I2C_ADDR_EEPROM_POE 0x50
#define IOT_GATE_IMX8_EXT_I2C_ADDR_EEPROM_POEV2 0x51
#define IOT_GATE_IMX8_EXT_I2C_ADDR_GPIO 0x22 /* I2C address of the GPIO
						extender */
static int iot_gate_imx8_ext_id = IOT_GATE_EXT_EMPTY; /* Extension board ID */
/*
 * iot_gate_imx8_detect_ext() - extended board detection
 * The detection is done according to the detected I2C devices.
 */
static void iot_gate_imx8_detect_ext(void)
{
	int ret;
	struct udevice *i2c_bus, *i2c_dev;

	ret = uclass_get_device_by_seq(UCLASS_I2C, IOT_GATE_IMX8_EXT_I2C,
				       &i2c_bus);
	if (ret) {
		printf("%s: Failed getting i2c device\n", __func__);
		return;
	}

	ret = dm_i2c_probe(i2c_bus, IOT_GATE_IMX8_EXT_I2C_ADDR_EEPROM_POE, 0,
			   &i2c_dev);
	if (!ret) {
		iot_gate_imx8_ext_id = IOT_GATE_EXT_POE;
		return;
	}

	ret = dm_i2c_probe(i2c_bus, IOT_GATE_IMX8_EXT_I2C_ADDR_EEPROM_POEV2, 0,
			   &i2c_dev);
	if (!ret) {
		iot_gate_imx8_ext_id = IOT_GATE_EXT_POEV2;
		return;
	}

	ret = dm_i2c_probe(i2c_bus, IOT_GATE_IMX8_EXT_I2C_ADDR_EEPROM, 0,
			   &i2c_dev);
	if (ret){
		iot_gate_imx8_ext_id = IOT_GATE_EXT_EMPTY;
		return;
	}
	/* Only the bridge extension includes the GPIO extender */
	ret = dm_i2c_probe(i2c_bus, IOT_GATE_IMX8_EXT_I2C_ADDR_GPIO, 0,
			   &i2c_dev);
	if (ret) /* GPIO extender not detected */
		iot_gate_imx8_ext_id = IOT_GATE_EXT_CAN;
	else /* GPIO extender detected */
		iot_gate_imx8_ext_id = IOT_GATE_EXT_IED;
}

#define IOT_GATE_IMX8_ENV_FDT_FILE "fdt_file"
/*
 * iot_gate_imx8_select_dtb() - select the kernel device tree blob
 * The device tree blob is selected according to the detected extended board.
 */
static void iot_gate_imx8_select_dtb(void)
{
	char *env_fdt_file = env_get(IOT_GATE_IMX8_ENV_FDT_FILE);

	if (env_fdt_file == NULL)
		env_set(IOT_GATE_IMX8_ENV_FDT_FILE,
			iot_gate_imx8_dtb[iot_gate_imx8_ext_id]);
}

static iomux_v3_cfg_t const iot_gate_imx8_ext_ied_pads[] = {
	IMX8MM_PAD_NAND_ALE_GPIO3_IO0	 | MUX_PAD_CTRL(PAD_CTL_PE),
	IMX8MM_PAD_NAND_CE0_B_GPIO3_IO1	 | MUX_PAD_CTRL(PAD_CTL_PE),
	IMX8MM_PAD_NAND_DATA00_GPIO3_IO6 | MUX_PAD_CTRL(PAD_CTL_PE),
	IMX8MM_PAD_NAND_DATA01_GPIO3_IO7 | MUX_PAD_CTRL(PAD_CTL_PE),
	IMX8MM_PAD_NAND_DATA02_GPIO3_IO8 | MUX_PAD_CTRL(PAD_CTL_PE),
	IMX8MM_PAD_NAND_DATA03_GPIO3_IO9 | MUX_PAD_CTRL(PAD_CTL_PE),
};
static iomux_v3_cfg_t const iot_gate_imx8_ext_poev2_pads[] = {
	IMX8MM_PAD_SAI3_TXD_GPIO5_IO1	 | MUX_PAD_CTRL(PAD_CTL_PE |
							PAD_CTL_PUE),
};
/* Extension board bridge GPIOs */
#define IOT_GATE_IMX8_GPIO_EXT_IED_I0 IMX_GPIO_NR(3, 0) /* IN 0 */
#define IOT_GATE_IMX8_GPIO_EXT_IED_I1 IMX_GPIO_NR(3, 1) /* IN 1 */
#define IOT_GATE_IMX8_GPIO_EXT_IED_I2 IMX_GPIO_NR(3, 6) /* IN 2 */
#define IOT_GATE_IMX8_GPIO_EXT_IED_I3 IMX_GPIO_NR(3, 7) /* IN 3 */
#define IOT_GATE_IMX8_GPIO_EXT_IED_O0 IMX_GPIO_NR(3, 8) /* OUT 0 */
#define IOT_GATE_IMX8_GPIO_EXT_IED_O1 IMX_GPIO_NR(3, 9) /* OUT 1 */
#define IOT_GATE_IMX8_GPIO_EXT_IED_O2 IMX_GPIO_NR(6, 9) /* OUT 2 */
#define IOT_GATE_IMX8_GPIO_EXT_IED_O3 IMX_GPIO_NR(6, 10)/* OUT 3 */
/* Extension board POE GPIOs */
#define IOT_GATE_IMX8_GPIO_EXT_POE_MUX IMX_GPIO_NR(5, 1)/* USB_MUX */
/*
 * iot_gate_imx8_update_pinmux() - update the pinmux
 * Update the pinmux according to the detected extended board.
 */
static void iot_gate_imx8_update_pinmux(void)
{
	if (iot_gate_imx8_ext_id == IOT_GATE_EXT_POEV2) {
		imx_iomux_v3_setup_multiple_pads(iot_gate_imx8_ext_poev2_pads,
				ARRAY_SIZE(iot_gate_imx8_ext_poev2_pads));
		gpio_request(IOT_GATE_IMX8_GPIO_EXT_POE_MUX, "poev2_usb-mux");
		/* Update USB MUX state */
		gpio_direction_output(IOT_GATE_IMX8_GPIO_EXT_POE_MUX, 1);

		return;
	}
	if (iot_gate_imx8_ext_id != IOT_GATE_EXT_IED)
		return;

	imx_iomux_v3_setup_multiple_pads(iot_gate_imx8_ext_ied_pads,
					 ARRAY_SIZE(iot_gate_imx8_ext_ied_pads));

	gpio_request(IOT_GATE_IMX8_GPIO_EXT_IED_I0, "ied-di4o4_i0");
	gpio_direction_input(IOT_GATE_IMX8_GPIO_EXT_IED_I0);
	gpio_request(IOT_GATE_IMX8_GPIO_EXT_IED_I1, "ied-di4o4_i1");
	gpio_direction_input(IOT_GATE_IMX8_GPIO_EXT_IED_I1);
	gpio_request(IOT_GATE_IMX8_GPIO_EXT_IED_I2, "ied-di4o4_i2");
	gpio_direction_input(IOT_GATE_IMX8_GPIO_EXT_IED_I2);
	gpio_request(IOT_GATE_IMX8_GPIO_EXT_IED_I3, "ied-di4o4_i3");
	gpio_direction_input(IOT_GATE_IMX8_GPIO_EXT_IED_I3);
	gpio_request(IOT_GATE_IMX8_GPIO_EXT_IED_O0, "ied-di4o4_o0");
	gpio_direction_output(IOT_GATE_IMX8_GPIO_EXT_IED_O0, 0);
	gpio_request(IOT_GATE_IMX8_GPIO_EXT_IED_O1, "ied-di4o4_o1");
	gpio_direction_output(IOT_GATE_IMX8_GPIO_EXT_IED_O1, 0);
	gpio_request(IOT_GATE_IMX8_GPIO_EXT_IED_O2, "ied-di4o4_o2");
	gpio_direction_output(IOT_GATE_IMX8_GPIO_EXT_IED_O2, 0);
	gpio_request(IOT_GATE_IMX8_GPIO_EXT_IED_O3, "ied-di4o4_o3");
	gpio_direction_output(IOT_GATE_IMX8_GPIO_EXT_IED_O3, 0);
}

/*
 * sub_board_late_init() - IOT-GATE-iMX8 implementation of board_late_init
 */
int sub_board_late_init(void)
{
	iot_gate_imx8_detect_ext(); /* Extended board detection */
	iot_gate_imx8_select_dtb(); /* Kernel device tree blob selection */
	iot_gate_imx8_update_pinmux();
	return 0;
}

#ifdef CONFIG_OF_BOARD_SETUP
#define IOT_GATE_IMX8_GPIO_S0B0 IMX_GPIO_NR(6, 0) /* Slot ID slot 0 bit 0 */
#define IOT_GATE_IMX8_GPIO_S0B1 IMX_GPIO_NR(6, 1) /* Slot ID slot 0 bit 1 */
#define IOT_GATE_IMX8_GPIO_S0B2 IMX_GPIO_NR(6, 2) /* Slot ID slot 0 bit 2 */
#define IOT_GATE_IMX8_GPIO_S1B0 IMX_GPIO_NR(6, 3) /* Slot ID slot 1 bit 0 */
#define IOT_GATE_IMX8_GPIO_S1B1 IMX_GPIO_NR(6, 4) /* Slot ID slot 1 bit 1 */
#define IOT_GATE_IMX8_GPIO_S1B2 IMX_GPIO_NR(6, 5) /* Slot ID slot 1 bit 2 */
#define IOT_GATE_IMX8_GPIO_S2B0 IMX_GPIO_NR(6, 6) /* Slot ID slot 2 bit 0 */
#define IOT_GATE_IMX8_GPIO_S2B1 IMX_GPIO_NR(6, 7) /* Slot ID slot 2 bit 1 */
#define IOT_GATE_IMX8_GPIO_S2B2 IMX_GPIO_NR(6, 8) /* Slot ID slot 2 bit 2 */
#define IOT_GATE_IMX8_CARD_ID_EMPTY  0 /* card id - uninhabited */
#define IOT_GATE_IMX8_CARD_ID_DI4O4  1 /* Card ID - IED-DI4O4   */
#define IOT_GATE_IMX8_CARD_ID_RS_485 2 /* Card ID - IED-RS485   */
#define IOT_GATE_IMX8_CARD_ID_TPM    3 /* Card ID - IED-TPM     */
#define IOT_GATE_IMX8_CARD_ID_CAN    4 /* Card ID - IED-CAN     */
#define IOT_GATE_IMX8_CARD_ID_CL420  5 /* Card ID - IED-CL420   */
#define IOT_GATE_IMX8_CARD_ID_RS_232 6 /* Card ID - IED-RS232   */
#define IOT_GATE_IMX8_DTB_PATH_UART2 "/soc@0/bus@30800000/serial@30890000"
#define IOT_GATE_IMX8_DTB_PATH_UART4 "/soc@0/bus@30800000/serial@30a60000"
#define IOT_GATE_IMX8_DTB_PATH_ECSPI2 "/soc@0/bus@30800000/spi@30830000"
#define IOT_GATE_IMX8_DTB_PATH_ECSPI3 "/soc@0/bus@30800000/spi@30840000"
#define IOT_GATE_IMX8_DTB_PATH_CAN0 "/soc@0/bus@30800000/spi@30830000/can@0"
#define IOT_GATE_IMX8_DTB_PATH_CAN1 "/soc@0/bus@30800000/spi@30840000/can@0"
#define IOT_GATE_IMX8_DTB_PATH_TPM0 "/soc@0/bus@30800000/spi@30830000/tpm@0"
#define IOT_GATE_IMX8_DTB_PATH_TPM1 "/soc@0/bus@30800000/spi@30840000/tpm@0"
#define IOT_GATE_IMX8_DTB_PATH_ADC0 "/soc@0/bus@30800000/spi@30830000/adc@0"
#define IOT_GATE_IMX8_DTB_PATH_ADC1 "/soc@0/bus@30800000/spi@30840000/adc@0"

/*
 * iot_gate_imx8_update_ext_ied()
 * Update device tree of the extended board IED-BASE.
 * The device tree is updated according to the detected sub modules.
 *
 * @blob: ptr to device tree
 *
 * Return 0 for success, 1 for failure.
 */
static int iot_gate_imx8_update_ext_ied(void *blob)
{
	int revision;

	if (iot_gate_imx8_ext_id != IOT_GATE_EXT_IED)
		return 0;

	/* ID GPIO initializations */
	if (gpio_request(IOT_GATE_IMX8_GPIO_S0B0, "id_s0b0") ||
	    gpio_request(IOT_GATE_IMX8_GPIO_S0B1, "id_s0b1") ||
	    gpio_request(IOT_GATE_IMX8_GPIO_S0B2, "id_s0b2") ||
	    gpio_request(IOT_GATE_IMX8_GPIO_S1B0, "id_s1b0") ||
	    gpio_request(IOT_GATE_IMX8_GPIO_S1B1, "id_s1b1") ||
	    gpio_request(IOT_GATE_IMX8_GPIO_S1B2, "id_s1b2") ||
	    gpio_request(IOT_GATE_IMX8_GPIO_S2B0, "id_s2b0") ||
	    gpio_request(IOT_GATE_IMX8_GPIO_S2B1, "id_s2b1") ||
	    gpio_request(IOT_GATE_IMX8_GPIO_S2B2, "id_s2b2")) {
		printf("%s: ID GPIO request failure\n", __func__);
		return 1;
	}
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S0B0);
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S0B1);
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S0B2);
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S1B0);
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S1B1);
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S1B2);
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S2B0);
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S2B1);
	gpio_direction_input(IOT_GATE_IMX8_GPIO_S2B2);

	/* Get slot 0 card ID */
	revision =	gpio_get_value(IOT_GATE_IMX8_GPIO_S0B0)		|
			gpio_get_value(IOT_GATE_IMX8_GPIO_S0B1) << 1	|
			gpio_get_value(IOT_GATE_IMX8_GPIO_S0B2) << 2;
	/* Slot 0 device tree blob manipulation */
	switch (revision)
	{
	case IOT_GATE_IMX8_CARD_ID_EMPTY:
		break;
	case IOT_GATE_IMX8_CARD_ID_RS_485:
	case IOT_GATE_IMX8_CARD_ID_RS_232:
		do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_UART2, "status",
				 "okay", sizeof("okay"), 1);
		break;
	case IOT_GATE_IMX8_CARD_ID_CAN:
	case IOT_GATE_IMX8_CARD_ID_TPM:
	case IOT_GATE_IMX8_CARD_ID_CL420:
		do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_ECSPI2, "status",
				 "okay", sizeof("okay"), 1);
		if (revision == IOT_GATE_IMX8_CARD_ID_CAN)
			do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_CAN0, "status",
					 "okay", sizeof("okay"), 1);
		else if (revision == IOT_GATE_IMX8_CARD_ID_TPM)
			do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_TPM0, "status",
					 "okay", sizeof("okay"), 1);
		else if (revision == IOT_GATE_IMX8_CARD_ID_CL420)
			do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_ADC0, "status",
					 "okay", sizeof("okay"), 1);
		break;
	default:
		printf("%s: invalid slot 0 card ID: %d\n", __func__, revision);
		return 1;
	}

	/* Get slot 1 card ID */
	revision =	gpio_get_value(IOT_GATE_IMX8_GPIO_S1B0)		|
			gpio_get_value(IOT_GATE_IMX8_GPIO_S1B1) << 1	|
			gpio_get_value(IOT_GATE_IMX8_GPIO_S1B2) << 2;
	/* Slot 0 device tree blob manipulation */
	switch (revision)
	{
	case IOT_GATE_IMX8_CARD_ID_EMPTY:
		break;
	case IOT_GATE_IMX8_CARD_ID_RS_485:
	case IOT_GATE_IMX8_CARD_ID_RS_232:
		do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_UART4, "status",
				 "okay", sizeof("okay"), 1);
		break;
	case IOT_GATE_IMX8_CARD_ID_CAN:
	case IOT_GATE_IMX8_CARD_ID_TPM:
	case IOT_GATE_IMX8_CARD_ID_CL420:
		do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_ECSPI3, "status",
				 "okay", sizeof("okay"), 1);
		if (revision == IOT_GATE_IMX8_CARD_ID_CAN)
			do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_CAN1, "status",
					 "okay", sizeof("okay"), 1);
		else if (revision == IOT_GATE_IMX8_CARD_ID_TPM)
			do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_TPM1, "status",
					 "okay", sizeof("okay"), 1);
		else if (revision == IOT_GATE_IMX8_CARD_ID_CL420)
			do_fixup_by_path(blob, IOT_GATE_IMX8_DTB_PATH_ADC1, "status",
					 "okay", sizeof("okay"), 1);
		break;
	default:
		printf("%s: invalid slot 1 card ID: %d\n", __func__, revision);
		return 1;
	}

	/* Get slot 2 card ID */
	revision =	gpio_get_value(IOT_GATE_IMX8_GPIO_S2B0)		|
			gpio_get_value(IOT_GATE_IMX8_GPIO_S2B1) << 1	|
			gpio_get_value(IOT_GATE_IMX8_GPIO_S2B2) << 2;
	/* Slot 0 device tree blob manipulation */
	switch (revision)
	{
	case IOT_GATE_IMX8_CARD_ID_EMPTY:
	case IOT_GATE_IMX8_CARD_ID_DI4O4:
		break;
	default:
		printf("%s: invalid slot 2 card ID: %d\n", __func__, revision);
		return 1;
	}

	return 0;
}

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
	prop_data_gpio[0]=
		fdt_getprop_u32_default(blob, IOT_GATE_IMX8_DTB_PATH_UART1_MODE,
					"gpio", 0);
	prop_data_gpio[0] = cpu_to_fdt32(prop_data_gpio[0]);
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
	/* Update device tree of the extended board IED-BASE */
	ret = iot_gate_imx8_update_ext_ied(blob);
	if (ret)
		printf("%s: Failt to Update device tree of the extended boarda "
		       "IED-BASE\n", __func__);

	return ret;
}
#endif /* CONFIG_OF_BOARD_SETUP */
