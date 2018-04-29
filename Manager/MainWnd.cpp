// MainWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Manager.h"
#include "MainWnd.h"
#include "afxdialogex.h"


// CMainWnd �Ի���

IMPLEMENT_DYNAMIC(CMainWnd, CDialogEx)

CMainWnd::CMainWnd(CWnd* pParent /*=NULL*/)
	: CDlgBase(IDD_MAIN_DIALOG, pParent)
{

}

CMainWnd::~CMainWnd()
{
}

void CMainWnd::DoDataExchange(CDataExchange* pDX)
{
	CDlgBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainWnd, CDlgBase)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SKIN,         OnMessageSkin)
	ON_MESSAGE(WM_MIAN_MENU,    OnMessageMainMenu)
	ON_MESSAGE(WM_CHILDMESSAGE, OnMessageChild)
	ON_BN_CLICKED(IDOK,         &CMainWnd::OnOK)
	ON_BN_CLICKED(IDCANCEL,     &CMainWnd::OnCancel)
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMainWnd ��Ϣ�������


int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDlgBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(6);
	m_WndShadow.SetPosition(0, 0);

	return 0;
}


BOOL CMainWnd::OnInitDialog()
{
	CDlgBase::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	// Frame
	m_ViewFrame = new CViewFrame();

	// ���������ڣ������˳�ʱMainFrame���Զ��ͷ�
	LPCTSTR lpszClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, NULL);
		//::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

	BOOL bResult = m_ViewFrame->CreateEx(WS_EX_TOOLWINDOW | WS_EX_ACCEPTFILES, lpszClassName, NULL,
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, this->GetSafeHwnd(), 0);
	if (!bResult)
	{
		AfxMessageBox(_T("��Դ��ܳ�ʼ��ʧ�ܣ�"));
		return FALSE;
	}

	m_ViewFrame->SetWindowPos(NULL, 3, 160, 0, 0, SWP_NOSIZE);
	m_ViewFrame->UpdateWindow();
	m_ViewFrame->ShowWindow(SW_SHOW);


	// ��ʼ������
	theApp.ChildType = 0;
	theApp.Class = _T("�ƻ�ģ��");
	theApp.Type = _T("GBA");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


HCURSOR CMainWnd::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CMainWnd::PreTranslateMessage(MSG* pMsg)
{
	return CDlgBase::PreTranslateMessage(pMsg);
}


void CMainWnd::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDlgBase::OnPaint();
	}
}


void CMainWnd::OnOK()
{
}


//UI����
void CDlgBase::InitUI(CRect rcClient)
{
	CRect rcTemp;
	int nStartX = 0;
	int nStartY = 0;
	CControlBase * pControlBase = NULL;

	int nXStartButton = rcClient.right - 45 - 31 - 31;
	int nYStartButton = 0;
	rcTemp.SetRect(nXStartButton, 0, nXStartButton + 31, 29);
	pControlBase = new CImageButton(GetSafeHwnd(), this, BT_MENU, rcTemp);
	((CImageButton *)pControlBase)->SetBitmap(IDB_BT_MENU);
	m_vecControl.push_back(pControlBase);

	nXStartButton -= 31;
	rcTemp.SetRect(nXStartButton, 0, nXStartButton + 31, 29);
	pControlBase = new CImageButton(GetSafeHwnd(), this, BT_SKIN, rcTemp);
	((CImageButton *)pControlBase)->SetBitmap(IDB_BT_SKIN);
	m_vecControl.push_back(pControlBase);

	nXStartButton -= 46;
	rcTemp.SetRect(nXStartButton, 0, nXStartButton + 46, 29);
	pControlBase = new CImageButton(GetSafeHwnd(), this, BT_FEEDBACK, rcTemp);
	((CImageButton *)pControlBase)->SetBitmap(IDB_BT_FEEDBACK);
	m_vecControl.push_back(pControlBase);

	rcTemp.SetRect(13, 13, 13 + 340, 13 + 60);
	pControlBase = new CPicture(GetSafeHwnd(), this, PIC_LOGO, rcTemp);
	((CPicture *)pControlBase)->SetBitmap(IDB_PNG_LOGO);
	m_vecArea.push_back(pControlBase);

	rcTemp.SetRect(10, 90, rcClient.right - 1, 90 + 45);
	pControlBase = new CTab(GetSafeHwnd(), this, TAB, rcTemp);
	CTab *pTab = (CTab *)pControlBase;;
	pTab->SetSeperator(true, IDB_TAB_SEPERATOR);
	pTab->InsertItem(-1, 1, TEXT("�ƻ�ģ��"), IDB_TAB_1);
	pTab->InsertItem(-1, 2, TEXT("������Ϸ"), IDB_TAB_2);
	pTab->InsertItem(-1, 3, TEXT("��ҳ��Ϸ"), IDB_TAB_3);
	pTab->InsertItem(-1, 4, TEXT("��������"), IDB_TAB_4);
	pTab->InsertItem(-1, 5, TEXT("��Ϸ����"), IDB_TAB_5);
	pTab->InsertItem(-1, 6, TEXT("�������"), IDB_TAB_6);
	pTab->InsertItem(-1, 7, TEXT("��Ϸ�˻�"), IDB_TAB_7);
	pTab->InsertItem(-1, 8, TEXT("ʹ�ð���"), IDB_TAB_8);
	pTab->SetSelectItem(0);
	m_vecControl.push_back(pControlBase);

	nXStartButton = 10;
	nYStartButton = rcClient.bottom - 22;
	rcTemp.SetRect(nXStartButton, nYStartButton, nXStartButton + 330, nYStartButton + 16);
	pControlBase = new CHideButton(GetSafeHwnd(), this, BT_VERSION, rcTemp, TEXT("��Ϸ�ܼҰ汾:") + theApp.HostVersion + _T("     ���ӿ�汾:") + theApp.GetApplicationVersion(), TEXT("������"));
	((CHideButton *)pControlBase)->SetFont(TEXT("����"), 12);
	m_vecControl.push_back(pControlBase);

	rcTemp.SetRect(rcClient.right - 70, rcClient.bottom - 26, rcClient.right - 10, rcClient.bottom - 7);
	pControlBase = new CImageButton(GetSafeHwnd(), this, BT_BOX, rcTemp, false);
	((CImageButton *)pControlBase)->SetBitmap(IDB_BT_BOX);
	m_vecControl.push_back(pControlBase);


	//���ر���ͼ
	sqlite3_prepare(theApp.db, "select * from �ܼ����� where Name = '�ܼ�Ƥ��' and Type = '��������' ", -1, &theApp.stmt, NULL);
	if (sqlite3_step(theApp.stmt) != SQLITE_ROW)
		LoadImage(IDB_SKIN_PIC_0);
	else
	{
		CString SkinName = (CString)sqlite3_column_text(theApp.stmt, 2);
		if (SkinName.Replace(_T("Default-"), _T("")))
		{
			LoadImage((UINT)atoi((CStringA)SkinName));
		}
		else if (SkinName.Replace(_T("Custom-"), _T("")))
		{
			LoadImage(SkinName);
		}
	}
}


// ��Ϣ��Ӧ
LRESULT CMainWnd::OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (uID == TAB && Msg == BUTTOM_DOWN)
	{
		if (wParam == 0)
		{
			theApp.ChildType = 0;
			theApp.Class = _T("�ƻ�ģ��");
			theApp.Type = _T("GBA");

			m_ViewFrame->m_Page = 1;
			m_ViewFrame->m_iSelected = 0;

			m_ViewFrame->SetTimer(1, 30, NULL);
			m_ViewFrame->m_bAutoMove = TRUE;
		}
		else if (wParam == 1)
		{
			theApp.ChildType = 1;
			theApp.Class = _T("������Ϸ");

			m_ViewFrame->m_Page = 1;
			m_ViewFrame->m_iSelected = 0;

			m_ViewFrame->SetTimer(1, 30, NULL);
			m_ViewFrame->m_bAutoMove = TRUE;
		}
		else if (wParam == 2)
		{
			theApp.ChildType = 2;
			theApp.Class = _T("��ҳ��Ϸ");

			m_ViewFrame->m_Page = 1;
			m_ViewFrame->m_iSelected = 0;

			m_ViewFrame->SetTimer(1, 30, NULL);
			m_ViewFrame->m_bAutoMove = TRUE;
		}
		else if (wParam == 3)
		{
			theApp.ChildType = 3;
			theApp.Class = _T("��Ϸ��Դ");
		}
		else if (wParam == 4)
		{
			theApp.ChildType = 4;
			theApp.Class = _T("�ı���Դ");
		}
		else if (wParam == 5)
		{

		}
		else if (wParam == 6)
		{
		}
		else if (wParam == 7)
		{

		}
	}
	else if (uID == BT_SKIN && Msg == BUTTOM_UP)
	{
		CControlBase * pControlBase = GetControl(BT_SKIN);
		if (pControlBase)
		{
			CDlgSkin *pDlgSkin = new CDlgSkin;
			CRect rc = pControlBase->GetRect();
			rc.OffsetRect(-95, rc.Height());
			ClientToScreen(&rc);
			pDlgSkin->Create(this, rc, WM_SKIN, IDB_SKIN_BKG, enBMImage);
			pDlgSkin->ShowWindow(SW_SHOW);
		}
	}
	else if (uID == BT_MENU && Msg == BUTTOM_UP)
	{
		CControlBase * pControlBase = GetControl(BT_MENU);
		if (pControlBase)
		{
			CMenuEx *pMenuEx = new CMenuEx(TEXT("����"), 12);
			CPoint point;
			CRect rc = pControlBase->GetRect();
			point.SetPoint(rc.left + rc.Width() / 2, rc.bottom);
			ClientToScreen(&point);
			pMenuEx->Create(this, point, WM_MIAN_MENU, IDB_MENUEX_BACK, 30);
			pMenuEx->AddMenu(TEXT("�ܼ�����"), MENU_MAIN_SETING);
			pMenuEx->AddMenu(TEXT("������"), MENU_MAIN_UPDATE, IDB_MENU_UPDATE);
			pMenuEx->AddSeparator();
			pMenuEx->AddMenu(TEXT("�����ֱ�"), MENU_MAIN_HANDLE);
			pMenuEx->AddMenu(TEXT("�������"), MENU_MAIN_BBS, IDB_MENU_BBS);
			pMenuEx->AddMenu(TEXT("ʹ�ð���"), MENU_MAIN_HELP);
			pMenuEx->AddMenu(TEXT("���ڹܼ�"), MENU_MAIN_ABOUT);
			pMenuEx->ShowWindow(SW_SHOW);
		}
	}
	else if (uID == BT_VERSION && Msg == BUTTOM_UP)
	{
		/*OnDetect();*/
	}
	else if (uID == BT_BOX && Msg == BUTTOM_UP)
	{
		/*OnShowDownload();*/
	}
	else if (uID == BT_HANDLE && Msg == BUTTOM_UP)
	{
		/*OnSearchHandle();*/
	}

	return 0;
}


// ����
LRESULT CMainWnd::OnMessageSkin(WPARAM wParam, LPARAM lParam)
{
	if (wParam == MSG_CLOSE)
	{
		;
	}
	else if (wParam == MSG_SKIN)
	{
		SkinInfo *pSkinInfo = (SkinInfo *)lParam;
		if (pSkinInfo != NULL)
		{
			if (pSkinInfo->nType == 1)
			{
				Color &clr = pSkinInfo->clr;
				COLORREF crlBack = RGB(clr.GetRed(), clr.GetGreen(), clr.GetBlue());
				DrawBackground(crlBack);
			}
			else if (pSkinInfo->nType == 0)
			{
				// Ӧ��Ƥ��
				LoadImage(IDB_SKIN_PIC_0 + pSkinInfo->uIndex);

				CString SkinType;
				SkinType.Format(_T("Default-%d"), IDB_SKIN_PIC_0 + pSkinInfo->uIndex);

				// д�����ݿ�
				sqlite3_prepare(theApp.db, "select * from �ܼ����� where Name = '�ܼ�Ƥ��' and Type = '��������' ", -1, &theApp.stmt, NULL);
				if (sqlite3_step(theApp.stmt) != SQLITE_ROW)
				{
					if (sqlite3_exec(theApp.db, "INSERT INTO �ܼ����� VALUES( '��������', '�ܼ�Ƥ��','" + (CStringA)SkinType + "')", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
					{
						MessageBox((CString)theApp.errMsg);
						return 1;
					}
				}
				else
				{
					if (sqlite3_exec(theApp.db, "UPDATE �ܼ����� SET Value = '" + (CStringA)SkinType + "' WHERE Name = '�ܼ�Ƥ��' and  Type = '��������' ", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
					{
						MessageBox((CString)theApp.errMsg);
						return 1;
					}
				}

			}
		}
	}
	else if (wParam == MSG_SELECT_SKIN)
	{
		CFileDialog DlgFile(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
			TEXT("Ƥ������(*.bmp;*.png;*.jpg;*.jpeg)|*.bmp;*.png;*.jpg;*.jpeg||"), this, 0, 0);
		DlgFile.m_ofn.nFilterIndex = 1;
		DlgFile.m_ofn.hwndOwner = m_hWnd;
		DlgFile.m_ofn.lStructSize = sizeof(OPENFILENAME);
		DlgFile.m_ofn.lpstrTitle = TEXT("��");
		DlgFile.m_ofn.nMaxFile = MAX_PATH;
		if (DlgFile.DoModal() == IDOK)
		{
			CString strFileName = DlgFile.GetPathName();
			CString strFileType = strFileName.Right(3);
			if (0 == strFileType.CompareNoCase(TEXT("bmp")) || 0 == strFileType.CompareNoCase(TEXT("jpg")) || 0 == strFileType.CompareNoCase(TEXT("png")))
			{
				LoadImage(strFileName);

				strFileName = _T("Custom-") + strFileName;

				// д�����ݿ�
				sqlite3_prepare(theApp.db, "select * from �ܼ����� where Name = '�ܼ�Ƥ��' and Type = '��������' ", -1, &theApp.stmt, NULL);
				if (sqlite3_step(theApp.stmt) != SQLITE_ROW)
				{
					if (sqlite3_exec(theApp.db, "INSERT INTO �ܼ����� VALUES( '��������', '�ܼ�Ƥ��','" + (CStringA)strFileName + "')", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
					{
						MessageBox((CString)theApp.errMsg);
						return 1;
					}
				}
				else
				{
					if (sqlite3_exec(theApp.db, "UPDATE �ܼ����� SET Value = '" + (CStringA)strFileName + "' WHERE Name = '�ܼ�Ƥ��' and  Type = '��������' ", NULL, NULL, &theApp.errMsg) != SQLITE_OK)
					{
						MessageBox((CString)theApp.errMsg);
						return 1;
					}
				}
			}
		}
	}

	return 0;
}


// ���˵�
LRESULT CMainWnd::OnMessageMainMenu(WPARAM wParam, LPARAM lParam)
{
	tagMenuInfo *pMenuInfo = (tagMenuInfo *)lParam;
	if (wParam == BUTTOM_UP)
	{
		switch (pMenuInfo->uMenuID)
		{
		case 2020:
			PostMessage(WM_COMMAND, 101);
			break;
		case 2021:
			PostMessage(WM_COMMAND, 102);
			break;
		case 2022:
			PostMessage(WM_COMMAND, 103);
			break;
		case 2023:
			//ShellExecuteA(NULL, "open", "http://bbs.xiaoji001.com/forum.php", "", "", SW_SHOWNORMAL);
			break;
		case 2024:
			//ShellExecuteA(NULL, "open", "http://faq.xiaoji001.com", "", "", SW_SHOWNORMAL);
			break;
		case 2025:
			PostMessage(WM_COMMAND, 104);
		}
	}

	delete pMenuInfo;

	return 0;
}


// �Ӵ�����Ϣ
LRESULT CMainWnd::OnMessageChild(WPARAM wParam, LPARAM lParam)
{
	CString Path = *(CString*)(LPARAM*)&lParam;

	switch (wParam)
	{
	case 0:
		//m_ViewFrame->m_Pictures->ShowPictures(Path);
		break;

	case 1:
		//m_ViewFrame->m_Videos->OnStartMovies(Path);
		break;

	case 2:
		//m_ViewFrame->m_Videos->OnStartMovies(Path);
		//m_ViewFrame->m_Audios->OnPlayMusic(Path);
		break;

	case 3:
		//m_ViewFrame->m_Games->OnStartGame(Path);
		break;

	case 4:
		//m_ViewFrame->m_Documents->OnShowText(Path);
		break;

	case 5:
		break;

	default:
		break;
	}

	return TRUE;
}


void CMainWnd::OnCancel()
{
	sqlite3_close(theApp.db);
	//m_ViewFrame->m_Manager->m_Resource_List.SetItemState(m_ViewFrame->m_Manager->m_Resource_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

	EndDialog(FALSE);
}
