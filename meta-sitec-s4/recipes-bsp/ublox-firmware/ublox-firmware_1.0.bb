DESCRIPTION = "Firmware files for u-blox modules"
HOMEPAGE = "www.sitec-systems.de"

LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.1;md5=1a6d268fd218675ffea8be556788b780"
SECTION = "base"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

PR = "r1"

SRC_URI += "file://sd8787_uapsta.bin"
SRC_URI += "file://sd8887_uapsta.bin"

S = "${WORKDIR}"

do_install() {
    install -d ${D}/lib/firmware/mrvl
    install -m 0644 ${S}/sd8787_uapsta.bin ${D}/lib/firmware/mrvl
    install -m 0644 ${S}/sd8887_uapsta.bin ${D}/lib/firmware/mrvl
}

FILES_${PN} += "/lib/firmware/mrvl/*"