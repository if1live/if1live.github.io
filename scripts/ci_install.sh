#!/bin/bash

set -ex

# wt
wget -O /tmp/wt.tar.gz https://github.com/wellington/wellington/releases/download/v1.0.5/wt_v1.0.5_linux_amd64.tar.gz
sudo tar xvfz /tmp/wt.tar.gz -C /usr/local/bin
