#pragma once

#include <vector>
#include <map>
#include <memory>

#include "stdafx.h"
using namespace std;
using std::vector;


#include "SortListCtrl.h"
#include "ListControl.h"


class CImageInfo
{
public:
	static CImageInfo * Instance();

	~CImageInfo(void);

	// 释放所有界面资源
	void	Release();

	// 释放界面资源Id加载界面资源
	Image*	ImageFromResource(UINT uId);

private:
	CImageInfo(void);
	static std::auto_ptr<CImageInfo> sm_inst;

	Image*	FromResource(UINT uId);

	map<UINT, Image*> m_mpImage;
};


class CMemoryDC : public CDC 
{
public:
	CMemoryDC(CDC* pDC, CRect rect = CRect(0, 0, 0, 0), BOOL bCopyFirst = FALSE)
		: CDC(), m_oldBitmap(NULL), m_pDC(pDC)
	{
		ASSERT(m_pDC != NULL);	// If you asserted here, you passed in a NULL CDC.

		m_bMemDC = !pDC->IsPrinting();

		if (m_bMemDC){
			// Create a Memory DC
			CreateCompatibleDC(pDC);
			if (rect == CRect(0, 0, 0, 0))
				pDC->GetClipBox(&m_rect);
			else
				m_rect = rect;

			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);
			if (bCopyFirst)
			{
				this->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
					m_pDC, m_rect.left, m_rect.top, SRCCOPY);
			}
		}
		else {
			// Make a copy of the relevent parts of the current DC for printing
			m_bPrinting = pDC->m_bPrinting;
			m_hDC = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}

	~CMemoryDC()
	{
		if (m_bMemDC) {
			// Copy the offscreen bitmap onto the screen.
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);
			// Swap back the original bitmap.
			SelectObject(m_oldBitmap);
		}
		else {
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.
			m_hDC = m_hAttribDC = NULL;
		}
	}

	// Allow usage as a pointer
	CMemoryDC* operator->() { return this; }

	// Allow usage as a pointer
	operator CMemoryDC*() { return this; }

private:
	CBitmap   m_bitmap;			// Offscreen bitmap
	CBitmap*  m_oldBitmap;		// bitmap originally found in CMemoryDC
	CDC*      m_pDC;			// Saves CDC passed in constructor
	CRect     m_rect;			// Rectangle of drawing area.
	BOOL      m_bMemDC;			// TRUE if CDC really is a Memory DC.
};


class CXButton
{
public:
	// 标准构造函数
	CXButton()
	{
		m_bCheck = FALSE;
		m_bHovering = FALSE;
		m_bPressed = FALSE;
		m_bDisabled = FALSE;
		m_nImageContorl = 0;
		m_nAltImageContorl = 0;
	}

	// 设置按钮矩形位置
	void SetRect(LPCRECT lpRect)
	{ 
		m_rcRect = lpRect;
	}

	// 获取按钮矩形位置
	void GetRect(LPRECT lpRect)
	{ 
		*lpRect = *m_rcRect;
	}

	// 检测指定点是否处于按钮内
	BOOL PtInButton(CPoint& point)
	{ 
		return m_rcRect.PtInRect(point); 
	}

	// 绘制按钮
	void DrawButton(Gdiplus::Graphics&  graphics);

	// 创建按钮
	void CreateButton(CWnd* pParent, LPRECT lpRect, UINT nImage, UINT nCount = 4, UINT sAltImage = 0, UINT nAltCount = 0, UINT nID = 0);

	// 设置按钮状态
	void EnableButton(BOOL bEnable)
	{
		m_bDisabled = !bEnable;
		m_pParent->InvalidateRect(m_rcRect);
	}

	// 设置按钮选定状态
	void SetCheck(BOOL bCheck)
	{
		m_bCheck = bCheck;
		m_pParent->InvalidateRect(m_rcRect);
	}

	// 获取按钮Id
	UINT GetControlId()
	{ 
		return m_nID; 
	}

	// 触发按钮事件
	void DoCommand()
	{
		if (!m_bDisabled)
		{
			m_bCheck = !m_bCheck;
			m_pParent->InvalidateRect(m_rcRect);
			m_pParent->PostMessage(WM_COMMAND, m_nID);
		}
	}

	// 鼠标按下按钮事件
	void LButtonDown()
	{
		if (!m_bDisabled)
		{
			m_bPressed = TRUE;
			m_pParent->InvalidateRect(m_rcRect);
		}
	}

	// 鼠标放下按钮事件
	void LButtonUp()
	{
		if (!m_bDisabled)
		{
			if (m_bPressed == TRUE)
			{
				m_bCheck = !m_bCheck;
				m_bPressed = FALSE;
				m_pParent->InvalidateRect(m_rcRect);
				m_pParent->PostMessage(WM_COMMAND, m_nID);
			}
		}
	}

	// 鼠标进入按钮区域事件
	void MouseHover()
	{
		if (!m_bDisabled)
		{
			m_bHovering = TRUE;
			m_pParent->InvalidateRect(m_rcRect);
		}
	}

	// 鼠标离开按钮区域事件
	void MouseLeave()
	{
		if (!m_bDisabled)
		{
			m_bPressed = FALSE;
			m_bHovering = FALSE;
			m_pParent->InvalidateRect(m_rcRect);
		}
	}

private:
	// 按钮矩形位置
	CRect	m_rcRect;

	// 标识鼠标进入按钮区域
	BOOL	m_bHovering;

	// 标识已经点击按钮
	BOOL	m_bPressed;

	// 标识按钮已经被禁用
	BOOL	m_bDisabled;

	// 标识按钮已经被选中
	BOOL	m_bCheck;

	// 按钮图片编号
	UINT	m_nImageContorl;

	// 按钮按下状态图片编号
	UINT	m_nAltImageContorl;

	// 按钮图片中包含的图片个数
	UINT	m_nImageCount;

	// 按钮选中图片中包含的图片个数
	UINT	m_nAltImageCount;

	// 按钮Id号
	int		m_nID;

	// 按钮父窗口
	CWnd*	m_pParent;
};


class CViewFrame : public CDialog
{
public:
	// 标准构造析构函数
	CViewFrame();
	virtual ~CViewFrame();

	//上一页与下一页
	void OnPrevious(), OnNextPage();
	int  m_Page; //当前页


	// 判断游戏类型
	void OnSwitchType();

	// 游戏操作
	void OnAddGame();
	void OnModify();
	void OnDelete();
	void OnStart();

protected:
	
	// 图片切换所需资源信息
	struct ButtonInfo
	{
		CString sName;
		UINT nIcon; 
		UINT nBack;
	};

	DECLARE_MESSAGE_MAP()

	// WM_CREATE消息处理函数
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);

	// WM_PAINT消息处理函数
	afx_msg void OnPaint();

	// WM_ERASEBKGND消息处理函数
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// 窗口关闭按钮事件
	afx_msg void OnBtnClose();

	// WM_TIMER消息处理函数
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// 在窗口销毁后调用
	virtual void PostNcDestroy();

	// WM_MOUSEMOVE消息处理函数
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// WM_LBUTTONDOWN消息处理函数
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	// WM_LBUTTONUP消息处理函数
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	// WM_LBUTTONDBLCLK消息处理函数
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	// 鼠标离开窗口消息通知
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

	// 鼠标进入窗口消息通知
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;

	// 添加图片页面函数
	int		AddButton(LPCTSTR szName, UINT nIcon, UINT nBack );

	// 获取指定切换按钮所在位置
	BOOL	GetButtonRect(int iButton, CRect& rtButton);

	// 测试鼠标所在切换按钮位置
	int		HitTest(CPoint point, CRect& rtButton);

	// 测试鼠标所在其它按钮位置
	int		XButtonHitTest(CPoint point, CRect& rtButton);

public:
	// 指示鼠标已经进行窗口
	BOOL	m_bHovering;

	// 指示已经开始进行鼠标进入离开窗口事件跟踪
	BOOL	m_bTracking;

	// 当前鼠标所在按钮
	int		m_iHovering;

	// 当前选定的按钮
	int		m_iSelected;

	// 当前窗口图片是否自动移动
	BOOL	m_bAutoMove;

	// 当前鼠标是否已经按下
	BOOL	m_bMouseDown;

	// 当前鼠标按下位置
	int		m_iMouseDownX;

	// 当前鼠标按下后移动宽度
	int		m_iMouseMoveWidth;

	// 当前窗口图片每秒自动移动的宽度
	int		m_iMoveWidthPerSecond;

	// 主窗口图片切换按钮列表
	vector<ButtonInfo>    m_ButtonList;

	// 当前鼠标所在其它按钮编号
	int					   m_iXButtonHovering;

	// 其它按钮所在位置
	vector<CRect>		   m_XButtonRectList;

	// 其它按钮列表
	vector<CXButton>	   m_XButtonList;


	// 游戏列表位置
	vector<CRect>	       m_GameRectList;


	// 游戏列表所在位置
	CRect rtGameList;

	// 游戏列表控制程序 
	CListControl * m_ListControl;
};
