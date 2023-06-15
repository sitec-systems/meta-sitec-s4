#!/bin/bash

### BEGIN INIT INFO
# Provides:          cellularctl
# Required-Start:    sysfs
# Required-Stop:
# Default-Start:     S
# Default-Stop:
# Short-Description: Start/Stop/Restart cellular modules.
### END INIT INFO

DT=/proc/device-tree/sitec/v1
DT_UNIT_NAME=$DT/unit_name
DBG_GPIO=/sys/kernel/debug/gpio
GPIO=/sys/class/gpio/gpio
MODEM_PORT=/dev/ttyMODEMA0

[ -e /etc/init.d/functions.initscripts ] || exit 1
. /etc/init.d/functions.initscripts

##
# Get the gpio number for the power enable gpio (gsm_pwr_e)
#
# Return:
#   0 Successfully getted the gpio number
#   1 gpio list in kernel debug is not availible
#   2 the gpio is not exported in the system
get_gsm_pwr_e() {
	if [ ! -f $DBG_GPIO ]; then
		echo "0"
		return 1
	fi
	gsm_pwr_e=$(cat $DBG_GPIO | grep gsm_pwr_e | sed 's/\s\+/ /g' | cut -d " " -f 2 | sed 's/gpio-//g')

	# Check if pwr_e line for gsm is availible
	if [ ! -f /sys/class/gpio/gpio$gsm_pwr_e/value ]; then
		echo "0"
		return 2
	fi

	echo $gsm_pwr_e
	return 0
}

##
# Waiting for module to be started
#
# Parameter:
#   - Timeout in seconds
# Return:
#   0 Module is activated
#   1 Invalid arguments
#   2 Timeout reached, no module is found
wait_for_module() {
	timeout=$1
	if [ $timeout == "" ]; then
		return 1
	fi

	found=0
	for i in $(seq $timeout); do
		if [ ! -e $MODEM_PORT ]; then
			sleep 1
		else
			sleep 1
			if [ -e $MODEM_PORT ]; then
				found=1
				break
			fi
		fi
	done

	if [ $found -eq 1 ]; then
		return 0
	else
		return 2
	fi
}

##
# Activate the module and wait for activation
#
# Parameters:
#   - Number for gpio pin
# Return:
#   0 Successful activated
#   1 Invalid argument
module_activate() {
	pin=$1

	if [ $pin == "" ]; then
		return 1
	fi

	echo 1 > $GPIO$pin/value
	sleep 1
	udevadm trigger
	wait_for_module 30
	ret=$?
	if [ $ret -ne 0 ]; then
		if [ $ret -eq 2 ]; then
			echo "No Modem availible"
		fi
	else
		echo "Modem started"
	fi
}

##
# Deactivate the module
#
# Parameters:
#   - Number for gpio pin
# Return:
#   0 Successful activated
#   1 Invalid argument
module_deactivate() {
	pin=$1

	if [ $pin = "" ]; then
		return 1
	fi

	echo 0 > $GPIO$pin/value
	usleep 200000 # Sleep for 200 ms
	echo "Modem stopped"
}

case "$1" in
	start)
		echo "Starting cellular module"
		pin=$(get_gsm_pwr_e)
		if [ $? -ne 0 ]; then
			echo "Unable to get the GSM power enable line from the system"
		fi
		
		module_activate $pin
		;;
	stop)
		echo "Stopping cellular module"
		pin=$(get_gsm_pwr_e)
		if [ $? -ne 0 ]; then
			echo "Unable to get the GSM power enable line from the system"
		fi

		module_deactivate $pin
		;;
	restart)
		pin=$(get_gsm_pwr_e)
		if [ $? -ne 0 ]; then
			echo "Unable to get the GSM power enable line from the system"
		fi
		echo "Stopping cellular module"
		module_deactivate $pin
		sleep 1
		echo "Starting cellular module"
		module_activate $pin
		;;
	*)
		echo "$0 start | stop | restart"
esac
