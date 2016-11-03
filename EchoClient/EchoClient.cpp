#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <future>
#include <functional>

#include "../EchoCommon/echo_proxy.h"
#include "../EchoCommon/echo_proxy.cpp"
#include "../EchoCommon/echo_stub.h"
#include "../EchoCommon/echo_stub.cpp"


class CEchoClient : public Zero::IEventClient
{
public:
	Rmi::Proxy proxy;
	Rmi::Stub stub;
	Zero::ICoreClientPtr m_Core;


	CEchoClient()
	{
		m_Core = Zero::ICoreClientPtr(Zero::ICoreClient::NewCore(this));
		m_Core->Attach(&proxy, &stub);
	}
	
	virtual void OnJoin(const Zero::ConnectionInfo& info) override
	{
		printf_s("Joined server.\n");
	}
	virtual void OnLeave(const Zero::ConnectionInfo& info) override
	{
		printf_s("Leave server\n");
	}

	virtual void OnInformation(const Zero::CResultInfo& info) override
	{
		printf_s("info : %s\n", CW2A(info.msg.c_str()));
	}
	virtual void OnException(Zero::CException& e) override
	{
		printf_s("exception : %s\n", e.what());
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	CEchoClient echo_client;

	if (echo_client.m_Core->Connect(
		L"127.0.0.1",
		20000,
		0,
		0,
		false
		))
	{
		printf_s("connect server ok.\n");
	}
	else
	{
		printf_s("connect server fail\n");
		_getch();
		return 0;
	}

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

			if (cmd =="/q")
			{
				echo_client.m_Core->Leave();	// 퇴장
			}
			else if (cmd =="/exit")
			{
				run_program = false;

				// 아직 fut가 실행중인경우 종료를 위한 처리(아무거나 입력을 받는다)
				if (!fut._Is_ready())
				{
					printf_s("input any string to exit\n");
					fut.wait();
					return 0;
				}
			}
			else
			{
				RemoteClass::CUserClass _testClass;
				Zero::CDictionary<Int32,Int32> _testDic;
				_testDic.Add(12345,54321);

				// 패킷보내고
				echo_client.proxy.request_Echo(Zero::Remote_Server, Zero::CPackOption::Basic, _testClass, _testDic, Zero::ZA2W(cmd));

				// 응답받기
				echo_client.stub.reponse_Echo = []Func_Rmi_reponse_Echo
				{
					printf_s("receved : %s\n", CW2A(msg));
					return true;
				};
			}

			if (run_program)
				fut = std::async(std::launch::async, ReadLineAsync);
		}

		echo_client.m_Core->NetLoop();
		Sleep(10);
	}

	printf_s("press any key to exit\n");
	_getch();
	return 0;
}

