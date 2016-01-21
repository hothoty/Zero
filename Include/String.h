#pragma once
#include <string>
#include <algorithm>
#include "String_.inl"


/**

StdString의 오래된 함수와 불안한 기능들을 보완한 String

*/
namespace Zero
{
	template<typename CT>
	class CStdStr : public std::basic_string<CT>
	{
		enum
		{
			MAX_FMT_TRIES = 5,		// #of times we try 
			FMT_BLOCK_SIZE = 2048,	// #of bytes to increment per try
			BUFSIZE_1ST = 256,
			BUFSIZE_2ND = 512,
			STD_BUF_SIZE = 1024,
		};
		typedef typename std::basic_string<CT>		MYBASE;	 // my base class
		typedef typename MYBASE::const_pointer		PCMYSTR; // PCSTR or PCWSTR 
		typedef typename MYBASE::pointer			PMYSTR;	 // PSTR or PWSTR
		typedef typename MYBASE::iterator			MYITER;  // my iterator type
		typedef typename MYBASE::const_iterator		MYCITER; // you get the idea...
		typedef typename MYBASE::reverse_iterator	MYRITER;
		typedef typename MYBASE::size_type			MYSIZE;   
		typedef typename MYBASE::value_type			MYVAL; 
		typedef typename MYBASE::allocator_type		MYALLOC;
	
	public:
		CStdStr();
		CStdStr(const CStdStr<CT>& str) : MYBASE(SSREF(str)) {}
		CStdStr(const std::string& str);
		CStdStr(const std::wstring& str);
		CStdStr(const char* str);
		CStdStr(const wchar_t* str);
		CStdStr(PCMYSTR pT, MYSIZE n) : MYBASE(pT, n) {}
		CStdStr(MYCITER first, MYCITER last) : MYBASE(first, last) {}
		CStdStr(MYSIZE nSize, MYVAL ch, const MYALLOC& al=MYALLOC()) : MYBASE(nSize, ch, al) {}

		CStdStr<CT>& operator=(const CStdStr<CT>& str);
		CStdStr<CT>& operator=(const std::string& str);
		CStdStr<CT>& operator=(const std::wstring& str);
		CStdStr<CT>& operator=(PCSTR pA);
		CStdStr<CT>& operator=(PCWSTR pW);
		CStdStr<CT>& operator=(CT t);
		CStdStr<CT>& operator+=(const CStdStr<CT>& str);
		CStdStr<CT>& operator+=(const std::string& str);
		CStdStr<CT>& operator+=(const std::wstring& str);
		CStdStr<CT>& operator+=(PCSTR pA);
		CStdStr<CT>& operator+=(PCWSTR pW);
		CStdStr<CT>& operator+=(CT t);

		CStdStr<CT>& Trim();
		CStdStr<CT>& TrimLeft();
		CStdStr<CT>& TrimLeft(CT tTrim);
		CStdStr<CT>& TrimLeft(PCMYSTR szTrimChars);
		CStdStr<CT>& TrimRight();
		CStdStr<CT>& TrimRight(CT tTrim);
		CStdStr<CT>& TrimRight(PCMYSTR szTrimChars);

		CStdStr<CT>& Normalize();
		CStdStr<CT>& ToUpper();
		CStdStr<CT>& ToLower();
		CStdStr<CT> Mid(int nFirst ) const;
		CStdStr<CT> Mid(int nFirst, int nCount) const;
		CStdStr<CT> Left(int nCount) const;
		CStdStr<CT> Right(int nCount) const;
		CStdStr<CT> SpanExcluding(PCMYSTR szCharSet) const;
		CStdStr<CT> SpanIncluding(PCMYSTR szCharSet) const;

		CT& operator[](int nIdx);
		CT& operator[](unsigned int nIdx);
		const CT& operator[](int nIdx) const;
		const CT& operator[](unsigned int nIdx) const;

		operator const CT*() const;

		inline const CT* GetString() const { return c_str(); }
		
		bool Equals(const CT* pT, bool bUseCase=false) const;

		void MakeLower();
		void MakeReverse();
		void MakeUpper();

		int Insert(int nIdx, CT ch);
		int Insert(int nIdx, PCMYSTR sz);

		int Remove(CT ch);
		int Replace(CT chOld, CT chNew);
		int Replace(PCMYSTR szOld, PCMYSTR szNew);

		// ReverseFind overload that's not in CString but might be useful
		int ReverseFind(PCMYSTR szFind, MYSIZE pos=MYBASE::npos) const;
		int ReverseFind(CT ch) const;

		int Delete(int nIdx, int nCount=1);
		int Collate(PCMYSTR szThat) const;
		int CollateNoCase(PCMYSTR szThat) const;
		int Compare(PCMYSTR szThat) const;
		int CompareNoCase(PCMYSTR szThat) const;

		void FreeExtra();
		void SetAt(int nIndex, CT ch);
		
		void Empty();
		bool IsEmpty() const;

		// GetLength() -- MFC docs say this is the # of BYTES but
		// in truth it is the number of CHARACTERs (chars or wchar_ts)
		int GetLength() const;


		// CString::Find docs say add 1 to nStart when it's not zero
		// CString::Find code doesn't do that however.  We'll stick
		// with what the code does
		int Find(CT ch, int nStart) const;
		int Find(PCMYSTR szSub, int nStart) const;
		int Find(CT ch) const;
		int Find(PCMYSTR szSub) const;
		int FindOneOf(PCMYSTR szCharSet) const;



		CT* GetBuf(int nMinLen=-1);
		CT* SetBuf(int nLen);
		void RelBuf(int nNewLen=-1);
		void BufferRel();			// backwards compatability
		CT*  Buffer();				// backwards compatability
		CT*  BufferSet(int nLen);	// backwards compatability

		
		void Format(const CT* szFmt, ...);
		void FormatMessage(PCMYSTR szFormat, ...);
		void FormatMessage(UINT nFormatId, ...);
		void AppendFormat(const CT* szFmt, ...);
		void AppendFormatV(const CT* szFmt, va_list argList);


		// -------------------------------------------------------------------------
		// FUNCTION:  FormatV
		//		void FormatV(PCSTR szFormat, va_list, argList);
		//           
		// DESCRIPTION:
		//		This function formats the string with sprintf style format-specs. 
		//		It makes a general guess at required buffer size and then tries
		//		successively larger buffers until it finds one big enough or a
		//		threshold (MAX_FMT_TRIES) is exceeded.
		//
		// PARAMETERS: 
		//		szFormat - a PCSTR holding the format of the output
		//		argList - a Microsoft specific va_list for variable argument lists
		//
		// RETURN VALUE: 
		// -------------------------------------------------------------------------
		void FormatV(const CT* szFormat, va_list argList);
		

		// -------------------------------------------------------------------------
		// GetXXXX -- Direct access to character buffer
		// -------------------------------------------------------------------------
		CT GetAt(int nIdx) const;
		CT* GetBuffer(int nMinLen=-1);
		CT* GetBufferSetLength(int nLen);
		void ReleaseBuffer(int nNewLen=-1);
	};

	typedef CStdStr<char>		StringA;
	typedef CStdStr<wchar_t>	StringW;

	#ifdef _UNICODE
		typedef StringW			String;
		#define Num2StrComma	Num2StrCommaW
	#else
		typedef StringA			String;
		#define Num2StrComma	Num2StrCommaA
	#endif



	// wchar <-> char 변환
	StringA ZW2A(const wchar_t *src);
	StringW ZA2W(const char *src);

	// 숫자 -> ,포함된 스트링으로
	void Num2StrCommaW(const Int64 n64Number, OUT StringW& strNum);
	void Num2StrCommaA(const Int64 n64Number, OUT StringA& strNum);
}

#include "String.inl"


