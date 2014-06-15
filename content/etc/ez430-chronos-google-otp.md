Title: eZ430-Chronos Google OTP 삽질기
Tags: otp, ez430, chronos
Slug: ez430-chronos-google-otp
Author: if1live
Date: 2013-05-25

몇달전에 손목시계가 필요했다 + 개발가능이면 더 좋겠지 해서 [eZ430-Chronos][wiki_ez430_chronos]를 충동적으로 샀다.
막상 질러놓고 이걸 어따 쓸까 하다가 마땅한 방법이 생각이 안나서 방치해놧다가 [Google OTP][base_ez430_otp]로 쓰는 걸 보고 나도 저거로 써야지해서 삽질했다.
삽질 마무리할 겸 주요 삽질 내용을 정리해봤다.
(물론 여기에 나온게 내가 한 모드 삽질내용은 아니다. 짜잘한 삽질은 까먹어서... 윈7/8에 드라이버 설치하는 방법이라든가... 그정도는 근성으로 해결할수 있을거라고 믿고 건너뛴다)

## 펌웨어 업글 됩니까?

[eZ430-Chronos][wiki_ez430_chronos]는 Wireless Firmware Update는 지원한다.
펌웨어 업그레이드 하겠다고 시계 뚜껑따는 삽질을 안해도 되니까 정말 좋은 기능이다.
하지만 한가지 함정이 있으니, 최근 생산품의 경우는 무선 펌웨어 업그레이드가 안된다. -_-
내가 병신인지 시계가 고장인지 찾아봤지만 [똑같은 주제로 논의][wireless_update_not_work]가 된걸 보고서 포기했다.
자신이 EZ430-Chronos을 샀을때 CD가 들어있었으면 아마도 별 문제없이 될거다.
CD가 없던 사람들은 펌웨어 업그레이드를 한번 시도해보고 안되면 무선 펌웨어 업글은 포기하자.
커뮤니티에서 문제된 제품, 내거 모두 2013/01 생산품인거 보니 올해 산 사람은 포기하면 된다.

하지만 무선업뎃이 안된다고 포기하면 근성의 공돌이가 아니다. 뚜껑따고 직접 연결하면 되지 않겠는가?
그렇지만 이것 역시 그렇게 쉽지 않다.
USB연결하고 [펌웨어 업그레이드 툴][firmware_tool]을 사용해서 펌웨어 올리는것을 시도했으나 되지 않았다.
내 경우 펌웨어 올리겠다고 집에 있는 컴퓨터 4대로 시도해서 한대에서만 펌웨어 올리는 것을 성공했다.
아마도 드라이버 관련 문제가 아닐까 싶은데......
어쨋든 자신의 PC에서 펌웨어 올리는것이 불가능하다면 포기하자.
펌웨어 올리는게 가능하면 마저 삽질을 할 수 있다.

참고로 펌웨어 업글하다가 말아먹어서 원상복구를 하고싶은 경우 [Control Center][contorl_center] 를 설치하자.
복구용 원본 펌웨어도 같이 깔린다.

기본 설치를 했으면 경로는 ```C:\Program Files (x86)\Texas Instruments\eZ430-Chronos\Recovery\Chronos Watch\```

* 펌웨어 파일 : ```Applications/Recovery_ez430_chronos_433MHz_2_0.txt``` 자신의 주파수에 맞게 적절히 쓰자
* 무선 업글 관련 : ```Wireless Updater/Recovery_eZ430_Chronos_rfbsl_433MHz_1_0.txt``` 역시 자신의 주파수에 맞게쓰자. 유선 펌웨어 업그레이드 할 때만 필요하다.

유선 펌웨어 업글이 되는것을 확인했으면 시계를 벽돌로 만들어도 원상복구에 전혀 문제없으니 마음놓고 삽질하자!
(내가 벽돌 만들어봐서 아는데 복구에 전혀 문제없음)


## 이제는 무선 펌웨어 업그레이드가 된다!

이 글을 쓰던 2013년 5월에는 알려진 무선 펌웨어 업그레이드 방법이 없었다. 하지만 연말이 되니까 새로운 방법이 나왔더라.
[업그레이드 방법을 못 찾았던 포럼에도 방법이 올라왔고](http://e2e.ti.com/support/microcontrollers/msp430/f/166/t/255460.aspx?pi283121=3)
우리나라에도 [todaysppc를 통해 eZ430 무선 펌웨어 업그레이드 방법](http://www.todaysppc.com/mbzine/bbs/view.php?id=free&page=1&sn1=&divpage=51&sn=off&ss=on&sc=on&keyword=ez430&select_arrange=headnum&desc=asc&no=264464)이 알려졌다. 하지만 나는 지금 쓰는 펌웨어에 만족하고 테스트하기도 귀찮은 관계로 해보지는 않았다.


## 적절한 GCC 버전을 찾아서

내 경우는 윈도우에서 컴파일하기로 했다. 모든 EZ430-Chronos 펌웨어의 기반이 된 [OpenChronos][open_chronos]의 경우는 README에 나온대로 msp430-GCC 대충 받아서 컴파일하면 컴파일이 잘 된다.
하지만 [Google OTP 지원하겠다고 수정한 코드][otp_code]는 컴파일이 되지 않더라.
구형 GCC를 쓰면 ```legacymsp430.h```가 없다고 컴파일을 못하질 않나...
최신 GCC를 쓰면 ```READ_SR```을 못찾겠다고 컴파일을 못하지 않나....
이 문제는 리눅스에서 컴파일해도 똑같이 발생한다. 역시 더러운 임베디드.... 곱게 컴파일될리가 없지.

[msp430-gcc for windows][win_msp430_gcc]에서 버전별로 시도해봐서 **mspgcc-20110716-p20120311.zip**에서 성공했다.
해당 버전의 GCC는 **4.5.3**이다. 리눅스에서도 msp430-gcc 4.5.3을 구해서 컴파일을 하면 아마도 특별한 문제가 없을거라고 예상한다.

추가로, 윈도우에서 컴파일하면 ```config.h```를 못찾겠다고 할수 있는데 makefile의 ```CC_INCLUDE```에다가 ```-I$(PROJ_DIR)```를 추가해주자. 리눅스랑 윈도우랑 path정책이 달라서 그런듯하다.


## config.h은 어떻게 만드는가?
펌웨어의 기능 활성화/비활성화에는 ```config.h```가 사용된다. 펌웨어 소스 안의 ```tools/config.py```를 실행하면 config.h 파일을 적절히 생성해줍니다 라고 하지만 문제는 이게 윈도우에서는 실행이 불가능하다.
파이썬의 [termios][termios]를 기반으로 구현되어있는데 이놈이 Unix계열에만 존재하는 라이브러리이다. vm을 끌고와서 리눅스 깔아놓고 작업하는게 속편하다. 다른 기능은 손수 config.h를 짜서 처리할수 있지만 Google OTP의 경우는 그게 안되겟더라.


## OTP를 쓰자!
Google OTP는 해당 스펙이 공개되어있어서 JavaScript로 돌아가는 [예제][otp_js]까지 존재한다.
덕분에 시계를 Google OTP로 만든다는 짓이 가능하다. 은행OTP도 알고리즘이 공개되어있으면 똑같이 구현 할 수 있겠지만 우리나라 보안 환경에서 그런게 공개되어있을리는 없으니까 이건 포기하자. (개인적으로는 이게 더 좋은데...)
어쨋든 Google OTP를 세팅하려면 Secret key가 필요하다. Secret key는 구글의 2차 로그인 설정에서 찾을 수 있다.

![step1]({filename}../static/ez430-chronos-google-otp/step1.png)

2차 로그인 설정에서 블랙베리를 누른다.

![step2]({filename}../static/ez430-chronos-google-otp/step2.png)

secret key가 뜬다. 적절히 저장해놓자. 사실 위에서 아이폰/안드로이드 눌러도 별 문제 없지만 블랙베리는 바코드가 없어서 바로 키가 보인다. -_-

secret key를 ```config.py``` 실행중에 적절한 영역에 입력해준다. 물론 OTP도 알아서 활성화 하기. 그리고 UTC는 9(대한민국 기준)를 설정해준다.

그리고 컴파일을 돌리면 다음과 같은 느낌의 에러가 날거다.
```
msp430/bin/ld: build/eZChronos.elf section `.text' will not fit in region `text'
 .../msp430/bin/ld: section .vectors loaded at [000000000000ff80,000000000000ffff] overlaps section .text loaded at [0000000000008000,0000000000013343]
 .../msp430/bin/ld: region `text' overflowed by 13250 bytes
 collect2: ld returned 1 exit status
```
Google OTP 관련 로직이 너무 크고 아름다워서 펌웨어가 시계에 안올라갈듯해서 뜨는 에러다.
알아서 적절히 옵션을 끄면서 테스트해보면 될거다.

내 경우 펌웨어를 올린 다음에 RF를 통한 시간 동기화를 하면 몇초뒤에 시계가 재부팅하는 문제가 있더라.
어디가 영향받았나 코드를 읽기 귀찮아서 그냥 수동으로 시간 설정했다. 그거외에는 특별한 문제는 없는듯

## Reference
* [eZ430-Chronos][wiki_ez430_chronos]
* [EZ430-Chronos OTP][base_ez430_otp]
* [Firmware update tool for eZ430-Chronos watches][firmware_tool]
* [Wireless Update Not Working][wireless_update_not_work]
* [GCC toolchain for MSP430][win_msp430_gcc]
* [Google Authenticator One-time Password Algorithm in Javascript][otp_js]


[wiki_ez430_chronos]: http://processors.wiki.ti.com/index.php/EZ430-Chronos?DCMP=Chronos&HQS=Other+OT+chronoswiki
[base_ez430_otp]: http://tinyhack.com/2011/03/02/ez430-chronos-otp/
[open_chronos]: https://github.com/poelzi/OpenChronos
[otp_code]: https://github.com/qwandor-google/OpenChronos
[firmware_tool]: http://www.bm-innovations.com/index.php/ez430-chronos
[wireless_update_not_work]: http://e2e.ti.com/support/microcontrollers/msp430/f/166/t/255460.aspx?pi77810=2
[win_msp430_gcc]: http://sourceforge.net/projects/mspgcc/files/Windows/mingw32/
[contorl_center]: http://processors.wiki.ti.com/index.php/EZ430-Chronos?DCMP=Chronos&HQS=Other+OT+chronoswiki#Control_Center_for_Windows_.28Install_this_first.29
[termios]: http://docs.python.org/2/library/termios.html
[otp_js]: http://blog.tinisles.com/2011/10/google-authenticator-one-time-password-algorithm-in-javascript/
