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

