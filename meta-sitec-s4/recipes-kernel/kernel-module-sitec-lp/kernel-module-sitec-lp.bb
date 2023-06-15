SUMMARY = "Module for Low Power Actions on sitec systems S4 Comm Pro X"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

inherit module

require recipes-kernel/linux/sitec-kernel-revision.inc

PR = "${SITEC_KERNEL_REVISION}"
PV = "1.0.0"

SRC_URI = "file://Makefile \
           file://main.c \
           file://sitec_lp.h \
           file://sts_fm.c \
           file://sts_fm.h \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
