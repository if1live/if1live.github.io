Title: Kernel Synchronization
Subtitle: 2014년 2학기 시스템 프로그래밍 시험 공부
Tag: linux, system-programming, synchronization
Slug: system-prog-kernel-synchronization
Author: if1live
DAte: 2014-11-24

시스템 프로그래밍 시험 공부하면서 정리한 내용이다. 내용 갱신은 앞으로 없다.

# Kernel Synchronization
## Kernel Control Paths
* 커널 함수는 다음 요청에 따라서 실행된다
    * 유저 모드에서 실행되는 프로세스에서 예외 발생
        * int 0x80, sysenter : system call
    * 외부 장비가 IRQ Line을 이용해서 PIC로 시스널을 보낼때
        * 해당되는 인터럽트가 활성화 되어있는 경우
* Kernel Control Paths (KCP)
    * 커널모드에서 커널요청을 처리하려고 수행되는 코드
        * 요청이란? system call, interrupt, exception

* CPU는 kernel control paths를 상호배치(interleave)
	* ```schedule()``` 가 호출될때 context switch 발생
	* kernel control path 실행 도중 인터럽트가 끼어들수 있다 (nested interrupt)
		* 이경우, 첫번째 kernel control path는 끝나지 않은 상태로 남게된다.
		* 인터럽트를 처리하기 위해서 다른 kernel control path가 시작된다.
* kernel control path를 상호배치하는 것은 중요
	* 멀티프로세싱 구현
	* PIC와 디바이스 컨트롤러의 처리량 개선
* 이슈
	* kernel control path를 상호배치하면 커널자료 구조에 동시에 접근하는 경우를 고려해야한다
	* **Race condition**을 막기 위해서 커널 동기화가 필요
	
## Race Conditions
* 정의
	* 2개 이상의 KCP가 동시에 실행되면 결과가 영향을 받는 경우
	* **Critical Region** : 각각의 kernel control path에 의해 완전히 실행해야 되는 코드
* 리눅스 커널에는 경쟁상태가 발생할수 있는 가짓수가 많다
	* 커널 코드는 여러 CPU 위에서 돌아간다
	* 2.6 커널은 선점형
		* 스케줄러는 커널 코드를 선점할수 있고 다른 프로세스를 스케줄할 수 있다.
	* 인터럽트 : 인터럽트 처리중에 인터럽트가 발생할수 있다.
	* softirq, tasklet : softirq나 tasklet에 의해 현재 실행중인 코드가 인터럽트 받을수 있다.
	* 프로세스가 KCP 실행중에 잠들면, 다른 프로세스가 스케줄링 될 수 있다.
* 예제
	* 시스템콜을 호출해서 커널 모드에 진입
	* kernel control path가 critical section에 진입
	* 인터럽트가 발생! 인터럽트 핸들러가 실행됨
	* 인터럽트 핸들러가 critical section에 진입
	* 인터럽트가 끝나고 처음 kernel control path의 critical section으로 복귀
	* 인터럽트 핸들러가 첫번째 KCP와 동일한 자료구조를 건드렸다면?

## Synchronization Primitives
* 목표 : **커널 자료구조를 보호**
	* 공유데이터 사이의 경쟁상태를 피하면서 kernel control path를 상호배치하는 메카니즘 제공하기
* 리눅스 커널의 동기화 방법
	* Atomic Operation
	* Locking (spin lock, kernel semaphores)
	* Interrupt disabling

## Atomic Operations
* chip 수준에서 원자적 연산을 보장
	* read-modify-write 과정을 1개의 명령어로 처리한다. 처리도중 인터럽트 당하지 않는다
* Atomic operation은 유연한 커널을 구현하고 critical section을 만드는 강력한 메커니즘으로써 사용된다.
* Atomic operation in C
	* ```a = a + 1```은 원자적 연산이라고 보장하지 못한다
	* 리눅스 커널의 경우 별도의 함수를 이용
* interger의 원자적 연산
	* include/asm-i386/atomic.h
	* data structure : ```atomic_t```
	* ```atomic_read(v)```
	* ```atomic_set(v, i)```
	* ```atomic_add(i, v)```
	* ```atomic_inv(v)``` ...
* Bit operations : 특정 주소의 bit을 원자적으로 바꿈

## Locking
* Kernel locking
	* 공유 커널 자료구조나 임계구역(critical section)에 접근하려면 락(lock)을 획득해야한다
* 리눅스에서 제공하는 2종류의 락
	* Spin lock : busy-waiting lock
		* 멀티프로세서 시스템에서만 가능
	* Kenel semaphores : blocking lock
		* Uni-프로세서/멀티프로세스 시스템 모두에서 널리 사용

## Programming with Locks
* Lock은 알아서 구현해야한다
	* 프로그래머가 락없이 공유 자료구조에 접근하는 것이 가능하다.
	* 락없이 공유 자료구조에 접근하면 경쟁상태가 발생하거나 공유 데이터가 깨질수 있다.
* 무엇에 보호가 필요한가? 다음을 확인한다.
	* 전역 데이터인가? 쓰레드 실행중에 다른 쓰레드가 접근 가능한가?
	* process context와 interrupt context 사이에 공유하는 자료인가?
	* 2개의 다른 인터럽트 핸들러에서 공유하는 자료인가?
	* 현재 프로세스가 블럭될수 있는가? 그렇다면 공유데이터는 어떤 상태로 남는가?
	* 프로세스가 공유데이터 접근 도중 선점된다면 다른 스케줄링된 프로세스가 같은 데이터에 접근 가능한가?

## Lock Contention and Scalability
* Lock contention
	* 락이 현재 사용되는데 다른 프로세스가 락을 얻으려고 시도했다.
	* **highly contended lock**은 많은 프로세스가 동시에 락을 얻으려고 한다.
	* 발생원인 : 락을 자주 잡는다, 락을 오래 잡고 있다, 둘다.
* **Lock은 작업을 직렬화한다**
	* highly contented lock은 병목지점이 되어 시스템의 성능을 제한
* Scalability (확장성)
	* 시스템이 얼마나 늘어날수 있는가?
	* CPU 2개 넣어서 성능이 2개가 되는가?
* **Highly contended lock는 확장성을 제한**
	* 모든 프로세스가 queue에 늘어서서 작업을 처리하는 형태가 된다
	* 낮은 수준의 병렬 작업

## Locking Granulartiy
* Locking Granularity
	* 락이 보호하는 데이터의 양을 표현
* Coarse-grained lock : 많은 양의 데이터를 보호
	* 개발 쉽다
	* 작업 직렬화 => 낮은 성능
* Find-grained lock : 적은 양의 데이터를 보호
* 개발은 coarse-grained로 시작해서 점점 fine-grained로 발전시켜 나간다
* Example
	* Coarse-grained lock
		* 시스템에 존재하는 runqueue 전체를 락 1개로 관리
	* Medium-grained lock
		* CPU에 있는 runqueue별로 락 1개
	* Find-grained lock
		* runqueue의 priority-list별로 락을 가진다

## Spin Locks
* SMP (multiprocessor) 시스템에서 사용되는 락
	* 공유 변수를 이용
	* 변수를 설정 == 락 획득
	* spin == 변수가 unset될때까지 busy-wait loop
		* context switch 하는 부하가 없다.
	* spin lock을 너무 오래 잡고있으면 다른 CPU가 busy-wait 하느라 시간 낭비한다
	* ```spinlock_t``` : include/asm-i386/spinlock.h
* Uni-프로세스 시스템의 경우
	* **spin lock가 필요없다**
	* 어차피 동시에 돌아가는 프로세스가 없다
	* 대기하는 프로세스는 계속 돌아가고 다른 프로세스가 락을 놓을 가능성은 없다.
	
## Spin Locks and Interrupt Handlers
* Spin lock은 SMP 환경의 인터럽트 핸들러에서 사용할수 있다.
	* spin lock은 block가 아니니까
* 인터럽트 핸들러에서 락을 사용하면...
	* 락을 얻기 전에 **local interrupt 비활성화**
		* 인터럽트 발생하면 nested된다. 이상태에서 락이 꼬일수 있다.
	* local interrupt만 비활성한다. 다른 CPU의 인터럽트는 비활성화할 필요 없다.
		* spin lock은 local CPU에서만 돌아가니까.
* Local interrupt 를 끄지 않을때 Deadlock 발생하는 예제
	* 인터럽트 핸들러에서 lock을 획득
	* 인터럽트가 다시 들어옴, nested 발생
	* nested 인터럽트 핸들어에서 lock을 획득하려고 함
	* 근데 lock은 이미 획득되었잖아? 그리고 nested 끝나기 전까지는 풀 수가 없잖아?
	* DEADLOCK
	

## Read/Write Spin Locks
* 1 Writer, N Reader
* ```rwlock_t```

## Kernel Semaphores
* Kernel semaphores == Sleeping locks
	* 프로레스가 세마포어를 잡으려고 했지만 세마포어가 이미 잡혀있는 경우 세마포어의 대기큐에 프로세스를 넣는다.
		* 스핀락과의 차이. 스핀락이었으면 spin 시작함
	* 다른 프로세스가 스케줄 된다
	* 세마포어를 잡고있던 프로세스가 세마포어를 놓으면 세마포어의 대기큐에서 작업을 하나 꺼내서 깨운다
		* 깨어난 프로세스가 세마포어를 잡고 작업 시작
* 커널 세마포어는 잠재적으로 block
	* **커널 세마포어는 sleep가 필요 => interrupt, softirq, tasklet에서 사용 불가**

### Using Kernel Semaphores
* count 
	* count == 1 : MUTEX
	* count > 1 : Counting Semaphore
* Acquire (```down()```)
	* atomic(sem->count--)
	* count < 0 이면 current를 대기큐로
* Release (```up()```)
	* atomic(sem->count++)
	* count <= 0이면 대기큐에서 1개 깨우기

## Avoiding Deadlocks on Semaphores
* Deadlock
	* 프로그램이 2개 이상의 세마포어를 사용할때
	* 2개의 다른 경로가 서로 세마포어가 풀리는 것을 기다리는 경우
* 데드락 피하는 방법
	* **리소스 정렬** : 정해진 순서로 세마포어 요청

## Semaphores vs Spin Locks
* 프로세스는 세마포어가 사용 가능해질 때까지 잠든다
	* 적합한 락은 오랜시간동안 잡고 있을 것이다 (스핀락은 CPU 사이클을 낭비)
* 짧은 시간동안 잡고있는 락은 적합하지 않다
	* 대기큐를 유지 + 프로세스 스케줄링 + 깨어나는것의 오버헤드가 총 잠금 시간보다 클 수 있다.
* 프로세스가 세마포어를 획득하려고 하면 잠들 수도 있다 (획득 실패시)
	* **Interrupt context에서 사용 불가능**
		* interrupt handler
		* softirq
		* tasklet
	* **Process context에서만 세마포어 사용 가능**
		* system call
		* workqueue
* 락과 달리 세마포어를 잡고있는 것은 커널 선점을 비활성화 하지 않는다
	* 세마포어를 잡고있는 프로세스는 선점될 수 있다
	* 스케줄링 지연시간에 영향을 주지 않는다
* counting semaphore는 동시에 여러개 진입 가능
	* spin lock은 락당 1개만 허용

## Completions
* Completion 변수는 세마포어의 특별한 경우
	* 두 커널 작업을 동기화 하는 쉬운 방법
	* 멀티프로세스 시스템에서 down(), up()가 동시에 돌아갈때의 경쟁상태를 해결

## Interrupt Disabling
* local CPU의 인터럽트를 전부 비활성화/활성화
	* 커널 코드가 임계영역대로 작동하는것을 보장한다
	* 인터럽트를 끄면 인터럽트 핸들러가 현재 코드를 선점하지 못한다
	* 인터럽트를 끄면 커널 선점도 꺼진다
* 멀티프로세스 시스템
	* 인터럽트를 끄는것은 다른 CPU에는 영향이 없다. 그래서 다른 프로세스의 동시 접근을 막을 수 없다
	* **spin lock + local 인터럽트 비활성화**
* 인터럽트를 꺼도 interleaving은 발생한다
	* Kernel control path에서 직접 ```schedule()``` 호출
	* **커널은 인터럽트가 꺼져있는 동안 절대로 blocking 함수를 호출하면 안된다**
		* 시스템이 멈춘다
		* 왜냐하면 깨워줄 인터럽트가 없으니까
* 인터럽트를 끄는 임계구역은 짧아야한다.
	* 긴 임계구역은 locking을 이용해서 구현해야한다
	* 인터럽트 끄는 시간이 길다 -> 시스템 반응성이 떨어진다 

## Global Kernel Lock (The Big Kernel Lock)
* 리눅스 2.0의 커널 모드에서는 오직 1개의 CPU만 작동한다
	* 거의 조잡한 스핀락이었음
* 2.2/2.4에서 개선됨
	* 더이상 단일 스핀락에 의존하지 않는다
* 2.6에서 big kernel lock은 옛날 코드와 VFS, 일부 filesystem 관련 코드에서만 사용
	* 성능상의 문제로 거의 대부분 사라짐
* 2.6.11의 경우 ```kernel_sem``` semaphore == big kenel lock
	* 2.6 초반에는 스핀락으로 구현되어 있었음
* ```lock_kernel()```, ```unlock_kernel()```


## Synchronizing Access to Kernel Data Structures
* int 1개의 공유 자료구조
	* ```atomic_t``` + atomic operation

| KCP에서 접근하는 자료구조 보호           | Uni                | Multi       |
|------------------------------------------|--------------------|-------------|
| exception                                | 세마포어           |             |
| Interrupt                                | 로컬 인터럽트 끄기 |      스핀락 |
| deferrable func                          |                    | x or 스핀락 |
| exception + interrupt                    | 로컬 인터럽트 끄기 | 스핀락      |
| exception + deferrable func              | 로컬 softirq 끄기  | 스핀락      |
| interrupt + deferrable func              | 로컬 인터럽트 끄기 | 스핀락      |
| exception + interrupt + deferrable funnc | 로컬 인터럽트 끄기 | 스핀락      |

### Exception에 의해 접근하는 자료구조 보호
* 일반적으로 system call
* 세마포어 (UP + MP)

### Interrupt에 의해 접근하는 자료구조 보호
* 각각의 인터럽트 핸들러는 그 자체로 직렬화 된다 -> 하나의 타입의 인터럽트에 대해서는 동기화 할 필요없음
* 멀티 CPU의 경우
	* 다른 종류의 인터럽트 동시 작동 가능
	* 같은 종료우 인터럽트 동시 작동 불가능
* Uni-processor : 로컬 인터럽트 끄기
* Multi-processor : 로컬 인터럽트 끄기 + 스핀락

### deferrable function에 의해 접근하는 자료구조 보호
* Uni프로세서에서는 경쟁상태 없다
	* 모든 deferrable 함수는 CPU에서 직렬화되어 실행된다
* SMP의 경우 몇몇 deferrable 함수가 동시에 작동할 수 있어서 경쟁상태 존재
	* Softirq
		* 스핀락
		* 같은 softirq가 다른 CPU에서 동시 작동 가능
	* 한 종류의 tasklet
		* 필요없음
		* 같은 종류의 tasklet은 동시에 돌아가지 않는다
	* 여러 종류의 tasklet
		* 스핀락
		* 여러 종류의 tasklet은 다른 CPU에서 동시 작동 가능
### etc
* 경우의 수
	* Exception + Interrupt
	* Interrupt + Deferrable func
	* Exception + Interrupt + Deferrable func
* Uni프로세서
	* 로컬 인터럽트 끄기
* SMP
	* 로컬 인터럽트 끄기 + 스핀락

## Synchronization Mechanisms Summary
* Atomic Operation
* 인터럽트 끄기
	* 간단하지만 병렬성 낮아짐
* 스핀락
	* 다른 CPU로부터 보호하는데는 적절하지만 Uni프로세서에서는 불가능
* 세마포어
	* 다른 작업간의 적절한 동기화 방법
	* 스케쥴러 실행 가능성 있음, interrupt context에서 사용 불가능
* 일반적인 대응
	* Uni프로세서 : 인터럽트 끄기
	* SMP : 스핀락 + 인터럽트 끄기
