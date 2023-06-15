# Copyright (C) 2016 Robert Lehmann <robert.lehmann@sitec-systems.de>

DESCRIPTION = "Terminal Mutlitplexer for GSM Modems"
HOMEPAGE = "http://www.sitec-systems.de"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/COPYING;md5=8f0e2cd40e05189ec81232da84bd6e1a"
SECTION = "base"
PR = "r5"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

inherit insane

SRC_URI += "file://${PN}-${PV}.zip"
SRC_URI += "file://gsmmuxctl.sh"
SRC_URI += "file://gsmmux.conf"

S = "${WORKDIR}"

do_compile() {
    cd ${S}
	oe_runmake all
}

do_install() {
    install -d ${D}${bindir}/
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${S}/gsmmux ${D}${bindir}
    install -m 0755 ${S}/gsmmuxctl.sh ${D}${sysconfdir}/init.d/gsmmuxctl
    install -m 0755 ${S}/gsmmux.conf ${D}${sysconfdir}/gsmmux.conf
}

FILES_${PN} += "${bindir}/gsmmux"
FILES_${PN} += "${sysconfdir}/init.d/gsmmuxctl"
FILES_${PN} += "${sysconfdir}/gsmmux.conf"
