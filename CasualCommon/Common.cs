using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CasualCommon
{
    /// <summary>
    /// 최초 로그인할 서버 주소
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
        Lobby,
        Room
    }

    public class Common
    {
        static public async Task<string> ReadLineAsync()
        {
            var line = await Task.Run(() => Console.ReadLine());
            return line;
        }

        static public void DisplayHelpCommand()
        {
            Console.WriteLine("/Cmd:  q(Quit) h(Help) stat(status info)");
        }
    }
}
