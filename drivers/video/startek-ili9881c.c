// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 NXP
 * Copyright 2021 CompuLab LTD.
 *
 */

#include <common.h>
#include <dm.h>
#include <mipi_dsi.h>
#include <panel.h>
#include <asm/gpio.h>
#include <linux/err.h>

#define   LCD_XSIZE_TFT   720
#define   LCD_YSIZE_TFT   1280
#define   PCLOCK          62000
#define   LCD_VBPD        20
#define   LCD_VFPD        10
#define   LCD_VSPW        10
#define   LCD_HBPD        30
#define   LCD_HFPD        10
#define   LCD_HSPW        20

enum ili9881c_op {
    ILI9881C_SWITCH_PAGE,
    ILI9881C_COMMAND,
};

struct ili9881c_instr {
    enum ili9881c_op	op;

    union arg {
        struct cmd {
            u8	cmd;
            u8	data;
        } cmd;
        u8	page;
    } arg;
};

#define ILI9881C_SWITCH_PAGE_INSTR(_page)	\
    {					\
        .op = ILI9881C_SWITCH_PAGE,	\
        .arg = {			\
            .page = (_page),	\
        },				\
    }

#define ILI9881C_COMMAND_INSTR(_cmd, _data)		\
    {						\
        .op = ILI9881C_COMMAND,		\
        .arg = {				\
            .cmd = {			\
                .cmd = (_cmd),		\
                .data = (_data),	\
            },				\
        },					\
    }

static const struct ili9881c_instr ili9881c_init[] = {
    ILI9881C_SWITCH_PAGE_INSTR(3),
    ILI9881C_COMMAND_INSTR(0x01, 0x00),
    ILI9881C_COMMAND_INSTR(0x02, 0x00),
    ILI9881C_COMMAND_INSTR(0x03, 0x72),
    ILI9881C_COMMAND_INSTR(0x04, 0x00),
    ILI9881C_COMMAND_INSTR(0x05, 0x00),
    ILI9881C_COMMAND_INSTR(0x06, 0x09),
    ILI9881C_COMMAND_INSTR(0x07, 0x00),
    ILI9881C_COMMAND_INSTR(0x08, 0x00),
    ILI9881C_COMMAND_INSTR(0x09, 0x01),
    ILI9881C_COMMAND_INSTR(0x0a, 0x00),
    ILI9881C_COMMAND_INSTR(0x0b, 0x00),
    ILI9881C_COMMAND_INSTR(0x0c, 0x01),
    ILI9881C_COMMAND_INSTR(0x0d, 0x00),
    ILI9881C_COMMAND_INSTR(0x0e, 0x00),
    ILI9881C_COMMAND_INSTR(0x0f, 0x00),
    ILI9881C_COMMAND_INSTR(0x10, 0x00),
    ILI9881C_COMMAND_INSTR(0x11, 0x00),
    ILI9881C_COMMAND_INSTR(0x12, 0x00),
    ILI9881C_COMMAND_INSTR(0x13, 0x00),
    ILI9881C_COMMAND_INSTR(0x14, 0x00),
    ILI9881C_COMMAND_INSTR(0x15, 0x00),
    ILI9881C_COMMAND_INSTR(0x16, 0x00),
    ILI9881C_COMMAND_INSTR(0x17, 0x00),
    ILI9881C_COMMAND_INSTR(0x18, 0x00),
    ILI9881C_COMMAND_INSTR(0x19, 0x00),
    ILI9881C_COMMAND_INSTR(0x1a, 0x00),
    ILI9881C_COMMAND_INSTR(0x1b, 0x00),
    ILI9881C_COMMAND_INSTR(0x1c, 0x00),
    ILI9881C_COMMAND_INSTR(0x1d, 0x00),
    ILI9881C_COMMAND_INSTR(0x1e, 0x40),
    ILI9881C_COMMAND_INSTR(0x1f, 0x80),
    ILI9881C_COMMAND_INSTR(0x20, 0x05),
    ILI9881C_COMMAND_INSTR(0x20, 0x05),
    ILI9881C_COMMAND_INSTR(0x21, 0x02),
    ILI9881C_COMMAND_INSTR(0x22, 0x00),
    ILI9881C_COMMAND_INSTR(0x23, 0x00),
    ILI9881C_COMMAND_INSTR(0x24, 0x00),
    ILI9881C_COMMAND_INSTR(0x25, 0x00),
    ILI9881C_COMMAND_INSTR(0x26, 0x00),
    ILI9881C_COMMAND_INSTR(0x27, 0x00),
    ILI9881C_COMMAND_INSTR(0x28, 0x33),
    ILI9881C_COMMAND_INSTR(0x29, 0x02),
    ILI9881C_COMMAND_INSTR(0x2a, 0x00),
    ILI9881C_COMMAND_INSTR(0x2b, 0x00),
    ILI9881C_COMMAND_INSTR(0x2c, 0x00),
    ILI9881C_COMMAND_INSTR(0x2d, 0x00),
    ILI9881C_COMMAND_INSTR(0x2e, 0x00),
    ILI9881C_COMMAND_INSTR(0x2f, 0x00),
    ILI9881C_COMMAND_INSTR(0x30, 0x00),
    ILI9881C_COMMAND_INSTR(0x31, 0x00),
    ILI9881C_COMMAND_INSTR(0x32, 0x00),
    ILI9881C_COMMAND_INSTR(0x32, 0x00),
    ILI9881C_COMMAND_INSTR(0x33, 0x00),
    ILI9881C_COMMAND_INSTR(0x34, 0x04),
    ILI9881C_COMMAND_INSTR(0x35, 0x00),
    ILI9881C_COMMAND_INSTR(0x36, 0x00),
    ILI9881C_COMMAND_INSTR(0x37, 0x00),
    ILI9881C_COMMAND_INSTR(0x38, 0x3C),
    ILI9881C_COMMAND_INSTR(0x39, 0x00),
    ILI9881C_COMMAND_INSTR(0x3a, 0x40),
    ILI9881C_COMMAND_INSTR(0x3b, 0x40),
    ILI9881C_COMMAND_INSTR(0x3c, 0x00),
    ILI9881C_COMMAND_INSTR(0x3d, 0x00),
    ILI9881C_COMMAND_INSTR(0x3e, 0x00),
    ILI9881C_COMMAND_INSTR(0x3f, 0x00),
    ILI9881C_COMMAND_INSTR(0x40, 0x00),
    ILI9881C_COMMAND_INSTR(0x41, 0x00),
    ILI9881C_COMMAND_INSTR(0x42, 0x00),
    ILI9881C_COMMAND_INSTR(0x43, 0x00),
    ILI9881C_COMMAND_INSTR(0x44, 0x00),
    ILI9881C_COMMAND_INSTR(0x50, 0x01),
    ILI9881C_COMMAND_INSTR(0x51, 0x23),
    ILI9881C_COMMAND_INSTR(0x52, 0x45),
    ILI9881C_COMMAND_INSTR(0x53, 0x67),
    ILI9881C_COMMAND_INSTR(0x54, 0x89),
    ILI9881C_COMMAND_INSTR(0x55, 0xab),
    ILI9881C_COMMAND_INSTR(0x56, 0x01),
    ILI9881C_COMMAND_INSTR(0x57, 0x23),
    ILI9881C_COMMAND_INSTR(0x58, 0x45),
    ILI9881C_COMMAND_INSTR(0x59, 0x67),
    ILI9881C_COMMAND_INSTR(0x5a, 0x89),
    ILI9881C_COMMAND_INSTR(0x5b, 0xab),
    ILI9881C_COMMAND_INSTR(0x5c, 0xcd),
    ILI9881C_COMMAND_INSTR(0x5d, 0xef),
    ILI9881C_COMMAND_INSTR(0x5e, 0x11),
    ILI9881C_COMMAND_INSTR(0x5f, 0x01),
    ILI9881C_COMMAND_INSTR(0x60, 0x00),
    ILI9881C_COMMAND_INSTR(0x61, 0x15),
    ILI9881C_COMMAND_INSTR(0x62, 0x14),
    ILI9881C_COMMAND_INSTR(0x63, 0x0E),
    ILI9881C_COMMAND_INSTR(0x64, 0x0F),
    ILI9881C_COMMAND_INSTR(0x65, 0x0C),
    ILI9881C_COMMAND_INSTR(0x66, 0x0D),
    ILI9881C_COMMAND_INSTR(0x67, 0x06),
    ILI9881C_COMMAND_INSTR(0x68, 0x02),
    ILI9881C_COMMAND_INSTR(0x69, 0x07),
    ILI9881C_COMMAND_INSTR(0x6a, 0x02),
    ILI9881C_COMMAND_INSTR(0x6b, 0x02),
    ILI9881C_COMMAND_INSTR(0x6c, 0x02),
    ILI9881C_COMMAND_INSTR(0x6d, 0x02),
    ILI9881C_COMMAND_INSTR(0x6e, 0x02),
    ILI9881C_COMMAND_INSTR(0x6f, 0x02),
    ILI9881C_COMMAND_INSTR(0x70, 0x02),
    ILI9881C_COMMAND_INSTR(0x71, 0x02),
    ILI9881C_COMMAND_INSTR(0x72, 0x02),
    ILI9881C_COMMAND_INSTR(0x73, 0x02),
    ILI9881C_COMMAND_INSTR(0x74, 0x02),
    ILI9881C_COMMAND_INSTR(0x75, 0x01),
    ILI9881C_COMMAND_INSTR(0x76, 0x00),
    ILI9881C_COMMAND_INSTR(0x77, 0x14),
    ILI9881C_COMMAND_INSTR(0x78, 0x15),
    ILI9881C_COMMAND_INSTR(0x79, 0x0E),
    ILI9881C_COMMAND_INSTR(0x7a, 0x0F),
    ILI9881C_COMMAND_INSTR(0x7b, 0x0C),
    ILI9881C_COMMAND_INSTR(0x7c, 0x0D),
    ILI9881C_COMMAND_INSTR(0x7d, 0x06),
    ILI9881C_COMMAND_INSTR(0x7e, 0x02),
    ILI9881C_COMMAND_INSTR(0x7f, 0x07),
    ILI9881C_COMMAND_INSTR(0x80, 0x02),
    ILI9881C_COMMAND_INSTR(0x81, 0x02),
    ILI9881C_COMMAND_INSTR(0x83, 0x02),
    ILI9881C_COMMAND_INSTR(0x84, 0x02),
    ILI9881C_COMMAND_INSTR(0x85, 0x02),
    ILI9881C_COMMAND_INSTR(0x86, 0x02),
    ILI9881C_COMMAND_INSTR(0x87, 0x02),
    ILI9881C_COMMAND_INSTR(0x88, 0x02),
    ILI9881C_COMMAND_INSTR(0x89, 0x02),
    ILI9881C_COMMAND_INSTR(0x8A, 0x02),
    ILI9881C_SWITCH_PAGE_INSTR(0x4),
    ILI9881C_COMMAND_INSTR(0x6C, 0x15),
    ILI9881C_COMMAND_INSTR(0x6E, 0x2A),
    ILI9881C_COMMAND_INSTR(0x6F, 0x33),
    ILI9881C_COMMAND_INSTR(0x3A, 0x94),
    ILI9881C_COMMAND_INSTR(0x8D, 0x15),
    ILI9881C_COMMAND_INSTR(0x87, 0xBA),
    ILI9881C_COMMAND_INSTR(0x26, 0x76),
    ILI9881C_COMMAND_INSTR(0xB2, 0xD1),
    ILI9881C_COMMAND_INSTR(0xB5, 0x06),
    ILI9881C_SWITCH_PAGE_INSTR(0x1),
    ILI9881C_COMMAND_INSTR(0x22, 0x0A),
    ILI9881C_COMMAND_INSTR(0x31, 0x00),
    ILI9881C_COMMAND_INSTR(0x53, 0x90),
    ILI9881C_COMMAND_INSTR(0x55, 0xA2),
    ILI9881C_COMMAND_INSTR(0x50, 0xB7),
    ILI9881C_COMMAND_INSTR(0x51, 0xB7),
    ILI9881C_COMMAND_INSTR(0x60, 0x22),
    ILI9881C_COMMAND_INSTR(0x61, 0x00),
    ILI9881C_COMMAND_INSTR(0x62, 0x19),
    ILI9881C_COMMAND_INSTR(0x63, 0x10),
    ILI9881C_COMMAND_INSTR(0xA0, 0x08),
    ILI9881C_COMMAND_INSTR(0xA1, 0x1A),
    ILI9881C_COMMAND_INSTR(0xA2, 0x27),
    ILI9881C_COMMAND_INSTR(0xA3, 0x15),
    ILI9881C_COMMAND_INSTR(0xA4, 0x17),
    ILI9881C_COMMAND_INSTR(0xA5, 0x2A),
    ILI9881C_COMMAND_INSTR(0xA6, 0x1E),
    ILI9881C_COMMAND_INSTR(0xA7, 0x1F),
    ILI9881C_COMMAND_INSTR(0xA8, 0x8B),
    ILI9881C_COMMAND_INSTR(0xA9, 0x1B),
    ILI9881C_COMMAND_INSTR(0xAA, 0x27),
    ILI9881C_COMMAND_INSTR(0xAB, 0x78),
    ILI9881C_COMMAND_INSTR(0xAC, 0x18),
    ILI9881C_COMMAND_INSTR(0xAD, 0x18),
    ILI9881C_COMMAND_INSTR(0xAE, 0x4C),
    ILI9881C_COMMAND_INSTR(0xAF, 0x21),
    ILI9881C_COMMAND_INSTR(0xB0, 0x27),
    ILI9881C_COMMAND_INSTR(0xB1, 0x54),
    ILI9881C_COMMAND_INSTR(0xB2, 0x67),
    ILI9881C_COMMAND_INSTR(0xB3, 0x39),
    ILI9881C_COMMAND_INSTR(0xC0, 0x08),
    ILI9881C_COMMAND_INSTR(0xC1, 0x1A),
    ILI9881C_COMMAND_INSTR(0xC2, 0x27),
    ILI9881C_COMMAND_INSTR(0xC3, 0x15),
    ILI9881C_COMMAND_INSTR(0xC4, 0x17),
    ILI9881C_COMMAND_INSTR(0xC5, 0x2A),
    ILI9881C_COMMAND_INSTR(0xC6, 0x1E),
    ILI9881C_COMMAND_INSTR(0xC7, 0x1F),
    ILI9881C_COMMAND_INSTR(0xC8, 0x8B),
    ILI9881C_COMMAND_INSTR(0xC9, 0x1B),
    ILI9881C_COMMAND_INSTR(0xCA, 0x27),
    ILI9881C_COMMAND_INSTR(0xCB, 0x78),
    ILI9881C_COMMAND_INSTR(0xCC, 0x18),
    ILI9881C_COMMAND_INSTR(0xCD, 0x18),
    ILI9881C_COMMAND_INSTR(0xCE, 0x4C),
    ILI9881C_COMMAND_INSTR(0xCF, 0x21),
    ILI9881C_COMMAND_INSTR(0xD0, 0x27),
    ILI9881C_COMMAND_INSTR(0xD1, 0x54),
    ILI9881C_COMMAND_INSTR(0xD2, 0x67),
    ILI9881C_COMMAND_INSTR(0xD3, 0x39),
    ILI9881C_SWITCH_PAGE_INSTR(0),
    ILI9881C_COMMAND_INSTR(0x35, 0x00),
    ILI9881C_COMMAND_INSTR(0x3A, 0x7),
};

struct ili9881c_panel_priv {
	struct gpio_desc reset;
	unsigned int lanes;
	enum mipi_dsi_pixel_format format;
	unsigned long mode_flags;
};

static const struct display_timing default_timing = {
	.pixelclock.typ		= ( 61776000 << 1 ),
	.hactive.typ		= LCD_XSIZE_TFT,
	.hfront_porch.typ	= LCD_HFPD,
	.hback_porch.typ	= LCD_HBPD,
	.hsync_len.typ		= LCD_HSPW,
	.vactive.typ		= LCD_YSIZE_TFT,
	.vfront_porch.typ	= LCD_VFPD,
	.vback_porch.typ	= LCD_VBPD,
	.vsync_len.typ		= LCD_VSPW,
	.flags = DISPLAY_FLAGS_HSYNC_LOW |
		 DISPLAY_FLAGS_VSYNC_LOW |
		 DISPLAY_FLAGS_DE_LOW,
};

static u8 color_format_from_dsi_format(enum mipi_dsi_pixel_format format)
{
	switch (format) {
	case MIPI_DSI_FMT_RGB565:
		return 0x55;
	case MIPI_DSI_FMT_RGB666:
	case MIPI_DSI_FMT_RGB666_PACKED:
		return 0x66;
	case MIPI_DSI_FMT_RGB888:
		return 0x77;
	default:
		return 0x77; /* for backward compatibility */
	}
};

static int ili9881c_switch_page(struct mipi_dsi_device *dsi, u8 page)
{
    u8 buf[4] = { 0xff, 0x98, 0x81, page };
    int ret;

    ret = mipi_dsi_dcs_write_buffer(dsi, buf, sizeof(buf));
    if (ret < 0)
        return ret;

    return 0;
}

static int ili9881c_send_cmd_data(struct mipi_dsi_device *dsi, u8 cmd, u8 data)
{
	u8 buf[2] = { cmd, data };
    int ret;

    ret = mipi_dsi_dcs_write_buffer(dsi, buf, sizeof(buf));
    if (ret < 0)
        return ret;

    return 0;
}

static int ili9881c_enable(struct udevice *dev)
{
	struct ili9881c_panel_priv *priv = dev_get_priv(dev);
	struct mipi_dsi_panel_plat *plat = dev_get_platdata(dev);
	struct mipi_dsi_device *dsi = plat->device;
	u8 color_format = color_format_from_dsi_format(priv->format);
	int ret;
	unsigned int i;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	for (i = 0; i < ARRAY_SIZE(ili9881c_init); i++) {
		const struct ili9881c_instr *instr = &ili9881c_init[i];

		if (instr->op == ILI9881C_SWITCH_PAGE)
			ret = ili9881c_switch_page(dsi, instr->arg.page);
		else if (instr->op == ILI9881C_COMMAND)
			ret = ili9881c_send_cmd_data(dsi, instr->arg.cmd.cmd,
			      instr->arg.cmd.data);

		if (ret) {
			dev_warn(dev, "%s %d Failed command  # = [ 0x%x ]; ret = [ %d ]\n",__func__,__LINE__,i,ret);
			return ret;
		}
	}

	/* The default value is 4-lane mode
	* Issue if 2-lane required
	*/
	if (dsi->lanes == 2) {
	ret = ili9881c_switch_page(dsi, 1);
	if (ret)
	    return ret;

	ret = ili9881c_send_cmd_data(dsi, 0xB7, 0x03);
	if (ret)
	    return ret;

	ret = ili9881c_switch_page(dsi, 0);
	if (ret)
	    return ret;
	}

	ret = mipi_dsi_dcs_set_tear_on(dsi, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	if (ret)
		return ret;

	/* Set tear scanline */
	ret = mipi_dsi_dcs_set_tear_scanline(dsi, 0x380);
	if (ret < 0) {
		dev_warn(dev, "Failed to set tear scanline (%d)\n", ret);
		return ret;
	}

	/* Set pixel format */
	ret = mipi_dsi_dcs_set_pixel_format(dsi, color_format);
	if (ret < 0) {
		dev_warn(dev, "Failed to set pixel format (%d)\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_display_brightness(dsi, 0xffff);
	if (ret < 0) {
		dev_warn(dev, "Failed to set display brightness (%d)\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret) {
		dev_warn(dev, "Failed to exit sleep mode (%d)\n", ret);
		return ret;
	}
	mdelay(120);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret) {
		dev_warn(dev, "Failed to set display on (%d)\n", ret);
		return ret;
	}
	mdelay(20);

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	dev_dbg(dev,"Set %d-lane mode ; Color %d\n",dsi->lanes,color_format);

	return 0;
}

static int ili9881c_panel_enable_backlight(struct udevice *dev)
{
	struct mipi_dsi_panel_plat *plat = dev_get_platdata(dev);
	struct mipi_dsi_device *device = plat->device;
	int ret;

	ret = mipi_dsi_attach(device);
	if (ret < 0)
		return ret;

	return ili9881c_enable(dev);
}

static int ili9881c_panel_get_display_timing(struct udevice *dev,
					    struct display_timing *timings)
{
	struct mipi_dsi_panel_plat *plat = dev_get_platdata(dev);
	struct mipi_dsi_device *device = plat->device;
	struct ili9881c_panel_priv *priv = dev_get_priv(dev);

	memcpy(timings, &default_timing, sizeof(*timings));

	/* fill characteristics of DSI data link */
	if (device) {
		device->lanes = priv->lanes;
		device->format = priv->format;
		device->mode_flags = priv->mode_flags;
	}

	return 0;
}

static int ili9881c_panel_probe(struct udevice *dev)
{
	struct ili9881c_panel_priv *priv = dev_get_priv(dev);
	int ret;

	priv->format = MIPI_DSI_FMT_RGB888;
	priv->mode_flags = MIPI_DSI_MODE_VIDEO_SYNC_PULSE;
	priv->mode_flags |= MIPI_DSI_MODE_VIDEO;

	ret = dev_read_u32(dev, "dsi-lanes", &priv->lanes);
	if (ret) {
		dev_warn(dev, "Failed to get dsi-lanes property (%d)\n", ret);
		return ret;
	}

	ret = gpio_request_by_name(dev, "reset-gpio", 0, &priv->reset,
				   GPIOD_IS_OUT);
	if (ret) {
		dev_warn(dev, "Warning: cannot get reset GPIO\n");
		if (ret != -ENOENT)
			return ret;
	}

	/* reset panel */
	ret = dm_gpio_set_value(&priv->reset, true);
	if (ret)
		dev_warn(dev, "reset gpio fails to set true\n");

	mdelay(100);

	ret = dm_gpio_set_value(&priv->reset, false);
	if (ret)
		dev_warn(dev, "reset gpio fails to set true\n");

	mdelay(100);

	return 0;
}

static int ili9881c_panel_disable(struct udevice *dev)
{
	struct ili9881c_panel_priv *priv = dev_get_priv(dev);

	dm_gpio_set_value(&priv->reset, true);

	return 0;
}

static const struct panel_ops ili9881c_panel_ops = {
	.enable_backlight = ili9881c_panel_enable_backlight,
	.get_display_timing = ili9881c_panel_get_display_timing,
};

static const struct udevice_id ili9881c_panel_ids[] = {
	{ .compatible = "startek,ili9881c" },
	{ }
};

U_BOOT_DRIVER(ili9881c_panel) = {
	.name			  = "ili9881c_panel",
	.id			  = UCLASS_PANEL,
	.of_match		  = ili9881c_panel_ids,
	.ops			  = &ili9881c_panel_ops,
	.probe			  = ili9881c_panel_probe,
	.remove			  = ili9881c_panel_disable,
	.platdata_auto_alloc_size = sizeof(struct mipi_dsi_panel_plat),
	.priv_auto_alloc_size	= sizeof(struct ili9881c_panel_priv),
};
