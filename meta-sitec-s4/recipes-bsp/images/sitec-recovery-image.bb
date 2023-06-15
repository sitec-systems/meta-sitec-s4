require recipes-core/images/core-image-minimal.bb

PR = "r2"

IMAGE_INSTALL += "u-boot-fw-utils"
IMAGE_INSTALL += "iproute2"
IMAGE_INSTALL += "packagegroup-core-ssh-openssh"
IMAGE_INSTALL += "vbusenable"
IMAGE_INSTALL += "kernel-image"
IMAGE_INSTALL += "kernel-devicetree"
IMAGE_INSTALL += "bsp-version"
