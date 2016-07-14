using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CasualLobby
{
    public class LobbyServer
    {
        public ZNet.CoreServerNet m_Core = null;

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public bool run_program = true;


        // 클라이언트 목록
        public Dictionary<ZNet.RemoteID, CasualCommonSvr.CUser> RemoteClients = new Dictionary<ZNet.RemoteID, CasualCommonSvr.CUser>();


        // 방 목록
        public Dictionary<Guid, CasualCommonSvr.CRoom> RemoteRooms = new Dictionary<Guid, CasualCommonSvr.CRoom>();


        // 이 서버의 remoteID : 이 로비서버에서 만들어진 방에서 다시 나갈때 원래의 로비서버로 돌아갈때 구분하기 위해서
        public int svrRemoteID;



        public LobbyServer()
        {
            m_Core = new ZNet.CoreServerNet();

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            m_Core.Attach(proxy, stub);



            // --- 클라에게 받는 패킷 ---

            // 채팅 메세지 처리
            stub.Chat = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg) =>
            {
                Console.WriteLine("Remote[{0}] msg : {1}", remote, msg);
                proxy.Chat(remote, ZNet.CPackOption.Basic, msg);
                return true;
            };

            // 방 만들기 : 룸 서버로 이동하기
            stub.request_make_room = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string name) =>
            {
                int server_type = (int)CasualCommon.Server.Room;

                // 아무 서버나 하나 고른다
                ZNet.MasterInfo svr;
                if (m_Core.SelectServer(server_type, out svr))
                {
                    // 이동 파라미터 구성
                    ZNet.ArrByte param_buffer;
                    CasualCommonSvr.MoveParam param = new CasualCommonSvr.MoveParam();
                    param.From(CasualCommonSvr.MoveParam.ParamMove.MoveToRoom, CasualCommonSvr.MoveParam.ParamRoom.RoomMake, Guid.NewGuid(), this.svrRemoteID);
                    CasualCommonSvr.Common.ServerMoveParam1(param, out param_buffer);

                    m_Core.ServerMoveStart(remote, svr.m_Addr, param_buffer, param.room_id);
                }
                return true;
            };

            // 방 입장하기 : 룸서버 이동
            stub.request_join_room = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID) =>
            {
                int server_type = (int)CasualCommon.Server.Room;

                ZNet.MasterInfo[] svr_array;
                m_Core.GetServerList(server_type, out svr_array);

                ZNet.MasterInfo find_server = null;
                if (roomID == Guid.Empty)
                {
                    // 방 검색
                    foreach (var one_room in RemoteRooms)
                    {
                        // 인원숫자 검사 : 입장 가능한 방
                        if (one_room.Value.users.Count < one_room.Value.max_users)
                        {
                            // 서버 검색
                            foreach (var obj in svr_array)
                            {
                                // 해당 방이 존재하는지 확인
                                if ((ZNet.RemoteID)one_room.Value.remote_svr == obj.m_remote)
                                {
                                    roomID = one_room.Value.roomID;
                                    find_server = obj;
                                    break;
                                }
                            }
                        }
                        if (find_server != null)
                            break;
                    }

                    if (find_server == null)
                    {
                        // 해당 서버 없음 : 이동 실패
                        return true;
                    }


                    // 이동 파라미터 구성
                    ZNet.ArrByte param_buffer;
                    CasualCommonSvr.MoveParam param = new CasualCommonSvr.MoveParam();
                    param.From(CasualCommonSvr.MoveParam.ParamMove.MoveToRoom, CasualCommonSvr.MoveParam.ParamRoom.RoomJoin, roomID, this.svrRemoteID);
                    CasualCommonSvr.Common.ServerMoveParam1(param, out param_buffer);

                    m_Core.ServerMoveStart(remote, find_server.m_Addr, param_buffer, roomID);
                }
                else
                {
                    // 지정된 roomID가 존재하는 룸서버를 골라서 해당 방으로 입장시켜준다
                }
                return true;
            };




            // --- 서버간 통신 패킷 ---

            // 방생성 + 입장
            stub.room_lobby_makeroom = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, string name, int number, ZNet.RemoteID remote_svr, ZNet.RemoteID remote_lobby, Guid userID) =>
            {
                CasualCommonSvr.CRoom new_room = new CasualCommonSvr.CRoom();
                new_room.From(roomID, name, number, (int)remote_svr, (int)remote_lobby);
                new_room.users.Add(userID);
                RemoteRooms.Add(roomID, new_room);
                return true;
            };

            // 방입장
            stub.room_lobby_joinroom = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, Guid userID) =>
            {
                CasualCommonSvr.CRoom room_join;
                if (RemoteRooms.TryGetValue(roomID, out room_join))
                {
                    room_join.users.Add(userID);
                }
                return true;
            };

            // 방퇴장
            stub.room_lobby_outroom = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, Guid userID) =>
            {
                CasualCommonSvr.CRoom room_join;
                if (RemoteRooms.TryGetValue(roomID, out room_join))
                {
                    room_join.users.Remove(userID);
                }

                if (room_join.users.Count == 0)
                {
                    RemoteRooms.Remove(roomID);
                }
                return true;
            };




            // -- Core Event --

            m_Core.client_join_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr, ZNet.ArrByte move_server, ZNet.ArrByte move_param) =>
            {
                // 서버이동으로 입장한 경우
                if (move_server.Count > 0)
                {
                    CasualCommonSvr.CUser rc;
                    CasualCommonSvr.Common.ServerMoveComplete(move_server, out rc);

                    CasualCommonSvr.MoveParam param;
                    CasualCommonSvr.Common.ServerMoveParam2(move_param, out param);

                    Console.WriteLine("move server complete  {0} {1} {2}", rc.data.userID, rc.data.info, rc.data.temp);
                    RemoteClients.Add(remote, rc);
                }
                else
                {
                    // 일반 입장은 허용하지 않음
                    m_Core.CloseRemoteClient(remote);
                }

                Console.WriteLine("Client {0} is Join {1}:{2}. Current={3}\n", remote, addr.m_ip, addr.m_port, RemoteClients.Count);
            };
            m_Core.client_leave_handler = (ZNet.RemoteID remote, bool bMoveServer) =>
            {
                RemoteClients.Remove(remote);
                Console.WriteLine("Client {0} Leave. MoveServer{1}  Current={2}\n", remote, bMoveServer, RemoteClients.Count);
            };

            m_Core.move_server_start_handler = (ZNet.RemoteID remote, out ZNet.ArrByte buffer) =>
            {
                CasualCommonSvr.CUser rc;
                if (RemoteClients.TryGetValue(remote, out rc) == false)
                {
                    buffer = null;
                    return;
                }
                rc.data.temp = "로비서버";

                // 여기서는 이동할 서버로 동기화 시킬 유저 데이터를 구성하여 buffer에 넣어둔다 -> 완료서버에서 해당 데이터를 그대로 받게된다
                CasualCommonSvr.Common.ServerMoveStart(rc, out buffer);

                Console.WriteLine("move server start  {0} {1} {2}", rc.data.userID, rc.data.info, rc.data.temp);
            };

            // 파라미터 검사후 서버이동 승인 여부 결정하기
            m_Core.move_server_param_handler = (ZNet.ArrByte move_param, int count_idx) =>
            {
                CasualCommonSvr.MoveParam param;
                CasualCommonSvr.Common.ServerMoveParam2(move_param, out param);

                Console.WriteLine("MoveParam_2 {0} {1} {2}", param.moveTo, param.roomJoin, param.room_id);

                // 이서버가 로비서버이므로 파라미터가 로비서버일때만 승인해준다
                if (param.moveTo == CasualCommonSvr.MoveParam.ParamMove.MoveToLobby)
                    return true;

                return false;
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
            };
            m_Core.server_leave_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr) =>
            {
                //Console.WriteLine(string.Format("서버P2P맴버 퇴장 remoteID {0}", remote));
            };

            m_Core.server_master_join_handler = (ZNet.RemoteID remote, ZNet.RemoteID myRemoteID) =>
            {
                this.svrRemoteID = (int)myRemoteID;
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
            };
        }
    }


    class Program
    {
        static void Main(string[] args)
        {
            UInt16 portnum = CasualCommonSvr.Lobby.portnum;

            // 로드벨런싱을 위한 여러개의 서버를 실행하기 위해 이 서버의 Listen Port번호를 파라미터로 입력가능
            if (args.Length == 1)
            {
                portnum = UInt16.Parse(args[0]);
            }

            LobbyServer Svr = new LobbyServer();

            ZNet.StartOption param = new ZNet.StartOption();
            param.m_IpAddressListen = CasualCommonSvr.Lobby.ipaddr;
            param.m_PortListen = portnum;
            param.m_MaxConnectionCount = 50000;
            param.m_RefreshServerTickMs = 10000;
            param.m_ProtocolVersion = CasualCommon.Join.protocol_ver;

            Svr.m_Core.SetKeepAliveOption(30);

            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (Svr.m_Core.Start(param, outResult))
            {
                Console.WriteLine("LobbyServer Start ok.\n");
                DisplayCommand();
            }
            else
            {
                Console.WriteLine("Start error : {0} \n", outResult.msg);
            }

            Svr.m_Core.MasterConnect(
                CasualCommonSvr.MasterServerConnect.master_ipaddr,
                CasualCommonSvr.MasterServerConnect.master_portnum,
                string.Format("LobbyServer{0}", portnum),
                (int)CasualCommon.Server.Lobby
                );

            var ret = CasualCommon.Common.ReadLineAsync();
            while (Svr.run_program)
            {
                if (ret.IsCompleted)
                {
                    switch (ret.Result)
                    {
                        case "/h":
                            DisplayCommand();
                            break;

                        case "/stat":
                            CasualCommonSvr.Common.DisplayStatus(Svr.m_Core);
                            break;

                        case "/room":
                            if (Svr.RemoteRooms.Count > 0)
                            {
                                foreach (var obj in Svr.RemoteRooms)
                                {
                                    Console.WriteLine("room {0}/{1}", obj.Value.users.Count, obj.Value.max_users);
                                }
                            }
                            else
                            {
                                Console.WriteLine("room empty");
                            }
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

        static public void DisplayCommand()
        {
            Console.WriteLine("/Cmd:  q(Quit) h(Help) stat(status info) room(room state)");
        }
    }
}
