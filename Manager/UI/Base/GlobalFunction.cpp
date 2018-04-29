#include "StdAfx.h"
#include <winuser.h>
#include <cmath>
#include "GlobalFunction.h"

// ��ȡͼƬ
BOOL LoadImage(const CString strPathFile, CBitmap &bitmap, CSize &size)
{	
	HBITMAP hBitmap = NULL;
	Bitmap* pBitmap = Bitmap::FromFile(strPathFile);
	Status status = pBitmap->GetLastStatus();
	if(Ok == status)
	{		
		status = pBitmap->GetHBITMAP(Color(0,0,0), &hBitmap);
		if(Ok == status)
		{
			if(bitmap.m_hObject != NULL)
			{
				bitmap.Detach();
			}
			bitmap.Attach(hBitmap);
			
			BITMAP bmInfo;
			::GetObject( bitmap.m_hObject, sizeof(BITMAP), &bmInfo );
			size.cx = bmInfo.bmWidth;
			size.cy = bmInfo.bmHeight;

			delete pBitmap;

			return true;
		}
	}

	return false;
}

// ��ȡͼƬ
BOOL LoadImage(UINT nID, CBitmap &bitmap, CSize &size, CString strType)
{
	HINSTANCE hInst = AfxGetResourceHandle();  
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID), strType);
	if (!hRsrc)  
	{
		return FALSE; 
	}

	DWORD len = SizeofResource(hInst, hRsrc);  
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
	if (!lpRsrc)  
	{
		return FALSE;
	}
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);  
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
	memcpy(pmem,lpRsrc,len);  
	IStream* pstm;  
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);  
	Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pstm);  
	GlobalUnlock(m_hMem);  
	GlobalFree(m_hMem);
	pstm->Release();  
	FreeResource(lpRsrc); 


	HBITMAP hBitmap = NULL;
	Status status = pBitmap->GetLastStatus();
	if(Ok == status)
	{		
		status = pBitmap->GetHBITMAP(Color(0,0,0), &hBitmap);
		delete pBitmap;

		if(Ok == status)
		{
			if(bitmap.m_hObject != NULL)
			{
				bitmap.Detach();
			}
			bitmap.Attach(hBitmap);

			BITMAP bmInfo;
			::GetObject( bitmap.m_hObject, sizeof(BITMAP), &bmInfo );
			size.cx = bmInfo.bmWidth;
			size.cy = bmInfo.bmHeight;				

			return true;
		}
	}

	return true; 
}

// ����ͼƬ
BOOL ImageFromIDResource(UINT nID, CString strType, Image * & pImg)  
{  
	HINSTANCE hInst = AfxGetResourceHandle();  
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),strType);
	if (!hRsrc)  
	{
		return FALSE; 
	}

	DWORD len = SizeofResource(hInst, hRsrc);  
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
	if (!lpRsrc)  
	{
		return FALSE;
	}
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);  
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
	memcpy(pmem,lpRsrc,len);  
	IStream* pstm;  
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);  
	pImg=Gdiplus::Image::FromStream(pstm);  
	GlobalUnlock(m_hMem);  
	//GlobalFree(m_hMem);
	pstm->Release();  
	FreeResource(lpRsrc);  
	return TRUE;  
} 

// ȡ��ͼƬƽ����ɫ
BOOL GetAverageColor(CDC *pDC, CBitmap &bitmap, const CSize &sizeImage, COLORREF &clrImage)
{
	BOOL bIsResult = false;

	CDC TempDC;
	TempDC.CreateCompatibleDC(pDC);
	CBitmap *pOldBitmap = TempDC.SelectObject(&bitmap);

	int nWidth = sizeImage.cx;
	int nHeight = sizeImage.cy;
	// ����λͼ��Ϣ   
	BITMAPINFO bi;  
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);  
	bi.bmiHeader.biWidth = nWidth;  
	bi.bmiHeader.biHeight = nHeight;  
	bi.bmiHeader.biPlanes = 1;  
	bi.bmiHeader.biBitCount = 32;   
	bi.bmiHeader.biCompression = BI_RGB;   
	bi.bmiHeader.biSizeImage = nWidth * nHeight * 4; // 32 bit   
	bi.bmiHeader.biClrUsed = 0;  
	bi.bmiHeader.biClrImportant = 0;  

	// ��ȡλͼ����   
	BYTE* pBits = (BYTE*)new BYTE[bi.bmiHeader.biSizeImage];  
	::ZeroMemory(pBits, bi.bmiHeader.biSizeImage);  

	int nLine = ::GetDIBits(TempDC.m_hDC, (HBITMAP)bitmap.GetSafeHandle(), 0, nHeight, pBits, &bi, DIB_RGB_COLORS);
	if (!nLine)  
	{  
		delete []pBits;  
		pBits = NULL;  
	} 
	else
	{
		long r = 0, g = 0, b = 0;

		for (LONG i = 0; i < nWidth; i++)     
		{     
			for (LONG j = 0; j < nHeight; j++)     
			{     
				b += pBits[(j * nWidth + i) * 4];					
				g += pBits[(j * nWidth + i) * 4 + 1];
				r += pBits[(j * nWidth + i) * 4 + 2];		
			}     
		}  

		delete []pBits;  
		pBits = NULL;  
		int n = nWidth * nHeight;
		clrImage = RGB(r / n, g / n, b / n);
		bIsResult = true;
	}

	TempDC.SelectObject(pOldBitmap);
	TempDC.DeleteDC();

	return bIsResult;
}

// ȡ��ͼƬ��С
BOOL GetSize(CBitmap &bitmap, CSize &size)
{
	if(bitmap.m_hObject)
	{
		BITMAP bmInfo;
		::GetObject( bitmap.m_hObject, sizeof(BITMAP), &bmInfo );
		size.cx = bmInfo.bmWidth;
		size.cy = bmInfo.bmHeight;
		return true;
	}
	return false;
}

Size GetTextBounds(const Font& font,const StringFormat& strFormat,const CString& strText)
{
	GraphicsPath path;
	FontFamily fontfamily;
	font.GetFamily(&fontfamily);
	path.AddString(strText.AllocSysString(),-1,&fontfamily,font.GetStyle(),font.GetSize(),PointF(0,0),&strFormat);
	RectF rcBound;
	path.GetBounds(&rcBound);

	REAL rHeight = font.GetHeight(0.0f);

	return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
		(int)(rHeight > (int)rHeight ? rHeight + 2 : rHeight + 1));

	//return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
	//	(int)(rcBound.Height > (int)rcBound.Height ? rcBound.Height + 2 : rcBound.Height + 1));
}

Size GetTextBounds(const Font& font,const CString& strText)
{
	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);   
	strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);

	GraphicsPath path;
	FontFamily fontfamily;
	font.GetFamily(&fontfamily);
	path.AddString(strText.AllocSysString(),-1,&fontfamily,font.GetStyle(),font.GetSize(),PointF(0,0),&strFormat);
	RectF rcBound;
	path.GetBounds(&rcBound);
	REAL rHeight = font.GetHeight(0.0f);

	return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
		(int)(rHeight > (int)rHeight ? rHeight + 2 : rHeight + 1));

	/*return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
		(int)(rcBound.Height > (int)rcBound.Height ? rcBound.Height + 2 : rcBound.Height + 1));*/
}

// ȡ��λ��
CPoint GetOriginPoint(int nWidth, int nHeight, int nChildWidth, int nChildHeight, UINT uAlignment, UINT uVAlignment)
{
	int nXPos = 0;
	int nYPos = 0;
	if(uAlignment == DT_CENTER)
	{
		nXPos = (nWidth -  nChildWidth) / 2;
	}
	else if(uAlignment == DT_RIGHT)
	{
		nXPos = nWidth - nChildWidth;
	}

	if(uVAlignment == DT_VCENTER)
	{
		nYPos = (nHeight - nChildHeight) / 2;
	}
	else if(uVAlignment == DT_BOTTOM)
	{
		nYPos = nHeight - nChildHeight;
	}

	return CPoint(nXPos, nYPos);
}

// ȡ��λ��
CPoint GetOriginPoint(CRect rc, int nChildWidth, int nChildHeight, UINT uAlignment, UINT uVAlignment)
{
	CPoint point = GetOriginPoint(rc.Width(), rc.Height(), nChildWidth, nChildHeight, uAlignment, uVAlignment);
	
	return CPoint(rc.left + point.x, rc.top + point.y);
}

// ת������
CString DecimalFormat(int nNumber)
{
	CString str;
	CString strTemp;
	int nNumberTemp = nNumber;

	if(nNumberTemp / 1000 > 0)
	{
		strTemp.Format(TEXT("%03d"), nNumberTemp % 1000);
	}
	else
	{
		strTemp.Format(TEXT("%d,"), nNumberTemp % 1000);
	}
	nNumberTemp = nNumberTemp / 1000;
	str = strTemp;

	while(nNumberTemp > 0)
	{
		if(nNumberTemp / 1000 > 0)
		{
			strTemp.Format(TEXT("%03d,"), nNumberTemp % 1000);
		}
		else
		{
			strTemp.Format(TEXT("%d,"), nNumberTemp % 1000);
		}
		nNumberTemp = nNumberTemp / 1000;
		str = strTemp + str;
	}

	return str;
}

// ����ֱ����
int DrawVerticalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent/* = 0*/, int nEndTransparent/* = 100*/)
{
	//ASSERT(rcDes.Width() == rcSrc.Width() && rcDes.Height() == rcSrc.Height());

	BOOL bIsDownTransition = TRUE;
	if (nEndTransparent <= nBeginTransparent)
	{
		bIsDownTransition = FALSE;
		int nTemp = nBeginTransparent;
		nBeginTransparent = nEndTransparent;
		nEndTransparent = nTemp;
	}

	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;

	int nWidth = rcDes.Width();
	int nHeight =  rcDes.Height();

	int nMinTransition = (int)(255 - 255.0 * nBeginTransparent / 100.0);
	int nMaxTransition = (int)(255.0 * (100 - nEndTransparent) / 100.0);
	double dTransition = (nMinTransition - nMaxTransition) / (double)nHeight;
 
	for (int i = 0; i < nHeight; i++)
	{
		blend.SourceConstantAlpha = (BYTE)(bIsDownTransition ? nMinTransition - (dTransition * i) : nMaxTransition + (dTransition * i)); 
		dcDes.AlphaBlend(rcDes.left, rcDes.top + i,nWidth , 1, &dcSrc, rcSrc.left, rcSrc.top + i, nWidth, 1, blend); 
	}

	return blend.SourceConstantAlpha;
}

// ��ˮƽ����
int DrawHorizontalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent/* = 0*/, int nEndTransparent/* = 100*/)
{
	ASSERT(rcDes.Width() == rcSrc.Width() && rcDes.Height() == rcSrc.Height());

	BOOL bIsDownTransition = TRUE;
	if (nEndTransparent <= nBeginTransparent)
	{
		bIsDownTransition = FALSE;
		int nTemp = nBeginTransparent;
		nBeginTransparent = nEndTransparent;
		nEndTransparent = nTemp;
	}

	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;

	int nWidth = rcDes.Width();
	int nHeight =  rcDes.Height();

	int nMinTransition = (int)(255 - 255.0 * nBeginTransparent / 100.0);
	int nMaxTransition = (int)(255.0 * (100 - nEndTransparent) / 100.0);
	double dTransition = (nMinTransition - nMaxTransition) / (double)nWidth;

	for (int i = 0; i < nWidth; i++)
	{
		blend.SourceConstantAlpha = (BYTE)(bIsDownTransition ? nMinTransition - (dTransition * i) : nMaxTransition + (dTransition * i)); 
		dcDes.AlphaBlend(rcDes.left + i, rcDes.top , 1, nHeight, &dcSrc, rcSrc.left + i, rcSrc.top, 1, nHeight, blend); 
	}

	return blend.SourceConstantAlpha;
}

// �����½ǹ���
void DrawRightBottomTransition(CDC &dc, CDC &dcTemp, CRect rc, const int nOverRegio, const COLORREF clrBackground)
{
	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp= AC_SRC_OVER;
	blend.SourceConstantAlpha = 255; 

	int nStartX = rc.Width() - nOverRegio;
	int nStartY = rc.Height() - nOverRegio;
	double dRadius = (double)sqrt((double)(nOverRegio * nOverRegio + nOverRegio * nOverRegio));
	double dRadiusTemp = 0.0;

	for (int i = 0; i < nOverRegio; i++)
	{
		for (int j = 0; j < nOverRegio; j++)
		{
			dRadiusTemp  = (double)sqrt((double)(i * i + j * j));
			if (dRadiusTemp > nOverRegio)
			{
				dRadiusTemp = nOverRegio;
			}
			blend.SourceConstantAlpha = (BYTE)(255 - (255.0 / nOverRegio * (dRadiusTemp)));
			dc.AlphaBlend(nStartX + i, nStartY + j, 1, 1, &dcTemp, nStartX + i, nStartY + j, 1, 1, blend);
		} 	
	}
}

// ��ͼƬ�߿�
void DrawImageFrame(Graphics &graphics, Image *pIamge, const CRect &rcControl, int nX, int nY, int nW, int nH, int nFrameSide/* = 4*/)
{
	// ���Ͻ�
	graphics.DrawImage(pIamge, Rect(rcControl.left ,rcControl.top, nFrameSide, nFrameSide),
		nX, nY, nFrameSide, nFrameSide, UnitPixel);

	// ���б߿�
	graphics.DrawImage(pIamge, Rect(rcControl.left ,rcControl.top + nFrameSide, nFrameSide, rcControl.Height() - 2 * nFrameSide),
		nX, nY + nFrameSide, nFrameSide, nH - 2 * nFrameSide, UnitPixel);

	// ���½�
	graphics.DrawImage(pIamge, Rect(rcControl.left ,rcControl.bottom - nFrameSide, nFrameSide, nFrameSide), 
		nX, nY + nH - nFrameSide, nFrameSide, nFrameSide, UnitPixel);

	// ���б߿�
	graphics.DrawImage(pIamge, Rect(rcControl.left + nFrameSide ,rcControl.top, rcControl.Width() - 2 * nFrameSide, nFrameSide),
		nX + nFrameSide, nY, nW - 2 * nFrameSide, nFrameSide, UnitPixel);	

	// ���Ͻ�
	graphics.DrawImage(pIamge, Rect(rcControl.right - nFrameSide ,rcControl.top, nFrameSide, nFrameSide), 
		nX + nW - nFrameSide, nY, nFrameSide, nFrameSide, UnitPixel);

	// ���б߿�
	graphics.DrawImage(pIamge, Rect(rcControl.right - nFrameSide ,rcControl.top + nFrameSide, nFrameSide, rcControl.Height() - 2 * nFrameSide), 
		nX + nW - nFrameSide, nY + nFrameSide, nFrameSide, nH - 2 * nFrameSide, UnitPixel);	

	// ���½�
	graphics.DrawImage(pIamge, Rect(rcControl.right - nFrameSide ,rcControl.bottom - nFrameSide, nFrameSide, nFrameSide), 
		nX + nW - nFrameSide, nY + nH - nFrameSide, nFrameSide, nFrameSide, UnitPixel);		

	 // ���б߿�
	graphics.DrawImage(pIamge, Rect(rcControl.left + nFrameSide ,rcControl.bottom - nFrameSide, rcControl.Width() - 2 * nFrameSide, nFrameSide), 
		nX + nFrameSide, nY + nH - nFrameSide, nW - 2 * nFrameSide, nFrameSide, UnitPixel);	

	// �м�
	graphics.DrawImage(pIamge, Rect(rcControl.left + nFrameSide ,rcControl.top + nFrameSide, rcControl.Width() - 2 * nFrameSide, rcControl.Height() - 2 * nFrameSide), 
		nX + nFrameSide, nY + nFrameSide, nW - 2 * nFrameSide, nH - 2 *  nFrameSide, UnitPixel);	

}

// ������Բ�Ǿؼ�
void DrawRectangle(CDC &dc, const CRect &rc, BOOL bUp/* = TRUE*/, int nBeginTransparent/* = 50*/, int nEndTransparent/* = 90*/)
{
	int nWidth = rc.Width();
	int nHeight = rc.Height();
	int nDesStartX = rc.left;
	int nDesStartY = rc.top;	

	CDC TempDC;
	TempDC.CreateCompatibleDC(&dc);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, nWidth, nHeight);
	CBitmap *pOldmap =  TempDC.SelectObject(&memBmp);

	TempDC.FillSolidRect(0, 0, nWidth, nHeight, RGB(0, 0, 0));
	TempDC.FillSolidRect(1, 1, nWidth - 2, nHeight - 2, RGB(255, 255, 255));

	// �м�
	CRect rcSrc;
	CRect rcDes;
	if(bUp)
	{
		rcSrc.SetRect(1, 1, nWidth - 1, nHeight / 2);
		rcDes.SetRect(nDesStartX + 1, nDesStartY + 1, nDesStartX + nWidth - 1, nDesStartY + nHeight / 2);
	}
	else
	{
		rcSrc.SetRect(1, nHeight / 2, nWidth - 1, nHeight - 1);
		rcDes.SetRect(nDesStartX + 1, nDesStartY + nHeight / 2, nDesStartX + nWidth - 1, nDesStartY + nHeight - 1);		
	}

	DrawVerticalTransition(dc, TempDC, rcDes, rcSrc, nBeginTransparent, nEndTransparent);

	// �߿�
	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp= AC_SRC_OVER;
	blend.SourceConstantAlpha = 100;
	// ��
	dc.AlphaBlend(nDesStartX, nDesStartY + 1, 2, nHeight - 2, &TempDC, 0, 1, 2, nHeight - 2, blend);
	// ��
	dc.AlphaBlend(nDesStartX + 1, nDesStartY, nWidth - 2, 2, &TempDC, 1, 0, nWidth - 2, 2, blend);
	// ��
	dc.AlphaBlend(nDesStartX + nWidth - 2, nDesStartY + 1, 2, nHeight - 2, &TempDC, nWidth - 2, 1, 2, nHeight - 2, blend);
	// ��
	dc.AlphaBlend(nDesStartX + 1, rc.bottom - 2, nWidth - 2, 2, &TempDC, 1, nHeight - 2, nWidth - 2, 2, blend);

	blend.SourceConstantAlpha = 12;
	// ����
	dc.AlphaBlend(nDesStartX, nDesStartY, 2, 2, &TempDC, 0, 0, 1, 1, blend);
	// ����
	dc.AlphaBlend(nDesStartX + nWidth, nDesStartY, 2, 2, &TempDC, 0, 0, 1, 1, blend);
	// ����
	dc.AlphaBlend(nDesStartX + nWidth, nDesStartY + nHeight, 2, 2, &TempDC, 0, 0, 1, 1, blend);
	// ����
	dc.AlphaBlend(nDesStartX , nDesStartY + nHeight, 2, 2, &TempDC, 0, 0, 1, 1, blend);

	TempDC.SelectObject(pOldmap);
	memBmp.DeleteObject();
	TempDC.DeleteDC();
}
