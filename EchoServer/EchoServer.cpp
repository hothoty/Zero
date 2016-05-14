#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <future>
#include <functional>

#include "../EchoCommon/echo_proxy.h"
#include "../EchoCommon/echo_proxy.cpp"
#include "../EchoCommon/echo_stub.h"
#include "../EchoCommon/echo_stub.cpp"


class CEchoServer : public Zero::IEventServer
{
public:
	Rmi::Proxy proxy;
	Rmi::Stub stub;
	Zero::CoreServerPtr	m_Core;

	bool Start(OUT Zero::CResultInfo &result)
	{
		m_Core = Zero::CoreServerPtr(Zero::ICoreServer::NewCore(this));
		m_Core->Attach(&proxy, &stub);

		return m_Core->Start(Zero::CStartOption(), result);
	}

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

};

int _tmain(int argc, _TCHAR* argv[])
{
	CEchoServer echo_server;

	Zero::CResultInfo result;
	if (echo_server.Start(result))
	{
		printf_s("server start ok\n");
	}
	else
	{
		printf_s("server start error [%s]\n", CW2A(result.msg));
		_getch();
		return 0;
	}

	// 패킷 받아서 바로 돌려주기
	echo_server.stub.request_Echo = [&echo_server]Func_Rmi_request_Echo
	{
		printf_s("recved remote[%d] : %s\n", remote, CW2A(msg));
		echo_server.proxy.reponse_Echo(remote, Zero::CPackOption::Basic, testClass, dic_test, msg);
		return true;
	};

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
				Zero::CServerState status;
				echo_server.m_Core->GetCurrentState(status);
				printf_s("[NetInfo]  Joined Client = %d\n", status.m_JoinedClient);
			}
			else if (cmd=="/q")
			{
				echo_server.m_Core->StopStart();
				run_program = false;
			}

			if (run_program)
				fut = std::async(std::launch::async, ReadLineAsync);
		}

		Sleep(10);
	}

	printf_s("Close...");

	echo_server.m_Core->Stop();
	printf_s("Complete. press any key to exit\n");
	_getch();
	return 0;
}


