using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CasualClient
{
    public class CClient
    {
        public ZNet.CoreClientNet m_Core;

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public CasualCommon.Server server_now = CasualCommon.Server.Login;  // 현재 서버 위치
        public CasualCommon.Server server_tag = CasualCommon.Server.Login;  // 목표 서버 위치


        // 갱신된 로비 리스트
        public Dictionary<ZNet.RemoteID, ZNet.MasterInfo> lobbys;


        public CClient()
        {
            m_Core = new ZNet.CoreClientNet();

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);


            stub.Chat = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg) =>
            {
                Console.WriteLine(string.Format("recv : {0}", msg));
                return true;
            };

            stub.notify_lobby_list = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, Dictionary<ZNet.RemoteID, ZNet.MasterInfo> lobby_list) =>
            {
                lobbys = lobby_list;
                foreach (var obj in lobby_list)
                {
                    Console.WriteLine(string.Format("lobby : {0}", obj.Value.m_Description));
                }
                return true;
            };


            // 서버이동 시도의 실패 이벤트 처리
            m_Core.move_fail_handler = () =>
            {
                Console.WriteLine("Auto move server fail!");
            };

            m_Core.server_join_handler = (ZNet.ConnectionInfo info) =>
            {
                if (info.moved)
                {
                    // 서버이동이 성공한 시점 : 위치를 목표했던 서버로 설정
                    server_now = server_tag;
                    Console.WriteLine("서버이동성공 [{0}:{1}] {2}", info.addr.m_ip, info.addr.m_port, server_now);
                }
                else
                {
                    // 최초 입장의 성공시점 : 위치를 로그인 서버로 설정
                    server_now = CasualCommon.Server.Login;
                    Console.WriteLine("서버입장성공 {0}", server_now);
                }
                Console.WriteLine("({0}) {1}", this.server_now, HelpCommand(this.server_now));
            };
            m_Core.server_leave_handler = (ZNet.ConnectionInfo info) =>
            {
                if (info.moved)
                    Console.WriteLine("서버이동을 위해 퇴장, 이동할서버 [{0}:{1}]", info.addr.m_ip, info.addr.m_port);
                else
                    Console.WriteLine("서버퇴장성공");
            };

            m_Core.message_handler = (ZNet.ResultInfo result) =>
            {
                string str_msg = "Msg : ";
                str_msg += result.msg;
                Console.WriteLine(str_msg);
            };
        }

        public string HelpCommand(CasualCommon.Server pos)
        {
            switch (pos)
            {
                case CasualCommon.Server.Login:
                    return "로비이동: /lobby   로비목록갱신 /refresh";

                case CasualCommon.Server.Lobby:
                    return "로그인서버: /login  방목록: /list  방만들기: /make  방입장: /번호";

                case CasualCommon.Server.Room:
                    return "방이다";

                default:
                    return "정의되지않음";
            }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            CClient Client = new CClient();

            Client.m_Core.Connect(CasualCommon.Join.ipaddr, CasualCommon.Join.portnum, CasualCommon.Join.protocol_ver);
            Console.WriteLine("Connect to server ({0}:{1}) ...", CasualCommon.Join.ipaddr, CasualCommon.Join.portnum);


            // 최초 서버 접속과 입장완료까지 확인하고 넘어가는 부분
            bool connect_fail = false;
            Client.m_Core.server_connect_result_handler = (bool isConnectSuccess) =>
            {
                if (isConnectSuccess == false)
                    connect_fail = true;
            };
            while (true)
            {
                Client.m_Core.NetLoop();
                System.Threading.Thread.Sleep(10);

                if (Client.m_Core.IsJoined()) break;
                if (connect_fail)
                {
                    Console.WriteLine("Connect FAIL");
                    System.Threading.Thread.Sleep(3000);
                    return;
                }
            }


            CasualCommon.Common.DisplayHelpCommand();
            Console.WriteLine("current({0})", Client.server_now);

            var ret = CasualCommon.Common.ReadLineAsync();
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

                        case "/h":
                            CasualCommon.Common.DisplayHelpCommand();
                            break;

                        case "/refresh":
                            if (Client.server_now == CasualCommon.Server.Login)
                            {
                                Client.proxy.request_lobby_list(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic);
                            }
                            break;

                        case "/lobby":
                            if (Client.server_now == CasualCommon.Server.Login)
                            {
                                // 일단 간단한 처리를 위해 첫번째 로비서버를 골라서 들어가도록 처리
                                Client.server_tag = CasualCommon.Server.Lobby;
                                foreach (KeyValuePair<ZNet.RemoteID, ZNet.MasterInfo> obj in Client.lobbys)
                                {
                                    Client.proxy.request_go_lobby(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, obj.Value.m_Description);
                                    break;
                                }
                            }
                            else if (Client.server_now == CasualCommon.Server.Room)
                            {
                                // 현재 룸서버인경우 원래의 로비로 나가기만 가능
                                Client.server_tag = CasualCommon.Server.Lobby;
                                Client.m_Core.MoveToServer((int)Client.server_tag); // 일단 아무 로비로든 그냥 보내버린다
                            }
                            break;

                        case "/login":
                            if (Client.server_now == CasualCommon.Server.Lobby)
                            {
                                // 현재 로비서버인 경우 로그인서버로 이동 요청
                                Client.server_tag = CasualCommon.Server.Login;
                                Client.m_Core.MoveToServer((int)Client.server_tag);
                            }
                            break;

                        case "/join":
                            if (Client.server_now == CasualCommon.Server.Lobby)
                            {
                                Client.server_tag = CasualCommon.Server.Room;
                                Client.proxy.request_join_room(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, new Guid());
                            }
                            break;

                        case "/make":
                            if (Client.server_now == CasualCommon.Server.Lobby)
                            {
                                Client.server_tag = CasualCommon.Server.Room;
                                Client.proxy.request_make_room(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, "방이름AZ");
                            }
                            break;

                        case "/out":
                            if (Client.server_now == CasualCommon.Server.Room)
                            {
                                Client.server_tag = CasualCommon.Server.Lobby;
                                Client.proxy.request_out_room(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic);
                            }
                            break;

                        default:
                            Client.proxy.Chat(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, ret.Result);
                            Console.WriteLine("({0}) {1}", Client.server_now, Client.HelpCommand(Client.server_now));
                            break;
                    }

                    if (run_program)
                        ret = CasualCommon.Common.ReadLineAsync();
                }

                Client.m_Core.NetLoop();
                System.Threading.Thread.Sleep(1);
            }

            Client.m_Core.Destroy();
            System.Threading.Thread.Sleep(1000 * 2);
        }
    }
}
