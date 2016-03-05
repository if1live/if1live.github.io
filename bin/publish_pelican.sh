#!/bin/bash
./bin/build_all_article.sh pelican > /dev/null
cd pelican

rm -f content
ln -s ../content content

make clean_theme
make publish
status=$?

cd -

exit $status
