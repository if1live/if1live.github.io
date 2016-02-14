#!/usr/bin/env python
# -*- coding: utf-8 -*- #
from __future__ import unicode_literals

# This file is only used if you use `make publish` or
# explicitly specify it as your config file.

import os
import sys
sys.path.append(os.curdir)
from pelicanconf import *

cname_filepath = os.path.join(
    os.path.dirname(__file__),
    "content", "extra", "CNAME"
)
cname = open(cname_filepath).readline().strip()

# siteurl: //libsora.so, siteurl_full: https://libsors.so
# 같은식으로 해서 http/https동시 지원을 가능하게 설계하려고 했으나
# feed는 site_url만 지원하도록 설계되어있다
# https://github.com/getpelican/pelican/blob/72ee73f886d05cea10c39bddec1ba99a3aae9504/pelican/writers.py#L39
# 그래서 siteurl, siteurl_short로 바꿈
SITEURL = "https://" + cname
SITEURL_SHORT = "//" + cname
SITEDOMAIN = cname
RELATIVE_URLS = False

FEED_ALL_ATOM = 'feeds/all.atom.xml'

#DELETE_OUTPUT_DIRECTORY = True
DELETE_OUTPUT_DIRECTORY = False

# Following items are often useful when publishing

#DISQUS_SITENAME = ""
#GOOGLE_ANALYTICS = ""
