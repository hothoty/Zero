using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UdpClient
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

            m_Core.ready_udp_handler = (bool bTrust) =>
            {
                /**
                bTrust값이 false인 경우 모바일모드이거나 udp수신이 안정적이지 않은 경우입니다
                어쨋든 ready상태이므로 조금 불안정하긴 하지만 udp패킷송수신이 가능하며 서버에서 지속적으로 상태를
                체크하여 자동 관리함에 따라 일시적으로 손실되어도 신경쓸필요 없습니다(자동 홀펀칭)
                */
                Console.WriteLine("ReadyUdp trust {0}", bTrust);
            };
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            // udp 송신 모드
            bool mode_send_udp = false;

            CClient Client = new CClient();

            Client.m_Core.Connect(
                "127.0.0.1",
                20000,
                0,          // protocol version
                77,         // udp recve port : if 0 = disable
                false,      // mobile option (모바일 클라이언트인 경우 true사용 권장)
                false       // enable auto connection recovery mode
                );

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

                        case "/h":
                            Console.WriteLine("/Cmd:  h(Help) q(Quit) u(udp toggle) exit(ProgramExit)");
                            break;

                        case "/u":
                            // udp 모드 토글
                            mode_send_udp = !mode_send_udp;
                            Console.WriteLine("Current mode udp is {0}", mode_send_udp);
                            break;

                        default:
                            // 패킷 보내기
                            if (mode_send_udp)
                                Client.proxy.request_Udp(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Udp, ret.Result);
                            else
                                Client.proxy.request_Udp(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, ret.Result);

                            // 패킷 받을때 처리
                            Client.stub.reponse_Udp = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg) =>
                            {
                                if (pkOption.IsUdp())
                                    Console.WriteLine(string.Format("recv(udp) : {0}", msg));
                                else
                                    Console.WriteLine(string.Format("recv(tcp) : {0}", msg));
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
    }
}
