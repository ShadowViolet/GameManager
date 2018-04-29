#pragma once
#include <vector>

using namespace  std;

#define						MSG_SKIN						1001
#define						MSG_SELECT_SKIN					1002

struct SkinInfo
{
	int nType;
	Color clr;
	UINT uIndex;
};

class CSelectBox : public CControlBase
{
public:
	CSelectBox(HWND hWnd, CMessageInterface* pMessage, UINT uControlID, CRect rc, int nXCount, int nYCount, BOOL bImage,  
		Color clrFrame = Color(254, 0, 0, 0), Color clrHover = Color(128, 0, 0, 0), Color clrSelect = Color(254, 255, 255, 255), BOOL bIsVisible = TRUE);
	~CSelectBox(void);

	BOOL SetBitmap(UINT nResourceID, unsigned nIndex = -1, CString strType = TEXT("PNG"));
	BOOL SetBitmap(CString strImage, unsigned nIndex = -1);
	BOOL SetColor(Color clr, unsigned nIndex = -1);
	BOOL SetColor(Color clr[], unsigned nColorCount);

protected:
	void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);

	BOOL m_bImage;
	vector<Image *> m_vecpImage;
	vector<CSize>  m_vecsizeImage;
	vector<Color>  m_vecclr;
	unsigned m_nXCount;
	unsigned m_nYCount;
	Color m_clrFrame;
	Color m_clrHover;
	Color m_clrSelect;

	unsigned m_nXHover;
	unsigned m_nYHover;
	unsigned m_nXSelect;
	unsigned m_nYSelect;
};
