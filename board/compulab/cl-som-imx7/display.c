/*
 * Display configuration for the CompuLab CL-SOM-iMX7 board
 *
 * (C) Copyright 2017 CompuLab, Ltd. http://compulab.com/
 *
 * Author: Uri Mashiach <uri.mashiach@compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <search.h>
#include <env_callback.h>
#include "../common/common.h"
#include <asm/arch/mx7-pins.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <linux/fb.h>
#include <i2c.h>
#include <pca953x.h>
#include <splash.h>
#include "common.h"

/* GPIO extender LCD enable bit */
#define CONFIG_SYS_I2C_PCA953X_LCD_ENA	14
/* The TFP410 address */
#define CONFIG_SYS_TFP410_ADDR 0x39
/* The TFP410 registers */
#define TFP410_REG_DEV_ID_L 0x02
#define TFP410_REG_DEV_ID_H 0x03
#define TFP410_REG_CTL_1_MODE 0x08
#define TFP410_REG_CTL_2_MODE 0x09
#define TFP410_REG_CTL_3_MODE 0x0A
/* LCD backlight GPIO */
#define GPIO_LCD_LIGHT IMX_GPIO_NR(1, 2)

#define FDT_TIMING_DVI "/soc/aips-bus@30400000/lcdif@30730000/display/display-timings/dvi"
#define FDT_NODE_REG_DVI "/regulators/dvi_en_3v3"
#define FDT_NODE_REG_LCD "/regulators/lcd_nstby_3v3"
#define FLIP_32B(val) ((val>>24)&0xff) | ((val<<8)&0xff0000) | ((val>>8)&0xff00) | ((val<<24)&0xff000000)

/* Display type code */
typedef enum {
	DISP_DVI,
	DISP_LCD,
	DISP_NONE,
} display_type;

/* Display parameters preset */
typedef enum {
	PRSET_LCD_KD050C,
	PRSET_DVI_1280x720,
	PRSET_DVI_1024X768,
	PRSET_DVI_1280X1024,
	PRSET_DVI_1920x1080,
	PRSET_NONE,
} display_preset;

/* Doisplay parameters */
struct disp_param {
	char* del_node;
	struct fb_videomode timings;
};

static struct disp_param dispparams[] = {
	{/* PRSET_LCD_KD050C */
		.del_node = FDT_NODE_REG_DVI,
		.timings = {
			.refresh	= 29580000,
			.pixclock	= 29500,
			.xres		= 800,
			.left_margin	= 1,
			.right_margin	= 16,
			.hsync_len	= 80,
			.yres		= 480,
			.upper_margin	= 13,
			.lower_margin	= 16,
			.vsync_len	= 16,
		}
	},
	{/* PRSET_DVI_1280x720 */
		.del_node = FDT_NODE_REG_LCD,
		.timings = {
			.refresh	= 74250000,
			.pixclock	= 13468,
			.xres		= 1280,
			.left_margin	= 220,
			.right_margin	= 110,
			.hsync_len	= 40,
			.yres		= 720,
			.upper_margin	= 20,
			.lower_margin	= 5,
			.vsync_len	= 5,
		}
	},
	{/* PRSET_DVI_1024X768 */
		.del_node = FDT_NODE_REG_LCD,
		.timings = {
			.refresh	= 65000000,
			.pixclock	= 15384,
			.xres		= 1024,
			.left_margin	= 168,
			.right_margin	= 8,
			.hsync_len	= 144,
			.yres		= 768,
			.upper_margin	= 29,
			.lower_margin	= 3,
			.vsync_len	= 4,
		}
	},
	{/* PRSET_DVI_1280X1024 */
		.del_node = FDT_NODE_REG_LCD,
		.timings = {
			.refresh	= 110000000,
			.pixclock	= 9090,
			.xres		= 1280,
			.left_margin	= 200,
			.right_margin	= 48,
			.hsync_len	= 184,
			.yres		= 1024,
			.upper_margin	= 26,
			.lower_margin	= 1,
			.vsync_len	= 3,
		}
	},
	{/* PRSET_DVI_1920x1080 */
		.del_node = FDT_NODE_REG_LCD,
		.timings = {
			.refresh	= 148500000,
			.pixclock	= 6734,
			.xres           = 1920,
			.left_margin    = 80,
			.right_margin   = 48,
			.hsync_len      = 32,
			.yres           = 1080,
			.upper_margin   = 26,
			.lower_margin   = 3,
			.vsync_len      = 6,
		}
	},
};

/* Selected display preset ID */
static display_preset selected_preset = PRSET_NONE;
/* Selected display type */
static display_type disp_type = DISP_NONE;

#define TFP410_ID_H 0x00
#define TFP410_ID_L 0x06

/*
 * tfp410_validate() - TFP410 chip validation.
 * - validate chip device ID.
 *
 * Returns -1 on read failure, 1 on validation failure 0 on success.
 */
static int tfp410_validate(void)
{
	uchar buf;
	int ret;

	/* check version */
	ret = i2c_read(CONFIG_SYS_TFP410_ADDR, TFP410_REG_DEV_ID_L, 1, &buf, 1);
	if (ret != 0) {
		puts ("TFP410 validation read failure.\n");
		return -1;
	}
	if (buf != TFP410_ID_L)
		return 1;
	ret = i2c_read(CONFIG_SYS_TFP410_ADDR, TFP410_REG_DEV_ID_H, 1, &buf, 1);
	if (ret != 0) {
		puts ("TFP410 validation read failure.\n");
		return -1;
	}
	if (buf != TFP410_ID_H)
		return 1;

	return 0;
}

/*
 * dvi_on() - turn on the DVI display.
 * - Enable the TFP410 chip.
 *
 * Returns -1 on failure, 0 on success.
 */
static int dvi_on(void)
{
	uchar buf;
	int ret = -1;

	if (!cl_som_imx7_base_i2c_init)
	  return 0;

	ret = i2c_set_bus_num(CL_SOM_IMX7_I2C_BUS_EXT);
	if (ret != 0) {
		puts ("Failed to select the external I2C bus.\n");
		return -1;
	}
	ret = tfp410_validate();
	if (ret != 0)
		return 0;
	/* Enable the TFP410 chip - RGP to DVI converter */
	buf = 0x35;
	ret = i2c_write(CONFIG_SYS_TFP410_ADDR,
			TFP410_REG_CTL_1_MODE, 1, &buf, 1);
	buf = 0xa6;
	ret |= i2c_write(CONFIG_SYS_TFP410_ADDR,
			 TFP410_REG_CTL_2_MODE, 1, &buf, 1);
	buf = 0x10;
	ret |= i2c_write(CONFIG_SYS_TFP410_ADDR,
			 TFP410_REG_CTL_3_MODE, 1, &buf, 1);
	if (ret != 0) {
		puts ("TFP410 chip enable failure.\n");
		return -1;
	}

	return 0;
}

/*
 * lcd_on() - turn on the LCD display.
 * - Turn on the LCD backlight.
 * - Enable LCD GPIO.
 *
 * Returns -1 on failure, 0 on success.
 */
static int lcd_on(void)
{
	int ret = -1;

	if (cl_som_imx7_base_id != CL_SOM_IMX7_SB_SOM)
		return 0;

	/* Turn on the LCD backlight */
	ret = gpio_direction_output(GPIO_LCD_LIGHT , 1);
	if (ret != 0) {
		puts ("Failed to turn on the LCD backlight.\n");
		return -1;
	}
	ret = i2c_set_bus_num(CL_SOM_IMX7_I2C_BUS_EXT);
	if (ret != 0) {
		puts ("Failed to select the external I2C bus.\n");
		return -1;
	}
	/* Enable LCD GPIO */
	ret = pca953x_set_dir(CONFIG_SYS_I2C_PCA953X_ADDR,
			1 << CONFIG_SYS_I2C_PCA953X_LCD_ENA,
			PCA953X_DIR_OUT << CONFIG_SYS_I2C_PCA953X_LCD_ENA);
	ret |= pca953x_set_val(CONFIG_SYS_I2C_PCA953X_ADDR,
			1 << CONFIG_SYS_I2C_PCA953X_LCD_ENA,
			1 << CONFIG_SYS_I2C_PCA953X_LCD_ENA);
	if (ret != 0) {
		puts ("LCD GPIO enable failure.\n");
		return -1;
	}

	return 0;
}

/*
 * dvi_off() - turn off the DVI display.
 * - Disable the TFP410 chip.
 *
 * Returns -1 on failure, 0 on success.
 */
static int dvi_off(void)
{
	uchar buf;
	int ret = -1;

	if (!cl_som_imx7_base_i2c_init)
	  return 0;

	ret = i2c_set_bus_num(CL_SOM_IMX7_I2C_BUS_EXT);
	if (ret != 0) {
		puts ("Failed to select the external I2C bus.\n");
		return -1;
	}
	ret = tfp410_validate();
	if (ret != 0)
		return 0;
	/* Disable the TFP410 chip - RGP to DVI converter */
	buf = 0xfe;
	ret = i2c_write(CONFIG_SYS_TFP410_ADDR,
			TFP410_REG_CTL_1_MODE, 1, &buf, 1);
	if (ret != 0) {
		puts ("TFP410 chip disable failure.\n");
		return -1;
	}

	return 0;
}

/*
 * lcd_off() - turn off the LCD display.
 * - Turn off the LCD backlight.
 * - Disable LCD GPIO.
 *
 * Returns -1 on failure, 0 on success.
 */
static int lcd_off(void)
{
	int ret = -1;

	if (cl_som_imx7_base_id != CL_SOM_IMX7_SB_SOM)
		return 0;

	/* Turn off the LCD backlight */
	ret = gpio_direction_output(GPIO_LCD_LIGHT , 0);
	if (ret != 0) {
		puts ("Failed to turn off the LCD backlight.\n");
		return -1;
	}
	/* Disable LCD GPIO */
	ret |= pca953x_set_dir(CONFIG_SYS_I2C_PCA953X_ADDR,
			1 << CONFIG_SYS_I2C_PCA953X_LCD_ENA,
			PCA953X_DIR_OUT << CONFIG_SYS_I2C_PCA953X_LCD_ENA);
	ret |= pca953x_set_val(CONFIG_SYS_I2C_PCA953X_ADDR,
			1 << CONFIG_SYS_I2C_PCA953X_LCD_ENA, 0);
	if (ret != 0) {
		puts ("LCD GPIO disable failure.\n");
		return -1;
	}

	return 0;
}

/*
 * set_preset() - apply specific display preset.
 * The parameters are used by Freescale videomode module.
 *
 * Returns -1 on failure, display type code on success.
 */
static display_type set_preset(void)
{
	int ret = -1;
	display_type disp_type = DISP_DVI;
	char videomod_buf[100];

	if ((selected_preset < 0) || (selected_preset >= PRSET_NONE))
		return DISP_NONE;

	sprintf(videomod_buf,
		"video=ctfb:x:%u,y:%u,depth:24,pclk:%u,le:%u,ri:%u,up:%u," \
		"lo:%u,hs:%u,vs:%u,sync:0,vmode:0",
		dispparams[selected_preset].timings.xres,
		dispparams[selected_preset].timings.yres,
		dispparams[selected_preset].timings.pixclock,
		dispparams[selected_preset].timings.left_margin,
		dispparams[selected_preset].timings.right_margin,
		dispparams[selected_preset].timings.upper_margin,
		dispparams[selected_preset].timings.lower_margin,
		dispparams[selected_preset].timings.hsync_len,
		dispparams[selected_preset].timings.vsync_len);
	ret = setenv("videomode", videomod_buf);
	if (ret)
		return -1;

	if (selected_preset == PRSET_LCD_KD050C)
		disp_type = DISP_LCD;

	return disp_type;
}

/*
 * env_parse_displaytype() - parse environment parameter.
 * - Determine display type.
 * - Set display timing in environment parameter "videomode".
 *
 * @displaytype: The environment variable containing the display parameters.
 * Returns negative value on failure, display type code on success.
 */
static display_type env_parse_displaytype(char *disp_type)
{
	if ((!strcmp(disp_type, "dvi1024x768")) ||
	    (!strcmp(disp_type, "dvi"))) {
		selected_preset = PRSET_DVI_1024X768;
	}
	else if (!strcmp(disp_type, "dvi1920x1080"))
		selected_preset = PRSET_DVI_1920x1080;
	else if (!strcmp(disp_type, "dvi1280x1024"))
		selected_preset = PRSET_DVI_1280X1024;
	else if (!strcmp(disp_type, "dvi1280x720"))
		selected_preset = PRSET_DVI_1280x720;
	else if (!strcmp(disp_type, "lcd_KD050C"))
		selected_preset = PRSET_LCD_KD050C;
	return set_preset();
}

/* Enable the appropriate display */
/*
 * enable_display() - enable specific display type.
 *
 * Returns negative value on failure, 0 on success.
 */
int enable_display(void)
{
	int ret = 0;
	char *stdout_env = getenv("stdout");

	switch (disp_type) {
	case DISP_DVI:
		ret = lcd_off();
		ret |= dvi_on();
		break;
	case DISP_LCD:
		ret = dvi_off();
		ret |= lcd_on();
		break;
	default:
		break;
	}

	/*
	 * Update stdout and enderr environment parameters according to the
	 * baseboard type
	 */
	if (stdout_env) /* stdout was set */
		return 0;
	if ((cl_som_imx7_base_id ==  CL_SOM_IMX7_SB_IOT) ||
	    (cl_som_imx7_base_id ==  CL_SOM_IMX7_SBC_IOT) ||
	    (cl_som_imx7_base_id ==  CL_SOM_IMX7_IOTG)) {
		setenv("stdout", "serial,vga");
		setenv("stderr", "serial,vga");
	}
	else {
		setenv("stdout", "serial");
		setenv("stderr", "serial");
	}

	return ret;
}

/*
 * setup_display() - set display signal MUX, enable display, set timing.
 * The function is called from drv_video_init @ cfb_console.c
 * Returns negative value on failure, 0 on success.
 */
int board_video_skip(void)
{
	int ret;
	char *displaytype = getenv("displaytype");

	cl_som_imx7_lcd_pads_set();
	if (cl_som_imx7_base_id == CL_SOM_IMX7_SB_SOM)
		cl_som_imx7_lcd_control_pads_set();

	disp_type = env_parse_displaytype(displaytype);
	if (disp_type < 0) {
		printf("displaytype parsing failure\n");
		return ret;
	}

	return 0;
}

/*
 * on_displaytype() - callback function for the environment "displaytype"
 *
 * @name: environment name
 * @value: environment value
 * @op: operation - create, update, delete.
 * Returns negative value on failure, 0 on success.
 */
static int on_displaytype(const char *name, const char *value, enum env_op op,
			  int flags)
{
	int ret;

	if (op != env_op_overwrite)
		return 0;

	ret = env_parse_displaytype((char*) value);
	if (ret < 0) {
		printf("displaytype parsing failure\n");
		return ret;
	}

	return 0;
}

U_BOOT_ENV_CALLBACK(displaytype, on_displaytype);

#ifdef CONFIG_SPLASH_SCREEN
/* Splash screen sources options */
static struct splash_location cl_som_imx7_splash_locations[] = {
	{
		.name = "sf",
		.storage = SPLASH_STORAGE_SF,
		.flags = SPLASH_STORAGE_RAW,
		.offset = 0x100000,
	},
	{
		.name = "mmc_fs",
		.storage = SPLASH_STORAGE_MMC,
		.flags = SPLASH_STORAGE_FS,
		.devpart = "0:1",
	},
	{
		.name = "usb_fs",
		.storage = SPLASH_STORAGE_USB,
		.flags = SPLASH_STORAGE_FS,
		.devpart = "0:1",
	},
};

/*
 * splash_screen_prepare() - load splash screen image to RAM.
 *
 * Returns -1 on failure, 0 on success.
 */
int splash_screen_prepare(void)
{
	int ret = splash_source_load(cl_som_imx7_splash_locations,
				     ARRAY_SIZE(cl_som_imx7_splash_locations));

	if (!ret) {/* Splash screen loaded - prevent VGA console */
		setenv("stdout", "serial");
	}

	return ret;
}
#endif /* CONFIG_SPLASH_SCREEN */

/*
 * fdt_board_adjust_display() - update device tree.
 * * Updating device tree with display timing parameters.
 * * Disable the regulator of the not active display.
 * Returns -1 on failure, 0 on success.
 */
int fdt_board_adjust_display(void)
{
	u32 flip_val;
	int ret;

	if ((selected_preset < 0) || (selected_preset >= PRSET_NONE))
		return 0;

	flip_val = FLIP_32B(dispparams[selected_preset].timings.refresh);
	ret = fdt_prop_set(FDT_TIMING_DVI, "clock-frequency", &flip_val, 4, 0);
	flip_val = FLIP_32B(dispparams[selected_preset].timings.xres);
	ret |= fdt_prop_set(FDT_TIMING_DVI, "hactive", &flip_val, 4, 0);
	flip_val = FLIP_32B(dispparams[selected_preset].timings.yres);
	ret |= fdt_prop_set(FDT_TIMING_DVI, "vactive", &flip_val, 4, 0);
	flip_val = FLIP_32B(dispparams[selected_preset].timings.left_margin);
	ret |= fdt_prop_set(FDT_TIMING_DVI, "hback-porch", &flip_val, 4, 0);
	flip_val = FLIP_32B(dispparams[selected_preset].timings.right_margin);
	ret |= fdt_prop_set(FDT_TIMING_DVI, "hfront-porch", &flip_val, 4, 0);
	flip_val = FLIP_32B(dispparams[selected_preset].timings.upper_margin);
	ret |= fdt_prop_set(FDT_TIMING_DVI, "vback-porch", &flip_val, 4, 0);
	flip_val = FLIP_32B(dispparams[selected_preset].timings.lower_margin);
	ret |= fdt_prop_set(FDT_TIMING_DVI, "vfront-porch", &flip_val, 4, 0);
	flip_val = FLIP_32B(dispparams[selected_preset].timings.hsync_len);
	ret |= fdt_prop_set(FDT_TIMING_DVI, "hsync-len", &flip_val, 4, 0);
	flip_val = FLIP_32B(dispparams[selected_preset].timings.vsync_len);
	ret |= fdt_prop_set(FDT_TIMING_DVI, "vsync-len", &flip_val, 4, 0);
	if (ret) {
		printf("Failed updating device tree with display \
			timing parameters\n");
		return -1;
	}
	fdt_prop_del(dispparams[selected_preset].del_node,
		     "regulator-always-on");

	return 0;
}
