using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/// <summary>
/// [EchoServer와 모두 동일하지만 udp설정법을 보여주는 서버 예제]
/// 
/// 최초 Client가 서버로 Connect할때 udp 포트를 지정하면 udp가 활성화 됩니다
/// 
/// 패킷 송신시 udp사용 여부는 pkOption에 지정하는 것으로 결정됩니다
/// 
/// udp는 전달이 보장되지 않기때문에 손실되어도 상관없는 경우에만 사용해야합니다
/// 
/// 
/// </summary>
namespace UdpServer
{
    public class UdoServer
    {
        public ZNet.CoreServerNet m_Core = new ZNet.CoreServerNet();

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public UdoServer()
        {
            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);

            // 받은 프로토콜 옵션 그대로 돌려준다 : pkOption을 그대로 사용함
            stub.request_Udp = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg) =>
            {
                if (pkOption.m_protocol == ZNet.Protocol8.Protocol_Udp)
                    Console.WriteLine("Remote[{0}] msg(udp) : {1}", remote, msg);
                else
                    Console.WriteLine("Remote[{0}] msg(tcp) : {1}", remote, msg);

                proxy.reponse_Udp(remote, pkOption, msg);
                return true;
            };

            // udp ready 이벤트
            m_Core.ready_udp_handler = (ZNet.RemoteID remote, bool bTrust) =>
            {
                Console.WriteLine("ready udp remote {0}   trust {1}", remote, bTrust);
            };


            m_Core.client_join_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr, ZNet.ArrByte move_server, ZNet.ArrByte move_param) =>
            {
                Console.WriteLine("Client {0} is Join {1}:{2}.\n", remote, addr.m_ip, addr.m_port);
            };

            m_Core.client_leave_handler = (ZNet.RemoteID remote, bool bMoveServer) =>
            {
                Console.WriteLine("Client {0} Leave.\n", remote);
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
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            UdoServer Svr = new UdoServer();

            // udp 관련 옵션 설정을 위해 : StartOption의 디폴트 값을 그대로 사용해도 되지만 세부설정을 위해 별도 선언
            ZNet.StartOption op = new ZNet.StartOption();

            op.m_UdpPorts = new int[2]; // 서버에서 사용할 udp 포트 개수
            op.m_UdpPorts[0] = 20004;   // udp 포트 설정
            op.m_UdpPorts[1] = 20005;   // udp 포트 설정

            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (Svr.m_Core.Start(op, outResult))
            {
                Console.WriteLine("Start Server ok. ver[{0}.{1:0000}]\n", Svr.m_Core.GetCoreVersion() / 10000, Svr.m_Core.GetCoreVersion() % 10000);
                Console.WriteLine("/Cmd:  q(Quit) h(Help) stat(status info)\n");
            }
            else
            {
                Console.WriteLine("Error Start...  : {0} \n", outResult.msg);
                System.Threading.Thread.Sleep(1000 * 4);
                Svr.m_Core.Dispose();
                return;
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
                            Console.WriteLine("/Cmd:  q(Quit) h(Help) stat(status info)");
                            break;

                        case "/stat":
                            ZNet.ServerState status;
                            Svr.m_Core.GetCurrentState(out status);
                            Console.WriteLine(string.Format("[NetInfo]  Joined Client = {0}\n", status.m_JoinedClient));
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
    }
}
