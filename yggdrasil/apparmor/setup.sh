#!/bin/bash
APPARMOR=/etc/apparmor.d
MJOLLNIR=$APPARMOR/mjollnir

rm -rf $MJOLLNIR/
mkdir $MJOLLNIR/

for base in $(ls *.base); do
    cp $base $MJOLLNIR/$base
done

for profile in $(ls *.profile); do
    cp $profile $MJOLLNIR/$profile
    if apparmor_parser -a $MJOLLNIR/$profile 2>/dev/null; then
    	echo -e "$profile => LOADED"
    elif apparmor_parser -r $MJOLLNIR/$profile 2>/dev/null; then
    	echo -e "$profile => RELOADED"
    else
    	echo -e "$profile => ERROR"
    fi
done
