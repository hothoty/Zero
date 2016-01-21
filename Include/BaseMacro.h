#pragma once

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{ if (p) { delete p; p=0; }	}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{ if( p ) delete [] p; p=NULL; }
#endif

#ifndef getrandom
#define getrandom(min, max)		((rand() % (int)((max)-(min)+1)) + (min))
#endif


#ifdef _UNICODE
#define __TFUNCTION__		__FUNCTIONW__
#else
#define __TFUNCTION__		__FUNCTION__
#endif


#define NonCopyClass(name)\
	private:\
		name(const name &src);\
		name &operator=(const name &src)


/* Minimum and maximum macros */
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))


#define Z_RANGE(num,min,max)	( (num) >= (min) && (num) <= (max) )
#define Z_INFINITE				0xFFFFFFFF


#define MacroStringA(e)	#e
#define MacroStringW(e)	CA2W(#e)


#define DECLARE_SetAccess( Datatype, FuncName, Member )	inline void Set##FuncName( Datatype value )	{ Member = value; }
#define DECLARE_GetAccess( Datatype, FuncName, Member )	inline Datatype Get##FuncName()				{ return Member; }
#define DECLARE_FullAccess( Datatype, FuncName, Member )\
	DECLARE_SetAccess( Datatype, FuncName, Member )\
	DECLARE_GetAccess( Datatype, FuncName, Member )




