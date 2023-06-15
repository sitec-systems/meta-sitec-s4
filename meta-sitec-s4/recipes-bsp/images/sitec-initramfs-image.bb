require recipes-core/images/core-image-minimal.bb

PR = "r0"

IMAGE_FSTYPES = "${INITRAMFS_FSTYPES}"
IMAGE_FEATURES_remove = "package-management"

IMAGE_INSTALL += "mtd-utils"
IMAGE_INSTALL += "mtd-utils-ubifs"
IMAGE_INSTALL += "dropbear"
IMAGE_INSTALL += "usbnet"
