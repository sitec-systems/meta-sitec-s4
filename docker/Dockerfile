FROM debian:8
MAINTAINER Robert Lehmann <robert.lehmann@sitec-systems.de>

RUN sed -i '/deb http:\/\/deb.debian.org\/debian jessie-updates main/d' /etc/apt/sources.list && \
    sed -i 's/deb.debian.org/debian.inf.tu-dresden.de\/debian-archive/g' /etc/apt/sources.list && \
    sed -i 's/security.debian.org/debian.inf.tu-dresden.de\/debian-archive/g' /etc/apt/sources.list

RUN apt-get update && apt-get install --force-yes -yq \
    sudo \
    build-essential \
    git \
    python \
    python3 \
    man \
    bash \
    diffstat \
    gawk \
    chrpath \
    wget \
    cpio \
    texinfo \
    lzop \
    apt-utils \
    bc \
    vim \
    zip \
    libx11-dev \
    upx \
    screen && \
    rm -rf /var/lib/apt-lists/*

RUN groupadd -g 1000 build && \
    useradd -ms /bin/bash -u 1000 -g build build && \
    usermod -aG sudo build

USER build
WORKDIR /home/build
