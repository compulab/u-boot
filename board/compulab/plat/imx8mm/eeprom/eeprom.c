/*
 * (C) Copyright 2011 CompuLab, Ltd. <www.compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <i2c.h>
#include <eeprom_layout.h>
#include <eeprom_field.h>
#include <linux/kernel.h>
#include <asm/setup.h>

#ifndef CONFIG_SYS_I2C_EEPROM_ADDR
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1
#endif

#ifndef CONFIG_SYS_I2C_EEPROM_BUS
#define CONFIG_SYS_I2C_EEPROM_BUS	1
#endif

#define EEPROM_LAYOUT_VER_OFFSET	44
#define BOARD_SERIAL_OFFSET		20
#define BOARD_SERIAL_OFFSET_LEGACY	8
#define BOARD_REV_OFFSET		0
#define BOARD_REV_OFFSET_LEGACY		6
#define BOARD_REV_SIZE			2
#define PRODUCT_NAME_OFFSET		128
#define PRODUCT_NAME_SIZE		16
#define MAC_ADDR_OFFSET			4
#define MAC_ADDR_OFFSET_LEGACY		0

#define LAYOUT_INVALID	0
#define LAYOUT_LEGACY	0xff

static int cpl_eeprom_bus;
static int cpl_eeprom_layout;

static struct udevice  *g_dev = NULL;

static int cpl_eeprom_init(void) {

	int i2c_bus = CONFIG_SYS_I2C_EEPROM_BUS;
	uint8_t chip = CONFIG_SYS_I2C_EEPROM_ADDR;

	struct udevice *bus, *dev;
	int ret;

	if (!g_dev) {

		ret = uclass_get_device_by_seq(UCLASS_I2C, i2c_bus, &bus);
		if (ret) {
			printf("%s: No bus %d\n", __func__, i2c_bus);
			return ret;
		}

		ret = dm_i2c_probe(bus, chip, 0, &dev);
		if (ret) {
			printf("%s: Can't find device id=0x%x, on bus %d\n",
				__func__, chip, i2c_bus);
			return ret;
		}

		/* Init */
		g_dev = dev;
	}

	return 0;
}

static int cpl_eeprom_read(uint offset, uchar *buf, int len)
{
	int res;

	res = cpl_eeprom_init();
	if (res < 0)
		return res;

	res  = dm_i2c_read(g_dev, offset, buf, len);

	return res;
}


static int cpl_eeprom_setup(uint eeprom_bus)
{
	int res;

	/*
	 * We know the setup was already done when the layout is set to a valid
	 * value and we're using the same bus as before.
	 */
	if (cpl_eeprom_layout != LAYOUT_INVALID && eeprom_bus == cpl_eeprom_bus)
		return 0;

	cpl_eeprom_bus = eeprom_bus;
	res = cpl_eeprom_read(EEPROM_LAYOUT_VER_OFFSET,
			     (uchar *)&cpl_eeprom_layout, 1);

	if (res) {
		cpl_eeprom_layout = LAYOUT_INVALID;
		return res;
	}

	if (cpl_eeprom_layout == 0 || cpl_eeprom_layout >= 0x20)
		cpl_eeprom_layout = LAYOUT_LEGACY;

	return 0;
}

/*
 * Routine: cpl_eeprom_read_mac_addr
 * Description: read mac address and store it in buf.
 */
int cpl_eeprom_read_mac_addr(uchar *buf, uint eeprom_bus)
{
	uint offset;
	int err;

	err = cpl_eeprom_setup(eeprom_bus);
	if (err)
		return err;

	offset = (cpl_eeprom_layout != LAYOUT_LEGACY) ?
			MAC_ADDR_OFFSET : MAC_ADDR_OFFSET_LEGACY;

	return cpl_eeprom_read(offset, buf, 6);
}

static u32 board_rev;

/*
 * Routine: cpl_eeprom_get_board_rev
 * Description: read system revision from eeprom
 */
u32 cpl_eeprom_get_board_rev(uint eeprom_bus)
{
	char str[5]; /* Legacy representation can contain at most 4 digits */
	uint offset = BOARD_REV_OFFSET_LEGACY;

	if (board_rev)
		return board_rev;

	if (cpl_eeprom_setup(eeprom_bus))
		return 0;

	if (cpl_eeprom_layout != LAYOUT_LEGACY)
		offset = BOARD_REV_OFFSET;

	if (cpl_eeprom_read(offset, (uchar *)&board_rev, BOARD_REV_SIZE))
		return 0;

	/*
	 * Convert legacy syntactic representation to semantic
	 * representation. i.e. for rev 1.00: 0x100 --> 0x64
	 */
	if (cpl_eeprom_layout == LAYOUT_LEGACY) {
		sprintf(str, "%x", board_rev);
		board_rev = simple_strtoul(str, NULL, 10);
	}

	return board_rev;
};

/*
 * Routine: cpl_eeprom_get_board_rev
 * Description: read system revision from eeprom
 *
 * @buf: buffer to store the product name
 * @eeprom_bus: i2c bus num of the eeprom
 *
 * @return: 0 on success, < 0 on failure
 */
int cpl_eeprom_get_product_name(uchar *buf, uint eeprom_bus)
{
	int err;

	if (buf == NULL)
		return -EINVAL;

	err = cpl_eeprom_setup(eeprom_bus);
	if (err)
		return err;

	err = cpl_eeprom_read(PRODUCT_NAME_OFFSET, buf, PRODUCT_NAME_SIZE);
	if (!err) /* Protect ourselves from invalid data (unterminated str) */
		buf[PRODUCT_NAME_SIZE - 1] = '\0';

	return err;
}
