#include "stdafx.h"
#include "ViewFrame.h"
#include "Manager.h"

#include <comdef.h>
#include <shlwapi.h>
#include <cassert>


std::auto_ptr<CImageInfo> CImageInfo::sm_inst;


CImageInfo::CImageInfo(void)
{
}


CImageInfo::~CImageInfo(void)
{
	// Release�������е��ã���ΪGdiplusShutdown��ԭ�򼺵��õ�ԭ��ᵼ���ڴ����
	// Release();
}


CImageInfo * CImageInfo::Instance()
{
	if (sm_inst.get() == 0)
		sm_inst = auto_ptr<CImageInfo>(new CImageInfo);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CMachine);
	return sm_inst.get();
	*/
}


Image* CImageInfo::ImageFromResource(UINT nFile)
{
	Image* pImage = NULL;
	if (m_mpImage.find(nFile) != m_mpImage.end())
	{
		return m_mpImage[nFile];
	}

	pImage = FromResource(nFile);
	if (pImage != NULL)
	{
		m_mpImage[nFile] = pImage;
	}

	return pImage;
}


void CImageInfo::Release()
{
	for (map<UINT, Image*>::iterator ciIter = m_mpImage.begin();
		ciIter != m_mpImage.end(); ciIter++)
	{
		delete ciIter->second;
	}
	m_mpImage.clear();
}


Image* CImageInfo::FromResource(UINT uId)
{
	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(uId), _T("PNG"));
	if (hRes == NULL)
	{
		assert(FALSE);
		return NULL;
	}

	DWORD dwSize = ::SizeofResource(hModule, hRes);
	if (dwSize == 0)
	{
		assert(FALSE);
		return NULL;
	}

	HGLOBAL hGlobal = ::LoadResource(hModule, hRes);
	if (hGlobal == NULL)
	{
		assert(FALSE);
		return NULL;
	}

	LPVOID pBuffer = ::LockResource(hGlobal);
	if (pBuffer == NULL)
	{
		assert(FALSE);
		::FreeResource(hGlobal);
		return NULL;
	}


	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (hMem == NULL)
	{
		assert(FALSE);
		::FreeResource(hGlobal);
		return NULL;
	}

	void* pData = GlobalLock(hMem);
	memcpy(pData, (void *)hGlobal, dwSize);
	GlobalUnlock(hMem);

	Image *pRtImg = NULL;
	IStream* pStream = NULL;
	if (CreateStreamOnHGlobal(hMem, TRUE, &pStream) == S_OK)
	{
		pRtImg = Image::FromStream(pStream, FALSE);
		pStream->Release();
	}

	::FreeResource(hGlobal);

	return pRtImg;
}


// ���ư�ť
void CXButton::DrawButton(Gdiplus::Graphics&  graphics)
{
	// ��ȡ��ťͼƬ��Ϣ
	Image* pImage = CImageInfo::Instance()->ImageFromResource(m_nImageContorl);
	UINT iCount = m_nImageCount;
	if (m_bCheck && m_nAltImageContorl != 0)
	{
		pImage = CImageInfo::Instance()->ImageFromResource(m_nAltImageContorl);;
		iCount = m_nAltImageCount;
	}

	// ��ȡ��ť״̬��Ϣ
	int	iButtonIndex = 0;
	if (m_bDisabled && iCount >= 4) iButtonIndex = 3;
	else if (m_bPressed && iCount >= 3)iButtonIndex = 2;
	else if (m_bHovering && iCount >= 2)iButtonIndex = 1;
	else iButtonIndex = 0;

	// ��ָ��λ�û��ư�ť
	int iWidth = pImage->GetWidth() / iCount;
	int iHeight = pImage->GetHeight();
	RectF grect;
	grect.X = (Gdiplus::REAL)m_rcRect.left, grect.Y = (Gdiplus::REAL)m_rcRect.top; grect.Width = (Gdiplus::REAL)m_rcRect.Width(); grect.Height = (Gdiplus::REAL)m_rcRect.Height();
	graphics.DrawImage(pImage, grect, (Gdiplus::REAL)iWidth*iButtonIndex, 0, (Gdiplus::REAL)iWidth, (Gdiplus::REAL)iHeight, UnitPixel);
}


// ������ť
void CXButton::CreateButton(CWnd* pParent, LPRECT lpRect, UINT nImage, UINT nCount /*= 4*/, UINT nAltImage /*= _T("")*/, UINT nAltCount /*= 0*/, UINT nID/* = 0*/)
{
	m_pParent = pParent;
	m_rcRect = lpRect;
	m_nImageContorl = nImage;
	m_nImageCount = nCount;
	m_nAltImageContorl = nAltImage;
	m_nAltImageCount = nAltCount;
	m_nID = nID;
}


// �л���ť��С����
#define BTN_WIDTH  170
#define BTN_HEIGHT 63


// �Խ���ťId����
#define IDC_CLOSE		4000
#define IDC_BTN_ADD		4001
#define IDC_BTN_MODIFY	4002
#define IDC_BTN_DELETE	4003
#define IDC_BTN_START	4004


// ͼƬ�ƶ���ʱ�����
#define TIMER_MOVE		1


// ��׼���캯��
CViewFrame::CViewFrame()
{
	m_iHovering = -1;
	m_iSelected = -1;

	m_iXButtonHovering = -1;

	m_bHovering = FALSE;
	m_bTracking = FALSE;

	m_bAutoMove = FALSE;
	m_bMouseDown = FALSE;
	m_iMouseMoveWidth = 0;

	m_iMoveWidthPerSecond = 0;
}


// ��׼��������
CViewFrame::~CViewFrame()
{
}


// ������Ϣӳ���
BEGIN_MESSAGE_MAP(CViewFrame, CDialog)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_MOUSELEAVE,  &CViewFrame::OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,  &CViewFrame::OnMouseHover)
	ON_COMMAND(IDC_CLOSE,      &CViewFrame::OnBtnClose)
	ON_COMMAND(IDC_BTN_ADD,    &CViewFrame::OnAddGame)
	ON_COMMAND(IDC_BTN_MODIFY, &CViewFrame::OnModify)
	ON_COMMAND(IDC_BTN_DELETE, &CViewFrame::OnDelete)
	ON_COMMAND(IDC_BTN_START,  &CViewFrame::OnStart)
END_MESSAGE_MAP()


// WM_ERASEBKGND��Ϣ������
BOOL CViewFrame::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


// WM_CREATE��Ϣ������
int  CViewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("��Ϸ��ͼ���"));

	// ��ҳ��ǰ����Դ
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDB_FRAME);

	// ��ҳ�汳����Դ
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDB_BACK_FRAME);

	// ���������ڴ�С��λ��
	CRect cClientRect;
	GetClientRect(&cClientRect);
	cClientRect.right = pFrameImage->GetWidth();
	cClientRect.bottom = pFrameImage->GetHeight();
	MoveWindow(&cClientRect);
	CenterWindow(GetParent());

	// �������ڱ�ΪԲ�Ǵ���
	CRgn hRgnClient;
	hRgnClient.CreateRoundRectRgn(0, pFrameImage->GetHeight() - pBackFree->GetHeight(), pFrameImage->GetWidth() + 1, pFrameImage->GetHeight() + 1, 7, 7);
	SetWindowRgn(hRgnClient,FALSE);

	// �����ҳ����ʾ��Ϣ
	AddButton(_T("GBA"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("PSP"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("NDS"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("PS2"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("�ֻ�"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

	// ���õ�ǰ��ʾ��1����ҳ��
	m_iSelected = 0;

	//���õ�ǰҳΪ��һҳ
	m_Page = 1;

	CRect rtButton;
	CXButton dcControl;

	// �����رհ�ť
	rtButton.top = pFrameImage->GetHeight() - pBackFree->GetHeight();
	rtButton.bottom = rtButton.top + 22;
	rtButton.right = cClientRect.Width() - 4;
	rtButton.left = rtButton.right - 47;
	//dcControl.CreateButton(this, rtButton, IDB_BT_CLOSE, 4, NULL, 0, IDC_CLOSE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// ������Ϸ�б�
	rtGameList.left = 10;
	rtGameList.top = 10;
	rtGameList.right = rtGameList.left + 750;
	rtGameList.bottom = rtGameList.top + 455;
	m_GameRectList.push_back(rtGameList);

	m_ListControl = new CListControl;
	if (m_ListControl->Create(IDD_LIST_CONTROL, this))
	{
		m_ListControl->MoveWindow(rtGameList);
		m_ListControl->m_Game_List.MoveWindow(0, 0, rtGameList.right - 10, rtGameList.bottom - 10);

		m_ListControl->ShowWindow(SW_SHOW);
	}


	// ������ҳ��1�� ��Ӱ�ť
	rtButton.left = 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��1�� �޸İ�ť
	rtButton.left = 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��1�� ɾ����ť
	rtButton.left = 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// ������ҳ��1�� ��ʼ��ť
	rtButton.left = 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);



	// ������ҳ��2�� ��Ӱ�ť
	rtButton.left = pFrameImage->GetWidth() * 1 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��2�� �޸İ�ť
	rtButton.left = pFrameImage->GetWidth() * 1 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��2�� ɾ����ť
	rtButton.left = pFrameImage->GetWidth() * 1 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��2�� ��ʼ��ť
	rtButton.left = pFrameImage->GetWidth() * 1 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);



	// ������ҳ��3�� ��Ӱ�ť
	rtButton.left = pFrameImage->GetWidth() * 2 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��3�� �޸İ�ť
	rtButton.left = pFrameImage->GetWidth() * 2 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��3�� ɾ����ť
	rtButton.left = pFrameImage->GetWidth() * 2 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��3�� ��ʼ��ť
	rtButton.left = pFrameImage->GetWidth() * 2 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// ������ҳ��4�� ��Ӱ�ť
	rtButton.left = pFrameImage->GetWidth() * 3 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��4�� �޸İ�ť
	rtButton.left = pFrameImage->GetWidth() * 3 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��4�� ɾ����ť
	rtButton.left = pFrameImage->GetWidth() * 3 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��4�� ��ʼ��ť
	rtButton.left = pFrameImage->GetWidth() * 3 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��5�� ��Ӱ�ť
	rtButton.left = pFrameImage->GetWidth() * 4 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��5�� �޸İ�ť
	rtButton.left = pFrameImage->GetWidth() * 4 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��5�� ɾ����ť
	rtButton.left = pFrameImage->GetWidth() * 4 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��5�� ��ʼ��ť
	rtButton.left = pFrameImage->GetWidth() * 4 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// ������ҳ��6�� ��Ӱ�ť
	rtButton.left = pFrameImage->GetWidth() * 5 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��6�� �޸İ�ť
	rtButton.left = pFrameImage->GetWidth() * 5 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��6�� ɾ����ť
	rtButton.left = pFrameImage->GetWidth() * 5 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��6�� ��ʼ��ť
	rtButton.left = pFrameImage->GetWidth() * 5 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	return 0;
}


// WM_PAINT��Ϣ������
void CViewFrame::OnPaint()
{
	// ʹ��UNICODE�ַ�ת��
	USES_CONVERSION;

	CPaintDC dc(this);
	CMemoryDC dcMem(&dc);

	// ��ȡ���ھ���λ��
	CRect rtClient;
	GetClientRect(rtClient);

	// ��ȡ�ػ����λ��
	CRect rcPaint;
	dcMem.GetClipBox(&rcPaint);

	Gdiplus::Graphics graphics(dcMem.m_hDC);

	// ��ҳ��ǰ����Դ
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDB_FRAME);

	// ��ť�Ϸ���ͷ��Դ
	Image *pArrowImage = CImageInfo::Instance()->ImageFromResource(IDB_ARROW);

	// ��ҳ�汳����Դ
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDB_BACK_FRAME);

	// �������Ϸ��°����԰�ť�ı���
	RectF gdiRect;
	gdiRect.X = 19, gdiRect.Y = 0, gdiRect.Width = 120, gdiRect.Height = 5;
	graphics.DrawImage(pBackFree, gdiRect, 0, 0, 120, 5,UnitPixel);

	// ��������ܱ���ͼƬ
	for(int i = -1; i <= (int)m_ButtonList.size(); i++)
	{
		// ���㵱ǰͼƬ��ʾλ���Ƿ�����������ʾ��Χ
		int iCurrentX = (i*(int)pFrameImage->GetWidth() - (m_iSelected * (int)pFrameImage->GetWidth())+m_iMouseMoveWidth);
		if(iCurrentX > rtClient.Width() ||  iCurrentX + rtClient.Width() < 0)
		{
			continue;
		}

		Image* pBackImage = NULL;
		if(i == -1 || i == (int)m_ButtonList.size())
		{
			pBackImage = pBackFree;
		}
		else
		{
			pBackImage = CImageInfo::Instance()->ImageFromResource(m_ButtonList[i].nBack);
		}

		gdiRect.X = (Gdiplus::REAL)iCurrentX;
		gdiRect.Y = (Gdiplus::REAL)pFrameImage->GetHeight() - (Gdiplus::REAL)pBackImage->GetHeight();
		gdiRect.Height = (Gdiplus::REAL)pBackImage->GetHeight();
		gdiRect.Width = (Gdiplus::REAL)pBackImage->GetWidth();
		graphics.DrawImage(pBackImage, gdiRect, 0, 0, (Gdiplus::REAL)pBackImage->GetWidth(), (Gdiplus::REAL)pBackImage->GetHeight(), UnitPixel);
	}
  
	// ���������ǰ��ͼƬ
	graphics.DrawImage(pFrameImage, 0, 0, pFrameImage->GetWidth(), pFrameImage->GetHeight());

	// ���ƽ����л���ť�Ϸ��ļ�ͷ
	int iCurrent = ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth)*BTN_WIDTH/(int)pFrameImage->GetWidth();
	if(iCurrent >= 0)
	{
		gdiRect.X = 0, gdiRect.Y = 490; gdiRect.Width = (Gdiplus::REAL)iCurrent; gdiRect.Height = (Gdiplus::REAL)pArrowImage->GetHeight();
		graphics.DrawImage(pArrowImage, gdiRect, 0, 0, 1, (Gdiplus::REAL)pArrowImage->GetHeight(), UnitPixel);
	}

	gdiRect.X = (Gdiplus::REAL)iCurrent, gdiRect.Y = 490; gdiRect.Width = (Gdiplus::REAL)pArrowImage->GetWidth(); gdiRect.Height = (Gdiplus::REAL)pArrowImage->GetHeight();
	graphics.DrawImage(pArrowImage, gdiRect, 0, 0, (Gdiplus::REAL)pArrowImage->GetWidth(), (Gdiplus::REAL)pArrowImage->GetHeight(), UnitPixel);

	if( iCurrent + (int)pArrowImage->GetWidth() < (int)pFrameImage->GetWidth())
	{
		gdiRect.X = (Gdiplus::REAL)iCurrent + (int)pArrowImage->GetWidth(), gdiRect.Y = 490; gdiRect.Width = (Gdiplus::REAL)pFrameImage->GetWidth() - (iCurrent + pArrowImage->GetWidth()); gdiRect.Height = (Gdiplus::REAL)pArrowImage->GetHeight();
		graphics.DrawImage(pArrowImage, gdiRect, 0, 0, 1, (Gdiplus::REAL)pArrowImage->GetHeight(), UnitPixel);
	}

	// ������չʾ����ͼƬ�л���ť
	StringFormat stringFormat;
	stringFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentNear);

	LOGFONT lfFont;
	memset(&lfFont, 0, sizeof(lfFont));
	lfFont.lfHeight = -15;
	lfFont.lfWeight |= FW_BOLD;
	lstrcpy(lfFont.lfFaceName, _T("����"));
	Font font(dcMem.GetSafeHdc(), &lfFont);
	Image *pBackHover = CImageInfo::Instance()->ImageFromResource(IDB_HOVER);
	for(int i = 0; i < (int)m_ButtonList.size(); i++)
	{
		CRect rcButton;
		GetButtonRect( i, rcButton );
		Point point(rcButton.left, rcButton.top);
		// �жϰ�ť�Ƿ���Ҫˢ��
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcButton, rcPaint))
		{
			continue;
		}

		// ��ȡ��ť״̬
		int iOffset = 0;
		if(m_iSelected == i)iOffset = 2;
		else if(m_iHovering == i)iOffset = 1;
		else iOffset = 0;

		// ���ư�ť����
		RectF gdiRect;
		gdiRect.X = (Gdiplus::REAL)point.X, gdiRect.Y = (Gdiplus::REAL)point.Y + 192; gdiRect.Width = (Gdiplus::REAL)pBackHover->GetWidth() / 3; gdiRect.Height = (Gdiplus::REAL)pBackHover->GetHeight();
		graphics.DrawImage(pBackHover, gdiRect, (Gdiplus::REAL)pBackHover->GetWidth() / 3 * iOffset, 0, (Gdiplus::REAL)pBackHover->GetWidth() / 3, (Gdiplus::REAL)pBackHover->GetHeight(), UnitPixel);

		// ���ư�ťͼ��
		Image* pIconImage = CImageInfo::Instance()->ImageFromResource(m_ButtonList[i].nIcon);
		gdiRect.X = (Gdiplus::REAL)point.X + 10, gdiRect.Y = (Gdiplus::REAL)point.Y + 198; gdiRect.Width = (Gdiplus::REAL)pIconImage->GetWidth(); gdiRect.Height = (Gdiplus::REAL)pIconImage->GetHeight();
		graphics.DrawImage(pIconImage, gdiRect);

		// ���ư�ť����
		CString sName = m_ButtonList[i].sName;
		CStringW wName = T2W(sName.GetBuffer());
		SolidBrush brush((ARGB)Color::White);
		gdiRect.X = (Gdiplus::REAL)point.X + 49, gdiRect.Y = (Gdiplus::REAL)point.Y + 207; gdiRect.Width = (Gdiplus::REAL)rcButton.Width() - 52; gdiRect.Height = (Gdiplus::REAL)rcButton.Height() - 15;
		graphics.DrawString(wName, wName.GetLength(), &font, gdiRect,&stringFormat, &brush);
	}

	// ���ƽ���رհ�ť��������ť
	for(size_t i = 0; i< m_XButtonList.size(); i++)
	{
		CXButton& dcControl = m_XButtonList[i];
		CRect rcControl;
		if(i == 0)
		{
			// ��ǰ��ť�Ǵ��ڹرհ�ť
			dcControl.GetRect(&rcControl);
		}
		else
		{
			// ��ǰ��ť����ҳ���еĹ��ܰ�ť
			rcControl = m_XButtonRectList[i];
			rcControl.left -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			rcControl.right -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			dcControl.SetRect(&rcControl);
		}
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		dcControl.DrawButton(graphics);
	}


	// ��ʾ�б���ͼ
	CRect rcControl = m_GameRectList[0];

	if (m_iSelected == 0)
		rcControl.left = 10;
	else
		rcControl.left = pFrameImage->GetWidth() * m_iSelected + 10;

	rcControl.top = 10;
	rcControl.bottom = rcControl.top + 455;
	rcControl.right = rcControl.left + 750;

	rcControl.left -= ((m_iSelected * (int)pFrameImage->GetWidth()) - m_iMouseMoveWidth);
	rcControl.right -= ((m_iSelected * (int)pFrameImage->GetWidth()) - m_iMouseMoveWidth);

	m_ListControl->MoveWindow(rcControl);
	m_ListControl->m_Game_List.MoveWindow(0, 0, rcControl.right - 10, rcControl.bottom - 10);
}


// WM_LBUTTONDOWN��Ϣ������
void CViewFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bMouseDown)
		return;

	// ��������Ƿ����л���ť��
	CRect rtButton;
	int iButton = HitTest(point, rtButton);
	if(iButton != -1)
	{
		if(m_bAutoMove)
		{
			return;
		}

		// ����������ͼƬ�Զ��ƶ�����ת��ѡ��ͼƬ
		CRect rtClient;
		GetClientRect(&rtClient);
		m_iMouseMoveWidth = (iButton-m_iSelected)*rtClient.Width();
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;

		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;


		// �ж��л���ť
		if (iButton == 0 && m_Page != 1)
			OnPrevious();

		else if (iButton == 5 && m_Page != 5)
			OnNextPage();

		else
			m_iSelected = iButton;

		// �ж���Ϸ����
		OnSwitchType();

		return;
	}

	// ��������Ƿ���������ť��
	iButton = XButtonHitTest(point, rtButton);
	if(iButton != -1)
	{
		if(m_bAutoMove)
		{
			return;
		}
		m_XButtonList[iButton].LButtonDown();
		return;
	}

	// ��괦�ڱ���λ��
	if(point.y < 28)
	{
		//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
		return;
	}

	// ��괦�ڽ���ͼƬ֮��
	if(m_bAutoMove)
	{
		// ��ǰͼƬ�����Զ��ƶ�
		KillTimer(TIMER_MOVE);
		m_bAutoMove = FALSE;
		m_iMouseDownX = point.x - m_iMouseMoveWidth;
	}
	else
	{
		m_iMouseDownX = point.x;
	}
	m_bMouseDown = TRUE;
	SetCapture();
}


// WM_MOUSEMOVE��Ϣ������
void CViewFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	// ������뿪�¼����и���
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	if(m_bAutoMove)
	{
		return;
	}

	// ����Ѿ���������ͼƬ����
	if(m_bMouseDown)
	{
		CRect rtClient;
		GetClientRect(&rtClient);
		m_iMouseMoveWidth = point.x - m_iMouseDownX;
		if(m_iSelected == 0 && m_iMouseMoveWidth > rtClient.Width())
		{
			m_iMouseMoveWidth = rtClient.Width();
		}
		if(m_iSelected == (int)m_ButtonList.size()-1 && -m_iMouseMoveWidth > rtClient.Width())
		{
			m_iMouseMoveWidth = -rtClient.Width();
		}
		Invalidate();
		return;
	}

	// ��������Ƿ����л���ť��
	CRect rtButton;
	int iButton = HitTest(point, rtButton);
	if(iButton != m_iHovering)
	{
		/*if(m_iHovering != -1)
		{
			CRect oldRect;
			GetButtonRect(m_iHovering, oldRect);
			m_iHovering = -1;
			InvalidateRect(oldRect);
		}
		if(iButton != -1)
		{
			m_iHovering = iButton;
			InvalidateRect(rtButton);
		};*/
	}

	// ��������Ƿ���������ť��
	iButton = XButtonHitTest(point, rtButton);
	if(iButton != m_iXButtonHovering)
	{
		if(m_iXButtonHovering != -1)
		{
			m_XButtonList[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = -1;
		}
		if(iButton != -1)
		{
			m_iXButtonHovering = iButton;
			m_XButtonList[m_iXButtonHovering].MouseHover();
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


// WM_LBUTTONUP��Ϣ������
void CViewFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// ����Ѿ���������ͼƬ����
	if(m_bMouseDown)
	{
		m_bMouseDown = FALSE;
		ReleaseCapture();

		CRect rtClient;
		GetClientRect(&rtClient);
		// ��������϶���
		if(m_iMouseMoveWidth > 0)
		{
			if(m_iSelected > 0 && m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth -= rtClient.Width();
				m_iSelected--;
			}
		}
		// ��������϶���
		else
		{
			if(m_iSelected < (int)m_ButtonList.size()-1 && -m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth += rtClient.Width();
				m_iSelected++;
			}
		}
		// ����������ͼƬ�Զ��ƶ����ָ���ǰͼƬ������ת��ѡ��ͼƬ
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;
		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;
		return;
	}

	// ��������Ƿ���������ť��
	CRect rtButton;
	int iButton = XButtonHitTest(point, rtButton);
	if(iButton != -1)
	{
		m_XButtonList[iButton].LButtonUp();
	}
}


// WM_LBUTTONDBLCLK��Ϣ������
void CViewFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}


// WM_TIMER��Ϣ������
void CViewFrame::OnTimer(UINT_PTR nIDEvent)
{
	if(!m_bAutoMove)
		return;


	// �ж��л���ť
	if (m_iSelected == 0 && m_Page != 1)
		OnPrevious();

	else if (m_iSelected == 5 && m_Page != 5)
		OnNextPage();

	// �ж���Ϸ����
	OnSwitchType();


	// ����Ѿ������϶������Ѿ�ѡ����ͼƬ
	if(m_iMouseMoveWidth > 0)
	{
		if(m_iMouseMoveWidth - m_iMoveWidthPerSecond <= 0)
		{
			m_iMouseMoveWidth = 0;
			Invalidate();
			KillTimer(TIMER_MOVE);
			m_bAutoMove = FALSE;
		}
		else
		{
			m_iMouseMoveWidth-= m_iMoveWidthPerSecond;
			Invalidate();
		}
	}
	// ����Ѿ������϶������Ѿ�ѡ���ҷ�ͼƬ
	else
	{
		if(m_iMouseMoveWidth + m_iMoveWidthPerSecond >= 0)
		{
			m_iMouseMoveWidth = 0;
			Invalidate();
			KillTimer(TIMER_MOVE);
			m_bAutoMove = FALSE;
		}
		else
		{
			m_iMouseMoveWidth+= m_iMoveWidthPerSecond;
			Invalidate();
		}
	}

	CWnd::OnTimer(nIDEvent);
}


// �ڴ������ٺ����
void CViewFrame::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	
	// �ͷ��б���Դ
	delete m_ListControl;

	// �Զ��ͷŴ�����Դ
	delete this;
}


// ���ڹرհ�ť�¼�
void CViewFrame::OnBtnClose()
{
	SendMessage( WM_SYSCOMMAND,SC_CLOSE);
}


// ���ͼƬҳ�溯��
int  CViewFrame::AddButton(LPCTSTR szName, UINT nIcon, UINT nBack)
{
	ButtonInfo buttonInfo;
	buttonInfo.sName = szName;
	buttonInfo.nIcon = nIcon;
	buttonInfo.nBack = nBack;
	m_ButtonList.push_back(buttonInfo);
	return (int)m_ButtonList.size()-1;
}


// ������������л���ťλ��
int  CViewFrame::HitTest(CPoint point, CRect& rtButton)
{
	if(point.y < 519 || point.y > 554)
	{
		return -1;
	}

	if (point.x % BTN_WIDTH < 6 || point.x % BTN_WIDTH > 162)
	{
		return -1;
	}

	if(!GetButtonRect(point.x/BTN_WIDTH, rtButton))
	{
		return -1;
	}

	return point.x / BTN_WIDTH;
}


// ��ȡָ���л���ť����λ��
BOOL CViewFrame::GetButtonRect(int iButton, CRect& rtButton)
{
	if(iButton >= (int)m_ButtonList.size())
	{
		return FALSE;
	}

	rtButton.top = 317;
	rtButton.bottom = 362;
	rtButton.left = 1+ iButton * BTN_WIDTH + 6;
	rtButton.right =  1+ iButton * BTN_WIDTH + 163;

	return TRUE;
}
 

// �����������������ťλ��
int  CViewFrame::XButtonHitTest(CPoint point, CRect& rtButton)
{
	for(size_t i = 0; i< m_XButtonList.size(); i++)
	{
		CXButton& dcControl = m_XButtonList[i];
		if(dcControl.PtInButton(point))
		{
			m_XButtonList[i].GetRect(&rtButton);
			return (int)i;
		}
	}
	return -1;
}


// �����봰����Ϣ֪ͨ
LRESULT CViewFrame::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bHovering = TRUE;

	return 0;
}


// ����뿪������Ϣ֪ͨ
LRESULT CViewFrame::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHovering = FALSE;
	if(m_iHovering != -1)
	{
		CRect oldRect;
		GetButtonRect(m_iHovering, oldRect);
		m_iHovering = -1;
		InvalidateRect(oldRect);
	}
	if(m_iXButtonHovering != -1)
	{
		m_XButtonList[m_iXButtonHovering].MouseLeave();
		m_iXButtonHovering = -1;
	}
	return 0;
}


// ��һҳ��ť����
void CViewFrame::OnPrevious()
{
	switch (m_Page)
	{
	case 0: case 1:
		break;

	case 2:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("GBA"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PSP"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NDS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PS2"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("�ֻ�"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 0;
		m_Page -= 1;

		// ������Ϸ����
		theApp.Type = _T("GBA");
		break;

	case 3:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PS1"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("FC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("N64"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NGC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 1;
		m_Page -= 1;

		// ������Ϸ����
		theApp.Type = _T("PS1");
		break;

	case 4:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("WII"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SFC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PCE"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 1;
		m_Page -= 1;

		// ������Ϸ����
		theApp.Type = _T("WII");
		break;

	case 5:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SEGA"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("DC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("WSC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PC98"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 1;
		m_Page -= 1;

		// ������Ϸ����
		theApp.Type = _T("SEGA");
		break;


	case 6:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("3DS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("Atari"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("OpenBor"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NGPC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("Java"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 1;
		m_Page -= 1;

		// ������Ϸ����
		theApp.Type = _T("3DS");
		break;


	default:
		break;
	}
}


// ��һҳ��ť����
void CViewFrame::OnNextPage()
{
	switch (m_Page)
	{
	case 0: case 6:
		break;

	case 1:
		// �������
		m_ButtonList.clear();

		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PS1"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("FC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("N64"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NGC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 1;
		m_Page += 1;

		// ������Ϸ����
		theApp.Type = _T("PS1");
		break;

	case 2:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("WII"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SFC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PCE"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 1;
		m_Page += 1;

		// ������Ϸ����
		theApp.Type = _T("WII");
		break;

	case 3:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SEGA"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("DC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("WSC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PC98"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 1;
		m_Page += 1;

		// ������Ϸ����
		theApp.Type = _T("SEGA");
		break;

	case 4:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("3DS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("Atari"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("OpenBor"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NGPC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("Java"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 1;
		m_Page += 1;

		// ������Ϸ����
		theApp.Type = _T("3DS");
		break;


	case 5:
		// �������
		m_ButtonList.clear();

		// �����ҳ����ʾ��Ϣ
		AddButton(_T("GBA"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PSP"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NDS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PS2"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("�ֻ�"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("��һҳ"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// ���õ�ǰҳ��
		m_iSelected = 0;
		m_Page = 1;

		// ������Ϸ����
		theApp.Type = _T("GBA");
		break;


	default:
		break;
	}
}


// ��Ϸ�����ж�
void CViewFrame::OnSwitchType()
{
	// �ж�ѡ�������
	switch (m_Page)
	{
	case 0: case 1:

		if (m_iSelected == 0)
			theApp.Type = _T("GBA");

		else if (m_iSelected == 1)
			theApp.Type = _T("PSP");

		else if (m_iSelected == 2)
			theApp.Type = _T("NDS");

		else if (m_iSelected == 3)
			theApp.Type = _T("PS2");

		else if (m_iSelected == 4)
			theApp.Type = _T("�ֻ�");

		else
			theApp.Type = _T("PS1");

		break;

	case 2:

		if (m_iSelected == 0)
			theApp.Type = _T("�ֻ�");

		else if (m_iSelected == 1)
			theApp.Type = _T("PS1");

		else if (m_iSelected == 2)
			theApp.Type = _T("FC");

		else if (m_iSelected == 3)
			theApp.Type = _T("N64");

		else if (m_iSelected == 4)
			theApp.Type = _T("NGC");

		else
			theApp.Type = _T("WII");

		break;

	case 3:

		if (m_iSelected == 0)
			theApp.Type = _T("NGC");

		else if (m_iSelected == 1)
			theApp.Type = _T("WII");

		else if (m_iSelected == 2)
			theApp.Type = _T("SFC");

		else if (m_iSelected == 3)
			theApp.Type = _T("SS");

		else if (m_iSelected == 4)
			theApp.Type = _T("PCE");

		else
			theApp.Type = _T("SEGA");

		break;

	case 4:

		if (m_iSelected == 0)
			theApp.Type = _T("PCE");

		else if (m_iSelected == 1)
			theApp.Type = _T("SEGA");

		else if (m_iSelected == 2)
			theApp.Type = _T("DC");

		else if (m_iSelected == 3)
			theApp.Type = _T("WSC");

		else if (m_iSelected == 4)
			theApp.Type = _T("PC98");

		else
			theApp.Type = _T("3DS");

		break;

	case 5:

		if (m_iSelected == 0)
			theApp.Type = _T("PC98");

		else if (m_iSelected == 1)
			theApp.Type = _T("3DS");

		else if (m_iSelected == 2)
			theApp.Type = _T("Atari");

		else if (m_iSelected == 3)
			theApp.Type = _T("OpenBor");

		else if (m_iSelected == 4)
			theApp.Type = _T("NGPC");

		else if (m_iSelected == 5)
			theApp.Type = _T("JAVA");

		else
			theApp.Type = _T("GBA");

		break;

	default:
		break;
	}

	//  ˢ����Ϸ�б�
	m_ListControl->OnRefresh();

	return;
}


// ����Ϸ�б��������Ϸ
void CViewFrame::OnAddGame()
{
	m_ListControl->OnAddGame();
}


// ����Ϸ�б����޸���Ϸ
void CViewFrame::OnModify()
{
	m_ListControl->OnModifyGame();
}


// ����Ϸ�б���ɾ����Ϸ
void CViewFrame::OnDelete()
{
	m_ListControl->OnDeleteGame();
}


// ��ʼ������Ϸ�б���ѡ�����Ϸ
void CViewFrame::OnStart()
{
	m_ListControl->OnStartGame();
}
