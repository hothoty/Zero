#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <future>
#include <functional>

//#define USE_EXAMPLE_ASYNC_QUERY	// 비동기 Query 예제 : 멀티스레드일때에도 remote단위로 순서를 보장함


// RMI Stub 람다사용하지 않고 클래스 오버라이딩 방식으로 처리하기
//#define USE_RMI_OVERRIDE_MODE


#include "../SampleCommon/Sample_proxy.h"
#include "../SampleCommon/Sample_proxy.cpp"
#include "../SampleCommon/Sample_stub.h"
#include "../SampleCommon/Sample_stub.cpp"


#ifdef USE_RMI_OVERRIDE_MODE
class C2SStub : public Rmi::Stub
{
public:
	Stub_Rmi_request_message_override;
};
C2SStub g_C2SStub;
Rmi::Proxy g_S2CProxy;


Stub_Rmi_request_message(C2SStub)
{
	printf_s("recved remote[%d] : %s\n", remote, CW2A(msg));
	g_S2CProxy.reponse_message(remote, Zero::CPackOption::Basic, testClass, dic_test, msg);

#ifdef USE_EXAMPLE_ASYNC_QUERY
	Svr.ExampleSQL_QueryAsyncRemote(remote);
#endif
	return true;
}
#endif


class CSampleServer : public Zero::IEventServer
{
public:
	#ifndef USE_RMI_OVERRIDE_MODE
	Rmi::Proxy proxy;
	Rmi::Stub stub;
	#endif
	Zero::CoreServerPtr	m_Core;

	#ifdef USE_EXAMPLE_ASYNC_QUERY
	Zero::AdoManager m_SQL;
	#endif
	
	bool Start(OUT Zero::CResultInfo &result)
	{
		m_Core = Zero::CoreServerPtr(Zero::ICoreServer::NewCore(this));
		#ifdef USE_RMI_OVERRIDE_MODE
		m_Core->Attach(&g_S2CProxy, &g_C2SStub);
		#else
		m_Core->Attach(&proxy, &stub);
		#endif
		

		Zero::CStartOption option;
		option.m_LogicThreadCount = 8;
		option.m_IoThreadCount = 8;

		return m_Core->Start(option, result);
	}

	void DisplayStatus();

	virtual void OnClientJoin(const Zero::RemoteID remote, const Zero::NetAddress& addr, Zero::ArrByte move_server, Zero::ArrByte move_param) override
	{
		printf_s("Join remote[%d]  addr[%s:%d]\n", remote, addr.m_ip.c_str(), addr.m_port);
	}
	virtual void OnClientLeave(const Zero::RemoteID remote, const bool bMoveServer) override
	{
		printf_s("Leave remote[%d]\n", remote);
	}

	virtual void OnInformation(const Zero::CResultInfo& info) override
	{
		printf_s("info : %s\n", CW2A(info.msg));
	}
	virtual void OnException(Zero::CException& e) override
	{
		printf_s("exception : %s\n", e.what());
	}


	// 연결복구 관련 이벤트


	#ifdef USE_EXAMPLE_ASYNC_QUERY
	void ExampleSQL_Open();
	void ExampleSQL_QueryAsync();
	void ExampleSQL_QueryAsyncRemote(Zero::RemoteID remote);	// 멀티 스레드일때 순서보장 샘플(remote단위)
	void Example_AsyncRequest();
	#endif

};


int _tmain(int argc, _TCHAR* argv[])
{
	CSampleServer Svr;

	Zero::CResultInfo result;
	if (Svr.Start(result))
	{
		printf_s("server start ok\n");
	}
	else
	{
		printf_s("server start error [%s]\n", CW2A(result.msg));
		_getch();
		return 0;
	}

	#ifndef USE_RMI_OVERRIDE_MODE
	Svr.stub.request_message = [&]Func_Rmi_request_message
	{
		printf_s("recved remote[%d] : %s\n", remote, CW2A(msg));
		Svr.proxy.reponse_message(remote, Zero::CPackOption::Basic, testClass, dic_test, msg);

		#ifdef USE_EXAMPLE_ASYNC_QUERY
		Svr.ExampleSQL_QueryAsyncRemote(remote);
		#endif
		return true;
	};
	#endif

	#ifdef USE_EXAMPLE_ASYNC_QUERY
	Svr.ExampleSQL_Open();
	Svr.ExampleSQL_QueryAsync();
	Svr.Example_AsyncRequest();
	#endif
	

	bool run_program = true;
	auto ReadLineAsync = [&run_program]()
	{
		Zero::StringA str;
		char aa[1024];
		std::cin >> aa;
		str = aa;
		return str;
	};

	auto fut = std::async(std::launch::async, ReadLineAsync);
	while (run_program)
	{
		if (fut._Is_ready())
		{
			auto cmd = fut.get();

			if (cmd=="/stat")
			{
				Svr.DisplayStatus();
			}
			else if (cmd=="/q")
			{
				Svr.m_Core->StopStart();
				run_program = false;
			}

			if (run_program)
				fut = std::async(std::launch::async, ReadLineAsync);
		}

		Sleep(10);
	}

	printf_s("Close...");

	Svr.m_Core->Stop();
	printf_s("Complete. press any key to exit\n");
	_getch();
	return 0;
}

void CSampleServer::DisplayStatus()
{
	Zero::CServerState status;
	m_Core->GetCurrentState(status);

	Zero::StringA temp;

	temp.Format(
        "[NetInfo]  Connect/Join %d(%d)/%d  Connect(Server) %d/%d  Accpet/Max %d/%d]\n",

        // 실제 연결된 client
        status.m_CurrentClient,

        // 연결복구 처리과정인 client
        status.m_RecoveryCount,

        // 서버에 입장완료상태의 client
        status.m_JoinedClient,

        // 서버간 direct p2p 연결된 server
        status.m_ServerP2PCount,

        // 서버간 direct p2p 연결 모니터링중인 server(서버간 연결 자동복구를 위한 모니터링)
        status.m_ServerP2PConCount,

        // 이 서버에 추가 연결 가능한 숫자
        status.m_nIoAccept,

        // 이 서버에 최대 연결 가능한 숫자
        status.m_MaxAccept
    );
	printf_s(temp);

	temp.Format(
        "[IO Info]  Close %d  Event %d  Recv %d  Send %d\n",

        // current io close
        status.m_nIoClose,

        // current io event
        status.m_nIoEvent,

        // current io recv socket
        status.m_nIoRecv,

        // current io send socket
        status.m_nIoSend
    );
    printf_s(temp);

	temp.Format(
        "[MemInfo]  Alloc/Instant[%d/%d], test[%s], EngineVersion[%d.%.4d]\n",

        // 미리 할당된 IO 메모리
        status.m_nAlloc,

        // 즉석 할당된 IO 메모리
        status.m_nAllocInstant,

        // test data
        status.m_test_data.c_str(),

        // Core버전
		m_Core->GetCoreVersion() / 10000,
		m_Core->GetCoreVersion() % 10000
    );
    printf_s(temp);


    // display thread information
	Zero::StringA strThr = "[ThreadInfo] (";
	intptr_t MaxDisplayThreadCount = status.m_arrThread.Count;;
    if (MaxDisplayThreadCount > 8)   // Limit max display thread information to 8
    {
		Zero::StringA tmp;
		tmp.Format("%d", MaxDisplayThreadCount);

        strThr += tmp;
        strThr += ") : ";
        MaxDisplayThreadCount = 8;
    }
    else
    {
		Zero::StringA tmp;
		tmp.Format("%d", MaxDisplayThreadCount);

        strThr += tmp;
        strThr += ") : ";
    }

    for (int i = 0; i < MaxDisplayThreadCount; i++)
    {
		Zero::StringA tmp;

        strThr += "[";
		tmp.Format("%d", status.m_arrThread[i].m_ThreadID);
        strThr += tmp;
        strThr += "/";
		tmp.Format("%d", status.m_arrThread[i].m_CountQueue);
        strThr += tmp;
        strThr += "/";
		tmp.Format("%d", status.m_arrThread[i].m_CountWorked);
        strThr += tmp;
        strThr += "] ";
    }
	strThr += "\n";
    printf_s(strThr);
}


#ifdef USE_EXAMPLE_ASYNC_QUERY
void CSampleServer::ExampleSQL_Open()
{
	Zero::StartOptionAdo start_option;
	Zero::String connectAddr;
	connectAddr.Format( L"%s,%d", L"127.0.0.1", 5000 );
	start_option.SetParam(
		connectAddr,	// SQL 주소
		L"hot",			// 계정
		L"pass123",		// 암호
		L"AdoTest",		// Catalog
		3,				// timeout시간(초)
		true,			// retry옵션
		20,				// 커넥션풀 사이즈
		L"SQLOLEDB.1"	// Provider(MS_SQL)
		//L"MSDASQL.1"	// Provider(MY_SQL)
	);
	try
	{
		Zero::AdoManager p(new Zero::CAdoManager(start_option));
		m_SQL = p;
		if( m_SQL->IsOpenFail() )
		{
			printf_s("sql open fail\n");
		}
		else
		{
			printf_s("sql open success.\n");
		}
	}
	catch(Zero::DBException& e)
	{
		printf_s("sql exception %s\n", e.what());
	}
}

void CSampleServer::Example_AsyncRequest()
{
	// 비동기 요청 처리 샘플
	m_Core->Async_Request(
		Zero::Remote_None,	// 특정 remote지정시 스레드 순서보장됨
		[=](void* &p)
	{
		int* a = new int;
		*a = 12345;
		p = (void*)a;
		printf_s("비동기 Request : input  addr(%d) value(%d)\n", a, *a);
	},
		[=](void* p)
	{
		int* r = (int*)p;
		printf_s("비동기 Request : result addr(%d) value(%d)\n", r, *r);

		SAFE_DELETE(r);
	});
}

void CSampleServer::ExampleSQL_QueryAsync()
{
	const TCHAR* request_query = _T("SELECT uuid FROM Userinfo");

	// 비동기 요청 시작
	bool bRequestOk = m_Core->Async_Query(
		Zero::RemoteID_Define::Remote_None,
		m_SQL,
		request_query, [=](bool bComplete, Zero::CAdo* pAdo, intptr_t counter)
	{
		// 비동기 결과 처리
		if (bComplete)
		{
			try
			{
				while (!pAdo->GetEndOfFile())
				{
					short cnt = 0;

					Zero::Guid outGuid;	// 읽어올 값
					DBFAIL_THROW(pAdo->GetFieldValue(cnt++, outGuid));
					printf_s("비동기 query 성공 : guid[%s]\n", CW2A(outGuid.ToStringW()));

					pAdo->MoveNext();
				}

				if (!pAdo->IsSuccess())
				{
					// 실패
				}
				return;
			}
			catch(Zero::DBException& e)
			{
				printf_s("비동기 query exception %s\n", e.what());
			}
		}
	});
	if (bRequestOk == false)
	{
		printf_s("비동기 query 요청 실패!  [%s]\n", CW2A(request_query));
	}
}

void CSampleServer::ExampleSQL_QueryAsyncRemote(Zero::RemoteID remote)
{
	// 비동기 요청(SP호출)
	m_Core->Async_SP(
		remote,					// 이렇게 특정 remote client의 ID 지정시 결과받는 시점에 계속 remote client가 접속중이면 스레드 순서가 보장된다
		m_SQL,
		_T("InsertUser"),
		[=](Zero::CAdo* pAdo)
	{
		// SP 파라미터 준비 : 이 블럭에서는 예외적으로 스레드 순서보장이 안됨으로 파라미터 설정외 다른 데이터에 접근하지 말것
		pAdo->CreateParam(_T("return"), adInteger, adParamReturnValue, 0);
		pAdo->CreateParam(_T("@v_name"), adVarChar, adParamInput, Zero::String(L"ID이름"));
		pAdo->CreateParam(_T("@v_pass"), adVarChar, adParamInput, Zero::String(L"이것은암호abc"));
	},
		[=](bool bComplete, Zero::CAdo* pAdo, intptr_t counter)
	{
		/**
		요청한 결과 받아서 처리하는 시점
		- 이때 remote번호의 client가 현재 접속중인지 한번 체크해줄 필요가 있음
		  비동기 호출이니깐 요청 시점에 접속중이라도 응답됬을때 연결상태아닐수 있으니까
		*/

		if (bComplete)
		{
			try
			{
				Zero::Guid outGuid;

				while (!pAdo->GetEndOfFile())
				{
					short cnt = 0;
					DBFAIL_THROW(pAdo->GetFieldValue(cnt++, outGuid));
					pAdo->MoveNext();
				}
				if (!pAdo->IsSuccess()) return;

				// 리턴값 읽어오기
				int nRtnParam = 0;
				pAdo->GetParam(_T("return"), nRtnParam);

				//// OUT파라미터 test
				//pAdo->GetParam(_T("@v_test"), test_int);

				// 성공 결과 보내주기
				/*if (아직 해당 client가 접속중인경우)
				{
					해당 Remote client에게 결과 전송
				}*/
				printf_s("비동기 sp처리 성공 remoteID[%d] outGuid[%s]\n", remote, outGuid.ToStringA().c_str());
				return;
			}
			catch(Zero::DBException& e)
			{
				printf_s("비동기 sp exception : %s\n", e.what());
			}
		}

		printf_s("비동기 sp처리 실패 remoteID[%d]\n", remote);
	});
}
#endif