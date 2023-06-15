#!/bin/sh

# vbusenable -- Enable the vbus voltage

case $1 in
	start)
		i2cset -f -y 3 0x08 0x66 0x48
		;;
	stop)
		i2cset -f -y 3 0x08 0x66 0x00
		;;
	*)
		printf "Unsupported operations\n"
		;;
esac