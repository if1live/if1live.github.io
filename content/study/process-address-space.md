Title: Process Address Space
Subtitle: 2014년 2학기 시스템 프로그래밍 시험 공부
Tags: linux, system-programming, memory
Slug: system-programming-process-address-space
Author: if1live
Date: 2014-12-08

시스템 프로그래밍 시험 공부하면서 정리한 내용이다. 내용 갱신은 없을 예정이다.

## 개요
* 선형 주소 공간 : 2 ** BIT\_PER\_LONG byte, 32bit의 경우 4GB
* 0 ~ 3GB (PAGE_OFFSET) : Process Address Space
* 3G의 Process Address Space에 대해서 자세하게 다룬다.

## Kernel Memory Allocation
* 관련 커널 함수 (이전 내용의 연속)
    * alloc_pages()
        * 버디 시스템에서 연속된 프레임 할당
    * vmalloc()
        * 불연속적인 메모리 영역 할당
    * kmem_cache_alloc(), kmalloc()
        * slab 할당자 이요. 특수한 타입, 혹은 일반적인 목적
* KMA (Kernel Memory Allocation) 특징
    * 커널은 OS에서 우선순위가 가장 높은 컴포넌트이다. 따라서 커널 메모리 할당은 지연되면 안됨
    * 커널은 스스로를 믿어야한다.
        * 에러 없음
        * 프로그래밍 에러에 대한 어떤 보호도 넣지 않는다
        * 커널코드에서 요청한 것은 무조건 신뢰하며 반드시 처리한다

## User Memory Allocation
* 유저 모드 프로세스가 메모리 할당 요청하면...
    * KMA에도 우선순위가 낮다 (예: `malloc()`)
        * 일반적인 규칙으로, 커널은 유저 모드 프로세스에서의 동적 메모리 할당의 지연실행을 시도한다.
    * 유저 프로그램을 믿을 수 없기 때문에 커널은 유저 모드 프로세서에서 발생하는 모든 어드레싱 에러를 잡을 준비를 해야한다.
* 유저 프로세스를 위한 메모리 할당의 특징
    * 유저 모드 프로세스가 동적 메모리를 요청하면 추가적인 페이지 프레임을 얻지않는다.
    * 대신, 새로운 범위의 선형 주소를 할당한다. 이 메모리는 프로세스 주소 공간의 일부가 된다
* 커널이 유저 프로세스를 믿지 못하기 때문에 물리 메모리가 아닌 Linear Address를 일단 할당한다. 물리 페이지는 나중에 처리
    

## The Process's Address Space
### 프로세스의 주소 공간
* 프로세스가 사용할수 있도록 허용된 모든 선형 주소 공간으로 구성
* 커널은 프로세스의 주소공간을 선형 주소의 구간을 붙이고 제거함으로써 동적으로 수정할 수 있다.

### Memory Region (메모리 구역)
* 선형 주소의 시작점, 길이, 접근권한으로 규정된 선형 주소공간에서의 구간
* 효율성 문제로 시작 주소와 크기는 4KB의 배수
    * 페이지 프레임의 크기가 4KB, N개의 페이지에 맞추려고

## When Process Get New Memory Regions?
1. **새로운 프로세스가 생성될 때**
    * 새로운 프로세스용으로 메모리 구역을 할당
    * `fork()`
2. **완전히 다른 프로그램을 로딩할 때**
    * `exec()` 계열 함수. execution context의 내용을 새로운 실행파일로 교체
3. **Memory-mapping file을 사용할 때**
    * `mmap()` 시스템 콜을 사용해서 프로레스가 새로운 메모리 매핑 파일을 생성할 때
    * `mmap()` : 메모리 매핑 파일을 생성, 프로세스 주소 공간에 연결
4. **유저 모드 스택에 데이터를 추가할 때**
    * 메모리 구역의 크기를 늘린다
5. **다른 협력 프로세스와 데이터를 공유할 때*
    * IPC shared memory
6. ** malloc()를 통해 프로세스의 동적 영역(heap)를 확장 **
    * heap에 할당된 메모리 영역을 확장

### Related System Call
* brk : heap
* execv : 새로운 실행 파일 로드하고 프로세스 주소공간 변경
* fork : 새로운 프로세스 생성하면서 새로운 주소 공간 생성
* mmap, mmap2, mremap : memory mapped
* shmat, shmdt : shared memory region
* etc...

## Memory Descriptor
* mm_struct
* 프로세스 주소 공간과 관련된 정보
* task_struct의 mm에서 연결됨
* `struct vm_area_struct *mmap` : memory region을 linked list로 구성
* `struct rb_root mm_rb` : memory region을 RB-tree로 구성

## Memroy Regions
* Memory regions
    * 연속적인 페이지 번호를 가지고 있는 페이지들의 집합으로 구성
* Memory region descriptor : `struct vm_area_struct`
    * 선형 주소 공간을 식별
    * `vm_start`, `vm_end`
    * `vm_mm` : mm_struct 가리킴

## Adding/Removing Linear Address Interval
* 이미 존재하는 메모리 영역에 인접하게 같은 종류의 메모리 구역을 추가하는 경우
    * 두개를 합쳐서 하나로
* 이미 존재하는 메모리 영역에 인접하게 다른 종류의 메모리 구역을 추가하는 경우
    * 그냥 두개인 상태로 유지
* 기존의 메모리 영역에서 끝의 일부를 제거하는 경우
    * 메모리 영역의 크기가 작아진다
* 기존의 메모리 영역에서 가운데 일부를 제거하는 경우
    * 메모리 영역이 두개로 쪼개진다.
    * 머리+가슴+배 - 가슴 = 머리+배


## Handling Memroy Regions
* 커널은 메모리 구역을 찾는 함수를 자주 수행한다
    * 대부분의 리눅스 프로세스는 몇개의 메모리 구역만을 사용한다
    * 하지만 일부 큰 어플리케이션은 몇천개의 메모리 구역을 가질 수 있다.
        * 비효율적이고 느린 성능이 될수 있다
* 많은 수의 메모리 구역을 다루는 리눅스의 접근법
    * 프로세스가 많은 수의 메모리 구역을 가지면 리눅스는 디스크립터를 RB-Tree에 저장
    * RB-Tree는 요소의 수가 적을때 효율적이지 않다.
    * **linked-list / RB-tree** 모두 상황에 따라서 사용 (적으면 list, 많으면 RB)
    * **성능향상**을 목표로 추가 메모리를 사용해서라도 자료구조 2개 유지
    * 2.4.9 까지는 AVL-tree 이용했음
* 메모리 구역 다루기
    * 일반적으로 RB-Tree : 특정 주소를 포함하는 메모리 구역을 찾을때
    * 일반적으로 linked-list : 전체 메모리 구역을 검색
* 메모리 구역 접근 권한
    * `vm_area_struct` : vm_flags
    * 페이지와 연관되는 다른 플래스
        * 각각의 페이지 테이블 엔트리를 위한 읽기/쓰기, present flag
        * page descriptor를 위한 플래그
* 선형 주소 구역을 할당/해제 하는 함수
    * `do_mmap()` : include/linux/mm.h
        * VM area를 생성하고 mapping 수행 (메모리 공간 늘림)
    * `do_munmap()` : include/mmap.c
        * VM area를 해제하고 주소 공간을 축소
* 다른 함수
    * `find_vma()` : 주어진 주소에 가장 가까운 메모리 영역 찾기
    * `find_vma_intersection()` : 주어진 주소 구간에 겹치는 메모리 영역 찾기
    * `get_unmapped_area()` : 자유 주소 구간 찾기
    * `insert_vm_struct()` : 메모리 디스크립터 리스트안에 메모리 구역 넣기
    
## Page Fault Exception Handling
* **Page Fault Exception**
    * **Address Translate Hardware에서 발생됨**
        * 프로세스 주소 영역에는 포함되지만 아직 할당되지 않음 페이지를 참조할때 발생
    * `do_page_fault()` 이용해서 예외를 다룬다
* `do_page_fault()`
    * page fault ISR (interrupt service routine)
        * 아키텍쳐 특화, arch/i386/fault.c
    * 현재 프로세스의 메모리 영역에 대해 페이지 폴트가 발생한 선형 주소를 비교한다.
        * 물리 페이지에서 페이지 프레임 찾기
        * 없던 페이지를 로딩해서 집어넣기
        * 페이지 테이블 갱신 

## Linux Page Fault Handling : Overall Scheme
* 주소가 프로세스 주소 영역에 포함되는가? 예
    * 메모리 구역의 접근 권한이 일치하는가?
        * 예 : 합법적 접근, 새로운 페이지 프레임을 할당
        * 아니오 : 불법 접근, SIGSEGV
            * 예시 : 코드영역에 쓰기 시도
* 주소가 프로세스 주소 영역에 포함되는가? 아니오
    * 유저 모드에서 예외가 발생했는가?
        * 예 : 커널 버그, 프로세스 죽인다
        * 아니오 : 불법접근. SIGSEGV

## Linux Page Fault Handling
* VM Area -> Read/Write 권한 확인 -> Page Fault 처리
1. VM Area가 아닌 구역 읽기
    * 가상 주소가 올바른가?
    * `vm_area_struct` 가 정의된 영역인가?
    * 그렇지 않으면 segmentation violation 시그널
2. .text 쓰기
    * 프로세스가 해당 영역에 읽기/쓰기 가능한가?
    * 그렇지 않으면 protection violation 시그널
3. .data 쓰기
    * 조건 충족하니까 fault를 처리

## do_page_fault()
* good_area
    * **프로세스 주소 공간에 주소가 포함된다**
    * 쓰기 접근에서 예외 발생한 경우, 함수는 메모리 구역이 쓰기 가능한지 확인한다. 쓰기 구역이 아니면 goto bad_area
    * 읽기/실행 접근에서 예외 발생시, 함수는 페이지가 이미 램에 존재하는지 확인한다.
* bad_area
    * **프로세스 주소 공간에 주소가 포함되지 않는다**
    * 유저 모드에서 에러 발생시 SIGSEGV 를 현재 프로세스로 보낸다
    * 커널 모드에서 예외 발생시
        * 시스템콜 인자로 넘어온 유저 프로세스의 주소를 이용할때 발생
        * 이 예외는 진짜 커널 버그 때문에 발생했다

## Allocate a New Page Frame
* `handle_pte_fault()`
    * **프로세스를 위한 새로운 페이지 프레임을 어떻게 할당할지 결정**
* 접근한 페이지가 present가 아닌 경우
    * 그것이 어떤 페이지 프레임 안에도 저장되어 있지 않은 경우, 커널은 새로운 페이지 프레임을 할당하고 적절히 초기화
    * Demand paging
* 접근한 페이지가 present이지만 읽기 전용인 경우
    * 그것이 페이지 프레임에 저장되어 있는 경우, 커널은 새로운 페이지 프레밍을 할당하고 기존 프레임의 내용을 새로운 프레임에 복사
    * Copy On Write 

## Demand Paging
* 원리
    * 동적 메모리 할당 기술은 페이지 프레임 할당을 진짜로 접근할때까지 미루는 것으로 구성
        * 프로세스가 RAM에 올라가지 않는 주소의 페이지에 접근할때까지 연기 (Page Fault exception 발생)
    * 모든 주소가 사용되진 않는다 (지역성)
    * 페이지 폴트 예외 처리하는 오버헤드 발생
* 어드레싱된 페이지는 메인 메모리에 존재하지 않을 수 있다
    * `do_no_page()` : pte 엔트리가 전부 0 일때
        * 프로세스가 페이지에 접근한 적이 없다
    * `do_swap_page()` : pte 엔트리가 0이 아닐때
        * 프로세스가 페이지에 접근한 적이 있으나 페이지 내용이 일시적으로 디스크에 저장되었다
        * swapped out to disk

## Copy On Write
* 전통적인 유닉스에서는 fork()를 이용해서 새로운 프로세스를 만들었다
    * 부모의 전체 주소 공간을 자식 프로세스의 주소 공간으로 복제 -> 비용이 비싸고 시간이 오래 걸린다
    * 어떻게 실제 복사 과정을 미룰수 있는가?
* Copy On Write (`do_wp_page()`)
    * 페이지 프레임을 복사하는 대신 부모와 자식 프로세스는 페이지를 공유한다
    * **쓰기가능 영역의 페이지를 읽기 전용으로 만든다.**
    * 프로세스가 쓰기 시도시 하드웨어에 의해 page fault 발생
        * fault 핸들러는 COW를 인식하고 페이지를 복하한 다음에 쓰기 권한을 복구한다
    * 최종 결과
        * 복사과정은 진짜로 필요해질때까지 지연된다 (프로세스가 공유 페이지를 수정하려고 할때)

### Creating a Process Address Space
* copy_mm()
    * 프로세스 주소 공간을 생성
        * 새로운 프로세스의모든 페이지 테이블과 메모리 디스크립터를 설정
    * clone(), fork(), vfork(), ...
    * clone() + CLONE_VM = lightweight 프로세스 생성
* CLONE_VM 설정시
    * `tsk->mm = current->mm` 같은 식으로 복사

### Deleting a Process Address Space
* exit_mm()
    * 프로세스가 갖고있는 주소 공간을 해제

## Managing the Head
* Heap
    * 프로세스의 동적 메모리 할당요청에 사용되는 특별한 메모리 구역
    * start_brk, brk가 해당 구역의 시작과 끝을 표시
* C library
    * malloc
    * calloc(n, size)
    * free
    * brk
        * heap의 크기 직접 변경
        * heap 크기 줄이기 : `do_munmap()` -> `sys_brk()`
        * heap 크기 늘이기 : `do_mmap()` -> `sys_brk()`

## kswapd() : Kernel Swap Daemon
* 커널 쓰레드로 구현
    * `kswapd()` : mm/vmscan.c
    * 주기적으로 깨어남
        * 메모리가 부족할 경우 더 자주 깨어남
* 메모리가 부족한지 확인
    * 사용되지 않은 페이지의 나이 계산
    * 페이지를 inactive list로 이동
    * dirty page를 디스크에 쓰기
    * 페이지 swap out이 필요한 경우 수행
* Zone 자료 구조와 관련 (이전 내용 참고)
    * pages_min : 도달시, kswapd가 깨어남
    * pages_low : 도달시, 할당자는 kswapd()를 동기적 방법으로 수행
    * pages_high : kswapd가 깨어나면 high만큼의 자유 페이지 프레임이 생길때까지 잠들지 않는다

## Page Replacement
* 2개의 LRU List
    * inactive_list
        * 중요하다고 알고있지 않은 페이지 목록
        * 파일 시스템에 의해 디스크에 기록되는 동안 dirty page는 몇 패스동안 inactive_list에 유지된다
    * active_list
        * 자주 참조된다고 알고있는 페이지 목록
        * working set으로 정의됨
        * 프로세스가 계속 참조
* 새로운 페이지는 inactive_list의 머리에 추가됨
* 메모리가 부족해지면, active\_list의 일부 페이지는 inactive\_list의 머리에 들어가고 inactive_list의 꼬리에서 일부 페이지가 해제된다. 
* 감지된 working set은 active_list로 들어간다

## Summary : Process Memory
* Process Virtual Address Space는 Memory Area로 표현된다.
* 물리 메모리는 kswapd에 의해 swap out 당해 Backing Store(hard-disk) 에 들어갈수 있다
* Page Fault에 의해 Backing Store의 내용이 물리 메모리로 채워진다. 이때 Page Table을 이용한다.


