/*
 * (C) Copyright 2012
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

#include <common.h>
#include <environment.h>
#include <env_multi.h>

#ifndef CONFIG_ENV_MULTI

#define ENV_FUNC(name, type, fname)			\
type fname(void)					\
{							\
	return name##_##fname();			\
}

#define ENV_GET_CHAR_SPEC(name)				\
unsigned char env_get_char_spec(int index)		\
{							\
	return name##_env_get_char_spec(index);		\
}

#if defined(CONFIG_ENV_IS_IN_DATAFLASH)
ENV_FUNC(dataflash, int, env_init);
ENV_FUNC(dataflash, int, saveenv);
ENV_FUNC(dataflash, void, env_relocate_spec);
ENV_GET_CHAR_SPEC(dataflash);
#elif defined(CONFIG_ENV_IS_IN_EEPROM)
ENV_FUNC(eeprom, int, env_init);
ENV_FUNC(eeprom, int, saveenv);
ENV_FUNC(eeprom, void, env_relocate_spec);
ENV_GET_CHAR_SPEC(eeprom);
#elif defined(CONFIG_ENV_IS_IN_FAT)
ENV_FUNC(fat, int, env_init);
ENV_FUNC(fat, int, saveenv);
ENV_FUNC(fat, void, env_relocate_spec);
#elif defined(CONFIG_ENV_IS_IN_FLASH)
ENV_FUNC(flash, int, env_init);
ENV_FUNC(flash, int, saveenv);
ENV_FUNC(flash, void, env_relocate_spec);
#elif defined(CONFIG_ENV_IS_IN_MMC)
ENV_FUNC(mmc, int, env_init);
ENV_FUNC(mmc, int, saveenv);
ENV_FUNC(mmc, void, env_relocate_spec);
#elif defined(CONFIG_ENV_IS_IN_NAND)
ENV_FUNC(nand, int, env_init);
ENV_FUNC(nand, int, saveenv);
ENV_FUNC(nand, void, env_relocate_spec);
#elif defined(CONFIG_ENV_IS_NOWHERE)
ENV_FUNC(nowhere, int, env_init);
ENV_FUNC(nowhere, void, env_relocate_spec);
#elif defined(CONFIG_ENV_IS_IN_NVRAM)
ENV_FUNC(nvram, int, env_init);
ENV_FUNC(nvram, int, saveenv);
ENV_FUNC(nvram, void, env_relocate_spec);
ENV_GET_CHAR_SPEC(nvram);
#elif defined(CONFIG_ENV_IS_IN_ONENAND)
ENV_FUNC(onenand, int, env_init);
ENV_FUNC(onenand, int, saveenv);
ENV_FUNC(onenand, void, env_relocate_spec);
#elif defined(CONFIG_ENV_IS_IN_REMOTE)
ENV_FUNC(remote, int, env_init);
ENV_FUNC(remote, int, saveenv);
ENV_FUNC(remote, void, env_relocate_spec);
#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
ENV_FUNC(sf, int, env_init);
ENV_FUNC(sf, int, saveenv);
ENV_FUNC(sf, void, env_relocate_spec);
#endif

#else /* CONFIG_ENV_MULTI */

#ifdef CONFIG_ENV_MULTI_DEFAULT
static enum env_multi_dev cur_env_dev = CONFIG_ENV_MULTI_DEFAULT;
#else
# error "CONFIG_ENV_MULTI needs CONFIG_ENV_MULTI_DEFAULT!"
# error "it must be set to one of the enabled environment storages"
#endif

typedef enum {false = -1, true = 1} bool;

static bool env_init_is_done = false;
static bool env_relocate_spec_is_done = false;

int env_init(void)
{
	env_init_is_done = true;

	switch (cur_env_dev) {
#if defined(CONFIG_ENV_IS_IN_DATAFLASH)
		case ENV_DATAFLASH:
			return dataflash_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_EEPROM)
		case ENV_EEPROM:
			return eeprom_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_FAT)
		case ENV_FAT:
			return fat_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_FLASH)
		case ENV_FLASH:
			return flash_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_MMC)
		case ENV_MMC:
			return mmc_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_NAND)
		case ENV_NAND:
			return nand_env_init();
#endif
#if defined(CONFIG_ENV_IS_NOWHERE)
		case ENV_NOWHERE:
			return nowhere_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_NVRAM)
		case ENV_NVRAM:
			return nvram_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_ONENAND)
		case ENV_ONENAND:
			return onenand_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_REMOTE)
		case ENV_REMOTE:
			return remote_env_init();
#endif
#if defined(CONFIG_ENV_IS_IN_SF)
		case ENV_SF:
			return sf_env_init();
#endif
		default:;
	}

	return 0;
}

void env_relocate_spec(void)
{
	env_relocate_spec_is_done = true;

	switch (cur_env_dev) {
#if defined(CONFIG_ENV_IS_IN_DATAFLASH)
		case ENV_DATAFLASH:
			return dataflash_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_EEPROM)
		case ENV_EEPROM:
			return eeprom_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_FAT)
		case ENV_FAT:
			return fat_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_FLASH)
		case ENV_FLASH:
			return flash_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_MMC)
		case ENV_MMC:
			return mmc_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_NAND)
		case ENV_NAND:
			return nand_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_NOWHERE)
		case ENV_NOWHERE:
			return nowhere_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_NVRAM)
		case ENV_NVRAM:
			return nvram_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_ONENAND)
		case ENV_ONENAND:
			return onenand_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_REMOTE)
		case ENV_REMOTE:
			return remote_env_relocate_spec();
#endif
#if defined(CONFIG_ENV_IS_IN_SF)
		case ENV_SF:
			return sf_env_relocate_spec();
#endif
		default:;
	}
}

uchar env_get_char_spec(int index)
{
	switch (cur_env_dev) {
#if defined(CONFIG_ENV_IS_IN_DATAFLASH)
		case ENV_DATAFLASH:
			return dataflash_env_get_char_spec(index);
#endif
#if defined(CONFIG_ENV_IS_IN_EEPROM)
		case ENV_EEPROM:
			return eeprom_env_get_char_spec(index);
#endif
#if defined(CONFIG_ENV_IS_IN_NVRAM)
		case ENV_NVRAM:
			return nvram_env_get_char_spec(index);
#endif
		default:;
	}

	return __env_get_char_spec(index);
}

#ifdef CONFIG_CMD_SAVEENV
int saveenv(void)
{
	switch (cur_env_dev) {
#if defined(CONFIG_ENV_IS_IN_DATAFLASH)
		case ENV_DATAFLASH:
			return dataflash_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_EEPROM)
		case ENV_EEPROM:
			return eeprom_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_FAT)
		case ENV_FAT:
			return fat_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_FLASH)
		case ENV_FLASH:
			return flash_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_MMC)
		case ENV_MMC:
			return mmc_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_NAND)
		case ENV_NAND:
			return nand_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_NVRAM)
		case ENV_NVRAM:
			return nvram_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_ONENAND)
		case ENV_ONENAND:
			return onenand_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_REMOTE)
		case ENV_REMOTE:
			return remote_saveenv();
#endif
#if defined(CONFIG_ENV_IS_IN_SF)
		case ENV_SF:
			return sf_saveenv();
#endif
		default:;
	}

	/* we should not get here */
	return 1;
}
#endif

enum env_multi_dev env_multi_get_current(void)
{
	return cur_env_dev;
}

int env_multi_set_current(enum env_multi_dev env_dev, int import)
{
	int err;

	if (cur_env_dev == env_dev && (env_init_is_done == false || !import))
		return 0;

	cur_env_dev = env_dev;

	/* This makes it possible to set env device even before env_init() */
	if (env_init_is_done == true) {
		err = env_init();
		if (err) {
			printf("env multi: env_init() failed: %d\n", err);
			return err;
		}
	}

	if (import && env_relocate_spec_is_done == true)
		env_relocate_spec();

	return 0;
}

#endif /* CONFIG_ENV_MULTI */
