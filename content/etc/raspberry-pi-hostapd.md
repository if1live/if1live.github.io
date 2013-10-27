Title: Raspberry Pi를 무선공유기로 쓰기
Tags: raspberry pi, access point, hostapd, raspbian, wireless 
Slug: raspberry-pi-hostapd
Author: if1live
Date: 2013-04-09

내 방은 집의 무선네트워크에서 미묘하게 사각지대에 위치한다. 
그래서 누워서 인터넷하면 잘 끊긴다. 
이를 해결하고자 집에서 굴러다니는 라즈베리 파이와 옛날에 사서 묵혀놓은 USB무선랜을 조합해서 무선공유기로 쓰기로 했다. 
[적절한 문서][base_ref]를 기반으로 삽질하면서 몇군데 수정했다. 
기본적으로는 [문서][base_ref]를 기반으로 따라서 진행하고 문제가 생겼을때 아래의 내용을 참고하면 도움이 될거다.

## 내부 네트워크는 접속이 되는데 밖으로 나가질 못한다?
라즈베리파이를 무선공유기로 돌리는데 성공했고 외부장치에서 라즈베리파이로 무선접속하는것은 성공했다. 
라즈베리파이에 HTTP서버가 있어서 거기에 접속을 시도했더니 접속은 문제없이 되는데 외부 사이트로는 접속이 안된다? 
이 경우는 NAT 설정이 제대로 안된것이다. 
[기본문서][base_ref]의 9번 항목, NAT 설정을 다시한번 확인하자. 
만약 제대로 설정되었다면, ```sudo iptables -L```을 했을 때 다음과 같이 뜬다.

```
Chain INPUT (policy ACCEPT)
target     prot opt source               destination

Chain FORWARD (policy ACCEPT)
target     prot opt source               destination
ACCEPT     all  --  anywhere             anywhere             state RELATED,ESTABLISHED
ACCEPT     all  --  anywhere             anywhere

Chain OUTPUT (policy ACCEPT)
target     prot opt source               destination
```
iptables 정보를 부팅시에 불러오는게 제대로 되어있지 않은 상태에서 그냥 재부팅을 하면 iptables 정보가 날라가니까 ```iptbales -L```을 통해서 제대로 설정되어있나 확인해보자.

## 부팅시 iptables 설정을 불러오지 못한다?
문서에서는 ```/etc/network/interface```에다가 ```up iptables-restore < /etc/iptables.ipv4.nat```를 넣으라고 되어있지만 내 경우, 이를 넣어놓고 재부팅을 하면 iptables 정보를 불러오지 못하더라. 그래서 [IptablesHowTo][iptables]을 참고해서 ```pre-up iptables-restore < /etc/iptables.ipv4.nat```로 고쳤다.

## wlan0를 고정IP로 만들기
iptables 정보를 불러보은건 성공했는데 이번에는 부팅한 다음에 wlan0 에 적절한 고정IP가 설정되어있지 않을때가 있더라. 그래서 [Can't force wlan0: to use static IP][use_static_ip]를 참고해서 설정파일 일부에 적절히 주석처리 했다.


## Configure File
내가 현재 쓰는 설정 파일이다. 이걸 적절히 참고해서 쓰면 될듯?

### /etc/network/interface
```
auto lo

iface lo inet loopback

iface eth0 inet dhcp
    # Reference : iptables HowTo
    pre-up iptables-restore < /etc/iptables.ipv4.nat

#allow-hotplug wlan0 <- comment
iface wlan0 inet static
        address 192.168.42.1
        netmask 255.255.255.0
# 어차피 무선공유기로 쓰는데 무선랜 검색이 필요한가?
#wpa-roam /etc/wpa_supplicant/wpa_supplicant.conf 

#iface default inet dhcp <- comment
```

### /etc/hostapd/hostapd.conf
```
# http://linuxwireless.org/en/users/Documentation/hostapd
# http://www.timokorthals.de/?p=243
# Content of /etc/hostapd/hostapd.conf
# 1. The Device which will act as AP
interface=wlan0
driver=nl80211

# 2. Parameters so that the daemon runs
#ctrl_interface=/var/run/hostapd
#ctrl_interface_group=0

hw_mode=g
ssid=myhome02
channel=6

# 4. Security of the Wifi connection
wpa=2
wpa_passphrase=ProfitPassword
wpa_key_mgmt=WPA-PSK
wpa_pairwise=CCMP
rsn_pairwise=CCMP

# 5. Other settings
beacon_int=100
auth_algs=3
wmm_enabled=1
```

## Reference
* [Turning The Raspberry Pi Into A Wireless Access Point][base_ref]
* [IptablesHowTo][iptables]
* [Can't force wlan0: to use static IP][use_static_ip]

[base_ref]: http://thebitbangtheory.blogspot.kr/2012/12/turning-raspberry-pi-into-wireless.html
[iptables]: https://help.ubuntu.com/community/IptablesHowTo
[use_static_ip]: http://www.raspberrypi.org/phpBB3/viewtopic.php?f=26&t=22660