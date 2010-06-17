/*
 * cm_t3517.h - Header file for the CM-T3517.
 *
 * Authors: Mike Rapoport <mike@compulab.co.il>
 *	    Igor Grinberg <grinberg@compulab.co.il>
 *
 * Based on logicpd/am3517evm/am3517evm.h 
 *
 * Copyright (C) 2010
 * CompuLab, Ltd. -  http://www.compulab.co.il/
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

#ifndef _AM3517EVM_H_
#define _AM3517EVM_H_

const omap3_sysinfo sysinfo = {
	DDR_DISCRETE,
	"CM-T3517 Board",
	"NAND",
};
/* CM-T3517 specific mux configuration */
#define CONTROL_PADCONF_SYS_NRESWARM	0x0A08
/* CCDC */
#define CONTROL_PADCONF_CCDC_PCLK	0x01E4
#define CONTROL_PADCONF_CCDC_FIELD	0x01E6
#define CONTROL_PADCONF_CCDC_HD		0x01E8
#define CONTROL_PADCONF_CCDC_VD		0x01EA
#define CONTROL_PADCONF_CCDC_WEN	0x01EC
#define CONTROL_PADCONF_CCDC_DATA0	0x01EE
#define CONTROL_PADCONF_CCDC_DATA1	0x01F0
#define CONTROL_PADCONF_CCDC_DATA2	0x01F2
#define CONTROL_PADCONF_CCDC_DATA3	0x01F4
#define CONTROL_PADCONF_CCDC_DATA4	0x01F6
#define CONTROL_PADCONF_CCDC_DATA5	0x01F8
#define CONTROL_PADCONF_CCDC_DATA6	0x01FA
#define CONTROL_PADCONF_CCDC_DATA7	0x01FC
/* RMII */
#define CONTROL_PADCONF_RMII_MDIO_DATA	0x01FE
#define CONTROL_PADCONF_RMII_MDIO_CLK	0x0200
#define CONTROL_PADCONF_RMII_RXD0	0x0202
#define CONTROL_PADCONF_RMII_RXD1	0x0204
#define CONTROL_PADCONF_RMII_CRS_DV	0x0206
#define CONTROL_PADCONF_RMII_RXER	0x0208
#define CONTROL_PADCONF_RMII_TXD0	0x020A
#define CONTROL_PADCONF_RMII_TXD1	0x020C
#define CONTROL_PADCONF_RMII_TXEN	0x020E
#define CONTROL_PADCONF_RMII_50MHZ_CLK	0x0210
#define CONTROL_PADCONF_USB0_DRVBUS	0x0212
/* CAN */
#define CONTROL_PADCONF_HECC1_TXD	0x0214
#define CONTROL_PADCONF_HECC1_RXD	0x0216

#define CONTROL_PADCONF_SYS_BOOT7	0x0218
#define CONTROL_PADCONF_SDRC_DQS0N	0x021A
#define CONTROL_PADCONF_SDRC_DQS1N	0x021C
#define CONTROL_PADCONF_SDRC_DQS2N	0x021E
#define CONTROL_PADCONF_SDRC_DQS3N	0x0220
#define CONTROL_PADCONF_STRBEN_DLY0	0x0222
#define CONTROL_PADCONF_STRBEN_DLY1	0x0224
#define CONTROL_PADCONF_SYS_BOOT8	0x0226

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
#define MUX_CM_T3517() \
	/* SDRC */\
	MUX_VAL(CP(SDRC_D0),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D1),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D2),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D3),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D4),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D5),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D6),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D7),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D8),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D9),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D10),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D11),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D12),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D13),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D14),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D15),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D16),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D17),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D18),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D19),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D20),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D21),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D22),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D23),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D24),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D25),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D26),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D27),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D28),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D29),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D30),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_D31),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_CLK),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_DQS0),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_DQS1),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_DQS2),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_DQS3),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SDRC_DQS0N),		(IEN  | PTD | EN  | M0)) \
	MUX_VAL(CP(SDRC_DQS1N),		(IEN  | PTD | EN  | M0)) \
	MUX_VAL(CP(SDRC_DQS2N),		(IEN  | PTD | EN  | M0)) \
	MUX_VAL(CP(SDRC_DQS3N),		(IEN  | PTD | EN  | M0)) \
	MUX_VAL(CP(SDRC_CKE0),		(M0)) \
	MUX_VAL(CP(SDRC_CKE1),		(M0)) \
	/*sdrc_strben_dly0*/\
	MUX_VAL(CP(STRBEN_DLY0),	(IEN  | PTD | EN  | M0)) \
	 /*sdrc_strben_dly1*/\
	MUX_VAL(CP(STRBEN_DLY1),	(IEN  | PTD | EN  | M0)) \
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
	MUX_VAL(CP(GPMC_NCS1),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NCS2),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NCS3),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NCS4),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NCS5),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NCS6),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_NCS7),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_CLK),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NADV_ALE),	(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_NOE),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_NWE),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_NBE0_CLE),	(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NBE1),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_NWP),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(GPMC_WAIT0),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_WAIT1),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(GPMC_WAIT2),		(IEN  | PTU | EN  | M4)) /*GPIO_64*/\
							 /* - ETH_nRESET*/\
	MUX_VAL(CP(GPMC_WAIT3),		(IEN  | PTU | EN  | M0)) \
	/* MMC */\
	MUX_VAL(CP(MMC1_CLK),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(MMC1_CMD),		(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(MMC1_DAT0),		(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(MMC1_DAT1),		(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(MMC1_DAT2),		(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(MMC1_DAT3),		(IEN  | PTU | DIS | M0)) \
	/* WriteProtect */\
	MUX_VAL(CP(MMC1_DAT4),		(IEN  | PTU | EN  | M4)) \
	MUX_VAL(CP(MMC1_DAT5),		(IEN  | PTU | EN  | M4)) /*CardDetect*/\
	MUX_VAL(CP(MMC1_DAT6),		(IEN  | PTU | EN  | M4)) \
	MUX_VAL(CP(MMC1_DAT7),		(IEN  | PTU | EN  | M4)) \
	\
	MUX_VAL(CP(MMC2_CLK),		(IEN  | PTD | EN  | M0)) \
	MUX_VAL(CP(MMC2_CMD),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(MMC2_DAT0),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(MMC2_DAT1),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(MMC2_DAT2),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(MMC2_DAT3),		(IEN  | PTD | DIS | M0)) \
	/* UART */\
	MUX_VAL(CP(UART1_TX),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(UART1_RTS),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(UART1_CTS),		(IEN  | PTU | DIS | M0)) \
	\
	MUX_VAL(CP(UART1_RX),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(UART2_CTS),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(UART2_RTS),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(UART2_TX),		(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(UART2_RX),		(IEN  | PTD | DIS | M0)) \
	\
	MUX_VAL(CP(UART3_CTS_RCTX),	(IEN  | PTU | DIS | M0)) \
	MUX_VAL(CP(UART3_RTS_SD),	(IDIS | PTD | DIS | M0)) \
	MUX_VAL(CP(UART3_RX_IRRX),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(UART3_TX_IRTX),	(IDIS | PTD | DIS | M0)) \
	/* I2C */\
	MUX_VAL(CP(I2C1_SCL),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C1_SDA),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C2_SCL),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C2_SDA),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C3_SCL),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C3_SDA),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C4_SCL),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(I2C4_SDA),		(IEN  | PTU | EN  | M0)) \
	/* CCDC */\
	MUX_VAL(CP(CCDC_PCLK),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(CCDC_FIELD),		(IEN  | PTD | DIS | M1)) \
	MUX_VAL(CP(CCDC_HD),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(CCDC_VD),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(CCDC_WEN),		(IEN  | PTD | DIS | M1)) \
	MUX_VAL(CP(CCDC_DATA0),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(CCDC_DATA1),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(CCDC_DATA2),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(CCDC_DATA3),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(CCDC_DATA4),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(CCDC_DATA5),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(CCDC_DATA6),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(CCDC_DATA7),		(IEN  | PTD | DIS | M0)) \
	/* RMII */\
	MUX_VAL(CP(RMII_MDIO_DATA),	(IEN  |  M0)) \
	MUX_VAL(CP(RMII_MDIO_CLK),	(M0)) \
	MUX_VAL(CP(RMII_RXD0)	,	(IEN  | PTD | M0)) \
	MUX_VAL(CP(RMII_RXD1),		(IEN  | PTD | M0)) \
	MUX_VAL(CP(RMII_CRS_DV),	(IEN  | PTD | M0)) \
	MUX_VAL(CP(RMII_RXER),		(PTD | M0)) \
	MUX_VAL(CP(RMII_TXD0),		(PTD | M0)) \
	MUX_VAL(CP(RMII_TXD1),		(PTD | M0)) \
	MUX_VAL(CP(RMII_TXEN),		(PTD | M0)) \
	MUX_VAL(CP(RMII_50MHZ_CLK),	(IEN  | PTD | EN  | M0)) \
	/* HECC */\
	MUX_VAL(CP(HECC1_TXD),		(IEN  | PTU | EN  | M0)) \
	MUX_VAL(CP(HECC1_RXD),		(IEN  | PTU | EN  | M0)) \
	/* HSUSB */\
	MUX_VAL(CP(HSUSB0_CLK),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_STP),		(IDIS | PTU | EN  | M0)) \
	MUX_VAL(CP(HSUSB0_DIR),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_NXT),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_DATA0),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_DATA1),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_DATA2),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_DATA3),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_DATA4),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_DATA5),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_DATA6),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(HSUSB0_DATA7),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(USB0_DRVBUS),	(IEN  | PTD | EN  | M0)) \
	/* HDQ */\
	MUX_VAL(CP(HDQ_SIO),		(IEN  | PTU | EN  | M0)) \
	/* Control and debug */\
	MUX_VAL(CP(SYS_32K),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SYS_CLKREQ),		(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SYS_NIRQ),		(IEN  | PTU | EN  | M0)) \
	/*SYS_nRESWARM */\
	MUX_VAL(CP(SYS_NRESWARM),     	(IDIS | PTU | DIS | M4)) \
							/* - GPIO30 */\
	MUX_VAL(CP(SYS_BOOT0),		(IEN  | PTD | DIS | M4)) /*GPIO_2*/\
							 /* - PEN_IRQ */\
	MUX_VAL(CP(SYS_BOOT1),		(IEN  | PTD | DIS | M4)) /*GPIO_3 */\
	MUX_VAL(CP(SYS_BOOT2),		(IEN  | PTD | DIS | M4)) /*GPIO_4*/\
	MUX_VAL(CP(SYS_BOOT3),		(IEN  | PTD | DIS | M4)) /*GPIO_5*/\
	MUX_VAL(CP(SYS_BOOT4),		(IEN  | PTD | DIS | M4)) /*GPIO_6*/\
	MUX_VAL(CP(SYS_BOOT5),		(IEN  | PTD | DIS | M4)) /*GPIO_7*/\
	MUX_VAL(CP(SYS_BOOT6),		(IDIS | PTD | DIS | M4)) /*GPIO_8*/\
							 /* - VIO_1V8*/\
	MUX_VAL(CP(SYS_BOOT7),		(IEN  | PTD | EN  | M0)) \
	MUX_VAL(CP(SYS_BOOT8),		(IEN  | PTD | EN  | M0)) \
	\
	MUX_VAL(CP(SYS_OFF_MODE),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SYS_CLKOUT1),	(IEN  | PTD | DIS | M0)) \
	MUX_VAL(CP(SYS_CLKOUT2),	(IEN  | PTU | EN  | M0))
#endif
