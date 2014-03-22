Title: Awesome 3.5 install Guide for Ubuntu 13.10
Tags: awesome
Slug: awesome-3-5-install
Author: if1live
Date: 2014-03-22

## Why not 3.4?
On Ubuntu 13.10, current stable version is 3.4.15-1. [awesome][awesome] 3.4 doesn't support different wallpeper on each screen. awesome 3.5 support it. I want this feature.

![step1](|filename|../static/awesome-3-5-install/multiple-wallpaper.jpg)

## Object
Install awesome 3.5.3(current stable) on ubuntu 13.10

## Try PPA, but...
I'm lazy, so I like package rather than compile. I try using PPA.

* [PPA for Arnaud Guignard](https://launchpad.net/~aguignard/+archive/ppa)
* [PPA for Klaus Vormweg](https://launchpad.net/~klaus-vormweg/+archive/ppa)

After install awesome 3.5, I execute awesome, But crash occur.
I decide to install awesome 3.5 from source.

## Install Dependencies

Install default package.
```
sudo apt-get install cmake gcc imagemagick
```

Install headers.
```
sudo apt-get install libxcb-cursor0-dev libxcb-randr0-dev libxcb-xtest0-dev libxcb-xinerama0-dev libxcb-shape0-dev libxcb-util0-dev libxcb-keysyms1-dev libxcb-icccm4-dev libstartup-notification0-dev libxdg-basedir-dev
```

Install lua.
```
sudo apt-get install liblua5.1-dev lua-lgi
```

Install dbus support.
```
sudo apt-get install libdbus-1-dev
```

Install libxcb-cursor-dev, but libxcb-cursor-dev is not basic package. You have to add a extra repository. see [help](http://i3wm.org/docs/repositories.html#_ubuntu_repository)
```
echo "deb http://debian.sur5r.net/i3/ $(lsb_release -c -s) universe" >> /etc/apt/sources.list
apt-get update
apt-get --allow-unauthenticated install sur5r-keyring
apt-get update
apt-get install libxcb-cursor-dev
```

## Install
Get source then extract.

```
sudo cd /usr/local/src
sudo wget http://awesome.naquadah.org/download/awesome-3.5.2.tar.bz2
sudo tar xvfj awesome-3.5.2.tar.bz2
cd awesome-3.5.2
```

Install
```
sudo cmake .
sudo make install
```

After install, validate it.
```
awesome -v
awesome v3.5.2 (The Fox)
 • Build: Mar 19 2014 23:22:38 for x86_64 by gcc version 4.8.1 (root@sora)
 • Compiled against Lua 5.1.5 (running with Lua 5.1)
 • D-Bus support: ✔
```

If you use ubuntu 13.10, GUI login daemon is lightdm. Add awesome to lightdm session list.

```
sudo cp /usr/local/share/xsessions/awesome.desktop /usr/share/xsessions
```

If configuration file is not exist, awesome 3.5 will crash. Copy configuration file to your home directory. (If you use ```cmake -DCMAKE_PREFIX_PATH=/usr -DSYSCONFDIR=/etc```, this process is not necessary. awesome will find valid config file. see [Awesome-3-git-debian][awesome-3-git-debian])

```
mkdir -p ~/.config/awesome
cp /usr/local/src/awesome-3.5.2/awesomerc.lua ~/.config/awesome/rc.lua
```

Logout. Select awesome from session list. Then login. You will see awesome 3.5.

[awesome]: http://awesome.naquadah.org/
[awesome-3-git-debian]: http://awesome.naquadah.org/wiki/Awesome-3-git-debian
