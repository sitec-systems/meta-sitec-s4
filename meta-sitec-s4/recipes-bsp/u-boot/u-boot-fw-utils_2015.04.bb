# Copyright (C) 2016 Robert Lehmann <robert.lehmann@sitec-systems.de>
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "U-Boot bootlader fw_printenv/setenv utilities"
HOMEPAGE = "http://www.sitec-systems.de"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/gpl-2.0.txt;md5=b234ee4d69f5fce4486a80fdaf4a4263"
SECTION = "base"
DEPENDS = "mtd-utils"

FILESEXTRAPATHS_prepend := "${THISDIR}/u-boot-imx:"

SRCBRANCH = "nxp/imx_v2015.04_4.1.15_1.0.0_ga"
SRCREV = "384e1c1f527d732a863cef360f5da419a728b1d5"
SRC_URI = "git://source.codeaurora.org/external/imx/uboot-imx;protocol=https;branch=${SRCBRANCH}"

SRC_URI += "file://fw_env.config"
SRC_URI += "file://0001-score-Add-board-score.patch"
SRC_URI += "file://0002-arch-arm-Kconfig-Add-score-board-to-arm-configuratio.patch"
SRC_URI += "file://0003-board-score-Add-methode-for-getting-the-cs-of-spi.patch"
SRC_URI += "file://0004-config-score_defconfig-Add-option-for-SPINOR.patch"
SRC_URI += "file://0005-include-configs-score-Cleaning-Up.patch"
SRC_URI += "file://0006-tools-env-env_flags-Add-ARRAY_SIZE-macro.patch"

S = "${WORKDIR}/git"

PR = "r1"

inherit uboot-config

INSANE_SKIP_${PN} = "already-stripped"

EXTRA_OEMAKE_class-target = 'CROSS_COMPILE=${TARGET_PREFIX} CC="${CC} ${CFLAGS} ${LDFLAGS}" V=1'
EXTRA_OEMAKE_class-cross = 'ARCH=${TARGET_ARCH} CC="${CC} ${CFLAGS} ${LDFLAGS}" V=1'

do_compile() {
    oe_runmake ${UBOOT_MACHINE}
    oe_runmake env
}

do_install() {
    install -d ${D}${bindir}/
    install -d ${D}${sysconfdir}/
    install -m 0755 ${S}/tools/env/fw_printenv ${D}${bindir}/fw_printenv
    install -m 0755 ${S}/tools/env/fw_printenv ${D}${bindir}/fw_setenv
    install -m 0644 ${WORKDIR}/fw_env.config ${D}${sysconfdir}
}

FILES_${PN} += "${sysconfdir}/* ${bindir}/*"

BBCLASSEXTEND = "cross"
PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(mx6)"

