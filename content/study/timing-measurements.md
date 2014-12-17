Title: Timing Measurements
Subtitle: 2014년 2학기 시스템 프로그래밍 시험 공부
Tags: linux, system-programming, timing
Slug: system-prog-timing-measurements
Author: if1live
Date: 2014-11-21

시스템 프로그래밍 시험 공부하면서 정리한 내용이다. 내용 갱신은 앞으로 없다.

# Timing Measurements

## Computer Time
* 많은 컴퓨터 작업은 time-driven
* time-driven example
    * 주기적으로 소프트웨어 업데이트 확인
    * 유저가 일정시간동안 작업하지 않으면 화면 끄기
    * 일정 시간 경과후 비밀번호 묻기
    * 프로세스의 시간 사용 추적, 스케줄링
    * Timeout (네트워크, 하드웨어 장치, ...)
* 리눅스커널의 Main time service
    * system uptime 유지
    * wall clock time 유지 (what time is it?)
    * 일정시간 후에 작업을 처리할 메카니즘(Trigger)
        * Timer는 커널이나 유저 프로그램에게 특정 시간이 경과한 것을 알려준다
        * alarm clock과 유사
* 컴퓨터는 2개의 시간 단위에서 작동한다
    * Process hardware
        * microscopic (unit=ns)
        * CPU 명령어 수행
            * integer add
            * FP multiply
            * FP divide
            * ...
    * Operating system
        * macroscopic (unit=ms)
        * 키 입력
        * 디스크 접근 시간
        * Screen 갱신
    

## Kernel(OS) Notion of Time
* System uptime
    * 컴퓨터 시작 이후의 경과시간
    * 컴퓨터 내부의 시간은 이산(discrete)
    * 같은 시간 간격안에 발생하면 동시로 인식
    * discrete time Tc
* Hardware provides system timer
    * Kernel Timer
    * PIT (Programmable Interval Timer)
    * 일정 주기(tick rate)로 인터럽트를 발생
    * 리눅스 커널 인터럽트 핸들러가 처리
        * Tc ++
* Tick
    * 두 timer interrupt 사이의 시간
    * tick = 1 / tick rate

## Timer Interrupt Frequency
* Trade-off
    * 높은 주기의 타이머 인터럽트
        * 좋은 반응성. 시간과 시간의 경계가 명확하다.
        * 타이머 인터럽트 핸들링 오버헤드가 크다.
        * 실제로 쓸모있는 작업을 수행할 시간이 줄어든다.
    * 낮은 주기의 타이머 인터럽트
        * 위와 반대
* Tick rate == HZ
    * 1초동안 발생하는 timer interrupt 횟수
    * 타이머 인터럽트 주기를 늘리면 타이머 인터럽트가 더 자주 호출된다.
    * 3.14의 경우 ```include/asm-generic/param.h``` 참고
    * ```#define HZ CONFIG_HZ```
    * 2.4 시절에는 HZ=100
    * 2.6.13 부터 HZ=1000
    * 현재 HZ=CONFIG_HZ. CONFIG_HZ의 기본값은 250
        * [related commit][config-hz-commit]
* jiffies
    * 시스템 부팅 이후의 tick 발생 횟수
    * 부팅과정에서 jiffies=0 초기화
    * tick 발생 => jiffies ++
    * jiffies = 32 bit
        * HZ=1000 -> 50일 경과 -> Overflow!
    * jiffies64 = 64 bit
        * ```get_jiffies64()```
        * 32bit 아키텍쳐에서는 64비트를 한번에 못 읽기 때문에 경쟁상태 발생가능
        * 이를 막고자 32비트 한정으로 xtime_lock를 이용

## Timing Measurements
* 리눅스 커널은 다음 작업을 수행해야한다.
    * 현재 시간과 날짜 유지 (wall clock time)
    * 현재 프로세스의 실행 시간 결정
    * 리소스 통계 갱신
    * 일정 시간 경과후 유저 프로그램 또는 커널에 알림을 보내는 타이머 유지
* Components
    * Hardware clock devices
        * RTC, TSC Register, PIT
    * 커널 자료 구조 / 시간 측정 함수
    * 시간과 관련된 시스템콜

## Hardware Clock Devices
### RTC (Real Time Clock)
* 메인보드에 독립된 칩으로 존재
* CPU나 다른 칩과는 독립적
* 컴퓨터가 꺼져있는 동안에도 유지 (배터리 이용해서 RTC 작동)
* IRQ8, 2~8192Hz 또는 RTC가 특정 값에 도달했을때 인터럽트 발생
* **리눅스는 RTC를 부팅할때 시간을 가져오는 목적으로만 사용**
    * 부팅과정에서 커널은 RTC를 읽어서 wall clock time을 초기화
    * wall clock time은 ```xtime```로 저장. 이후 접근 가능
    * ```/dev/rtc```를 이용해서 RTC 프로그래밍
    * ```/sbin/clock``` 로 시계 설정

### TSC register (Time Stamp Counter)
* 고해상도 시간 측정
    * CPU cycle
* Intel x86 : 64 bit TSC register
    * 하드웨어에 의해서 갱신됨. clock signal마다 증가
    * 모든 인텔 CPU는 CLK input pin이 있다. 이것으로 clock signal을 받을 수 있음
    * ```rdtsc``` 어셈블리 명령으로 읽기 가능
        * 400MHz 기준, TSC는 2.5ns 마다 증가
* TSC를 사용하면 PIT보다 정확한 시간을 얻을 수 있다
    * 네트워크 (타임스탬프, 스케줄)
    * 일부 디바이스 드라이버

### PIT (Programmable Interval Timer)
* ex) 8254 CMOS chip using 0x40~0x43 port
* 커널이 시간을 추적하는데 사용
* **Timer Interrupt**
* 리눅스는 PIT를 이용해서 IRQ0로 1000Hz 주기의 Timer Interrupt 받음

## The Linux Timekeeping Architecture
* 시스템 부팅하는 동안
    * 커널은 RTC를 이용해서 wall clock time 초기화
    * wall clock time은 ```xtime```에 저장
* 커널 타이머 인터럽트는 PIT에 의해 IRQ0에서 발생
    * interrupt handler + bottom half (softirq)
* timer interrupt handler에서 커널은 다음 작업을 수행
    * jiffies_64 ++
    * xtime안의 wall clock 갱신
    * 리소스 사용 통계 갱신
        * 현재 프로세스에 시스템 타임이나 유저모드 시간의 마지막 tick을 기록
    * softirq 발생시켜서 dynamic timer 처리
    * ```scheduler_tick()```
        * 현재 프로세스의 time slice 감소시킴
        * ```TIF_NEED_RESCHED```를 설정할 필요가 있으면 설정

## Timer Interrupt Handler
### Architecture-dependent routine
* arch/i386/timer.c:```timer_interrupt()``` -> ```do_timer_interrupt()```
* ```do_timer_interrupt()```
    * PIT의 ISR(Interrupt Service Routine)
    * 갱신된 wall time을 RTC에 저장
    * 아키텍쳐 독립적인 함수 호출
        * ```do_timer()```
        * ```update_process_timers()```
    * 커널 코드 프로파일링
    
### Architecture-independent routine
* kernel/timer.c:```do_timer()```
    * jiffies_64 ++
    * ```update_times()``` 호출
* kernel/timer.c:```update_process_timers()```
    * 로컬 CPU의 부하 통계 갱신 (utime, stime)
    * expired된 dynamic timer있으면 실행시키기
        * ```raise_softirq()``` 호출해서 TIMER_SOFTIRQ tasklet 활성화
        * ```run_local_timers()``` 호출
    * ```scheduler_tick()``` 호출
        * 현재 프로세스의 time slice 감소
        * 현재 프로세스의 quantum이 다 떨어졌는지 확인

## Updating the Time and Date
### Wall Clock (current time of day) Management
* 자료구조 : ```struct timespec xtime```
    * xtime.tv_sec : 1970.01.01 이후의 경과 시간 (sec)
    * xtime.tv_nsec : 마지막 초 이후 경과한 nanoseconds
* ```xtime``` + ```update_times()```

### gettimeofday()
* wall clock time을 얻을 사용하는 user-space 함수
* ```sys_gettimeofday()``` system call로 구현
* wall clock time은 user-space에서 주로 쓰임
* 커널은 주로 파일시스템 행동 때문에 wall clock time을 사용
    * inode의 타임스탬프

## Updating System Statistics
커널은 주기적으로 몇몇 정보를 모아야한다.

* 커널 코드 프로파일링
    * 커널의 hot spot 확인
        * 가장 자주 실행되는 커널 코드 조각
    * ```profile_tick()``` 로 수집
        * ```do_timer_interrupt()```에 의해서 호출됨
* 평균 시스템 부하 계산
    * 시스템 로드는 ```calc_load()```에 의해서 수집됨
      * ```update_times()``` 에 의해서 호출됨
    * ```TASK_RUNNING```, ```TASK_UNINTERRUPTABLE``` 프로세스의 갯수 세기
* 작동하는 프로세스의 CPU 리소스 사용 확인
    * kernel/timer.c:```update_process_times()```
    * interval counting (see Resource Usage Statistics)

## Resouce Usage Statistics
* 커널은 book-keeping 정보를 관리한다...
    * task_struct.utime : 유저모드에서 실행된 tick 횟수
    * task_struct.stime : 커널모드에서 실행된 tick 횟수
* "Interval counting"은 실행 부하를 대충 계산 하는 방법
    * tick 기준점에 프로세스가 커널/유저 모드 였는지만 센다.
    * 실제로는 1 tick의 시간동안 유저/커널 모드를 왔다갔다 할 수 있지만 그것은 무시. 실제와는 오차가 존재할 수 있다.

## Supporting "Software Timers"
### Software Timer
* 주어진 시간 경과 후(time-out)에 함수를 실행하는 소프트웨어 기능
* 대부분의 장치 드라이버에서 이례적인 조건 감지용으로 사용
* 프로그래머나 유저 프로세스가 특정 함수를 미래에 시스템 콜을 통해 실행 시키고자 할때 사용
### Note
* timer function 확인은 bottom half에서 처리된다.
* bottom half는 타이머가 활성화 된 이후로부터  한참뒤에 실행된다.
    * **커널은 타이머가 expire된 정확한 시점에 타이머 함수가 호출된다는 보장을 못한다**
    * **Real-time 어플리케이션에는 부적합**
* 우선순위 낮음 -> 정확한 tick 시점 실행 보장 못한다

## Types of Linux Souftware Timers
* Dynamic Timer 
    * 커널에 의해 사용
    * 동적으로 생성, 파괴
    * Kernel (Event) Timer
* Interval Timer
  * 유저 모드에서 프로세스가 생성

### Dynamic Timer (Kernel Timer)
#### Kernel Event Timer
* 함수 실행을 특정시간/미리 정해진 시간에 되도록 예약
* 동적으로 생성/파괴
* 활성화된 dynamic timer 갯수 제한은 없다
* data struct : include/kernel/linux/timer.h

#### Usage
* struct timer_list 객체 생성
    * ```init_timer(struct timer_list *)```를 이용해서 초기화
* 필드 초기화
    * function, data 필드에 값 설정
* kernel timer list에 추가
    * ```add_timer()```
* expired 전에 가능한 행동
    * reschedule : ```mod_timer(t, new_expires)```
    * timer 삭제 : ```del_timer_sync()```, ```del_timer()```
* dynamic timer 검사/실행
    * ```TIMER_SOFTIRQ``` 에 의해서 처리
    * kernel/timer.c:```run_timer_softirq()``` 현재 프로세서의 모든 expired된 타이머 실행
    * kernel/timer.c:```update_process_timers()``` : ```run_local_timer()```를 호출해서 TIMER_SOFTIRQ 발생시킴

#### Impl
* 구현 이슈
    * 모든 dynamic timer를 매 tick마다 검사하는것은 부하가 크다
    * 답 : ```tvev_base_t``` 자료구조 이용
* 이벤트 타이머 관리용 커널 자료구조 : ```tvec_base_t```
    * expiration 시간을 이용해서 512개의 리스트로 그룹화
        * 첫번째 256 list : 다음 1, 2, 3, ... 256 tick 이후에 이벤트 expire
        * 다음 64 list : 1\*2^8, 2\*2^8, 3\*2^8, ... 64\*2^8 tick
        * 다음 64 list : 1\*2^14, 2\*2^14, 3\*2^14, ... 64\*2^14 tick
        * 다음 64 list : 1\*2^20, 2\*2^20, 3\*2^20, ... 64\*2^20 tick
        * 다음 64 list : 1\*2^26, 2\*2^26, 3\*2^26, ... 64\*2^26 tick
    * kernel/timer.c
* data struct : ```tvec_base_t```
    * tv1, tv2, tv3, tv4, tv5
    * tv1 안애는 index와 256개의 포인터 구성된 vec가 있다. vec는 timer_list 요소를 가리킨다.
    * (index + k)번 리스트에 있는 모든 dynamic timer는 k-tick 이후에 expire
        * index : 매 tick마다 1 증가
        * 256틱 마다 모든 tv1의 모든 타이머는 사용된다.
    * index==0 으로 되돌아오면 ```tv2.vec[tv2.index]```을 이용해서 tv1을 다시 채운다.

## Delaying Execution
### Situation
* **Software timer는 몇ms 이하의 짧은 시간에서는 쓸모가 없다.**
    * 너무 짧은 시간에 kernel timer을 쓰기에는 신뢰성이 없다
        * real-time system이 아니니까.
    * 예를 들어 실행이 1ms 밀리면 계산 오차가 커진다.
* dynamic timer는 초기화 오버헤드와 최소 대기시간이 존재한다.
* 커널 코드(예를 들면 드라이버)는 **타이머 없이 실행을 시간을 미루는 기능**이 필요
    * 매우 짧은 대기. 예를 들면 하드웨어에 추가 시간을 줘서 주어진 작업을 완료시킬 수 있다.
    * 예시: 이더넷 카드의 속도 설정하면 다시 사용 가능해질 때까지 2ms 걸린다.
* Delay execution of Kernel
    * small delay loop (idle loop)

### Small Delay Loop
* jiffies 기반의 딜레이는 큰 단위이다. (ms)
* 커널은 microsecond, nanosecond 단위의 딜레이를 목적으로 2개의 함수를 제공한다
    * ```void udelay(unsigned long usecs)```
    * ```void ndelay(unsigned long nsecs)```
    * ex: udelay(150); = 150 microseconds 대기
* 시스템 부팅하는 동안 보정
    * CPU가 실행할수 있는 spinning loop 반복 횟수를 결정
        * ```loops_per_jiffy```에 저장. BogoMIPS
    * delay function은 원하는 지연 시간동안 몇번 루프를 반복해야하는지 결정할때 이 값을 사용
    

## Timer-Related System Calls
몇몇 시스템 콜은 유저모드 프로세스가 시간을 읽고 수정하고 타이머를 생성하는 것을 허용한다.

* time()
    * 1970.01.01 00:00:00 이후 경과한 second
* gettimeofday()
    * timeval 구조체를 이용해서 epoch 이후 경과한 second, microsecond 반환
* adjtimex()
    * xtime를 조정
    * root 유저만 사용 가능
    * 시간 동기화에서 사용
    * NTP (Network Time Protocol)
* setitimer(), alarm()
    * 리눅스는 유저모드 프로세스가 interval timer를 활성화 하는것을 허용
    * interval timer
        * 프로세스에 주기적으로 UNIX 시스널 보냄
        * 일정 시간 이후 시그널 1번 보내기
    * setitimer() 시스템 콜로 interval timer 활성화
    * alarm()
        * 일정 시간 이후 SIGALRM 을 프로세스로 보냄
    


[config-hz-commit]: https://github.com/torvalds/linux/commit/59121003721a8fad11ee72e646fd9d3076b5679c
