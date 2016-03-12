#!/bin/bash

# Usage
# find . -name "*.md" | xargs -I{} ../bin/convert_pelican_md_to_maya.sh {}

# single value
sed -i -r -- 's/^Title: (.+)$/title: \1/Ig' $1
sed -i -r -- 's/^Subtitle: (.+)$/subtitle: \1/Ig' $1
sed -i -r -- 's/^Slug: (.+)$/slug: \1/Ig' $1
sed -i -r -- 's/^Author: (.+)$/author: \1/Ig' $1
sed -i -r -- 's/^Date: (.+)$/date: \1/Ig' $1
sed -i -r -- 's/^Status: (.+)$/status: \1/Ig' $1
sed -i -r -- 's/^Summary: (.+)$/summary: \1/Ig' $1

# multiple value
sed -i -r -- 's/^Tags: (.+)$/tags: [\1]/Ig' $1

# metadata head
ed -s $1 <<< $'1i\n+++\n.\nwq'

# metadata tail
meta_end_line=$(grep -n "^[a-zA-Z]*: [a-zA-Z0-9-][a-zA-Z0-9-]*$" $1|tail -n 1|awk -F ":" '{print $1}')

end_line=$((meta_end_line+1))
ed -s $1 <<< $end_line$'i\n+++\n.\nwq'
