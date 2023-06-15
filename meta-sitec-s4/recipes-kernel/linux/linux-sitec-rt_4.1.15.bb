# Copyright (C) 2013-2015 Freescale Semiconductor
# Copyright (C) 2016 sitec systems GmbH

SUMMARY = "Linux Kernel provided and supported by Freescale"
DESCRIPTION = "Linux Kernel provided and supported by Freescale with focus on \
i.MX Family Reference Boards. It includes support for many IPs such as GPU, VPU and IPU."

require recipes-kernel/linux/linux-imx.inc
require recipes-kernel/linux/linux-dtb.inc

DEPENDS += "lzop-native bc-native"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
LOCALVERSION = "-1.2.0"
PR = "r0"

SRCBRANCH = "imx_4.1.15_1.0.0_ga"
SRCREV = "a4d2a08f3bfc57ff5d7d1307fb23f52f1e681aca"
KERNEL_SRC ?= "git://github.com/sitec-systems/linux-sitec.git;protocol=git"

SRC_URI = "${KERNEL_SRC};branch=${SRCBRANCH}"
SRC_URI += "file://gcc6_integrate_fix.patch"
SRC_URI += "file://patch-4.1.15-rt18.patch"
SRC_URI += "file://bcmhd_gcc6_indent_warning_error_fix.patch"
SRC_URI += "file://gpu-viv_gcc6_indent_warning_error_fix.patch"
SRC_URI += "file://1000-Makefile-Add-sitec-label-to-version.patch"
SRC_URI += "file://1001-mach-imx6sx-Add-gpio-initialisation.patch"
SRC_URI += "file://1002-gpio-wakeup-Add-driver-for-wakeup-over-gpio.patch"
SRC_URI += "file://1003-tty-imx-Fix-dma-buffer-overflow-for-low-latency.patch"
SRC_URI += "file://1004-dts-imx6sx-score-Initial-commit.patch"
SRC_URI += "file://1005-misc-tja1145-Initial-commit.patch"
SRC_URI += "file://1006-dts-imx6sx-score-Add-config-for-uart2-and-uart6.patch"
SRC_URI += "file://1007-arm-mach-imx6sx-Add-GPIOs-for-init-routine.patch"
SRC_URI += "file://1008-misc-tja1145-Fix-read-and-write-access.patch"
SRC_URI += "file://1009-mach-imx6sx-Add-init-for-native-and-expander-gpios.patch"
SRC_URI += "file://1010-mach-imx6sx-Change-direction-of-reference-clk.patch"
SRC_URI += "file://1011-mach-imx6sx-Add-init-for-lan-power-enable-pin.patch"
SRC_URI += "file://1012-imx6sx-pinfunc-Add-SION-bit-for-ENET2-Ref-Clock.patch"
SRC_URI += "file://1013-imx6sx-score.dtsi-Change-the-ref-clk.patch"
SRC_URI += "file://1014-clk-imx6sx-Change-clk-root-for-ENET.patch"
SRC_URI += "file://1015-misc-tja1145-Extend-control-possibilites.patch"
SRC_URI += "file://1016-mach-imx-gpio_label-Add-label-for-kill_4P.patch"
SRC_URI += "file://1017-mach-imx-mach-imx6sx-Add-export-for-kill_4P.patch"
SRC_URI += "file://1018-mach-imx6sx-Add-fixup-for-ksz8091-phy.patch"
SRC_URI += "file://1019-mach-imx6sx-Export-IRQ-GPIOs-to-userspace.patch"
SRC_URI += "file://1020-mach-imx-gpio_label-Add-label-for-buzzer-gpio.patch"
SRC_URI += "file://1021-mach-imx6sx-Add-export-for-buzzer-gpio.patch"
SRC_URI += "file://1022-mach-imx-gpio-label-Add-gpio-labels-for-new-GPIOs.patch"
SRC_URI += "file://1023-mach-imx6sx-Change-init-routine-of-GPIOs-for-S4-hard.patch"
SRC_URI += "file://1024-mach-imx6sx-Remove-CAN2_Wake-from-export.patch"
SRC_URI += "file://1025-drivers-tja1145-Change-ID-value.patch"
SRC_URI += "file://1026-mach-imx6sx-Change-IO-State-add-init-of-the-kernel.patch"
SRC_URI += "file://1027-ti-ads8688-Initial-commit-of-driver.patch"
SRC_URI += "file://1028-mach-imx6sx-Add-init-of-fc-gpios.patch"
SRC_URI += "file://9000-dts-imx6sx-s4-Initial-commit.patch"
SRC_URI += "file://9001-dts-imx6sx-s4-Add-pin-muxing-for-K-Line-control-line.patch"
SRC_URI += "file://9002-dts-imx6sx-s4-Add-base-for-I2C-io-expander.patch"
SRC_URI += "file://9003-dts-imx6sx-s4-Change-the-roles-for-USB-interfaces.patch"
SRC_URI += "file://9004-dts-imx6sx-s4-Add-some-GPIOs-to-pin-mux.patch"
SRC_URI += "file://9005-dts-imx6sx-s4-Fix-dr_mode-of-usbotg1.patch"
SRC_URI += "file://9006-dts-imx6sx-s4-Cleanup-dts.patch"
SRC_URI += "file://9007-dts-imx6sx-s4-Rework-configuration-for-tja1145.patch"
SRC_URI += "file://9008-dts-imx6sx-s4-Add-init-for-CAN2-Controller.patch"
SRC_URI += "file://9009-imx6sx-s4-Add-pinmux-for-IRQ-pins.patch"
SRC_URI += "file://9010-imx6sx-s4-Add-pinmux-settings-for-buzzer-gpio.patch"
SRC_URI += "file://9011-imx6sx-s4-Add-initialisation-for-I2C2.patch"
SRC_URI += "file://9012-imx6sx-s4-Change-device-tree-for-new-hardware-revisi.patch"
SRC_URI += "file://9013-imx6sx-s4-Change-information-for-SPI-devices.patch"
SRC_URI += "file://9014-imx6sx-s4-Add-fully-support-for-rev-A1.patch"
SRC_URI += "file://9015-imx6sx-s4-Add-pinmux-settings-for-OTG-ID-pin.patch"
SRC_URI += "file://9016-imx6sx-s4-Change-settings-for-external-i2c.patch"
SRC_URI += "file://9017-imx6sx-s4-Change-settings-for-USB1.patch"
SRC_URI += "file://9018-imx6sx-s4-Add-support-for-onewire.patch"
SRC_URI += "file://9019-imx6sx-s4-Add-FC-IO-Expander-and-ADC.patch"

SRC_URI += "file://defconfig"
SRC_URI += "file://enable_preempt_rt.cfg"

DEFAULT_PREFERENCE = "1"

COMPATIBLE_MACHINE = "(mx6)"

pkg_postinst_kernel-image_append() {
    flash_erase /dev/mtd1 0 0
    nandwrite -p /dev/mtd1 $(readlink /boot/uImage)
}
