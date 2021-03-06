# libsora.so

[![Build Status](https://travis-ci.org/if1live/libsora.so.png?branch=master)](https://travis-ci.org/if1live/libsora.so)

[libsora.so](https://libsora.so) blog content

## dependency

* [hugo](https://gohugo.io/getting-started/installing/)
* sass
    - windows: [node-sass](https://www.npmjs.com/package/node-sass)
    - linux: [wellington](https://github.com/wellington/wellington)

## build

``` sh
# build template
bash ./bin/build_theme_win.sh

# build document
cd cli && go build && cd ..
./cli/libsora.so

# serve hugo
cd hugo
hugo serve 
```

## note

### Create https certificate

``` bash
mkdir -p output
cd output
openssl genrsa 1024 > key.pem
openssl req -x509 -new -key key.pem > cert.pem
cd ..
```

* http://blog.saltfactory.net/node/implements-nodejs-based-https-server.html
