# if1live.github.io

[![github pages](https://github.com/if1live/if1live.github.io/actions/workflows/blog.yml/badge.svg?branch=main)](https://github.com/if1live/if1live.github.io/actions/workflows/blog.yml)

[if1live.github.io](https://if1live.github.io) blog content

## dependency

* [hugo](https://gohugo.io/getting-started/installing/)
* [maya](https://github.com/if1live/maya): markdown preprocessor
    * `go install github.com/if1live/maya/maya-cli@latest`

## write maya based markdown

```bash
# build single maya markdown
cd content/posts/sample/maya-test/maya

# by bash
maya-cli -mode=hugo -file=document.mkdn -output=../index.md
# by powrshell
maya-cli -mode=hugo -file="document.mkdn" -output="../index.md"
```

or write plain markdown.

## build

```sh
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
