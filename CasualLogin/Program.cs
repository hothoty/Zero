using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CasualLogin
{
    public class LoginServer
    {
        public ZNet.CoreServerNet m_Core = null;

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public bool run_program = true;

        
        // 클라이언트 목록
        public Dictionary<ZNet.RemoteID, CasualCommonSvr.CUser> RemoteClients = new Dictionary<ZNet.RemoteID, CasualCommonSvr.CUser>();

        
        // 방 목록
        public Dictionary<ZNet.RemoteID, ZNet.MasterInfo> lobby_list = new Dictionary<ZNet.RemoteID, ZNet.MasterInfo>();



        public LoginServer()
        {
            m_Core = new ZNet.CoreServerNet();

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);



            stub.Chat = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg) =>
            {
                Console.WriteLine("Remote[{0}] msg : {1}", remote, msg);
                proxy.Chat(remote, ZNet.CPackOption.Basic, msg);
                return true;
            };

            stub.request_lobby_list = (ZNet.RemoteID remote, ZNet.CPackOption pkOption) =>
            {
                proxy.notify_lobby_list(remote, ZNet.CPackOption.Basic, this.lobby_list);
                return true;
            };




            m_Core.client_join_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr, ZNet.ArrByte move_server, ZNet.ArrByte move_param) =>
            {
                // 서버이동으로 입장한 경우
                if (move_server.Count > 0)
                {
                    CasualCommonSvr.CUser rc;
                    CasualCommonSvr.Common.ServerMoveComplete(move_server, out rc);

                    Console.WriteLine("move server complete  {0} {1} {2}", rc.data.userID, rc.data.info, rc.data.temp);
                    RemoteClients.Add(remote, rc);
                }
                else
                {
                    // 일반 입장
                    CasualCommonSvr.CUser rc = new CasualCommonSvr.CUser();
                    rc.data.userID = Guid.NewGuid();
                    rc.data.info = string.Format("유저_{0}", remote);
                    rc.data.temp = "일반입장";
                    RemoteClients.Add(remote, rc);
                }

                proxy.notify_lobby_list(remote, ZNet.CPackOption.Basic, this.lobby_list); // 입장시 로비 리스트 보냄

                Console.WriteLine("Client {0} is Join {1}:{2}. Current={3}\n", remote, addr.m_ip, addr.m_port, RemoteClients.Count);
            };
            m_Core.client_leave_handler = (ZNet.RemoteID remote, bool bMoveServer) =>
            {
                RemoteClients.Remove(remote);
                Console.WriteLine("Client {0} Leave. MoveServer{1}  Current={2}\n", remote, bMoveServer, RemoteClients.Count);
            };

            // 해당 서버 타입의 조건을 검사한후 응답처리
            stub.request_go_lobby = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string lobbyname) =>
            {
                // 실제 해당 서버의 검증 작업
                ZNet.MasterInfo[] svr_array;
                m_Core.GetServerList((int)CasualCommon.Server.Lobby, out svr_array);

                foreach (var obj in svr_array)
                {
                    // 지정한 로비 서버 이름 확인
                    if(lobbyname == obj.m_Description)
                    {
                        // 이동 파라미터 구성
                        ZNet.ArrByte param_buffer;
                        CasualCommonSvr.MoveParam param = new CasualCommonSvr.MoveParam();
                        param.moveTo = CasualCommonSvr.MoveParam.ParamMove.MoveToLobby;
                        CasualCommonSvr.Common.ServerMoveParam1(param, out param_buffer);

                        // 여기서 내부패킷으로 자동적으로 서버이동이 처리 된다
                        m_Core.ServerMoveStart(remote, obj.m_Addr, param_buffer, new Guid());

                        Console.WriteLine("MoveParam_1 {0} {1} {2}", param.moveTo, param.roomJoin, param.room_id);
                        return true;
                    }
                }
                return true;
            };

            m_Core.move_server_start_handler = (ZNet.RemoteID remote, out ZNet.ArrByte buffer) =>
            {
                CasualCommonSvr.CUser rc;
                if (RemoteClients.TryGetValue(remote, out rc) == false)
                {
                    buffer = null;
                    return;
                }
                rc.data.temp = "로그인서버";

                // 여기서는 이동할 서버로 동기화 시킬 유저 데이터를 구성하여 buffer에 넣어둔다 -> 완료서버에서 해당 데이터를 그대로 받게된다
                CasualCommonSvr.Common.ServerMoveStart(rc, out buffer);

                Console.WriteLine("move server start  {0} {1} {2}", rc.data.userID, rc.data.info, rc.data.temp);
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

            m_Core.server_join_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr) =>
            {
                //Console.WriteLine(string.Format("서버P2P맴버 입장 remoteID {0}", remote));

                // 입장한 시점 : 이 서버의 세부 정보는 이후 refresh_handler를 통해 들어오면 처리해야한다
            };
            m_Core.server_leave_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr) =>
            {
                //Console.WriteLine(string.Format("서버P2P맴버 퇴장 remoteID {0}", remote));

                // 퇴장한 서버에 대한 처리
                lobby_list.Remove(remote);
            };

            m_Core.server_master_join_handler = (ZNet.RemoteID remote, ZNet.RemoteID myRemoteID) =>
            {
                Console.WriteLine(string.Format("마스터서버에 입장성공 remoteID {0}", myRemoteID));
            };
            m_Core.server_master_leave_handler = () =>
            {
                Console.WriteLine(string.Format("마스터서버와 연결종료!!!"));
                run_program = false;    // 자동 종료처리를 위해
            };

            m_Core.server_refresh_handler = (ZNet.MasterInfo master_info) =>
            {
                //Console.WriteLine(string.Format("서버P2P remote:{0} type:{1}[{2}] current:{3} addr:{4}:{5}",
                //    master_info.m_remote,
                //    (CasualCommon.Server)master_info.m_ServerType,
                //    master_info.m_Description,
                //    master_info.m_Clients,
                //    master_info.m_Addr.m_ip,
                //    master_info.m_Addr.m_port
                //    ));

                // 로비 리스트 정보 갱신
                lobby_list.Remove(master_info.m_remote);
                if ((CasualCommon.Server)master_info.m_ServerType == CasualCommon.Server.Lobby)
                {
                    lobby_list.Add(master_info.m_remote, master_info);
                }
            };
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            LoginServer Svr = new LoginServer();

            ZNet.StartOption param = new ZNet.StartOption();
            param.m_IpAddressListen = CasualCommon.Join.ipaddr;
            param.m_PortListen = CasualCommon.Join.portnum;
            param.m_MaxConnectionCount = 50000;
            param.m_RefreshServerTickMs = 10000;
            param.m_ProtocolVersion = CasualCommon.Join.protocol_ver;

            Svr.m_Core.SetKeepAliveOption(30);

            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (Svr.m_Core.Start(param, outResult))
            {
                Console.WriteLine("LoginServer Start ok.\n");
                CasualCommon.Common.DisplayHelpCommand();
            }
            else
            {
                Console.WriteLine("Start error : {0} \n", outResult.msg);
            }

            Svr.m_Core.MasterConnect(
                CasualCommonSvr.MasterServerConnect.master_ipaddr,
                CasualCommonSvr.MasterServerConnect.master_portnum,
                "LoginServer",
                (int)CasualCommon.Server.Login
                );

            var ret = CasualCommon.Common.ReadLineAsync();
            while (Svr.run_program)
            {
                if (ret.IsCompleted)
                {
                    switch (ret.Result)
                    {
                        case "/h":
                            CasualCommon.Common.DisplayHelpCommand();
                            break;

                        case "/stat":
                            CasualCommonSvr.Common.DisplayStatus(Svr.m_Core);
                            break;

                        case "/q":
                            Console.WriteLine("quit Server...");
                            Svr.run_program = false;
                            break;
                    }

                    if (Svr.run_program)
                        ret = CasualCommon.Common.ReadLineAsync();
                }

                System.Threading.Thread.Sleep(10);
            }

            Console.WriteLine("Start Closing...  ");
            Svr.m_Core.Dispose();
            Console.WriteLine("Close complete.");

            System.Threading.Thread.Sleep(1000 * 2);
        }
    }
}
