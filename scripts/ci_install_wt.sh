#!/bin/bash

set -ex

wget -O /tmp/wt.tar.gz https://github.com/wellington/wellington/releases/download/v1.0.4/wt_v1.0.4_linux_amd64.tar.gz
sudo tar xvfz /tmp/wt.tar.gz -C /usr/local/bin
