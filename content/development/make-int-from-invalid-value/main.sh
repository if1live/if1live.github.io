#!/bin/bash

python ./str_to_int_success.py
python ./str_to_int_fail.py

ruby ./str_to_int_success.rb
ruby ./str_to_int_fail.rb
ruby ./str_to_int_fail_alt.rb

python ./null_to_int.py

ruby ./null_to_int.rb
ruby ./null_to_int_alt.rb
