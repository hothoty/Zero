#pragma once
#include "IEventBase.h"


namespace Zero
{
	class ConnectionInfo
	{
	public:
		Zero::NetAddress addr;
		bool moved;
		ConnectionInfo() { moved=false; }
		ConnectionInfo(Zero::NetAddress ad, bool m) { addr = ad; moved = m; }
	};

	class IEventClient : public IEventBase
	{
	public:
		virtual ~IEventClient() {}


		// 서버 입장 성공 : 실제 연결시점은 Connect()호출시 즉시 리턴되며, 연결이후 약간의 시간후에 입장이 성공된다
		virtual void OnJoin(const ConnectionInfo& info) = 0;


		// 서버 퇴장 시점
		virtual void OnLeave(const ConnectionInfo& info) = 0;


		// 접속시도 결과 (주의 : 입장이 아님)
		virtual void OnConnectResult(bool isConnectSuccess) {};


		// 서버 이동 시도가 실패한 경우
		virtual void OnMoveToServerFail() {};


		// UDP Ready : bTrust는 전달보장이 아닌 현재 상태를 의미한다 (안정적인 상태인지, 모바일이나 공유기같은 불안정한 상태인지)
		virtual void OnReadyUDP(const bool bTrust) {};


		// 일시적 네트워크 장애시 연결복구시작 : 연결복구옵션 사용시에만 적용됨
		virtual void OnRecoveryStart() {}


		// 일시적 네트워크 장애시 연결복구성공 : 복구실패시 OnLeave이벤트 처리됨
		virtual void OnRecoveryEnd() {}


		// 서버측 최대 연결 숫자 제한에 걸림, 이후 자동적으로 연결 해제 된다
		virtual void OnServerLimitConnection() {}
	};
}

