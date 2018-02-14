#!/usr/bin/env bash

INPUT_SASS="hugo/themes/sora/static/css/style.scss"
OUTPUT_CSS="hugo/themes/sora/static/css/style-gen.css"

# default sass
if [ -x "$(command -v sass)" ]; then
	echo "sass found"
    sass $INPUT_SASS $OUTPUT_CSS --trace
    exit
fi

# wellington
if [ -x "$(command -v wt)" ]; then
    echo "wellington found"
    wt compile $INPUT_SASS > $OUTPUT_CSS
    exit
fi

if [ -x "$(command -v node-sass)" ]; then
    echo "node-sass found"
    node-sass $INPUT_SASS $OUTPUT_CSS
    exit
fi

echo "not found any sass preprocessor"
exit 1
