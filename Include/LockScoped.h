#pragma once

namespace Zero
{
	class CScopedLock
	{
	public :
		  class CriticalSection
		  {
			 public:
				explicit CriticalSection(CScopedLock &crit);
				~CriticalSection();
			 private :
				CScopedLock &m_csSyncObject;
				NonCopyClass(CriticalSection);
		  };
		  CScopedLock();
		  ~CScopedLock();

	#if(_WIN32_WINNT >= 0x0400)
		  BOOL TryEnter();
	#endif
		  void Enter();
		  void Leave();

	   private :
		  CRITICAL_SECTION m_csSyncObject;
		  NonCopyClass(CScopedLock);
	};
}

