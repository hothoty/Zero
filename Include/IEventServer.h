#pragma once
#include "IEventBase.h"

namespace Zero
{
	/**
	로직을 멀티스레드 설정한 경우에도 remote 단위로 스레드 실행 순서가 보장됨
		
	- 이벤트 발생시마다 실행 스레드는 달라질 수 있으나 같은 remote에 대해서는 동시에 1스레드에서만 실행되는것을 보장

	- 로직 싱글스레드 설정한 경우 remoteID가 없는 기타 이벤트(Tick, 비동기 요청) 까지 완벽히 1스레드에서만 실행됨을 보장

	*/
	class IEventServer : public IEventBase
	{
	public:
		virtual ~IEventServer() {}



		/**********************************************************
		*******          클라이언트 관련 이벤트           *********
		***********************************************************/


		// 클라이언트의 실제 연결 시점 : 순수 클라이언트 또는 서버일수도 있다
		virtual void OnConnect(const Zero::RemoteID remote, const Zero::NetAddress& addr) {}


		// 클라이언트의 연결 해제 시점 : 순수 클라이언트 또는 서버일수도 있다
		virtual void OnDisconnect(const Zero::RemoteID remote) {}


		// 인증 완료된 순수 클라이언트의 입장 시점(서버를 제외한 클라이언트만 해당), 서버이동으로 입장된 경우 ==> move_server.Count > 0
		virtual void OnClientJoin(const Zero::RemoteID remote, const Zero::NetAddress& addr, Zero::ArrByte move_server, Zero::ArrByte move_param) = 0;


		// 인증 완료된 순수 클라이언트의 퇴장 시점(서버를 제외한 클라이언트만 해당)
		virtual void OnClientLeave(const Zero::RemoteID remote, const bool bMoveServer) = 0;


		// 접속숫자 제한에 걸린 클라이언트
		virtual void OnLimitConnection(const Zero::RemoteID remote, const Zero::NetAddress& addr) {}


		// 서버이동 시작 시점 : 완료시 이동한 서버의 OnClientJoin에서 userdata(move_server)값이 채워져 오게된다
		virtual void OnMoveServerStart(const Zero::RemoteID remote, OUT Zero::ArrByte& userdata) {}


		// 서버이동 요청받은 경우 파라미터 체크하여 승인/거부 처리
		virtual bool OnMoveServerCheckParam(Zero::ArrByte param, int cnt_idx) { return true; }


		// 서버이동 시작 시점 이후 목표서버로 실제 입장이 이루어지지 않은 경우 완벽한 로그아웃 처리를 위한 이벤트(회선문제 등의 이유로 발생할 수 있음)
		virtual void OnMoveServerFailed(Zero::ArrByte buffer) {}


		// Udp Ready 시점 : bTrust(true) : 신뢰할수 있음(전달보장의미가 아님),  bTrust(false) : Ready상태이나 모바일같이 불안정한 환경
		virtual void OnReadyUdp(const Zero::RemoteID remote, const bool bTrust) {}



		/**********************************************************
		*******          연결복구 관련된 이벤트           *********
		***********************************************************/


		/**
		연결복구 처리시작
		- 해당 remoteID에 대해 복구를 위한 대기 상태 시작
		- 일정시간 이후 OnRecoveryEnd가 호출되며 처리결과를 알수있음
		*/
		virtual void OnRecoveryStart(const Zero::RemoteID remote) {}


		/**
		연결복구 과정 정보
		- 새로운 연결 remoteNew를 연결복구 과정중인 remoteTo로 처리중
		*/
		virtual void OnRecoveryInfo(const Zero::RemoteID remoteNew, const Zero::RemoteID remoteTo) {}


		/**
		연결복구 과정종료
		- RecoveryTimeOut(true)  : 타임아웃으로 인해 복구실패
		- RecoveryTimeOut(false) : 해당 remote에 대한 연결복구성공, 복구된 remote에 대한 새로운 주소 = addrNew
		*/
		virtual void OnRecoveryEnd(const Zero::RemoteID remote, const Zero::NetAddress& addrNew, const bool RecoveryTimeOut) {}




		/**********************************************************
		*******          클라이언트 그룹 이벤트           *********
		***********************************************************/


		/**
		특정 그룹 맴버 탈퇴
		- 탈퇴 및 해체의 경우 요청에 의해 또는 기타 이유로 인해 일어남에 따라 이벤트로 처리
		- 그룹 생성과 합류의 경우 요청하는 즉시 결과를 알 수 있기 때문에 별도 이벤트가 발생되지 않는다
		- 생성 및 합류 : CreateGroup / JoinGroup
		*/
		virtual void OnGroupMemberOut(const Zero::RemoteID groupID, const Zero::RemoteID memberID, const int Members) {}


		/**
		그룹 해체
		- 탈퇴 및 해체의 경우 요청에 의해 또는 기타 이유로 인해 일어남에 따라 이벤트로 처리
		- 그룹 생성과 합류의 경우 요청하는 즉시 결과를 알 수 있기 때문에 별도 이벤트가 발생되지 않는다
		- 생성 및 합류 : CreateGroup / JoinGroup
		*/
		virtual void OnGroupDestroy(const Zero::RemoteID groupID) {}




		/**********************************************************
		*******          서버간 발생되는 이벤트           *********
		***********************************************************/

		/**
		다른서버의 입장 시점

		- 나와 이 서버간의 P2P연결이 실제 이루어진 시점이다

		- 내가 실행중일때 새로운 서버를 추가로 실행하게 되면 서버간 P2P연결이 자동으로 이루어지며 이벤트가 발생된다

		- 만약 여러대의 서버가 존재하는데 내가 실행된 경우 나와 이미 존재하는 모든 서버와 P2P연결이 이루어지며

		  따라서 나를 제외한 모든 서버와 이벤트가 발생하게 된다(마스터 서버는 당연히 예외 - P2P연결 해당없기때문)

		*/
		virtual void OnServerJoin(const Zero::RemoteID remote, const Zero::NetAddress& addr) {}


		/**
		다른서버의 퇴장 시점

		- 나와 이 서버간의 P2P연결이 해제된경우이다.

		  서버 종료같은 정상적인 종료하는 경우가 아니고 일시적으로 네트워크 등에 문제가 생겨서 연결이 해제된 경우

		  자동으로 지속적인 연결 복구가 시도된다. 재연결 성공시 즉시 OnServerJoin이벤트가 다시 발생하게된다

		*/
		virtual void OnServerLeave(const Zero::RemoteID remote, const Zero::NetAddress& addr) {}


		// 마스터 서버와의 연결+인증 성공 시점
		virtual void OnServerMasterJoin(const Zero::RemoteID remote, const Zero::RemoteID myRemoteID) {}


		// 마스터 서버와의 연결 해제 시점
		virtual void OnServerMasterLeave() {}


		// 서버간 정보 갱신
		virtual void OnServerRefreshInfo(Zero::CMasterInfoPtr info) {}


		// 마스터 서버 전용 이벤트 : 마스터 클라이언트가 인증성공 처리됨
		virtual void OnMaster_ServerJoin(const Zero::RemoteID remote, Zero::String Description, int type, Zero::NetAddress addr) {}




		/**********************************************************
		*******                기타 이벤트                *********
		***********************************************************/


		// Update Event발생
		virtual void OnUpdateEvent() {}


		// 비동기 Query 실행 완료 시점의 남은 pool count 정보
		virtual void OnQueryFinish(int PoolCount) {}
	};
}

