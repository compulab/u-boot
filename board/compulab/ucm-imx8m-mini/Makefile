#
# Copyright 2018 CompuLab
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += ucm-imx8m-mini.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_UCM_IMX8M_LPDDR4) += lpddr4_timing.o
obj-$(CONFIG_UCM_IMX8M_LPDDR4_VAL) += ddr/
endif
