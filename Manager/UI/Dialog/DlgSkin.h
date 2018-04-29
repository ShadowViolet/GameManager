#pragma once

class CDlgSkin : public CDlgPopup
{
	BOOL m_bImage;
public:
	CDlgSkin();
	~CDlgSkin();

	virtual void DrawWindowEx(CDC &dc, CRect rcClient);
	virtual void InitUI(CRect rcClient);

	// œ˚œ¢œÏ”¶
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);
};
