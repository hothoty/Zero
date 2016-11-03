#pragma once
#include "basetypes.h"
#include "StartOption.h"
#include "IEventServer.h"
#include "IPacket.h"
#include "CServerState.h"
#include "AdoManager.h"

namespace Zero
{
	class ICoreServer : public IPacket
	{
	public:
		virtual ~ICoreServer();


		/**********************************************************
		*******               주요 인터페이스            *********
		***********************************************************/


		static ICoreServer* NewCore(IEventServer* pEventHandler, bool bMaster=false/*마스터서버옵션*/);


		// 서비스 시작
		virtual bool Start(const CStartOption &param, CResultInfo& outError) = 0;


		// 서비스 정지
		virtual void Stop() = 0;


		// 접속 숫자 : bAuthed(true) 순수 클라이언트 (마스터 서버/클라 숫자 제외한 숫자)
		virtual size_t GetCountClient(bool bAuthed=false/*인증된것만구할땐true*/) = 0;


		// 현재 상태
		virtual void GetCurrentState(OUT CServerState &outVal) = 0;


		// 원격 연결 접속 종료, bForce=강제종료(정상적인종료단계가무시됨)
		virtual void CloseRemoteClient(Zero::RemoteID remoteID, bool bForce=false) = 0;

		// CloseRemoteClient와 동일한 함수
		virtual void CloseConnection(Zero::RemoteID remoteID, bool bForce = false) = 0;


		// 전체 연결 접속 종료
		virtual void CloseAllClient(bool bForce=false) = 0;


		// 서버 종료단계 시작 : 깔끔한 서버종료를 위해 갑자기 끄지 말고 사전에 호출할것
		virtual void StopStart() = 0;


		// 네트워크 처리 함수(외부 스레드 사용시)
		virtual void NetLoop() = 0;



		/**********************************************************
		*******          마스터 서버 관련 인터페이스      *********
		***********************************************************/


		// 마스터 서버로 연결
		virtual bool MasterConnect(String ipaddr, UInt16 portnum, String Description, int server_type) = 0;


		// 마스터 서버의 연결 해제(마스터 서버가 관리하는 모든 서버와 함께 연결 해제됨)
		virtual void MasterDisconnect() = 0;


		// 이동 가능한 서버 선택 : 로드벨런싱 처리 (ServerType값이 0보다 작으면 모든 타입 검색)
		virtual bool SelectServer(int ServerType, OUT Zero::CMasterInfo& ServerInfo) = 0;


		// 이동 가능한 서버 목록 구하기 (ServerType값이 0보다 작으면 모든 타입 구하기)
		virtual void GetServerList(int ServerType, OUT Zero::CSafeArray<Zero::CMasterInfo>& ServerArr) = 0;


		// 서버 이동 시도
		virtual void ServerMoveStart(const Zero::RemoteID remote, Zero::NetAddress addr, Zero::ArrByte param, Zero::Guid idx=Zero::Guid()) = 0;



		/**********************************************************
		*******          그룹 관련 인터페이스 함수들      *********
		***********************************************************/

		// 그룹 생성 : 결과가 즉시 리턴됨, 생성된 그룹은 모든 맴버 탈퇴시 자동적으로 해체됨
		virtual bool CreateGroup(OUT RemoteID& GroupID, const RemoteID remote) = 0;


		// 그룹 참여 : 결과가 즉시 리턴됨, 1개 그룹인원은 소규모 유지 권장(가급적 100명내)
		virtual bool JoinGroup(const RemoteID GroupID, const RemoteID remote, OUT int& Members) = 0;


		// 그룹 탈퇴 : 성공시 OnGroupMemberOut 이벤트가 발생된다 (요청이 아닌 클라이언트의 접속해제 등에 의해 탈퇴될 수 있기 때문)
		virtual void OutGroup(const RemoteID GroupID, const RemoteID remote) = 0;


		// 그룹 해체 : 모든맴버 탈퇴처리, 성공시 OnGroupDestroy 이벤트가 발생됨 (그룹탈퇴와 동일하게 외부요인에 의해 해체될수 있기 때문)
		virtual void DestroyGroup(const RemoteID GroupID) = 0;



		/**********************************************************
		*******          Async Query/Request              *********
		***********************************************************/

		/**
		아래의 모든 Async 요청 함수들에 대해 remote값이 Remote_None값이 아닌
		 특정 remoteID값이 지정된 경우 해당 remote에 대해 실행 스레드를 유일하게 보장해준다(동기화 신경쓸필요없음)
		*/

		// 비동기 쿼리
		virtual bool Async_Query(const Zero::RemoteID remote, Zero::AdoManager p, const Zero::String szQuery, const Zero::lamda_db_result& result_func) = 0;

		// 비동기 SP
		virtual bool Async_SP(const Zero::RemoteID remote, Zero::AdoManager p, const Zero::String szNameSP, const Zero::lamda_prepare_sp& param_func, const Zero::lamda_db_result& result_func) = 0;

		// 사용자 정의 비동기 요청 처리
		virtual bool Async_Request(const Zero::RemoteID remote, const Zero::lamda_request& request_func, const Zero::lamda_result& result_func) = 0;



		/**********************************************************
		*******          기타 인터페이스                  *********
		***********************************************************/

		// Alive설정, 미설정시 기본값 60초
		virtual void SetKeepAliveOption(int nSec/*0=ALIVE끄기*/) = 0;
		virtual bool IsNagleOption() = 0;


		// Version 정보
		virtual UInt32 GetCoreVersion() = 0;
		virtual UInt32 GetProtocolVersion() = 0;


		//virtual bool TestMessage(const RemoteID remote, const byte* buffer, const pktsize_t length, void* con) = 0;
	};
	typedef std::shared_ptr<ICoreServer> CoreServerPtr;
}

