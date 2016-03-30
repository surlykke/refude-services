#!/usr/bin/sh
VARNAME=`basename $1 | sed -e 's/\./_/g'`
echo "const char $VARNAME[] = {" > $2
echo "`xxd -i < $1`, 0x00" >> $2
echo "};" >> $2
