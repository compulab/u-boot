/*
 * (C) Copyright 2011 CompuLab, Ltd. <www.compulab.co.il>
 *
 * Authors: Nikita Kiryanov <nikita@compulab.co.il>
 *	    Igor Grinberg <grinberg@compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _EEPROM_
#define _EEPROM_
#include <errno.h>

#ifdef CONFIG_SYS_I2C
#ifdef CONFIG_CMD_EEPROM_LAYOUT
int cl_eeprom_layout_setup(struct eeprom_layout *layout, uchar *eeprom_buf,
			   int layout_version, uint eeprom_bus,
			   uint8_t eeprom_addr);
#endif /* CONFIG_CMD_EEPROM_LAYOUT */
int cl_eeprom_read_mac_addr(uchar *buf, uint eeprom_bus);
u32 cl_eeprom_get_board_rev(uint eeprom_bus);
int cl_eeprom_get_product_name(uchar *buf, uint eeprom_bus);
#else
static inline int cl_eeprom_read_mac_addr(uchar *buf, uint eeprom_bus)
{
	return 1;
}
static inline u32 cl_eeprom_get_board_rev(uint eeprom_bus)
{
	return 0;
}
static inline int cl_eeprom_get_product_name(uchar *buf, uint eeprom_bus)
{
	return -ENOSYS;
}
#endif

#endif
