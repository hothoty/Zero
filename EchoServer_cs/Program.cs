using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace EchoServer_cs
{
    public class EchoServer
    {
        public ZNet.CoreServerNet m_Core = new ZNet.CoreServerNet();

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public EchoServer()
        {
            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);

            stub.request_Echo = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32, Int32> dic_test, string msg) =>
            {
                Console.WriteLine("Remote[{0}] msg : {1}", remote, msg);
                proxy.reponse_Echo(remote, ZNet.CPackOption.Basic, testClass, dic_test, msg);
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
            EchoServer Svr = new EchoServer();

            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (Svr.m_Core.Start(new ZNet.StartOption(), outResult))
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
