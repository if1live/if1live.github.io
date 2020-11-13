#!/usr/bin/env bash

INPUT_SASS="hugo/themes/sora/static/css/style.scss"
OUTPUT_CSS="hugo/themes/sora/static/css/style-gen.css"

node-sass $INPUT_SASS $OUTPUT_CSS