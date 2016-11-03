#pragma once
#include "IEventClient.h"

namespace Zero
{
	class ICoreClient : public IPacket
	{
	public:
		virtual ~ICoreClient() {};

		static ICoreClient* NewCore(IEventClient* pEventHandler, bool bTestMode = false);

		/**

		서버와 연결, 성공했다고 즉각 인증이 떨어지는건 아니며 인증과 입장이 완료되면 OnJoin 이벤트가 오게된다

		@bMobile : true 지정시 여러기능들을 더 모바일에 최적화된 방식으로 처리

		@bUseRecovery

		- 자동연결복구옵션, 일시적 연결장애나 WIFI전환 등의 상황에서 자동으로 복구해준다

		- 내부적으로 알아서 복구해주기 때문에 아무일도 일어나지 않은것처럼 서버로직을 처리할 수 있다

		- 복구시작 시점 발생 이벤트 : OnRecoveryStart

		- 복구완료 시점 발생 이벤트 : OnRecoveryEnd

		- 복구실패 시점 발생 이벤트 : OnLeave

		*/
		virtual bool Connect(String ipaddr, UInt16 portnum, UInt32 protocol_version=0, int portudp=0/*udp사용안함=0*/, bool bMobile=false, bool bUseRecovery=false) = 0;


		// 재연결
		virtual bool ReConnect() = 0;
		virtual bool ReConnect(String ipaddr, UInt16 portnum) = 0;


		// 서버 이동 : 직접 서버 주소 입력 : 이동 할 서버가 없다든지 기타 이유로 시도가 실패할 경우 OnMoveToServerFail 이벤트 발생
		virtual void MoveToServer(NetAddress addr) = 0;


		// 서버 이동 : 서버 타입 입력 -> 해당 타입 서버로 자동 로드벨런싱
		virtual void MoveToServer(int ServerType) = 0;


		// 서버 퇴장하기
		virtual bool Leave() = 0;


		// 서버 입장상태
		virtual bool IsJoined() = 0;


		// 서버와의 네트워크 연결 상태
		virtual bool IsNetworkConnect() = 0;


		// 서버와의 네트워크 연결에 대한 인증상태
		virtual bool IsNetworkAuthed() = 0;


		// 네트워크 패킷 및 이벤트 처리(매프레임 호출필요)
		virtual void NetLoop() = 0;

		// NetLoop와 동일한 함수
		virtual void FrameMove() = 0;


		// 연결 강제 종료(일방적인 종료) : 긴급한 경우가 아니라면 Leave()를 사용하여 정상적인 퇴장처리를 하는것이 좋다
		virtual void ForceLeave() = 0;


		// 명시적으로 연결 강제 종료(일방적인 종료) : 긴급한 경우가 아니라면 Leave()를 사용하여 정상적인 퇴장처리를 하는것이 좋다
		virtual void Destroy() = 0;


		// 즉각 연결복구 : 연결복구 옵션일때 실시간 WIFI전환시점에 사용, IOS경우 Reachability로 감지가능
		virtual void FastRecovery() = 0;


		// 그룹 ID
		virtual RemoteID GetGroupRemoteID() = 0;


		virtual UInt32 GetCoreVersion() = 0;
		virtual UInt32 GetProtocolVersion() = 0;
	};
	typedef std::shared_ptr<Zero::ICoreClient> ICoreClientPtr;
}

