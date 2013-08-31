Title: Box2d Block Allocator
Tags: box2d, allocator
Slug: box2d-block-allocator
Author: if1live
Date: 2013-01-25

작은크기의 객체를 많이 할당할때 사용되는 할당자이다. 기본설정값의 경우, 대략 640byte보다 작은 메모리를 많이 요청할때 사용한다

## 기본 구조
Chunk, Block의 2가지 개념으로 나누어서 메모리를 관리한다. Chunk는 16kb(기본값)을 하나의 크기로 가짆다. Chunk는 하나의 거대한 메모리 덩어리이고 이를 적절히 block이라든 작은 단위로 잘라서 사용한다.

메모리를 몇 byte로 요청하든간에 정해진 크기의 메모리로 할당한다. 같은 크기의 메모리블럭을 모아서 관리하기 위한 편법으로 이를 채택한듯하다. 허용 가능한 메모리 할당 크기는 s_blockSizes에 정의되어잇다. 16, 13, 64, ....640 등이 가능하다.

메모리 할당 요청이 들어오면 m_freeLists를 사용해서 미리 할당된 메모리중에서 쪼개서 줄수잇다. m_freeLists는 blockSizes크기의 배열이다. 즉, 16, 32, 64...에 1:1로 대응되는 포인터를 저장한다.

s_blockSizeLookup에는 바이트별로 m_freeLists, s_blockSizes에 대응되는 인덱스를 가진다. 예를 들어서 3byte의 메모리를 요청하면 block allocator는 16byte의 메모리를 반환한다. 그렇다면 '3'을 값으로 넣으면 16byte에 대응되는 인덱스값이 '0'을 바로 얻을수 잇으면 여러모로 편할것이다. 그래서 이 변수가 생겻다. s_blockSizeLookup는 b2_maxBlockSize+1 크기의 배열이라서 0byte~b2_maxBlockSize byte에 1:1로 대응되는 블럭크기 인덱스를 저장한다.


```cpp
struct b2Chunk
{
	int32 blockSize;
	b2Block* blocks;
};
```
Chunk를 나타내는 구조체이다. 할당한 16kb의 메모리 블럭은 blocks가 가리킨다. blockSize로 Chunk가 처리할 메모리블럭의 크기를 표현한다(16, 32,,같은 s_blockSizes에 잇는 값) 디버깅용으로 쓰이지 실제 로직상에서는 blockSize를 직접 확인하지는 않는다

```cpp
struct b2Block
{
	b2Block* next;
};
```
Block를 나타내는 구조체이다. 내부에서 연산할때만 쓰이고 실제로 외부에서 메모리를 할당받아서 쓸때는 next값을 덮어써서 작업한다

## Malloc 
실제 로직이전에 처리되는 정책

* 0byte를 요쳥하면 NULL
* b2_maxBlockSize보다 큰 메모리를 요청하면 표준 malloc를 사용
  
### 원하는 크기의 블럭으로 이미 할당된 것이 남아잇는 경우
```cpp
int32 index = s_blockSizeLookup[size];
if (m_freeLists[index]) {
	b2Block* block = m_freeLists[index];
	m_freeLists[index] = block->next;
	return block;
}
```
할당가능할것을 반환하고 이전의 블럭이 가리키는 next를 새로운 freeList로 설정한다. block간의 연결은 block=>block=>NULL 로 해놓기 때문에 가능한것을 전부사용하면 최종적으로는 NULL이 되서 다른 로직으로 진입하게 된다

### 원하는 크기의 블럭이 할당되어잇지 않은 경우
```cpp
int32 index = s_blockSizeLookup[size];

if (m_freeLists[index] == false) {
	if (m_chunkCount == m_chunkSpace) {
		b2Chunk* oldChunks = m_chunks;
		m_chunkSpace += b2_chunkArrayIncrement;
		m_chunks = (b2Chunk*)b2Alloc(m_chunkSpace * sizeof(b2Chunk));
		memcpy(m_chunks, oldChunks, m_chunkCount * sizeof(b2Chunk));
		memset(m_chunks + m_chunkCount, 0, b2_chunkArrayIncrement * sizeof(b2Chunk));
		b2Free(oldChunks);
	}

	b2Chunk* chunk = m_chunks + m_chunkCount;
	chunk->blocks = (b2Block*)b2Alloc(b2_chunkSize);
#if defined(_DEBUG)
	memset(chunk->blocks, 0xcd, b2_chunkSize);
#endif
	int32 blockSize = s_blockSizes[index];
	chunk->blockSize = blockSize;
	int32 blockCount = b2_chunkSize / blockSize;
	b2Assert(blockCount * blockSize <= b2_chunkSize);
	for (int32 i = 0; i < blockCount - 1; ++i) {
		b2Block* block = (b2Block*)((int8*)chunk->blocks + blockSize * i);
		b2Block* next = (b2Block*)((int8*)chunk->blocks + blockSize * (i + 1));
		block->next = next;
	}
	b2Block* last = (b2Block*)((int8*)chunk->blocks + blockSize * (blockCount - 1));
	last->next = NULL;

	m_freeLists[index] = chunk->blocks->next;
	++m_chunkCount;

	return chunk->blocks;
}
```

새로운 chunk를 생성하고 이것으로 원하는 크기의 블럭을 관리한다. Chunk 저장 여유분이 떨어진 경우, 새로운 Chunk배열을 할당한다.

1. 새로 사용할 Chunk의 포인터를 얻는다.
2. Chunks에서 사용할 block 데이터를 할당한다. b2_chunkSize(16kb)를 할당한다
3. chunks가 커버할 block size를 저장한다. 디버깅에서나 쓰인다
4. blocks를 block size로 쪼갠다. 그리고 각각의 조각을 싱글 링크드 리스트 느낌으로 연결한다. block=>block=>block=>...=>NULL
5. chunk 사용수를 늘린다
6. freeList에 새로 할당한 블럭리스트중에서 2번째것을 넣는다
7. 새로 할당한 블럭리스트중에서 첫번째꺼를 반환한다

## Free
읽기 쉽게 하기위해서 디버깅용 코드는 날렷다. 전체소스는 원본코드를 참고한다.
로직전에 수행되는 정책

* size 0인 메모리를 해제하려고하면 그냥 리턴
* 음수 크기의 메모리는 해제 불가능
* b2_maxBlockSize보다 큰 메모리는 표준 free를 사용한다 (이것은 malloc부분과 대응됨)
  
```cpp
int32 index = s_blockSizeLookup[size];
b2Block* block = (b2Block*)p;
block->next = m_freeLists[index];
m_freeLists[index] = block;
```
해제한 메모리 블럭을 m_freeLists의 첫번째 요소로 집어넣는다.

##  Clear
Chunk에 연결된 블럭을 전부 해제한다.

## 특징 
* 이상한 크기의 메모리를 요청한다고 하더라도 16, 32, 64byte같은 식으로 맞춰서 반환하기 때문에 아키텍쳐 특성을 타지 않는다.
* 작은 객체의 많은 할당에는 유리하다. Chunk의 크기가 기본적으로는 16kb이다. 블럭크기별로 볼때 다음과 같은 갯수의 데이터를 저장할수잇다. 많은 객체를 생성한다고 하더라도 실제로는 16kb단위로 할당해서 하기떄문에 시스템malloc의 호출횟수는 상당히 줄어든다.
*  원하는 크기의 블럭이 남아잇으면 한번에 메모리를 획득할수있다
	* 16byte : 1024개 
	* 32byte : 512개 
	* 64byte : 256개 
	* 96byte : 170개 
	* 128byte : 128개 

* block마다 추가 데이터가 없다. block각각은 그냥 단순 포인터일뿐이다. block의 next를 저장하기는 하지만 그거는 임시값일뿐이고 실제로 외부에서 할당받아서 쓸때는 덮어써버린다.
  
## Reference
* [A Fast Efficient Allocatr for samll blocks of mem][1]
* box2d 소스
    * [b2BlockAllocator.h][2]
	* [b2BlockAllocator.cpp][3]

  [1]: http://www.codeproject.com/Articles/17060/A-Fast-Efficient-Allocator-for-Small-Blocks-of-Mem
  [2]: http://code.google.com/p/box2d/source/browse/trunk/Box2D/Box2D/Common/b2BlockAllocator.h
  [3]: http://code.google.com/p/box2d/source/browse/trunk/Box2D/Box2D/Common/b2BlockAllocator.cpp
