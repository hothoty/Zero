using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CasualCommonSvr
{
    /// <summary>
    /// 마스터 서버 관련 정보
    /// </summary>
    public class MasterServerConnect
    {
        public static string master_ipaddr = "127.0.0.1";
        public static readonly UInt16 master_portnum = 35000;
    }

    public class Lobby
    {
        public static readonly string ipaddr = "127.0.0.1";
        public static readonly UInt16 portnum = 22000;
    }

    public class Room
    {
        public static readonly string ipaddr = "127.0.0.1";
        public static readonly UInt16 portnum = 25000;
    }

    /// <summary>
    /// 서버이동시 동기화할 유저 데이터
    /// </summary>
    public struct UserDataSync
    {
        public Guid userID;
        public string info;
        public string temp;
    }

    /// <summary>
    /// 서버이동시 검사할 파라미터
    /// </summary>
    public class MoveParam
    {
        public enum ParamMove : int
        {
            MoveToLogin = 0,
            MoveToLobby,
            MoveToRoom,
        }
        public enum ParamRoom : int
        {
            RoomNull = 0,
            RoomMake,
            RoomJoin,
        }
        public ParamMove moveTo;
        public ParamRoom roomJoin;
        public Guid room_id;
        public int lobby_remote;
        public void From(ParamMove _to, ParamRoom _join, Guid id, int _lobby)
        {
            moveTo = _to;
            roomJoin = _join;
            room_id = id;
            lobby_remote = _lobby;
        }
    }

    /// <summary>
    /// 각 서버에서 처리할 유저 데이터
    /// </summary>
    public class CUser
    {
        public UserDataSync data;
        public Guid roomID; // 룸서버에 있을경우 방ID
    }

    /// <summary>
    /// 방 정보
    /// </summary>
    public class CRoom
    {
        public Guid roomID;
        public int number = 0;
        public string name = "empty";
        public List<Guid> users;
        public int max_users = 2;   // 일단 2명으로 제한한다
        public int remote_svr;      // 서버구분용 remoteID번호 (입장할때 방이 어떤 서버에 존재하는지 정보)
        public int remote_lobby;    // 서버구분용 remoteID번호 (원래입장시점의 로비서버 - 방에서 나갈때 사용)
        public CRoom()
        {
            users = new List<Guid>();
        }
        public void From(Guid id, string _name, int _num, int _remote_svr, int _remote_lobby)
        {
            roomID = id;
            name = _name;
            number = _num;
            remote_svr = _remote_svr;
            remote_lobby = _remote_lobby;
        }
    }

    public class Common
    {
        // 서버이동시 동기화할 유저 데이터 구성
        static public void ServerMoveStart(CasualCommonSvr.CUser rc, out ZNet.ArrByte buffer)
        {
            ZNet.CMessage msg = new ZNet.CMessage();
            msg.Write(rc.data.userID);
            msg.Write(rc.data.info);
            msg.Write(rc.data.temp);
            buffer = msg.m_array;
        }

        // 서버이동 완료시 동기화할 유저 데이터 복구
        static public void ServerMoveComplete(ZNet.ArrByte buffer, out CasualCommonSvr.CUser data)
        {
            CasualCommonSvr.CUser rc = new CasualCommonSvr.CUser();

            ZNet.CMessage msg = new ZNet.CMessage();
            msg.m_array = buffer;

            msg.Read(out rc.data.userID);
            msg.Read(out rc.data.info);
            msg.Read(out rc.data.temp);
            data = rc;
        }

        static public void ServerMoveParam1(MoveParam param, out ZNet.ArrByte buffer)
        {
            ZNet.CMessage msg = new ZNet.CMessage();
            msg.Write((int)param.moveTo);
            msg.Write((int)param.roomJoin);
            msg.Write(param.room_id);
            msg.Write(param.lobby_remote);
            buffer = msg.m_array;
        }
        static public void ServerMoveParam2(ZNet.ArrByte buffer, out MoveParam param)
        {
            param = new MoveParam();
            ZNet.CMessage msg = new ZNet.CMessage();
            msg.m_array = buffer;
            int _moveTo;
            int _roomJoin;
            msg.Read(out _moveTo);
            msg.Read(out _roomJoin);
            msg.Read(out param.room_id);
            msg.Read(out param.lobby_remote);
            param.moveTo = (MoveParam.ParamMove)_moveTo;
            param.roomJoin = (MoveParam.ParamRoom)_roomJoin;
        }

        static public void DisplayStatus(ZNet.CoreServerNet svr)
        {
            ZNet.ServerState status;
            svr.GetCurrentState(out status);


            // 기본 정보
            Console.WriteLine(string.Format(
                "[NetInfo]  Connect/Join {0}({1})/{2}  Connect(Server) {3}/{4}  Accpet/Max {5}/{6}",

                // 실제 연결된 client
                status.m_CurrentClient,

                // 연결복구 처리과정인 client
                status.m_RecoveryCount,

                // 서버에 입장완료상태의 client
                status.m_JoinedClient,

                // 서버간 direct p2p 연결된 server
                status.m_ServerP2PCount,

                // 서버간 direct p2p 연결 모니터링중인 server(서버간 연결 자동복구를 위한 모니터링)
                status.m_ServerP2PConCount,

                // 이 서버에 추가 연결 가능한 숫자
                status.m_nIoAccept,

                // 이 서버에 최대 연결 가능한 숫자
                status.m_MaxAccept
                ));


            // 엔진 내부에서 작업중인 IO 관련 상태 정보
            Console.WriteLine(string.Format(
                "[IO Info]  Close {0}  Event {1}  Recv {2}  Send {3}",

                // current io close
                status.m_nIoClose,

                // current io event
                status.m_nIoEvent,

                // current io recv socket
                status.m_nIoRecv,

                // current io send socket
                status.m_nIoSend
            ));


            // 엔진 메모리 관련 사용 정보
            Console.WriteLine(string.Format(
                "[MemInfo]  Alloc/Instant[{0}/{1}], test[{2}], EngineVersion[{3}.{4:0000}] ",

                // 미리 할당된 IO 메모리
                status.m_nAlloc,

                // 즉석 할당된 IO 메모리
                status.m_nAllocInstant,

                // test data
                status.m_test_data,

                // Core버전
                svr.GetCoreVersion() / 10000,
                svr.GetCoreVersion() % 10000
            ));


            // 스레드 정보
            string strThr = "[ThreadInfo] (";
            int MaxDisplayThreadCount = status.m_arrThread.Count();
            if (MaxDisplayThreadCount > 8)   // 화면이 복잡하니까 그냥 최대 8개까지만 표시
            {
                strThr += MaxDisplayThreadCount;
                strThr += ") : ";
                MaxDisplayThreadCount = 8;
            }
            else
            {
                strThr += MaxDisplayThreadCount;
                strThr += ") : ";
            }

            for (int i = 0; i < MaxDisplayThreadCount; i++)
            {
                strThr += "[";
                strThr += status.m_arrThread[i].m_ThreadID;     // 스레드ID
                strThr += "/";
                strThr += status.m_arrThread[i].m_CountQueue;   // 처리 대기중인 작업
                strThr += "/";
                strThr += status.m_arrThread[i].m_CountWorked;  // 처리된 작업(누적)
                strThr += "] ";
            }
            Console.WriteLine(strThr);
        }
    }
}
