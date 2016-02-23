#!/bin/bash

# Usage : ./<script> <example_filename>
SERVER_FILE=$1
TMP_FILE=./log_$SERVER_FILE.log
rm -rf $TMP_FILE

./$SERVER_FILE 2> /dev/null &
PID=$!
sleep 2

for i in {1..50}; do
	curl http://127.0.0.1:4567 -s >> $TMP_FILE
	echo "" >> $TMP_FILE
done
uniq_count=$(cat $TMP_FILE | sort | uniq | wc -l | xargs)
echo "$uniq_count"

kill $PID
ps -o pid,command|grep $SERVER_FILE|grep python|awk '{print $1}'|xargs kill
