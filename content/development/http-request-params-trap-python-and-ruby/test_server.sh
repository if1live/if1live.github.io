#!/bin/bash

# Usage : ./<script> <example_filename>

function kill_wise_saying_server() {
	rb_pids=$(ps -o pid,command|grep -E "^\d+ .*ruby ./wise_saying_.+\.rb$"|awk '{print $1}')
	py_pids=$(ps -o pid,command|grep -E "^\d+ .*python ./wise_saying_.+\.py$"|awk '{print $1}')

	for pid in ${rb_pids[@]}; do
		kill $pid
		wait $pid 2> /dev/null
	done
	for pid in ${py_pids[@]}; do
		kill $pid
		wait $pid 2> /dev/null
	done
}

SERVER_FILE=$1
QUERY_STRING=$2
TMP_FILE=./log_$SERVER_FILE.log

kill_wise_saying_server
rm -rf $TMP_FILE

./$SERVER_FILE 2> /dev/null &
PID=$!

# wait until server run
for i in {1..60}; do
	output=$(curl http://127.0.0.1:4567/ -s)
	if [[ $output == "" ]]; then
		#echo "server is launching... $i"
		sleep 1
	else
		break
	fi
done

for i in {1..10}; do
	curl http://127.0.0.1:4567/?$QUERY_STRING -s >> $TMP_FILE
	echo "" >> $TMP_FILE
done
uniq_count=$(cat $TMP_FILE | sort | uniq | wc -l | xargs)
echo "query_string : $QUERY_STRING => $uniq_count"

kill $PID
wait $PID 2> /dev/null

kill_wise_saying_server
