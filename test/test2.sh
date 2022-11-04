#! /bin/sh

if [ X${1:-} = X-u ]; then
  cp basic2.log basic2.txt
else
  sh basic2.sh ${1:-} | sed 's/\(v[0-9][0-9.]*\)[a-z]!/\1!/' >basic2.log
  diff -a basic2.txt basic2.log
fi
