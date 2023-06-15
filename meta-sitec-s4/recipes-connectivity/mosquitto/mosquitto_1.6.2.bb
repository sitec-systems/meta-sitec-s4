SUMMARY = "MQTT 3.1 compliant library"
HOMEPAGE = "https://mosquitto.org"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

PR = "r1"

LICENSE = "EPL-1.0"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=62ddc846179e908dc0c8efec4a42ef20"

SRC_URI = "https://mosquitto.org/files/source/mosquitto-${PV}.tar.gz"
SRC_URI += "file://0001-activate-websockets.patch"
SRC_URI += "file://mosquitto.sh"
SRC_URI[md5sum] = "754929d9606f3473cf28faf33f7b8b6a"
SRC_URI[sha256sum] = "33499e78dfa0ca1cb488fd196fde940a66305bdfd44ba763ce2001db2569a08b"

DEPENDS = "openssl libwebsockets"
RDEPENDS_${PN} = "util-linux libwebsockets"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}/usr/sbin
    install -d ${D}${bindir}
    install -d ${D}${sysconfdir}/mosquitto
    install -d ${D}${sysconfdir}/init.d
    install -d ${D}${libdir}
    install -d ${D}${includedir}

    install -m 0755 ${S}/lib/libmosquitto.so.1 ${D}${libdir}
    install -m 0755 ${S}/lib/cpp/libmosquittopp.so.1 ${D}${libdir}
    install -m 0755 ${S}/src/mosquitto ${D}/usr/sbin
    install -m 0755 ${S}/src/mosquitto_passwd ${D}${bindir}
    install -m 0755 ${S}/client/mosquitto_rr ${D}${bindir}
    install -m 0755 ${S}/client/mosquitto_pub ${D}${bindir}
    install -m 0755 ${S}/client/mosquitto_sub ${D}${bindir}

    install -m 0755 ${S}/mosquitto.conf ${D}${sysconfdir}/mosquitto
    install -m 0755 ${S}/../mosquitto.sh ${D}${sysconfdir}/init.d/mosquitto

    cd ${D}${libdir} && ln -s libmosquittopp.so.1 libmosquittopp.so
    cd ${D}${libdir} && ln -s libmosquitto.so.1 libmosquitto.so
    cd ${D}${libdir} && ln -s libmosquittopp.so.1 libmosquittopp.so.1.6.2
    cd ${D}${libdir} && ln -s libmosquitto.so.1 libmosquitto.so.1.6.2

    install -m 0655 ${S}/lib/mosquitto.h ${D}${includedir}
    install -m 0655 ${S}/lib/cpp/mosquittopp.h ${D}${includedir}
    install -m 0655 ${S}/src/mosquitto_plugin.h ${D}${includedir}
}
