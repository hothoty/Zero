#pragma once

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <fstream>
#include <stdint.h>

#include "BaseTypes.h"
#include "BaseMacro.h"
#include "NetDef.h"
#include "InterLocked.h"
#include "Memory.h"
#include "SafeArray.h"
#include "LockScoped.h"
#include "Dictionary.h"
#include "TMap.h"
#include "String.h"
#include "NetAddress.h"

#include "Guid.h"
#include "BaseEnums.h"
#ifdef _WIN32
#include "AdoManager.h"
#endif
#include "BaseNetType.h"

#include "PackOption.h"
#include "Serialize.h"
#include "Share.h"
#include "Guid64.h"
#include "Singleton.h"

#include "TimeUtil.h"
#include "Thread.h"

#include "Message.h"
#include "LogFile.h"
#include "IPacket.h"

#include "Master.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")