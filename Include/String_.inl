namespace Zero
{
	#ifndef ASSERT
		#define ASSERT(f) assert((f))
	#endif
	#define SSREF(x) (x)
	#if !defined(PCUSTR) && !defined(PCUSTR_DEFINED)
		typedef const unsigned char*	PCUSTR;
		typedef unsigned char*			PUSTR;
		#define PCUSTR_DEFINED
	#endif
	#ifndef UNUSED
		#define UNUSED(x) x
	#endif

	// -----------------------------------------------------------------------------
	// MIN and MAX.  The Standard C++ template versions go by so many names (at
	// at least in the MS implementation) that you never know what's available 
	// -----------------------------------------------------------------------------
	template<class Type>
	inline const Type& SSMIN(const Type& arg1, const Type& arg2)
	{
		return arg2 < arg1 ? arg2 : arg1;
	}
	template<class Type>
	inline const Type& SSMAX(const Type& arg1, const Type& arg2)
	{
		return arg2 > arg1 ? arg2 : arg1;
	}

	typedef std::string::size_type		SS_SIZETYPE; // just for shorthand, really
	typedef std::string::pointer		SS_PTRTYPE;  
	typedef std::wstring::size_type		SW_SIZETYPE;
	typedef std::wstring::pointer		SW_PTRTYPE;  

	// Define our conversion macros to look exactly like Microsoft's to
	// facilitate using this stuff both with and without MFC/ATL

	#ifndef _DEBUG
		#define SSCVT int _cvt; _cvt; UINT _acp=CP_ACP; _acp;\
				PCWSTR _pw; _pw; PCSTR _pa; _pa
	#else
		#define SSCVT int _cvt = 0; _cvt; UINT _acp=CP_ACP; \
			_acp; PCWSTR _pw=0; _pw; PCSTR _pa=0; _pa
	#endif

	#define SSA2W(pa) (\
		((_pa = pa) == 0) ? 0 : (\
			_cvt = (int)(strlen(_pa)+1),\
			StdCodeCvt((PWSTR) _alloca(_cvt*2), _pa, _cvt)))
	#define SSW2A(pw) (\
		((_pw = pw) == 0) ? 0 : (\
			_cvt = (int)(wcslen(_pw)+1)*2,\
			StdCodeCvt((LPSTR) _alloca(_cvt), _pw, _cvt)))

	#define SSA2CW(pa) ((PCWSTR)SSA2W((pa)))
	#define SSW2CA(pw) ((PCSTR)SSW2A((pw)))



	// -----------------------------------------------------------------------------
	// sslen: strlen/wcslen wrappers
	// -----------------------------------------------------------------------------
	template<typename CT> inline int sslen(const CT* pT)
	{
		return (int)(0 == pT ? 0 : std::char_traits<CT>::length(pT));
	}
	inline int sslen(const std::string& s)
	{
		return (int)s.length();
	}
	inline int sslen(const std::wstring& s)
	{
		return (int)s.length();
	}



	inline PWSTR StdCodeCvt(PWSTR pW, PCSTR pA, int nChars, UINT acp=CP_ACP)
	{
		ASSERT(0 != pA);
		ASSERT(0 != pW);
		pW[0] = '\0';
		MultiByteToWideChar(acp, 0, pA, -1, pW, nChars);
		return pW;
	}
	inline PWSTR StdCodeCvt(PWSTR pW, PCUSTR pA, int nChars, UINT acp=CP_ACP)
	{
		return StdCodeCvt(pW, (PCSTR)pA, nChars, acp);
	}

	inline PSTR StdCodeCvt(PSTR pA, PCWSTR pW, int nChars, UINT acp=CP_ACP)
	{
		ASSERT(0 != pA);
		ASSERT(0 != pW);
		pA[0] = '\0';
		WideCharToMultiByte(acp, 0, pW, -1, pA, nChars, 0, 0);
		return pA;
	}
	inline PUSTR StdCodeCvt(PUSTR pA, PCWSTR pW, int nChars, UINT acp=CP_ACP)
	{
		return (PUSTR)StdCodeCvt((PSTR)pA, pW, nChars, acp);
	}




	inline void	ssasn(std::string& sDst, const std::string& sSrc)
	{
		if ( sDst.c_str() != sSrc.c_str() )
		{
			sDst.erase();
			sDst.assign(SSREF(sSrc));
		}
	}
	inline void	ssasn(std::string& sDst, PCSTR pA)
	{
		// Watch out for NULLs, as always.

		if ( 0 == pA )
		{
			sDst.erase();
		}

		// If pA actually points to part of sDst, we must NOT erase(), but
		// rather take a substring

		else if ( pA >= sDst.c_str() && pA <= sDst.c_str() + sDst.size() )
		{
			sDst =sDst.substr(static_cast<SS_SIZETYPE>(pA-sDst.c_str()));
		}

		// Otherwise (most cases) apply the assignment bug fix, if applicable
		// and do the assignment

		else
		{
			sDst.assign(pA);
		}
	}
	inline void	ssasn(std::string& sDst, const std::wstring& sSrc)
	{
		SSCVT;
		sDst.assign(SSW2CA(sSrc.c_str()));
	}
	inline void	ssasn(std::string& sDst, PCWSTR pW)
	{
		SSCVT;
		sDst.assign(pW ? SSW2CA(pW) : "");
	}
	inline void ssasn(std::string& sDst, const int nNull)
	{
		UNUSED(nNull);
		ASSERT(nNull==0);
		sDst.assign("");
	}	
	inline void	ssasn(std::wstring& sDst, const std::wstring& sSrc)
	{
		if ( sDst.c_str() != sSrc.c_str() )
		{
			sDst.erase();
			sDst.assign(SSREF(sSrc));
		}
	}
	inline void	ssasn(std::wstring& sDst, PCWSTR pW)
	{
		// Watch out for NULLs, as always.

		if ( 0 == pW )
		{
			sDst.erase();
		}

		// If pW actually points to part of sDst, we must NOT erase(), but
		// rather take a substring

		else if ( pW >= sDst.c_str() && pW <= sDst.c_str() + sDst.size() )
		{
			sDst = sDst.substr(static_cast<SW_SIZETYPE>(pW-sDst.c_str()));
		}

		// Otherwise (most cases) apply the assignment bug fix, if applicable
		// and do the assignment

		else
		{
			sDst.assign(pW);
		}
	}
	#undef StrSizeType
	inline void	ssasn(std::wstring& sDst, const std::string& sSrc)
	{
		SSCVT;
		sDst.assign(SSA2CW(sSrc.c_str()));
	}
	inline void	ssasn(std::wstring& sDst, PCSTR pA)
	{
		SSCVT;
		sDst.assign(pA ? SSA2CW(pA) : L"");
	}
	inline void ssasn(std::wstring& sDst, const int nNull)
	{
		UNUSED(nNull);
		ASSERT(nNull==0);
		sDst.assign(L"");
	}


	// -----------------------------------------------------------------------------
	// ssadd: string object concatenation -- add second argument to first
	// -----------------------------------------------------------------------------
	inline void	ssadd(std::string& sDst, const std::wstring& sSrc)
	{
		SSCVT; 
		sDst.append(SSW2CA(sSrc.c_str())); 
	}
	inline void	ssadd(std::string& sDst, const std::string& sSrc)
	{ 
		sDst.append(sSrc.c_str());
	}
	inline void	ssadd(std::string& sDst, PCWSTR pW)
	{
		SSCVT;
		if ( 0 != pW )
			sDst.append(SSW2CA(pW)); 
	}
	inline void	ssadd(std::string& sDst, PCSTR pA)
	{
		if ( pA )
			sDst.append(pA); 
	}
	inline void	ssadd(std::wstring& sDst, const std::wstring& sSrc)
	{
		sDst.append(sSrc.c_str());
	}
	inline void	ssadd(std::wstring& sDst, const std::string& sSrc)
	{
		SSCVT;
		sDst.append(SSA2CW(sSrc.c_str()));
	}
	inline void	ssadd(std::wstring& sDst, PCSTR pA)
	{
		SSCVT;
		if ( 0 != pA )
			sDst.append(SSA2CW(pA));
	}
	inline void	ssadd(std::wstring& sDst, PCWSTR pW)
	{
		if ( pW )
			sDst.append(pW);
	}


	inline int	ssnprintf(PSTR pA, size_t nCount, PCSTR pFmtA, va_list vl)
	{ 
		return _vsnprintf_s(pA, nCount, _TRUNCATE, pFmtA, vl);
	}
	inline int	ssnprintf(PWSTR pW, size_t nCount, PCWSTR pFmtW, va_list vl)
	{
		return _vsnwprintf_s(pW, nCount, _TRUNCATE, pFmtW, vl);
	}


	// -----------------------------------------------------------------------------
	// ssicmp: comparison (case insensitive )
	// -----------------------------------------------------------------------------
	inline long sscmp(PCSTR pA1, PCSTR pA2)
	{
		return strcmp(pA1, pA2);
	}
	inline long ssicmp(PCSTR pA1, PCSTR pA2)
	{
		return _stricmp(pA1, pA2);
	}
	inline long sscmp(PCWSTR pW1, PCWSTR pW2)
	{
		return wcscmp(pW1, pW2);
	}
	inline long ssicmp(PCWSTR pW1, PCWSTR pW2)
	{
		return _wcsicmp(pW1, pW2);
	}


	// -----------------------------------------------------------------------------
	// sscoll/ssicoll: Collation wrappers
	// -----------------------------------------------------------------------------
	inline int sscoll(PCSTR sz1, int /*nLen1*/, PCSTR sz2, int /*nLen2*/)
	{
		return strcoll(sz1, sz2);
	}
	inline int ssicoll(PCSTR sz1, int /*nLen1*/, PCSTR sz2, int /*nLen2*/)
	{
		return _stricoll(sz1, sz2);
	}
	inline int sscoll(PCWSTR sz1, int /*nLen1*/, PCWSTR sz2, int /*nLen2*/)
	{
		return wcscoll(sz1, sz2);
	}
	inline int ssicoll(PCWSTR sz1, int /*nLen1*/, PCWSTR sz2, int /*nLen2*/)
	{
		return _wcsicoll(sz1, sz2);
	}


	// -----------------------------------------------------------------------------
	// ssfmtmsg: FormatMessage equivalents.  Needed because I added a CString facade
	// Again -- no equivalent of these on non-Win32 builds but their might one day
	// be one if the message facet gets implemented
	// -----------------------------------------------------------------------------
	inline DWORD ssfmtmsg(DWORD dwFlags, LPCVOID pSrc, DWORD dwMsgId,
						  DWORD dwLangId, PSTR pBuf, DWORD nSize,
						  va_list* vlArgs)
	{ 
		return FormatMessageA(dwFlags, pSrc, dwMsgId, dwLangId,
							  pBuf, nSize,vlArgs);
	}
	inline DWORD ssfmtmsg(DWORD dwFlags, LPCVOID pSrc, DWORD dwMsgId,
						  DWORD dwLangId, PWSTR pBuf, DWORD nSize,
						  va_list* vlArgs)
	{
		return FormatMessageW(dwFlags, pSrc, dwMsgId, dwLangId,
							  pBuf, nSize,vlArgs);
	}



	// This struct is used for TrimRight() and TrimLeft() function implementations.
	//template<typename CT>
	//struct NotSpace : public std::unary_function<CT, bool>
	//{
	//	const std::locale& loc;
	//	inline NotSpace(const std::locale& locArg) : loc(locArg) {}
	//	inline bool operator() (CT t) { return !std::isspace(t, loc); }
	//};
	template<typename CT>
	struct NotSpace : public std::unary_function<CT, bool>
	{
		const std::locale& loc;
		NotSpace(const std::locale& locArg) : loc(locArg) {}

		// DINKUMWARE BUG:
		// Note -- using std::isspace in a COM DLL gives us access violations
		// because it causes the dynamic addition of a function to be called
		// when the library shuts down.  Unfortunately the list is maintained
		// in DLL memory but the function is in static memory.  So the COM DLL
		// goes away along with the function that was supposed to be called,
		// and then later when the DLL CRT shuts down it unloads the list and
		// tries to call the long-gone function.
		// This is DinkumWare's implementation problem.  Until then, we will
		// use good old isspace and iswspace from the CRT unless they
		// specify SS_ANSI
		bool ssisp(char c) const { return FALSE != ::isspace((int) c); }
		bool ssisp(wchar_t c) const { return FALSE != ::iswspace((wint_t) c); }
		bool operator()(CT t) const  { return !ssisp(t); }
	};
}
