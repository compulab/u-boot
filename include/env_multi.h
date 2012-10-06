/*
 * (C) Copyright 2011
 * CompuLab, Ltd. <www.compulab.co.il>
 *
 * Author: Igor Grinberg <grinberg@compulab.co.il>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __ENV_MULTI_H__
#define __ENV_MULTI_H__

#if defined(CONFIG_ENV_IS_IN_DATAFLASH)
extern int dataflash_env_init(void);
extern int dataflash_saveenv(void);
extern void dataflash_env_relocate_spec(void);
unsigned char dataflash_env_get_char_spec(int index);
#endif
#if defined(CONFIG_ENV_IS_IN_EEPROM)
extern int eeprom_env_init(void);
extern int eeprom_saveenv(void);
extern void eeprom_env_relocate_spec(void);
unsigned char eeprom_env_get_char_spec(int index);
#endif
#if defined(CONFIG_ENV_IS_IN_FAT)
extern int fat_env_init(void);
extern int fat_saveenv(void);
extern void fat_env_relocate_spec(void);
#endif
#if defined(CONFIG_ENV_IS_IN_FLASH)
extern int flash_env_init(void);
extern int flash_saveenv(void);
extern void flash_env_relocate_spec(void);
#endif
#if defined(CONFIG_ENV_IS_IN_MMC)
extern int mmc_env_init(void);
extern int mmc_saveenv(void);
extern void mmc_env_relocate_spec(void);
#endif
#if defined(CONFIG_ENV_IS_IN_NAND)
extern int nand_env_init(void);
extern int nand_saveenv(void);
extern void nand_env_relocate_spec(void);
#endif
#if defined(CONFIG_ENV_IS_NOWHERE)
extern int nowhere_env_init(void);
extern void nowhere_env_relocate_spec(void);
#endif
#if defined(CONFIG_ENV_IS_IN_NVRAM)
extern int nvram_env_init(void);
extern int nvram_saveenv(void);
extern void nvram_env_relocate_spec(void);
unsigned char nvram_env_get_char_spec(int index);
#endif
#if defined(CONFIG_ENV_IS_IN_ONENAND)
extern int onenand_env_init(void);
extern int onenand_saveenv(void);
extern void onenand_env_relocate_spec(void);
#endif
#if defined(CONFIG_ENV_IS_IN_REMOTE)
extern int remote_env_init(void);
extern int remote_saveenv(void);
extern void remote_env_relocate_spec(void);
#endif
#if defined(CONFIG_ENV_IS_IN_SPI_FLASH)
extern int sf_env_init(void);
extern int sf_saveenv(void);
extern void sf_env_relocate_spec(void);
#endif

#ifdef CONFIG_ENV_MULTI

enum env_multi_dev {
#if defined(CONFIG_ENV_IS_IN_DATAFLASH)
	ENV_DATAFLASH,
#endif
#if defined(CONFIG_ENV_IS_IN_EEPROM)
	ENV_EEPROM,
#endif
#if defined(CONFIG_ENV_IS_IN_FAT)
	ENV_FAT,
#endif
#if defined(CONFIG_ENV_IS_IN_FLASH)
	ENV_FLASH,
#endif
#if defined(CONFIG_ENV_IS_IN_MMC)
	ENV_MMC,
#endif
#if defined(CONFIG_ENV_IS_IN_NAND)
	ENV_NAND,
#endif
#if defined(CONFIG_ENV_IS_NOWHERE)
	ENV_NOWHERE,
#endif
#if defined(CONFIG_ENV_IS_IN_NVRAM)
	ENV_NVRAM,
#endif
#if defined(CONFIG_ENV_IS_IN_ONENAND)
	ENV_ONENAND,
#endif
#if defined(CONFIG_ENV_IS_IN_REMOTE)
	ENV_REMOTE,
#endif
#if defined(CONFIG_ENV_IS_IN_SF)
	ENV_SF,
#endif
};

extern unsigned char __env_get_char_spec(int index);

enum env_multi_dev env_multi_get_current(void);
int env_multi_set_current(enum env_multi_dev env_dev, int import);

#endif /* CONFIG_ENV_MULTI */

#endif /* __ENV_MULTI_H__ */
