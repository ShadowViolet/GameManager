// ListControl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Manager.h"
#include "ListControl.h"
#include "afxdialogex.h"


// CListControl �Ի���

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


// CListControl ��Ϣ�������


BOOL CListControl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Game_List.SetExtendedStyle(LVS_EX_FULLROWSELECT  //��������ѡ��
		| LVS_EX_HEADERDRAGDROP				            //���������϶�
		| LVS_EX_SUBITEMIMAGES			                //����ѡ����
		| LVS_EX_GRIDLINES);				            //����������

	m_Game_List.SetHeadings(_T("ROM ͼƬ,79; ROM ����,245; �ƻ�����,80; ROM �ȶ�,80; ROM ·��,245"));
	m_Game_List.LoadColumnInfo();

	theApp.Class = _T("�ƻ�ģ��");
	theApp.Type = _T("GBA");
	OnRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CListControl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

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


// �÷�����������Դ�б����ͼƬ
void CListControl::OnSetImg(CString ImgPath)
{
	Bitmap bmp(ImgPath);              //����ͼƬ·��
	Bitmap* pThumbnail = (Bitmap*)bmp.GetThumbnailImage(75, 71, NULL, NULL); //�趨����ͼ�Ĵ�С

	HBITMAP hBitmap;
	pThumbnail->GetHBITMAP(Color(255, 255, 255), &hBitmap);
	CBitmap *pImage = CBitmap::FromHandle(hBitmap);                 //ת����CBitmap��ʽλͼ

	m_Imagelist.Add(pImage, RGB(0, 0, 0));

	// ����CImageListͼ���б���CListCtrl�ؼ����� LVSIL_SMALLСͼ���б�
	m_Game_List.SetImageList(&m_Imagelist, LVSIL_SMALL);
}


// �÷�����������Ϸ���ݿ������Ϸ����
BOOL CListControl::OnAddData(CString Name, CString Path)
{
	if (Path.Replace(_T("'"), _T("")))
	{
		CString ErrMsg;
		ErrMsg.Format(_T("��Դ·��: %s �����Դ��е�����!"), Path);

		AfxMessageBox(ErrMsg);
		return FALSE;
	}
	sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Path = '" + (CStringA)Path + "' and Type = '" + (CStringA)theApp.Type + "' ", -1, &theApp.stmt, NULL);
	if (sqlite3_step(theApp.stmt) != SQLITE_ROW)
	{
		int rc = sqlite3_exec(theApp.db, "INSERT INTO " + (CStringA)theApp.Class + " VALUES( './Image/Rom.jpg', '" + (CStringA)Name + "', '" + (CStringA)Path + "', '" + (CStringA)theApp.Type + "', '" + _T("1 ��") + "' )", NULL, NULL, &theApp.errMsg);
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


// ���˷���Ϸ�ļ�
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


// �����������̲߳�ˢ����Ϸ�б�
void CListControl::OnRefresh()
{
	ParameterType = 0;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


// �����������̲߳��򱾵���Ϸ���ݿ��������Ϸ����
void CListControl::OnAddGame()
{
	ParameterType = 2;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


// �����������̲߳��ӱ�����Ϸ���ݿ����޸���Ϸ����
void CListControl::OnModifyGame()
{
}


// �����������̲߳��ӱ�����Ϸ���ݿ���ɾ����Ϸ����
void CListControl::OnDeleteGame()
{
	ParameterType = 3;
	if (m_hOperate == NULL)
		m_hOperate = AfxBeginThread(Operate, this);
}


// ������Ϸ�̲߳���ʼ��Ϸ
void CListControl::OnStartGame()
{
	AfxBeginThread(StartGame, this);
}


// ����������Ϸ���ݿ��е���Ϸ����
void CListControl::OnSearch()
{
	//CString SearchText;
	//GetDlgItem(IDC_SEARCH_TEXT)->GetWindowText(SearchText);

	//sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Name like '%" + (CStringA)SearchText + "%' and Type = '" + (CStringA)theApp.Type + "'", -1, &stmt, NULL);
	//m_Games_List.DeleteAllItems();
	//m_Imagelist.Create(75, 73, ILC_COLORDDB | ILC_COLOR32, 0, 1);   //����ͼ������CImageList����

	//while (sqlite3_step(stmt) == SQLITE_ROW)
	//{
	//	OnSetImg((CString)sqlite3_column_text(stmt, 0));
	//	m_Games_List.AddItem((CString)sqlite3_column_text(stmt, 0), (CString)sqlite3_column_text(stmt, 1), (CString)sqlite3_column_text(stmt, 2), (CString)sqlite3_column_text(stmt, 3), (CString)sqlite3_column_text(stmt, 4));
	//}

	//sqlite3_finalize(stmt);
}


// �������̣߳�ִ�����к�ʱ����
UINT CListControl::Operate(LPVOID lpParameter)
{
	CListControl *pDlg = (CListControl*)lpParameter;
	if (pDlg->IsRunning)
		return FALSE;

	pDlg->IsRunning = TRUE;
	switch (pDlg->ParameterType)
	{
	case 0: //ˢ����Ϸ�б�
	{
	Refresh:

		if (sqlite3_prepare(theApp.db, "select * from " + (CStringA)theApp.Class + " where Type = '" + (CStringA)theApp.Type + "'", -1, &theApp.stmt, NULL) == SQLITE_OK)
		{
			pDlg->m_Game_List.DeleteAllItems();
			pDlg->m_Imagelist.Create(75, 71, ILC_COLORDDB | ILC_COLOR32, 0, 1);   //����ͼ������CImageList����

			while (sqlite3_step(theApp.stmt) == SQLITE_ROW)
			{
				pDlg->OnSetImg((CString)sqlite3_column_text(theApp.stmt, 0));
				pDlg->m_Game_List.AddItem((CString)sqlite3_column_text(theApp.stmt, 0), (CString)sqlite3_column_text(theApp.stmt, 1), (CString)sqlite3_column_text(theApp.stmt, 3), (CString)sqlite3_column_text(theApp.stmt, 4), (CString)sqlite3_column_text(theApp.stmt, 2));
			}

			sqlite3_finalize(theApp.stmt);
		}
	}break;

	case 1: //�Ϸ��ļ����ļ���
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
					AfxMessageBox(_T("�������ظ�����Դ: " + Name + " !"));
			}

			if (count > 0)
				goto Refresh;
		}

		DragFinish(pDlg->hDropInfo);
	}break;

	case 2: //�����Ϸ
	{
		CFileDialog FileDlg(TRUE, NULL, NULL, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
			_T("��Ϸ��Դ(*.*)|*.cdi;*.mds;*.nrg;*.gdi;*.chd;*.nes;*.fds;*.nsf;*.zip;*.7z;*.smc;*.smd;*.gen;*.zsg;*.32x;*.raw;*.gba;*.agb;*.bin;*.elf;*.mb;*.z;*.gz;*.iso;*.sms;*.sg;*.sc;*.mv;*.n64;*.rom;*.usa;*.jap;*.pal;*.nds;*.ds.gba;*.srl;*.rar;*.dol;*.gcm;*.ciso;.gcz;*.wad;*.ngp;*.ngc;*.npc;*.pce;*.hes;*.mdf;*.img;*.dump;*.cso;*.pdp;*.prx;*.ws;*.wsc;*.wbfs;*.fdi;*.a26;*.gb;*.gbc;||"), NULL);
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
				AfxMessageBox(_T("�������ظ�����Դ: " + Name + " !"));
		}

		goto Refresh;
	}break;

	case 3: //ɾ����Ϸ
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


// ���ڼ����Ϸ��ļ����ڵ���Ϸ����
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


// �Ϸ��ļ�����ִ�д˴��룬��������ļ�������Ϸ
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
				AfxMessageBox(_T("�������ظ�����Դ: " + Name + " !"));

			//theApp.Complete++;
		}
	}
}


// ������Ϸ�̣߳���ȡ��������ʼ��Ϸ
UINT CListControl::StartGame(LPVOID pParam)
{
	//�����ݿ��ȡģ��������
	sqlite3_prepare(theApp.db, "select * from �ƻ����� where Name = '" + (CStringA)theApp.Type + "' ", -1, &theApp.stmt, NULL);
	if (sqlite3_step(theApp.stmt) == SQLITE_ROW)
	{
		//����ģ��������
		CString EmulatorPath(sqlite3_column_text(theApp.stmt, 1)), Name = EmulatorPath.Right(EmulatorPath.GetLength() - EmulatorPath.ReverseFind('\\') - 1),
			Directory = EmulatorPath.Left(EmulatorPath.GetLength() - Name.GetLength());

		//��鴫�����
		theApp.Parameters.Format((CString)sqlite3_column_text(theApp.stmt, 2), theApp.Path);
		if (theApp.Parameters.IsEmpty())
		{
			((CListControl*)pParam)->MessageBox(_T("û�и�ģ�����Ĳ������ã������ø�ģ��������������֮���ٽ�����Ϸ��"));
			return FALSE;
		}

		//���ö�Ӧģ����
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpDirectory = Directory;
		ShExecInfo.lpFile = Name;
		ShExecInfo.lpParameters = theApp.Parameters;
		ShExecInfo.nShow = SW_SHOW;
		ShellExecuteEx(&ShExecInfo);

		//�ȴ�ģ�����ر�
		AfxGetApp()->BeginWaitCursor();
		AfxGetApp()->GetMainWnd()->ShowWindow(SW_MINIMIZE);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		AfxGetApp()->GetMainWnd()->ShowWindow(SW_RESTORE);
		AfxGetApp()->EndWaitCursor();


		// ��ʼ������
		theApp.Path = _T("");


		//�������ö�
		/*::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		if (!GetPrivateProfileIntA("Config", "KeepTop", 1, "./Config.ini"))
			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);*/
	}
	else
		((CListControl*)pParam)->MessageBox(_T("û�и�ģ���������ã������ø�ģ����֮���ٽ�����Ϸ��"));

	return TRUE;
}


// ������Ϸ�б�
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


// ˫����Ϸ�б�
void CListControl::OnNMDblclkGameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (!theApp.Path.IsEmpty() && m_Game_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		// ȡ��ѡ����Ϸ
		m_Game_List.SetItemState(m_Game_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		// ��ʼ��Ϸ
		OnStartGame();
	}
	

	*pResult = 0;
}


// �Ҽ������б�
void CListControl::OnNMRClickGameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// ȡ��ѡ����Ϸ
	m_Game_List.SetItemState(m_Game_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

	*pResult = 0;
}


// ��Ϸ�б���Ŀ���
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
