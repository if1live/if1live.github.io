#!/bin/bash

jupyter nbconvert *.ipynb --to markdown

echo "---" > article.mkdn
cat metadata.yml >> article.mkdn
echo "---" >> article.mkdn
cat *.md >> article.mkdn
