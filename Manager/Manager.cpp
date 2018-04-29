// Manager.cpp : 定义 DLL 的初始化例程。
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
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CManagerApp

BEGIN_MESSAGE_MAP(CManagerApp, CWinApp)
END_MESSAGE_MAP()


// CManagerApp 构造

CManagerApp::CManagerApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CManagerApp 对象
CManagerApp theApp;


// CManagerApp 初始化

BOOL CManagerApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	//阴影效果初始化
	CWndShadow::Initialize(AfxGetInstanceHandle());


	return TRUE;
}


BOOL CManagerApp::ConfirmHost(CString HostName)
{
	return HostName == _T("紫影龙游戏管家") ? true : false;
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

	// 运行环境初始化
	CoInitialize(NULL);
	AfxEnableControlContainer();
	AfxInitRichEdit();

	// 配置链接库
	if (!theApp.ConfirmHost(HostName))
	{
		AfxMessageBox(_T("本链接库只可以被紫影龙游戏管家调用,本次调用无效."));
		return FALSE;
	}
	else
	{
		theApp.HostName    = HostName;
		theApp.HostVersion = HostVersion;
	}


	// 数据库初始化
	CFileFind Finder;               // 搜索本地数据库
	BOOL Status = Finder.FindFile(_T("Database\\GameData.zdb"));
	if (!Status)
	{
		DWORD Path = GetFileAttributes(_T("Database"));
		if (Path == 0xFFFFFFFF)                             // 文件夹不存在
			CreateDirectory(_T("Database"), NULL);

		sqlite3_open("Database\\GameData.zdb", &theApp.db); // 创建并打开数据库,创建数据表
		sqlite3_exec(theApp.db, "CREATE TABLE 掌机模拟 (Image varchar(10), Name varchar(255), Path varchar(255), Type varchar(10), Enjoy varchar(10));", NULL, NULL, NULL);
		sqlite3_exec(theApp.db, "CREATE TABLE 本地游戏 (Image varchar(10), Name varchar(255), Path varchar(255), Type varchar(10), Enjoy varchar(10));", NULL, NULL, NULL);
		sqlite3_exec(theApp.db, "CREATE TABLE 网页游戏 (Image varchar(10), Name varchar(255), Path varchar(255), Type varchar(10), Enjoy varchar(10));", NULL, NULL, NULL);

		sqlite3_exec(theApp.db, "CREATE TABLE 掌机设置 (Name varchar(255), Path varchar(255), Parameters varchar(255));", NULL, NULL, NULL);
		sqlite3_exec(theApp.db, "CREATE TABLE 管家设置 (Type varchar(255), Name varchar(255), Value varchar(255));", NULL, NULL, NULL);

		sqlite3_exec(theApp.db, "CREATE TABLE 用户账户 (Image varchar(10), Name varchar(255), Level varchar(10), Exps varchar(10), Pswd varchar(255));", NULL, NULL, NULL);


		// 写入初始掌机设置数据
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'GBA', 'Emulator\\GBA\\VisualBoyAdvance-M.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'PSP', 'Emulator\\PSP\\PPSSPPWindows.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'NDS', 'Emulator\\NDS\\DeSmuME.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'PS2', 'Emulator\\PS2\\pcsx2.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( '街机', 'Emulator\\MAME\\MAME.exe', '-skip_gameinfo -nowindow -rompath \"%s\" \"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'PS1', 'Emulator\\PS1\\ePSXe.exe', '-nogui -slowboot -loadbin \"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'FC', 'Emulator\\FC\\VirtuaNES.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'N64', 'Emulator\\N64\\Project64.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'NGC', 'Emulator\\NGC\\Dolphin.exe', '/b /e\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'WII', 'Emulator\\NGC\\Dolphin.exe', '/b /e\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'SFC', 'Emulator\\SFC\\Snes9x.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'SS', 'Emulator\\SS\\SSF.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'PCE', 'Emulator\\PCE\\Ootake.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'SEGA', 'Emulator\\SEGA\\Fusion.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'DC', 'Emulator\\DC\\NullDC.exe', '-config nullDC:Emulator.Autostart=1 -config ImageReader:LoadDefaultImage=1 -config ImageReader:defaultImage=\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'WSC', 'Emulator\\WSC\\Oswan.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'PC98', 'Emulator\\PC98\\np21.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( '3DS', 'Emulator\\3DS\\citra-qt.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'Atari', 'Emulator\\Atari\\stella.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'OpenBor', 'Emulator\\OpenBor\\OpenBOR.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'NGPC', 'Emulator\\NGPC\\NeoPop.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
		if (sqlite3_exec(theApp.db, "INSERT INTO 掌机设置 VALUES( 'JAVA', 'Emulator\\J2ME\\KEmulator.exe', '\"%s\"' )", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL, NULL);
			return FALSE;
		}
	}
	else if (sqlite3_open("Database\\GameData.zdb", &theApp.db) != SQLITE_OK)  // 连接失败
	{
		MessageBox(NULL, (CString)sqlite3_errmsg(theApp.db), NULL, NULL);
		sqlite3_close(theApp.db);
		return FALSE;
	}

	// 主窗口初始化
	CMainWnd dlg;
	theApp.m_pMainWnd = &dlg;


	//GDI+ 初始化
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	// 若主窗口创建失败
	if (dlg.DoModal() == -1) 
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");

		AfxMessageBox(_T("主窗口创建失败，游戏管家将意外终止。"));
		return FALSE;
	}

	// 关闭gdiplus的环境
	Gdiplus::GdiplusShutdown(gdiplusToken);

	// 内存泄露检测
	_CrtDumpMemoryLeaks();                   

	// 应用程序结束
	return TRUE;
}


Export LPCTSTR GetLibraryVersion()
{
	return theApp.GetApplicationVersion();
}
