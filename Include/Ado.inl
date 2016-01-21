namespace Zero
{
	inline void CAdo::SetQuery(IN const TCHAR* tszQuery)
	{
		m_strQuery = tszQuery;
	}

	inline void CAdo::BeginTransaction()
	{
		try
		{
			m_pConnection->BeginTrans();
		}
		catch (_com_error &e)
		{
			dump_com_error(e);
			dump_user_error();
			return;
		}
	}

	inline void CAdo::CommitTransaction()
	{
		try
		{
			m_pConnection->CommitTrans();
		}
		catch(_com_error &e)
		{
			dump_com_error(e);
			dump_user_error();
			return;
		}
	}
	inline void CAdo::RollbackTransaction()
	{
		try
		{
			m_pConnection->RollbackTrans();
		}
		catch(_com_error &e){
			dump_com_error(e);
			dump_user_error();
			return;
		}
	}

	inline BOOL CAdo::IsSuccess()
	{
		if (FALSE == m_IsSuccess)
		{
			dump_user_error();
			m_strQuery.Empty();
			m_strCommand.Empty();
			m_strColumnName.Empty();
			m_strParameterName.Empty();
		}
		return m_IsSuccess;
	}

	inline void CAdo::SetCommit(BOOL bIsSuccess)
	{
		SetSuccess(bIsSuccess);
	}

	template<typename T> BOOL CAdo::GetFieldValue(IN const _variant_t& indexID, OUT T& Value)
	{
		m_strCommand = _T("GetFieldValue(T)");

		try
		{
			if( indexID.vt == VT_BSTR )
			{
				m_strColumnName = (TCHAR*)(_bstr_t(indexID.bstrVal));;
			}
			else
			{
				m_strColumnName.Format( L"index[%d]", (short)indexID );
			}

			_variant_t& vFieldValue = m_pRecordset->GetCollect(indexID);

			switch(vFieldValue.vt)
			{
			case VT_BOOL:		//bool
			case VT_I1:			//BYTE WORD
			case VT_I2:
			case VT_UI1:
			case VT_I4:			//DWORD
			case VT_DECIMAL:	//INT64
			case VT_R8:			//float double
			case VT_DATE:
				Value = vFieldValue;
				break;
			case VT_NULL:
			case VT_EMPTY:
				m_strColumnName += _T(" null value");
				dump_user_error();
				return FALSE;
			default:
				TCHAR tsz[7]={0,};
				String temp;
				temp.Format( _T(" type error(vt = %d ) "), _ltot_s(vFieldValue.vt, tsz, 10) );
				m_strColumnName += temp;
				m_IsSuccess = FALSE;
				return FALSE;
			}
		}
		catch (_com_error &e)
		{
			dump_com_error(e);
			return FALSE;
		}
		return TRUE;
	}

	template<typename T> void CAdo::CreateParam(IN TCHAR* tszName,IN enum DataTypeEnum Type, IN enum ParameterDirectionEnum Direction, IN T rValue)
	{
		if(!IsSuccess()) return;

		m_strCommand = _T("CreateParameter(T)");
		m_strParameterName = tszName;
		try
		{
			_ParameterPtr pParametor = m_pCommand->CreateParameter(tszName,Type,Direction, 0);
			m_pCommand->Parameters->Append(pParametor);
			pParametor->Value = static_cast<_variant_t>(rValue);
		}
		catch (_com_error &e)
		{
			dump_com_error(e);
		}

		return;
	}

	template<typename T> void CAdo::UpdateParameter(IN TCHAR* tszName, IN T rValue)
	{
		if(!IsSuccess()) return;

		m_strCommand = _T("UpdateParameter(T)");
		m_strParameterName = tszName;
		try
		{
			m_pCommand->Parameters->GetItem(tszName)->Value = static_cast<_variant_t>(rValue);
		}
		catch (_com_error &e)
		{
			dump_com_error(e);
		}

		return;
	}

	template<typename T> BOOL CAdo::GetParam(TCHAR* tszName, OUT T& Value)
	{
		if(!IsSuccess()) return FALSE;
		m_IsSuccess = FALSE;

		m_strCommand = _T("GetParam(T)");
		m_strParameterName = tszName;
		try
		{
			_variant_t& vFieldValue = m_pCommand->Parameters->GetItem(tszName)->Value;

			switch(vFieldValue.vt)
			{
			case VT_BOOL:	//bool
			case VT_I1:
			case VT_I2:		//BYTE WORD
			case VT_UI1:
			case VT_I4:		//DWORD
			case VT_DECIMAL: //INT64
			case VT_R8:	//float double
			case VT_DATE:
				Value = vFieldValue;
				break;
			case VT_NULL:
			case VT_EMPTY:
				m_strColumnName += _T(" null value");
				dump_user_error();
				return FALSE;
			default:
				TCHAR tsz[7]={0,};
				String temp;
				temp.Format( _T(" type error(vt = %d ) "), _ltot_s(vFieldValue.vt, tsz, 10) );
				m_strParameterName += temp;
				m_IsSuccess = FALSE;
				return FALSE;
			}
		}
		catch (_com_error &e)
		{
			dump_com_error(e);
			return FALSE;
		}
		return m_IsSuccess = TRUE;
	}
}






