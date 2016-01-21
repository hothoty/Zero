using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RemoteClass
{
    public class CUserClass
    {
        public Dictionary<Int32, Int32> map_test;
        public string str_test;
        public Int64 int_test;

	    public CUserClass()
	    {
            map_test = new Dictionary<Int32, Int32>();
		    map_test.Add(123,456);
		    map_test.Add(456,789);
		    str_test = "abc가나다";
		    int_test = 12345000;
	    }
    };

    public class Marshaler : ZNet.Marshaler
    {
        public static void Write(ZNet.CMessage msg, Dictionary<Int32, Int32> p)
        {
            Int32 data = p.Count;
            msg.Write(data);

            foreach (KeyValuePair<Int32, Int32> obj in p)
            {
                msg.Write(obj.Key);
                msg.Write(obj.Value);
            }
        }
        public static void Read(ZNet.CMessage msg, out Dictionary<Int32, Int32> p)
        {
            Int32 data;
            msg.Read(out data);

            p = new Dictionary<Int32, Int32>();

            for (Int32 i = 0; i < data; i++)
            {
                int _key;
                int _value;
                msg.Read(out _key);
                msg.Read(out _value);
                p.Add(_key, _value);
            }
        }

        public static void Write(ZNet.CMessage msg, CUserClass b)
        {
            Write(msg, b.map_test);
            msg.Write(b.str_test);
            msg.Write(b.int_test);
        }
        public static void Read(ZNet.CMessage msg, out CUserClass b)
        {
            b = new CUserClass();
            Read(msg, out b.map_test);
            msg.Read(out b.str_test);
            msg.Read(out b.int_test);
        }

        public static void Write(ZNet.CMessage msg, CUserClass[] b)
        {
            Int32 data = b.Count();
            Write(msg, data);
            for (Int32 i = 0; i < data; i++)
            {
                Write(msg, b[i]);
            }
        }
        public static void Read(ZNet.CMessage msg, out CUserClass[] b)
        {
            Int32 data;
            Read(msg, out data);

            b = new CUserClass[data];
            for (Int32 i = 0; i < data; i++)
            {
                Read(msg, out b[i]);
            }
        }
    }
}
