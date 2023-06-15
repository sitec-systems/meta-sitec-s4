SUMMARY = "fmt library"
LICENSE =  "BSD-2-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.rst;md5=c2e38bc8629eac247a73b65c1548b2f0"

FILES_${PN} += "${libdir}/cmake/fmt/*.cmake"

SRC_URI = "https://github.com/fmtlib/fmt/archive/${PV}.tar.gz"
SRC_URI[sha256sum] = "73d4cab4fa8a3482643d8703de4d9522d7a56981c938eca42d929106ff474b44"

S = "${WORKDIR}/fmt-${PV}"

inherit pkgconfig cmake

