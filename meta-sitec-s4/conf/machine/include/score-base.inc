SERIAL_CONSOLE = "115200 ttymxc0"
SOC_FAMILY = "mx6:mx6sx"
DEFAULTTUNE_mx6sx ?= "cortexa9hf-neon"

KERNEL_IMAGETYPE = "uImage"
UBOOT_CONFIG[score] = "score_config"

IMAGE_FSTYPES += "tar.bz2"

PREFERRED_PROVIDER_virtual/kernel_mx6 = "linux-sitec"
PREFERRED_VERSION_linux-sitec = "4.1.15"
PREFERRED_PROVIDER_u-boot = "u-boot-imx"
PREFERRED_VERSION_u-boot-imx_mx6 = "2015.04"
PREFERRED_VERSION_linux-libc-headers = "4.1"

UBOOT_ENTRYPOINT_mx6sx = "0x80008000"
