#!/bin/bash
cd output
rm -rf *

cp -r ../$1/output/* .

git add -f .
git commit -a -m "add new site content"
git push https://${GH_TOKEN}@github.com/if1live/if1live.github.io.git master --quiet
