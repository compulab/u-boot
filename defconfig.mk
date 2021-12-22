.DEFAULT: compound_defconfig
compound_defconfig:
	scripts/kconfig/merge_config.sh -O configs/ -m configs/cl-imx8m-mini_defconfig configs/${MACHINE}.config
	mv configs/.config configs/${MACHINE}_defconfig
	$(MAKE) -f Makefile ${MACHINE}_defconfig

