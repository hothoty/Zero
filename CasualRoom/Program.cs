using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CasualRoom
{
    public class RoomServer
    {
        public ZNet.CoreServerNet m_Core = null;

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        public bool run_program = true;


        // 클라이언트 목록
        public Dictionary<ZNet.RemoteID, CasualCommonSvr.CUser> RemoteClients = new Dictionary<ZNet.RemoteID, CasualCommonSvr.CUser>();


        // 방 목록
        public Dictionary<Guid, CasualCommonSvr.CRoom> RemoteRooms = new Dictionary<Guid, CasualCommonSvr.CRoom>();


        // 이 서버의 remoteID : 이 서버에서 만들어진 방을 다른 서버에서 구분 용도로 사용하기 위해 방생성시 정보에 넣어 사용한다
        public int svrRemoteID;



        public RoomServer()
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

            // 방 나가기
            stub.request_out_room = (ZNet.RemoteID remote, ZNet.CPackOption pkOption) =>
            {
                int server_type = (int)CasualCommon.Server.Lobby;

                CasualCommonSvr.CUser rc;
                if (RemoteClients.TryGetValue(remote, out rc) == false)
                    return true;

                CasualCommonSvr.CRoom room_join;
                if (RemoteRooms.TryGetValue(rc.roomID, out room_join))
                {
                    ZNet.MasterInfo[] svr_array;
                    m_Core.GetServerList(server_type, out svr_array);

                    // 서버 검색
                    foreach (var obj in svr_array)
                    {
                        // 원래 방입장하기 전에 있었던 로비서버로 돌아가기 위해
                        if (obj.m_remote == (ZNet.RemoteID)room_join.remote_lobby)
                        {
                            // 이동 파라미터 구성 : 타겟을 로비서버로 (나머지 파라미터는 룸->로비 이동시에는 별 의미없음)
                            ZNet.ArrByte param_buffer;
                            CasualCommonSvr.MoveParam param = new CasualCommonSvr.MoveParam();
                            param.From(CasualCommonSvr.MoveParam.ParamMove.MoveToLobby, CasualCommonSvr.MoveParam.ParamRoom.RoomNull, Guid.NewGuid(), this.svrRemoteID);
                            CasualCommonSvr.Common.ServerMoveParam1(param, out param_buffer);

                            m_Core.ServerMoveStart(remote, obj.m_Addr, param_buffer, param.room_id);
                            return true;
                        }
                    }
                }
                return true;
            };




            // 파라미터 검사후 서버이동 승인 여부 결정하기
            m_Core.move_server_param_handler = (ZNet.ArrByte move_param, int count_idx) =>
            {
                CasualCommonSvr.MoveParam param;
                CasualCommonSvr.Common.ServerMoveParam2(move_param, out param);

                Console.WriteLine("MoveParam_2 {0} {1} {2}", param.moveTo, param.roomJoin, param.room_id);

                if (param.moveTo == CasualCommonSvr.MoveParam.ParamMove.MoveToRoom)
                {
                    if (param.roomJoin == CasualCommonSvr.MoveParam.ParamRoom.RoomMake)
                    {
                        return true;
                    }
                    else if (param.roomJoin == CasualCommonSvr.MoveParam.ParamRoom.RoomJoin)
                    {
                        // 현재 이 방의 인원숫자 + allow숫자(count_idx)를 비교하여 입장 허용 여부를 결정한다
                        CasualCommonSvr.CRoom room_join;
                        if (RemoteRooms.TryGetValue(param.room_id, out room_join))
                        {
                            //if (room_join.users.Count + count_idx < 1)
                            if (room_join.users.Count + count_idx < room_join.max_users)
                                return true;
                        }
                    }
                }
                return false;
            };

            m_Core.client_join_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr, ZNet.ArrByte move_server, ZNet.ArrByte move_param) =>
            {
                // 서버이동으로 입장한 경우
                if (move_server.Count > 0)
                {
                    CasualCommonSvr.CUser rc;
                    CasualCommonSvr.Common.ServerMoveComplete(move_server, out rc);

                    CasualCommonSvr.MoveParam param;
                    CasualCommonSvr.Common.ServerMoveParam2(move_param, out param);

                    if (param.roomJoin == CasualCommonSvr.MoveParam.ParamRoom.RoomMake)
                    {
                        // 방생성
                        CasualCommonSvr.CRoom new_room = new CasualCommonSvr.CRoom();
                        new_room.From(param.room_id, "방이름", 123, this.svrRemoteID, param.lobby_remote);

                        if (RemoteRooms.ContainsKey(param.room_id))
                        {
                            // 동일한 guid생성을 허용금지 : 예외처리필요
                        }

                        RemoteRooms.Add(param.room_id, new_room);

                        new_room.users.Add(rc.data.userID);

                        // 로비서버에게 방생성을 알린다
                        proxy.room_lobby_makeroom((ZNet.RemoteID)new_room.remote_lobby, ZNet.CPackOption.Basic,
                            new_room.roomID, new_room.name, new_room.number, (ZNet.RemoteID)new_room.remote_svr, (ZNet.RemoteID)new_room.remote_lobby, rc.data.userID);
                    }
                    else if(param.roomJoin == CasualCommonSvr.MoveParam.ParamRoom.RoomJoin)
                    {
                        // 방입장
                        CasualCommonSvr.CRoom room_join;
                        if (RemoteRooms.TryGetValue(param.room_id, out room_join))
                        {
                            room_join.users.Add(rc.data.userID);
                        }
                        else
                        {
                            Console.WriteLine("move server join error!!! {0} {1} {2}", rc.data.userID, rc.data.info, rc.data.temp);
                            m_Core.CloseRemoteClient(remote);
                            return;
                        }

                        // 로비서버에게 방입장을 알린다
                        proxy.room_lobby_joinroom((ZNet.RemoteID)room_join.remote_lobby, ZNet.CPackOption.Basic, room_join.roomID, rc.data.userID);
                    }

                    rc.roomID = param.room_id;

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
                CasualCommonSvr.CUser rc;
                if (RemoteClients.TryGetValue(remote, out rc))
                {
                    CasualCommonSvr.CRoom room_join;
                    if (RemoteRooms.TryGetValue(rc.roomID, out room_join))
                    {
                        room_join.users.Remove(rc.data.userID);
                    }

                    if (room_join.users.Count == 0)
                    {
                        RemoteRooms.Remove(rc.roomID);
                    }

                    // 로비서버에게 방퇴장을 알린다
                    proxy.room_lobby_outroom((ZNet.RemoteID)room_join.remote_lobby, ZNet.CPackOption.Basic, room_join.roomID, rc.data.userID);

                    Console.WriteLine("Client {0} Leave.\n", remote);
                }
            };

            m_Core.move_server_start_handler = (ZNet.RemoteID remote, out ZNet.ArrByte buffer) =>
            {
                CasualCommonSvr.CUser rc;
                if (RemoteClients.TryGetValue(remote, out rc) == false)
                {
                    buffer = null;
                    return;
                }
                rc.data.temp = "룸서버";

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
            UInt16 portnum = CasualCommonSvr.Room.portnum;

            // 로드벨런싱을 위한 여러개의 서버를 실행하기 위해 이 서버의 Listen Port번호를 파라미터로 입력가능
            if (args.Length == 1)
            {
                portnum = UInt16.Parse(args[0]);
            }

            RoomServer Svr = new RoomServer();

            ZNet.StartOption param = new ZNet.StartOption();
            param.m_IpAddressListen = CasualCommonSvr.Room.ipaddr;
            param.m_PortListen = portnum;
            param.m_MaxConnectionCount = 50000;
            param.m_RefreshServerTickMs = 10000;
            param.m_ProtocolVersion = CasualCommon.Join.protocol_ver;

            Svr.m_Core.SetKeepAliveOption(30);

            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (Svr.m_Core.Start(param, outResult))
            {
                Console.WriteLine("RoomServer Start ok.\n");
                DisplayCommand();
            }
            else
            {
                Console.WriteLine("Start error : {0} \n", outResult.msg);
            }

            Svr.m_Core.MasterConnect(
                CasualCommonSvr.MasterServerConnect.master_ipaddr,
                CasualCommonSvr.MasterServerConnect.master_portnum,
                "RoomServer",
                (int)CasualCommon.Server.Room
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
