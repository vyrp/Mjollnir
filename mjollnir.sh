#!/bin/bash -e

usage() {
    echo "Usage:"
    echo "  $0 start|stop|restart all|bif|jot|mon|ygg"
    echo "  Note: this script takes into account dependencies"
    exit 1
}

## Start functions ##

start_all() {
    start_mon
    start_bif
    start_ygg
    start_jot
}

start_bif() {
    start_mon quiet
    sleep 1

    if pgrep -f foreman 1> /dev/null; then
        if [ "$1" != "quiet" ]; then
            echo "Bifrost already running"
        fi
    else
        echo "Starting Bifrost,   log at /Mjollnir/bifrost/bifrost.log"
        cd /Mjollnir/bifrost
        foreman start &>> bifrost.log &
    fi
}

start_jot() {
    start_ygg quiet
    sleep 1

    if pgrep -f jotunheim 1> /dev/null; then
        if [ "$1" != "quiet" ]; then
            echo "Jotunheim already running"
        fi
    else
        echo "Starting Jotunheim, log at /Mjollnir/jotunheim/jotunheim.log"
        sudo service jotunheim start 1> /dev/null
    fi
}

start_mon() {
    if pgrep -f mongod 1> /dev/null; then
        if [ "$1" != "quiet" ]; then
            echo "MongoDB already running"
        fi
    else
        echo "Starting MongoDB,   log at /var/log/mongodb/mongodb.log"
        sudo mongod --fork --config /etc/mongodb.conf 1> /dev/null
    fi
}

start_ygg() {
    start_mon quiet
    sleep 1

    if pgrep -f yggdrasil 1> /dev/null; then
        if [ "$1" != "quiet" ]; then
            echo "Yggdrasil already running"
        fi
    else
        echo "Starting Yggdrasil,   log at /Mjollnir/yggdrasil/server/logs/yggdrasil.log"
        sudo service yggdrasil start 1> /dev/null
    fi
}

## Stop functions ##

stop_all() {
    stop_jot
    stop_ygg
    stop_bif
    stop_mon
}

stop_bif() {
    if pgrep -f foreman 1> /dev/null; then
        echo "Stopping Bifrost,   log at /Mjollnir/bifrost/bifrost.log"
        pkill -f foreman
    elif [ "$1" != "quiet" ]; then
        echo "Bifrost not running"
    fi
}

stop_jot() {
    if pgrep -f jotunheim 1> /dev/null; then
        echo "Stopping Jotunheim, log at /Mjollnir/jotunheim/jotunheim.log"
        sudo service jotunheim stop 1> /dev/null
    elif [ "$1" != "quiet" ]; then
        echo "Jotunheim not running"
    fi
}

stop_mon() {
    stop_bif quiet
    stop_ygg quiet
    
    if pgrep -f mongod 1> /dev/null; then
        echo "Stopping MongoDB,   log at /var/log/mongodb/mongodb.log"
        sudo pkill -f mongod
    elif [ "$1" != "quiet" ]; then
        echo "MongoDB not running"
    fi
}

stop_ygg() {
    stop_jot quiet

    if pgrep -f yggdrasil 1> /dev/null; then
        echo "Stopping Yggdrasil, log at /Mjollnir/yggdrasil/server/logs/yggdrasil.log"
        sudo service yggdrasil stop 1> /dev/null
    elif [ "$1" != "quiet" ]; then
        echo "Yggdrasil not running"
    fi
}

## Status functions ##

status() {
    if pgrep -f $1 1> /dev/null; then
        echo "$2 is running"
    else
        echo "$2 is NOT running"
    fi
}

status_all() {
    status_bif
    status_jot
    status_mon
    status_ygg
}

status_bif() {
    status foreman Bifrost
}

status_jot() {
    status jotunheim Jotunheim
}

status_mon() {
    status mongod MongoDB
}

status_ygg() {
    status yggdrasil Yggdrasil
}

## Handling input ##

case "$1" in
    start|stop|status) ;;
    *) usage ;;
esac

case "$2" in
    all|bif|jot|mon|ygg) $1_$2 true;;
    *) usage ;;
esac

