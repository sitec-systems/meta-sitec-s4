# Copyright (C) 2016 Robert Lehmann <robert.lehmann@sitec-systems.de>

SUMMARY = "Example configuration for ppp service"
LICENSE = "LGPL-2.1"
SECTION = "base"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=1a6d268fd218675ffea8be556788b780"
DEPENDS_${PN} = "ppp"
RDEPENDS_${PN} += "ublox-udev"

PR = "r4"

SRC_URI = "file://09connfile"
SRC_URI += "file://93removeconnfile"
SRC_URI += "file://apn"
SRC_URI += "file://gprs"
SRC_URI += "file://modem.chat"
SRC_URI += "file://options.mobile"
SRC_URI += "file://LICENSE"

do_install () {
    install -d ${D}${sysconfdir}/ppp/peers
    install -d ${D}${sysconfdir}/ppp/ip-up.d
    install -d ${D}${sysconfdir}/ppp/ip-down.d
    install -d ${D}${sysconfdir}/chatscripts

    install -m 0644 ${WORKDIR}/options.mobile ${D}${sysconfdir}/ppp
    install -m 0644 ${WORKDIR}/gprs ${D}${sysconfdir}/ppp/peers
    install -m 0755 ${WORKDIR}/09connfile ${D}${sysconfdir}/ppp/ip-up.d
    install -m 0755 ${WORKDIR}/93removeconnfile ${D}${sysconfdir}/ppp/ip-down.d

    install -m 0644 ${WORKDIR}/modem.chat ${D}${sysconfdir}/chatscripts
    install -m 0644 ${WORKDIR}/apn ${D}${sysconfdir}/chatscripts
}
