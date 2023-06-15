SUMMARY = "Writing rootFS from compressed tar archive to ubi volume"
HOMEPAGE = "http://www.sitec-systems.de"
SECTION = "base"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"
DESCRIPTION = "This tool extracts a compressed rootFS archive an writes the\
content to an ubi volume. flashubi detects the flash-chip specific parameters\
(like physical erase blocks) and create the filesystem with these parameters"

LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.1;md5=1a6d268fd218675ffea8be556788b780"

SRC_URI = "file://flashubi.sh"
S = "${WORKDIR}"

RDEPENDS_${PN} = "mtd-utils-ubifs"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${S}/flashubi.sh ${D}/${bindir}/flashubi
}