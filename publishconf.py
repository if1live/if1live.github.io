#!/usr/bin/env python
# -*- coding: utf-8 -*- #
from __future__ import unicode_literals

# This file is only used if you use `make publish` or
# explicitly specify it as your config file.

import os
import sys
sys.path.append(os.curdir)
from pelicanconf import *

#SITEURL = ''
#https작동 확인후 돌리기
SITEURL = 'http://libsora.so'
RELATIVE_URLS = False

FEED_ALL_ATOM = 'feeds/all.atom.xml'

#DELETE_OUTPUT_DIRECTORY = True
DELETE_OUTPUT_DIRECTORY = False

# Following items are often useful when publishing

#DISQUS_SITENAME = ""
#GOOGLE_ANALYTICS = ""
