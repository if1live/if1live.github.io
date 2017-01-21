#!/bin/bash

category=$(echo $1 | awk -F "/" '{print $3}')
filename=$(echo $1 | awk -F "/" '{print $4}')

name=$(echo $filename | awk -F "." '{print $1}')
ext=$(echo $filename | awk -F "." '{print $2}')

mkdir -p ./content/$category/$name
mv $1 ./content/$category/$name/article.mkdn
