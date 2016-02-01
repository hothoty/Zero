using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/**
 * 
 * SampleClientW : 연결장애 및 모바일 네트워크 전환시 연결 복구 기능 활용 샘플
 * 
 * - Window용 ZNetW.dll사용
 * 
 * - (멀티플렛폼용dll => ZNet.dll)
 * 
 * */

namespace SampleClientW_cs
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

            m_Core.recovery_start_handler = () =>
            {
                Console.WriteLine("Recovery Connection Start...");
            };

            m_Core.recovery_end_handler = () =>
            {
                Console.WriteLine("Recovery Connection Complete.");
            };

            m_Core.server_connect_result_handler = (bool isConnectSuccess) =>
            {
                if (isConnectSuccess)
                {
                    Console.WriteLine("Connected!");
                    Program.DisplayHelpCommand();
                }
                else
                {
                    Console.WriteLine("Connect Fail!");
                }
            };

            m_Core.server_limit_handler = () =>
            {
                Console.WriteLine("Limit connection from server");
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
            Console.WriteLine("연결복구테스트방법 : '/k'명령으로 강제연결종료시 일정시간 이후 자동연결복구됨(연결죽인상태의 채팅내용모두 연결복구시점에 복구되는것을 확인 가능) 또한 'f'명령으로 현재 상태에서 즉각적인 연결복구를 테스트해볼수도 있습니다\n");

            CClient Client = new CClient();

            Client.m_Core.Connect(
                "127.0.0.1",
                20000,
                0,          // protocol version
                55,         // udp recve port : if 0 = disable
                false,      // mobile optimize option
                true//false       // enable auto connection recovery mode
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
                            // leave from server
                            Client.m_Core.Leave();
                            break;

                        case "/c":
                            Console.WriteLine("reconnect try...");
                            if (Client.m_Core.ReConnect() == false)
                            {
                                Console.WriteLine("reconnect fail. check connection not disconnect or recovery mode");
                            }
                            break;

                        case "/exit":
                            run_program = false;
                            break;

                        case "/h":
                            DisplayHelpCommand();
                            break;

                        case "/f":
                            Client.m_Core.FastRecovery(); // 즉시 연결복구 처리하기
                            break;

                        case "/k":
                            Client.m_Core.ForceLeave();   // 일방적인 서버와의 연결종료
                            break;

                        default:
                            // packet send test
                            RemoteClass.CUserClass _testClass = new RemoteClass.CUserClass();
                            Dictionary<Int32, Int32> _testDic = new Dictionary<Int32, Int32>();
                            _testDic.Add(1212345, 1230);

                            Client.proxy.request_message(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Encrypt, _testClass, _testDic, ret.Result);

                            Client.stub.reponse_message = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32, Int32> dic_test, string msg) =>
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
            Console.WriteLine("/Cmd:  h(Help) q(Quit) exit(ProgramExit) c(서버와연결)");
            Console.WriteLine("/Cmd:  f(즉각연결복구) k(연결강제종료)");
        }
    }
}
