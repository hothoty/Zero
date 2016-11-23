using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleServer2
{
    public class RemoteClient : NetCommon.CUser
    {
        public ZNet.RemoteID remoteID = ZNet.RemoteID.Remote_None;
    }
}
