#pragma once

#include <vector>
using namespace std;

struct tagMenuInfo
{
	UINT		uMenuID;			// �˵�ID
	BOOL		bSelect;			// ѡ��
	BOOL		bDown;				// ����
};

class CMenuEx :  public CDlgPopup
{
	CString	m_strTitle;				// ����
	CString m_strFont;
	int m_nFontWidth;
	FontStyle m_fontStyle;
	UINT m_uAlignment;
	UINT m_uVAlignment;

	int m_nLeft;				// ��߼��
	int m_nHeight;				// �˵���Ĭ�ϸ߶�
	int m_nWidth;				// �˵��������
	int m_nSeparatorHeight;		// �ָ��߸߶�
public:
	CMenuEx(CString strFont = TEXT("����"), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular);
	~CMenuEx(void);

	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID, UINT nResourceID, int nFrameSize = 4, int nMinWidth = 113, enumBackMode enBackMode = enBMFrame);
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID, CString strImage, int nFrameSize = 4, int nMinWidth = 113, enumBackMode enBackMode = enBMFrame);

	int AddMenu(CString strText, UINT uMenuID, int nResourceID = -1, BOOL bSelect = false, unsigned nIndex = -1);
	int AddSeparator(unsigned nIndex = -1);

	// ���ò˵���λ��
	void SetMenuPoint();

	virtual void DrawWindowEx(CDC &dc, CRect rcClient);
	virtual void InitUI(CRect rcClient);

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);
};
