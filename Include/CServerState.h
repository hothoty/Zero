#pragma once

namespace Zero
{
	class CServerState
	{
	public:
		CServerState();

		class CMonitorThread
		{
		public:
			UINT m_ThreadID;
			intptr_t m_CountQueue;
			intptr_t m_CountWorked;
			CMonitorThread();
		};
		CSafeArray<CMonitorThread> m_arrThread;
		intptr_t m_MaxAccept;

		volatile intptr_t m_nIoSend;
		volatile intptr_t m_nIoRecv;
		volatile intptr_t m_nIoAccept;
		volatile intptr_t m_nIoClose;
		volatile intptr_t m_nIoEvent;

		int m_nAlloc;					// 할당된 전체 블럭(즉석+일반)
		int m_nAllocInstant;			// 즉석할당
		int m_nAllocNormal;				// 일반할당(Pool이용)

		Int64 m_n64RecvFps;				// CStartOption의 m_MonitorTransfer를 끄는경우 계산되지 않음
		Int64 m_n64RecvCount;
		Int64 m_n64RecvByte;
		Int64 m_n64SendFps;
		Int64 m_n64SendCount;
		Int64 m_n64SendByte;

		intptr_t m_RecoveryCount;		// 연결복구 처리중인 클라이언트
		intptr_t m_AllowCount;			// 입장예정숫자
		intptr_t m_CurrentClient;		// 실제 연결된 클라(마스터client포함) 개수
		intptr_t m_JoinedClient;		// 실제 입장된 클라이언트 (연결과 인증완료)
		intptr_t m_ServerP2PCount;		// 서버p2p(마스터client) 숫자
		intptr_t m_ServerP2PConCount;	// 서버p2p(마스터client) 연결유지중인 숫자(연결상태모니터링)

		Zero::String m_test_data;		// test용도
	};
}

