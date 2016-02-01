using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GroupClient
{
    public class CClient
    {
        public ZNet.CoreClientNet m_Core;

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public CClient()
        {
            m_Core = new ZNet.CoreClientNet();

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);

            m_Core.server_join_handler = (ZNet.ConnectionInfo info) =>
            {
                Console.WriteLine("JoinOK");
            };
            m_Core.server_leave_handler = (ZNet.ConnectionInfo info) =>
            {
                Console.WriteLine("Leave");
            };

            // 그룹 이벤트
            m_Core.group_join_handler = (bool bJoin, ZNet.RemoteID groupID, ZNet.RemoteID memberID, int memberCount) =>
            {
                if (bJoin)
                    Console.WriteLine(string.Format("Group Join : groupID {0}, memberID {1}, membercount {2}", groupID, memberID, memberCount));
                else
                    Console.WriteLine(string.Format("Group Out  : groupID {0}, memberID {1}, membercount {2}", groupID, memberID, memberCount));
            };

            // 메세지 처리
            stub.Chat = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg) =>
            {
                Console.WriteLine(string.Format("recv : {0}", msg));
                return true;
            };
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            CClient Client = new CClient();

            Client.m_Core.Connect("127.0.0.1", 20000, 0);

            Console.WriteLine("/Cmd:  h(Help) q(Quit) c(reconnect) exit(ProgramExit)");
            Console.WriteLine("그룹테스트:  /make(생성/합류) /destroy(파괴) /leave(탈퇴) /msg(그룹채팅) \n");
            Console.Write("> ");

            var ret = ReadLineAsync();
            bool run_program = true;
            while (run_program)
            {
                if (ret.IsCompleted)
                {
                    switch (ret.Result)
                    {
                        case "/q":
                            Client.m_Core.Leave();
                            break;

                        case "/exit":
                            run_program = false;
                            break;

                        case "/c":
                            Client.m_Core.ReConnect();
                            break;

                        case "/make":       // 그룹생성/가입
                        case "/destroy":    // 그룹해체
                        case "/leave":      // 그룹탈퇴
                        case "/msg":        // 그룹메세지
                            Client.proxy.GroupTestCommand(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, ret.Result);
                            break;

                        default:
                            Client.proxy.Chat(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, ret.Result);
                            break;
                    }

                    if (run_program)
                        ret = ReadLineAsync();
                }

                Client.m_Core.NetLoop();
                System.Threading.Thread.Sleep(1);
            }

            Client.m_Core.Destroy();
            System.Threading.Thread.Sleep(1000 * 2);
        }

        static async Task<string> ReadLineAsync()
        {
            var line = await Task.Run(() => Console.ReadLine());
            return line;
        }
    }
}
