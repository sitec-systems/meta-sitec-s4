SUMMARY = "Activate and initialize the usb net interface"
LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.1;md5=1a6d268fd218675ffea8be556788b780"
SECTION = "base"
RDEPENDS_${PN} = "initscripts"
PR = "r3"

SRC_URI = "file://usbnet"

INITSCRIPT_NAME = "usbnet"
INITSCRIPT_PARAMS = "defaults 90"

inherit update-rc.d

do_install () {
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/usbnet ${D}${sysconfdir}/init.d
}
