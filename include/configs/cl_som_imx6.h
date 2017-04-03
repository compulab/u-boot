/*
 * Config file for Compulab CM-FX6 board
 *
 * Copyright (C) 2016, Compulab Ltd - http://compulab.co.il/
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_CL_SOM_IMX6_H
#define __CONFIG_CL_SOM_IMX6_H

#define CL_SOM_IMX6
#include "cm_fx6.h"

#define CONFIG_CMD_MII


#define CONFIG_PCA953X
#define CONFIG_CMD_PCA953X
#define CONFIG_CMD_PCA953X_INFO
#define CONFIG_SYS_I2C_PCA953X_ADDR     0x26
#define CONFIG_SYS_I2C_PCA953X_WIDTH    { {CONFIG_SYS_I2C_PCA953X_ADDR, 16} }

#define CONFIG_BOARD_LATE_INIT

#endif	/* __CONFIG_CL_SOM_IMX6_H */
