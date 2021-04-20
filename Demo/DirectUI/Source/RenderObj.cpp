/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/

#include "stdafx.h"
#include "RenderObj.h"
//
//namespace dui{
#define DPI_SCALE_EX(a, b) (b ?  GetUIEngine()->GetDPIObj()->Scale(a) : CDPI::Scale(a, 100))
#define DPI_SCALE_BACK_EX(a, b) (b ?  GetUIEngine()->GetDPIObj()->ScaleBack(a) : CDPI::ScaleBack(a, 100))
inline int MulDivEx(__in int nNumber,
	__in int nNumerator,
	__in int nDenominator)
{
	return MulDiv(nNumber, nNumerator, nDenominator);
}

ClipObj::ClipObj()
{
	hOldRgn = NULL;
	hRgn = NULL;
}

ClipObj::~ClipObj()
{
	::SelectClipRgn(hDC, hOldRgn);
	if (hOldRgn)
		::DeleteObject(hOldRgn);
	if (hRgn)
		::DeleteObject(hRgn);
}

void ClipObj::GenerateClip(HDC hDC, RECT& rcItem, ClipObj& clip)
{
	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	if (EqualRect(&rcClip, &rcItem)) return;

	if (clip.hOldRgn)
	{
		::DeleteObject(clip.hOldRgn);
		clip.hOldRgn = NULL;
	}
	if (clip.hRgn)
	{
		::DeleteObject(clip.hRgn);
		clip.hOldRgn = NULL;
	}

   
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rcItem);
	 ::SelectClipRgn(hDC, clip.hRgn);
    clip.hDC = hDC;
}


void ClipObj::GenerateClipWithAnd(HDC hDC, RECT& rcItem, ClipObj& clip)
{
	if (clip.hOldRgn)
	{
		::DeleteObject(clip.hOldRgn);
		clip.hOldRgn = NULL;
	}
	if (clip.hRgn)
	{
		::DeleteObject(clip.hRgn);
		clip.hOldRgn = NULL;
	}

	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	IntersectRect(&rcClip, &rcClip, &rcItem);
	clip.hRgn = ::CreateRectRgnIndirect(&rcClip);
	 ::SelectClipRgn(hDC, clip.hRgn);
	clip.hDC = hDC;
}

void ClipObj::GenerateClipWithPath(HDC hDC, RECT& rcItem, ClipObj& clip)
{
	if (clip.hOldRgn)
	{
		::DeleteObject(clip.hOldRgn);
		clip.hOldRgn = NULL;
	}
	if (clip.hRgn)
	{
		::DeleteObject(clip.hRgn);
		clip.hOldRgn = NULL;
	}

	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	HRGN itemRgn = ::CreateRectRgnIndirect(&rcItem);
	clip.hRgn = ::PathToRegion(hDC);
	::CombineRgn(clip.hRgn, clip.hRgn, itemRgn, RGN_AND);
	::SelectClipRgn(hDC, clip.hRgn);
	clip.hDC = hDC;
	::DeleteObject(itemRgn);
}

void ClipObj::UseOldClipBegin(HDC hDC, ClipObj& clip)
{
    ::SelectClipRgn(hDC, clip.hOldRgn);
}

void ClipObj::UseOldClipEnd(HDC hDC, ClipObj& clip)
{
    ::SelectClipRgn(hDC, clip.hRgn);
}

//////////////////////////////////////////////////////////////////////////////////////////
IRenderDC::IRenderDC()
{
	m_pDibObj = NULL;
	m_hDC = NULL;
	m_bEnableAlpha = false;
	ZeroMemory(&m_rectUpdate, sizeof(RECT));
}

IRenderDC::~IRenderDC()
{

}

void IRenderDC::SetDevice(HDC hDC)
{
	m_pDibObj = NULL;
	m_hDC = hDC;
}
void IRenderDC::SetDevice(DibObj* pDibObj)
{
	m_hDC = NULL;
	m_pDibObj = pDibObj;
}

HDC IRenderDC::GetDC()
{ 
	if (m_hDC) return m_hDC;
	else if (m_pDibObj)
		return m_pDibObj->GetSafeHdc();
	else
		return NULL;
}
DibObj* IRenderDC::GetDibObj()
{
	return m_pDibObj;
}

void IRenderDC::SetUpdateRect(const RECT& rect)
{
	m_rectUpdate = rect;
}

RECT IRenderDC::GetUpdateRect()
{
	return m_rectUpdate;
}

void IRenderDC::SetEnableAlpha(bool bEnableAlpha)
{
	m_bEnableAlpha = bEnableAlpha;
}

bool IRenderDC::GetEnableAlpha()
{
	return m_bEnableAlpha;
}

void IRenderDC::TextOut(FontObj* pFontObj,  int x, int y, LPCTSTR lpszText, int c, DWORD dwColor, DWORD* pBkColor, UITYPE_FONT nFontType)
{
	if (!pFontObj) 
		return;
	pFontObj->TextOut(this, x, y, lpszText,  c, dwColor, pBkColor, nFontType);
}

void IRenderDC::DrawText(FontObj* pFontObj, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format, UITYPE_FONTEFFECT nFontEffect, UITYPE_FONT nFontType)
{
	if (!pFontObj) 
		return;
	pFontObj->DrawText(this, rc, lpszText,  dwColor, format, nFontEffect, nFontType);
}


void IRenderDC::DrawShadowText(FontObj* pFontObj, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format, DWORD dwShadowColor, UINT uiShadowSize)
{
	if (!pFontObj)
		return;
	pFontObj->DrawShadowText(this, rc, lpszText, dwColor, format, dwShadowColor, uiShadowSize);
}

void IRenderDC::DrawRect(const RECT& rcDst, DWORD color)
{
	//if (color < 0xFF000000)
	//	color+= 0xFF000000; // 以后颜色全面支持透明度以后要把 “|| color <= 0x00FFFFFF”注销掉
	BYTE a = LOBYTE((color)>>24);
	if ((!GetEnableAlpha() && (a == 255))) // 以后颜色全面支持透明度以后要把 “|| color <= 0x00FFFFFF”注销掉
	{
		HBRUSH hbr = ::CreateSolidBrush(RGB(GetRValue(color),GetGValue(color), GetBValue(color)));
		if (!hbr) return;
		::FillRect(GetDC(), &rcDst, hbr);
		::DeleteObject(hbr);
	}
	else
	{
		
		Graphics graphics(GetDC());
		Gdiplus::SolidBrush brush(Gdiplus::Color(a,GetRValue(color),GetGValue(color), GetBValue(color))); // 红色和蓝色要相反
		graphics.FillRectangle(&brush, rcDst.left, rcDst.top, rcDst.right - rcDst.left, rcDst.bottom - rcDst.top);
	}

}

void IRenderDC::DrawRoundRect(const RECT& dstRect, DWORD fillColor, DWORD borderColor, int lineWidth, int cornerX, int cornerY)
{
	if (lineWidth == 0 && cornerX == 0 && cornerY == 0)
	{
		DrawRect(dstRect, fillColor);
		return;
	}
	int x = dstRect.left;
	int y = dstRect.top;
	int width = dstRect.right - dstRect.left;
	int height = dstRect.bottom - dstRect.top;

	if (cornerX > 0 || cornerY > 0)
	{
		//if (lineWidth > 0)
		{
			width -= 1; // 这边需要修正一下，否则感觉会超过边界
			height -= 1;

		}
		
		GraphicsPath path;
		int elWid = 2*cornerX;
		int elHei = 2*cornerY;
		path.AddArc(x,y,elWid,elHei,180,90); // 左上角圆弧
		path.AddLine(x+cornerX,y,x+width-cornerX,y); // 上边

		path.AddArc(x+width-elWid,y, elWid,elHei,270,90); // 右上角圆弧
		path.AddLine(x+width,y+cornerY, x+width,y+height-cornerY);// 右边

		path.AddArc(x+width-elWid,y+height-elHei, elWid,elHei,0,90); // 右下角圆弧
		path.AddLine(x+width-cornerX,y+height, x+cornerX,y+height); // 下边

		path.AddArc(x,y+height-elHei, elWid,elHei,90,90); 
		path.AddLine(x,y+cornerY, x, y+height-cornerY);

		Graphics graphics(GetDC());
		SmoothingMode mode = graphics.GetSmoothingMode();
		graphics.SetSmoothingMode(SmoothingModeHighQuality);

		Gdiplus::SolidBrush brush(Gdiplus::Color((LOBYTE((fillColor)>>24)),GetRValue(fillColor),GetGValue(fillColor), GetBValue(fillColor))); // 红色和蓝色要相反
		graphics.FillPath(&brush, &path);
		if (lineWidth > 0)
		{
			Pen pen(Gdiplus::Color((LOBYTE((borderColor) >> 24)), GetRValue(borderColor), GetGValue(borderColor), GetBValue(borderColor)), (REAL)lineWidth);
			graphics.DrawPath(&pen, &path);
		}
		graphics.SetSmoothingMode(mode);

	}
	else
	{
		if (lineWidth > 0)
		{
			width -= 1; // 这边需要修正一下，否则感觉会超过边界
			height -= 1;

		}
		Graphics graphics(GetDC());


		if ((LOBYTE((fillColor) >> 24)) > 0)
		{
			Gdiplus::SolidBrush brush(Gdiplus::Color((LOBYTE((fillColor) >> 24)), GetRValue(fillColor), GetGValue(fillColor), GetBValue(fillColor))); // 红色和蓝色要相反
			graphics.FillRectangle(&brush, x, y, width, height);
		}

		if (lineWidth > 0)
		{
			Pen pen(Gdiplus::Color((LOBYTE((borderColor) >> 24)), GetRValue(borderColor), GetGValue(borderColor), GetBValue(borderColor)), (REAL)lineWidth);
			graphics.DrawRectangle(&pen, x, y, width, height);
		}
	}
	
}


void IRenderDC::DrawGradientRoundRect(const RECT& dstRect, DWORD gradientColor1, DWORD gradientColor2, DWORD borderColor, int lineWidth, int cornerX, int cornerY, bool bVert)
{
	int x = dstRect.left;
	int y = dstRect.top;
	int width = dstRect.right - dstRect.left;
	int height = dstRect.bottom - dstRect.top;

	if (cornerX > 0 || cornerY > 0)
	{
		//if (lineWidth > 0)
		{
			width -= 1; // 这边需要修正一下，否则感觉会超过边界
			height -= 1;

		}

		GraphicsPath path;
		int elWid = 2*cornerX;
		int elHei = 2*cornerY;
		path.AddArc(x,y,elWid,elHei,180,90); // 左上角圆弧
		path.AddLine(x+cornerX,y,x+width-cornerX,y); // 上边

		path.AddArc(x+width-elWid,y, elWid,elHei,270,90); // 右上角圆弧
		path.AddLine(x+width,y+cornerY, x+width,y+height-cornerY);// 右边

		path.AddArc(x+width-elWid,y+height-elHei, elWid,elHei,0,90); // 右下角圆弧
		path.AddLine(x+width-cornerX,y+height, x+cornerX,y+height); // 下边

		path.AddArc(x,y+height-elHei, elWid,elHei,90,90); 
		path.AddLine(x,y+cornerY, x, y+height-cornerY);

		Graphics graphics(GetDC());
		SmoothingMode mode = graphics.GetSmoothingMode();
		graphics.SetSmoothingMode(SmoothingModeHighQuality);


		Gdiplus::Color color1((LOBYTE((gradientColor1)>>24)),GetRValue(gradientColor1),GetGValue(gradientColor1), GetBValue(gradientColor1));
		Gdiplus::Color color2((LOBYTE((gradientColor2)>>24)),GetRValue(gradientColor2),GetGValue(gradientColor2), GetBValue(gradientColor2));

		LinearGradientMode gradientMode = LinearGradientModeVertical;
		if (!bVert)
			gradientMode = LinearGradientModeHorizontal;
		LinearGradientBrush brush(Rect(x, y, width, height), color1, color2, gradientMode);

		graphics.FillPath(&brush, &path);
		if (lineWidth > 0)
		{
			Pen pen(Gdiplus::Color((LOBYTE((borderColor) >> 24)), GetRValue(borderColor), GetGValue(borderColor), GetBValue(borderColor)), (REAL)lineWidth);
			graphics.DrawPath(&pen, &path);
		}
		graphics.SetSmoothingMode(mode);

	}
	else
	{
		if (lineWidth > 0)
		{
			width -= 1; // 这边需要修正一下，否则感觉会超过边界
			height -= 1;

		}
		Graphics graphics(GetDC());
		Gdiplus::Color color1((LOBYTE((gradientColor1)>>24)),GetRValue(gradientColor1),GetGValue(gradientColor1), GetBValue(gradientColor1));
		Gdiplus::Color color2((LOBYTE((gradientColor2)>>24)),GetRValue(gradientColor2),GetGValue(gradientColor2), GetBValue(gradientColor2));
		LinearGradientMode gradientMode = LinearGradientModeVertical;
		if (!bVert)
			gradientMode = LinearGradientModeHorizontal;
		LinearGradientBrush brush(Rect(x, y, width, height), color1, color2, gradientMode);

		graphics.FillRectangle(&brush, x, y, width, height);

		if (lineWidth > 0)
		{
			Pen pen(Gdiplus::Color((LOBYTE((borderColor) >> 24)), GetRValue(borderColor), GetGValue(borderColor), GetBValue(borderColor)), (REAL)lineWidth);
			graphics.DrawRectangle(&pen, x, y, width, height);
		}
	}

}


void IRenderDC::DrawImage(ImageObj* pImageObj,  RECT& rcDst, RECT& rcSrc, PaintParams* params)
{
	if (params && params->bAngle)
	{
		DrawImageAngle(pImageObj, rcDst, rcSrc, params);
		return;
	}

	if (params && params->bRotate)
	{
		DrawImageRotate(pImageObj, rcDst, rcSrc, params);
		return;
	}

	if (!pImageObj) return;
	if (IsRectEmpty(&rcDst) || IsRectEmpty(&rcSrc)) return;

	RECT rcPaint = IsRectEmpty(&m_rectUpdate) ? rcDst : m_rectUpdate;

	HDC hDestDC = GetDC();
	int nWidth = pImageObj->GetWidth();
	int nHeight = pImageObj->GetHeight();

	int nAlpha = 255;
	if (params)
		nAlpha = params->nAlpha;

	RECT rectClip;
	if (!IntersectRect(&rectClip, &rcPaint, &rcDst)) return;
	::GetClipBox(hDestDC, &rectClip);
	if (!IntersectRect(&rectClip, &rcPaint, &rectClip)) return;

	// 使用HBITMAP绘图
	HBITMAP hBitmap = pImageObj->GetHBitmap();
	if (hBitmap)
	{
		// 带ALPHA通道
		if (!params || params->bAlpha)
		{
 			DibObj* pDibObj = GetDibObj();
 			if (!pDibObj)
			{
				ClipObj clip;
				ClipObj::GenerateClip(hDestDC, rectClip, clip);

				HBITMAP hbmOldGlyphs = (HBITMAP)SelectObject (hdcGrobal, hBitmap);
				BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, nAlpha, AC_SRC_ALPHA};
				AlphaBlend(hDestDC,
					rcDst.left,
					rcDst.top,
					rcDst.right - rcDst.left,
					rcDst.bottom - rcDst.top, 
					hdcGrobal, 
					rcSrc.left, 
					rcSrc.top, 
					rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top,
					pixelblend);
				SelectObject (hdcGrobal, hbmOldGlyphs);

			}
			else
			{
				ImageData imgSrc, imgDst;
				LPVOID lpLockData;
				imgSrc.SetPixels(pImageObj->LockBits(&lpLockData), nWidth, nHeight, true);
				imgDst.SetPixels(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(), true);
				AlphaBlendEx(&imgDst,
					rcDst.left,
					rcDst.top,
					rcDst.right - rcDst.left,
					rcDst.bottom - rcDst.top, 
					&imgSrc,
					rcSrc.left, 
					rcSrc.top, 
					rcSrc.right - rcSrc.left, 
					rcSrc.bottom - rcSrc.top,
					nAlpha, &rectClip, NULL);
			}
		}
		else
		{
			ClipObj clip;
			ClipObj::GenerateClip(hDestDC, rectClip, clip);

			HBITMAP hbmOldGlyphs = (HBITMAP)SelectObject (hdcGrobal, hBitmap);
			int nStretchBltMode = COLORONCOLOR;
			if (params->eSmoothingMode == UISMOOTHING_HighSpeed)
				nStretchBltMode = ::SetStretchBltMode (hDestDC, COLORONCOLOR); 
			else
				nStretchBltMode = ::SetStretchBltMode (hDestDC, HALFTONE);

			::StretchBlt (hDestDC, 
				rcDst.left,
				rcDst.top,
				rcDst.right - rcDst.left,
				rcDst.bottom - rcDst.top, 
				hdcGrobal, 
				rcSrc.left, 
				rcSrc.top, 
				rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top,SRCCOPY);

			::SetStretchBltMode (hDestDC, 	nStretchBltMode);
			SelectObject (hdcGrobal, hbmOldGlyphs);
		}
		
		return;
	}

	// 使用像素绘图
	BYTE* pPixels = pImageObj->GetPixels();
	if (pPixels)
	{
		DibObj* pDibObj = GetDibObj();
		if (!pDibObj) return;

		ImageData imgSrc, imgDst;
		imgSrc.SetPixels(pPixels, nWidth, nHeight, true);
		imgDst.SetPixels(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(), true);
		AlphaBlendEx(&imgDst,
			rcDst.left,
			rcDst.top,
			rcDst.right - rcDst.left,
			rcDst.bottom - rcDst.top, 
			&imgSrc,
			rcSrc.left, 
			rcSrc.top, 
			rcSrc.right - rcSrc.left, 
			rcSrc.bottom - rcSrc.top,
			nAlpha, &rectClip, NULL);
		return;
	}

	// 使用GDI+绘图
#ifdef _GDIPLUS_SUPPORT
	Bitmap* pBitmap = pImageObj->GetBitmap();
	if (pBitmap)
	{
		Graphics g(GetDC());
		g.SetClip(Rect(rectClip.left,rectClip.top,rectClip.right - rectClip.left, rectClip.bottom - rectClip.top));

		
		int eWarpMode = params ? params->eGDIPLUSWarpMode : -1;
		ImageAttributes imAtt;
		if (eWarpMode != -1)
			imAtt.SetWrapMode(WrapMode(eWarpMode));

		if (nAlpha != 255)
		{
			REAL fAlpha = (REAL)nAlpha / 255;
			ColorMatrix colorMartrix = {
				1, 0, 0, 0, 0,
				0, 1, 0, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 0, fAlpha, 0,
				0, 0, 0, 0, 1 };
			imAtt.SetColorMatrix(&colorMartrix);
		}
		

		if (!params ||  params->eSmoothingMode == UISMOOTHING_HighSpeed)
		{
			g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);
			/*g.SetInterpolationMode(Gdiplus::InterpolationModeDefault);*/
			g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

		}
		else
		{
				g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
				g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);

		}
		if (params && (pImageObj->GetFrameCount() > 1)) pImageObj->SetActiveFrame(params->nActiveFrame);
		g.DrawImage(pBitmap, 
			Rect(rcDst.left, rcDst.top, rcDst.right - rcDst.left, rcDst.bottom - rcDst.top),
			rcSrc.left, rcSrc.top, rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top, 
			UnitPixel, &imAtt);
	}
#endif
	// 使用HICON绘图
	HICON hIcon = pImageObj->GetHIcon();
	if (hIcon)
	{
		DrawIconEx(hDestDC, rcDst.left, rcDst.top, hIcon,   rcDst.right - rcDst.left, rcDst.bottom - rcDst.top,  0, NULL, DI_NORMAL);
		return;
	}
}


void IRenderDC::DrawImage(ImageObj* pImageObj, int xDst, int yDst, int nImageIndex, PaintParams* params)
{
	if (!pImageObj) return;
	// 这暂时不加上HDPI处理，不然皮肤背景图就拉伸了
	RECT rcDst = { xDst, yDst, xDst + pImageObj->GetCellWidth(), yDst + pImageObj->GetCellHeight() };
	DrawImage(pImageObj, rcDst, nImageIndex, params);
}

void IRenderDC::DrawImageWithDpiScale(ImageObj* pImageObj, int xDst, int yDst, int nImageIndex, PaintParams* params)
{
	if (!pImageObj) return;
	// 这暂时不加上HDPI处理，不然皮肤背景图就拉伸了
	bool bEnableHDPI = params ? params->bEnableHDPI : true;
	RECT rcDst = { xDst, yDst, xDst + DPI_SCALE_EX(pImageObj->GetCellWidth(), bEnableHDPI), yDst + DPI_SCALE_EX(pImageObj->GetCellHeight(), bEnableHDPI) };
	DrawImage(pImageObj, rcDst, nImageIndex, params);
}

void IRenderDC::DrawImage(ImageObj* pImageObj, RECT& rcDst, int nImageIndex, PaintParams* params)
{
	bool bEnableHDPI = params ? params->bEnableHDPI : true;

	if (!pImageObj) return;
	int nWidth = pImageObj->GetWidth();
	int nHeight = pImageObj->GetHeight();

	// 如果是gif图
	if (pImageObj->GetFrameCount() > 1)
	{
		RECT rcSrc = {0, 0, nWidth, nHeight};
		PaintParams paramstmp;
		if (params) paramstmp = *params;
		if ((nImageIndex > 0) && (paramstmp.nActiveFrame == 0))
			paramstmp.nActiveFrame = nImageIndex;
		DrawImage(pImageObj, rcDst, rcSrc, &paramstmp);	
		return;
	}
	//

	RECT rcInset = pImageObj->GetInset();
	int iNumCellRows = pImageObj->GetCellRow();
	int iNumCellColumns = pImageObj->GetCellCol();
	int iRow = 0, iColumn = 0;
	if (iNumCellRows > 1)
	{
		if (nImageIndex < iNumCellRows)
			iRow = nImageIndex;
		else
			iRow = iNumCellRows - 1;
	}
	else
	{
		if (nImageIndex < iNumCellColumns)
			iColumn = nImageIndex;
		else
			iColumn = iNumCellColumns - 1;
	}

	int iCellWidth = pImageObj->GetCellWidth();
	int iCellHeight =  pImageObj->GetCellHeight();

	
	RECT rectImage = {iColumn * iCellWidth, iRow * iCellHeight, 
		iColumn * iCellWidth + iCellWidth, iRow * iCellHeight + iCellHeight};
	

	if ((rcInset.left == 0 && rcInset.right == 0) && (rcInset.top != 0 || rcInset.bottom != 0)) // 垂直分成三张图片
	{
		DrawThreePatchImageV(pImageObj, rcDst, rectImage, params);

	} 
	else if((rcInset.top == 0 && rcInset.bottom == 0) && (rcInset.left != 0 || rcInset.right != 0)) // 水平分成三张图片
	{
		DrawThreePatchImageH(pImageObj, rcDst, rectImage, params);
	}
	else // 分成九张图片
	{
		DrawNinePatchImage(pImageObj, rcDst, rectImage, params);
	}
	return;

	
}

void IRenderDC::DrawThreePatchImageH(ImageObj* pImageObj, RECT& rcDst, RECT& rectImage, PaintParams* params)
{
	int destHeight = rcDst.bottom - rcDst.top;
	int srcHeight = pImageObj->GetCellHeight();
	RECT rcInset = pImageObj->GetInset();

	RECT rtInnerDest = { 0 };
	rtInnerDest.left = rcDst.left + MulDivEx(rcInset.left, destHeight, srcHeight);
	rtInnerDest.right = rcDst.right - MulDivEx(rcInset.right, destHeight, srcHeight);
	rtInnerDest.top = rcDst.top;
	rtInnerDest.bottom = rcDst.bottom;

	// 画中间
	if (!params || (!params->bHole))
	{
		RECT rtInterSrc = { 0 };
		rtInterSrc.left = rectImage.left + rcInset.left;
		rtInterSrc.right = rectImage.right - rcInset.right;
		rtInterSrc.top = rectImage.top;
		rtInterSrc.bottom = rectImage.bottom;

		DrawImage(pImageObj, rtInnerDest, rtInterSrc, params);
	}

	// 画左边
	if (rcInset.left > 0)
	{
		RECT rtLeftDest = { 0 };
		rtLeftDest.left = rcDst.left;
		rtLeftDest.right = rtInnerDest.left;
		rtLeftDest.top = rcDst.top;
		rtLeftDest.bottom = rcDst.bottom;

		RECT rtLeftSrc = { 0 };
		rtLeftSrc.left = rectImage.left;
		rtLeftSrc.right = rectImage.left + rcInset.left;
		rtLeftSrc.top = rectImage.top;
		rtLeftSrc.bottom = rectImage.bottom;

		DrawImage(pImageObj, rtLeftDest, rtLeftSrc, params);
	}

	// 画右边
	if (rcInset.right > 0)
	{
		RECT rtRightDest = { 0 };
		rtRightDest.left = rtInnerDest.right;
		rtRightDest.right = rcDst.right;
		rtRightDest.top = rcDst.top;
		rtRightDest.bottom = rcDst.bottom;

		RECT rtRightSrc = { 0 };
		rtRightSrc.left = rectImage.right - rcInset.right;
		rtRightSrc.right = rectImage.right;
		rtRightSrc.top = rectImage.top;
		rtRightSrc.bottom = rectImage.bottom;

		DrawImage(pImageObj, rtRightDest, rtRightSrc, params);
	}
}

void IRenderDC::DrawThreePatchImageV(ImageObj* pImageObj, RECT& rcDst, RECT& rectImage, PaintParams* params)
{
	int destWidth = rcDst.right - rcDst.left;
	int srcWidth = pImageObj->GetCellWidth();
	RECT rcInset = pImageObj->GetInset();

	RECT rtInnerDest = { 0 };
	rtInnerDest.left = rcDst.left ;
	rtInnerDest.right = rcDst.right;
	rtInnerDest.top = rcDst.top + MulDivEx(rcInset.top, destWidth, srcWidth);
	rtInnerDest.bottom = rcDst.bottom - MulDivEx(rcInset.bottom, destWidth, srcWidth);

	// 画中间
	if (!params || (!params->bHole))
	{
		RECT rtInterSrc = { 0 };
		rtInterSrc.left = rectImage.left ;
		rtInterSrc.right = rectImage.right ;
		rtInterSrc.top = rectImage.top + +rcInset.top;
		rtInterSrc.bottom = rectImage.bottom - rcInset.bottom;

		DrawImage(pImageObj, rtInnerDest, rtInterSrc, params);
	}

	// 画上边
	if (rcInset.top > 0)
	{
		RECT rtTopDest = { 0 };
		rtTopDest.left = rcDst.left;
		rtTopDest.right = rcDst.right;
		rtTopDest.top = rcDst.top;
		rtTopDest.bottom = rtInnerDest.top;

		RECT rtTopSrc = { 0 };
		rtTopSrc.left = rectImage.left;
		rtTopSrc.right = rectImage.right;
		rtTopSrc.top = rectImage.top;
		rtTopSrc.bottom = rectImage.top + +rcInset.top;

		DrawImage(pImageObj, rtTopDest, rtTopSrc, params);
	}

	// 画下边
	if (rcInset.bottom > 0)
	{
		RECT rtBottomDest = { 0 };
		rtBottomDest.left = rcDst.left;
		rtBottomDest.right = rcDst.right;
		rtBottomDest.top = rtInnerDest.bottom;
		rtBottomDest.bottom = rcDst.bottom;

		RECT rtBottomSrc = { 0 };
		rtBottomSrc.left = rectImage.left;
		rtBottomSrc.right = rectImage.right;
		rtBottomSrc.top = rectImage.bottom - rcInset.bottom;
		rtBottomSrc.bottom = rectImage.bottom;

		DrawImage(pImageObj, rtBottomDest, rtBottomSrc, params);
	}
}


void IRenderDC::DrawNinePatchImage(ImageObj* pImageObj, RECT& rcDst, RECT& rectImage, PaintParams* params)
{
	bool bEnableHDPI = params ? params->bEnableHDPI : true;
	if (!pImageObj) return;
	RECT rcInset = pImageObj->GetInset();

	RECT rtInnerDest;
	rtInnerDest.left = rcDst.left + DPI_SCALE_EX(rcInset.left, bEnableHDPI);
	rtInnerDest.top = rcDst.top + DPI_SCALE_EX(rcInset.top, bEnableHDPI);
	rtInnerDest.right = rcDst.right - DPI_SCALE_EX(rcInset.right, bEnableHDPI);
	rtInnerDest.bottom = rcDst.bottom - DPI_SCALE_EX(rcInset.bottom, bEnableHDPI);

	// 画中间部分
	if (!params || (!params->bHole))
	{
		
		RECT rectInterSrc = { rcInset.left, rcInset.top, pImageObj->GetCellWidth() - rcInset.right, pImageObj->GetCellHeight() - rcInset.bottom };
		OffsetRect(&rectInterSrc, rectImage.left, rectImage.top);

		DrawImage(pImageObj, rtInnerDest, rectInterSrc, params);
	}

	RECT rectPart, r;
	if (rtInnerDest.right - rtInnerDest.left > 0 || rtInnerDest.bottom - rtInnerDest.top > 0)
	{
		if (rtInnerDest.bottom - rtInnerDest.top > 0)
		{
			// 左边中间部分
			if (rcInset.left > 0)
			{
				r.left = rcDst.left;
				r.right = rtInnerDest.left;
				r.top = rtInnerDest.top;
				r.bottom = rtInnerDest.bottom;

				rectPart.left = rectImage.left;
				rectPart.top  = rectImage.top + rcInset.top; 
				rectPart.right = rectImage.left + rcInset.left;
				rectPart.bottom = rectImage.bottom - rcInset.bottom;

				DrawImage(pImageObj, r, rectPart, params);
			}

			// 右边中间部分
			if (rcInset.right > 0)
			{
				r.left = rtInnerDest.right;
				r.right = rcDst.right;
				r.top = rtInnerDest.top;
				r.bottom = rtInnerDest.bottom;

				rectPart.left = rectImage.right - rcInset.right;
				rectPart.top  = rectImage.top+ rcInset.top;
				rectPart.right = rectImage.right;
				rectPart.bottom = rectImage.bottom - rcInset.bottom;

				DrawImage(pImageObj, r, rectPart, params);
			}
		}

		if (rtInnerDest.right - rtInnerDest.left > 0)
		{
			// 上边中间部分
			if (rcInset.top > 0)
			{
				r.left = rtInnerDest.left;
				r.right = rtInnerDest.right;
				r.top = rcDst.top;
				r.bottom = rtInnerDest.top;

				rectPart.left = rectImage.left + rcInset.left;
				rectPart.top  = rectImage.top;
				rectPart.right = rectImage.right - rcInset.right;
				rectPart.bottom = rectImage.top + rcInset.top;

				DrawImage(pImageObj, r, rectPart, params);
			}

			// 下边中间部分
			if (rcInset.bottom > 0)
			{
				r.left = rtInnerDest.left;
				r.right = rtInnerDest.right;
				r.top = rtInnerDest.bottom;
				r.bottom = rcDst.bottom;

				rectPart.left = rectImage.left + rcInset.left;
				rectPart.top  = rectImage.bottom - rcInset.bottom;
				rectPart.right = rectImage.right - rcInset.right;
				rectPart.bottom = rectImage.bottom;

				DrawImage(pImageObj, r, rectPart, params);
			}
		}

		// 左上角
		if (rcInset.left > 0 && rcInset.top > 0)
		{
			r.left = rcDst.left;
			r.right = rtInnerDest.left;
			r.top = rcDst.top;
			r.bottom = rtInnerDest.top;

			rectPart.left = rectImage.left;
			rectPart.top  = rectImage.top;
			rectPart.right = rectImage.left + rcInset.left;
			rectPart.bottom = rectImage.top + rcInset.top;

			DrawImage(pImageObj, r, rectPart, params); 
		}

		// 右上角
		if (rcInset.right > 0 && rcInset.top > 0)
		{
			r.left = rtInnerDest.right;
			r.right = rcDst.right;
			r.top = rcDst.top;
			r.bottom = rtInnerDest.top;

			rectPart.left = rectImage.right - rcInset.right;
			rectPart.top  = rectImage.top;
			rectPart.right = rectImage.right;
			rectPart.bottom = rectImage.top + rcInset.top;

			DrawImage(pImageObj, r, rectPart, params);
		}

		// 左下角
		if (rcInset.left > 0 && rcInset.bottom > 0)
		{
			r.left = rcDst.left;
			r.right = rtInnerDest.left;
			r.top = rtInnerDest.bottom;
			r.bottom = rcDst.bottom;

			rectPart.left = rectImage.left;
			rectPart.top  = rectImage.bottom - rcInset.bottom;
			rectPart.right = rectImage.left + rcInset.left;
			rectPart.bottom = rectImage.bottom;

			DrawImage(pImageObj, r, rectPart, params);
		}

		// 右下角
		if (rcInset.right > 0 && rcInset.bottom > 0)
		{
			r.left = rtInnerDest.right;
			r.right = rcDst.right;
			r.top = rtInnerDest.bottom;
			r.bottom = rcDst.bottom;

			rectPart.left = rectImage.right - rcInset.right;
			rectPart.top  = rectImage.bottom - rcInset.bottom;
			rectPart.right = rectImage.right;
			rectPart.bottom = rectImage.bottom;

			DrawImage(pImageObj, r, rectPart, params);
		}
	}
}

 
#define DEFAULT_COLOR_SELECTEDBK	0xBAE4FF
FontObj* AddHtmlTextFont(vector<FontObj *> &vecFonts,  LPCTSTR lpszFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	FontObj* pFontObj = new FontObj;
	pFontObj->SetId(_T("htmltext"));
	if (lpszFontName)
	{
		pFontObj->SetFaceName(lpszFontName);
	}
	pFontObj->SetFontSize(nSize);
	pFontObj->SetBold(bBold);
	pFontObj->SetUnderline(bUnderline);
	pFontObj->SetItalic(bItalic);
	vecFonts.push_back(pFontObj);
	return pFontObj;
}

FontObj* GetHtmlTextFont(vector<FontObj *> &vecFonts, LPCTSTR lpszFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	int nFontSize = (int)vecFonts.size();
	for (int i=0; i < nFontSize; i++)
	{
		FontObj * pFontObj = vecFonts[i];
		if (equal_icmp(pFontObj->GetFaceName(), lpszFontName) && pFontObj->GetHeight()==nSize && pFontObj->IsBold()==bBold
			&& pFontObj->IsUnderline()== bUnderline && pFontObj->IsItalic()==bItalic)
		{
			return pFontObj;
		}
	}
	return NULL;
}

extern DWORD add_alpha_to_color(DWORD dwColor, int alpha);
void IRenderDC::DrawHtmlText(FontObj* pFontObjDefault, RECT& rcDst, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, tstring* sLinks, int& nLinkRects, tstring sHoverLink, DWORD dwLinkColor, DWORD dwLinkHoverColor, UINT uStyle, HtmlPaintParams &paintParams, bool &bEndEllipsis, UITYPE_FONT nFontType, int alpha)
{
	RECT rc = rcDst;
	if (nFontType == UIFONT_DEFAULT)
		nFontType = GetResObj()->GetDefaultFontType();

	HDC hDC = GetDC();
	if (pFontObjDefault==NULL || pstrText==NULL || ::IsRectEmpty(&rc))
	{
		return;
	}

	RECT rcCalcClipBox;
	if (GetEnableAlpha() && GetDibObj() && (nFontType == UIFONT_GDI) && ((uStyle&DT_CALCRECT) == 0))
	{
		_CalcClipBox(this, rc, rcCalcClipBox);
		CSSE::IncreaseAlpha(GetDibObj()->GetBits(), GetDibObj()->GetWidth(), GetDibObj()->GetHeight(),  rcCalcClipBox);
	}

	RECT rcClip = {0};
	::GetClipBox(hDC, &rcClip);
	HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	HRGN hRgn = ::CreateRectRgnIndirect(&rc);
	bool bDraw = (uStyle&DT_CALCRECT)==0;
	if (bDraw)
	{
		::ExtSelectClipRgn(hDC, hRgn, RGN_AND);
	}

	// gdi+画文字会偏移一点，这里简单修正一下
	if (((uStyle&DT_CALCRECT) == 0) && (nFontType == UIFONT_GDIPLUS))
	{
		rc.right -=1;
	}

	DWORD dwCurTextColor = dwTextColor;
	DWORD dwDefaultColor = dwTextColor;
	FontObj *pCurFontObj = pFontObjDefault;
	TEXTMETRIC textMetric = pFontObjDefault->GetTextMetric(hDC); 
	DWORD dwBkColor = RGB(GetBValue(DEFAULT_COLOR_SELECTEDBK), GetGValue(DEFAULT_COLOR_SELECTEDBK), GetRValue(DEFAULT_COLOR_SELECTEDBK));
	DWORD *pCurBkColor = NULL;

	// If the drawstyle include a alignment, we'll need to first determine the text-size so we can draw it at the correct position...
	if (((uStyle&DT_CENTER)!=0 || (uStyle&DT_RIGHT)!=0 || (uStyle&DT_VCENTER)!=0 || (uStyle&DT_BOTTOM)!=0) && (uStyle&DT_CALCRECT)==0)
	{
		int nLinks = 0;
		RECT rcText = {0, 0, 9999, 100};
		HtmlPaintParams tempParams;
		DrawHtmlText(pFontObjDefault, rcText, pstrText, dwTextColor, NULL, NULL, nLinks, sHoverLink, dwLinkColor, dwLinkHoverColor, uStyle | DT_CALCRECT, tempParams, bEndEllipsis, nFontType);
		if ((uStyle&DT_SINGLELINE) != 0)
		{
			if ((uStyle&DT_CENTER) != 0) 
			{
				rc.left = rc.left + ((rc.right - rc.left)/2) - ((rcText.right - rcText.left)/2);
				rc.right = rc.left + (rcText.right - rcText.left);
			}
			if ((uStyle&DT_RIGHT) != 0)
			{
				rc.left = rc.right - (rcText.right - rcText.left);
			}
		}
		if ((uStyle&DT_VCENTER) != 0) 
		{
			rc.top = rc.top + ((rc.bottom - rc.top)/2) - ((rcText.bottom - rcText.top)/2);
			rc.bottom = rc.top + (rcText.bottom - rcText.top);
		}
		if ((uStyle&DT_BOTTOM) != 0)
		{
			rc.top = rc.bottom - (rcText.bottom - rcText.top);
		}
	}

	bool bHoverLink = false;
	if (!sHoverLink.empty())
	{
		bHoverLink = true;
	}
	
	vector<FontObj *> vecFonts;
	CStdPtrArray aFontArray(10);
	CStdPtrArray aColorArray(10);
	CStdPtrArray aPIndentArray(10);

	int iLinkIndex = 0;
	POINT pt = {rc.left, rc.top};
	int cyLine = textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
	int cyMinHeight = 0;
	int cxMaxWidth = 0;
	POINT ptLinkStart = {0};
	bool bLineEnd = false;
	bool bInRaw = false;
	bool bInLink = false;
	bool bInSelected = false;
	int iLineLinkIndex = 0;
	int iLineNum = 0;
	DWORD dwPreLineTextColor = dwTextColor;

	// 排版习惯是图文底部对齐，所以每行绘制都要分两步，先计算高度，再绘制
	CStdPtrArray aLineFontArray;
	CStdPtrArray aLineColorArray;
	CStdPtrArray aLinePIndentArray;
	LPCTSTR pstrLineBegin = pstrText;
	bool bLineInRaw = false;
	bool bLineInLink = false;
	bool bLineInSelected = false;
	int cyLineHeight = 0;
	bool bLineDraw = false;
	while (*pstrText != _T('\0')) 
	{
		if (pt.x >= rc.right || *pstrText == _T('\n') || bLineEnd) 
		{
			if (*pstrText == _T('\n')) pstrText++;
			if (bLineEnd)
			{
				bLineEnd = false;
				if (bDraw && !bLineDraw)
				{
					dwCurTextColor = dwPreLineTextColor;
				}
			}
			if (!bLineDraw) 
			{
				if (bInLink && iLinkIndex < nLinkRects)
				{
					::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, min(pt.x, rc.right), pt.y + cyLine);
					tstring *pStr1 = (tstring*)(sLinks + iLinkIndex - 1);
					tstring *pStr2 = (tstring*)(sLinks + iLinkIndex);
					*pStr2 = *pStr1;
				}
				for (int i = iLineLinkIndex; i < iLinkIndex; i++)
				{
					prcLinks[i].bottom = pt.y + cyLine;
				}
				if (bDraw) 
				{
					bInLink = bLineInLink;
					iLinkIndex = iLineLinkIndex;
				}
			}
			else
			{
				if (bInLink && iLinkIndex < nLinkRects) iLinkIndex++;
				bLineInLink = bInLink;
				iLineLinkIndex = iLinkIndex;
			}
			if ((uStyle & DT_SINGLELINE) != 0 && (!bDraw || bLineDraw))
			{
				break;
			}
			if (bDraw)
			{
				bLineDraw = !bLineDraw; // !
			}
			pt.x = rc.left;
			if (!bLineDraw)
			{
				pt.y += cyLine;
			}
			if (pt.y > rc.bottom && bDraw)
			{
				break;
			}
			ptLinkStart = pt;
			cyLine = textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize()-1);
			if (pt.x >= rc.right)
			{
				break;
			}
		}
		else if (!bInRaw && (*pstrText==_T('<') || *pstrText==_T('{'))
			&& (pstrText[1]>=_T('a') && pstrText[1]<=_T('z'))
			&& (pstrText[2]==_T(' ') || pstrText[2]==_T('>') || pstrText[2]==_T('}')))
		{
			pstrText++;
			LPCTSTR pstrNextStart = NULL;
			switch( *pstrText )
			{
			case _T('a'):
				{
					pstrText++;
					while (*pstrText > _T('\0') && *pstrText <= _T(' '))
					{
						pstrText = ::CharNext(pstrText);
					}
					if (iLinkIndex < nLinkRects && !bLineDraw)
					{
						tstring *pStr = (tstring*)(sLinks + iLinkIndex);
						if (pStr != NULL)
						{
							pStr->clear();
						}
						while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
						{
							LPCTSTR pstrTemp = ::CharNext(pstrText);
							while (pstrText < pstrTemp)
							{
								if (pStr)
									*pStr += *pstrText++;
							}
						}
					}

					DWORD clrColor = dwLinkColor;
					if (bHoverLink && iLinkIndex < nLinkRects)
					{
						tstring *pStr = (tstring*)(sLinks + iLinkIndex);
						if (sHoverLink == *pStr)
						{
							clrColor = dwLinkHoverColor;
						}
					}
					/*
					else if( prcLinks == NULL ) 
					{
					    if( ::PtInRect(&rc, ptMouse) )
						{
							clrColor = DEFAULT_COLOR_LINKHOVERFONT;
						}        
					}
					*/
					aColorArray.Add((LPVOID)clrColor);
					dwCurTextColor = clrColor;
					FontObj* pFontObj = pFontObjDefault;
					if (aFontArray.GetSize() > 0)
					{
						pFontObj = (FontObj *)aFontArray.GetAt(aFontArray.GetSize() - 1);
					}
					if (clrColor==dwLinkHoverColor && pFontObj->IsUnderline()==false)
					{
						FontObj *pFontObjFind = GetHtmlTextFont(vecFonts, pFontObj->GetFaceName(), pFontObj->GetHeight(), pFontObj->IsBold(), true, pFontObj->IsItalic());
						if (pFontObjFind == NULL)
						{
							pFontObjFind = AddHtmlTextFont(vecFonts, pFontObj->GetFaceName(), pFontObj->GetHeight(), pFontObj->IsBold(), true, pFontObj->IsItalic());
						}
						aFontArray.Add(pFontObjFind);
						textMetric = pFontObjFind->GetTextMetric(hDC);
						cyLine = max(cyLine, textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
						pCurFontObj = pFontObjFind;
					}
					ptLinkStart = pt;
					bInLink = true;
				}
				break;
			case _T('b'):
				{
					pstrText++;
					FontObj *pFontObj = pFontObjDefault;
					if (aFontArray.GetSize() > 0)
					{
						pFontObj = (FontObj *)aFontArray.GetAt(aFontArray.GetSize() - 1);
					}
					if (pFontObj->IsBold() == false) 
					{
						FontObj *pFontObjFind = GetHtmlTextFont(vecFonts, pFontObj->GetFaceName(), pFontObj->GetHeight(), true, pFontObj->IsUnderline(), pFontObj->IsItalic());
						if (pFontObjFind == NULL)
						{
							pFontObjFind = AddHtmlTextFont(vecFonts, pFontObj->GetFaceName(), pFontObj->GetHeight(), true, pFontObj->IsUnderline(), pFontObj->IsItalic());
						}
						aFontArray.Add(pFontObjFind);
						textMetric = pFontObjFind->GetTextMetric(hDC);
						cyLine = max(cyLine, textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
						pCurFontObj = pFontObjFind;
					}
				}
				break;
			case _T('c'):
				{
					pstrText++;
					while (*pstrText > _T('\0') && *pstrText <= _T(' '))
					{
						pstrText = ::CharNext(pstrText);
					}


					LPCTSTR pstrTmp = NULL;
					if (*pstrText == _T('#'))
					{
						pstrText++;
						pstrTmp = pstrText;

						while (*pstrText != _T('}'))
						{
							pstrText++;
						}

						TCHAR szColor[50] = { '\0' };

						int len = pstrText - pstrTmp;
						_tcsncpy(szColor, pstrTmp, min(len, 50));

						ColorObj* colorObj = GetUIRes()->GetColorObj(szColor);
						if (colorObj)
						{
							DWORD clrColor = colorObj->GetColor();
							aColorArray.Add((LPVOID)clrColor);
							dwCurTextColor = clrColor;
						}
						else
						{
							//DWORD clrValue = tcstol(szColor, NULL, 16);
							DWORD clrColor = CSSE::StringToColor(szColor);// RGB(GetBValue(clrValue), GetGValue(clrValue), GetRValue(clrValue));
							aColorArray.Add((LPVOID)clrColor);
							dwCurTextColor = clrColor;
						}
					}	
				}
				break;
			case _T('f'):
				{
					pstrText++;
					while (*pstrText > _T('\0') && *pstrText <= _T(' '))
					{
						pstrText = ::CharNext(pstrText);
					}

					LPCTSTR pstrTemp = pstrText;
					{
						tstring sFontName;
						tstring sFontAttr;
						while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ')) 
						{
							pstrTemp = ::CharNext(pstrText);
							while (pstrText < pstrTemp) 
							{
								sFontName += *pstrText++;
							}
						}

			
						FontObj* pFontObj = GetUIRes()->GetFont(sFontName.c_str());
						if (pFontObj)
						{
							aFontArray.Add(pFontObj);
							textMetric = pFontObj->GetTextMetric(hDC);
							pCurFontObj = pFontObj;
						}
						else
						{
							int iFontSize = 10;
							bool bBold = false;
							bool bUnderline = false;
							bool bItalic = false;
							
							iFontSize = (int)_tcstol(sFontName.c_str(), nullptr, 10);
							while (*pstrText > _T('\0') && *pstrText <= _T(' '))
							{
								pstrText = ::CharNext(pstrText);
							}
							while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
							{
								pstrTemp = ::CharNext(pstrText);
								while (pstrText < pstrTemp)
								{
									sFontAttr += *pstrText++;
								}
							}
							StrUtil::ToLowerCase(sFontAttr);
							if (sFontAttr.find(_T("bold")) != tstring::npos) bBold = true;
							if (sFontAttr.find(_T("underline")) != tstring::npos) bUnderline = true;
							if (sFontAttr.find(_T("italic")) != tstring::npos) bItalic = true;
							FontObj *pFontObjFind = GetHtmlTextFont(vecFonts, sFontName.c_str(), iFontSize, bBold, bUnderline, bItalic);
							if (pFontObjFind == NULL)
							{
								pFontObjFind = AddHtmlTextFont(vecFonts, sFontName.c_str(), iFontSize, bBold, bUnderline, bItalic);
							}
							aFontArray.Add(pFontObjFind);
							textMetric = pFontObjFind->GetTextMetric(hDC);
							pCurFontObj = pFontObjFind;
						}
					}
					cyLine = max(cyLine, textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize()-1));
				}
				break;
			case _T('i'):
				{    
					pstrNextStart = pstrText - 1;
					pstrText++;
					tstring sImageString = pstrText;
					//int iWidth = 0;
					//int iHeight = 0;
					while (*pstrText > _T('\0') && *pstrText <= _T(' ')) pstrText = ::CharNext(pstrText);
					ImageObj* pImageObj = NULL;
					tstring sName;
					while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ')) 
					{
						LPCTSTR pstrTemp = ::CharNext(pstrText);
						while (pstrText < pstrTemp)
						{
							sName += *pstrText++;
						}
					}
					if (sName.empty())
					{ 
						pstrNextStart = NULL;
						FontObj *pFontObj = pFontObjDefault;
						if (aFontArray.GetSize() > 0)
						{
							pFontObj = (FontObj *)aFontArray.GetAt(aFontArray.GetSize() - 1);
						}
						if (pFontObj->IsItalic() == false) 
						{
							FontObj *pFontObjFind = GetHtmlTextFont(vecFonts, pFontObj->GetFaceName(), pFontObj->GetHeight(), pFontObj->IsBold(), pFontObj->IsUnderline(), true);
							if (pFontObjFind == NULL)
							{
								pFontObjFind = AddHtmlTextFont(vecFonts, pFontObj->GetFaceName(), pFontObj->GetHeight(), pFontObj->IsBold(), pFontObj->IsUnderline(), true);
							}
							aFontArray.Add(pFontObjFind);
							textMetric = pFontObjFind->GetTextMetric(hDC);
							cyLine = max(cyLine, textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
							pCurFontObj = pFontObjFind;
						}
					}
					else
					{
						while( *pstrText > _T('\0') && *pstrText <= _T(' ') )
						{
							pstrText = ::CharNext(pstrText);
						}
						int iImageListNum = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						if( iImageListNum <= 0 )
						{
							iImageListNum = 1;
						}
						while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) 
						{
							pstrText = ::CharNext(pstrText);
						}
						int iImageListIndex = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						if( iImageListIndex < 0 || iImageListIndex >= iImageListNum )
						{
							iImageListIndex = 0;
						}
						if( _tcsstr(sImageString.c_str(), _T("file=\'")) != NULL || _tcsstr(sImageString.c_str(), _T("res=\'")) != NULL )
						{
							tstring sImageResType;
							tstring sImageName;
							LPCTSTR pStrImage = sImageString.c_str();
							tstring sItem;
							tstring sValue;
							while( *pStrImage != _T('\0') ) 
							{
								sItem.clear();
								sValue.clear();
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
								while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) 
								{
									LPTSTR pstrTemp = ::CharNext(pStrImage);
									while( pStrImage < pstrTemp)
									{
										sItem += *pStrImage++;
									}
								}
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') )
								{
									pStrImage = ::CharNext(pStrImage);
								}
								if( *pStrImage++ != _T('=') )
								{
									break;
								}
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') )
								{
									pStrImage = ::CharNext(pStrImage);
								}
								if( *pStrImage++ != _T('\'') )
								{
									break;
								}
								while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) 
								{
									LPTSTR pstrTemp = ::CharNext(pStrImage);
									while( pStrImage < pstrTemp) 
									{
										sValue += *pStrImage++;
									}
								}
								if( *pStrImage++ != _T('\'') )
								{
									break;
								}
								if( !sValue.empty() ) 
								{
									if( sItem == _T("file") || sItem == _T("res") ) 
									{
										sImageName = sValue;
									}
									else if( sItem == _T("restype") )
									{
										sImageResType = sValue;
									}
								}
								if( *pStrImage++ != _T(' ') )
								{
									break;
								}
							}

							pImageObj = GetUIRes()->LoadImage(sImageName.c_str(), false);
						}
						else
						{
							pImageObj = GetUIRes()->LoadImage(sName.c_str(), false);
						}

						if( pImageObj )
						{
							int iWidth = /*DPI_SCALE*/(pImageObj->GetWidth());
							int iHeight = /*DPI_SCALE*/(pImageObj->GetHeight());
							if( iImageListNum > 1 )
							{
								iWidth /= iImageListNum;
							}
							if( pt.x + iWidth > rc.right && pt.x > rc.left && (uStyle & DT_SINGLELINE) == 0 ) 
							{
								bLineEnd = true;
							}
							else
							{
								pstrNextStart = NULL;
								if( bDraw && bLineDraw )
								{
									RECT rcImage = {pt.x, pt.y+cyLineHeight-iHeight, pt.x+iWidth, pt.y+cyLineHeight};
									if( iHeight < cyLineHeight ) 
									{ 
										rcImage.bottom -= (cyLineHeight - iHeight) / 2;
										rcImage.top = rcImage.bottom -  iHeight;
									}
									RECT rcBmpPart = { 0, 0, /*DPI_SCALE_BACK*/(iWidth), /*DPI_SCALE_BACK*/(iHeight) };
									rcBmpPart.left = /*DPI_SCALE_BACK*/(iWidth) * iImageListIndex;
									rcBmpPart.right = /*DPI_SCALE_BACK*/(iWidth) * (iImageListIndex + 1);
									if (rcImage.bottom <= rc.bottom)
									{
										DrawImage(pImageObj, rcImage, rcBmpPart);
									}
								}

								cyLine = max(iHeight, cyLine);
								pt.x += iWidth;
								cyMinHeight = pt.y + iHeight;
								cxMaxWidth = max(cxMaxWidth, pt.x);
								// 没释放内存泄漏了
								pImageObj->Release();
							}
						}
						else
						{
							pstrNextStart = NULL;
						}
					}
				}
				break;
			case _T('n'):
				{
					pstrText++;
					if ((uStyle & DT_SINGLELINE) != 0)
					{
						break;
					}
					bLineEnd = true;
				}
				break;
			case _T('p'):
				{
					pstrText++;
					if (pt.x > rc.left)
					{
						bLineEnd = true;
					}
					while (*pstrText > _T('\0') && *pstrText <= _T(' '))
					{
						pstrText = ::CharNext(pstrText);
					}
					int cyLineExtra = (int)_tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
					aPIndentArray.Add((LPVOID)cyLineExtra);
					cyLine = max(cyLine, textMetric.tmHeight + textMetric.tmExternalLeading + cyLineExtra);
				}
				break;
			case _T('r'):
				{
					pstrText++;
					bInRaw = true;
				}
				break;
			case _T('s'):
				{
					pstrText++;
					bInSelected = !bInSelected;
					if (bDraw && bLineDraw) 
					{
						if (bInSelected)
						{
							pCurBkColor = &dwBkColor;
						}
						else
						{
							pCurBkColor = NULL;
						}
					}
				}
				break;
			case _T('u'):
				{
					pstrText++;
					FontObj* pFontObj = pFontObjDefault;
					if (aFontArray.GetSize() > 0)
					{
						pFontObj = (FontObj *)aFontArray.GetAt(aFontArray.GetSize() - 1);
					}
					if (pFontObj->IsUnderline() == false)
					{
						FontObj *pFontObjFind = GetHtmlTextFont(vecFonts, pFontObj->GetFaceName(), pFontObj->GetHeight(), pFontObj->IsBold(), true, pFontObj->IsItalic());
						if (pFontObjFind == NULL)
						{
							pFontObjFind = AddHtmlTextFont(vecFonts, pFontObj->GetFaceName(), pFontObj->GetHeight(), pFontObj->IsBold(), true, pFontObj->IsItalic());
						}
						aFontArray.Add(pFontObjFind);
						textMetric = pFontObjFind->GetTextMetric(hDC);
						cyLine = max(cyLine, textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
						pCurFontObj = pFontObjFind;
					}
				}
				break;
			case _T('x'):
				{
					pstrText++;
					while (*pstrText > _T('\0') && *pstrText <= _T(' '))
					{
						pstrText = ::CharNext(pstrText);
					}
					int iWidth = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
					pt.x += iWidth;
					cxMaxWidth = max(cxMaxWidth, pt.x);
				}
				break;
			case _T('y'):
				{
					pstrText++;
					while (*pstrText > _T('\0') && *pstrText <= _T(' '))
					{
						pstrText = ::CharNext(pstrText);
					}
					cyLine = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
				}
				break;
			}
			if( pstrNextStart != NULL )
			{
				pstrText = pstrNextStart;
			}
			else 
			{
				while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
				{
					pstrText = ::CharNext(pstrText);
				}
				pstrText = ::CharNext(pstrText);
			}
		}
		else if (!bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') ) && pstrText[1] == _T('/'))
		{
			pstrText++;
			pstrText++;
			switch(*pstrText)
			{
			case _T('c'):
				{
					pstrText++;
					aColorArray.Remove(aColorArray.GetSize() - 1);
					DWORD clrColor = dwDefaultColor;
					if (aColorArray.GetSize() > 0)
					{
						clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
					}
					dwCurTextColor = clrColor;
				}
				break;
			case _T('p'):
				{
					pstrText++;
					if (pt.x > rc.left)
					{
						bLineEnd = true;
					}
					aPIndentArray.Remove(aPIndentArray.GetSize() - 1);
					cyLine = max(cyLine, textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
				}
				break;
			case _T('s'):
				{
					pstrText++;
					bInSelected = !bInSelected;
					if (bDraw && bLineDraw)
					{
						if (bInSelected)
						{
							pCurBkColor = &dwBkColor;
						}
						else
						{
							pCurBkColor = NULL;
						}
					}
				}
				break;
			case _T('a'):
				{
					if (iLinkIndex < nLinkRects)
					{
						if (!bLineDraw)
						{
							::SetRect(&prcLinks[iLinkIndex], ptLinkStart.x, ptLinkStart.y, min(pt.x, rc.right), pt.y + textMetric.tmHeight + textMetric.tmExternalLeading);
						}
						iLinkIndex++;
					}
					aColorArray.Remove(aColorArray.GetSize() - 1);
					DWORD clrColor = dwTextColor;
					if (aColorArray.GetSize() > 0)
					{
						clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
					}
					dwCurTextColor = clrColor;
					bInLink = false;
				}
			case _T('b'):
			case _T('f'):
			case _T('i'):
			case _T('u'):
				{
					pstrText++;
					aFontArray.Remove(aFontArray.GetSize() - 1);
					FontObj* pFontObj = (FontObj *)aFontArray.GetAt(aFontArray.GetSize() - 1);
					if (pFontObj == NULL)
					{
						pFontObj = pFontObjDefault;
					}
					if (textMetric.tmItalic && pFontObj->IsItalic() == false)
					{
						ABCFLOAT abcf;
						ZeroMemory(&abcf, sizeof(ABCFLOAT));
						::GetCharABCWidthsFloat(hDC, _T(' '), _T(' '), &abcf);
						int abcfWidth = int((abcf.abcfA+abcf.abcfB+abcf.abcfC)/2+0.50000f); //简单修正可以直接赋值为3
						pt.x += abcfWidth;
						//ABC abc;
						//::GetCharABCWidths(hDC, _T(' '), _T(' '), &abc);
						//pt.x += abc.abcC / 2; // 简单修正一下斜体混排的问题, 正确做法应该是http://support.microsoft.com/kb/244798/en-us
					}
					textMetric = pFontObj->GetTextMetric(hDC);
					cyLine = max(cyLine, textMetric.tmHeight + textMetric.tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
					pCurFontObj = pFontObj;
				}
				break;
			}
			while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
			{
				pstrText = ::CharNext(pstrText);
			}
			pstrText = ::CharNext(pstrText);
		}
		else if (!bInRaw &&  *pstrText == _T('<') && pstrText[2] == _T('>') && (pstrText[1] == _T('{')  || pstrText[1] == _T('}')))
		{
			RECT rcSpace = {0};
			pCurFontObj->CalcText(hDC, rcSpace, &pstrText[1], DT_CALC_SINGLELINE, nFontType, 1);
			SIZE szSpace = {rcSpace.right-rcSpace.left, rcSpace.bottom-rcSpace.left};

			if (bDraw && bLineDraw)
			{
				DWORD dwTextColor = add_alpha_to_color(dwCurTextColor, alpha);
				this->TextOut(pCurFontObj, pt.x, pt.y + cyLineHeight - textMetric.tmHeight - textMetric.tmExternalLeading, &pstrText[1], 1, dwTextColor, pCurBkColor, nFontType);
			}
			pt.x += szSpace.cx;
			cxMaxWidth = max(cxMaxWidth, pt.x);
			pstrText++;
			pstrText++;
			pstrText++;
		}
		else if (!bInRaw &&  *pstrText == _T('{') && pstrText[2] == _T('}') && (pstrText[1] == _T('<')  || pstrText[1] == _T('>')))
		{
			RECT rcSpace = {0};
			pCurFontObj->CalcText(hDC, rcSpace, &pstrText[1], DT_CALC_SINGLELINE, nFontType, 1);
			SIZE szSpace = {rcSpace.right-rcSpace.left, rcSpace.bottom-rcSpace.left};

			if (bDraw && bLineDraw)
			{
				DWORD dwTextColor = add_alpha_to_color(dwCurTextColor, alpha);
				this->TextOut(pCurFontObj, pt.x, pt.y + cyLineHeight - textMetric.tmHeight - textMetric.tmExternalLeading, &pstrText[1], 1, dwTextColor, pCurBkColor, nFontType);
			}
			pt.x += szSpace.cx;
			cxMaxWidth = max(cxMaxWidth, pt.x);
			pstrText++;
			pstrText++;
			pstrText++;
		}
		else if (!bInRaw &&  *pstrText == _T(' '))
		{
			RECT rcSpace={0};
			pCurFontObj->CalcText(hDC, rcSpace, _T(" "), DT_CALC_SINGLELINE, nFontType, 1);
			SIZE szSpace = {rcSpace.right-rcSpace.left, rcSpace.bottom-rcSpace.left};

			// Still need to paint the space because the font might have underline formatting.
			if (bDraw && bLineDraw)
			{
				DWORD dwTextColor = add_alpha_to_color(dwCurTextColor, alpha);
				this->TextOut(pCurFontObj, pt.x, pt.y + cyLineHeight - textMetric.tmHeight - textMetric.tmExternalLeading, _T(" "), 1, dwTextColor, pCurBkColor, nFontType);
			}
			pt.x += szSpace.cx;
			cxMaxWidth = max(cxMaxWidth, pt.x);
			pstrText++;
		}
		else
		{
			POINT ptPos = pt;
			int cchChars = 0;
			int cchSize = 0;
			int cchLastGoodWord = 0;
			int cchLastGoodSize = 0;
			LPCTSTR p = pstrText;
			//LPCTSTR pstrNext;
			SIZE szText = {0};
			RECT rcText = {0};
			if (!bInRaw && *p == _T('<') || *p == _T('{'))
			{
				p++, cchChars++, cchSize++;
			}
			while (*p != _T('\0') && *p != _T('\n')) 
			{
				// This part makes sure that we're word-wrapping if needed or providing support for DT_END_ELLIPSIS. 
				// Unfortunately the GetTextExtentPoint32() call is pretty slow when repeated so often.
				// TODO: Rewrite and use GetTextExtentExPoint() instead!
				if (bInRaw)
				{
					if (( *p == _T('<') || *p == _T('{') ) && p[1] == _T('/') 
&& p[2] == _T('r') && (p[3] == _T('>') || p[3] == _T('}')))
					{
						p += 4;
						bInRaw = false;
						break;
					}
				}
				else
				{
					if (*p == _T('<') || *p == _T('{')) // 碰到这两种字符退出
					{
						break;
					}
				}
				LPCTSTR pstrNext = ::CharNext(p);
				cchChars++;
				cchSize += (int)(pstrNext - p);
				szText.cx = cchChars * textMetric.tmMaxCharWidth;
				if (pt.x + szText.cx >= rc.right /*-textMetric.tmMaxCharWidth*/)
				{
					pCurFontObj->CalcText(hDC, rcText, pstrText, DT_CALC_SINGLELINE, nFontType, cchSize);
					szText.cx = rcText.right - rcText.left;
					szText.cy = rcText.bottom - rcText.left;
				}
				if (pt.x + szText.cx > rc.right)
				{
					if (pt.x + szText.cx > rc.right && pt.x != rc.left)
					{
						cchChars--;
						cchSize -= (int)(pstrNext - p);
					}
					if ((uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0)
					{
						cchChars = cchLastGoodWord;
						cchSize = cchLastGoodSize;
					}
					if ((uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 0 && ((ptPos.y + cyLineHeight * 2 > rc.bottom) || (uStyle & DT_SINGLELINE) != 0))
					{
						cchChars -= 1;
						LPCTSTR pstrPrev = ::CharPrev(pstrText, p);
						if (cchChars > 0)
						{
							cchChars -= 1;
							pstrPrev = ::CharPrev(pstrText, pstrPrev);
							cchSize -= (int)(p - pstrPrev);
						}
						else
						{
							cchSize -= (int)(p - pstrPrev);
						}
						pt.x = rc.right;
					}
					bLineEnd = true;
					cxMaxWidth = max(cxMaxWidth, pt.x);
					break;
				}
				if (!((p[0] >= _T('a') && p[0] <= _T('z')) || (p[0] >= _T('A') && p[0] <= _T('Z'))))
				{
					cchLastGoodWord = cchChars;
					cchLastGoodSize = cchSize;
				}
				if (*p == _T(' '))
				{
					cchLastGoodWord = cchChars;
					cchLastGoodSize = cchSize;
				}
				p = ::CharNext(p);
			}

			pCurFontObj->CalcText(hDC, rcText, pstrText, DT_CALC_SINGLELINE, nFontType, cchSize);
			szText.cx = rcText.right - rcText.left;
			szText.cy = rcText.bottom - rcText.left;

			if (bDraw && bLineDraw)
			{
				if ((uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_CENTER) != 0)
				{
					ptPos.x += (rc.right - rc.left - szText.cx) / 2;
				}
				else if ((uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_RIGHT) != 0)
				{
					ptPos.x += (rc.right - rc.left - szText.cx);
				}

				/*if ((uStyle & DT_SINGLELINE) != 0)
				{
					int margin = (rc.bottom - rc.top - cyLineHeight) / 2;
					margin = margin >= 0 ? margin : 0;
					ptPos.y = ptPos.y + margin;
				}*/

				if (ptPos.y + cyLineHeight <= rc.bottom)
				{
					dwPreLineTextColor = dwCurTextColor;
					paintParams.nHeight = (pt.y+cyLineHeight) - rc.top;
					DWORD dwTextColor = add_alpha_to_color(dwCurTextColor, alpha);
					this->TextOut(pCurFontObj, ptPos.x, ptPos.y + cyLineHeight - textMetric.tmHeight - textMetric.tmExternalLeading, pstrText, cchSize, dwTextColor, pCurBkColor, nFontType);

	
					if (pt.x >= rc.right && (uStyle & DT_END_ELLIPSIS) != 0)
					{
						if ((uStyle & DT_SINGLELINE))
						{
							this->TextOut(pCurFontObj, ptPos.x + szText.cx, ptPos.y, _T("..."), 3, dwTextColor, NULL, nFontType);
						}
						else if (ptPos.y + cyLineHeight * 2 > rc.bottom)
						{
							this->TextOut(pCurFontObj, ptPos.x + szText.cx, ptPos.y, _T("..."), 3, dwTextColor, NULL, nFontType);
						}

					}
				}
			}
			pt.x += szText.cx;
			cxMaxWidth = max(cxMaxWidth, pt.x);
			pstrText += cchSize;
		}

		if (pt.x >= rc.right || *pstrText == _T('\n') || *pstrText == _T('\0')) bLineEnd = true;
		if (bDraw && bLineEnd)
		{
			if (!bLineDraw)
			{
				aFontArray.Resize(aLineFontArray.GetSize());
				::CopyMemory(aFontArray.GetData(), aLineFontArray.GetData(), aLineFontArray.GetSize() * sizeof(LPVOID));
				aColorArray.Resize(aLineColorArray.GetSize());
				::CopyMemory(aColorArray.GetData(), aLineColorArray.GetData(), aLineColorArray.GetSize() * sizeof(LPVOID));
				aPIndentArray.Resize(aLinePIndentArray.GetSize());
				::CopyMemory(aPIndentArray.GetData(), aLinePIndentArray.GetData(), aLinePIndentArray.GetSize() * sizeof(LPVOID));

				cyLineHeight = cyLine;
				pstrText = pstrLineBegin;
				bInRaw = bLineInRaw;
				bInSelected = bLineInSelected;

				DWORD clrColor = dwDefaultColor;
				if (aColorArray.GetSize() > 0)
				{
					clrColor = (int)aColorArray.GetAt(aColorArray.GetSize()-1);
				}
				dwTextColor = clrColor;
				FontObj* pFontObj = (FontObj *)aFontArray.GetAt(aFontArray.GetSize()-1);
				pFontObj = pFontObj!=NULL ? pFontObj:pFontObjDefault;
				textMetric = pFontObj->GetTextMetric(hDC);
				pCurFontObj = pFontObj;
				if (bInSelected)
				{
					pCurBkColor = &dwBkColor;
				}
			}
			else
			{
				aLineFontArray.Resize(aFontArray.GetSize());
				::CopyMemory(aLineFontArray.GetData(), aFontArray.GetData(), aFontArray.GetSize()*sizeof(LPVOID));
				aLineColorArray.Resize(aColorArray.GetSize());
				::CopyMemory(aLineColorArray.GetData(), aColorArray.GetData(), aColorArray.GetSize()*sizeof(LPVOID));
				aLinePIndentArray.Resize(aPIndentArray.GetSize());
				::CopyMemory(aLinePIndentArray.GetData(), aPIndentArray.GetData(), aPIndentArray.GetSize()*sizeof(LPVOID));
				pstrLineBegin = pstrText;
				bLineInSelected = bInSelected;
				bLineInRaw = bInRaw;

				iLineNum++;
				
			}
		}
	}

	nLinkRects = iLinkIndex;
	if ((uStyle&DT_CALCRECT) != 0)
	{
		rc.bottom = max(cyMinHeight, pt.y + cyLine);
		rc.right = min(rc.right, cxMaxWidth);
	}

	int nFontSize = (int)vecFonts.size();
	for (int i=0; i < nFontSize; i++)
	{
		FontObj *pFontObj = vecFonts[i];
		delete pFontObj;
		pFontObj = NULL;
	}
	vecFonts.clear();

	paintParams.nLineNum = iLineNum;

	if (bDraw)
	{
		::SelectClipRgn(hDC, hOldRgn);
	}
	::DeleteObject(hOldRgn);
	::DeleteObject(hRgn);

	if (GetEnableAlpha() && GetDibObj() && (nFontType == UIFONT_GDI) && ((uStyle&DT_CALCRECT) == 0))
	{
		CSSE::DecreaseAlpha(GetDibObj()->GetBits(), GetDibObj()->GetWidth(), GetDibObj()->GetHeight(),  rcCalcClipBox);
	}

	if ((uStyle&DT_CALCRECT) != 0)
	{
		rcDst = rc;
	}
}

void IRenderDC::DrawImageRotate(ImageObj* pImageObj, RECT& rcDst, RECT& rcSrc, PaintParams* params /*= NULL*/)
{
	if (!params->bRotate) return;
	if (!pImageObj) return;
	if (IsRectEmpty(&rcDst) || IsRectEmpty(&rcSrc)) return;

	RECT rcPaint = IsRectEmpty(&m_rectUpdate) ? rcDst : m_rectUpdate;

	HDC hDestDC = GetDC();

	RECT rectClip;
	if (!IntersectRect(&rectClip, &rcPaint, &rcDst)) return;
	::GetClipBox(hDestDC, &rectClip);
	if (!IntersectRect(&rectClip, &rcPaint, &rectClip)) return;
	{
#ifdef _GDIPLUS_SUPPORT
		bool bDelete = false;
		Bitmap* pBitmap = pImageObj->GetBitmap();
		if (!pBitmap)
		{
			HBITMAP hBitmap = pImageObj->GetHBitmap();
			if (hBitmap)
				pBitmap = CImageLib::HBITMAPToBitmap(hBitmap);
			else
			{
				BYTE* pPixels = pImageObj->GetPixels();
				if (pPixels)
					pBitmap = CImageLib::PixelsToBitmap(pPixels, pImageObj->GetWidth(), pImageObj->GetHeight());
			}
			if (pBitmap)
				bDelete = true;
		}
		if (pBitmap)
		{
			Graphics g(GetDC());
			g.SetClip(Rect(rectClip.left, rectClip.top, rectClip.right - rectClip.left, rectClip.bottom - rectClip.top));
			if (!params || params->eSmoothingMode == UISMOOTHING_HighSpeed)
			{
				g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);
				g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
				g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
			}
			else
			{
				g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
				g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);

			}

			if (params && (pImageObj->GetFrameCount() > 1)) pImageObj->SetActiveFrame(params->nActiveFrame);

			bool bEnableHDPI = params ? params->bEnableHDPI : true;

			int orgX =(rcDst.right + rcDst.left) / 2 ;
			int orgY = (rcDst.bottom + rcDst.top) / 2;

			Matrix rotateMatrix;
			rotateMatrix.RotateAt((REAL)params->nRotateAngle,PointF((REAL)orgX, (REAL)orgY), MatrixOrderAppend);
			g.SetTransform(&rotateMatrix);

			RectF rtDest((REAL)rcDst.left, (REAL)rcDst.top, (REAL)(rcDst.right - rcDst.left), (REAL)(rcDst.bottom - rcDst.top));
			g.DrawImage(pBitmap,
				rtDest,
				(REAL)rcSrc.left, (REAL)rcSrc.top, (REAL)(rcSrc.right - rcSrc.left), (REAL)(rcSrc.bottom - rcSrc.top),
				UnitPixel);

			if (bDelete)
				delete pBitmap;

			return;
		}
#endif
	}
}

void IRenderDC::DrawImageAngle( ImageObj* pImageObj, RECT& rcDst, RECT& rcSrc, PaintParams* params /*= NULL*/ )
{
	if (!params->bAngle) return;
	if (!pImageObj) return;
	if (IsRectEmpty(&rcDst) || IsRectEmpty(&rcSrc)) return;

	RECT rcPaint = IsRectEmpty(&m_rectUpdate) ? rcDst : m_rectUpdate;

	HDC hDestDC = GetDC();

	int nAlpha = 255;
	if (params)
		nAlpha = params->nAlpha;

	RECT rectClip;
	if (!IntersectRect(&rectClip, &rcPaint, &rcDst)) return;
	::GetClipBox(hDestDC, &rectClip);
	if (!IntersectRect(&rectClip, &rcPaint, &rectClip)) return;

	{
#ifdef _GDIPLUS_SUPPORT
		bool bDelete = false;
		Bitmap* pBitmap = pImageObj->GetBitmap();
		if (!pBitmap)
		{
			HBITMAP hBitmap = pImageObj->GetHBitmap();
			if (hBitmap)
				pBitmap = CImageLib::HBITMAPToBitmap(hBitmap);
			else
			{
				BYTE* pPixels = pImageObj->GetPixels();
				if (pPixels)
					pBitmap = CImageLib::PixelsToBitmap(pPixels, pImageObj->GetWidth(), pImageObj->GetHeight());
			}
			if (pBitmap)
				bDelete = true;
		}
		if (pBitmap)
		{

			int eWarpMode = params ? params->eGDIPLUSWarpMode : -1;
			ImageAttributes imAtt;
			if (eWarpMode != -1)
				imAtt.SetWrapMode(WrapMode(eWarpMode));

			if (nAlpha != 255)
			{
				REAL fAlpha = (REAL)nAlpha / 255;
				ColorMatrix colorMartrix = {
					1, 0, 0, 0, 0,
					0, 1, 0, 0, 0,
					0, 0, 1, 0, 0,
					0, 0, 0, fAlpha, 0,
					0, 0, 0, 0, 1 };
				imAtt.SetColorMatrix(&colorMartrix);
			}

			Graphics g(GetDC());
			g.SetClip(Rect(rectClip.left,rectClip.top,rectClip.right - rectClip.left, rectClip.bottom - rectClip.top));
			if (!params ||  params->eSmoothingMode == UISMOOTHING_HighSpeed)
			{
				g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);
				g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
				g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
			}
			else
			{
				g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
				g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);

			}
			if (params && (pImageObj->GetFrameCount() > 1)) pImageObj->SetActiveFrame(params->nActiveFrame);


			int nDestWidth = rcDst.right - rcDst.left;
			int nDestHeight = rcDst.bottom - rcDst.top;

			bool bEnableHDPI = params ? params->bEnableHDPI : true;

			int nPosX = (int)((double)nDestWidth  * (1.0 - sin((90 - params->nAngle) * M_PI / 180.0)) / 2.0);
			int nPosY = (int)((double)DPI_SCALE_EX(params->nAngleHeight, bEnableHDPI) * sin(params->nAngle * M_PI / 180.0));

			Point newpoints[] = {
				Point(rcDst.left + nPosX, rcDst.top + nPosY),
				Point(rcDst.left + nDestWidth-nPosX, rcDst.top-nPosY),
				Point(rcDst.left + nPosX, rcDst.top + nDestHeight + nPosY)};

				g.DrawImage(pBitmap, 
					newpoints, 3,
					rcSrc.left, rcSrc.top, rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top, 
					UnitPixel, &imAtt);

				if (bDelete)
					delete pBitmap;

				return;
		}
#endif
	}
}

void IRenderDC::Line( LPPOINT ptBegin, LPPOINT ptEnd, LineStyleType lineStyle, int lineWidth, DWORD lineColor)
{

	//if (lineColor < 0xFF000000)
	//	lineColor+= 0xFF000000; // 以后颜色全面支持透明度以后要把 “|| color <= 0x00FFFFFF”注销掉

	BYTE a = LOBYTE((lineColor)>>24);
	/*if ((!GetEnableAlpha() && (a == 255))) 
	{
		
	}
	else*/
	{

		Graphics graphics(GetDC());
		Pen pen(Gdiplus::Color(a,GetRValue(lineColor),GetGValue(lineColor), GetBValue(lineColor)), (REAL)lineWidth);
		pen.SetDashStyle((DashStyle)lineStyle);
		graphics.DrawLine(&pen, ptBegin->x, ptBegin->y, ptEnd->x, ptEnd->y);
	}

	//if (nStyle == PS_DOT)
	//{
	//	HDC hDC = GetDC();
	//	LOGBRUSH logBrush;
	//	logBrush.lbColor = dwColor;
	//	logBrush.lbStyle = BS_SOLID;
	//	HPEN hPen = ExtCreatePen(PS_COSMETIC | PS_ALTERNATE, nWidth, &logBrush, 0, NULL);
	//	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	//	POINT ptOld;
	//	MoveToEx(hDC, ptBegin->x, ptBegin->y, &ptOld);
	//	LineTo(hDC, ptEnd->x, ptEnd->y);
	//	MoveToEx(hDC, ptOld.x, ptOld.y, NULL);
	//	(LPVOID)SelectObject(hDC, hOldPen);
	//	DeleteObject(hPen);
	//}
	//else
	//{
	//	HDC hDC = GetDC();
	//	HPEN hPen = CreatePen(nStyle, nWidth, dwColor);
	//	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	//	POINT ptOld;
	//	MoveToEx(hDC, ptBegin->x, ptBegin->y, &ptOld);
	//	LineTo(hDC, ptEnd->x, ptEnd->y);
	//	MoveToEx(hDC, ptOld.x, ptOld.y, NULL);
	//	(LPVOID)SelectObject(hDC, hOldPen);
	//	DeleteObject(hPen);
	//}
}
///////////////////////////////////////////////////////////////////////////
DibObj::DibObj()
{
	m_hMemDC = NULL;
	m_hMemBmp = NULL;
	m_hOldBmp = NULL;
	m_pBits = NULL;
	memset(&m_bmpHeader, 0, sizeof(m_bmpHeader));
	m_bmpHeader.biSize = sizeof(m_bmpHeader);
	m_bmpHeader.biBitCount = 32;
	m_bmpHeader.biCompression = BI_RGB;
	m_bmpHeader.biPlanes = 1;
}

DibObj::~DibObj(void)
{
	Clear();
}

BOOL DibObj::Create(HDC hdc, int cx, int cy, UINT usage)
{
	if(m_hMemDC)
		Clear();

	m_hMemDC = CreateCompatibleDC(hdc);
	
	//m_bmpHeader.biSizeImage = cx * cy * sizeof(DWORD);
	m_bmpHeader.biWidth = cx;
	m_bmpHeader.biHeight = -cy;
	m_hMemBmp = CreateDIBSection(hdc, (BITMAPINFO *)&m_bmpHeader, usage, (void **)&m_pBits, NULL, 0);
	//m_hMemBmp = ::CreateCompatibleBitmap(hdc, cx, cy); 
	if(m_hMemBmp == NULL)
		return false;
	m_hOldBmp = (HBITMAP)SelectObject(m_hMemDC, m_hMemBmp);
	return TRUE;	
}

void DibObj::Clear()
{
	if (m_hMemDC)
	{
		if (m_hOldBmp)
			SelectObject(m_hMemDC, m_hOldBmp);
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}

	if(m_hMemBmp)
	{
		DeleteObject(m_hMemBmp);
		m_hMemBmp = NULL;
		m_pBits = NULL;
	}

}

HBITMAP DibObj::Detach()
{
	if(m_hMemDC)
	{
		if(m_hOldBmp)
			SelectObject(m_hMemDC, m_hOldBmp);
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;

	}

	HBITMAP hMemBmp = m_hMemBmp;
	m_hMemBmp = NULL;

	return hMemBmp;
	
}

int DibObj::GetSize()
{
	if(m_pBits == NULL)
		return 0;

	return abs(-m_bmpHeader.biWidth * m_bmpHeader.biHeight * 4);
}

int DibObj::GetWidth()
{
	if(m_pBits == NULL)
		return 0;

	return m_bmpHeader.biWidth;
}
int DibObj::GetHeight()
{
	if(m_pBits == NULL)
		return 0;

	return abs(-m_bmpHeader.biHeight);
}
void DibObj::Fill(DWORD dwColor, DWORD dwAlpha)
{
	int R = ((dwColor & 0xFF0000 )>>16) * dwAlpha / 255;  //把蓝色和红色对调
	int G = ((dwColor & 0x00FF00 )>>8) * dwAlpha / 255;
	int B =  (dwColor & 0x0000FF) * dwAlpha / 255;
	dwColor = RGB(R,G, B);
	dwColor |= dwAlpha << 24;
	CSSE::MemSetDWord(GetBits(), dwColor, GetSize());
}

void DibObj::Fill(RECT& rect, COLORREF dwColor, DWORD dwAlpha)
{
	int R = ((dwColor & 0xFF0000 )>>16) * dwAlpha / 255;  //把蓝色和红色对调
	int G = ((dwColor & 0x00FF00 )>>8) * dwAlpha / 255;
	int B =  (dwColor & 0x0000FF) * dwAlpha / 255;
	dwColor = RGB(R,G, B);
	dwColor |= dwAlpha << 24;

	BYTE * pSrc = (BYTE *)GetBits();
	int width = GetWidth();
	int widebytes = rect.right - rect.left;
	widebytes <<= 2;
	for (int i = rect.top; i < rect.bottom; i++)
	{
		int start = width * i  + rect.left;
		start <<= 2;
		BYTE * p = pSrc + start;
		CSSE::MemSetDWord(p, dwColor, widebytes);
	}
}

void DibObj::DoAlpha(RECT &rect)
{
	CSSE::DoAlpha(GetBits(), GetWidth(), rect);
}

//
//} // dui命名空间