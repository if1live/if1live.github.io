#!/bin/bash

echo "# test flask"
./test_server.sh wise_saying_flask.py

echo "# test sinatra"
./test_server.sh wise_saying_sinatra.rb

echo "# key not exist - python"
./key_not_exist.py

echo "# key not exist - ruby"
./key_not_exist.rb

echo "# null to int - python"
./null_to_int.py

echo "# null to int - ruby"
./null_to_int.rb
