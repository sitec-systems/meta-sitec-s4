#!/bin/sh

# Copyright (c) 2017 sitec systems GmbH

### BEGIN INIT INFO
# Provides:          mosquitto
# Required-Start:    $local_fs $syslog networking
# Required-Stop:    
# Default-Start:     S
# Default-Stop:      0 6
# Short-Description: Handling mosquitto service
### END INIT INFO

DAEMON=/usr/sbin/mosquitto
DAEMON_OPTS="-c /etc/mosquitto/mosquitto.conf -d"
NAME=mosquitto
DESC="MQTT Broker"
PIDFILE="/var/run/mosquitto.pid"

set -e

test -r /etc/mosquitto/mosquitto.conf || exit 0

case $1 in
  start)
    if [ -f $PIDFILE ]; then
      echo "$NAME is already running"
      exit 0
    fi

    echo -n "Starting $DESC: "
    $DAEMON $DAEMON_OPTS
    echo "$NAME"
    ;;
  stop)
    if [ ! -f $PIDFILE ]; then
      echo "$NAME is not running"
      exit 0
    fi

    echo -n "Stopping $DESC: "
    pid=$(cat $PIDFILE)
    kill $pid
    rm -rf $PIDFILE
    echo "$NAME"
    ;;
  *)
    echo "$NAME: [start|stop]"
    exit 1
    ;;
esac
