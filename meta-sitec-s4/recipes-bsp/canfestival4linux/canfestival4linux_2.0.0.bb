SUMMARY = "canfestival library for Linux"
DESCRIPTION = "Library is a port of the famous canfestival library.\
    All microcontroller code is gone. It's a linux only library which uses cmake as buildsystem"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"
HOMEPAGE = "https://github.com/sitec-systems/canfestival4linux"
LICENSE = "LGPLv2"
LIC_FILES_CHKSUM = "file://LICENCE;md5=085e7fb76fb3fa8ba9e9ed0ce95a43f9"

PR = "r0"

SRC_URI = "https://github.com/sitec-systems/canfestival4linux/archive/${PV}.zip"
SRC_URI[md5sum] = "f322d1cb4d3b06aa46d8f751ef6fc177"
SRC_URI[sha256sum] = "ea75af62db697d11d83ff157b0ead027d52b376f885ea98d32097e47d5a15021"

S = "${WORKDIR}/${PN}-${PV}"

inherit cmake

EXTRA_OECMAKE = " -DSDO_MAX_LENGTH_TRANSFER=65536 "

do_install() {
    install -d ${D}${libdir} ${D}${includedir}/canfestival ${D}${includedir}/canfestival/unix
    install -m 0655 ${WORKDIR}/build/drivers/libcanfestival_can_socket.so ${D}${libdir}/libcanfestival_can_socket.so
    install -m 0655 ${WORKDIR}/build/drivers/libcanfestival_unix.so ${D}${libdir}/libcanfestival_unix.so
    install -m 0655 ${WORKDIR}/build/src/libcanfestival.so ${D}${libdir}/libcanfestival.so

    install -m 0655 ${WORKDIR}/build/config.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/unix/applicfg.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/unix/canfestival.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/can_driver.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/can.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/data.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/dcf.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/def.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/emcy.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/lifegrd.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/lss.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/nmtMaster.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/nmtSlave.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/objacces.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/objdictdef.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/pdo.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/sdo.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/states.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/sync.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/sysdep.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/timer.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/timers_unix/timerscfg.h ${D}${includedir}/canfestival
    install -m 0655 ${S}/include/timers_driver.h ${D}${includedir}/canfestival
}

FILES_${PN} = "${libdir}/*.so"
FILES_${PN}-dev = "${includedir}/*"
