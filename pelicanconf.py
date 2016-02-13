#!/usr/bin/env python
# -*- coding: utf-8 -*- #

from __future__ import unicode_literals
import os

AUTHOR = u'if1live'
SITENAME = u'libsora.so'
SITEURL = ''
#SITEURL = 'http://libsora.so'

TIMEZONE = u'Asia/Seoul'

DEFAULT_LANG = u'ko'

DEFAULT_DATE_FORMAT = '%Y/%m/%d'

ARTICLE_URL = 'posts/{slug}/'
ARTICLE_SAVE_AS = 'posts/{slug}/index.html'
u"""
_SAVE_AS=False 설정시 travis-ci 빌드에서
CRITICAL: 'bool' object has no attribute 'startswith'
문제가 발생하며 죽는다
"""
#CATEGORY_SAVE_AS = False
#TAG_SAVE_AS = False
#AUTHOR_SAVE_AS = False

TWITTER_USERNAME = 'if1live'

DISQUS_SITENAME = "libsora"
GOOGLE_ANALYTICS = "UA-37862172-2"
DNSEVER_BANNER = True

# Feed generation is usually not desired when developing
FEED_ALL_ATOM = None
CATEGORY_FEED_ATOM = None
TRANSLATION_FEED_ATOM = None
AUTHOR_FEED_ATOM = None
AUTHOR_FEED_RSS = None

WEBASSETS = True
LESS_RUN_IN_DEBUG = True

# Blogroll
LINKS =  (
#    ('shipduck', '//github.com/shipduck'),
)

# Social widget
SOCIAL = (
    ('Twitter', '//twitter.com/if1live'),
    ('GitHub', '//github.com/if1live'),
    ('BitBucket', '//bitbucket.org/if1live'),
)

DEFAULT_PAGINATION = False


# Uncomment following line if you want document-relative URLs when developing
RELATIVE_URLS = True

# for pelican 3.4.0
PLUGIN_PATHS = ['ext/pelican-plugins']


THEME = 'theme-sora'

PLUGINS = [
    'sitemap',
    'assets',
    'code_include.code_include',
    'html_rst_directive',
    'pelican_youtube',
    'pelican_jsfiddle',
    'better_figures_and_images',
    'code_include',
    # enable w3c validate if py_w3c is fixed
    #'w3c_validate',
]

DIRECT_TEMPLATES = ('index', 'tags', 'archives', '404', 'search')
MD_EXTENSIONS = ['codehilite(css_class=highlight)', 'extra', 'headerid']

STATIC_PATHS = ['static', 'extra']
PAGE_DIRS = ('pages',)
ARTICLE_EXCLUDES = ('pages',)
IGNORE_FILES = ['.#*', '*.html']

# 뻐킹 윈도 때문에 extra/CNAME하면 망한다
EXTRA_PATH_METADATA = {
    os.sep.join(['extra', 'CNAME']): {'path': 'CNAME'},
    os.sep.join(['extra', 'favicon.ico']): {'path': 'favicon.ico'},
    os.sep.join(['extra', 'robots.txt']): {'path': 'robots.txt'},
}

SITEMAP_SAVE_AS = 'sitemap.xml'

SITEMAP = {
    'format': 'xml',
    'priorities': {
        'articles': 0.5,
        'indexes': 0.5,
        'pages': 0.5
    },
    'changefreqs': {
        'articles': 'monthly',
        'indexes': 'daily',
        'pages': 'monthly'
    }
}

RESPONSIVE_IMAGES = True

# optional attributes for pelican-sora
SITE_DESCRIPTION = u'libsora.so'
SITESUBTITLE = ''
SITE_LICENSE = ''
RECENT_ARTICLES_COUNT = 10

#=============
# Twitter Card
#=============
# https://dev.twitter.com/cards
# http://michaelmartinez.in/twitter-card-plugin-for-pelican.html
TWITTER_CARD_USE = (True) # (False)
TWITTER_CARD_SITE = '@if1live'  # The site's Twitter handle like @my_blog
TWITTER_CARD_SITE_ID = ''  # The site's Twitter ID
TWITTER_CARD_CREATOR = ''  # Your twitter handle like @monkmartinez
TWITTER_CARD_CREATOR_ID = ''  # The site creator's id
GRAVATAR_URL = 'https://www.gravatar.com/avatar/fb9672e7e0d256f39369595381d1ea07'

TEMPLATE_PAGES = {
    'about.html': 'about.html',
    'project.html': 'project.html'
}
