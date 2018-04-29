#pragma once

#include "ViewFrame.h"

// CMainWnd 对话框

class CMainWnd : public CDlgBase
{
	DECLARE_DYNAMIC(CMainWnd)

public:
	CMainWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainWnd();

	// 消息响应
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);
	// 换肤
	afx_msg LRESULT OnMessageSkin(WPARAM wParam, LPARAM lParam);
	// 主菜单
	afx_msg LRESULT OnMessageMainMenu(WPARAM wParam, LPARAM lParam);

	// 子窗口消息
	afx_msg LRESULT OnMessageChild(WPARAM wParam, LPARAM lParam);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
