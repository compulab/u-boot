/*
 * Copyright 2020 CompuLab
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __UCM_IMX8M_MINI_H
#define __UCM_IMX8M_MINI_H

#include "cpl-imx8m-mini.h"

#if defined(CONFIG_ANDROID_SUPPORT)
#include "ucm-imx8m-mini_android.h"
#endif
#endif

#undef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_BOOTCOMMAND

#define CONFIG_EXTRA_ENV_SETTINGS		\
	CONFIG_MFG_ENV_SETTINGS \
	"autoload=off\0" \
	"script=boot.scr\0" \
	"image=Image\0" \
	"console=ttymxc2,115200	earlycon=ec_imx6q,0x30880000,115200\0" \
	"fdt_addr=0x43000000\0"	\
	"fdt_high=0xffffffffffffffff\0"	\
	"initrd_high=0xffffffffffffffff\0" \
	"root_opt=rootwait rw\0" \
	"emmc_ul=setenv boot_dev_str eMMC; run boot_try_msg; " \
	"setenv iface mmc; setenv dev 2; setenv part 1;" \
	"setenv bootargs console=${console} root=/dev/mmcblk2p2 " \
	"${root_opt};\0" \
	"usb_ul=setenv boot_dev_str USB; run boot_try_msg; usb reset; " \
	"setenv iface usb; setenv dev 0; setenv part 1; " \
	"setenv bootargs console=${console} root=/dev/sda2 ${root_opt};\0" \
	"ulbootscript=load ${iface} ${dev}:${part} ${loadaddr} ${script};\0" \
	"ulimage=load ${iface} ${dev}:${part} ${loadaddr} ${image}\0" \
	"ulfdt=load ${iface} ${dev}:${part} ${fdt_addr} ${fdt_file};\0" \
	"bootscript=echo Running bootscript from ${boot_dev_str} ...; " \
	"source;\0" \
	"boot_try_msg=echo Trying to boot from ${boot_dev_str} ...;\0"
#define CONFIG_BOOTCOMMAND \
	"for src in usb_ul emmc_ul; do " \
		"run ${src}; " \
		"if run ulbootscript; then " \
			"run bootscript; " \
		"else " \
			"if run ulimage; then " \
				"if run ulfdt; then " \
					"echo Booting from ${boot_dev_str} script ...; " \
					"booti ${loadaddr} - ${fdt_addr}; " \
				"fi; " \
			"fi; " \
		"fi; " \
	"done; "
