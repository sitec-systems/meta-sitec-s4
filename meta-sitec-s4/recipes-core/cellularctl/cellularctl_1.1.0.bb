DESCRIPTION = "Tool for controlling cellular modules on the S4"
HOMEPAGE = "http://www.sitec-systems.de"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"
SECTION = "base"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.1;md5=1a6d268fd218675ffea8be556788b780"
PR = "r0"

S = "${WORKDIR}"
SRC_URI += "file://cellularctl"

do_install() {
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${S}/cellularctl ${D}${sysconfdir}/init.d/cellularctl
}
