#!/bin/bash

# Usage (at article path)
# ../../../bin/build_article.sh

curr_dir=${PWD##*/}
filename=$(find . -name "*.mkdn" | head)
maya -mode=hugo -file=$filename > ../$curr_dir.md
