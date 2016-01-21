#pragma once

// 작업 디파인
//#define DEF_RSA_MODE	// RSA활성화 : C++전용, 해제시 접속시점의 인증관련 데이터가 제거됨(통신량감소)

#ifndef ZASSERT
#define	ZASSERT(condition, state)	if( (condition) == false ) { _ASSERT(condition); state; }
#endif

#ifdef _UNICODE
	#define	ASSERT_THROW(condition)\
		if( (condition) == false ) {\
			_ASSERT(condition);\
			Zero::String txt;\
			txt.Format( _T("%s (%s)"), __TFUNCTION__, (LPCWSTR)CA2W(#condition) );\
			throw Zero::CException(txt);\
		}
	#define FAIL_THROW(condition)\
		if( (condition) == false ) {\
			Zero::String txt;\
			txt.Format( _T("%s (%s)"), __TFUNCTION__, (LPCWSTR)CA2W(#condition) );\
			throw Zero::CException(txt);\
		}
#else
	#define	ASSERT_THROW(condition)\
		if( (condition) == false ) {\
			_ASSERT(condition);\
			Zero::String txt;\
			txt.Format( _T("%s (%s)"), __TFUNCTION__, #condition );\
			throw Zero::CException(txt);\
		}
	#define FAIL_THROW(condition)\
		if( (condition) == false ) {\
			Zero::String txt;\
			txt.Format( _T("%s (%s)"), __TFUNCTION__, #condition );\
			throw Zero::CException(txt);\
		}
#endif





