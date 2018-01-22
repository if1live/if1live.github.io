#!/bin/bash

go run main.go
bash ./bin/copy_static.sh

cd hugo
hugo -t sora --destination=output
status=$?
cd -
exit $status
