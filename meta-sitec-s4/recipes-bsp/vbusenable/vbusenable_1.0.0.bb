# Copyright (C) 2016 Robert Lehmann <robert.lehmann@sitec-systems.de>

DESCRIPTION = "Enable the vbus voltage for USBOTG2"
HOMEPAGE = "www.sitec-systems.de"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"
LICENSE = "CLOSED"
SECTION = "base"
RDEPENDS_${PN} = "i2c-tools"

PR = "r0"

SRC_URI = "file://vbusenable.sh"

S = "${WORKDIR}"

INITSCRIPT_NAME = "vbusenable"
INITSCRIPT_PARAMS = "start 11 S ."

inherit update-rc.d

do_install() {
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${S}/vbusenable.sh ${D}${sysconfdir}/init.d/vbusenable
}
