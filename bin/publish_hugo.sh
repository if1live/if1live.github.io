#!/bin/bash

function sanitize_hugo_markdown() {
	find ./content -name "*.md" | xargs -I{} gsed -i -e  's/{attach}/\/posts\//g' {}
	find ./content -name "*.md" | xargs -I{} gsed -i -e  's/{filename}/\/posts\//g' {}
	find ./content -name "*.md" | xargs -I{} gsed -i -e  's/\.md//g' {}
}

function copy_hugo_markdown() {
	for name in $(ls ./content/); do
		rm -rf ./hugo/content/$name
	done
	cp -r ./content hugo
}

function copy_hugo_simple_static() {
	mkdir -p ./hugo/static/
	cp -r ./content/extra/* ./hugo/static
}

function copy_hugo_article_static() {
	mkdir -p ./hugo/static/posts
	rm -rf ./hugo/static/posts/*
	for category in $(ls content); do
		for dirname in $(ls -d content/$category/*/ 2> /dev/null); do
			cp -r ${dirname%/} ./hugo/static/posts
		done
	done
}

./bin/build_all_article.sh hugo > /dev/null

sanitize_hugo_markdown
copy_hugo_markdown
copy_hugo_simple_static
copy_hugo_article_static

cd hugo
status=$?
cd -
exit $status
