#/bin/sh

# Control Script for gsmmux daemon
#
# Copyright (C) 2016 sitec systems GmbH
#
# Author: Robert Lehmann <robert.lehmann@sitec-systems.de>

# Debug: set to one if you want debug outprints
DEBUG=0

# Constants
SCRIPT=$0
GSM_DEV_PATH="/proc/device-tree/sitec/v1/device@1/equipment_category/gsm_module/gsm_module@1/"
PID_FILE="/var/run/gsmmux.pid"
CONFIG="/etc/gsmmux.conf"

# Default values
isgsm=0           # GSM is not availible
uart=/dev/ttymxc5 # Use UART6 by default
wait=1            # wait for start

msg() {
    log_msg="$SCRIPT [$1] $2"

    logger "$log_msg"

    if [ $DEBUG -eq 1 ]; then
        echo "$log_msg"
    fi
}

info() {
    msg "INFO" "$1"
    echo "[INFO] $1"
}

err() {
    msg "ERROR" "$1"
    echo "ERROR" "$1"
}

debug() {
    if [ $DEBUG -eq 1 ]; then
        echo "DEBUG: $1"
    fi
}

checkgsm() {
    mod_name=$(cat "$GSM_DEV_PATH/mod_name")
    debug $mod_name
    if [ $mod_name = "Sara-G" ]; then
        isgsm=1
        uart=$(cat "$GSM_DEV_PATH/equipment@1/interface")
        debug $uart
    else
        isgsm=0
    fi
}

start_gsmmux() {
    info "starting gsmmux ..."
    if [ $wait = 1 ]; then
        gsmopt="-w"
    fi
    gsmmux $gsmopt -p $uart -s /dev/ttyMODEMA /dev/ptmx /dev/ptmx > /dev/null
	if [ $? -ne 0 ]; then
		err "Can't start gsmmux"
        exit 1
	fi
    pidof gsmmux > /dev/null
    if [ $? -eq 0 ]; then
        echo $(pidof gsmmux) > $PID_FILE
    fi
    info "... gsmmux is up"
}

stop_gsmmux() {
    if [ -f $PID_FILE ]; then
        info "shutdown gsmmux ..."
        kill $(cat $PID_FILE)
        info "... gsmmux is down"
        rm $PID_FILE
    else
        echo "gsmmux is not running"
    fi
}

read_config() {
    if [ -f $CONFIG ]; then
        . $CONFIG
    fi
}

checkgsm
read_config

case $1 in
    start)
        if [ $isgsm -ne 1 ]; then
            info "No GSM Modem availible"
            exit 2
        fi
        start_gsmmux
        ;;
    stop)
        stop_gsmmux
        ;;
    restart)
        stop_gsmmux
        start_gsmmux
        ;;
    status)
        if [ -f $PID_FILE ]; then
            echo "gsmmux is running on pid $(cat $PID_FILE)"
        else
            pidof gsmmux > /dev/null
            if [ $? -eq 0 ]; then
                echo $(pidof gsmmux) > $PID_FILE
                exit 0
            else
                echo "gsmmux is not running"
                exit 3
            fi
        fi
        ;;
    *)
        echo "$SCRIPT start|stop|restart|status"
        echo "Return values:"
        echo "1: Error during start process"
        echo "2: No GSM availible"
        echo "3: Service not running"
        ;;
esac
