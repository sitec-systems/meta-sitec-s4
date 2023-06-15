#!/bin/sh

echo "Starting Zulu Postinstallation"
if [ ! -d /usr/lib/jvm/java-8-zulu/jre ]; then
    mkdir -p /usr/lib/jvm/java-8-zulu
    mv /opt/zulu/jre.tar.gz /tmp
    tar -xzf /tmp/jre.tar.gz -C /usr/lib/jvm/java-8-zulu
    rm -rf /tmp/jre.tar.gz
    update-alternatives --install /usr/bin/java java /usr/lib/jvm/java-8-zulu/jre/bin/java 90
fi
update-rc.d -f zulupostinst remove