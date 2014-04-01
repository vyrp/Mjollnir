#!/bin/sh
APPARMOR=/etc/apparmor.d
MJOLLNIR=$APPARMOR/mjollnir
DISABLE=$APPARMOR/disable

if [[ ($# -eq  1) && ($1 == "--disable" || $1 == "-d") ]]; then
	for profile in $(ls *.profile); do
		if ln -s $MJOLLNIR/$profile $DISABLE && apparmor_parser -R $MJOLLNIR/$profile 2>/dev/null; then
			echo -e "$profile => DISABLED"
		else
			echo -e "$profile => ERROR"
		fi
	done
elif [[ ($# -eq  1) && ($1 == "--enable" || $1 == "-e") ]]; then
	for profile in $(ls *.profile); do
		if rm -f $DISABLE/$profile && apparmor_parser -a $MJOLLNIR/$profile 2>/dev/null; then
			echo -e "$profile => ENABLED"
		else
			echo -e "$profile => ERROR"
		fi
	done
else
	echo "Choose --disable (-d) or --enable (-e)"
fi
