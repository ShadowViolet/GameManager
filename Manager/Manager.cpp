// Manager.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Manager.h"
#include "MainWnd.h"

#pragma comment(lib,"version.lib")
#define Export extern "C" __declspec(dllexport)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CManagerApp

BEGIN_MESSAGE_MAP(CManagerApp, CWinApp)
END_MESSAGE_MAP()


// CManagerApp ����

CManagerApp::CManagerApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CManagerApp ����
CManagerApp theApp;


// CManagerApp ��ʼ��

BOOL CManagerApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	//��ӰЧ����ʼ��
	CWndShadow::Initialize(AfxGetInstanceHandle());


	return TRUE;
}


BOOL CManagerApp::ConfirmHost(CString HostName)
{
	return HostName == _T("��Ӱ����Ϸ�ܼ�") ? true : false;
}


CString CManagerApp::GetApplicationVersion()
{
	TCHAR szFullPath[MAX_PATH];
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;

	GetModuleFileName(NULL, szFullPath, sizeof(szFullPath));
	dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = GlobalLock(hMem);
		GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);

		::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);

		int ret = GetLastError();
		WORD m_nProdVersion[4];

		// Product version from the FILEVERSION of the version info resource 
		m_nProdVersion[0] = HIWORD(pFileInfo->dwProductVersionMS);
		m_nProdVersion[1] = LOWORD(pFileInfo->dwProductVersionMS);
		m_nProdVersion[2] = HIWORD(pFileInfo->dwProductVersionLS);
		m_nProdVersion[3] = LOWORD(pFileInfo->dwProductVersionLS);

		CString strVersion;
		strVersion.Format(_T("%d.%d.%d.%d"), m_nProdVersion[0],
			m_nProdVersion[1], m_nProdVersion[2], m_nProdVersion[3]);

		GlobalUnlock(hMem);
		GlobalFree(hMem);

		return strVersion;
	}
}


Export BOOL ShowMainWnd(CString HostName, CString HostVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// ���л�����ʼ��
	CoInitialize(NULL);
	AfxEnableControlContainer();
	AfxInitRichEdit();

	// �������ӿ�
	if (!theApp.ConfirmHost(HostName))
	{
		AfxMessageBox(_T("�����ӿ�ֻ���Ա���Ӱ����Ϸ�ܼҵ���,���ε�����Ч."));
		return FALSE;
	}
	else
	{
		theApp.HostName    = HostName;
		theApp.HostVersion = HostVersion;
	}


	// ���ݿ��ʼ��
	CFileFind Finder;               // �����������ݿ�
	BOOL Status = Finder.FindFile(_T("Database\\GameData.zdb"));
	if (!Status)
	{
		DWORD Path = GetFileAttributes(_T("Database"));
		if (Path == 0xFFFFFFFF)                             // �ļ��в�����
			CreateDirectory(_T("Database"), NULL);

		sqlite3_open("Database\\GameData.zdb", &theApp.db); // �����������ݿ�,�������ݱ�
		sqlite3_exec(theApp.db, "CREATE TABLE �ƻ�ģ�� (Image varchar(10), Name varchar(255), Path varchar(255), Type varchar(10), Enjoy varchar(10));", NULL, NULL, NULL);
		sqlite3_exec(theApp.db, "CREATE TABLE ������Ϸ (Image varchar(10), Name varchar(255), Path varchar(255), Type varchar(10), Enjoy varchar(10));", NULL, NULL, NULL);
		sqlite3_exec(theApp.db, "CREATE TABLE ��ҳ��Ϸ (Image varchar(10), Name varchar(255), Path varchar(255), Type varchar(10), Enjoy varchar(10));", NULL, NULL, NULL);

		sqlite3_exec(theApp.db, "CREATE TABLE �ƻ����� (Name varchar(255), Path varchar(255), Parameters varchar(255));", NULL, NULL, NULL);
		sqlite3_exec(theApp.db, "CREATE TABLE �ܼ����� (Type varchar(255), Name varchar(255), Value varchar(255));", NULL, NULL, NULL);

		sqlite3_exec(theApp.db, "CREATE TABLE �û��˻� (Image varchar(10), Name varchar(255), Level varchar(10), Exps varchar(10), Pswd varchar(255));", NULL, NULL, NULL);


		// д���ʼ�ƻ���������
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'GBA', 'Emulator\\GBA\\VisualBoyAdvance-M.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'PSP', 'Emulator\\PSP\\PPSSPPWindows.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'NDS', 'Emulator\\NDS\\DeSmuME.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'PS2', 'Emulator\\PS2\\pcsx2.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( '�ֻ�', 'Emulator\\MAME\\MAME.exe', '-skip_gameinfo -nowindow -rompath \"%s\" \"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'PS1', 'Emulator\\PS1\\ePSXe.exe', '-nogui -slowboot -loadbin \"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'FC', 'Emulator\\FC\\VirtuaNES.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'N64', 'Emulator\\N64\\Project64.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'NGC', 'Emulator\\NGC\\Dolphin.exe', '/b /e\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'WII', 'Emulator\\NGC\\Dolphin.exe', '/b /e\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'SFC', 'Emulator\\SFC\\Snes9x.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'SS', 'Emulator\\SS\\SSF.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'PCE', 'Emulator\\PCE\\Ootake.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'SEGA', 'Emulator\\SEGA\\Fusion.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'DC', 'Emulator\\DC\\NullDC.exe', '-config nullDC:Emulator.Autostart=1 -config ImageReader:LoadDefaultImage=1 -config ImageReader:defaultImage=\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'WSC', 'Emulator\\WSC\\Oswan.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'PC98', 'Emulator\\PC98\\np21.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( '3DS', 'Emulator\\3DS\\citra-qt.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'Atari', 'Emulator\\Atari\\stella.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'OpenBor', 'Emulator\\OpenBor\\OpenBOR.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'NGPC', 'Emulator\\NGPC\\NeoPop.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO �ƻ����� VALUES( 'JAVA', 'Emulator\\J2ME\\KEmulator.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
	}
	else if (sqlite3_open("Database\\GameData.zdb", &theApp.db) != SQLITE_OK)  // ����ʧ��
	{
		MessageBox(NULL, (CString)sqlite3_errmsg(theApp.db), NULL, NULL);
		sqlite3_close(theApp.db);
		return FALSE;
	}

	// �����ڳ�ʼ��
	CMainWnd dlg;
	theApp.m_pMainWnd = &dlg;


	//GDI+ ��ʼ��
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	// �������ڴ���ʧ��
	if (dlg.DoModal() == -1) 
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");

		AfxMessageBox(_T("�����ڴ���ʧ�ܣ���Ϸ�ܼҽ�������ֹ��"));
		return FALSE;
	}

	// �ر�gdiplus�Ļ���
	Gdiplus::GdiplusShutdown(gdiplusToken);

	// �ڴ�й¶���
	_CrtDumpMemoryLeaks();                   

	// Ӧ�ó������
	return TRUE;
}


Export LPCTSTR GetLibraryVersion()
{
	return theApp.GetApplicationVersion();
}
