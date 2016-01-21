#pragma once
#include <atlcomtime.h>
#include "String.h"

//#define DEF_MAKE_ADO
//#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename("EOF", "EndOfFile") no_namespace
#ifdef DEF_MAKE_ADO
	#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF", "EndOfFile")rename("BOF","adoBOF")
#else
	#include "msado15.tlh"
#endif

#ifdef _UNICODE
#define DBFAIL_THROW(a)\
	if(!(a))\
	{\
		Zero::String str;\
		str.Format( _T("FAIL : %s, %s"), __FUNCTIONW__, (LPCWSTR)CA2W(#a) );\
		throw Zero::DBException( str );\
	}

#else
#define DBFAIL_THROW(a)\
	if(!(a))\
	{\
		Zero::String str;\
		str.Format(_T("FAIL : %s, %s"), __FUNCTIONW__, #a);\
		throw Zero::DBException(str);\
	}
#endif


namespace Zero
{
	class DBException : public CException
	{
	public:
		DBException(const TCHAR* msg);
		DBException(const TCHAR* msg,_com_error &e);
	};

	struct StartOptionAdo
	{
	public:
		StartOptionAdo();

		DECLARE_FullAccess(String,	InitCatalog,		m_strInitialCatalog);
		DECLARE_FullAccess(String,	UserID,				m_strUserID);
		DECLARE_FullAccess(String,	Password,			m_strPassword);
		DECLARE_FullAccess(int,		CommandTimeout,		m_nCommandTimeout);
		DECLARE_FullAccess(int,		ConnectionTimeout,	m_nConnectionTimeout);
		DECLARE_FullAccess(bool,	RetryConnection,	m_bRetryConnection);
		DECLARE_FullAccess(int,		MaxConnectionPool,	m_nMaxConnectionPool);


		/**
		timeout : 타임아웃(초단위)
		retry : 재연결 기능

		poolsize : db 커넥션 풀 개수(최소1개이상)
		provider : MSSQL = "SQLOLEDB.1", MySQL = "MSDASQL.1"

		*/
		void SetParam(String ip, String userID, String password, String catalog, int timeout=3, bool retry=true, int poolsize=20, String provider = _T("SQLOLEDB.1"));

		String GetConnectionString();
		String& GetProvider();
		String& GetDSN();

	private:
		String m_strConnectingString;
		String m_strInitialCatalog;
		String m_strDataSource;
		String m_strUserID;
		String m_strPassword;
		String m_strProvider;
		String m_strDSN;

		int m_nConnectionTimeout;
		int m_nCommandTimeout;
		bool m_bRetryConnection;
		int m_nMaxConnectionPool;

		void SetIP(String pString);
		void SetDSN(String pString);
		void SetProvider(String pString);
	};



	class CAdo
	{
	public:
		CAdo(StartOptionAdo&);
		~CAdo();

		void Init();
		BOOL Open(CursorLocationEnum CursorLocation=adUseClient);
		void Close();
		void Release();		// 커넥션풀에서 재사용하기 위한 커맨드 객체 재생성
		void SetQuery(IN const TCHAR* tszQuery);
		void SetConnectionMode(ConnectModeEnum nMode);
		void ManualScopeRelease();

		DECLARE_FullAccess(bool, AutoCommit, m_bAutoCommit);
		DECLARE_FullAccess(BOOL, Success, m_IsSuccess);


		// 명시적 트랜잭션 사용 - CScopedAdo 생성/소멸시 필요(CScopedAdo참조)
		void BeginTransaction();
		void CommitTransaction();
		void RollbackTransaction();

		BOOL IsSuccess();
		void SetCommit(BOOL bIsSuccess);

		void dump_com_error(_com_error&);
		void dump_user_error();

		BOOL GetFieldCount(int&);
		void MoveNext();
		BOOL GetEndOfFile();
		BOOL NextRecordSet();


		/**
		프로시저 및 SQL Text을 실행한다.
		- 부가기능 adCmdStoreProc, adCmdText처리 가능
		@param		CommandTypeEnum, ExecuteOptionEnum
		@return		성공(TRUE) 실패(FLASE)
		*/
		BOOL Execute(CommandTypeEnum CommandType = adCmdStoredProc, ExecuteOptionEnum OptionType = adOptionUnspecified);


		/**
		정수/실수/날짜시간 필드값을 읽는다.
		- 읽은 값이 null이면 실패를 리턴한다.

		@return		성공(TRUE) 실패(FLASE)
		@param indexID 컬럼이름(스트링) 또는 인덱스번호(숫자)를 넣을수 있음

		주의 : 인덱스번호를 숫자로 넣을경우 반드시 short형으로 입력하여야한다 0번인덱스 ---> (short)0

		*/
		template<typename T> BOOL GetFieldValue(IN const _variant_t& indexID, OUT T& Value);


		/**
		문자형 필드값을 읽는다.
		- 읽은 값이 null이거나 버퍼가 작다면 실패를 리턴한다.
		@param		읽은 문자을 담을 버퍼의 크기
		@return		성공(TRUE) 실패(FLASE)
		*/
		BOOL GetFieldValue(IN const _variant_t& indexID, OUT Zero::String& outString);


		/**
		binary 필드값을 읽는다.
		- 읽은 값이 null이거나 버퍼가 작다면 실패를 리턴한다.
		@param		읽은 binary을 담을 버퍼의 크기, 읽은 binary 크기
		@return		성공(TRUE) 실패(FLASE)
		*/
		BOOL GetFieldValue(IN const _variant_t& indexID, OUT BYTE* pbyBuffer, IN int inSize, OUT int& outSize);


		/**
		Guid처리
		*/
		BOOL GetFieldValue(IN const _variant_t& indexID, OUT Zero::Guid& outGuid);
	

		/**
		정수/실수/날짜시간 타입의 파라메터 생성
		- null값의 파라메터 생성은 CreateNullParameter을 사용
		*/
		template<typename T> void CreateParam(IN TCHAR* tszName,IN enum DataTypeEnum Type, IN enum ParameterDirectionEnum Direction, IN T rValue);


		/**
		정수/실수/날짜시간 타입의 null값 파라메터 생성
		*/
		void CreateNullParameter(IN TCHAR*, IN enum DataTypeEnum, IN enum ParameterDirectionEnum);

		
		/**
		문자열 타입 파라메터 생성, 길이 변수는 최소 0보다 커야 한다. null값 생성은 TCHAR*에 NULL값을 넘긴다.
		*/
		void CreateParam(IN TCHAR* tszName,IN enum DataTypeEnum Type, IN enum ParameterDirectionEnum Direction, IN TCHAR* pValue, IN int nSize);


		/**
		String 파라미터 생성

		@param nSize : String param크기 지정( 0인경우 str의 크기로 자동 지정 )

		- adParamInput상황에서는 nSize값을 넣지말고 사용 : 디폴트값 0인경우 자동으로 스트링 사이즈로 계산됨

		- adParamOutput 또는 adParamInputOutput로 Output으로 사용하는경우

		  nSize값을 잘 지정해서 사용해야 스트링이 짤리지 않게 읽어올수 있다(SP param값과 같거나 크게)

		*/
		void CreateParam(IN TCHAR* tszName,IN enum DataTypeEnum Type, IN enum ParameterDirectionEnum Direction, IN const String& str, IN int nSize = 0);

		/**
		binary 타입 파라메터 생성, 길이 변수는 최소 0보다 커야 한다. null값 생성은 BYTE*에 NULL값을 넘긴다.
		*/
		void CreateParam(IN TCHAR* tszName,IN enum DataTypeEnum Type, IN enum ParameterDirectionEnum Direction, IN BYTE* pValue, IN int nSize);


		/**
		정수/실수/날짜시간 타입의 파라메터값 변경
		- null값의 파라메터 변경은 UpdateNullParameter을 사용
		*/
		template<typename T> void UpdateParameter(IN TCHAR* tszName, IN T rValue);

		// 정수/실수/날짜시간 타입의 파라메터 값을 null로 변경
		void UpdateNullParameter(IN TCHAR*);

		// 문자열 타입 파라메터 변경, 길이 변수는 최소 0보다 커야 한다. null값 변경 TCHAR*에 NULL값을 넘긴다.
		void UpdateParameter(IN TCHAR*, IN TCHAR*, IN int);

		// binary 타입 파라메터 변경, 길이 변수는 최소 0보다 커야 한다. null값 변경 BYTE*에 NULL값을 넘긴다.
		void UpdateParameter(IN TCHAR*, IN BYTE*, IN int);

		// 정수/실수/날짜시간 타입의 파라메터 값 읽기
		template<typename T> BOOL GetParam(TCHAR* tszName, OUT T& Value);


		/**
		문자형 파라메터값을 읽는다.
		- 읽은 값이 null이면 실패를 리턴한다.
		@return		성공(TRUE) 실패(FLASE)
		*/
		BOOL GetParam(IN TCHAR*, OUT Zero::String&);

		/**
		바이너리형 파라메터값을 읽는다.
		- 읽은 값이 null이거나 버퍼가 작다면 실패를 리턴한다.
		@param		읽은 문자을 담을 버퍼의 크기, 읽은 버퍼의 크기
		@return		성공(TRUE) 실패(FLASE)
		*/
		BOOL GetParam(IN TCHAR*, OUT BYTE*, IN int, OUT int&);


	private:
		_ConnectionPtr m_pConnection;      
		_RecordsetPtr m_pRecordset;
		_CommandPtr m_pCommand;

		String m_strConnectingString;
		String m_strUserID;
		String m_strPassword;
		String m_strInitCatalog;
		String m_strProvider;
		String m_strDSN;
		int m_nConnectionTimeout;
		int m_nCommandTimeout;
		bool m_bRetryConnection;
		bool m_bAutoCommit;

		String m_strQuery;
		BOOL m_IsSuccess;
		String m_strCommand;
		String m_strColumnName;
		String m_strParameterName;

		// 재연결 옵션이 있을 경우 재연결 시도
		BOOL RetryOpen();

		CAdo(const CAdo&);
		CAdo& operator= (const CAdo&);
	};

}

#include "Ado.inl"





