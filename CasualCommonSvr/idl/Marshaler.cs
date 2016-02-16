using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RemoteClass
{
    public class Marshaler : ZNet.Marshaler
    {
        public static void Write(ZNet.CMessage msg, Dictionary<ZNet.RemoteID, ZNet.MasterInfo> b)
        {
            Int32 data = b.Count;
            Write(msg, data);

            foreach (KeyValuePair<ZNet.RemoteID, ZNet.MasterInfo> obj in b)
            {
                Write(msg, obj.Key);
                Write(msg, obj.Value);
            }
        }
        public static void Read(ZNet.CMessage msg, out Dictionary<ZNet.RemoteID, ZNet.MasterInfo> b)
        {
            Int32 data;
            Read(msg, out data);

            b = new Dictionary<ZNet.RemoteID, ZNet.MasterInfo>();

            for (Int32 i = 0; i < data; i++)
            {
                ZNet.RemoteID _key;
                ZNet.MasterInfo _value;
                Read(msg, out _key);
                Read(msg, out _value);
                b.Add(_key, _value);
            }
        }
    }
}
