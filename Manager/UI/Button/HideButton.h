#pragma once
#include "UI/Base/ControlBase.h"

class CHideButton : public CControlBaseFont
{
public:
	CHideButton(HWND hWnd, CMessageInterface* pMessage, UINT uControlID, CRect rc, CString strTip, CString strTitle, Color clrTip = Color(56, 56, 56),
		Color clrTextNormal = Color(39, 134, 230) ,Color clrTextHover = Color(93, 169, 242),
		Color clrTextDown = Color(21, 112, 235) ,Color clrTextDisable =  Color(128, 128, 128),
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	~CHideButton(void);
	void SetTitleText(CString strTitle);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	void DrawControl(CDC &dc, CRect rcUpdate);

public:
	Color m_clrTextNormal;
	Color m_clrTextHover;
	Color m_clrTextDown;
	Color m_clrTextDisable;
	Color m_clrTip;
	CRect m_rcText;
	enumButtonState m_enButtonState;
	BOOL m_bDown;
	CString m_strTip;
	BOOL m_bShowBitton;
};