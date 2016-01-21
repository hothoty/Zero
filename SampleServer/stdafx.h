// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "../include/NetLibServer.h"

#if defined(_WIN64)
#ifdef	_DEBUG
#pragma comment(lib, "../Lib/x64/debug/netlib.lib" )
#else	//	_DEBUG
#pragma comment(lib, "../Lib/x64/release/netlib.lib" )
#endif	//	_DEBUG
#else
#ifdef	_DEBUG
#pragma comment(lib, "../Lib/w32/debug/netlib.lib" )
#else	//	_DEBUG
#pragma comment(lib, "../Lib/w32/release/netlib.lib" )
#endif	//	_DEBUG
#endif

#include "../SampleCommon/Marshaler.h"