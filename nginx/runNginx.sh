#!/usr/bin/env bash

NGINXDIR=$XDG_RUNTIME_DIR/nginx
mkdir -p $NGINXDIR
sed 's@NGINXDIR@'"$XDG_RUNTIME_DIR/nginx"'@' nginx.conf.template > $NGINXDIR/nginx.conf
nginx -c $NGINXDIR/nginx.conf 

