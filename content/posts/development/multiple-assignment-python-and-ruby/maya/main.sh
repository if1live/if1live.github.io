#!/bin/bash

echo "# python 2.x"
pyenv local 2.7.11
python python.py

echo "# python 3.x"
pyenv local 3.5.1
python python.py

echo "# ruby"
tmp_ruby_src=tmp.rb
./codegen.rb > $tmp_ruby_src
ruby $tmp_ruby_src
rm $tmp_ruby_src
