#!/usr/bin/env python
# -*- coding: utf-8 -*- #
from __future__ import unicode_literals
import markdown

AUTHOR = u'if1live'
SITENAME = u'libsora.so'
#SITEURL = ''
SITEURL = 'http://libsora.so'

TIMEZONE = u'Asia/Seoul'

DEFAULT_LANG = u'ko'

DEFAULT_DATE_FORMAT = '%Y/%m/%d'
TWITTER_USERNAME = 'if1live'

DISQUS_SITENAME = "libsora"
GOOGLE_ANALYTICS = "UA-37862172-2"

# Feed generation is usually not desired when developing
FEED_ALL_ATOM = None
CATEGORY_FEED_ATOM = None
TRANSLATION_FEED_ATOM = None

WEBASSETS = True
LESS_GENERATOR = True
LESS_RUN_IN_DEBUG = True

ARTICLE_URL = 'posts/{slug}/'
ARTICLE_SAVE_AS = 'posts/{slug}/index.html'

# Blogroll
LINKS = (('shipduck', 'http://github.com/shipduck'),)
SOCIAL = (('Twitter', 'http://twitter.com/if1live'),
		('GitHub', 'http://github.com/if1live'),)

DEFAULT_PAGINATION = 10

# Uncomment following line if you want document-relative URLs when developing
RELATIVE_URLS = True

PLUGIN_PATH = 'ext/pelican-plugins'

		
THEME = 'ext/pelican-sora'

# attributes for pelican-sora
PLUGINS = [
	'tipue_search',
	'sitemap',
	'related_posts',
    'assets',
    ]
	
DIRECT_TEMPLATES = (('index', 'tags', 'categories','archives', 'search', '404'))
MD_EXTENSIONS = ['codehilite(css_class=highlight)', 'extra', 'headerid']
STATIC_PATHS = ['theme/images', 'images', 'static']

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

# optional attributes for pelican-sora
SITE_DESCRIPTION = u'libsora.so'
SITESUBTITLE = ''
SITE_LICENSE = ''
RECENT_ARTICLES_COUNT = 10
COMMENTS_INTRO = u"So what do you think? Did I miss something? Is any part unclear? Leave your comments below."
