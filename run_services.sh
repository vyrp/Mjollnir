#!/bin/bash -e

usage() {
    echo "Usage:"
    echo "  $0 all|bif|jot|mon|ygg"
}

bif() {
    echo "Running Bifrost, log at /Mjollnir/bifrost/bifrost.log"
    cd /Mjollnir/bifrost
    ps aux | grep -v grep | grep -q foreman || foreman start &>> bifrost.log &
}

jot() {
    echo "Running Jotunheim, log at /Mjollnir/jotunheim/jotunheim.log"
    sudo service jotunheim start 1> /dev/null
}

mon() {
    echo "Running MongoDB, log at /var/log/mongodb/mongodb.log"
    ps aux | grep -v grep | grep -q mongod || mongod --fork --config /etc/mongodb.conf
}

ygg() {
    echo "Running Yggdrasil, log at /Mjollnir/yggdrasil/server/logs/yggdrasil.log"
    sudo service yggdrasil start 1> /dev/null
}

case "$1" in
    all)
        mon
        ygg
        jot
        bif
    ;;
    bif)
        mon
        bif
    ;;
    jot)
        mon
        ygg
        jot
    ;;
    mon)
        mon
    ;;
    ygg)
        mon
        ygg
    ;;
    *)
        usage
    ;;
esac

