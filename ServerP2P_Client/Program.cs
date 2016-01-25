using System;
using System.Collections.Generic;
using System.Threading.Tasks;

/// <summary>
/// 서버 이동 자동화 기능 샘플 : 복잡한 서버간 이동을 간단히 처리
/// </summary>
namespace ServerP2P_Client
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


            // 반자동으로 서버이동 처리하기 : 선택된 서버의 주소 정보를 받아 직접적으로 서버 주소를 넣어 이동하기
            stub.reponse_move_to_server = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, bool result, ZNet.NetAddress addr) =>
            {
                if (result)
                {
                    Console.WriteLine("Manual move server to : {0}:{1}", addr.m_ip, addr.m_port);

                    /**
                    address를 넣어 직접적인 서버이동 요청 (MoveToServer로 서버이동시 처리되는 과정)
                    - 서버 내부적으로 암호화된 128비트 인증키를 발급하여 목표 서버로 이동시점에 인증키 검증하여 완료처리
                    - 모든 과정은 자동으로 진행되며 예외상황으로 서버이동이 실패하는 경우 move_fail_handler 이벤트가 발생합니다
                    */
                    m_Core.MoveToServer(addr);
                }
                else
                {
                    Console.WriteLine("Manual move server fail");
                }
                return true;
            };

            // MoveToServer에서 서버타입을 넣어 전자동으로 서버이동을 시도한경우의 실패 이벤트
            m_Core.move_fail_handler = () =>
            {
                Console.WriteLine("Auto move server fail!");
            };

            m_Core.server_join_handler = (ZNet.ConnectionInfo info) =>
            {
                Console.WriteLine("서버입장성공 [{0}:{1}] - 서버이동으로 입장한경우[{2}]", info.addr.m_ip, info.addr.m_port, info.moved);
            };

            m_Core.server_leave_handler = (ZNet.ConnectionInfo info) =>
            {
                Console.WriteLine("서버퇴장성공 [{0}:{1}] - 서버이동으로 입장한경우[{2}]", info.addr.m_ip, info.addr.m_port, info.moved);
            };

            m_Core.message_handler = (ZNet.ResultInfo result) =>
            {
                string str_msg = "Msg : ";
                str_msg += result.msg;
                Console.WriteLine(str_msg);
            };
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            CClient Client = new CClient();

            Client.m_Core.Connect(ServerP2P_Common.Join.ipaddr, ServerP2P_Common.Join.portnum, ServerP2P_Common.Join.protocol_ver);

            DisplayHelpCommand();

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

                        case "/h":
                            DisplayHelpCommand();
                            break;


                        // 자동으로 서버이동 처리하기 : 요청 즉시 자동으로 해당타입의 서버로 이동(자동 로드벨런싱 처리)
                        case "/maina":
                            Client.m_Core.MoveToServer((int)ServerP2P_Common.Server.Main);
                            break;

                        // 자동으로 서버이동 처리하기 : 요청 즉시 자동으로 해당타입의 서버로 이동(자동 로드벨런싱 처리)
                        case "/logina":
                            Client.m_Core.MoveToServer((int)ServerP2P_Common.Server.Login);
                            break;


                        // 반자동으로 서버이동 처리하기 : 서버이동 과정에 부가적인 처리할 내용이 있을경우
                        case "/main":
                            Client.proxy.request_move_to_server(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, (int)ServerP2P_Common.Server.Main);
                            break;

                        // 반자동으로 서버이동 처리하기 : 서버이동 과정에 부가적인 처리할 내용이 있을경우
                        case "/login":
                            Client.proxy.request_move_to_server(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, (int)ServerP2P_Common.Server.Login);
                            break;

                        default:
                            // packet send test
                            Client.proxy.request_message(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Encrypt, ret.Result);

                            Client.stub.reponse_message = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg) =>
                            {
                                Console.WriteLine(string.Format("recv : {0}", msg));
                                return true;
                            };
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

        static public void DisplayHelpCommand()
        {
            //Console.WriteLine("/Cmd:  h(Help) q(Quit) exit(ProgramExit)");
            Console.WriteLine("ManualMoveLogin : /login    ManualMoveMain : /main");
            Console.WriteLine("AutoMoveLogin   : /logina   AutoMoveMain   : /maina");
        }
    }
}
