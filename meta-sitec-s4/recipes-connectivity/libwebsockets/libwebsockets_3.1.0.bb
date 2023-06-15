SUMMARY = "Websocket C library"
HOMEPAGE = "https://libwebsockets.org"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

LICENSE = "LGPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=4ce87f3facb6f911c142c8bef9bfb380"

DEPENDS_${PN} = "openssl"
RDEPENDS_${PN} = "libcap openssl"

PR = "r0"

SRC_URI = "https://github.com/warmcat/libwebsockets/archive/v${PV}.zip"
SRC_URI[md5sum] = "72da65198e8137c5ba1468d219408d94"
SRC_URI[sha256sum] = "0fd3c2bcd5de7b45ac2f54542da39798259d10d53ce5878bdca7071f985073f3"

inherit cmake

FILES_${PN}-dev += "/usr/lib/cmake"
FILES_${PN}-doc += "/usr/share"
