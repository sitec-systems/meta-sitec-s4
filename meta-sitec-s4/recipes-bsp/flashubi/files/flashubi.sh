#!/bin/sh

# flash UBI volume from archived rootFS
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

show_help() {
	printf "flashubi - tool for updating UBI volumes from tar.gz or tar.bz2 rootfs files\n\n"
	printf "Usage: flashubi [-h] [-v] [-q] <UBI volume node file name> <rootfs compressed archvie>\n"
	printf "Options:\n"
	printf "  -h:  Show help text\n"
	printf "  -v:  Be verbose\n"
	printf "  -q:  Be quiet (overrides -v Option)\n"
}

log() {
	if [ $quiet -eq 0 ]; then
		printf "$1\n"
	fi
}

verbose=0
quiet=0
while getopts "hvq" opt; do
	case $opt in
		h)
			show_help
			exit 0
			;;
		v)
			verbose=1
			;;
		q)
			quiet=1
			;;
		*)
			show_help
			exit 1
	esac
done

if [ $verbose -eq 1 ]; then
	shift
fi

if [ $quiet -eq 1 ]; then
	verbose=0
	shift
fi

if [ $# -le 1 ]; then
	printf "Error: Not enough arguments\n"
	exit 1
fi

ubivol=$1
archive=$2

# Check ubivol file
if [ ! -e $ubivol ]; then
	printf "Error: $ubivol doesn't exist on the system\n"
	exit 1
fi

devnumber=$(file $ubivol | grep -o -E '[[:digit:]]+/[1-9]')
if [ $? -eq 1 ]; then
   printf "Error: $ubivol is not a UBI volume device node (like /dev/ubi0_1)\n"
   exit 1
fi

echo $devnumber | grep -q -E '247/[1-9]'
if [ $? -eq 1 ]; then
   printf "Error: $ubivol is not a UBI volume device node (like /dev/ubi0_1)\n"
   exit 1
fi

# Check archive file
if [ ! -e $archive ]; then
	printf "Error: $archive doesn't exist on the system\n"
	exit 1
fi

archive_finfo=$(file $archive)
tar_arg=''

echo $archive_finfo | grep -q 'bzip2'
if [ $? -eq 0 ]; then
	tar_arg='-xjf'
fi

echo $archive_finfo | grep -q 'gzip'
if [ $? -eq 0 ]; then
	tar_arg='-xzf'
fi

if [ "$tar_arg" = "" ]; then
	printf "Error: $archive is no bzip2 or gzip compressed tar archive\n"
	exit 1
fi

TMPROOT='root'

rm -rf $TMPROOT
mkdir $TMPROOT

if [ $verbose -eq 1 ]; then
	tar_arg="-v $tar_arg $archive -C $TMPROOT"
else
	tar_arg="$tar_arg $archive -C $TMPROOT"
fi

log "Extraction rootFS from $archive"
tar $tar_arg

if [ $? -eq 1 ]; then
	printf "Error: Extraction was not successfull\n"
	exit 1
fi

if [ $verbose -eq 1 ]; then
	ubi_arg="-v -y"
else
	ubi_arg="-y"
fi

log "Writing rootFS to $ubivol"
mkfs.ubifs $ubi_arg -r $TMPROOT $ubivol

rm -rf $TMPROOT
