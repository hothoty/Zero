namespace Zero
{
	inline CSerializer::CSerializer(const CSerializer &src)
	{
		Initialize();
		src.CopyTo(*this);
	}

	inline intptr_t CSerializer::GetBuffSize() const
	{
		return m_bExternalBuffer ? m_nExternalSize : m_Data.Count;
	}

	inline void CSerializer::CopyTo(CSerializer &dest) const
	{
		dest.m_bExternalBuffer = m_bExternalBuffer;
		dest.m_nExternalSize = m_nExternalSize;
		dest.m_Data = m_Data;
		if( dest.m_bExternalBuffer )
		{
			dest.m_pBuffer = m_pBuffer;
		}
		else
		{
			dest.m_pBuffer = dest.m_Data.GetData();
		}
		dest.m_nCursor = m_nCursor;
	}

	template<typename T>
	inline CSerializer& CSerializer::operator<<(const T& data)
	{
		CSerializable* pData = (CSerializable*)&data;
		(*this) << pData;
		return *this;
	}
	template<typename T>
	inline CSerializer& CSerializer::operator<<(const T* data)
	{
		(*this) << (CSerializable*)data;
		return *this;
	}
	template<typename T>
	inline CSerializer& CSerializer::operator>>(T& data)
	{
		(*this) >> (CSerializable*)&data;
		return *this;
	}
	template<typename T>
	inline CSerializer& CSerializer::operator>>(T* data)
	{
		(*this) >> (CSerializable*)data;
		return *this;
	}

	template<typename elem, bool raw_type, typename size_type>
	inline CSerializer& CSerializer::operator<<(const CSafeArray<elem, raw_type, size_type>& Ar)
	{
		Int32 arrSize = (Int32)Ar.Count;
		(*this) << arrSize;
		for( Int32 i = 0; i < arrSize; i++ )
		{
			(*this) << Ar[i];
		}
		return *this;
	}
	template<typename elem, bool raw_type, typename size_type>
	inline CSerializer& CSerializer::operator>>(CSafeArray<elem, raw_type, size_type>& Ar)
	{
		Int32 nNum = 0;
		(*this) >> nNum;
		Ar.Clear();
		Ar.Resize( nNum, true, true );
		for( Int32 i = 0; i < nNum; i++ )
		{
			(*this) >> Ar[i];
		}
		return *this;
	}

	template<typename elem>
	inline CSerializer& CSerializer::operator<<(const std::vector<elem>& Ar)
	{
		Int32 arrSize = (Int32)Ar.size();
		(*this) << arrSize;
		for( Int32 i = 0; i < arrSize; i++ )
		{
			(*this) << Ar[i];
		}
		return *this;
	}
	template<typename elem>
	inline CSerializer& CSerializer::operator>>(std::vector<elem>& Ar)
	{
		Int32 nNum = 0;
		(*this) >> nNum;
		Ar.reserve( nNum );
		for( Int32 i = 0; i < nNum; i++ )
		{
			elem e;
			(*this) >> e;
			Ar.push_back( e );
			
		}
		return *this;
	}

	template<class K, class T>
	inline CSerializer& CSerializer::operator<<(const TMap<K,T>& Ar)
	{
		Int32 nNum = (Int32)Ar.Count;
		(*this) << nNum;

		TMap<K,T>::iterator citrp = Ar.begin();
		TMap<K,T>::iterator eitrp = Ar.end();

		for(; citrp != eitrp; citrp++)
		{
			(*this) << ((*citrp).first);
			(*this) << ((*citrp).second);
		}
		return *this;
	}

	template<class K, class T>
	inline CSerializer& CSerializer::operator>>(TMap<K,T>& Ar)
	{
		Int32 nNum = 0;
		(*this) >> nNum;

		Ar.Clear();

		for( intptr_t i = 0; i < nNum; i++ )
		{
			K key;
			T Temp;
			(*this) >> key;
			(*this) >> Temp;
			Ar.Add( key, Temp );
		}
		return *this;
	}

	template<typename K, typename V>
	inline CSerializer& CSerializer::operator<<(const CDictionary<K,V>& Ar)
	{
		Int32 nNum = (Int32)Ar.Count;
		(*this) << nNum;

		typedef typename CDictionary<K,V>::const_iterator ConstIter;

		for( auto iter : Ar )
		{
			(*this) << iter.first;
			(*this) << iter.second;
		}
		return *this;
	}
		
	template<typename K, typename V>
	inline CSerializer& CSerializer::operator>>(CDictionary<K,V>& Ar)
	{
		Ar.Clear();

		Int32 nNum = 0;
		(*this) >> nNum;

		// 맵 크기체크
		if( nNum<0 || nNum >= MaxSizeMap )
		{
			Throw("Dictionary Size Error");
			return *this;
		}

		for( intptr_t i=0; i<nNum; i++ )
		{
			K key; V val;
			(*this) >> key;
			(*this) >> val;
			Ar.Add(key,val);
		}
		return *this;
	}


	// enum 자동화(32비트)
	#define AUTO_SERIALIZE_ENUM(enumValue)\
	inline Zero::CSerializer& operator<<(Zero::CSerializer &a, enumValue b) {\
		Int32 temp = (Int32)b;\
		a.PushData( (char*)&temp, sizeof(Int32) );\
		return a;\
	}\
	inline Zero::CSerializer& operator>>(Zero::CSerializer &a, enumValue &b) {\
		Int32 temp;\
		a.PopData( (char*)&temp, sizeof(Int32) );\
		b = (enumValue)temp;\
		return a;\
	}

	// enum 16비트 짜리
	#define AUTO_SERIALIZE_ENUM16(enumValue)\
	inline Zero::CSerializer& operator<<(Zero::CSerializer &a, enumValue b) {\
		Int16 temp = (Int16)b;\
		a.PushData( (char*)&temp, sizeof(Int16) );\
		return a;\
	}\
	inline Zero::CSerializer& operator>>(Zero::CSerializer &a, enumValue &b) {\
		Int16 temp;\
		a.PopData( (char*)&temp, sizeof(Int16) );\
		b = (enumValue)temp;\
		return a;\
	}

	// enum 8비트
	#define AUTO_SERIALIZE_ENUM8(enumValue)\
	inline Zero::CSerializer& operator<<(Zero::CSerializer &a, enumValue b) {\
		sbyte temp = (sbyte)b;\
		a.PushData( (char*)&temp, sizeof(sbyte) );\
		return a;\
	}\
	inline Zero::CSerializer& operator>>(Zero::CSerializer &a, enumValue &b) {\
		sbyte temp;\
		a.PopData( (char*)&temp, sizeof(sbyte) );\
		b = (enumValue)temp;\
		return a;\
	}

	/**
	Serialize Macro
	*/
	#define SERIAL_IN(val)	clsSerial << val
	#define SERIAL_OT(val)	clsSerial >> val

	#define AUTO_SERIALIZE_IN() \
		virtual void Serialize(bool bStore, Zero::CSerializer& clsSerial) override { \
			if( bStore ) {

	#define AUTO_SERIALIZE_OT() \
		} \
		else {

	#define AUTO_SERIALIZE_END() } }

	#define AUTO_SERIALIZE1(val1)\
		AUTO_SERIALIZE_IN()	SERIAL_IN(val1); \
		AUTO_SERIALIZE_OT()	SERIAL_OT(val1); \
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE2(val1,val2)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE3(val1,val2,val3)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE4(val1,val2,val3,val4)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE5(val1,val2,val3,val4,val5)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE6(val1,val2,val3,val4,val5,val6)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE7(val1,val2,val3,val4,val5,val6,val7)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE8(val1,val2,val3,val4,val5,val6,val7,val8)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE9(val1,val2,val3,val4,val5,val6,val7,val8,val9)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE10(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE11(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE12(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE13(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE14(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE15(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE16(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE17(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE18(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE19(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19);\
		AUTO_SERIALIZE_END()


	#define AUTO_SERIALIZE20(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE21(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE22(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE23(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22,val23)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22); SERIAL_IN(val23);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22); SERIAL_OT(val23);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE24(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22,val23,val24)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22); SERIAL_IN(val23); SERIAL_IN(val24);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22); SERIAL_OT(val23); SERIAL_OT(val24);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE25(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22,val23,val24,val25)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22); SERIAL_IN(val23); SERIAL_IN(val24); SERIAL_IN(val25);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22); SERIAL_OT(val23); SERIAL_OT(val24); SERIAL_OT(val25);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE26(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22,val23,val24,val25,val26)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22); SERIAL_IN(val23); SERIAL_IN(val24); SERIAL_IN(val25); SERIAL_IN(val26);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22); SERIAL_OT(val23); SERIAL_OT(val24); SERIAL_OT(val25); SERIAL_OT(val26);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE27(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22,val23,val24,val25,val26,val27)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22); SERIAL_IN(val23); SERIAL_IN(val24); SERIAL_IN(val25); SERIAL_IN(val26); SERIAL_IN(val27);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22); SERIAL_OT(val23); SERIAL_OT(val24); SERIAL_OT(val25); SERIAL_OT(val26); SERIAL_OT(val27);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE28(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22,val23,val24,val25,val26,val27,val28)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22); SERIAL_IN(val23); SERIAL_IN(val24); SERIAL_IN(val25); SERIAL_IN(val26); SERIAL_IN(val27); SERIAL_IN(val28);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22); SERIAL_OT(val23); SERIAL_OT(val24); SERIAL_OT(val25); SERIAL_OT(val26); SERIAL_OT(val27); SERIAL_OT(val28);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE29(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22,val23,val24,val25,val26,val27,val28,val29)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22); SERIAL_IN(val23); SERIAL_IN(val24); SERIAL_IN(val25); SERIAL_IN(val26); SERIAL_IN(val27); SERIAL_IN(val28); SERIAL_IN(val29);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22); SERIAL_OT(val23); SERIAL_OT(val24); SERIAL_OT(val25); SERIAL_OT(val26); SERIAL_OT(val27); SERIAL_OT(val28); SERIAL_OT(val29);\
		AUTO_SERIALIZE_END()

	#define AUTO_SERIALIZE30(val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,val13,val14,val15,val16,val17,val18,val19,val20,val21,val22,val23,val24,val25,val26,val27,val28,val29,val30)\
		AUTO_SERIALIZE_IN() SERIAL_IN(val1); SERIAL_IN(val2); SERIAL_IN(val3); SERIAL_IN(val4); SERIAL_IN(val5); SERIAL_IN(val6); SERIAL_IN(val7); SERIAL_IN(val8); SERIAL_IN(val9); SERIAL_IN(val10); SERIAL_IN(val11); SERIAL_IN(val12); SERIAL_IN(val13); SERIAL_IN(val14); SERIAL_IN(val15); SERIAL_IN(val16); SERIAL_IN(val17); SERIAL_IN(val18); SERIAL_IN(val19); SERIAL_IN(val20); SERIAL_IN(val21); SERIAL_IN(val22); SERIAL_IN(val23); SERIAL_IN(val24); SERIAL_IN(val25); SERIAL_IN(val26); SERIAL_IN(val27); SERIAL_IN(val28); SERIAL_IN(val29); SERIAL_IN(val30);\
		AUTO_SERIALIZE_OT() SERIAL_OT(val1); SERIAL_OT(val2); SERIAL_OT(val3); SERIAL_OT(val4); SERIAL_OT(val5); SERIAL_OT(val6); SERIAL_OT(val7); SERIAL_OT(val8); SERIAL_OT(val9); SERIAL_OT(val10); SERIAL_OT(val11); SERIAL_OT(val12); SERIAL_OT(val13); SERIAL_OT(val14); SERIAL_OT(val15); SERIAL_OT(val16); SERIAL_OT(val17); SERIAL_OT(val18); SERIAL_OT(val19); SERIAL_OT(val20); SERIAL_OT(val21); SERIAL_OT(val22); SERIAL_OT(val23); SERIAL_OT(val24); SERIAL_OT(val25); SERIAL_OT(val26); SERIAL_OT(val27); SERIAL_OT(val28); SERIAL_OT(val29); SERIAL_OT(val30);\
		AUTO_SERIALIZE_END()
}


