SUMMARY = "Module for Low Power Actions on sitec systems S4 Comm Pro X"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

inherit module

PR = "r1"
PV = "${KERNEL_VERSION}"

SRC_URI = "file://Makefile \
           file://main.c \
           file://COPYING \
           file://sitec_lp.h \
           file://sts_fm.c \
           file://sts_fm.h \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
