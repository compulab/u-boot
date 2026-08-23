#
# Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
#
# SPDX-License-Identifier:	GPL-2.0+
#
ifdef CONFIG_SPL_BUILD
ALL-y	+= MLO
ALL-$(CONFIG_SPL_SPI_SUPPORT) += MLO.byteswap
else
ALL-y	+= u-boot.img

ifdef CONFIG_TARGET_CM_T43
ALL-y	+= cm-t43-firmware

quiet_cmd_cm_t43_firmware = FWIMAGE $@
cmd_cm_t43_firmware = \
	set -e; \
	test $$(wc -c < MLO.byteswap) -le $$((64 * 1024)) || \
		{ echo "MLO.byteswap is too large for a 64 KiB firmware slot" >&2; exit 1; }; \
	test $$(wc -c < MLO) -le $$((64 * 1024)) || \
		{ echo "MLO is too large for a 64 KiB firmware slot" >&2; exit 1; }; \
	test $$(wc -c < u-boot.img) -le $$((512 * 1024)) || \
		{ echo "u-boot.img is too large for the CM-T43 firmware image" >&2; exit 1; }; \
	rm -f $@.tmp; \
	trap 'rm -f $@.tmp' 0; \
	dd if=/dev/zero bs=1K count=768 2>/dev/null | \
		tr '\000' '\377' > $@.tmp; \
	dd if=MLO.byteswap of=$@.tmp bs=1K seek=0 conv=notrunc 2>/dev/null; \
	dd if=MLO.byteswap of=$@.tmp bs=1K seek=64 conv=notrunc 2>/dev/null; \
	dd if=MLO.byteswap of=$@.tmp bs=1K seek=128 conv=notrunc 2>/dev/null; \
	dd if=MLO of=$@.tmp bs=1K seek=192 conv=notrunc 2>/dev/null; \
	dd if=u-boot.img of=$@.tmp bs=1K seek=256 conv=notrunc 2>/dev/null; \
	mv $@.tmp $@; \
	trap - 0

cm-t43-firmware: spl/u-boot-spl.bin u-boot.img FORCE
	$(call if_changed,cm_t43_firmware)

CLEAN_FILES += cm-t43-firmware
endif
endif
