#pragma once
#include "Ado.h"

/**
사용법

- CAdoManager를 이용하여 커넥션 풀을 미리 생성해두고 필요할때마다 CScopedAdo를 이용해 사용한다

@code sample

	// ---------------------------------------------------------
	// -------------    사전 준비 작업    --------------------
	// ---------------------------------------------------------
	// 연결을 위한 컨피그 파일 설정
	Zero::StartOptionAdo start_option;

	Zero::String strIP;
	int nPort = 1234;
	strIP.Format( L"%s,%d",  L"127.0.0.1", nPort );
	start_option.SetParam(strIP, "계정아이디", "계정암호", "DB이름");

	try
	{
		Zero::AdoManager pAdoManager(new Zero::CAdoManager(start_option));
		if( pAdoManager->IsOpenFail() )
		{
			// 기본 설정 실패
			return;
		}
	}
	catch(Zero::DBException& e)
	{
		// 실패
	}
	// ---------------------------------------------------------


	// ---------------------------------------------------------
	// -------------    Query 사용법   --------------------
	// ---------------------------------------------------------
	Zero::CAdo* pAdo = NULL;
	try
	{
		Zero::CScopedAdo scopedado(pAdo, pAdoManager);
		if( !pAdo->IsSuccess() ) return 0;

		pAdo->SetQuery(_T("SELECT uuid FROM Userinfo"));
		pAdo->Execute(adCmdText);
		if( !pAdo->IsSuccess() ) return 0;

		Zero::Guid outGuid;	// 읽어올 값

		while(!pAdo->GetEndOfFile())
		{
			short cnt = 0;
			DBFAIL_THROW(pAdo->GetFieldValue(cnt++, outGuid));

			pAdo->MoveNext();
		}
		if( !pAdo->IsSuccess() ) return 0;
	}
	catch(Zero::DBException& e)
	{
		Zero::ErrMsgA( e.what() );	// 실패
		return 0;
	}
	return 1;
	// ---------------------------------------------------------


	// ---------------------------------------------------------
	// -------------    SP 사용법   --------------------
	// ---------------------------------------------------------
	Zero::String Proc = _T("프로시저이름");
	Zero::CAdo* pAdo = NULL;

	try
	{
		Zero::CScopedAdo scopedado(pAdo, pAdoManager);

		// 입력시킬 파라미터 변수
		Zero::String strID = L"QWE";
		Zero::String strPW = L"asdzxc";

		pAdo->CreateParameter(_T("return"),adInteger, adParamReturnValue, 0);
		pAdo->CreateParameter(_T("@v_name"),adVarChar, adParamInput, strID );
		pAdo->CreateParameter(_T("@v_pass"),adVarChar, adParamInput, strPW );
		pAdo->CreateParameter(_T("@v_out"),adInteger, adParamOutput, 0);
		if( !pAdo->IsSuccess() ) return 0;

		// SP호출
		pAdo->SetQuery(Proc);
		pAdo->Execute();
		if( !pAdo->IsSuccess() ) return 0;

		// 필드 읽어오기
		Zero::Guid testGuid;
		Zero::String readID, readPW;
		while(!pAdo->GetEndOfFile())
		{
			short cnt = 0;
			DBFAIL_THROW(pAdo->GetFieldValue(cnt++, testGuid));
			DBFAIL_THROW(pAdo->GetFieldValue(cnt++, readID));
			DBFAIL_THROW(pAdo->GetFieldValue(cnt++, readPW));

			pAdo->MoveNext();
		}
		if( !pAdo->IsSuccess() ) return 0;

		// 다음 레코드
		pAdo->NextRecordSet();
		if( !pAdo->IsSuccess() ) return 0;

		// 필드 읽어오기
		int readValue = 0;
		while(!pAdo->GetEndOfFile())
		{
			DBFAIL_THROW(pAdo->GetFieldValue(L"aaa", readValue));
			pAdo->MoveNext();
		}
		if( !pAdo->IsSuccess() ) return 0;


		// 리턴값 읽어오기
		int nRtnParam = 0;
		pAdo->GetParameter(_T("return"), nRtnParam);


		// output파라미터의 경우 읽어오기
		int nOutParam = 0;
		pAdo->GetParameter(_T("@v_out"), nOutParam);
	}
	catch(Zero::DBException& e)
	{
		// 실패
	}
	// ---------------------------------------------------------
	// ---------------------------------------------------------


@sp code
	USE [AdoTest]
	GO
	SET ANSI_NULLS ON
	GO
	SET QUOTED_IDENTIFIER OFF
	GO

	ALTER  PROC [dbo].[InsertUser]
		@arg_name			NVARCHAR(50),
		@arg_pass			NVARCHAR(50),
		@arg_aaa            INT				OUTPUT
	AS
	BEGIN
		SET NOCOUNT ON
		SET XACT_ABORT ON

		DECLARE @v_UUID		uniqueidentifier
		Set @v_UUID = NEWID()

		INSERT INTO dbo.Userinfo values(@v_UUID, @arg_name, @arg_pass)

		SELECT * FROM Userinfo where uuid = @v_UUID

		SELECT top 1 aaa FROM AdoTest
		
		SET @arg_aaa = 123678
		
		return 321
	END

*/
namespace Zero
{
	class CAdoManager;
	typedef std::shared_ptr<CAdoManager> AdoManager;

	class CAdoManager
	{
		enum{ MAX_ARRAY_SIZE=100 };

		class CriticalSection
		{
		public:
			CriticalSection()	{ InitializeCriticalSection(&cs); }
			~CriticalSection()	{ DeleteCriticalSection(&cs); }
			void Lock()			{ EnterCriticalSection(&cs); }
			void Unlock()		{ LeaveCriticalSection(&cs); }
		private:
			CRITICAL_SECTION cs;
		};

		class ScopedLock
		{
		public:
			ScopedLock(CriticalSection &cs):m_cs(cs){ m_cs.Lock(); }
			~ScopedLock(){ m_cs.Unlock(); }
		private:
			CriticalSection &m_cs;
		};
		
	public:
		explicit CAdoManager(StartOptionAdo& start_option);
		~CAdoManager();

		void PutAdo(CAdo* pAdo);
		CAdo* GetAdo();
		StartOptionAdo GetConfigAdo() { return m_start_option; }

		int GetPoolCount() { return m_nTopPos+1; }	// 남아있는 ADO풀 개수 (+1하는 이유는 0부터니깐 : 0=1개)

		bool IsOpenFail() { return m_bOpenFail; }

	private:
		int m_count_new;
		bool m_bOpenFail;
		int m_nTopPos;
		int m_nMaxAdo;
		CAdo* m_pAdoStack[MAX_ARRAY_SIZE];
		StartOptionAdo m_start_option;
		CriticalSection m_Lock;
	};

	/**
	객체 생성시 커넥션풀로부터 ADO객체를 얻은 후 소멸시 ADO객체를 커넥션풀로 돌려준다.

	- 풀에서 얻어오는데 실패하는 경우 즉석에서 새로 객체를 할당하여 사용한다

	@param bAutoCommit	부가기능 명시적 트랜잭션

	*/
	class CScopedAdo
	{
	public:
		explicit CScopedAdo(CAdo* &pAdo, Zero::AdoManager pAdoManager, bool bAutoCommit = false);
		~CScopedAdo();

		bool IsError() { return m_bErrorOpen; }

		int SetManualMode();	// 현재 남은 Ado Pool 개수를 리턴한다
		void Release();

	private:
		Zero::AdoManager m_pAdoManager;
		CAdo* m_pAdo;
		bool m_bErrorOpen;
		bool m_bManualRelease;
		bool m_bReleased;
	};
}






