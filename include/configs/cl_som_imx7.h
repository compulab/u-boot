/*
 * Copyright (C) 2015 CompuLab, Ltd.
 *
 * Configuration settings for the CompuLab CL-SOM-iMX7 System-on-Module.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CL_SOM_IMX7_CONFIG_H
#define __CL_SOM_IMX7_CONFIG_H

#include "mx7_common.h"

#define CONFIG_DBG_MONITOR
#define PHYS_SDRAM_SIZE			CONFIG_BOARD_DRAM_SIZE

#define CONFIG_MXC_UART_BASE            UART1_IPS_BASE_ADDR

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(32 * SZ_1M)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT

#define CONFIG_DISPLAY_BOARDINFO

/* Uncomment to enable secure boot support */
/* #define CONFIG_SECURE_BOOT */
#define CONFIG_CSF_SIZE			0x4000

/* Network */
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define CONFIG_FEC_XCV_TYPE             RGMII
#define CONFIG_ETHPRIME                 "FEC"
#define CONFIG_FEC_MXC_PHYADDR          0

#define CONFIG_PHYLIB
#define CONFIG_PHY_ATHEROS
/* ENET1 */
#define IMX_FEC_BASE			ENET_IPS_BASE_ADDR

/* PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_PFUZE3000
#define CONFIG_POWER_PFUZE3000_I2C_ADDR	0x08

#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_PLAN9
#undef CONFIG_BOOTM_RTEMS

/* I2C configs */
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */
#define CONFIG_SYS_I2C_MXC_I2C4		/* enable I2C bus 4 */
#define CONFIG_SYS_I2C_SPEED		100000

#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1
#define CONFIG_SYS_I2C_EEPROM_BUS	0

#define CONFIG_PCA953X
#define CONFIG_CMD_PCA953X
#define CONFIG_CMD_PCA953X_INFO
#define CONFIG_SYS_I2C_PCA953X_ADDR	0x20
#define CONFIG_SYS_I2C_PCA953X_WIDTH	{ {0x20, 16} }

#define CONFIG_SYS_MMC_IMG_LOAD_PART	1

#undef CONFIG_SYS_AUTOLOAD
#undef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTDELAY

#define CONFIG_BOOTDELAY		3
#define CONFIG_SYS_AUTOLOAD		"no"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"autoload=off\0" \
	"script=boot.scr\0" \
	"image=zImage\0" \
	"console=ttymxc0\0" \
	"fdt_high=0xffffffff\0" \
	"fdt_file=imx7d-sbc-imx7.dtb\0" \
	"fdt_addr=0x83000000\0" \
	"mmcblk=0\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_DEV)"\0" \
	"mmcpart=" __stringify(CONFIG_SYS_MMC_IMG_LOAD_PART) "\0" \
	"loadbootscript=" \
		"load mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"source\0" \
	"loadimage=load mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=load mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}\0" \
	"mmcargs=setenv mmcroot /dev/mmcblk${mmcblk}p2 rootwait rw; " \
		"setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot}\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if run loadfdt; then " \
			"run set_display; " \
			"bootz ${loadaddr} - ${fdt_addr}; " \
		"fi;\0" \
	"set_display=fdt addr ${fdt_addr}; fdt rm lcdif/display/display-timings/lcd\0" \
	"defaultboot=setenv mmcdev 1; setenv mmcblk 2; mmc dev ${mmcdev}; "\
		"if run loadimage; then " \
			"run mmcboot; " \
		"fi;\0" \

#define CONFIG_BOOTCOMMAND \
	   "mmc dev ${mmcdev};" \
	   "if mmc rescan; then " \
		   "if run loadbootscript; then " \
			   "run bootscript; " \
		   "else " \
			   "if run loadimage; then " \
				   "run mmcboot; " \
			   "fi; " \
		   "fi; " \
	   "fi; " \
	   "run defaultboot"

#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x20000000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000

#define CONFIG_STACKSIZE		SZ_128K

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* SPI Flash support */
#define CONFIG_SPI
#define CONFIG_MXC_SPI
#define CONFIG_SF_DEFAULT_BUS		0
#define CONFIG_SF_DEFAULT_CS		0
#define CONFIG_SF_DEFAULT_SPEED		20000000
#define CONFIG_SF_DEFAULT_MODE		(SPI_MODE_0)

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_SIZE			SZ_8K
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET		(768 * 1024)
#define CONFIG_ENV_SECT_SIZE		(64 * 1024)
#define CONFIG_ENV_SPI_BUS		CONFIG_SF_DEFAULT_BUS
#define CONFIG_ENV_SPI_CS		CONFIG_SF_DEFAULT_CS
#define CONFIG_ENV_SPI_MODE		CONFIG_SF_DEFAULT_MODE
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED

#define CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x40000000
#define CONFIG_SYS_NAND_MAX_CHIPS	1
#define CONFIG_NAND_MXS
#define CONFIG_SYS_NAND_ONFI_DETECTION
/* APBH DMA is required for NAND support */
#define CONFIG_APBH_DMA
#define CONFIG_APBH_DMA_BURST
#define CONFIG_APBH_DMA_BURST8

/* MMC Config*/
#define CONFIG_FSL_USDHC
#ifdef CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR       USDHC1_BASE_ADDR

#define CONFIG_SYS_FSL_USDHC_NUM	2
#define CONFIG_SYS_MMC_DEV		0 /* USDHC1 */
#define CONFIG_MMCROOT			"/dev/mmcblk0p2" /* USDHC1 */
#define CONFIG_SUPPORT_EMMC_BOOT /* eMMC specific */
#endif

/* USB Configs */
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_MXC_USB_PORTSC  (PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS   0
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2

#define CONFIG_IMX_THERMAL

/* Status LED */
#define CONFIG_STATUS_LED
#define CONFIG_GPIO_LED
#define CONFIG_BOARD_SPECIFIC_LED
#define STATUS_LED_BIT			IMX_GPIO_NR(6, 14)
#define STATUS_LED_STATE		STATUS_LED_OFF
#define STATUS_LED_PERIOD		(CONFIG_SYS_HZ / 2)
#define STATUS_LED_BOOT			0
#define CONFIG_GPIO_LED_INVERTED_TABLE	{STATUS_LED_BOOT,}

#endif	/* __CONFIG_H */
