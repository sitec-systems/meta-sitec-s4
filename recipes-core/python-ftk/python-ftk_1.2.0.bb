SUMMARY = "Factory tool kit"
HOMEPAGE = "http://www.sitec-systems.de"
SECTION = "core"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

PR = "r0"

LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.1;md5=1a6d268fd218675ffea8be556788b780"

RDEPENDS_${PN} = "python python-argparse python-subprocess mtd-utils-ubifs u-boot-fw-utils"

SRCNAME = "ftk"
SRC_URI = "https://github.com/sitec-systems/ftk/releases/download/${PV}/${SRCNAME}-${PV}.tar.gz"
SRC_URI[md5sum] = "fb7b6f267f215fb1090b325be5c9f587"

S = "${WORKDIR}/${SRCNAME}-${PV}"

inherit setuptools
