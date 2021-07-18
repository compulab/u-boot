#
# Copyright 2018 CompuLab
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += mcm-imx8m-mini.o

ccflags-y += -I$(srctree)/board/compulab/plat/imx8mm/

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
endif

obj-y += ../plat/imx8mm/
