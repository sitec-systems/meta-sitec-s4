# Copyright (C) 2016 Robert Lehmann <robert.lehmann@sitec-systems.de>

DESCRIPTION = "UDEV Rules for u-Blox modems"
HOMEPAGE = "www.sitec-systems.de"
LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=1a6d268fd218675ffea8be556788b780"
SECTION = "base"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"
RDEPENDS_${PN} = "bash udev"

PR = "r0"

SRC_URI += "file://LICENSE"
SRC_URI += "file://60-ublox-neo.rules"
SRC_URI += "file://60-ublox-lara-r2.rules"
SRC_URI += "file://60-ublox-lara-r6.rules"
SRC_URI += "file://60-ublox-sara.rules"
SRC_URI += "file://60-ublox-toby.rules"
SRC_URI += "file://ublox-modem-helper"

do_install() {
    install -d ${D}${sysconfdir}/udev/rules.d
    install -d ${D}/bin
    install -m 0644 ${WORKDIR}/60-ublox-sara.rules ${D}${sysconfdir}/udev/rules.d
    install -m 0644 ${WORKDIR}/60-ublox-lara-r2.rules ${D}${sysconfdir}/udev/rules.d
    install -m 0644 ${WORKDIR}/60-ublox-lara-r6.rules ${D}${sysconfdir}/udev/rules.d
    install -m 0644 ${WORKDIR}/60-ublox-neo.rules ${D}${sysconfdir}/udev/rules.d
    install -m 0644 ${WORKDIR}/60-ublox-toby.rules ${D}${sysconfdir}/udev/rules.d
    install -m 0744 ${WORKDIR}/ublox-modem-helper ${D}/bin/ublox-modem-helper
}

FILES_${PN} += "${sysconfdir}/udev/rules.d/60-ublox-lisa.rules"
FILES_${PN} += "${sysconfdir}/udev/rules.d/60-ublox-sara.rules"
FILES_${PN} += "${sysconfdir}/udev/rules.d/60-ublox-lara-r2.rules"
FILES_${PN} += "${sysconfdir}/udev/rules.d/60-ublox-lara-r6.rules"
FILES_${PN} += "${sysconfdir}/udev/rules.d/60-ublox-neo.rules"
FILES_${PN} += "${sysconfdir}/udev/rules.d/60-ublox-toby.rules"
FILES_${PN} += "/bin/ublox-modem-helper"
