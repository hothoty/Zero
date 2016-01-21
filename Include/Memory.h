#pragma once
#include <memory>

namespace Zero
{
	/**
	클래스 메모리풀 사용 방법

	- 프로그램 시작시 자동 메모리풀 할당, 종료시 해제 (할당 시점을 직접 관리해야할 경우 CPoolMem클래스 직접 사용)

	- 클래스 내부에서 DECLARE_POOL_MEMORY

	- cpp에서 IMPLEMENT_POOL_MEMORY 선언

	  @param sizeBlock : 메모리블록 숫자
	  @param bThreadSafe : 스레드Safe옵션

	*/
	#define DECLARE_POOL_MEMORY(className)\
		typedef std::shared_ptr<Zero::CPoolMem<className>> CPoolMemPtr;\
		static CPoolMemPtr g_p##className##MemPool;\
	public:\
		inline void* operator new(size_t size)\
		{\
			return (void*)(g_p##className##MemPool->Alloc());\
		}\
		inline void operator delete(void* ptr, size_t size)\
		{\
			g_p##className##MemPool->Free((className*)ptr);\
		}

	#define IMPLEMENT_POOL_MEMORY(className,sizeBlock,bThreadSafe)\
		className::CPoolMemPtr className::g_p##className##MemPool(new Zero::CPoolMem<className>(sizeBlock,bThreadSafe));


	// 메모리 할당 관리자
	class CMemory
	{
	public:
		static void* Alloc(size_t size);
		static void* Realloc(void* p,size_t size);
		static void Free(void* p);
	};


	/**
	메모리 풀 클래스 (고정 메모리풀 + 실시간 할당 방식)
	-  a = new CPoolMem<ClassType>(nSize);
	*/
	template<class ClassType>
	class CPoolMem
	{
	private:
		struct tgaData
		{
			ClassType data;
			tgaData* pNext;
			bool bNeedFree;	// 즉석 할당받은 메모리
		};

	public:
		CPoolMem(DWORD dwMax, bool bThreadSafe=true);
		~CPoolMem();

		DWORD GetCountInstant()		{ return m_dwAlloc; }				// 즉석할당 블럭 숫자
		DWORD GetCountAlloc()		{ return m_dwCount; }				// 일반할당 블럭
		DWORD GetCount()			{ return m_dwCount + m_dwAlloc; }	// 전체 블럭

		tgaData* GetBlock() {
			return m_pBlocks;
		}

		ClassType* Alloc();
		void Free(ClassType* pDel);
		void Lock();
		void UnLock();

	private:
		tgaData* m_pBlocks;
		tgaData* m_pInters;

		DWORD m_dwIndex;
		DWORD m_dwInters;
		DWORD m_dwMax;
		DWORD m_dwAlloc;
		DWORD m_dwCount;

		bool m_UseLock;
		CRITICAL_SECTION m_cs;
	};
}

#include "Memory.inl"
