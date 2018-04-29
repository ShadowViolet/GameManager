#pragma once

#include "ViewFrame.h"

// CMainWnd �Ի���

class CMainWnd : public CDlgBase
{
	DECLARE_DYNAMIC(CMainWnd)

public:
	CMainWnd(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMainWnd();

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);
	// ����
	afx_msg LRESULT OnMessageSkin(WPARAM wParam, LPARAM lParam);
	// ���˵�
	afx_msg LRESULT OnMessageMainMenu(WPARAM wParam, LPARAM lParam);

	// �Ӵ�����Ϣ
	afx_msg LRESULT OnMessageChild(WPARAM wParam, LPARAM lParam);

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CWndShadow   m_WndShadow;
	CViewFrame * m_ViewFrame;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
