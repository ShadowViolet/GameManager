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
	// Release在析构中调用，因为GdiplusShutdown的原因己调用的原因会导致内存崩溃
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


// 绘制按钮
void CXButton::DrawButton(Gdiplus::Graphics&  graphics)
{
	// 获取按钮图片信息
	Image* pImage = CImageInfo::Instance()->ImageFromResource(m_nImageContorl);
	UINT iCount = m_nImageCount;
	if (m_bCheck && m_nAltImageContorl != 0)
	{
		pImage = CImageInfo::Instance()->ImageFromResource(m_nAltImageContorl);;
		iCount = m_nAltImageCount;
	}

	// 获取按钮状态信息
	int	iButtonIndex = 0;
	if (m_bDisabled && iCount >= 4) iButtonIndex = 3;
	else if (m_bPressed && iCount >= 3)iButtonIndex = 2;
	else if (m_bHovering && iCount >= 2)iButtonIndex = 1;
	else iButtonIndex = 0;

	// 在指定位置绘制按钮
	int iWidth = pImage->GetWidth() / iCount;
	int iHeight = pImage->GetHeight();
	RectF grect;
	grect.X = (Gdiplus::REAL)m_rcRect.left, grect.Y = (Gdiplus::REAL)m_rcRect.top; grect.Width = (Gdiplus::REAL)m_rcRect.Width(); grect.Height = (Gdiplus::REAL)m_rcRect.Height();
	graphics.DrawImage(pImage, grect, (Gdiplus::REAL)iWidth*iButtonIndex, 0, (Gdiplus::REAL)iWidth, (Gdiplus::REAL)iHeight, UnitPixel);
}


// 创建按钮
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


// 切换按钮大小定义
#define BTN_WIDTH  170
#define BTN_HEIGHT 63


// 自建按钮Id定义
#define IDC_CLOSE		4000
#define IDC_BTN_ADD		4001
#define IDC_BTN_MODIFY	4002
#define IDC_BTN_DELETE	4003
#define IDC_BTN_START	4004


// 图片移动定时器编号
#define TIMER_MOVE		1


// 标准构造函数
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


// 标准析构函数
CViewFrame::~CViewFrame()
{
}


// 窗口消息映射表
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


// WM_ERASEBKGND消息处理函数
BOOL CViewFrame::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


// WM_CREATE消息处理函数
int  CViewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("游戏视图框架"));

	// 主页面前景资源
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDB_FRAME);

	// 主页面背景资源
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDB_BACK_FRAME);

	// 调整主窗口大小及位置
	CRect cClientRect;
	GetClientRect(&cClientRect);
	cClientRect.right = pFrameImage->GetWidth();
	cClientRect.bottom = pFrameImage->GetHeight();
	MoveWindow(&cClientRect);
	CenterWindow(GetParent());

	// 将主窗口变为圆角窗口
	CRgn hRgnClient;
	hRgnClient.CreateRoundRectRgn(0, pFrameImage->GetHeight() - pBackFree->GetHeight(), pFrameImage->GetWidth() + 1, pFrameImage->GetHeight() + 1, 7, 7);
	SetWindowRgn(hRgnClient,FALSE);

	// 添加主页面显示信息
	AddButton(_T("GBA"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("PSP"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("NDS"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("PS2"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("街机"), IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

	// 设置当前显示第1个子页面
	m_iSelected = 0;

	//设置当前页为第一页
	m_Page = 1;

	CRect rtButton;
	CXButton dcControl;

	// 创建关闭按钮
	rtButton.top = pFrameImage->GetHeight() - pBackFree->GetHeight();
	rtButton.bottom = rtButton.top + 22;
	rtButton.right = cClientRect.Width() - 4;
	rtButton.left = rtButton.right - 47;
	//dcControl.CreateButton(this, rtButton, IDB_BT_CLOSE, 4, NULL, 0, IDC_CLOSE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// 创建游戏列表
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


	// 创建子页面1中 添加按钮
	rtButton.left = 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面1中 修改按钮
	rtButton.left = 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面1中 删除按钮
	rtButton.left = 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// 创建子页面1中 开始按钮
	rtButton.left = 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);



	// 创建子页面2中 添加按钮
	rtButton.left = pFrameImage->GetWidth() * 1 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面2中 修改按钮
	rtButton.left = pFrameImage->GetWidth() * 1 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面2中 删除按钮
	rtButton.left = pFrameImage->GetWidth() * 1 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面2中 开始按钮
	rtButton.left = pFrameImage->GetWidth() * 1 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);



	// 创建子页面3中 添加按钮
	rtButton.left = pFrameImage->GetWidth() * 2 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面3中 修改按钮
	rtButton.left = pFrameImage->GetWidth() * 2 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面3中 删除按钮
	rtButton.left = pFrameImage->GetWidth() * 2 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面3中 开始按钮
	rtButton.left = pFrameImage->GetWidth() * 2 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// 创建子页面4中 添加按钮
	rtButton.left = pFrameImage->GetWidth() * 3 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面4中 修改按钮
	rtButton.left = pFrameImage->GetWidth() * 3 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面4中 删除按钮
	rtButton.left = pFrameImage->GetWidth() * 3 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面4中 开始按钮
	rtButton.left = pFrameImage->GetWidth() * 3 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面5中 添加按钮
	rtButton.left = pFrameImage->GetWidth() * 4 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面5中 修改按钮
	rtButton.left = pFrameImage->GetWidth() * 4 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面5中 删除按钮
	rtButton.left = pFrameImage->GetWidth() * 4 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面5中 开始按钮
	rtButton.left = pFrameImage->GetWidth() * 4 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// 创建子页面6中 添加按钮
	rtButton.left = pFrameImage->GetWidth() * 5 + 800;
	rtButton.top = 50;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_ADD);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面6中 修改按钮
	rtButton.left = pFrameImage->GetWidth() * 5 + 800;
	rtButton.top = 150;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_MODIFY);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面6中 删除按钮
	rtButton.left = pFrameImage->GetWidth() * 5 + 800;
	rtButton.top = 250;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_DELETE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面6中 开始按钮
	rtButton.left = pFrameImage->GetWidth() * 5 + 800;
	rtButton.top = 350;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;
	dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_START);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	return 0;
}


// WM_PAINT消息处理函数
void CViewFrame::OnPaint()
{
	// 使用UNICODE字符转换
	USES_CONVERSION;

	CPaintDC dc(this);
	CMemoryDC dcMem(&dc);

	// 获取窗口矩形位置
	CRect rtClient;
	GetClientRect(rtClient);

	// 获取重绘矩形位置
	CRect rcPaint;
	dcMem.GetClipBox(&rcPaint);

	Gdiplus::Graphics graphics(dcMem.m_hDC);

	// 主页面前景资源
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDB_FRAME);

	// 按钮上方箭头资源
	Image *pArrowImage = CImageInfo::Instance()->ImageFromResource(IDB_ARROW);

	// 主页面背景资源
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDB_BACK_FRAME);

	// 绘制最上方新版特性按钮的背景
	RectF gdiRect;
	gdiRect.X = 19, gdiRect.Y = 0, gdiRect.Width = 120, gdiRect.Height = 5;
	graphics.DrawImage(pBackFree, gdiRect, 0, 0, 120, 5,UnitPixel);

	// 绘制主框架背景图片
	for(int i = -1; i <= (int)m_ButtonList.size(); i++)
	{
		// 计算当前图片显示位置是否在主界面显示范围
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
  
	// 绘制主框架前景图片
	graphics.DrawImage(pFrameImage, 0, 0, pFrameImage->GetWidth(), pFrameImage->GetHeight());

	// 绘制界面切换按钮上方的箭头
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

	// 绘制主展示界面图片切换按钮
	StringFormat stringFormat;
	stringFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentNear);

	LOGFONT lfFont;
	memset(&lfFont, 0, sizeof(lfFont));
	lfFont.lfHeight = -15;
	lfFont.lfWeight |= FW_BOLD;
	lstrcpy(lfFont.lfFaceName, _T("宋体"));
	Font font(dcMem.GetSafeHdc(), &lfFont);
	Image *pBackHover = CImageInfo::Instance()->ImageFromResource(IDB_HOVER);
	for(int i = 0; i < (int)m_ButtonList.size(); i++)
	{
		CRect rcButton;
		GetButtonRect( i, rcButton );
		Point point(rcButton.left, rcButton.top);
		// 判断按钮是否需要刷新
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcButton, rcPaint))
		{
			continue;
		}

		// 获取按钮状态
		int iOffset = 0;
		if(m_iSelected == i)iOffset = 2;
		else if(m_iHovering == i)iOffset = 1;
		else iOffset = 0;

		// 绘制按钮背景
		RectF gdiRect;
		gdiRect.X = (Gdiplus::REAL)point.X, gdiRect.Y = (Gdiplus::REAL)point.Y + 192; gdiRect.Width = (Gdiplus::REAL)pBackHover->GetWidth() / 3; gdiRect.Height = (Gdiplus::REAL)pBackHover->GetHeight();
		graphics.DrawImage(pBackHover, gdiRect, (Gdiplus::REAL)pBackHover->GetWidth() / 3 * iOffset, 0, (Gdiplus::REAL)pBackHover->GetWidth() / 3, (Gdiplus::REAL)pBackHover->GetHeight(), UnitPixel);

		// 绘制按钮图标
		Image* pIconImage = CImageInfo::Instance()->ImageFromResource(m_ButtonList[i].nIcon);
		gdiRect.X = (Gdiplus::REAL)point.X + 10, gdiRect.Y = (Gdiplus::REAL)point.Y + 198; gdiRect.Width = (Gdiplus::REAL)pIconImage->GetWidth(); gdiRect.Height = (Gdiplus::REAL)pIconImage->GetHeight();
		graphics.DrawImage(pIconImage, gdiRect);

		// 绘制按钮文字
		CString sName = m_ButtonList[i].sName;
		CStringW wName = T2W(sName.GetBuffer());
		SolidBrush brush((ARGB)Color::White);
		gdiRect.X = (Gdiplus::REAL)point.X + 49, gdiRect.Y = (Gdiplus::REAL)point.Y + 207; gdiRect.Width = (Gdiplus::REAL)rcButton.Width() - 52; gdiRect.Height = (Gdiplus::REAL)rcButton.Height() - 15;
		graphics.DrawString(wName, wName.GetLength(), &font, gdiRect,&stringFormat, &brush);
	}

	// 绘制界面关闭按钮等其它按钮
	for(size_t i = 0; i< m_XButtonList.size(); i++)
	{
		CXButton& dcControl = m_XButtonList[i];
		CRect rcControl;
		if(i == 0)
		{
			// 当前按钮是窗口关闭按钮
			dcControl.GetRect(&rcControl);
		}
		else
		{
			// 当前按钮是子页面中的功能按钮
			rcControl = m_XButtonRectList[i];
			rcControl.left -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			rcControl.right -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			dcControl.SetRect(&rcControl);
		}
		// 判断当前按钮是否需要重绘
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		dcControl.DrawButton(graphics);
	}


	// 显示列表视图
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


// WM_LBUTTONDOWN消息处理函数
void CViewFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bMouseDown)
		return;

	// 测试鼠标是否处在切换按钮上
	CRect rtButton;
	int iButton = HitTest(point, rtButton);
	if(iButton != -1)
	{
		if(m_bAutoMove)
		{
			return;
		}

		// 设置主界面图片自动移动，跳转到选定图片
		CRect rtClient;
		GetClientRect(&rtClient);
		m_iMouseMoveWidth = (iButton-m_iSelected)*rtClient.Width();
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;

		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;


		// 判断切换按钮
		if (iButton == 0 && m_Page != 1)
			OnPrevious();

		else if (iButton == 5 && m_Page != 5)
			OnNextPage();

		else
			m_iSelected = iButton;

		// 判断游戏类型
		OnSwitchType();

		return;
	}

	// 测试鼠标是否处在其它按钮上
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

	// 鼠标处于标题位置
	if(point.y < 28)
	{
		//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
		return;
	}

	// 鼠标处于界面图片之上
	if(m_bAutoMove)
	{
		// 当前图片正在自动移动
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


// WM_MOUSEMOVE消息处理函数
void CViewFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	// 对鼠标离开事件进行跟踪
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

	// 鼠标已经在主界面图片按下
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

	// 测试鼠标是否处在切换按钮上
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

	// 测试鼠标是否处在其它按钮上
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


// WM_LBUTTONUP消息处理函数
void CViewFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 鼠标已经在主界面图片按下
	if(m_bMouseDown)
	{
		m_bMouseDown = FALSE;
		ReleaseCapture();

		CRect rtClient;
		GetClientRect(&rtClient);
		// 鼠标向右拖动动
		if(m_iMouseMoveWidth > 0)
		{
			if(m_iSelected > 0 && m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth -= rtClient.Width();
				m_iSelected--;
			}
		}
		// 鼠标向左拖动动
		else
		{
			if(m_iSelected < (int)m_ButtonList.size()-1 && -m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth += rtClient.Width();
				m_iSelected++;
			}
		}
		// 设置主界面图片自动移动，恢复当前图片，或跳转到选定图片
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;
		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;
		return;
	}

	// 测试鼠标是否处在其它按钮上
	CRect rtButton;
	int iButton = XButtonHitTest(point, rtButton);
	if(iButton != -1)
	{
		m_XButtonList[iButton].LButtonUp();
	}
}


// WM_LBUTTONDBLCLK消息处理函数
void CViewFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}


// WM_TIMER消息处理函数
void CViewFrame::OnTimer(UINT_PTR nIDEvent)
{
	if(!m_bAutoMove)
		return;


	// 判断切换按钮
	if (m_iSelected == 0 && m_Page != 1)
		OnPrevious();

	else if (m_iSelected == 5 && m_Page != 5)
		OnNextPage();

	// 判断游戏类型
	OnSwitchType();


	// 鼠标已经向右拖动，或已经选择左方图片
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
	// 鼠标已经向左拖动，或已经选择右方图片
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


// 在窗口销毁后调用
void CViewFrame::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	
	// 释放列表资源
	delete m_ListControl;

	// 自动释放窗口资源
	delete this;
}


// 窗口关闭按钮事件
void CViewFrame::OnBtnClose()
{
	SendMessage( WM_SYSCOMMAND,SC_CLOSE);
}


// 添加图片页面函数
int  CViewFrame::AddButton(LPCTSTR szName, UINT nIcon, UINT nBack)
{
	ButtonInfo buttonInfo;
	buttonInfo.sName = szName;
	buttonInfo.nIcon = nIcon;
	buttonInfo.nBack = nBack;
	m_ButtonList.push_back(buttonInfo);
	return (int)m_ButtonList.size()-1;
}


// 测试鼠标所在切换按钮位置
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


// 获取指定切换按钮所在位置
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
 

// 测试鼠标所在其它按钮位置
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


// 鼠标进入窗口消息通知
LRESULT CViewFrame::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bHovering = TRUE;

	return 0;
}


// 鼠标离开窗口消息通知
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


// 上一页按钮操作
void CViewFrame::OnPrevious()
{
	switch (m_Page)
	{
	case 0: case 1:
		break;

	case 2:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("GBA"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PSP"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NDS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PS2"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("街机"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 0;
		m_Page -= 1;

		// 设置游戏类型
		theApp.Type = _T("GBA");
		break;

	case 3:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("上一页"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PS1"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("FC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("N64"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NGC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 1;
		m_Page -= 1;

		// 设置游戏类型
		theApp.Type = _T("PS1");
		break;

	case 4:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("上一页"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("WII"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SFC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PCE"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 1;
		m_Page -= 1;

		// 设置游戏类型
		theApp.Type = _T("WII");
		break;

	case 5:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("上一页"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SEGA"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("DC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("WSC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PC98"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 1;
		m_Page -= 1;

		// 设置游戏类型
		theApp.Type = _T("SEGA");
		break;


	case 6:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("上一页"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("3DS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("Atari"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("OpenBor"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NGPC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("Java"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 1;
		m_Page -= 1;

		// 设置游戏类型
		theApp.Type = _T("3DS");
		break;


	default:
		break;
	}
}


// 下一页按钮操作
void CViewFrame::OnNextPage()
{
	switch (m_Page)
	{
	case 0: case 6:
		break;

	case 1:
		// 清空链表
		m_ButtonList.clear();

		AddButton(_T("上一页"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PS1"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("FC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("N64"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NGC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 1;
		m_Page += 1;

		// 设置游戏类型
		theApp.Type = _T("PS1");
		break;

	case 2:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("上一页"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("WII"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SFC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PCE"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 1;
		m_Page += 1;

		// 设置游戏类型
		theApp.Type = _T("WII");
		break;

	case 3:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("上一页"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("SEGA"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("DC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("WSC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PC98"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 1;
		m_Page += 1;

		// 设置游戏类型
		theApp.Type = _T("SEGA");
		break;

	case 4:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("上一页"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("3DS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("Atari"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("OpenBor"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NGPC"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("Java"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 1;
		m_Page += 1;

		// 设置游戏类型
		theApp.Type = _T("3DS");
		break;


	case 5:
		// 清空链表
		m_ButtonList.clear();

		// 添加主页面显示信息
		AddButton(_T("GBA"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PSP"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("NDS"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("PS2"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("街机"), IDB_GAME_ITEM, IDB_GAME_LIST);
		AddButton(_T("下一页"), IDB_GAME_ITEM, IDB_GAME_LIST);

		// 设置当前页面
		m_iSelected = 0;
		m_Page = 1;

		// 设置游戏类型
		theApp.Type = _T("GBA");
		break;


	default:
		break;
	}
}


// 游戏类型判断
void CViewFrame::OnSwitchType()
{
	// 判断选择的类型
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
			theApp.Type = _T("街机");

		else
			theApp.Type = _T("PS1");

		break;

	case 2:

		if (m_iSelected == 0)
			theApp.Type = _T("街机");

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

	//  刷新游戏列表
	m_ListControl->OnRefresh();

	return;
}


// 向游戏列表中添加游戏
void CViewFrame::OnAddGame()
{
	m_ListControl->OnAddGame();
}


// 从游戏列表中修改游戏
void CViewFrame::OnModify()
{
	m_ListControl->OnModifyGame();
}


// 从游戏列表中删除游戏
void CViewFrame::OnDelete()
{
	m_ListControl->OnDeleteGame();
}


// 开始运行游戏列表中选择的游戏
void CViewFrame::OnStart()
{
	m_ListControl->OnStartGame();
}
