#!/usr/bin/env bash
tmp=`mktemp`
cat license-header-template-js.txt > tmp
cat $1 >> tmp
mv tmp $1

