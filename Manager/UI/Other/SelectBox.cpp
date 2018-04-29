#include "StdAfx.h"
#include "SelectBox.h"

CSelectBox::CSelectBox(HWND hWnd, CMessageInterface* pMessage, UINT uControlID, CRect rc, int nXCount, int nYCount, BOOL bImage,
					   Color clrFrame/* = Color(254, 0, 0, 0)*/, Color clrHover/* = Color(64, 128, 128, 128)*/, Color clrSelect/* = Color(254, 255, 255, 255)*/, BOOL bIsVisible/* = TRUE*/)
			: CControlBase(hWnd, pMessage, uControlID, rc, bIsVisible)
{
	m_nXCount = nXCount;
	m_nYCount = nYCount;
	m_clrFrame = clrFrame;
	m_clrHover = clrHover;
	m_clrSelect = clrSelect;

	m_nXHover = -1;
	m_nYHover = -1;
	m_nXSelect = -1;
	m_nYSelect = -1;

	m_bImage = bImage;
}

CSelectBox::~CSelectBox(void)
{
}

BOOL CSelectBox::SetBitmap(UINT nResourceID, unsigned nIndex, CString strType)
{
	if(nIndex >= 0 && nIndex < m_vecpImage.size())
	{
		Image *&pImage = m_vecpImage[nIndex];
		if(pImage != NULL)
		{
			delete pImage;
			pImage = NULL;
		}

		if(ImageFromIDResource(nResourceID, strType, pImage))
		{
			m_vecsizeImage[nIndex].SetSize(pImage->GetWidth(), pImage->GetHeight());
			UpdateControl(true);
			return true;
		}
	}
	else if(nIndex == -1 && m_vecpImage.size() < m_nXCount * m_nYCount)
	{
		Image *pImage = NULL;
		if(ImageFromIDResource(nResourceID, strType, pImage))
		{
			CSize sizeImage(pImage->GetWidth(), pImage->GetHeight());
			
			m_vecpImage.push_back(pImage);
			m_vecsizeImage.push_back(sizeImage);

			UpdateControl(true);
			return true;
		}
	}

	return false;
}

BOOL CSelectBox::SetBitmap(CString strImage, unsigned nIndex)
{
	if(nIndex >= 0 && nIndex < m_vecpImage.size())
	{
		Image *&pImage = m_vecpImage[nIndex];
		if(pImage != NULL)
		{
			delete pImage;
			pImage = NULL;
		}

		pImage = Image::FromFile(strImage, TRUE);

		if(pImage->GetLastStatus() == Ok)
		{
			m_vecsizeImage[nIndex].SetSize(pImage->GetWidth(), pImage->GetHeight());
			UpdateControl(true);
			return true;
		}
	}
	else if(nIndex < -1 && m_vecpImage.size() < m_nXCount * m_nYCount)
	{
		Image *pImage = NULL;
		pImage = Image::FromFile(strImage, TRUE);

		if(pImage->GetLastStatus() == Ok)
		{
			CSize sizeImage(pImage->GetWidth(), pImage->GetHeight());

			m_vecpImage.push_back(pImage);
			m_vecsizeImage.push_back(sizeImage);

			UpdateControl(true);
			return true;
		}
	}

	return false;
}

BOOL CSelectBox::SetColor(Color clr, unsigned nIndex/* = -1*/)
{
	if(nIndex >= 0 && nIndex < m_vecclr.size())
	{
		m_vecclr[nIndex] = clr;
		UpdateControl(true);
		return true;
	}
	else if(nIndex < -1 && m_vecclr.size() < m_nXCount * m_nYCount)
	{
		m_vecclr.push_back(clr);
		UpdateControl(true);
		return true;
	}

	return false;
}

BOOL CSelectBox::SetColor(Color clr[], unsigned nColorCount)
{
	m_vecclr.clear();
	for (unsigned i = 0; i < nColorCount; i++)
	{
		if(i >= m_nXCount * m_nYCount) break;

		m_vecclr.push_back(clr[i]);
	}
	
	m_bUpdate = false;
	return true;
}

void CSelectBox::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	int nItemWidth = nWidth / m_nXCount;
	int nItemHeight = nHeight / m_nYCount;
	int nXPos = (nWidth - nItemWidth * m_nXCount) / 2;		
	int nYPos = (nHeight - nItemHeight * m_nYCount) / 2;

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight * 3);

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);
		
		int nXPosTemp = nXPos;
		int nYPosTemp = nYPos;

		Graphics graphics(m_memDC);
		Pen pen(m_clrFrame, 1);

		for (unsigned i = 0; i <= m_nYCount; i++)
 		{
 			graphics.DrawLine(&pen, nXPos, nYPosTemp, nXPos + nItemWidth * m_nXCount, nYPosTemp);
 			nYPosTemp += nItemHeight;
 		}
 
		for (unsigned i = 0; i <= m_nXCount; i++)
 		{
 			graphics.DrawLine(&pen, nXPosTemp, nYPos, nXPosTemp, nYPos + nItemHeight * m_nYCount);
 			nXPosTemp += nItemWidth;
 		}

		if(m_bImage)
		{
			for(size_t i = 0; i < m_vecpImage.size(); i++)
			{
				if(m_vecpImage[i] != NULL)
				{
					graphics.DrawImage(m_vecpImage[i], Rect(nXPos + nItemWidth * (i % m_nXCount) + 1, nYPos + nItemHeight * (i / m_nXCount) + 1, nItemWidth - 1, nItemHeight - 1), 
						0, 0, m_vecsizeImage[i].cx, m_vecsizeImage[i].cy, UnitPixel);
				}
			}
		}
		else
		{
			for(size_t i = 0; i < m_vecclr.size(); i++)
			{
				SolidBrush brush(m_vecclr[i]);
				graphics.FillRectangle(&brush, nXPos + nItemWidth * (i % m_nXCount) + 1, nYPos + nItemHeight * (i / m_nXCount) + 1, nItemWidth - 1, nItemHeight - 1);
			}
		}

		m_memDC.BitBlt(0, nHeight, nWidth, nHeight, &m_memDC, 0, 0, SRCCOPY);
		m_memDC.BitBlt(0, nHeight * 2, nWidth, nHeight, &m_memDC, 0, 0, SRCCOPY);

		//ѡ��
		SolidBrush brush(m_clrHover);
		nYPosTemp = nYPos + nHeight;
		if(m_bImage)
		{
			for(size_t i = 0; i < m_vecpImage.size(); i++)
			{			
				graphics.FillRectangle(&brush, nXPos + nItemWidth * (i % m_nXCount) + 1, nYPosTemp + nItemHeight * (i / m_nXCount) + 1, nItemWidth - 1, nItemHeight - 1);
			}
		}
		else
		{
			for(size_t i = 0; i < m_vecclr.size(); i++)
			{			
				graphics.FillRectangle(&brush, nXPos + nItemWidth * (i % m_nXCount) + 1, nYPosTemp + nItemHeight * (i / m_nXCount) + 1, nItemWidth - 1, nItemHeight - 1);
			}
		}

		int nLineWidth = m_bImage ? 2 : 1;
		//ѡ��
		pen.SetColor(m_clrSelect);
		pen.SetWidth((Gdiplus::REAL)nLineWidth);

		nYPosTemp = nYPos + nHeight * 2;
		for(unsigned i = 0; i < m_nYCount; i++)
		{
			nXPosTemp = nXPos;
			for (unsigned j = 0; j < m_nXCount; j++)
			{				
				Rect rect(nXPosTemp + nLineWidth, nYPosTemp + nLineWidth, nItemWidth - 1 - nLineWidth, nItemHeight - 1 - nLineWidth);

				//���ƾ���
				graphics.DrawRectangles(&pen, &rect, 1);
				nXPosTemp += nItemWidth;
			}
			nYPosTemp += nItemHeight;
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);

	if(m_nXSelect != -1 && m_nYSelect != -1)
	{
		dc.BitBlt(m_rc.left + nXPos + m_nXSelect * nItemWidth, m_rc.top + nYPos + m_nYSelect * nItemHeight, nItemWidth, nItemHeight, &m_memDC, m_nXSelect * nItemWidth, nHeight * 2 + m_nYSelect * nItemHeight, SRCCOPY);
	}

	if(m_nXHover != -1 && m_nYHover != -1)
	{
		dc.BitBlt(m_rc.left + nXPos + m_nXHover * nItemWidth, m_rc.top + nYPos + m_nYHover * nItemHeight, nItemWidth, nItemHeight, &m_memDC, m_nXHover * nItemWidth, nHeight + m_nYHover * nItemHeight, SRCCOPY);
	}
}

BOOL CSelectBox::OnControlMouseMove(UINT nFlags, CPoint point)
{
	int nOldXHover = m_nXHover;
	int nOldYHover = m_nYHover;

	if (!m_bIsDisable)
	{
		int nWidth = m_rc.Width();
		int nHeight = m_rc.Height();
		int nItemWidth = nWidth / m_nXCount;
		int nItemHeight = nHeight / m_nYCount;
		int nXPos = (nWidth - nItemWidth * m_nXCount) / 2;		
		int nYPos = (nHeight - nItemHeight * m_nYCount) / 2;
		
		CRect rc = m_rc;
		rc.left += nXPos;
		rc.top += nYPos;
		rc.right = rc.left + nItemWidth * m_nXCount;
		rc.bottom = rc.top + nItemHeight * m_nYCount;

		if(rc.PtInRect(point))
		{
			m_nXHover = (point.x - rc.left) / nItemWidth;
			m_nYHover = (point.y - rc.top) / nItemHeight;

			if(m_nXHover == m_nXSelect && m_nYHover == m_nYSelect)
			{
				m_nXHover = -1;
				m_nYHover = -1;
			}
		}
		else
		{
			m_nXHover = -1;
			m_nYHover = -1;
		}
	}	

	if(nOldXHover != m_nXHover || nOldYHover != m_nYHover)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CSelectBox::OnControlLButtonDown(UINT nFlags, CPoint point)
{	
	int nOldXHover = m_nXHover;
	int nOldYHover = m_nYHover;
	int nOldXSelect = m_nXSelect;
	int nOldYSelect = m_nYSelect;

	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_nXHover != -1 && m_nYHover != -1)
			{
				m_nXSelect = m_nXHover;
				m_nYSelect = m_nYHover;
				m_nXHover = -1;
				m_nYHover = -1;
			}

			if(nOldXSelect != m_nXSelect || m_nYSelect != nOldYSelect)
			{
				SkinInfo skinInfo;
				if(m_bImage)
				{
					skinInfo.nType = 0;
					skinInfo.uIndex = m_nXCount * m_nYSelect + m_nXSelect;
				}
				else
				{
					skinInfo.nType = 1;
					skinInfo.clr = m_vecclr[m_nXCount * m_nYSelect + m_nXSelect];
				}
				SendMessage(BUTTOM_DOWN, 0,(LPARAM)&skinInfo);
			}
		}
	}

	if(nOldXHover != m_nXHover || nOldYHover != m_nYHover || nOldXSelect != m_nXSelect ||  nOldYSelect != m_nYSelect)
	{
		UpdateControl();
		return true;
	}
	return false;
}
