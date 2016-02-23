#!/bin/bash

echo "# test flask"
./test_server.sh wise_saying_flask.py
echo "# test flask : idx=1"
./test_server.sh wise_saying_flask.py idx=1
echo "# test flask : idx=asdf"
./test_server.sh wise_saying_flask.py idx=asdf

echo "# test sinatra"
./test_server.sh wise_saying_sinatra.rb
echo "# test sinatra : idx=1"
./test_server.sh wise_saying_sinatra.rb idx=1
echo "# test sinatra : idx=asdf"
./test_server.sh wise_saying_sinatra.rb idx=asdf

echo "# test sinatra => alternative"
./test_server.sh wise_saying_sinatra_alt.rb
echo "# test sinatra => alternative : idx=1"
./test_server.sh wise_saying_sinatra_alt.rb idx=1
echo "# test sinatra => alternative : idx=asdf"
./test_server.sh wise_saying_sinatra_alt.rb idx=asdf


echo "# key not exist - python"
./key_not_exist.py

echo "# key not exist - ruby"
./key_not_exist.rb

echo "# key not exist - ruby => alternative"
./key_not_exist_alt.rb

echo "# null to int - python"
./null_to_int.py

echo "# null to int - ruby"
./null_to_int.rb
