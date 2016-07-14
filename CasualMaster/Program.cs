using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/// <summary>
/// 마스터 서버
/// 
/// - 로그인 서버
/// 
/// - 로비서버
/// 
/// - 룸서버
/// 
/// </summary>
namespace CasualMaster
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
            param.m_IpAddressListen = CasualCommonSvr.MasterServerConnect.master_ipaddr;
            param.m_PortListen = CasualCommonSvr.MasterServerConnect.master_portnum;
            param.m_MaxConnectionCount = 5000;

            Svr.m_Core.SetKeepAliveOption(60);

            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (Svr.m_Core.Start(param, outResult))
            {
                Console.WriteLine("MasterServer start ok.\n");
                DisplayHelpCommand();

                // 테스트 위해 서버들 자동실행 (로그인1개, 로비2개, 룸서버2개)
                System.Threading.Thread.Sleep(1000);
                System.Diagnostics.Process.Start("CasualLogin.exe");
                System.Diagnostics.Process.Start("CasualLobby.exe");
                //System.Diagnostics.Process.Start("CasualLobby.exe", "22005");
                System.Diagnostics.Process.Start("CasualRoom.exe");
                //System.Diagnostics.Process.Start("CasualRoom.exe", "25010");
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
                            CasualCommonSvr.Common.DisplayStatus(Svr.m_Core);
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
    }
}
