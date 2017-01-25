SUMMARY = "sitec systems Recovery Script"
HOMEPAGE = "http://www.sitec-systems.de"
SECTION = "core"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"
DESCRIPTION = "This package contains a recovery script which restores the factory image into the working image"

LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.1;md5=1a6d268fd218675ffea8be556788b780"

SRC_URI = "file://srecovery.sh"
S = "${WORKDIR}"

PR = "r0"

RDEPENDS_${PN} = "mtd-utils-ubifs u-boot-imx-fw-utils flashubi"

inherit update-rc.d

INITSCRIPT_PACKAGES = "${PN}"
INITSCRIPT_NAME_${PN} = "srecovery"
INITSCRIPT_PARAMS_${PN} = "defaults 9"

do_install() {
	install -d ${D}${sysconfdir}/init.d
	install -d ${D}/var/ftk
	install -m 0755 ${S}/srecovery.sh ${D}/${sysconfdir}/init.d/srecovery
}
