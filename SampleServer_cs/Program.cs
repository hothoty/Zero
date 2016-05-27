using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/// <summary>
/// 다양한 기능들을 보여주는 샘플서버
/// </summary>
namespace SampleServer_cs
{
    public class SampleServer
    {
        public ZNet.CoreServerNet m_Core = null;

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public SampleServer()
        {
            m_Core = new ZNet.CoreServerNet();

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);

            stub.request_message = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32, Int32> dic_test, string msg) =>
            {
                Console.WriteLine("Remote[{0}] msg : {1}", remote, msg);
                proxy.reponse_message(remote, ZNet.CPackOption.Basic, testClass, dic_test, msg);
                return true;
            };

            m_Core.client_join_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr, ZNet.ArrByte move_server, ZNet.ArrByte move_param) =>
            {
                Console.WriteLine("Client {0} is Join {1}:{2}.\n", remote, addr.m_ip, addr.m_port);
            };

            m_Core.client_leave_handler = (ZNet.RemoteID remote, bool bMoveServer) =>
            {
                Console.WriteLine("Client {0} Leave.\n", remote);
            };


            // 주기적으로 처리할 내용 : m_UpdateTimeMs 에서 설정한 시간마다 콜백됩니다
            m_Core.update_event_handler = () =>
            {
                Console.WriteLine("Server Update Event\n");
            };


            m_Core.message_handler = (ZNet.ResultInfo result) =>
            {
                string str_msg = "Msg : ";
                str_msg += result.msg;
                Console.WriteLine(str_msg);
            };

            m_Core.exception_handler = (Exception e) =>
            {
                string str_msg = "Exception : ";
                str_msg += e.ToString();
                Console.WriteLine(str_msg);
            };


            // 연결복구 관련 이벤트
            m_Core.recovery_info_handler = (ZNet.RemoteID remoteNew, ZNet.RemoteID remoteTo) =>
            {
                Console.WriteLine("Recovery try... new connection Client[{0}] to Client[{1}].\n", remoteNew, remoteTo);
            };
            m_Core.recovery_start_handler = (ZNet.RemoteID remote) =>
            {
                Console.WriteLine("Recovery Start Client {0}.\n", remote);
            };
            m_Core.recovery_end_handler = (ZNet.RemoteID remote, ZNet.NetAddress addrNew, bool bTimeOut) =>
            {
                if (bTimeOut)
                    Console.WriteLine("Recovery TimeOUT Client {0}.\n", remote);
                else
                    Console.WriteLine("Recovery Complete Client {0}.  NewAddr[{1}:{2}]\n", remote, addrNew.m_ip, addrNew.m_port);
            };

            // 서버 접속제한시점의 이벤트
            m_Core.limit_connection_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr) =>
            {
                Console.WriteLine("Client {0}, {1} is Leave.\n", remote, addr.m_ip, addr.m_port);
            };
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            UInt16 portnum = 20000;
            string ipaddr = "127.0.0.1";

            SampleServer Svr = new SampleServer();

            ZNet.StartOption param = new ZNet.StartOption();
            param.m_IpAddressListen = ipaddr;
            param.m_PortListen = portnum;

            // 최대 동접 숫자 설정
            param.m_MaxConnectionCount = 5000;

            // Update Event 시간 설정
            param.m_UpdateTimeMs = 5000;


            // 연결 유지 시간 설정(초단위)
            Svr.m_Core.SetKeepAliveOption(10);

            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (Svr.m_Core.Start(param, outResult))
            {
                Console.WriteLine("Server Start ok. ver[{0}.{1:0000}]\n", Svr.m_Core.GetCoreVersion() / 10000, Svr.m_Core.GetCoreVersion() % 10000);
                DisplayHelpCommand();
            }
            else
            {
                Console.WriteLine("Start error : {0} \n", outResult.msg);
            }

            var ret = ReadLineAsync();
            bool run_program = true;
            while (run_program)
            {
                if (ret.IsCompleted)
                {
                    switch (ret.Result)
                    {
                        case "/h":
                            DisplayHelpCommand();
                            break;

                        case "/stat":
                            DisplayStatus(Svr.m_Core);
                            break;

                        case "/q":
                            Console.WriteLine("quit Server...");
                            run_program = false;
                            break;
                    }

                    if (run_program)
                        ret = ReadLineAsync();
                }

                System.Threading.Thread.Sleep(10);
            }

            Console.WriteLine("Start Closing...  ");
            Svr.m_Core.Dispose();
            Console.WriteLine("Close complete.");

            System.Threading.Thread.Sleep(1000 * 2);
        }

        static async Task<string> ReadLineAsync()
        {
            var line = await Task.Run(() => Console.ReadLine());
            return line;
        }

        static void DisplayHelpCommand()
        {
            Console.WriteLine("/Cmd:  q(Quit) h(Help) stat(status info)");
        }

        static void DisplayStatus(ZNet.CoreServerNet svr)
        {
            ZNet.ServerState status;
            svr.GetCurrentState(out status);


            // 기본 정보
            Console.WriteLine(string.Format(
                "[NetInfo]  Connect/Join {0}({1})/{2}  Connect(Server) {3}/{4}  Accpet/Max {5}/{6}",

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
                ));


            // 엔진 내부에서 작업중인 IO 관련 상태 정보
            Console.WriteLine(string.Format(
                "[IO Info]  Close {0}  Event {1}  Recv {2}  Send {3}",

                // current io close
                status.m_nIoClose,

                // current io event
                status.m_nIoEvent,

                // current io recv socket
                status.m_nIoRecv,

                // current io send socket
                status.m_nIoSend
            ));


            // 엔진 메모리 관련 사용 정보
            Console.WriteLine(string.Format(
                "[MemInfo]  Alloc/Instant[{0}/{1}], test[{2}], EngineVersion[{3}.{4:0000}] ",

                // 미리 할당된 IO 메모리
                status.m_nAlloc,

                // 즉석 할당된 IO 메모리
                status.m_nAllocInstant,

                // test data
                status.m_test_data,

                // Core버전
                svr.GetCoreVersion() / 10000,
                svr.GetCoreVersion() % 10000
            ));


            // 스레드 정보
            string strThr = "[ThreadInfo] (";
            int MaxDisplayThreadCount = status.m_arrThread.Count();
            if (MaxDisplayThreadCount > 8)   // 화면이 복잡하니까 그냥 최대 8개까지만 표시
            {
                strThr += MaxDisplayThreadCount;
                strThr += ") : ";
                MaxDisplayThreadCount = 8;
            }
            else
            {
                strThr += MaxDisplayThreadCount;
                strThr += ") : ";
            }

            for (int i = 0; i < MaxDisplayThreadCount; i++)
            {
                strThr += "[";
                strThr += status.m_arrThread[i].m_ThreadID;     // 스레드ID
                strThr += "/";
                strThr += status.m_arrThread[i].m_CountQueue;   // 처리 대기중인 작업
                strThr += "/";
                strThr += status.m_arrThread[i].m_CountWorked;  // 처리된 작업(누적)
                strThr += "] ";
            }
            Console.WriteLine(strThr);
        }
    }
}
