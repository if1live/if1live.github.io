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

ARTICLE_URL = 'posts/{slug}/'
ARTICLE_SAVE_AS = 'posts/{slug}/index.html'

# Blogroll
LINKS = ()
#LINKS =  (('Pelican', 'http://getpelican.com/'),
#          ('Python.org', 'http://python.org/'),
#          ('Jinja2', 'http://jinja.pocoo.org/'),
#          ('You can modify those links in your config file', '#'),)

# Social widget
SOCIAL = (('twitter', 'http://twitter.com/if1live'),
          ('github', 'http://github.com/if1live'),)

DEFAULT_PAGINATION = 10

# Uncomment following line if you want document-relative URLs when developing
RELATIVE_URLS = True

PLUGIN_PATH = 'ext/pelican-plugins'
PLUGINS = [
    'sitemap',
    'related_posts',
    'assets',
    'extract_toc',
    'tipue_search',
    ]

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

THEME = 'ext/pelican-elegant'

COMMENTS_INTRO = u"So what do you think? Did I miss something? Is any part unclear? Leave your comments below."
MD_EXTENSIONS = ['codehilite(css_class=highlight)', 'extra', 'headerid']
DIRECT_TEMPLATES = (('index', 'tags', 'categories','archives', 'search', '404'))
STATIC_PATHS = ['theme/images', 'images', 'static']
TAG_SAVE_AS = ''
CATEGORY_SAVE_AS = ''
AUTHOR_SAVE_AS = ''
RECENT_ARTICLES_COUNT = 10

ABOUT_DETAILS = u'''
평범한 개발자 [if1live][github]입니다. 또는 [libsora][github]를 사용하기도 합니다.
절 찾고싶은 사람은 트위터 [@if1live](twitter)에서 찾으면됩니다.

왭(Web)질로 먹고살고 있지만 최근 관심분야는 Oculus Rift입니다. 당분간은 이거만 하겠네요. (Developer Kit까지 구입한 호구)

web으로 먹고 살고 관심분야는 Oculus Rift지만 프로젝트는 딱히 분야 가리지 않고 다 손댑니다. 
덕분에 GitHub Repository 목록이 쓸데없이 깁니다.
요즘에 뭐하고 사는지는 [GitHub][github]에서 스토킹 가능합니다.

주력언어는 C++, Python, 한쿡어입니다. 다른 언어는 필요할때는 쓰지만 신규프로젝트는 항상 C++/Python 중에서 선택합니다.
(피할수 없는 JavaScript의 세계...) Graphics는 GL진영 소속입니다. 입문을 gles2.0으로 해서요.

[twitter]: https://twitter.com/if1live
[github]: https://github.com/if1live/
'''

LANDING_PAGE_ABOUT = {'title' : 'libsora.so', 'details' : markdown.markdown(ABOUT_DETAILS)}

GLSL_EXAMPLE_DESC = u'''
[셰이더 프로그래밍 입문](http://www.hanb.co.kr/book/look.html?isbn=978-89-7914-949-4) for GLSL. 
다만 완벽하게 동일한 예제는 아님
'''

EPIC_EDITOR_DESC = u'''
[EpicEditor][epic_editor]를 살짝 수정해서 미리보기에서 Code Syntax Highlight이 지원 추가
[Demo][if1live_epic_editor]

[epic_editor]: http://epiceditor.com/
[if1live_epic_editor]: ../proj/epiceditor/index.html
'''

PROJECTS = [
    {
    'name' : u'참참참 with Oculus Rift + Leap Motion',
    'url' : 'http://shipduck.github.io/cham-cham-cham',
    'description' : u'Oculus Rift + Leap Motion 으로 만든 간단한 기술데모(?)'
    },
    {
    'name' : 'LibOVR',
    'url' : 'https://github.com/if1live/LibOVR',
    'description' : 'UnOfficial LibOVR Repository with CMake',
    },
    {
    'name' : 'irrlicht-console',
    'url' : 'http://libsora.so/irrlicht-console',
    'description' : u'''irrlicht engine에서 quake console 같은거를 쓰자!'''
    },
    {
    'name' : 'Controller for Raspberry Pi + MPD',
    'url' : 'https://github.com/if1live/rpi-mpd-controller',
    'description' : u'''눈 못뜨는 상태에서도 Raspberry Pi로 음악 들르라고 만든거'''
    },
    {
    'name' : 'gles2framework',
    'url' : 'https://github.com/if1live/gles2framework',
    'description' : u'''원본코드 버그 수정+ 코드 약간 수정''',
    },
    {
    'name': 'GLSL Example',
    'url': 'http://libsora.so/glsl_example/',
    'description': markdown.markdown(GLSL_EXAMPLE_DESC)
    },
    {
    'name': 'EpicEditor',
    'url': 'https://github.com/if1live/epiceditor',
    'description': markdown.markdown(EPIC_EDITOR_DESC)
    }
]