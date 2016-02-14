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
cname = open(cname_filepath).readline()

#SITEURL = ''
SITEURL = "https://" + cname
RELATIVE_URLS = False

FEED_ALL_ATOM = 'feeds/all.atom.xml'

#DELETE_OUTPUT_DIRECTORY = True
DELETE_OUTPUT_DIRECTORY = False

# Following items are often useful when publishing

#DISQUS_SITENAME = ""
#GOOGLE_ANALYTICS = ""
