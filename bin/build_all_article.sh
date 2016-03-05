#!/bin/bash

# Usage (at root path)
# ./bin/build_all_articles.sh <mode>

mode=$1

for filepath in $(find ./content -name "*.mkdn"); do
	category=$(echo $filepath | awk -F "/" '{print $3}')
	dirname=$(echo $filepath | awk -F "/" '{print $4}')
	filename=$(echo $filepath | awk -F "/" '{print $5}')
	echo "$category : $dirname : $filename"

	cd ./content/$category/$dirname
	maya -mode=$mode -file=$filename > ../$dirname.md
    cd -
done
