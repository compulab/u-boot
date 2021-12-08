/*
 * (C) Copyright 2011 CompuLab, Ltd. <www.compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _EEPROM_
#define _EEPROM_
#include <errno.h>

#ifdef CONFIG_DM_I2C
int cpl_eeprom_read_mac_addr(uchar *buf, uint eeprom_bus);
u32 cpl_eeprom_get_board_rev(uint eeprom_bus);
int cpl_eeprom_get_product_name(uchar *buf, uint eeprom_bus);
#else
static inline int cpl_eeprom_read_mac_addr(uchar *buf, uint eeprom_bus)
{
	return 1;
}
static inline u32 cpl_eeprom_get_board_rev(uint eeprom_bus)
{
	return 0;
}
static inline int cpl_eeprom_get_product_name(uchar *buf, uint eeprom_bus)
{
	return -ENOSYS;
}
#endif

#endif
