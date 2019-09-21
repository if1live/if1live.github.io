#!/bin/bash

set -ex

wget -O /tmp/hugo.deb https://github.com/gohugoio/hugo/releases/download/v0.58.3/hugo_0.58.3_Linux-64bit.deb
sudo dpkg -i /tmp/hugo.deb
