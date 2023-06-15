# Copyright (c) 2018 sitec systems GmbH

DESCRIPTION = "Java Runtime environment based by Azul-Systems"
HOMEPAGE = "https://www.azul.com/downloads/zulu-embedded/"
LICENSE = "CLOSED"
SECTION = "base"
RDEPENDS_${PN} = ""

PR = "r0"

ZULU_NAME= "ezdk-1.8.0_172-8.30.0.106-eval-linux_aarch32hf"
SRC_URI = "file://${ZULU_NAME}.tar.gz"
SRC_URI = "file://zulupostinst.sh"

INSANE_SKIP_${PN} = "${ERROR_QA} ${WARN_QA}"

# INSANE_SKIP_${PN} = "ldflags"
# INSANE_SKIP_${PN} += "already-stripped"
# INSANE_SKIP_${PN} += "textrel"

BBCLASSEXTEND = "native"

inherit update-rc.d

INITSCRIPT_NAME = "zulupostinst"
INITSCRIPT_PARAMS = "start 10 S ."

do_configure() {
}

do_compile() {
}

do_install() {
    install -d ${D}/opt/zulu
    install -d ${D}${sysconfdir}/init.d
    install -m 0644 ${THISDIR}/files/${ZULU_NAME}.tar.gz ${D}/opt/zulu/jre.tar.gz
    install -m 0755 ${WORKDIR}/zulupostinst.sh ${D}${sysconfdir}/init.d/zulupostinst
}

pkg_postinst_${PN} () {
#!/bin/sh -e
mkdir -p /usr/lib/jvm/java-8-zulu
mv /opt/zulu/jre.tar.gz /tmp
tar -xzf /tmp/jre.tar.gz -C /usr/lib/jvm/java-8-zulu
rm -rf /tmp/jre.tar.gz
update-alternatives --install /usr/bin/java java /usr/lib/jvm/java-8-zulu/jre/bin/java 90
}

pkg_prerm_${PN} () {
#!/bin/sh -e
update-alternatives --remove java /usr/lib/jvm/java-8-zulu/jre/bin/java
cd /usr/lib/jvm/java-8-zulu/ && tar -czf /tmp/jre.tar.gz jre
rm -rf /usr/lib/jvm/java-8-zulu
mv /tmp/jre.tar.gz /opt/zulu/jre.tar.gz
}

FILES_${PN} = "/opt/zulu"