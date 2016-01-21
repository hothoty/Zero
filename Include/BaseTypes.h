#pragma once
#include <assert.h>
#include <stdint.h>

#ifndef __FILEW__          
#define __FILEW__          _STR2WSTR(__FILE__)
#endif

#ifndef __FUNCTIONW__      
#define __FUNCTIONW__      _STR2WSTR(__FUNCTION__)
#endif

#define Throw(msg)			throw std::invalid_argument(msg "(" __FUNCTION__ ")");


/**
기본 데이터형 통일 (C#기준)
*/
typedef signed char        sbyte;
typedef short              Int16;
typedef int                Int32;
typedef long long          Int64;
typedef unsigned char      byte;
typedef unsigned short     UInt16;
typedef unsigned int       UInt32;
typedef unsigned long long UInt64;
