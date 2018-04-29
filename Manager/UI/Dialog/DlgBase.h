#ifndef __DLG_MY_BASE_X_H__
#define __DLG_MY_BASE_X_H__
#include <vector>


using namespace  std;

class  CDlgBase : public CDialog, public CTimer, public CMessageInterface
{
	DECLARE_DYNAMIC(CDlgBase)

public:
	CString			m_strPath;
	BOOL			m_bInit;

protected:
	UINT			m_uTimerAnimation;			// ������ʱ��

	BOOL			m_bChangeSize;				// �ı䴰�ڴ�С
	CSize			m_MinSize;					// �����޶���С��С
	CRgn			m_Rgn;						// �����򴰿�����
	COLORREF		m_clrBK;					// �Զ���ǰ����ɫ

	CFont			m_TitleFont;				// ���Ʊ�����������
	HICON			m_hIcon;		

	CBitmap			m_BKImage;					// ��ܱ���ͼƬ
	CSize			m_sizeBKImage;
	CDC				m_MemBKDC;					// ����dc
	CBitmap			*m_pOldMemBK;
	CBitmap			m_MemBK;
	BOOL			m_bDrawImage;				// ͼƬ��ɫ����

	int				m_nFrameTopBottomSpace;
	int				m_nFrameLeftRightSpace;

	int				m_nOverRegioX;				//���ȵĴ�С
	int				m_nOverRegioY;				//���ȵĴ�С
	BOOL			m_bNCActive;

	BOOL			m_bTracking;
	BOOL			m_bIsLButtonDown;	
	BOOL			m_bIsLButtonDblClk;
	BOOL			m_bIsSetCapture;

	vector<CControlBase *>	m_vecControl;	
	vector<CControlBase *>	m_vecArea;	
	CControlBase *m_pControl;
	CControlBase *m_pFocusControl;
private:
	vector<CControlBase *>	m_vecBaseControl;
	vector<CControlBase *>	m_vecBaseArea;

public:
	CDlgBase(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CDlgBase();

	void SetMinSize(int iWidth, int iHeight);	// ������С���ڴ�С
	CSize GetMinSize();

protected:
	// ���ò�����������
	void SetupRegion(int border_offset[], int nSize);
	void DrawImageStyle(CDC &dc, const CRect &rcClient, const CRect &rcUpdate);
	
	// ����ͼƬ
	void LoadImage(UINT nIDResource, CString strType = TEXT("PNG"));
	void LoadImage(CString strFileName);
	// ��ʼ�����ڿؼ���Ĭ��ֵ
	void InitUIState();
	// ���ò�����������
	void SetupRegion(int nSize);
	// ������ͼƬ
	void DrawBackground(CBitmap &bitBackground);
	// ������ͼƬ
	void DrawBackground(COLORREF clr);
	// ǰ��ͼƬ
	virtual void DrawControl(CDC &dc, const CRect &rcClient);	
	// ���ÿؼ�
	virtual void ResetControl();
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

	void InitBaseUI(CRect rcClient);
	void InitUI(CRect rcClient);
	void OnSize(CRect rcClient);
	CControlBase *GetControl(UINT uControlID);
	CControlBase *GetBaseControl(UINT uControlID);

	// ��ʱ����Ϣ
	virtual void OnTimer(UINT uTimerID);

	// ��Ϣ��Ӧ
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	// ���ػ��ƴ�������	
	virtual void OnBaseSize(CRect rcFrame);
	virtual void OnClose();
	virtual void OnMinimize();
	virtual BOOL OnMaximize();

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);


	LRESULT OnMessageButtomMin(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtomClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
};

#endif __DLG_MY_BASE_X_H__
