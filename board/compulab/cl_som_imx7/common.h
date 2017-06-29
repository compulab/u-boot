/*
 * (C) Copyright 2017 CompuLab, Ltd. http://compulab.com/
 *
 * Author: Uri Mashiach <uri.mashiach@compulab.co.il>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* CL-SOM-iMX7 base board ID */
typedef enum {
	SOM_IMX7_SB_SOM,
	SOM_IMX7_SB_IOT,
	SOM_IMX7_IOTG,
	SOM_IMX7_OTHER,
} som_imx7_base;

extern som_imx7_base som_imx7_base_id;
