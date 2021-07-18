#
# Copyright 2018 CompuLab
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += ucm-imx8m-mini.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
ifdef CONFIG_IMX8M_LPDDR4
obj-y += lpddr4_timing.o
else
obj-y += ddr/
endif
endif
