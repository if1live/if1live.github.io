# libsora.so

[![Build Status](https://travis-ci.org/if1live/libsora.so.png?branch=master)](https://travis-ci.org/if1live/libsora.so)
[![Stories in Ready](https://badge.waffle.io/if1live/libsora.so.png?label=ready)](https://waffle.io/if1live/libsora.so)
[![Requirements Status](https://requires.io/github/if1live/libsora.so/requirements.png?branch=master)](https://requires.io/github/if1live/libsora.so/requirements/?branch=master)

[libsora.so](https://libsora.so) blog content

## Local development

### Install - hugo

```
go get -u github.com/if1live/maya
go get -u -v github.com/spf13/hugo
go get github.com/wellington/wellington/wt
```

### Install - pelican

``` bash
virtualenv .venv
. ./.venv/bin/activate
pip install -r requirements.txt

npm install -g http-server lessc
```

### Create https certificate
``` bash
mkdir -p output
cd output
openssl genrsa 1024 > key.pem
openssl req -x509 -new -key key.pem > cert.pem
cd ..
```

* http://blog.saltfactory.net/node/implements-nodejs-based-https-server.html

### Run Local Dev Server - pelican

``` bash
# shell 1
cd output
http-server -S

# shell 2
cd hugo
hugo server --theme=sora
or
hugo server --theme=sora --bind 0.0.0.0 --baseURL=192.168.200.106:1313
```

### Run Local Dev Server - pelican
``` bash
# shell 1
cd output
http-server -S

# shell 2
cd pelican
make devserver
```
