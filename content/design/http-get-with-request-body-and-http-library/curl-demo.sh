#!/bin/bash

curl -X POST http://127.0.0.1:3100/ -H 'Content-Type: application/json' -d '{"foo": 1}' -s
echo ""

curl -X GET http://127.0.0.1:3100/ -H 'Content-Type: application/json' -d '{"foo": 1}' -s

