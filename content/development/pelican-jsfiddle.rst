pelican-jsfiddle Release
=======================================

:date: 2014-04-07
:tags: pelican, plugin, jsfiddle
:slug: pelican-jsfiddle
:author: if1live
:subtitle: Easily embed JSFiddle in your articles


블로그 엔진으로 pelican을 쓰는건 좋은데 js,css,html 같은걸 문서에 넣는게 어렵더라.
pelican-gist_ 라는 괜찮은 물건이 있지만 gist는 코드를 보여주기만 할뿐, 데모를 돌려보는게 불가능하다.
하지만 jsfiddle_ 을 쓰면 그게 가능하다. 그래서 pelican에 jsfiddle을 붙이는 목적으로 pelican-youtube_ 을 참고해서 pelican-jsfiddle_ 이라는 플러그인 하나를 만들어봤다.

Demo
##################

기본 사용 예시
-------------------

.. code:: rst

    .. jsfiddle:: if1live/V2P28


.. jsfiddle:: if1live/V2P28


속성 응용 사용 예시
---------------------------

.. code:: rst

    .. jsfiddle:: if1live/V2P28
        :width: 100%
        :height: 150
        :tabs: js,result
        :skin: presentation


.. jsfiddle:: if1live/V2P28
    :width: 100%
    :height: 150
    :tabs: js,result
    :skin: presentation



.. _jsfiddle: http://jsfiddle.net/
.. _pelican-gist: https://github.com/streeter/pelican-gist
.. _pelican-jsfiddle: https://github.com/if1live/pelican-jsfiddle
.. _pelican-youtube: https://github.com/kura/pelican_youtube
