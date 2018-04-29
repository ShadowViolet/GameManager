#pragma once

// ����ͼƬ
BOOL LoadImage(const CString strPathFile, CBitmap &bitmap, CSize &size);
// ��ȡͼƬ
BOOL LoadImage(UINT nID, CBitmap &bitmap, CSize &size, CString strType);
// ����ͼƬ
BOOL ImageFromIDResource(UINT nID, CString strType, Image * & pImg);
// ȡ��ͼƬƽ����ɫ
BOOL GetAverageColor(CDC *pDC, CBitmap &bitmap, const CSize &sizeImage, COLORREF &clrImage);
// ȡ��ͼƬ��С
BOOL GetSize(CBitmap &bitmap, CSize &size);
// ȡ�������С
Size GetTextBounds(const Font& font,const StringFormat& strFormat,const CString& strText);
// ȡ�������С
Size GetTextBounds(const Font& font,const CString& strText);
// ȡ��λ��
CPoint GetOriginPoint(int nWidth, int nHeight, int nChildWidth, int nChildHeight, UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER);
// ȡ��λ��
CPoint GetOriginPoint(CRect rc, int nChildWidth, int nChildHeight, UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER);
// ת������
CString DecimalFormat(int nNumber);


// �滭����

// ����ֱ����
int DrawVerticalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent = 0, int nEndTransparent = 100);
// ��ˮƽ����
int DrawHorizontalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent = 0, int nEndTransparent = 100);
// �����½ǹ���
void DrawRightBottomTransition(CDC &dc, CDC &dcTemp, CRect rc, const int nOverRegio, const COLORREF clrBackground);
// ��ͼƬ�߿�
void DrawImageFrame(Graphics &graphics, Image *pIamge, const CRect &rcControl, int nX, int nY, int nW, int nH, int nFrameSide = 4);
// ������Բ�Ǿ���
void DrawRectangle(CDC &dcDes, const CRect &rcDes, BOOL bUp = TRUE, int nBeginTransparent = 60, int nEndTransparent = 90);
