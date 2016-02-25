#!/bin/bash

python ./key_not_exist.py
ruby ./key_not_exist.rb
ruby ./key_not_exist_alt.rb

make key_not_exist_bracket
./key_not_exist_bracket

make key_not_exist_find
./key_not_exist_find

make key_not_exist_count
./key_not_exist_count

make clean
