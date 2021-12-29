/*
 * (C) Copyright 2019 CompuLab, Ltd. <www.compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/arch/imx8mq_pins.h>
#include <asm/mach-imx/gpio.h>
#include <asm-generic/gpio.h>
#include <asm/setup.h>

#ifdef CONFIG_SPL_BUILD

#define CONFIG_SYS_I2C_EEPROM_ADDR_P1	0x51
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1

#ifndef CONFIG_SYS_I2C_EEPROM_BUS
#define CONFIG_SYS_I2C_EEPROM_BUS	1
#endif

static int cl_eeprom_bus = CONFIG_SYS_I2C_EEPROM_BUS;

static iomux_v3_cfg_t const eeprom_pads[] = {
	IMX8MQ_PAD_GPIO1_IO13__GPIO1_IO13 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

#define EEPROM_WP_GPIO IMX_GPIO_NR(1, 13)

static void cl_eeprom_we(int enable)
{
	static int done = 0;

	if (done) {
		gpio_direction_output(EEPROM_WP_GPIO, enable);
		return;
	}

	imx_iomux_v3_setup_multiple_pads(
		eeprom_pads, ARRAY_SIZE(eeprom_pads));
	gpio_request(EEPROM_WP_GPIO, "eeprom_wp");
	gpio_direction_output(EEPROM_WP_GPIO, enable);
	done = 1;
}

static int cl_eeprom_read(uint offset, uchar *buf, int len)
{
	int res;

	i2c_set_bus_num(cl_eeprom_bus);

	res = i2c_read(CONFIG_SYS_I2C_EEPROM_ADDR_P1, offset,
			CONFIG_SYS_I2C_EEPROM_ADDR_LEN, buf, len);

	return res;
}

static int cl_eeprom_write(uint offset, uchar *buf, int len)
{
	int res;

	cl_eeprom_we(1);

	i2c_set_bus_num(cl_eeprom_bus);

	res = i2c_write(CONFIG_SYS_I2C_EEPROM_ADDR_P1, offset,
			CONFIG_SYS_I2C_EEPROM_ADDR_LEN, buf, len);

	/*cl_eeprom_we(0);*/

	return res;
}

/* Reserved for fututre use area */
#define BOARD_DDRINFO_OFFSET 0x40
#define BOARD_DDRINFO_SIZE 4
static u32 board_ddrinfo = 0xdeadbeef;

#define BOARD_DDRSUBIND_OFFSET 0x44
#define BOARD_DDRSUBIND_SIZE 1
static u8 board_ddrsubind = 0xff;

#define BOARD_DRATE_OFFSET 0x50
#define BOARD_DRATE_SIZE 4
static u32 board_drate = 0xdeadbeef;

#define BOARD_OSIZE_OFFSET 0x80
#define BOARD_OSIZE_SIZE 4
static u32 board_osize = 0xdeadbeef;

#define BOARD_DDRINFO_VALID(A) (A != 0xdeadbeef)

u32 cl_eeprom_get_ddrinfo(void)
{
	if (!BOARD_DDRINFO_VALID(board_ddrinfo)) {
		if (cl_eeprom_read(BOARD_DDRINFO_OFFSET, (uchar *)&board_ddrinfo, BOARD_DDRINFO_SIZE))
			return 0;
	}
	return board_ddrinfo;
};

u32 cl_eeprom_set_ddrinfo(u32 ddrinfo)
{
	if (cl_eeprom_write(BOARD_DDRINFO_OFFSET, (uchar *)&ddrinfo, BOARD_DDRINFO_SIZE))
		return 0;

	board_ddrinfo = ddrinfo;

	return board_ddrinfo;
};

#define DRATE_OFFSET(R,C) (BOARD_DRATE_OFFSET + ( 0x10 * (R) ) + ( (C) << 2 ))
u32 cl_eeprom_get_drate(unsigned int r, unsigned int c)
{
	if (cl_eeprom_read(DRATE_OFFSET(r,c) , (uchar *)&board_drate, BOARD_DRATE_SIZE))
		return 0;

	return board_drate;
};

u32 cl_eeprom_set_drate(u32 drate, unsigned int r, unsigned int c)
{
	if (cl_eeprom_write(DRATE_OFFSET(r,c), (uchar *)&drate, BOARD_DRATE_SIZE))
		return 0;

	board_drate = drate;

	udelay(5000);

	return board_drate;
};

u8 cl_eeprom_get_subind(void)
{
	if (cl_eeprom_read(BOARD_DDRSUBIND_OFFSET, (uchar *)&board_ddrsubind, BOARD_DDRSUBIND_SIZE))
		return 0xff;

	return board_ddrsubind;
};

u8 cl_eeprom_set_subind(u8 ddrsubind)
{
	if (cl_eeprom_write(BOARD_DDRSUBIND_OFFSET, (uchar *)&ddrsubind, BOARD_DDRSUBIND_SIZE))
		return 0xff;
	board_ddrsubind = ddrsubind;

	return board_ddrsubind;
};

/* override-size ifaces */
u32 cl_eeprom_get_osize(void)
{
	if (cl_eeprom_read(BOARD_OSIZE_OFFSET, (uchar *)&board_osize, BOARD_OSIZE_SIZE))
		return 0;

	return board_osize;
};

u32 cl_eeprom_set_osize(u32 osize)
{
	if (cl_eeprom_write(BOARD_OSIZE_OFFSET, (uchar *)&osize, BOARD_OSIZE_SIZE))
		return 0;

	board_osize = osize;

	return board_osize;
};

#endif
