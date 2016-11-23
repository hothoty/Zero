using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

/// <summary>
/// 
/// 서버와 클라이언트 양측에서 공통적으로 접근할 내용은 이곳에 구성합니다
/// 
/// - 공통 클래스
/// 
/// - 마샬링 관련
/// 
/// 
/// </summary>
namespace NetCommon
{
    public class Vars
    {
        public static readonly UInt32 version = 0;
    }

    public interface IMarshallable
    {
        bool Read(ZNet.CMessage msg);
        void Write(ZNet.CMessage msg);
    }


    // 마샬링할 클래스를 추가할 경우 아래 내용 참고하세요
    public class CUser : IMarshallable
    {
        public string username = "";
        public int userID;
        public ZNet.NetAddress addr;
        public SampleEnum test_enum;

        public bool Read(ZNet.CMessage msg)
        {
            msg.Read(out username);
            msg.Read(out userID);
            msg.Read(out addr);
            Marshaler.Read(msg, out test_enum);
            return true;
        }
        public void Write(ZNet.CMessage msg)
        {
            msg.Write(username);
            msg.Write(userID);
            msg.Write(addr);
            Marshaler.Write(msg, test_enum);
        }
    }

    public partial class Marshaler : ZNet.Marshaler
    {
        protected static void ReadList<T>(ZNet.CMessage msg, out List<T> value) where T : IMarshallable, new()
        {
            value = new List<T>();
            int count = 0;
            msg.Read(out count);
            for (int i = 0; i < count; ++i)
            {
                T elem = new T();
                elem.Read(msg);
                value.Add(elem);
            }
        }
        protected static void WriteList<T>(ZNet.CMessage msg, List<T> value) where T : IMarshallable
        {
            msg.Write(value.Count);
            List<T>.Enumerator valueEnum = value.GetEnumerator();
            while (valueEnum.MoveNext())
            {
                T elem = valueEnum.Current;
                elem.Write(msg);
            }
        }
    }


    public partial class Marshaler : ZNet.Marshaler
    {
        public static bool Read(ZNet.CMessage msg, out CUser value)
        {
            value = new CUser();
            return value.Read(msg);
        }
        public static void Write(ZNet.CMessage msg, CUser value)
        {
            value.Write(msg);
        }

        public static bool Read(ZNet.CMessage msg, out List<string> value)
        {
            value = new List<string>();
            int count = 0;
            msg.Read(out count);
            for (int i = 0; i < count; ++i)
            {
                string elem;
                Read(msg, out elem);
                value.Add(elem);
            }
            return true;
        }
        public static void Write(ZNet.CMessage msg, List<string> value)
        {
            msg.Write(value.Count);
            foreach (string temp in value)
            {
                Write(msg, temp);
            }
        }
    }


    public enum SampleEnum : int
    {
        Abc = 0,
        Qwe,
        Zxc,
    }
    public partial class Marshaler : ZNet.Marshaler
    {
        public static bool Read(ZNet.CMessage msg, out SampleEnum value)
        {
            int _value;
            msg.Read(out _value);
            value = (SampleEnum)_value;
            return true;
        }
        public static void Write(ZNet.CMessage msg, SampleEnum value)
        {
            msg.Write((int)value);
        }
    }
}
