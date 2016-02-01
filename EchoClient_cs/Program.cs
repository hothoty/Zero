using System;
using System.Collections.Generic;
using System.Threading.Tasks;

/**
 * 
 * EchoClient
 * 
 * - 멀티플렛폼용 ZNet.dll사용 (윈도우용 ZNetW.dll의 경량화 버전)
 * 
 * - 일부 기능이 제한되나 거의 동일하게 사용가능
 * 
 * */

namespace EchoClient
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
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            CClient Client = new CClient();

            Client.m_Core.Connect("127.0.0.1", 20000, 0);
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
                            Console.WriteLine("/Cmd:  h(Help) q(Quit) exit(ProgramExit)");
                            break;

                        default:
                            // 보낼 패킷 데이터 준비
                            RemoteClass.CUserClass _testClass = new RemoteClass.CUserClass();
                            Dictionary<Int32, Int32> _testDic = new Dictionary<Int32, Int32>();
                            _testDic.Add(1212345, 1230);

                            // 패킷 보내고
                            Client.proxy.request_Echo(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Encrypt, _testClass, _testDic, ret.Result);

                            // 패킷 받을때 처리
                            Client.stub.reponse_Echo = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32, Int32> dic_test, string msg) =>
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
    }
}
