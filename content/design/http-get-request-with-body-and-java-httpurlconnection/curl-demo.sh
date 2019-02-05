#!/bin/bash

function request_common {
    echo -e "$1 => \c"
    curl -X $1 http://127.0.0.1:3100/ \
        -H 'Content-Type: application/json' \
        -d '{"foo": 1}' -s
    echo ""
}

request_common "GET"
# request_common "HEAD"
request_common "POST"
request_common "PUT"
request_common "DELETE"
# request_common "CONNECT"
request_common "OPTIONS"
request_common "TRACE"
request_common "PATCH"

