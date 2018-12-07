# libsora.so

[![Build Status](https://travis-ci.org/if1live/libsora.so.png?branch=master)](https://travis-ci.org/if1live/libsora.so)

[libsora.so](https://libsora.so) blog content

## Install - windows

* [node-sass](https://www.npmjs.com/package/node-sass)
* [hugo](https://gohugo.io/getting-started/installing/)
* dep: `go get github.com/golang/dep/cmd/dep`

### clone project + GOPATH

example
* GOPATH: `<root>/blog`
* libsora.so path: `<root>/blog/src/libsora.so`

```
export GOPATH=<blog root>
mkdir -p blog/src
cd blog/src
git clone https://github.com/if1live/libsora.so.git
cd libsora.so

dep ensure
```

## build

```
# build template
bash ./bin/build_theme_win.sh

# build document
# set GOPATH before execute command
go run main.go

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
