#!/bin/sh
# Very simple script to run refude services
# Call from inside a DE-session, as (at least) PATH and DISPLAY variables
# are needed by the services.
#
# If you have to desktop sessions running (as same user) and call this script from both
# things go haywire.
#
set +i
for app in RefudeDesktopService RefudeIconService RefudePowerService RefudeWmService ; do
	echo "Looking for $app"	
	ps -ef | grep -v grep | grep $app >/dev/null 2>/dev/null || { echo "starting $app"; nohup $app & }
done

HAPIDFILE=$XDG_RUNTIME_DIR/refude-haproxy.pid
ps --pid `cat $HAPIDFILE` >/dev/null || haproxy -f /etc/refude/haproxy.cfg -p $HAPIDFILE
