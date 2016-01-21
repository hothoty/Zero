namespace Zero
{
	template<typename CT>
	CStdStr<CT>::CStdStr()
	{
	}

	template<typename CT>
	CStdStr<CT>::CStdStr(const std::string& str)
	{
		ssasn(*this, SSREF(str));
	}

	template<typename CT>
	CStdStr<CT>::CStdStr(const std::wstring& str)
	{
		ssasn(*this, SSREF(str));
	}

	template<typename CT>
	CStdStr<CT>::CStdStr(const char* str)
	{
		ssasn(*this, SSREF(str));
	}

	template<typename CT>
	CStdStr<CT>::CStdStr(const wchar_t* str)
	{
		ssasn(*this, SSREF(str));
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator=(const CStdStr<CT>& str)
	{ 
		ssasn(*this, str);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator=(const std::string& str)
	{
		ssasn(*this, str);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator=(const std::wstring& str)
	{
		ssasn(*this, str);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator=(PCSTR pA)
	{
		ssasn(*this, pA);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator=(PCWSTR pW)
	{
		ssasn(*this, pW);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator=(CT t)
	{
		MYBASE::assign(1, t);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator+=(const CStdStr<CT>& str)
	{
		ssadd(*this, str);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator+=(const std::string& str)
	{
		ssadd(*this, str);
		return *this; 
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator+=(const std::wstring& str)
	{
		ssadd(*this, str);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator+=(PCSTR pA)
	{
		ssadd(*this, pA);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator+=(PCWSTR pW)
	{
		ssadd(*this, pW);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::operator+=(CT t)
	{
		append(1, t);
		return *this;
	}


	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::ToUpper()
	{
		if ( !empty() )
			std::transform(begin(), end(), begin(), toupper);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::ToLower()
	{
		if ( !empty() )
			std::transform(begin(), end(), begin(), tolower);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::Normalize()
	{
		return Trim().ToLower();
	}
		
	template<typename CT>
	CT* CStdStr<CT>::GetBuf(int nMinLen=-1)
	{
		if ( static_cast<int>(size()) < nMinLen )
			resize(static_cast<MYSIZE>(nMinLen));

		return empty() ? const_cast<CT*>(data()) : &(at(0));
	}

	template<typename CT>
	CT* CStdStr<CT>::SetBuf(int nLen)
	{
		nLen = ( nLen > 0 ? nLen : 0 );
		if ( capacity() < 1 && nLen == 0 )
			resize(1);

		resize(static_cast<MYSIZE>(nLen));
		return const_cast<CT*>(data());
	}

	template<typename CT>
	void CStdStr<CT>::RelBuf(int nNewLen=-1)
	{
		resize(static_cast<MYSIZE>(nNewLen > -1 ? nNewLen : sslen(c_str())));
	}

	template<typename CT>
	void CStdStr<CT>::BufferRel()
	{
		RelBuf();
	}

	template<typename CT>
	CT*  CStdStr<CT>::Buffer()
	{
		return GetBuf();
	}

	template<typename CT>
	CT*  CStdStr<CT>::BufferSet(int nLen)
	{
		return SetBuf(nLen);
	}

	template<typename CT>
	bool CStdStr<CT>::Equals(const CT* pT, bool bUseCase=false) const
	{	// get copy, THEN compare (thread safe)
		return  bUseCase ? compare(pT) == 0 : ssicmp(CStdStr<CT>(*this), pT) == 0;
	} 
	
	template<typename CT>
	void CStdStr<CT>::Format(const CT* szFmt, ...)
	{
		va_list argList;
		va_start(argList, szFmt);
		FormatV(szFmt, argList);
		va_end(argList);
	}

	template<typename CT>
	void CStdStr<CT>::AppendFormat(const CT* szFmt, ...)
	{
		va_list argList;
		va_start(argList, szFmt);
		AppendFormatV(szFmt, argList);
		va_end(argList);
	}

	template<typename CT>
	void CStdStr<CT>::AppendFormatV(const CT* szFmt, va_list argList)
	{
		CT szBuf[STD_BUF_SIZE];
		int nLen = ssnprintf(szBuf, STD_BUF_SIZE-1, szFmt, argList);
		if ( 0 < nLen )
			append(szBuf, nLen);
	}

	template<typename CT>
	void CStdStr<CT>::FormatV(const CT* szFormat, va_list argList)
	{
		CT* pBuf			= NULL;
		int nChars			= 1;
		int nUsed			= 0;
		size_type nActual	= 0;
		int nTry			= 0;

		do	
		{
			// Grow more than linearly (e.g. 512, 1536, 3072, etc)

			nChars			+= ((nTry+1) * FMT_BLOCK_SIZE);
			pBuf			= reinterpret_cast<CT*>(_alloca(sizeof(CT)*nChars));
			nUsed			= ssnprintf(pBuf, nChars-1, szFormat, argList);

			// Ensure proper NULL termination.

			nActual			= nUsed == -1 ? nChars-1 : SSMIN(nUsed, nChars-1);
			pBuf[nActual+1]= '\0';


		} while ( nUsed < 0 && nTry++ < MAX_FMT_TRIES );

		// assign whatever we managed to format
		assign(pBuf, nActual);
	}

	template<typename CT>
	int CStdStr<CT>::Collate(PCMYSTR szThat) const
	{
		return sscoll(c_str(), (int)length(), szThat, sslen(szThat));
	}

	template<typename CT>
	int CStdStr<CT>::CollateNoCase(PCMYSTR szThat) const
	{
		return ssicoll(c_str(), (int)length(), szThat, sslen(szThat));
	}

	template<typename CT>
	int CStdStr<CT>::Compare(PCMYSTR szThat) const
	{
		return MYBASE::compare(szThat);	
	}

	template<typename CT>
	int CStdStr<CT>::CompareNoCase(PCMYSTR szThat)	const
	{
		return ssicmp(c_str(), szThat);
	}

	template<typename CT>
	int CStdStr<CT>::Delete(int nIdx, int nCount=1)
	{
		if ( nIdx < GetLength() )
			erase(static_cast<MYSIZE>(nIdx), static_cast<MYSIZE>(nCount));

		return GetLength();
	}

	template<typename CT>
	void CStdStr<CT>::Empty()
	{
		erase();
	}

	template<typename CT>
	int CStdStr<CT>::Find(CT ch) const
	{
		MYSIZE nIdx	= find_first_of(ch);
		return static_cast<int>(MYBASE::npos == nIdx  ? -1 : nIdx);
	}

	template<typename CT>
	int CStdStr<CT>::Find(PCMYSTR szSub) const
	{
		MYSIZE nIdx	= find(szSub);
		return static_cast<int>(MYBASE::npos == nIdx ? -1 : nIdx);
	}

	template<typename CT>
	int CStdStr<CT>::Find(CT ch, int nStart) const
	{
		// CString::Find docs say add 1 to nStart when it's not zero
		// CString::Find code doesn't do that however.  We'll stick
		// with what the code does

		MYSIZE nIdx	= find_first_of(ch, static_cast<MYSIZE>(nStart));
		return static_cast<int>(MYBASE::npos == nIdx ? -1 : nIdx);
	}

	template<typename CT>
	int CStdStr<CT>::Find(PCMYSTR szSub, int nStart) const
	{
		// CString::Find docs say add 1 to nStart when it's not zero
		// CString::Find code doesn't do that however.  We'll stick
		// with what the code does

		MYSIZE nIdx	= find(szSub, static_cast<MYSIZE>(nStart));
		return static_cast<int>(MYBASE::npos == nIdx ? -1 : nIdx);
	}

	template<typename CT>
	int CStdStr<CT>::FindOneOf(PCMYSTR szCharSet) const
	{
		MYSIZE nIdx = find_first_of(szCharSet);
		return static_cast<int>(MYBASE::npos == nIdx ? -1 : nIdx);
	}

	template<typename CT>
	void CStdStr<CT>::FormatMessage(PCMYSTR szFormat, ...)
	{
		va_list argList;
		va_start(argList, szFormat);
		PMYSTR szTemp;
		if ( ssfmtmsg(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ALLOCATE_BUFFER,
						szFormat, 0, 0,
						reinterpret_cast<PMYSTR>(&szTemp), 0, &argList) == 0 ||
				szTemp == 0 )
		{
			Throw("out of memory");
		}
		*this = szTemp;
		LocalFree(szTemp);
		va_end(argList);
	}

	template<typename CT>
	void CStdStr<CT>::FormatMessage(UINT nFormatId, ...)
	{
		CStdStr<CT> sFormat;
		VERIFY(sFormat.LoadString(nFormatId) != 0);
		va_list argList;
		va_start(argList, nFormatId);
		PMYSTR szTemp;
		if ( ssfmtmsg(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ALLOCATE_BUFFER,
						sFormat, 0, 0,
						reinterpret_cast<PMYSTR>(&szTemp), 0, &argList) == 0 ||
			szTemp == 0)
		{
			Throw("out of memory");
		}
		*this = szTemp;
		LocalFree(szTemp);
		va_end(argList);
	}


	// -------------------------------------------------------------------------
	// GetXXXX -- Direct access to character buffer
	// -------------------------------------------------------------------------
	template<typename CT>
	CT CStdStr<CT>::GetAt(int nIdx) const
	{
		return at(static_cast<MYSIZE>(nIdx));
	}

	template<typename CT>
	CT* CStdStr<CT>::GetBuffer(int nMinLen=-1)
	{
		return GetBuf(nMinLen);
	}

	template<typename CT>
	CT* CStdStr<CT>::GetBufferSetLength(int nLen)
	{
		return BufferSet(nLen);
	}

	// GetLength() -- MFC docs say this is the # of BYTES but
	// in truth it is the number of CHARACTERs (chars or wchar_ts)
	template<typename CT>
	int CStdStr<CT>::GetLength() const
	{
		return static_cast<int>(length());
	}

	template<typename CT>
	int CStdStr<CT>::Insert(int nIdx, CT ch)
	{
		if ( static_cast<MYSIZE>(nIdx) > size() -1 )
			append(1, ch);
		else
			insert(static_cast<MYSIZE>(nIdx), 1, ch);

		return GetLength();
	}

	template<typename CT>
	int CStdStr<CT>::Insert(int nIdx, PCMYSTR sz)
	{
		if ( nIdx >= size() )
			append(sz, sslen(sz));
		else
			insert(static_cast<MYSIZE>(nIdx), sz);

		return GetLength();
	}

	template<typename CT>
	bool CStdStr<CT>::IsEmpty() const
	{
		return empty();
	}

	template<typename CT>
	CStdStr<CT> CStdStr<CT>::Left(int nCount) const
	{
		return substr(0, static_cast<MYSIZE>(nCount)); 
	}

	template<typename CT>
	void CStdStr<CT>::MakeLower()
	{
		ToLower();
	}

	template<typename CT>
	void CStdStr<CT>::MakeReverse()
	{
		std::reverse(begin(), end());
	}

	template<typename CT>
	void CStdStr<CT>::MakeUpper()
	{ 
		ToUpper();
	}

	template<typename CT>
	CStdStr<CT> CStdStr<CT>::Mid(int nFirst ) const
	{
		return substr(static_cast<MYSIZE>(nFirst));
	}

	template<typename CT>
	CStdStr<CT> CStdStr<CT>::Mid(int nFirst, int nCount) const
	{
		return substr(static_cast<MYSIZE>(nFirst), static_cast<MYSIZE>(nCount));
	}

	template<typename CT>
	void CStdStr<CT>::ReleaseBuffer(int nNewLen=-1)
	{
		RelBuf(nNewLen);
	}

	template<typename CT>
	int CStdStr<CT>::Remove(CT ch)
	{
		MYSIZE nIdx		= 0;
		int nRemoved	= 0;
		while ( (nIdx=find_first_of(ch)) != MYBASE::npos )
		{
			erase(nIdx, 1);
			nRemoved++;
		}
		return nRemoved;
	}

	template<typename CT>
	int CStdStr<CT>::Replace(CT chOld, CT chNew)
	{
		int nReplaced	= 0;
		for ( MYITER iter=begin(); iter != end(); iter++ )
		{
			if ( *iter == chOld )
			{
				*iter = chNew;
				nReplaced++;
			}
		}
		return nReplaced;
	}

	template<typename CT>
	int CStdStr<CT>::Replace(PCMYSTR szOld, PCMYSTR szNew)
	{
		int nReplaced		= 0;
		MYSIZE nIdx			= 0;
		MYSIZE nOldLen		= sslen(szOld);
		if ( 0 == nOldLen )
			return 0;

		static const CT ch	= CT(0);
		MYSIZE nNewLen		= sslen(szNew);
		PCMYSTR szRealNew	= szNew == 0 ? &ch : szNew;

		while ( (nIdx=find(szOld, nIdx)) != MYBASE::npos )
		{
			replace(begin()+nIdx, begin()+nIdx+nOldLen, szRealNew);
			nReplaced++;
			nIdx += nNewLen;
		}
		return nReplaced;
	}

	template<typename CT>
	int CStdStr<CT>::ReverseFind(CT ch) const
	{
		MYSIZE nIdx	= find_last_of(ch);
		return static_cast<int>(MYBASE::npos == nIdx ? -1 : nIdx);
	}

	// ReverseFind overload that's not in CString but might be useful
	template<typename CT>
	int CStdStr<CT>::ReverseFind(PCMYSTR szFind, MYSIZE pos=MYBASE::npos) const
	{
		MYSIZE nIdx	= rfind(0 == szFind ? CStdStr<CT>() : szFind, pos);
		return static_cast<int>(MYBASE::npos == nIdx ? -1 : nIdx);
	}

	template<typename CT>
	CStdStr<CT> CStdStr<CT>::Right(int nCount) const
	{
		nCount = SSMIN(nCount, static_cast<int>(size()));
		return substr(size()-static_cast<MYSIZE>(nCount));
	}

	template<typename CT>
	void CStdStr<CT>::SetAt(int nIndex, CT ch)
	{
		ASSERT(size() > static_cast<MYSIZE>(nIndex));
		at(static_cast<MYSIZE>(nIndex))		= ch;
	}

	template<typename CT>
	CStdStr<CT> CStdStr<CT>::SpanExcluding(PCMYSTR szCharSet) const
	{
		return Left(find_first_of(szCharSet));
	}

	template<typename CT>
	CStdStr<CT> CStdStr<CT>::SpanIncluding(PCMYSTR szCharSet) const
	{
		return Left(find_first_not_of(szCharSet));
	}

	// -------------------------------------------------------------------------
	// Trim and its variants
	// -------------------------------------------------------------------------
	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::Trim()
	{
		return TrimLeft().TrimRight();
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::TrimLeft()
	{
		erase(begin(), std::find_if(begin(),end(),NotSpace<CT>(std::locale())));
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::TrimLeft(CT tTrim)
	{
		erase(0, find_first_not_of(tTrim));
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::TrimLeft(PCMYSTR szTrimChars)
	{
		erase(0, find_first_not_of(szTrimChars));
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::TrimRight()
	{
		std::locale loc;
		MYRITER it = std::find_if(rbegin(), rend(), NotSpace<CT>(loc));
		if ( rend() != it )
			erase(rend() - it);

		erase(it != rend() ? find_last_of(*it) + 1 : 0);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::TrimRight(CT tTrim)
	{
		MYSIZE nIdx	= find_last_not_of(tTrim);
		erase(MYBASE::npos == nIdx ? 0 : ++nIdx);
		return *this;
	}

	template<typename CT>
	CStdStr<CT>& CStdStr<CT>::TrimRight(PCMYSTR szTrimChars)
	{
		MYSIZE nIdx	= find_last_not_of(szTrimChars);
		erase(MYBASE::npos == nIdx ? 0 : ++nIdx);
		return *this;
	}

	template<typename CT>
	void CStdStr<CT>::FreeExtra()
	{
		CStdStr<CT> mt;
		swap(mt);
		if ( !mt.empty() )
			assign(mt.c_str(), mt.size());
	}

	template<typename CT>
	CT& CStdStr<CT>::operator[](int nIdx)
	{
		return MYBASE::operator[](static_cast<MYSIZE>(nIdx));
	}

	template<typename CT>
	const CT& CStdStr<CT>::operator[](int nIdx) const
	{
		return MYBASE::operator[](static_cast<MYSIZE>(nIdx));
	}

	template<typename CT>
	CT& CStdStr<CT>::operator[](unsigned int nIdx)
	{
		return MYBASE::operator[](static_cast<MYSIZE>(nIdx));
	}

	template<typename CT>
	const CT& CStdStr<CT>::operator[](unsigned int nIdx) const
	{
		return MYBASE::operator[](static_cast<MYSIZE>(nIdx));
	}

	template<typename CT>
	CStdStr<CT>::operator const CT*() const
	{
		return c_str();
	}
}
