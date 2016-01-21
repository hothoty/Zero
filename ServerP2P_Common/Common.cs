using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerP2P_Common
{
    /// <summary>
    /// 마스터 서버 관련 정보
    /// </summary>
    public class MasterServerConnect
    {
        public static string master_ipaddr = "127.0.0.1";
        public static readonly UInt16 master_portnum = 35000;
    }

    /// <summary>
    /// 최초 서버입장 관련 정보 : 로그인 서버의 주소로 세팅함
    /// </summary>
    public class Join
    {
        public static readonly string ipaddr = "127.0.0.1";
        public static readonly UInt16 portnum = 20000;
        public static readonly UInt32 protocol_ver = 0;
    }

    /// <summary>
    /// 서버 종류
    /// </summary>
    public enum Server : int
    {
        Login = 0,
        Main
    }

    /// <summary>
    /// 서버이동시 동기화할 유저 데이터
    /// </summary>
    public struct UserDataSync
    {
        public string info;
        public Int64 item_id;
    }
}
