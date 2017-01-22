#!/bin/bash

# Usage (at article path)
# ../../../bin/build_article.sh

curr_dir=${PWD##*/}
filename=$(find . -name "*.mkdn" | head)
maya -mode=hugo -file=$filename > ../$curr_dir.md

dirname=$(pwd | rev | cut -d '/' -f 1 | rev)
category=$(pwd | rev | cut -d '/' -f 2 | rev)
md_filename=$(ls ..|grep "$dirname.md")
md_filepath="../$md_filename"
hugo_path="../../../hugo"

echo "category : $category"
echo "dirname : $dirname"
echo "md filepath : $md_filepath"

function sanitize_hugo_markdown() {
	sed -i -e  's/{attach}/\/posts\//g' $md_filepath
	sed -i -e  's/{filename}/\/posts\//g' $md_filepath
	sed -i -e  's/{article}/\/posts\//g' $md_filepath
	sed -i -e  's/\.md//g' $md_filepath
}

function copy_hugo_markdown() {
	mkdir -p $hugo_path/content/$category/
	cp $md_filepath $hugo_path/content/$category
}

function copy_hugo_article_static() {
	mkdir -p $hugo_path/static/posts/$dirname
	cp * $hugo_path/static/posts/$dirname
}

sanitize_hugo_markdown
copy_hugo_markdown
copy_hugo_article_static
