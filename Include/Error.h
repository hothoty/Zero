#pragma once
#include <tchar.h>

#define MSGBOX_VA_MACRO(hwnd, msgtype)\
	va_list ap;\
	va_start(ap, pcszFormat);\
	TCHAR caBuffer[1024];\
	_vstprintf_s(caBuffer, pcszFormat, ap);\
	va_end(ap);\
	MessageBox( hwnd, caBuffer, msgtype, 0 );

#define MSGBOX_VA_MACROA(hwnd, msgtype)\
	va_list ap;\
	va_start(ap, pcszFormat);\
	char caBuffer[1024];\
	vsprintf_s(caBuffer, pcszFormat, ap);\
	va_end(ap);\
	MessageBoxA( hwnd, caBuffer, msgtype, 0 );

namespace Zero
{
	inline void __cdecl ErrMsg(HWND hwnd, TCHAR* pcszFormat, ...)
	{
		MSGBOX_VA_MACRO(hwnd, _T("ERROR") );
	}
	inline void __cdecl ErrMsg(TCHAR* pcszFormat, ...)
	{
		MSGBOX_VA_MACRO(NULL, _T("ERROR"));
	}
	inline void __cdecl ErrMsgA(const char* pcszFormat, ...)
	{
		MSGBOX_VA_MACROA(NULL, "ERROR");
	}
	
	inline void __cdecl MsgBox(HWND hwnd, TCHAR* pcszFormat, ...)
	{
		MSGBOX_VA_MACRO(hwnd, _T("MESSAGE"));
	}
	inline void __cdecl MsgBox(TCHAR* pcszFormat, ...)
	{
		MSGBOX_VA_MACRO(NULL, _T("MESSAGE"));
	}
}
