Title: Memory Addressing
Subtitle: 2014년 2학기 시스템 프로그래밍 시험 공부
Tags: linux, system-programming, memory, memory-address
Slug: system-programming-memory-addressing
Author: if1live
Date: 2014-11-26

시스템 프로그래밍 시험 공부하면서 정리한 내용이다. 내용 갱신은 없을 예정이다.

## Memroy Addressing (Intel x86)
* 논리 주소 (Logical Address)
    * LDT 세그먼트는 모든 프로세스에서 공유
    
## Paging in Hardware (Intel x86)
* 페이지유닛 (Paging unit)은 선형주소를 물리주소로 바꾼다.
    * 접근 권한 검사 (페이지에 접근할 권한이 있는가?)
    * 실패시 page fault exception 발생시킴
* 페이지
    * 선형 주소는 고정 크기의 구간으로 그룹화 되어있다.
    * **페이지 안의 연속적인 선형주소는 연속적인 물리주소로 매핑된다.**
    * 페이지 프레임 (물리메모리)는 1페이지 크기
* 386 이상부터 지원
* CR0 레지스터의 PG bit를 설정해서 활성화
* 2-level 페이지 하드웨어
    * 페이지 디렉토리 : 페이지 테이블의 물리주소 (CR3 레지스터)
    * 페이지 테이블 : 페이지의 물리주소

## Regular Paging (Intel x86)
* Page Table
    * 선형주소를 물리주소로 매핑하는 자료구조
    * 페이지 크기 : 4KB
* 선형 주소 : 32 bit
    * 디렉토리 : 10
    * 테이블 : 10
    * 오프셋 : 12 bit (4KB page)
    * 1024 * 1024 * 4096 = 4GB
* 2-Level 페이징
    * 페이지 디렉토리
    * 페이지 테이블
* 최대 페이지 테이블 수 : 1025
    * 1 페이지 디렉토리
    * 1024 페이지 테이블
* 최대 페이지 갯수
    * 2 ** 20

## Page Directory and Page Table
* 테이블 구조
    * 1024개 항목 존재 (4KB, 항목당 4byte)
    * 하나의 페이지 프레임 안에 올라감 (=물리 메모리에서 1페이지 차지)
* 테이블 엔트리 구조 (32bit)
    * Present : 현재 메모리에 있는가?
    * 20 MSB : 페이지 프레임 물리 주소
    * Accessed : 페이지 유닛이 페이지 프레임에 접근할때마다 설정
        * 스왑될 페이지를 고를때 이용
    * Dirty : 페이지 프레임에 쓰기 발생시 설정
    * Read/Write : 페이지 권한 (read/write or read)
    * User/Supervisor : 접근하는데 필요한 권한 레벨
    * PCD, PWT : 하드웨어 캐시에서 사용
    * Page Sie : 페이지 디렉토리 엔트리에서만 사용
        * 설정하면 페이지 프레임은 4MB로 취급
* 32 bit의 테이블 엔트리는 MMU(memory management unit) Hardware에 의해서 해석된다.

## Extended Paging (Index x86)
* 펜티엄부터는 4MB 페이지 프레임을 허용
    * 페이지 테이블 조회하는 오버헤드 없다.
    * 크고 아름다운 연속적인 물리구역 사용.
* 32 bit 선형 주소
    * 디렉토리 : 10
    * 오프셋 : 22 (2**22 = 4MB)
    * 페이지 디렉토리 엔트리의 Page Size가 설정되어 있어야한다.
* 시스템에 존재하는 페이지 테이블 == 1개의 페이지 디렉토리

## Three-Level Paging Hardware
* 64-bit 아키텍쳐에서 사용
* 고려 사항
    * 페이지 크기 : 큰 페이지 크기가 합리적인가?
        * 페이지 크기가 커지면 swapping할때 오버헤드 발생. 왜냐하면 스왑은 페이지 단위
    * N-level
        * level가 깊어진다 -> 테이블 접근 부하 -> 성능 낮아짐
* 예제 : Alpha processor (64 bit)
    * 페이지 프레임 : 8KB, offset = 13bit
    * 43bit로 주소 표현, 최대 8TB
    * 10 + 10 + 10 + 13

## Paging in Linux
* 4-level 페이징 (2.6.11부터)
* 테이블
    * Page Global Directory
    * Page Upper Directory
    * Page Middle Directory
    * Page Table
    * Page (+ offset)

## 4-level Page Table
2-Level 페이징 하드웨어에서 4-레벨 페이징을 사용하는 경우는 다음과 같이 처리한다.

* Page Upper Directory, Page Middle Directory를 제거
    * 두 항목을 0으로 채운다
* 포인터에서 Page Upper Directory, Page Middle Directory의 위치는 변하지 않는다
    * 따라서 32비트, 64비트에 상관없이 같은 코드를 쓸 수 있다. 
* 커널은 Page Upper Directory, Page Middle Directory의 내부의 엔트리 갯수를 1개로 설정한다. 두개의 엔트리는 적절한 Page Global Direcotry의 엔트리로 매핑된다.


## Page Table Handling in Linux
* 페이지 주소를 얻기 위한 4-레벨의 테이블 접근
    1. Page Global Directory
    2. Page Upper Directory
    3. Page Middle Directory
    4. Page Table
    5. Page
* 페이지 테이블 관련 자료구조
    * `pgd_t`, `pud_t`, `pmd_t`, `pte_t`
* 페이지 테이블 `pte_t` : include/asm-i386/pgtable.h
    * 각각의 항목 = 32 비트 정수
    * fields : 20비트 주소 + 각종 플래그
* 페이지 테이블 함수
    * 페이지 테이블 할당/해제
    * `pte_*()` macro : include/asm-i386/pgtable.h
* 페이지 디렉토리 함수
    * `pgd_*()`, `pmd_*()` macro : include/asm-i386/pgtable.h

