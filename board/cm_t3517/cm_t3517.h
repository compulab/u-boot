/*
 * cm_t3517.h - Header file for CompuLab CM-T3517.
 *
 * Author: Igor Grinberg <grinberg@compulab.co.il>
 *
 * Based on ti/am3517evm/am3517evm.h 
 *
 * Copyright (C) 2010
 * CompuLab, Ltd. -  www.compulab.co.il
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _CM_T3517_H_
#define _CM_T3517_H_

const omap3_sysinfo sysinfo = {
	DDR_DISCRETE,
	"CM-T3517 Module",
	"NAND 128/512M",
};

/*
 * IEN  - Input Enable
 * IDIS - Input Disable
 * PTD  - Pull type Down
 * PTU  - Pull type Up
 * DIS  - Pull type selection is inactive
 * EN   - Pull type selection is active
 * M0   - Mode 0
 * The commented string gives the final mux configuration for that pin
 */

/* CM-T3517 specific mux configuration */
#define MUX_CM_T3517() \
	/* SDRC is setup by the x-loader and we should be in DRAM already */\
	/* GPMC */\
	MUX_VAL(CP(GPMC_A1),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A2),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A3),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A4),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A5),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A6),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A7),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A8),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A9),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_A10),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D0),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D1),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D2),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D3),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D4),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D5),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D6),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D7),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D8),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D9),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D10),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D11),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D12),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D13),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D14),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_D15),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NCS0),		(IDIS | PTU | EN  | M0)) \
	/* SB-T35 DVI transmitter reset GPIO54 */\
	MUX_VAL(CP(GPMC_NCS3),		(IDIS | PTU | EN  | M4)) \
	/* SB-T35 Ethernet nCS GPIO55 */\
	MUX_VAL(CP(GPMC_NCS4),		(IDIS | PTU | EN  | M0)) \
	/* SB-T35 LCD Back Light Enable GPIO58 */\
	MUX_VAL(CP(GPMC_NCS7),		(IDIS | PTD | EN  | M4)) \
	/* SB-T35 SD/MMC WP GPIO59 */\
	MUX_VAL(CP(GPMC_CLK),		(IEN  | PTU | EN  | M4)) \
	MUX_VAL(CP(GPMC_NWE),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_NOE),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_NADV_ALE),	(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_NBE0_CLE),	(IDIS | PTU | EN  | M0)) \
	/* SB-T35 Audio Enable GPIO61 */\
	MUX_VAL(CP(GPMC_NBE1),		(IDIS | PTU | EN  | M4)) \
	MUX_VAL(CP(GPMC_NWP),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_WAIT0),		(IEN  | PTU | EN  | M0)) \
	/* SB-T35 Ethernet IRQ GPIO65 */\
	MUX_VAL(CP(GPMC_WAIT3),		(IEN  | PTU | EN  | M4)) \
	\
	/* UART3 */\
	/* RTC V3020 nCS GPIO163 */\
	MUX_VAL(CP(UART3_CTS_RCTX),	(IEN  | PTU | EN  | M4)) \
	/* SB-T35 Ethernet nRESET GPIO164 */\
	MUX_VAL(CP(UART3_RTS_SD),	(IDIS | PTU | EN  | M4)) \
	/* Console */\
	MUX_VAL(CP(UART3_RX_IRRX),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(UART3_TX_IRTX),	(IDIS | PTD | DIS | M0)) \
	\
	/* UART2 */\
	/* SB-T35 SD/MMC CD GPIO144 */\
	MUX_VAL(CP(UART2_CTS),		(IEN  | PTU | EN  | M4)) \
	/* WIFI nRESET GPIO145 */\
	MUX_VAL(CP(UART2_RTS),		(IEN  | PTD | EN  | M4)) \
	/* USB1 PHY Reset GPIO 146 */\
	MUX_VAL(CP(UART2_TX),		(IEN  | PTD | EN  | M4)) \
	/* USB2 PHY Reset GPIO 147 */\
	MUX_VAL(CP(UART2_RX),		(IEN  | PTD | EN  | M4)) \
	\
	/* SB-T35 SD/MMC1 */\
	MUX_VAL(CP(MMC1_CLK),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(MMC1_CMD),		(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(MMC1_DAT0),		(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(MMC1_DAT1),		(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(MMC1_DAT2),		(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(MMC1_DAT3),		(IEN  | PTU | DIS | M0)) \
	\
	/* DSS */\
	MUX_VAL(CP(DSS_DATA0),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA1),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA2),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA3),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA4),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA5),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA6),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA7),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA8),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA9),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA10),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA11),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA12),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA13),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA14),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA15),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA16),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA17),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA18),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA19),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA20),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA21),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA22),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_DATA23),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_PCLK),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_HSYNC),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_VSYNC),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(DSS_ACBIAS),		(IDIS | PTD | DIS | M0)) \
	\
	/* I2C */\
	MUX_VAL(CP(I2C1_SCL),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C1_SDA),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C3_SCL),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C3_SDA),		(IEN  | PTU | EN  | M0)) \
	\
	/* CCDC */\
	/* SB-T35 USB HUB Reset GPIO98 */\
	MUX_VAL(CP(CCDC_WEN),		(IDIS | PTU | EN  | M4)) \
	\
	/* RMII */\
	MUX_VAL(CP(RMII_MDIO_DATA),	(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(RMII_MDIO_CLK),	(M0)) \
	MUX_VAL(CP(RMII_RXD0)	,	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(RMII_RXD1),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(RMII_CRS_DV),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(RMII_RXER),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(RMII_TXD0),		(IDIS | M0)) \
	MUX_VAL(CP(RMII_TXD1),		(IDIS | M0)) \
	MUX_VAL(CP(RMII_TXEN),		(IDIS | M0)) \
	MUX_VAL(CP(RMII_50MHZ_CLK),	(IEN  | PTU | DIS | M0)) \
	\
	/* Green LED GPIO186 */\
	MUX_VAL(CP(SYS_CLKOUT2),	(IDIS | PTD | DIS | M4)) \
	\
	/* MCSPI */\
	/* USB2 */\
	MUX_VAL(CP(MCSPI1_CS3),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(MCSPI2_CLK),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(MCSPI2_SIMO),	(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(MCSPI2_SOMI),	(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(MCSPI2_CS0),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(MCSPI2_CS1),		(IEN  | PTD | DIS | M3)) \
	\
	/* MCBSP */\
	/* USB2 HUB nRST GPIO152 */\
	MUX_VAL(CP(MCBSP4_CLKX),	(IDIS | PTD | DIS | M4)) \
	/* SB-T35 Toppoly LCD SPI CLK GPIO156 */\
	MUX_VAL(CP(MCBSP1_CLKR),	(IEN  | PTU | DIS | M1)) \
	/* SB-T35 Toppoly LCD Reset GPIO157 */\
	MUX_VAL(CP(MCBSP1_FSR),		(IEN  | PTU | DIS | M4)) \
	/* SB-T35 Toppoly LCD SPI SIMO GPIO158 */\
	MUX_VAL(CP(MCBSP1_DX),		(IEN  | PTU | DIS | M1)) \
	/* SB-T35 Toppoly LCD SPI SOMI GPIO159 */\
	MUX_VAL(CP(MCBSP1_DR),		(IEN  | PTU | DIS | M1)) \
	/* RTC V3020 CS Enable GPIO160 */\
	MUX_VAL(CP(MCBSP_CLKS),		(IEN  | PTD | EN  | M4)) \
	/* SB-T35 Toppoly LCD SPI CS0 GPIO161 */\
	MUX_VAL(CP(MCBSP1_FSX),		(IEN  | PTU | DIS | M1)) \
	/* SB-T35 LVDS Transmitter SHDN GPIO162 */\
	MUX_VAL(CP(MCBSP1_CLKX),	(IEN  | PTU | DIS | M4)) \
	\
	/* USB0 - mUSB */\
	MUX_VAL(CP(USB0_DRVBUS),	(IEN  | PTD | EN  | M0)) \
	\
	/* ETK */\
	/* USB1 */\
	MUX_VAL(CP(ETK_CLK),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_CTL),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D0),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D1),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D2),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D3),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D4),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D5),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D6),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D7),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D8),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D9),		(IEN  | PTD | DIS | M3)) \
	/* USB2 */\
	MUX_VAL(CP(ETK_D10),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D11),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D12),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D13),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D14),		(IEN  | PTD | DIS | M3)) \
	MUX_VAL(CP(ETK_D15),		(IEN  | PTD | DIS | M3)) \
	\
	/* SYS_BOOT */\
	MUX_VAL(CP(SYS_BOOT0),		(IEN  | PTU | DIS | M4)) \
	MUX_VAL(CP(SYS_BOOT1),		(IEN  | PTU | DIS | M4)) \
	MUX_VAL(CP(SYS_BOOT2),		(IEN  | PTU | DIS | M4)) \
	MUX_VAL(CP(SYS_BOOT3),		(IEN  | PTU | DIS | M4)) \
	MUX_VAL(CP(SYS_BOOT4),		(IEN  | PTD | DIS | M4)) \
	MUX_VAL(CP(SYS_BOOT5),		(IEN  | PTD | DIS | M4)) \

#endif /* _CM_T3517_H_ */
