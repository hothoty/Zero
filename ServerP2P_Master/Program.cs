using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/// <summary>
/// 분산서버 구현시 모든 서버들의 P2P연결을 자동관리하기 위한 마스터 서버 샘플
/// 
/// - 모든 서버들은 마스터 서버로 접속하는 순간 자동으로 서버간의 Direct P2P연결이 시작됩니다
/// 
/// - 따라서 마스터 서버는 단 1개만 실행해야합니다
/// 
/// </summary>
namespace ServerP2P_Master
{
    public class MasterServer
    {
        public ZNet.CoreServerNet m_Core = null;

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public MasterServer()
        {
            // 생성자에 true 입력 -> 마스터 서버 Core생성
            m_Core = new ZNet.CoreServerNet(true);

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);

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

            // 마스터 서버에서만 발생되는 이벤트 처리 : 마스터 클라이언트 서버 입장 시점
            m_Core.master_server_join_handler = (ZNet.RemoteID remote, string description, int type, ZNet.NetAddress addr) =>
            {
                Console.WriteLine(string.Format("마스터 Client Join remoteID({0}) {1} type({2})", remote, description, type));
            };

            // 마스터 서버에서의 접속해제 이벤트 -> 마스터 클라이언트의 퇴장
            m_Core.client_disconnect_handler = (ZNet.RemoteID remote) =>
            {
                Console.WriteLine(string.Format("마스터 Client Leave remoteID({0})", remote));
            };
        }
    }


    class Program
    {
        static void Main(string[] args)
        {
            MasterServer Svr = new MasterServer();

            ZNet.StartOption param = new ZNet.StartOption();
            param.m_IpAddressListen = ServerP2P_Common.MasterServerConnect.master_ipaddr;
            param.m_PortListen = ServerP2P_Common.MasterServerConnect.master_portnum;
            param.m_MaxConnectionCount = 2000;

            Svr.m_Core.SetKeepAliveOption(60);

            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (Svr.m_Core.Start(param, outResult))
            {
                Console.WriteLine("MasterServer start ok.\n");
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
