#!/bin/bash

# Usage (at root path)
# ./bin/build_all_articles.sh <mode>

mode=$1

pids=()
for filepath in $(find ./content -name "*.mkdn"); do
	category=$(echo $filepath | awk -F "/" '{print $3}')
	dirname=$(echo $filepath | awk -F "/" '{print $4}')
	filename=$(echo $filepath | awk -F "/" '{print $5}')
	echo "$category : $dirname : $filename"

	cd ./content/$category/$dirname
	maya -mode=$mode -file=$filename > ../$dirname.md &
	pids+=($!)
	cd -
done
for pid in ${pids[@]}; do
	wait $pid
done

# post-process - for hugo
for target in $(find ./content -name "*.md"); do
	line=$(grep -n "^+++$" $target|sed -n 2p|awk -F ":" '{print $1}')

	has_cjk_lang=$(grep "isCJKLanguage = true" $target; echo $?)
	if [[ $has_cjk_lang == "1" ]]; then
		ed -s $target <<< $line$'i\nisCJKLanguage = true\n.\nwq'
	fi

	has_url=$(grep "url =" $target; echo $?)
	if [[ $has_url == "1" ]]; then
		slug=$(grep "slug = " $target|head -n 1|awk -F "\"" '{print $2}')
		ed -s $target <<< $line$'i\nurl = "/posts/'$slug$'/"\n.\nwq'
	fi
done
