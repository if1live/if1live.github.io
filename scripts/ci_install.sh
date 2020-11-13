#!/bin/bash

set -ex

# wt
wget -O /tmp/wt.tar.gz https://github.com/wellington/wellington/releases/download/v1.0.4/wt_v1.0.4_linux_amd64.tar.gz
sudo tar xvfz /tmp/wt.tar.gz -C /usr/local/bin

# hugo
wget -O /tmp/hugo.tar.gz https://github.com/gohugoio/hugo/releases/download/v0.58.3/hugo_0.58.3_Linux-64bit.tar.gz
sudo tar xvfz /tmp/hugo.tar.gz -C /usr/local/bin
