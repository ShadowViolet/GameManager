#pragma once

class CDlgSkin : public CDlgPopup
{
	BOOL m_bImage;
public:
	CDlgSkin();
	~CDlgSkin();

	virtual void DrawWindowEx(CDC &dc, CRect rcClient);
	virtual void InitUI(CRect rcClient);

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);
};
