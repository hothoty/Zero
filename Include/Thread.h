#pragma once

namespace Zero
{
	class CThread
	{
	public:
		CThread();
		virtual ~CThread();

		bool Start();
		void Stop();
		
		virtual void OnProcess(CThread* p) = 0;		// Stop명령 내릴때까지 자동으로 계속 실행되는 함수

	private:
		HANDLE hThread;
		UINT ThreadID;
		bool bClose;
		bool bStart;

		static unsigned int WINAPI Loop(LPVOID p);
	};


	class CControlThread
	{
	public:
		CControlThread();
		virtual ~CControlThread();

		bool	CreateThread( DWORD dwTimeMS );		// 스레드 생성, 처음한번만 실행 -dwTimeMS 스레드 실행될 시간 MS단위
		void	DestroyThread();					// 스레드 종료
		void	Run();								// 스레드 생성후 스레드가 시작시점 호출
		void	Stop();								// 작동 정지 (재작동 -> Run()함수 사용)

		inline bool	IsRun() { return m_bIsRun; }	// 현재 스레드가 실행중인지

		// 스레드 실행 카운트 (OnProcess실행 회수)
		inline DWORD GetTickCount() { return m_dwTickCount; }

	private:
		HANDLE	m_hThread;
		HANDLE	m_hQuitEvent;
		bool	m_bIsRun;
		DWORD	m_dwWaitTickMS;
		DWORD	m_dwTickCount;

		static unsigned int WINAPI CallTickThread(LPVOID p);
		void	TickThread();

		virtual void	OnProcess() = 0;
	};
}

