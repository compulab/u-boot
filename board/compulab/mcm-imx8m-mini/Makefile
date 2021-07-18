#
# Copyright 2018 CompuLab
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += mcm-imx8m-mini.o

ifdef CONFIG_SPL_BUILD
ccflags-y += -I$(srctree)/board/compulab/plat/imx8mm/
obj-y += spl.o
endif

obj-y += ../plat/imx8mm/
