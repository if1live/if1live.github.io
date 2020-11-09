#!/bin/bash

set -ex

# dep
sudo wget -O /usr/local/bin/dep https://github.com/golang/dep/releases/download/v0.5.4/dep-linux-amd64
sudo chmod 755 /usr/local/bin/dep

# wt
wget -O /tmp/wt.tar.gz https://github.com/wellington/wellington/releases/download/v1.0.4/wt_v1.0.4_linux_amd64.tar.gz
sudo tar xvfz /tmp/wt.tar.gz -C /usr/local/bin

# hugo
wget -O /tmp/hugo.tar.gz https://github.com/gohugoio/hugo/releases/download/v0.58.3/hugo_0.58.3_Linux-64bit.tar.gz
sudo tar xvfz /tmp/hugo.tar.gz -C /usr/local/bin
