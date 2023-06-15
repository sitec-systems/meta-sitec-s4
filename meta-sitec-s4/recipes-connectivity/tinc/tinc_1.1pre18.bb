SUMMARY = "tinc vpn software"
HOMEPAGE = "https://tinc-vpn.org"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=0aa4fadd009fe1a7c5bc7823d4a3d1a6"

PR = "r0"

SRC_URI = "https://www.tinc-vpn.org/packages/${PN}-${PV}.tar.gz"
SRC_URI[md5sum] = "2983fcbdf3775120922b672b1cc4c2df"
SRC_URI[sha256sum] = "2757ddc62cf64b411f569db2fa85c25ec846c0db110023f6befb33691f078986"

DEPENDS = "openssl lzo zlib ncurses"
RDEPENDS_${PN} = "openssl lzo zlib ncurses readline"

inherit autotools

FILES_${PN} = "/"
