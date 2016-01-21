namespace Zero
{
	template<class ClassType>
	CPoolMem<ClassType>::CPoolMem(DWORD dwMax, bool bThreadSafe=true)
	{
		m_dwMax = dwMax;
		m_pBlocks = (tgaData*)CMemory::Alloc( m_dwMax * sizeof(tgaData) );
		if( NULL == m_pBlocks || 0x00000000 == m_pBlocks )
		{
			m_pBlocks = NULL;
			Throw("mem alloc error!");
			return;
		}
		m_pInters = NULL;
		m_dwInters = 0;
		m_dwIndex = 0;
		m_dwAlloc = 0;
		m_dwCount = 0;

		m_UseLock = bThreadSafe;
		if( !m_UseLock ) return;
		::InitializeCriticalSection( &m_cs );
	}

	template<class ClassType>
	CPoolMem<ClassType>::~CPoolMem()
	{
		if( m_dwAlloc != 0 )
		{
			Throw("instant block count not free!");
		}
		if( NULL != m_pBlocks )
		{
			CMemory::Free( m_pBlocks );
			m_pBlocks = NULL;
		}
		if( !m_UseLock ) return;
		::DeleteCriticalSection(&m_cs);
	}

	template<class ClassType>
	ClassType* CPoolMem<ClassType>::Alloc()
	{
		Lock();
		tgaData* pData = NULL;

		// 재활용 가능
		if( m_dwInters > 0 )
		{
			m_dwInters--;
			pData = m_pInters;
			pData->bNeedFree = false;
			m_pInters = m_pInters->pNext;

			m_dwCount++;
		}
		else // 재활용 불가
		{
			// 0부터 max까지 순서대로 할당
			if( m_dwIndex < m_dwMax )
			{
				pData = m_pBlocks + m_dwIndex;
				pData->bNeedFree = false;
				m_dwIndex++;
				m_dwCount++;
			}
			else	// 할당된 블럭없음 -> 즉석할당
			{
				pData = (tgaData*)CMemory::Alloc( sizeof(tgaData) );
				if( pData == NULL )
				{
					UnLock();
					return NULL;
				}
				pData->bNeedFree = true;
				m_dwAlloc++;
			}
		}
		UnLock();
		return (ClassType*)pData;
	}

	template<class ClassType>
	void CPoolMem<ClassType>::Free(ClassType* pDel)
	{
		Lock();
		tgaData* pData = (tgaData*)pDel;
		if( pData->bNeedFree )
		{
			CMemory::Free(pData);
			m_dwAlloc--;
		}
		else
		{
			m_dwCount--;
			m_dwInters++;
			pData->pNext = m_pInters;
			m_pInters = pData;
		}
		UnLock();
	}

	template<class ClassType>
	inline void CPoolMem<ClassType>::Lock()
	{
		if( !m_UseLock ) return;
		EnterCriticalSection(&m_cs);
	}

	template<class ClassType>
	inline void CPoolMem<ClassType>::UnLock()
	{
		if( !m_UseLock ) return;
		LeaveCriticalSection(&m_cs);
	}
}

