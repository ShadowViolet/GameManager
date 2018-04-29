#pragma once
#include <vector>

using namespace  std;

#define						MSG_CLOSE						1
#define						MSG_USER						2

enum enumBackMode
{
	enBMImage = 0,			// ����
	enBMFrame				// �߿�
};

// CDlgPopup

class CDlgPopup : public CWnd, public CTimer, public CMessageInterface
{
	BOOL			m_bInit;
	BOOL			m_bTracking;
	BOOL			m_bIsLButtonDown;	
	BOOL			m_bIsLButtonDblClk;
	BOOL			m_bIsSetCapture;

protected:
	Image*			m_pImage;
	CSize			m_sizeBKImage;
	CPoint			m_point;
	CSize			m_size;
	UINT			m_uMessageID;

	enumBackMode	m_enBackMode;
	int				m_nFrameSize;
	BOOL			m_bInitFinish;
	BOOL			m_bAutoClose;

	BOOL			m_bNCActive;
	vector<CControlBase *>	m_vecControl;
	CControlBase *m_pControl;
	CControlBase *m_pFocusControl;

	UINT			m_uTimerAnimation;			// ������ʱ��

	DECLARE_DYNAMIC(CDlgPopup)

public:
	CDlgPopup();
	virtual ~CDlgPopup();

	void SetBackMode(enumBackMode enBackMode, int nFrameSize = 4);
	void SetBackBitmap(UINT nResourceID, int nFrameSize = 4);
	void SetBackBitmap(CString strImage, int nFrameSize = 4);
	BOOL Create(CWnd *pParent, CRect rc, UINT uMessageID, UINT nResourceID, enumBackMode enBackMode = enBMFrame, int nFrameSize = 4);
	BOOL Create(CWnd *pParent, CRect rc, UINT uMessageID, CString strImage, enumBackMode enBackMode = enBMFrame, int nFrameSize = 4);
	void CloseWindow();

	void DrawWindow();	
	void DrawWindow(CDC &dc);
	void DrawWindow(CDC *pDC);
	virtual void DrawWindow(CDC &dc, CRect rcClient){};
	virtual void DrawWindowEx(CDC &dc, CRect rcClient){};
	virtual void InitUI(CRect rcClient) = 0;
	virtual BOOL OnMouseMove(CPoint point){ return false;};
	virtual BOOL OnLButtonDown(CPoint point){ return false;};
	virtual BOOL OnLButtonUp(CPoint point){ return false;};

	// ��ʱ����Ϣ
	virtual void OnTimer(UINT uTimerID);

	// ��Ϣ��Ӧ
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	// ����֪ͨ��Ϣ
	BOOL SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam);
	BOOL PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam);

	CControlBase *GetControl(UINT uControlID);

	// �ƶ��ؼ�
	virtual CControlBase * SetControlRect(UINT uControlID, CRect rc);
	// �ƶ��ؼ�
	virtual CControlBase * SetControlRect(CControlBase *pControlBase, CRect rc);
	// ��ʾ�ؼ�
	virtual CControlBase * SetControlVisible(UINT uControlID, BOOL bVisible);
	// ��ʾ�ؼ�
	virtual CControlBase * SetControlVisible(CControlBase *pControlBase, BOOL bVisible);
	// ���ÿؼ�
	virtual CControlBase * SetControlDisable(UINT uControlID, BOOL bDisable);
	// ���ÿؼ�
	virtual CControlBase * SetControlDisable(CControlBase *pControlBase, BOOL bDisable);
	// ����ѡ��
	void UpdateHover();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
};
