#!/bin/sh

# Recovery script for sitec systems S4 device
# Copyright (C) 2017  Robert Lehmann <robert.lehmann@sitec-systems.de>

# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

RECOVERY_FILE="/var/ftk/recovery"

case "$1" in
	start)
		if [ ! -e $RECOVERY_FILE ]; then
			exit 0
		fi

		logger "$0 Starting Recovery"
		echo timer > /sys/class/leds/B1/trigger
		rm $RECOVERY_FILE
		flashubi /dev/ubi0_1 /var/ftk/image.tar.bz2
		fw_setenv root_alt 0
		reboot
		;;
	*)
		;;
esac
