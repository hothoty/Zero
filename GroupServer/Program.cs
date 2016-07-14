using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/// <summary>
/// [그룹 관리 기능을 보여주는 예제]
/// 
/// 내장된 그룹 생성/합류/파괴/탈퇴 등의 기능들을 이용해 GroupID하나를 관리하는 방법을 보여줍니다
/// 
/// </summary>
namespace GroupServer
{
    public class GrpServer
    {
        public ZNet.CoreServerNet m_Core = new ZNet.CoreServerNet();

        public ZNet.RemoteID GroupID;
        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public GrpServer()
        {
            this.GroupID = ZNet.RemoteID.Remote_None;

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);

            // 채팅 메세지
            stub.Chat = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg) =>
            {
                Console.WriteLine("Remote[{0}] msg : {1}", remote, msg);
                proxy.Chat(remote, ZNet.CPackOption.Basic, msg);
                return true;
            };

            // 그룹 관리 처리하기 예제
            stub.GroupTestCommand = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string cmd) =>
            {
                switch(cmd)
                {
                    case "/make":   // 그룹 생성 또는 합류처리
                        if (this.GroupID == ZNet.RemoteID.Remote_None)
                        {
                            if (this.m_Core.CreateGroup(out this.GroupID, remote))
                            {
                                Console.WriteLine("그룹생성완료 groupID[{0}] remote[{1}] members[1]", this.GroupID, remote);
                            }
                            else
                            {
                                Console.WriteLine("그룹생성오류");
                            }
                        }
                        else
                        {
                            int Members;
                            if (this.m_Core.JoinGroup(this.GroupID, remote, out Members))
                            {
                                Console.WriteLine("그룹합류완료 groupID[{0}] remote[{1}] members[{2}]", this.GroupID, remote, Members);
                            }
                        }
                        break;

                    case "/destroy":
                        // 그룹파괴 시도하기 : group_destroy_handler를 통해 결과 통보
                        this.m_Core.DestroyGroup(this.GroupID);
                        break;

                    case "/leave":
                        // 그룹탈퇴 시도하기 : group_memberout_handler를 통해 결과 통보
                        this.m_Core.OutGroup(this.GroupID, remote);
                        break;

                    case "/msg":
                        // 그룹 전체 메세지 보내기 : remoteID에 그룹아이디(this.GroupID)를 넣는것으로 ok
                        proxy.Chat(this.GroupID, ZNet.CPackOption.Basic, "Group test message.");
                        break;
                }
                return true;
            };

            // 그룹 관련 이벤트 처리
            m_Core.group_memberout_handler = (ZNet.RemoteID groupID, ZNet.RemoteID memberID, int Members) =>
            {
                Console.WriteLine("GroupOut,   groupID {0}  remote {1}  members {2}\n", groupID, memberID, Members);
            };
            m_Core.group_destroy_handler = (ZNet.RemoteID groupID) =>
            {
                Console.WriteLine("GroupDestroy,   groupID {0}\n", groupID);
                this.GroupID = ZNet.RemoteID.Remote_None;
            };


            // 기타
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
            GrpServer Svr = new GrpServer();

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
