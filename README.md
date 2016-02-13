# libsora.so

[![Build Status](https://travis-ci.org/if1live/libsora.so.png?branch=master)](https://travis-ci.org/if1live/libsora.so)
[![Stories in Ready](https://badge.waffle.io/if1live/libsora.so.png?label=ready)](https://waffle.io/if1live/libsora.so)
[![Requirements Status](https://requires.io/github/if1live/libsora.so/requirements.png?branch=master)](https://requires.io/github/if1live/libsora.so/requirements/?branch=master)

blog content

## Local development

``` bash
git submodule init
git submodule update

virtualenv .venv
. ./.venv/bin/activate
pip install -r requirements.txt

npm install -g http-server lessc
mkdir -p output
cd output
openssl genrsa 1024 > key.pem
openssl req -x509 -new -key key.pem > cert.pem
cd ..

make devserver
```

* http://blog.saltfactory.net/node/implements-nodejs-based-https-server.html
