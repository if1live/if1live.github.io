---
title: Controller for Raspberry Pi + MPD
tags: [rpi, mpd, raspberry pi]
slug: rpi-mpd-controller
author: if1live
summary: Controller for Raspberry Pi + MPD
date: 2013-07-20
url: /posts/rpi-mpd-controller
---
눈수술하면 그동안 아무것도 못하고 심심하잖아? 음악이라도 있어야될거같은데 눈못뜨면 mp3플레이어고 나발이고 제대로 쓰기 어렵잖아? 눈감고도 쓸수있도록 뭐 하나 만들어야될거같은데...해서 만든 프로젝트이다.

* [Project Repo](https://github.com/if1live/rpi-mpd-controller)

Preview
-------

![controller for raspberry pi + mpd](image.jpg)

{{< youtube TRn8TviiMe0 >}}

소감
----

* 최근에 만들었던것 중에서 가장 쓸만했다. 이거라도 있으니까 덜 심심하더라
* GPIO는 간단해서 잠깐 보고 만들 수 있었다. PC에 GPIO가 있으면 재밌는걸 할수있겟지만 현실의 PC는 크고 아름다운 스펙의 USB뿐...
* `event_detected()` 함수가 있어서 RISING, FALLING 같은것이 발생했을때 callback function 호출이 가능하다.
문제는 한번 눌렀다고 callback이 꼭 한번 호출되는게 아니더라(-_-...) 나는 디버깅하기 귀찮아서 polling으로 버튼을 구현했다. 시간나면 한번 코드 확인해봐야겠다.
