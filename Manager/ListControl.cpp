// ListControl.cpp : 实现文件
//

#include "stdafx.h"
#include "Manager.h"
#include "ListControl.h"
#include "afxdialogex.h"


// CListControl 对话框

IMPLEMENT_DYNAMIC(CListControl, CDialogEx)

CListControl::CListControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LIST_CONTROL, pParent)
{
	IsRunning = IsFind = FALSE;
	m_hOperate = NULL;
}

CListControl::~CListControl()
{
}

void CListControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAME_LIST, m_Game_List);
}


BEGIN_MESSAGE_MAP(CListControl, CDialogEx)
	ON_BN_CLICKED(IDOK, &CListControl::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CListControl::OnCancel)
	ON_WM_DROPFILES()
	ON_NOTIFY(NM_CLICK, IDC_GAME_LIST, &CListControl::OnNMClickGameList)
	ON_NOTIFY(NM_DBLCLK, IDC_GAME_LIST, &CListControl::OnNMDblclkGameList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GAME_LIST, &CListControl::OnLvnItemchangedGameList)
	ON_NOTIFY(NM_RCLICK, IDC_GAME_LIST, &CListControl::OnNMRClickGameList)
END_MESSAGE_MAP()


// CListControl 消息处理程序


BOOL CListControl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Game_List.SetExtendedStyle(LVS_EX_FULLROWSELECT  //允许整行选中
		| LVS_EX_HEADERDRAGDROP				            //允许整列拖动
		| LVS_EX_SUBITEMIMAGES			                //单击选中项
		| LVS_EX_GRIDLINES);				            //画出网格线

	m_Game_List.SetHeadings(_T("ROM 图片,79; ROM 名称,245; 掌机类型,80; ROM 热度,80; ROM 路径,245"));
	m_Game_List.LoadColumnInfo();

	theApp.Class = _T("掌机模拟");
	theApp.Type = _T("GBA");
	OnRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CListControl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CListControl::OnDropFiles(HDROP hDropInfo)
{
	CListControl::hDropInfo = hDropInfo;
	ParameterType = 1;

	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);

	CDialogEx::OnDropFiles(hDropInfo);
}


void CListControl::OnOK()
{
}


void CListControl::OnCancel()
{
}


// 该方法用于向资源列表添加图片
void CListControl::OnSetImg(CString ImgPath)
{
	Bitmap bmp(ImgPath);              //传入图片路径
	Bitmap* pThumbnail = (Bitmap*)bmp.GetThumbnailImage(75, 71, NULL, NULL); //设定缩略图的大小

	HBITMAP hBitmap;
	pThumbnail->GetHBITMAP(Color(255, 255, 255), &hBitmap);
	CBitmap *pImage = CBitmap::FromHandle(hBitmap);                 //转换成CBitmap格式位图

	m_Imagelist.Add(pImage, RGB(0, 0, 0));

	// 设置CImageList图像列表与CListCtrl控件关联 LVSIL_SMALL小图标列表
	m_Game_List.SetImageList(&m_Imagelist, LVSIL_SMALL);
}


// 该方法用于向游戏数据库添加游戏数据
BOOL CListControl::OnAddData(CString Name, CString Path)
{
	if (Path.Replace(_T("'"), _T("")))
	{
		CString ErrMsg;
		ErrMsg.Format(_T("资源路径: %s 不可以带有单引号!"), Path);

		AfxMessageBox(ErrMsg);
		return FALSE;
	}
	sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Path = '" + (CStringA)Path + "' and Type = '" + (CStringA)theApp.Type + "' ", -1, &theApp.stmt, NULL);
	if (sqlite3_step(theApp.stmt) != SQLITE_ROW)
	{
		int rc = sqlite3_exec(theApp.db, "INSERT INTO " + (CStringA)theApp.Class + " VALUES( './Image/Rom.jpg', '" + (CStringA)Name + "', '" + (CStringA)Path + "', '" + (CStringA)theApp.Type + "', '" + _T("1 ☆") + "' )", NULL, NULL, &theApp.errMsg);
		if (rc)
		{
			MessageBox(NULL, (CString)theApp.errMsg, NULL);
			return 1;
		}
		else
			IsFind = 0;
	}
	else
		IsFind = 1;

	sqlite3_finalize(theApp.stmt);

	return IsFind;
}


// 过滤非游戏文件
BOOL CListControl::OnGameFilter(CString GamePath)
{
	CString Ext = PathFindExtension(GamePath);
	if (Ext.MakeLower() != _T(".cdi") && Ext.MakeLower() != _T(".mds") && Ext.MakeLower() != _T(".nrg") && Ext.MakeLower() != _T(".gdi") && Ext.MakeLower() != _T(".chd") && Ext.MakeLower() != _T(".nes")
		&& Ext.MakeLower() != _T(".fds") && Ext.MakeLower() != _T(".nsf") && Ext.MakeLower()  != _T(".zip") && Ext.MakeLower() != _T(".7z") && Ext.MakeLower()   != _T(".smc") && Ext.MakeLower()    != _T(".smd")
		&& Ext.MakeLower() != _T(".gen") && Ext.MakeLower() != _T(".zsg") && Ext.MakeLower()  != _T(".32x") && Ext.MakeLower() != _T(".raw") && Ext.MakeLower()  != _T(".gba") && Ext.MakeLower()    != _T(".agb")
		&& Ext.MakeLower() != _T(".bin") && Ext.MakeLower() != _T(".elf") && Ext.MakeLower()  != _T(".mb") && Ext.MakeLower()  != _T(".z") && Ext.MakeLower()    != _T(".gz") && Ext.MakeLower()     != _T(".iso")
		&& Ext.MakeLower() != _T(".sms") && Ext.MakeLower() != _T(".sg") && Ext.MakeLower()   != _T(".sc") && Ext.MakeLower()  != _T(".mv") && Ext.MakeLower()   != _T(".n64") && Ext.MakeLower()    != _T(".rom")
		&& Ext.MakeLower() != _T(".usa") && Ext.MakeLower() != _T(".jap") && Ext.MakeLower()  != _T(".pal") && Ext.MakeLower() != _T(".nds") && Ext.MakeLower()  != _T(".ds.gba") && Ext.MakeLower() != _T(".srl")
		&& Ext.MakeLower() != _T(".rar") && Ext.MakeLower() != _T(".dol") && Ext.MakeLower()  != _T(".gcm") && Ext.MakeLower() != _T(".ciso") && Ext.MakeLower() != _T(".gcz") && Ext.MakeLower()    != _T(".wad")
		&& Ext.MakeLower() != _T(".ngp") && Ext.MakeLower() != _T(".ngc") && Ext.MakeLower()  != _T(".npc") && Ext.MakeLower() != _T(".pce") && Ext.MakeLower()  != _T(".hes") && Ext.MakeLower()    != _T(".mdf")
		&& Ext.MakeLower() != _T(".img") && Ext.MakeLower() != _T(".dump") && Ext.MakeLower() != _T(".cso") && Ext.MakeLower() != _T(".pdp") && Ext.MakeLower()  != _T(".prx") && Ext.MakeLower()    != _T(".ws")
		&& Ext.MakeLower() != _T(".wsc") && Ext.MakeLower() != _T(".wbfs") && Ext.MakeLower() != _T(".fdi") && Ext.MakeLower() != _T(".a26") && Ext.MakeLower()  != _T(".gb") && Ext.MakeLower()     != _T(".gbc"))
		return FALSE;
	else
		return TRUE;
}


// 启动工作者线程并刷新游戏列表
void CListControl::OnRefresh()
{
	ParameterType = 0;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


// 启动工作者线程并向本地游戏数据库中添加游戏数据
void CListControl::OnAddGame()
{
	ParameterType = 2;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


// 启动工作者线程并从本地游戏数据库中修改游戏数据
void CListControl::OnModifyGame()
{
}


// 启动工作者线程并从本地游戏数据库中删除游戏数据
void CListControl::OnDeleteGame()
{
	ParameterType = 3;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


// 启动游戏线程并开始游戏
void CListControl::OnStartGame()
{
	AfxBeginThread(StartGame, this);
}


// 搜索本地游戏数据库中的游戏数据
void CListControl::OnSearch()
{
	//CString SearchText;
	//GetDlgItem(IDC_SEARCH_TEXT)->GetWindowText(SearchText);

	//sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Name like '%" + (CStringA)SearchText + "%' and Type = '" + (CStringA)theApp.Type + "'", -1, &stmt, NULL);
	//m_Games_List.DeleteAllItems();
	//m_Imagelist.Create(75, 73, ILC_COLORDDB | ILC_COLOR32, 0, 1);   //创建图像序列CImageList对象

	//while (sqlite3_step(stmt) == SQLITE_ROW)
	//{
	//	OnSetImg((CString)sqlite3_column_text(stmt, 0));
	//	m_Games_List.AddItem((CString)sqlite3_column_text(stmt, 0), (CString)sqlite3_column_text(stmt, 1), (CString)sqlite3_column_text(stmt, 2), (CString)sqlite3_column_text(stmt, 3), (CString)sqlite3_column_text(stmt, 4));
	//}

	//sqlite3_finalize(stmt);
}


// 工作者线程，执行所有耗时操作
UINT CListControl::Operate(LPVOID lpParameter)
{
	CListControl *pDlg = (CListControl*)lpParameter;
	if (pDlg->IsRunning)
		return FALSE;

	pDlg->IsRunning = TRUE;
	switch (pDlg->ParameterType)
	{
	case 0: //刷新游戏列表
	{
	Refresh:

		if (sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Type = '" + (CStringA)theApp.Type + "'", -1, &theApp.stmt, NULL) == SQLITE_OK)
		{
			pDlg->m_Game_List.DeleteAllItems();
			pDlg->m_Imagelist.Create(75, 71, ILC_COLORDDB | ILC_COLOR32, 0, 1);   //创建图像序列CImageList对象

			while (sqlite3_step(theApp.stmt) == SQLITE_ROW)
			{
				pDlg->OnSetImg((CString)sqlite3_column_text(theApp.stmt, 0));
				pDlg->m_Game_List.AddItem((CString)sqlite3_column_text(theApp.stmt, 0), (CString)sqlite3_column_text(theApp.stmt, 1), (CString)sqlite3_column_text(theApp.stmt, 3), (CString)sqlite3_column_text(theApp.stmt, 4), (CString)sqlite3_column_text(theApp.stmt, 2));
			}

			sqlite3_finalize(theApp.stmt);
		}
	}break;

	case 1: //拖放文件或文件夹
	{
		char FilePath[300];
		CString Path, Name;
		UINT count = DragQueryFile(pDlg->hDropInfo, 0xFFFFFFFF, NULL, 0);
		if (count)
		{
			for (UINT i = 0; i < count; i++)
			{
				int pathLen = DragQueryFileA(pDlg->hDropInfo, i, FilePath, sizeof(FilePath));
				Path = FilePath;
				Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1);

				CFileStatus Status;
				if (CFile::GetStatus(Path, Status))
				{
					if ((Status.m_attribute & 0x10) == 0x10)
					{
						pDlg->OnAddFloder(Path);
						continue;
					}
				}

				if (!pDlg->OnGameFilter(Path))
					continue;

				if (pDlg->OnAddData(Name, Path))
					AfxMessageBox(_T("已跳过重复的资源: " + Name + " !"));
			}

			if (count > 0)
				goto Refresh;
		}

		DragFinish(pDlg->hDropInfo);
	}break;

	case 2: //添加游戏
	{
		CFileDialog FileDlg(TRUE, NULL, NULL, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
			_T("游戏资源(*.*)|*.cdi;*.mds;*.nrg;*.gdi;*.chd;*.nes;*.fds;*.nsf;*.zip;*.7z;*.smc;*.smd;*.gen;*.zsg;*.32x;*.raw;*.gba;*.agb;*.bin;*.elf;*.mb;*.z;*.gz;*.iso;*.sms;*.sg;*.sc;*.mv;*.n64;*.rom;*.usa;*.jap;*.pal;*.nds;*.ds.gba;*.srl;*.rar;*.dol;*.gcm;*.ciso;.gcz;*.wad;*.ngp;*.ngc;*.npc;*.pce;*.hes;*.mdf;*.img;*.dump;*.cso;*.pdp;*.prx;*.ws;*.wsc;*.wbfs;*.fdi;*.a26;*.gb;*.gbc;||"), NULL);
		char fileBuffer[900000] = { 0 };
		FileDlg.m_ofn.lpstrFile = (LPWSTR)fileBuffer;
		FileDlg.m_ofn.nMaxFile = 50000;
		if (FileDlg.DoModal() != IDOK)
		{
			pDlg->m_hOperate = NULL;
			pDlg->IsRunning = FALSE;
			return FALSE;
		}

		POSITION Pos = FileDlg.GetStartPosition();
		while (Pos)
		{
			FileDlg.GetNextPathName(Pos);
		}

		Pos = FileDlg.GetStartPosition();
		while (Pos)
		{
			CString Path = FileDlg.GetNextPathName(Pos);
			CString Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1);

			if (pDlg->OnAddData(Name, Path))
				AfxMessageBox(_T("已跳过重复的资源: " + Name + " !"));
		}

		goto Refresh;
	}break;

	case 3: //删除游戏
	{
		int Count = pDlg->m_Game_List.GetSelectedCount();

		for (int i = 0; i < Count; i++)
		{
			int Row = pDlg->m_Game_List.GetNextItem(i - 1, LVIS_SELECTED);
			CString Path = pDlg->m_Game_List.GetItemText(Row, 4);

			//theApp.Complete++;

			sqlite3_exec(theApp.db, "delete From " + (CStringA)theApp.Class + " where Path = '" + (CStringA)Path + "' and Type = '" + (CStringA)theApp.Type + "' ", NULL, NULL, &theApp.errMsg);
		}

		if (Count > 0)
			goto Refresh;
	}break;

	default:
		break;
	}

	pDlg->m_hOperate = NULL;
	pDlg->IsRunning = FALSE;
	return TRUE;
}


// 用于计算拖放文件夹内的游戏数量
int CListControl::CountFile(CString Path)
{
	int count = 0;
	CFileFind finder;
	BOOL working = finder.FindFile(Path + "./*.*");

	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
			count += CountFile(finder.GetFilePath());
		else
		{
			if(!OnGameFilter(finder.GetFilePath()))
				continue;

			count++;
		}
	}
	return count;
}


// 拖放文件夹则执行此代码，用于添加文件夹内游戏
void CListControl::OnAddFloder(CString Path)
{
	CString Name, Type;
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(Path + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();
		Path = Finder.GetFilePath();
		Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1);
		Type = Name.Right(Name.GetLength() - Name.ReverseFind('.') - 1);

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			OnAddFloder(Path);
		else
		{
			if (!OnGameFilter(Path))
				continue;

			if (OnAddData(Name, Path))
				AfxMessageBox(_T("已跳过重复的资源: " + Name + " !"));

			//theApp.Complete++;
		}
	}
}


// 启动游戏线程，读取参数并开始游戏
UINT CListControl::StartGame(LPVOID pParam)
{
	//从数据库读取模拟器数据
	sqlite3_prepare(theApp.db, "select * from 掌机设置 where Name = '" + (CStringA)theApp.Type + "' ", -1, &theApp.stmt, NULL);
	if (sqlite3_step(theApp.stmt) == SQLITE_ROW)
	{
		//处理模拟器数据
		CString EmulatorPath(sqlite3_column_text(theApp.stmt, 1)), Name = EmulatorPath.Right(EmulatorPath.GetLength() - EmulatorPath.ReverseFind('\\') - 1),
			Directory = EmulatorPath.Left(EmulatorPath.GetLength() - Name.GetLength());

		//检查传入参数
		theApp.Parameters.Format((CString)sqlite3_column_text(theApp.stmt, 2), theApp.Path);
		if (theApp.Parameters.IsEmpty())
		{
			((CListControl*)pParam)->MessageBox(_T("没有该模拟器的参数设置，请设置该模拟器的启动参数之后再进行游戏。"));
			return FALSE;
		}

		//调用对应模拟器
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpDirectory = Directory;
		ShExecInfo.lpFile = Name;
		ShExecInfo.lpParameters = theApp.Parameters;
		ShExecInfo.nShow = SW_SHOW;
		ShellExecuteEx(&ShExecInfo);

		//等待模拟器关闭
		AfxGetApp()->BeginWaitCursor();
		AfxGetApp()->GetMainWnd()->ShowWindow(SW_MINIMIZE);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		AfxGetApp()->GetMainWnd()->ShowWindow(SW_RESTORE);
		AfxGetApp()->EndWaitCursor();


		// 初始化参数
		theApp.Path = _T("");


		//主窗口置顶
		/*::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		if (!GetPrivateProfileIntA("Config", "KeepTop", 1, "./Config.ini"))
			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);*/
	}
	else
		((CListControl*)pParam)->MessageBox(_T("没有该模拟器的设置，请设置该模拟器之后再进行游戏。"));

	return TRUE;
}


// 单击游戏列表
void CListControl::OnNMClickGameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	theApp.Path = m_Game_List.GetItemText(m_Game_List.GetNextItem(-1, LVIS_SELECTED), 4);
	if (!theApp.Path.IsEmpty() && m_Game_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
	}
	else
	{
		theApp.Path = _T("");
	}

	*pResult = 0;
}


// 双击游戏列表
void CListControl::OnNMDblclkGameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (!theApp.Path.IsEmpty() && m_Game_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		// 取消选中游戏
		m_Game_List.SetItemState(m_Game_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		// 开始游戏
		OnStartGame();
	}
	

	*pResult = 0;
}


// 右键单击列表
void CListControl::OnNMRClickGameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// 取消选中游戏
	m_Game_List.SetItemState(m_Game_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

	*pResult = 0;
}


// 游戏列表项目变更
void CListControl::OnLvnItemchangedGameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	theApp.Path = m_Game_List.GetItemText(m_Game_List.GetNextItem(-1, LVIS_SELECTED), 4);
	if (!theApp.Path.IsEmpty() && m_Game_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
	}
	else
	{
		theApp.Path = _T("");
	}

	*pResult = 0;
}
