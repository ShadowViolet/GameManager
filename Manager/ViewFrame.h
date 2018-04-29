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

	// �ͷ����н�����Դ
	void	Release();

	// �ͷŽ�����ԴId���ؽ�����Դ
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
	// ��׼���캯��
	CXButton()
	{
		m_bCheck = FALSE;
		m_bHovering = FALSE;
		m_bPressed = FALSE;
		m_bDisabled = FALSE;
		m_nImageContorl = 0;
		m_nAltImageContorl = 0;
	}

	// ���ð�ť����λ��
	void SetRect(LPCRECT lpRect)
	{ 
		m_rcRect = lpRect;
	}

	// ��ȡ��ť����λ��
	void GetRect(LPRECT lpRect)
	{ 
		*lpRect = *m_rcRect;
	}

	// ���ָ�����Ƿ��ڰ�ť��
	BOOL PtInButton(CPoint& point)
	{ 
		return m_rcRect.PtInRect(point); 
	}

	// ���ư�ť
	void DrawButton(Gdiplus::Graphics&  graphics);

	// ������ť
	void CreateButton(CWnd* pParent, LPRECT lpRect, UINT nImage, UINT nCount = 4, UINT sAltImage = 0, UINT nAltCount = 0, UINT nID = 0);

	// ���ð�ť״̬
	void EnableButton(BOOL bEnable)
	{
		m_bDisabled = !bEnable;
		m_pParent->InvalidateRect(m_rcRect);
	}

	// ���ð�ťѡ��״̬
	void SetCheck(BOOL bCheck)
	{
		m_bCheck = bCheck;
		m_pParent->InvalidateRect(m_rcRect);
	}

	// ��ȡ��ťId
	UINT GetControlId()
	{ 
		return m_nID; 
	}

	// ������ť�¼�
	void DoCommand()
	{
		if (!m_bDisabled)
		{
			m_bCheck = !m_bCheck;
			m_pParent->InvalidateRect(m_rcRect);
			m_pParent->PostMessage(WM_COMMAND, m_nID);
		}
	}

	// ��갴�°�ť�¼�
	void LButtonDown()
	{
		if (!m_bDisabled)
		{
			m_bPressed = TRUE;
			m_pParent->InvalidateRect(m_rcRect);
		}
	}

	// �����°�ť�¼�
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

	// �����밴ť�����¼�
	void MouseHover()
	{
		if (!m_bDisabled)
		{
			m_bHovering = TRUE;
			m_pParent->InvalidateRect(m_rcRect);
		}
	}

	// ����뿪��ť�����¼�
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
	// ��ť����λ��
	CRect	m_rcRect;

	// ��ʶ�����밴ť����
	BOOL	m_bHovering;

	// ��ʶ�Ѿ������ť
	BOOL	m_bPressed;

	// ��ʶ��ť�Ѿ�������
	BOOL	m_bDisabled;

	// ��ʶ��ť�Ѿ���ѡ��
	BOOL	m_bCheck;

	// ��ťͼƬ���
	UINT	m_nImageContorl;

	// ��ť����״̬ͼƬ���
	UINT	m_nAltImageContorl;

	// ��ťͼƬ�а�����ͼƬ����
	UINT	m_nImageCount;

	// ��ťѡ��ͼƬ�а�����ͼƬ����
	UINT	m_nAltImageCount;

	// ��ťId��
	int		m_nID;

	// ��ť������
	CWnd*	m_pParent;
};


class CViewFrame : public CDialog
{
public:
	// ��׼������������
	CViewFrame();
	virtual ~CViewFrame();

	//��һҳ����һҳ
	void OnPrevious(), OnNextPage();
	int  m_Page; //��ǰҳ


	// �ж���Ϸ����
	void OnSwitchType();

	// ��Ϸ����
	void OnAddGame();
	void OnModify();
	void OnDelete();
	void OnStart();

protected:
	
	// ͼƬ�л�������Դ��Ϣ
	struct ButtonInfo
	{
		CString sName;
		UINT nIcon; 
		UINT nBack;
	};

	DECLARE_MESSAGE_MAP()

	// WM_CREATE��Ϣ������
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);

	// WM_PAINT��Ϣ������
	afx_msg void OnPaint();

	// WM_ERASEBKGND��Ϣ������
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// ���ڹرհ�ť�¼�
	afx_msg void OnBtnClose();

	// WM_TIMER��Ϣ������
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// �ڴ������ٺ����
	virtual void PostNcDestroy();

	// WM_MOUSEMOVE��Ϣ������
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// WM_LBUTTONDOWN��Ϣ������
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	// WM_LBUTTONUP��Ϣ������
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	// WM_LBUTTONDBLCLK��Ϣ������
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	// ����뿪������Ϣ֪ͨ
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

	// �����봰����Ϣ֪ͨ
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;

	// ���ͼƬҳ�溯��
	int		AddButton(LPCTSTR szName, UINT nIcon, UINT nBack );

	// ��ȡָ���л���ť����λ��
	BOOL	GetButtonRect(int iButton, CRect& rtButton);

	// ������������л���ťλ��
	int		HitTest(CPoint point, CRect& rtButton);

	// �����������������ťλ��
	int		XButtonHitTest(CPoint point, CRect& rtButton);

public:
	// ָʾ����Ѿ����д���
	BOOL	m_bHovering;

	// ָʾ�Ѿ���ʼ�����������뿪�����¼�����
	BOOL	m_bTracking;

	// ��ǰ������ڰ�ť
	int		m_iHovering;

	// ��ǰѡ���İ�ť
	int		m_iSelected;

	// ��ǰ����ͼƬ�Ƿ��Զ��ƶ�
	BOOL	m_bAutoMove;

	// ��ǰ����Ƿ��Ѿ�����
	BOOL	m_bMouseDown;

	// ��ǰ��갴��λ��
	int		m_iMouseDownX;

	// ��ǰ��갴�º��ƶ����
	int		m_iMouseMoveWidth;

	// ��ǰ����ͼƬÿ���Զ��ƶ��Ŀ��
	int		m_iMoveWidthPerSecond;

	// ������ͼƬ�л���ť�б�
	vector<ButtonInfo>    m_ButtonList;

	// ��ǰ�������������ť���
	int					   m_iXButtonHovering;

	// ������ť����λ��
	vector<CRect>		   m_XButtonRectList;

	// ������ť�б�
	vector<CXButton>	   m_XButtonList;


	// ��Ϸ�б�λ��
	vector<CRect>	       m_GameRectList;


	// ��Ϸ�б�����λ��
	CRect rtGameList;

	// ��Ϸ�б���Ƴ��� 
	CListControl * m_ListControl;
};
