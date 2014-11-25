Title: System Calls
Subtitle: 2014년 2학기 시스템 프로그래밍 시험 공부
Tags: linux, system-programming, system-call
Slug: system-prog-system-call
Author: if1live
Date: 2014-11-22

시스템 프로그래밍 시험 공부하면서 정리한 내용이다. 내용 갱신은 앞으로 없다.

# System Call
## System Call Principles
* 어플리케이션과 하드웨어 사이에 별도의 레이어를 끼워넣는다
    * 프로래밍 하기 쉽다
        * 하드웨어 장치의 low-level 프로그래밍 특징을 몰라도 된다.
    * 시스템 보안 향상
        * 커널은 인터페이스 레벨에서 요청이 올바른지 확인할 수 있다
        * sys call == 커널과 소통하는 유일한 통로
    * 프로그램 이식성 향샹
* System calls
    * 유저 모드 프로세스 - 하드웨어 장치 사이의 인터페이스
    * 커널 서비스를 요청

## POSIX APIs and System calls
* API (Application Programming Interface)
    * 주어진 서비스를 구하는 방법을 지정하는 함수 정의
    * ex) POSIX API인 malloc(), calloc(), free()는 libc안에 brk() system call로 구현되어 있다.
    * 프로그래머의 관점 : 유저 모드 라이브러리
* System call
    * 소프트웨어 인터럽트를 통한 커널 모드로의 명시적 요청
        * 커널 디자이너의 관점 : 커널 안에 속한다
    * 일부 시스템 콜은 1개 이상의 인자를 받는다
    * Return interger value
        * 실패시 리턴값은 -1, errno를 설정(에러코드는 include/asm-i386/errno.h 참고)
    * **user space안의 libc안에 wrapper함수로써 구현**

## System Call Handling in IA32/Linux
### system call을 호출/종료하는 2가지 방법
* ```int 0x80```, ```iret``` Assembly 명령어
    * 옛날 버전 리눅스 커널
    * 유저모드에서 커널모드로 바꾸는 유일한 방법
* ```sysenter```, ```sysexit``` Assembly 명령어
    * Intel Pentinum II 부터 사용 가능
    * 리눅스 2.6 부터 지원
    
### Inplications
* 커널은 ```int 0x80```, ```sysenter``` 를 위한 라이브러리를 모두 지원해야 한다.
* sysenter 기반의 표준 라이브러리는 int 0x80밖에 못쓰는 옛날 커널에도 대처해야 한다.
* 커널과 표준 라이브러리는 옛날/새로운 프로세서 모두에서 작동해야 한다.

### Overview of system call handling
1. 대부분의 레지스터 내용을 커널 모드 스택에 저장
2. system call service routine을 호출해서 시스템콜 처리
    * system call 진입 : int 0x80 or sysenter
3. 핸들러에서 빠져나올때 레지스터 내용을 복구하고 유저모드로 복귀
    * system call 종료 : system_exit or sysexit



## System Call Wrapper Routines
* 매크로를 이용해서 시스템콜 정의
    * 커널 쓰레드는 시스템콜을 호출하는 라이브러리 함수 사용 못함
    * 해당 래퍼 루틴 선언을 단순화
    * macro ```_syscall0``` ~ ```_syscall6``` (include/asm-i386/unistd.h)
* Macro _syscall0 ~ _syscall6
    * 0~6 : 시스템 콜에서 사용하는 인자 갯수
        * system call number 제외
    * syscallN()은 정확히 (2+2*N)개의 인자 필요
    * 6개를 초과하는 인자가 필요한 시스템콜 정의 불가능
    * 비표준 리턴값(int 아닌거) 사용하는 시스템콜 정의 불가능

## System Call Handling via "int 0x80"
* 유저 모드 프로세스에서 시스템콜을 호출하면...
    * CPU는 커널모드로 바꾸고 커널 함수 실행을 시작
    * 리눅스의 경우, ```int $0x80``` 어셈블리 명령으로 시스템콜을 호출한다.
    * int 0x80은 vector 128을 갖고있는 programmed exception을 발생시킨다.
* System call number
    * 요청된 시스템콜을 구분하는 목적으로 eax 레지스터를 사용
* 시스템 콜 핸들러는 다음 절차를 수행
    1. system_call() : 커널 모드 스택에 레지스터의 내용을 저장
        * ```SAVE_ALL``` macro
    2. dispatch table 기반의 system call service routine라고 부르는 C함수 호출해서 시스템콜 처리
        * ```sys_SystemCallName()```
    3. system_exit 도달하면 핸들러 종료
        * ```RESTORE_ALL``` macro : 레지스터 내용 복구
* 시스템콜 초기화
    * 커널 초기화 과정에서 호출하는 ```trap_init()```
    * vector 128에 대응되는 IDT entry를 설정
    * ```set_system_gate(0x80, &system_call);```
* ```system_call()```
    * 어셈블리로 작성된 코드
    * system call number (eax), exception handler에서 사용하는 CPU 레지스터를 스택에 저장
        * eflags, cs, eip, ss, esp는 제외. 이것은 hardware control unit가 자동으로 저장
        * ```pushl %eax```
        * ```SAVE_ALL```
    * 현재 프로세스의 thread_info 주소를 ebp에 저장
        * ```GET_THREAD_INFO(%ebp)```
    * system call number의 검증
    * 올바르면 system call number에 연결된 서비스 루틴 호출

## System Call Dispatch Table
* 시스템콜의 서비스 루틴 주소를 저장
    * ```sys_call_table``` 배열
    * ```NR_syscalls``` 개의 엔트리 (2.6.11 기준 289)
    * 2.6 기준 실제로 구현된건 280개 정도
    * ```sys_ni_syscall``` : not implemented. 나중에 쓸 수 있도록 예약

    
## System Call Handling via "int 0x80" Summary
1. 유저 프로세스에서 ```fork()```를 호출
2. ```fork()```가 정의된 libc.a에서 ```int $0x80```
3. IDT의 0x80(128)번째 entry에는 ```system_call()``` 함수가 있다. 이를 호출.
4. ```system_call()```에서 eax에 저장된 system call number를 이용해서 ```sys_call_table``` 접근
5. fork의 번호는 2. ```sys_call_table```의 2번째 항목에는 ```sys_fork()``` 주소가 있다. 이를 호출

## System Call Handling via "sysenter"
* 빠른 시스템콜
    * ```int``` 명령어는 몇몇 일관성/보안 검증 때문에 느리다.
    * ```sysenter```는 유저모드에서 커널모드로 바꾸는 빠른 방법
    * 일부 x86에서만 사용가능 (펜티엄2에서 추가됨)
    * CPU와 리눅스 커널이 동시에 지원할때 libc의 래퍼함수가 sysenter를 이용할수 있다.
* 시스템 콜 진입
    * ```__kernel_vsyscall()```을 이용해서 레지스터 저장
    * ```sysenter_entry()```는 ```system_call()``` 과 유사한 행동
* 시스템 콜 탈출(exit)
    * ```sysexit``` 명령어 : 커널모드에서 유저모드로 바꾸는 빠른 방법

## Parameter Passing
* CPU 레지스터를 통해 커널 모드 스택으로 복사
    * 레지스터를 사용하면 시스템콜 핸들러의 구조를 다른 exception 핸들러와 유사하게 만들수 있다
* 레지스터로 인자 넘기기
    * 각각의 인자는 32bit
    * 레지스터 갯수 때문에 인자 최대 6개로 제한
        * eax, ebx, ecx, edx, esi, edi
* 복잡한 데이터?
    * 32비트보다 크거나 6개를 넘는 경우?
    * POSIX 표준의 경우, 메모리 주소를 인자로 넘겨서 이를 처리
    * 커널은 유저모드 메모리 영역에 읽기/쓰기 가능

## Parameter Checking
* 커널이 유저의 요청을 처리하기 전에 모든 인자를 확인해야 한다.
* 방법 : 주소 검사
    * 인자가 주소로 넘어오면 커널은 그것이 프로세스 주소 공간에 있는지 확인
    * 2가지 방법
* Method 1
    * 프로세스의 주소공간에 있는가? 메모리 구역은 적절한 접근 권한이 있는가?
    * 시간이 걸림 (기존 리눅스에서 사용)
    * 구현은 단순
* Method 2
    * ```PAGE_OFFSET``` 보다 낮은 주소인가?
    * 효율적이지만 대충 검증
    * 진짜 검증은 필요해질 때까지 미룬다 (dynamic address checking)
    * 2.2 부터 사용

## Accessing User-Space Memory Address
* 요구사항
    * 시스템 콜 서비스 루틴은 유저 주소 공간에 데이터를 읽기/쓰기 하는 일이 생긴다
* user-space memory access function
    * ```get_user()```, ```put_user()```, ```copy_from()```, ```copy_to_user()``` ...
    * macro (include/asm-i386/uaccess.h)
    * ```__get_user_x()```, ```__put_user_x()``` 또는 다른 어셈블리 코드 이용
* example
    * system call : int stime(time_t *t)
    * 목적 : 시스템 시간을 t로 설정
    * 인자는 메모리 주소

## Adding a New System Call
1. user-space code에 래퍼함수 추가
    * include <linux/unistd.h>
    * _syscall0() ~ _syscall6() 이용
2. ```__NR_newsystemcall``` 추가
    * include/asm-i386/unistd.h
    * ```sys_ni_syscall``` 항목을 사용할 수 있다
3. 커널에 서비스 루틴 작성
    * ```asmlinkage int sys_newsyscall()```
4. system call dispatch table에 항목 추가
    * arch/i386/kernel/entry.S:```entry(sys_call_table)```
    * ```.long sys_newsyscall```


