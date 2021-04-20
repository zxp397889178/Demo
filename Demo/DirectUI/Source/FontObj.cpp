#include "stdafx.h"
#include "FontObj.h"

static std::map<tstring, int> gs_mapFontSet;
//////////////////////////////////////////////////////////////////////////////////
// 枚举字体
int CALLBACK EnumFamScreenCallBackEx(CONST LOGFONTW * lplf,  CONST VOID * lpntm, DWORD FontType, LPARAM lpParam)
{
	gs_mapFontSet[lplf->lfFaceName] = 0;
	return 1; // Call me back	
}
//判定字体是否存在
bool FontObj::IsExistFontName(LPCTSTR lpszFontName)
{
	static bool bInit = true;
	if (bInit)
	{
		//获取当前全部字体
		bInit = false;
		HDC hDC;
		// Get screen fonts
		hDC = ::GetDC(NULL);

		LOGFONT lf;

		ZeroMemory(&lf,sizeof(lf));
		lf.lfCharSet = DEFAULT_CHARSET;

		EnumFontFamiliesEx(
			hDC,	// handle to device context
			&lf,	// pointer to logical font information
			(FONTENUMPROC)EnumFamScreenCallBackEx,	// pointer to callback function
			(LPARAM)0,	// application-supplied data
			(DWORD) 0);

		::ReleaseDC(NULL,hDC);	
		
	}
	if (gs_mapFontSet.find(lpszFontName) != gs_mapFontSet.end())
		return true;
	return false;

}
///////////////////////////////////////////////////////////////////
FontObj::FontObj()
{
	memset(&m_lfFont, 0, sizeof(LOGFONT));
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &m_lfFont);
	m_lfFont.lfCharSet = DEFAULT_CHARSET;
	m_lfFont.lfHeight = -12;
	m_nTextRenderingHint = TextRenderingHintClearTypeGridFit;
	m_bCached = false;
	m_pGdiplusFont = NULL;
	m_hFont = NULL;
	m_bUsedDefalultFontFace = false;
}

FontObj::~FontObj()
{
	Release();
}

void FontObj::Release()
{
	if (m_pGdiplusFont)
		delete m_pGdiplusFont;
	m_pGdiplusFont = NULL;

	if (m_hFont)
		::DeleteObject(m_hFont);
	m_hFont = NULL;
}


void FontObj::RebuildFont()
{
	if (IsUsedDefaultFaceName())
	{
		_tcscpy(m_lfFont.lfFaceName, GetUIRes()->GetDefaultFontName());
	}


	if (m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
		GetFont();
	}

	if (m_pGdiplusFont)
	{
		delete m_pGdiplusFont;
		m_pGdiplusFont = NULL;
		GetGdiplusFont();
	}

}

FontObj::FontObj(FontObj& obj)
{
	m_strId = obj.m_strId;
	m_lfFont = obj.m_lfFont;
	m_nTextRenderingHint = TextRenderingHintClearTypeGridFit;
	m_bCached = false;
	m_pGdiplusFont = NULL;
	m_hFont = NULL;
	m_bUsedDefalultFontFace = false;
}


void  FontObj::SetFaceName(LPCTSTR lpszFace)
{
	//判定设定的字体是否存在
	if (_tcsicmp(m_lfFont.lfFaceName, lpszFace) == 0)
		return;

	tstring strFaceName;
	if (_tcsicmp(lpszFace, _T("")) == 0 || _tcsicmp(lpszFace, _T("default")) == 0)
	{
		strFaceName = GetUIRes()->GetDefaultFontName();
		SetUsedDefaultFaceName(true);
	}
	else if (IsExistFontName(lpszFace))
	{
		strFaceName = lpszFace;
	}
	else
	{
		strFaceName = GetUIRes()->GetDefaultFontName();
		SetUsedDefaultFaceName(true);
	}
	
	_tcscpy(m_lfFont.lfFaceName, strFaceName.c_str());
	Release();
}

LPCTSTR FontObj::GetFaceName()
{
	return m_lfFont.lfFaceName;
}

bool FontObj::IsUsedDefaultFaceName() // 如果使用默认字体，字体将随着默认字体改变而改变
{
	return m_bUsedDefalultFontFace;
}

void FontObj::SetUsedDefaultFaceName(bool bUsedDefaultFaceName)
{
	m_bUsedDefalultFontFace = bUsedDefaultFaceName;
}

void  FontObj::SetFontSize(int nSize)
{
	if (m_lfFont.lfHeight != (-nSize))
	{
		m_lfFont.lfHeight = -nSize;
		Release();
	}
}

void  FontObj::SetBold(bool bBold)
{
	if( bBold )
		m_lfFont.lfWeight = FW_BOLD;
	else
		m_lfFont.lfWeight = FW_NORMAL;
	Release();
}

bool FontObj::IsBold()
{
	return m_lfFont.lfWeight == FW_BOLD;
}

void  FontObj::SetUnderline(bool bUnderline)
{
	if( bUnderline )
		m_lfFont.lfUnderline = TRUE;
	else
		m_lfFont.lfUnderline = FALSE;
	Release();
}

bool FontObj::IsUnderline()
{
	return !!m_lfFont.lfUnderline;
}

void  FontObj::SetItalic(bool bItalic)
{
	if( bItalic )
		m_lfFont.lfItalic = TRUE;
	else
		m_lfFont.lfItalic = FALSE;
	Release();
}

bool FontObj::IsStrikeOut()
{
	return m_lfFont.lfStrikeOut == TRUE;
}

void FontObj::SetStrikeOut( bool bStrikeOut )
{
	if (bStrikeOut)
		m_lfFont.lfStrikeOut = TRUE;
	else
		m_lfFont.lfStrikeOut = FALSE;
	Release();
}

bool FontObj::IsItalic()
{
	return !!m_lfFont.lfItalic;
}

HFONT FontObj::GetFont()
{
	if (!m_hFont)
	{
		LONG lfHeight = m_lfFont.lfHeight;
		m_lfFont.lfHeight = DPI_SCALE(m_lfFont.lfHeight);
		m_hFont = ::CreateFontIndirect(&m_lfFont);
		m_lfFont.lfHeight = lfHeight;
	}
	return m_hFont;
}

void FontObj::GetLogFont(LPLOGFONT lpLogFont)
{
	memcpy(lpLogFont, &m_lfFont, sizeof(LOGFONT));
}

void  FontObj::SetLogFont(const LPLOGFONT lpLogFont)
{
	Release();
	memcpy(&m_lfFont, lpLogFont, sizeof(LOGFONT));
}

TEXTMETRIC FontObj::GetTextMetric(HDC hDC)
{
	TEXTMETRIC tmTextMertric;
	memset(&tmTextMertric, 0, sizeof(TEXTMETRIC));
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetFont());
	::GetTextMetrics(hDC, &tmTextMertric);
	::SelectObject(hDC, hOldFont);
	return tmTextMertric;
}

Gdiplus::Font* FontObj::GetGdiplusFont()
{
	if (!m_pGdiplusFont)
		m_pGdiplusFont = new Gdiplus::Font(hdcGrobal, GetFont());
	return m_pGdiplusFont;
}

int  FontObj::GetHeight()
{
	return - m_lfFont.lfHeight;
}


void  FontObj::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, UINT format, UITYPE_FONT nFontType, int c)
{
	if (nFontType == UIFONT_DEFAULT)
		nFontType = GetResObj()->GetDefaultFontType();
	if (nFontType == UIFONT_GDI)
	{
		HFONT hFont = GetFont();
		HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);

		if ((DT_SINGLELINE & format))
		{
			SIZE size = {0};
			::GetTextExtentExPoint(hdc, lpszText, c == -1 ? _tcslen(lpszText) : c, 0, NULL, NULL, &size);
			rc.right = rc.left + size.cx;
			rc.bottom = rc.top + size.cy;
		}
		else
		{
			format &= ~DT_END_ELLIPSIS;
			format &= ~DT_PATH_ELLIPSIS;
			if (!(DT_SINGLELINE & format)) format |= DT_WORDBREAK | DT_EDITCONTROL;
			::DrawText(hdc, lpszText, c, &rc, format | DT_CALCRECT);
		}
		::SelectObject(hdc, hOldFont);
	}
	else if (nFontType == UIFONT_GDIPLUS)
	{
		StringFormat stringformat = StringFormat::GenericTypographic();
		INT nFlags = stringformat.GetFormatFlags();

		if (format & DT_CENTER)
			stringformat.SetAlignment(StringAlignmentCenter);
		else if (format & DT_RIGHT)
			stringformat.SetAlignment(StringAlignmentFar);

		if (format & DT_VCENTER)
			stringformat.SetLineAlignment(StringAlignmentCenter);
		else if (format & DT_BOTTOM)
			stringformat.SetLineAlignment(StringAlignmentFar);

		if ((format & DT_SINGLELINE))
			stringformat.SetFormatFlags(nFlags | StringFormatFlagsNoWrap);
		else
			stringformat.SetFormatFlags(nFlags & ~StringFormatFlagsNoWrap);

		if (format & DT_NOPREFIX)
			stringformat.SetHotkeyPrefix(HotkeyPrefixNone);
		else
			stringformat.SetHotkeyPrefix(HotkeyPrefixShow);

		Gdiplus::PointF pt((float)rc.left, (float)rc.top);
		RectF boundingBox;
		graphicsGlobal->SetTextRenderingHint((Gdiplus::TextRenderingHint)m_nTextRenderingHint);
		if (IsRectEmpty(&rc) && rc.right - rc.left <= 0 || (format & DT_SINGLELINE))
		{
			Gdiplus::PointF pt((float)rc.left, (float)rc.top);
			graphicsGlobal->MeasureString(lpszText, c, GetGdiplusFont(), pt, &stringformat, &boundingBox);
		}
		else
		{
			Gdiplus::RectF rectF(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			graphicsGlobal->MeasureString(lpszText, -1, GetGdiplusFont(), rectF, &stringformat, &boundingBox);
		}
		rc.right = rc.left + (int)ceil(boundingBox.Width);
		rc.bottom = rc.top + (int)ceil(boundingBox.Height);
	}
}

void FontObj::TextOut(IRenderDC* pRenderDC,  int x, int y, LPCTSTR lpszText, int c, DWORD dwColor, DWORD* pBkColor, UITYPE_FONT nFontType)
{
	if (nFontType == UIFONT_DEFAULT)
		nFontType = GetResObj()->GetDefaultFontType();
	c = (c == -1 ? _tcslen(lpszText): c);
	HDC hdc = pRenderDC->GetDC();
	int nOldMode;
	if (!pBkColor)
		nOldMode = ::SetBkMode(hdc, TRANSPARENT);
	else
		nOldMode = ::SetBkMode(hdc, OPAQUE);

	if (nFontType == UIFONT_GDI)
	{
		DWORD crOldBkColor;
		if (pBkColor)
			crOldBkColor = ::SetBkColor(hdc, *pBkColor);
		
		HFONT hFont = GetFont();
		HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);
		COLORREF crOldColor = ::SetTextColor(hdc, DUI_ARGB2RGB(dwColor));
		::TextOut(hdc, x, y, lpszText, c);

		::SetTextColor(hdc, crOldColor);
		::SelectObject(hdc, hOldFont);
		if (pBkColor)
			::SetBkColor(hdc, crOldBkColor);

	}
	else if (nFontType == UIFONT_GDIPLUS)
	{
		BYTE a = (dwColor & 0xFF000000) >> 24;
		Color clr(a,GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor));
		SolidBrush textBrush(clr);
		Graphics g(hdc);
		if (pBkColor)
		{
			Color clrBk(192,GetRValue(*pBkColor),GetGValue(*pBkColor),GetBValue(*pBkColor));
			SolidBrush bkBrush(clrBk);
			RECT rect={x, y, x, y};
			CalcText(hdc, rect, lpszText, DT_CALC_SINGLELINE, nFontType, c);
			g.FillRectangle(&bkBrush, RectF(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top));
		}
		// GDI+ 使用TextRenderingHintClearTypeGridFit等 绘制字体时，在透明背景下有问题，故强制采用TextRenderingHintAntiAlias
		if (GetUIRes()->GetDefaultTextRenderingHint() == TextRenderingHintSystemDefault)
			g.SetTextRenderingHint((Gdiplus::TextRenderingHint)m_nTextRenderingHint);
		else
			g.SetTextRenderingHint((Gdiplus::TextRenderingHint)GetUIRes()->GetDefaultTextRenderingHint());
		g.DrawString(lpszText, c, GetGdiplusFont(),PointF(x, y),&textBrush);

	}
	::SetBkMode(hdc, nOldMode);
}

void  FontObj::DrawText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format, UITYPE_FONTEFFECT nFontEffect, UITYPE_FONT nFontType)
{
	if (nFontType == UIFONT_DEFAULT)
		nFontType = GetResObj()->GetDefaultFontType();

	HDC hdc = pRenderDC->GetDC();
	if (nFontType == UIFONT_GDI)
	{
		switch (nFontEffect)
		{
		case UIFONTEFFECT_NORMAL:
			{
				RECT clipBox;
				DibObj* pDibObj = pRenderDC->GetDibObj();
				bool bNeedChange = pRenderDC->GetEnableAlpha() && pDibObj;
				if (bNeedChange)
				{
					_CalcClipBox(pRenderDC, rc, clipBox);
					CSSE::IncreaseAlpha(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(), clipBox);
				}

				int nOldMode = ::SetBkMode(hdc, TRANSPARENT);
				HFONT hFont = GetFont();
				HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);
				COLORREF crOldColor = ::SetTextColor(hdc, DUI_ARGB2RGB(dwColor));
				//add by hanzp. for multiline text. VAlign : center
				RECT rcDraw = rc;
				if (format & DT_VCENTER)
				{
					if (!(format & DT_SINGLELINE))
					{
						::DrawText(hdc, lpszText, -1, &rcDraw, format | DT_CALCRECT);
						int nHeight = (rc.bottom - rcDraw.bottom)/2;
						rcDraw = rc;
						if (nHeight > 0)
							rcDraw.top += nHeight;
					}
				}
				::DrawText(hdc, lpszText, -1, &rcDraw, format);

				::SetTextColor(hdc, crOldColor);
				::SelectObject(hdc, hOldFont);
				::SetBkMode(hdc, nOldMode);

				if (bNeedChange)
					CSSE::DecreaseAlpha(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(),  rc);
			}
			break;
		default:
			{
				DrawGrowText(pRenderDC, rc, lpszText, dwColor, format, nFontType, nFontEffect);
			}
			break;
		}
	}
	else if (nFontType == UIFONT_GDIPLUS)
	{
		switch (nFontEffect)
		{
		case UIFONTEFFECT_NORMAL:
			{
				DrawGDIPlusText(pRenderDC, rc, lpszText, dwColor, format, nFontEffect);
			}
			break;
		default:
			{
				DrawGrowText(pRenderDC, rc, lpszText, dwColor, format, nFontType, nFontEffect);
			}
			break;
		}

	}
	
}

void FontObj::DrawShadowText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwFontColor, UINT format, DWORD dwShadowColor, UINT uiShadowSize)
{
	HDC hdc = pRenderDC->GetDC();
	int nOldMode = ::SetBkMode(hdc, TRANSPARENT);
	RECT rcDraw = rc;
	Rect rect(rcDraw.left, rcDraw.top, (rcDraw.right - rcDraw.left), (rcDraw.bottom - rcDraw.top));

	StringFormat stringformat = StringFormat::GenericTypographic();
	INT nFlags = stringformat.GetFormatFlags();

	if (format & DT_CENTER)
		stringformat.SetAlignment(StringAlignmentCenter);
	else if (format & DT_RIGHT)
		stringformat.SetAlignment(StringAlignmentFar);

	if (format & DT_VCENTER)
		stringformat.SetLineAlignment(StringAlignmentCenter);
	else if (format & DT_BOTTOM)
		stringformat.SetLineAlignment(StringAlignmentFar);

	if (format & DT_END_ELLIPSIS)
		stringformat.SetTrimming(StringTrimmingEllipsisCharacter);
	else if (format & DT_PATH_ELLIPSIS)
		stringformat.SetTrimming(StringTrimmingEllipsisPath);

	if ((format & DT_SINGLELINE) || !(format & DT_WORDBREAK))
		stringformat.SetFormatFlags(nFlags | StringFormatFlagsNoWrap);
	else
		stringformat.SetFormatFlags(nFlags & ~StringFormatFlagsNoWrap);

	if (format & DT_NOPREFIX)
		stringformat.SetHotkeyPrefix(HotkeyPrefixNone);
	else
		stringformat.SetHotkeyPrefix(HotkeyPrefixShow);

	Graphics g(hdc);

	// GDI+ 使用TextRenderingHintClearTypeGridFit等 绘制字体时，在透明背景下有问题，故强制采用TextRenderingHintAntiAlias
	if (GetUIRes()->GetDefaultTextRenderingHint() == TextRenderingHintSystemDefault)
		g.SetTextRenderingHint((Gdiplus::TextRenderingHint)m_nTextRenderingHint);
	else
		g.SetTextRenderingHint((Gdiplus::TextRenderingHint)GetUIRes()->GetDefaultTextRenderingHint());

	GraphicsPath path;
	FontFamily ftFamily;
	Gdiplus::Font *txtFont = GetGdiplusFont();
	txtFont->GetFamily(&ftFamily);
	Status status = path.AddString(lpszText, -1, &ftFamily, txtFont->GetStyle(), txtFont->GetSize(), rect, &stringformat);

	if (status == S_OK)
	{
		BYTE a = (dwShadowColor & 0xFF000000) >> 24;
		Color clrShaDow(a, GetRValue(dwShadowColor), GetGValue(dwShadowColor), GetBValue(dwShadowColor));

		for (int i = 1; i <= uiShadowSize; i += 1)
		{
			Pen pen(clrShaDow, i);
			pen.SetLineJoin(LineJoinRound);
			g.DrawPath(&pen, &path);
		}
	}

	::SetBkMode(hdc, nOldMode);

	this->DrawGDIPlusText(pRenderDC, rc, lpszText, dwFontColor, format, UITYPE_FONTEFFECT::UIFONTEFFECT_NORMAL);
}

void  FontObj::DrawGDIPlusText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format,  UITYPE_FONTEFFECT nFontEffect)
{
	HDC hdc = pRenderDC->GetDC();
	int nOldMode = ::SetBkMode(hdc, TRANSPARENT);
	RECT rcDraw = rc;
	RectF rectF((Gdiplus::REAL)(rcDraw.left), (Gdiplus::REAL)rcDraw.top, (Gdiplus::REAL)(rcDraw.right - rcDraw.left), (Gdiplus::REAL)(rcDraw.bottom - rcDraw.top));

	StringFormat stringformat = StringFormat::GenericTypographic();
	INT nFlags = stringformat.GetFormatFlags();
	if (format & DT_CENTER)
		stringformat.SetAlignment(StringAlignmentCenter);
	else if (format & DT_RIGHT)
		stringformat.SetAlignment(StringAlignmentFar);
	
	if (format & DT_VCENTER)
		stringformat.SetLineAlignment(StringAlignmentCenter);
	else if (format & DT_BOTTOM)
		stringformat.SetLineAlignment(StringAlignmentFar);

	if (format & DT_END_ELLIPSIS)
		stringformat.SetTrimming(StringTrimmingEllipsisCharacter);
	else if (format & DT_PATH_ELLIPSIS)
		stringformat.SetTrimming(StringTrimmingEllipsisPath);

	if ((format & DT_SINGLELINE) || !(format & DT_WORDBREAK))
		stringformat.SetFormatFlags(nFlags | StringFormatFlagsNoWrap);
	else
		stringformat.SetFormatFlags(nFlags & ~StringFormatFlagsNoWrap);

	if (format & DT_NOPREFIX)
		stringformat.SetHotkeyPrefix(HotkeyPrefixNone);
	else
		stringformat.SetHotkeyPrefix(HotkeyPrefixShow);

	BYTE a =  (dwColor & 0xFF000000) >> 24;
	Color clr (a, GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor));

	SolidBrush textBrush(clr);
	Graphics g(hdc);

	// GDI+ 使用TextRenderingHintClearTypeGridFit等 绘制字体时，在透明背景下有问题，故强制采用TextRenderingHintAntiAlias
	if (GetUIRes()->GetDefaultTextRenderingHint() == TextRenderingHintSystemDefault)
		g.SetTextRenderingHint((Gdiplus::TextRenderingHint)m_nTextRenderingHint);
	else
		g.SetTextRenderingHint((Gdiplus::TextRenderingHint)GetUIRes()->GetDefaultTextRenderingHint());

	g.DrawString(lpszText, -1, GetGdiplusFont(), rectF, &stringformat, &textBrush);
	::SetBkMode(hdc, nOldMode);
}

void FontObj::DrawGrowText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format, UITYPE_FONT nFontType, UITYPE_FONTEFFECT nFontEffect)
{
	//qc 21462 modify by hanzp. 编码遗漏DT_NOPREFIX
	HDC hdc = pRenderDC->GetDC();
	bool bWhiteHalo = true;
	HFONT hFont = this->GetFont();

	//modify by hanzp.
	COLORREF clrSkin = GetEngineObj()->GetSkinColor();
	double H, S, L;
	CSSE::RGBtoHSL(clrSkin, &H, &S, &L);
	if (L <= 0.500)
		bWhiteHalo = false;

	static DibObj dibObj1, dibObj2;

	int width=0;
	int height=0;

	if(dibObj1.GetSafeHdc() == NULL)
	{
#ifdef _GDIPLUS_SUPPORT
		void* pImageBits=NULL;
		ImageObj* pImageObj = GetResObj()->LoadImage(_T("#textglow"), false, UIIAMGE_BITMAP);
		if (!pImageObj) return;
		Bitmap* pimage= pImageObj->GetBitmap();
		width= pimage->GetWidth();
		height = pimage->GetHeight();
		Rect rect(0,0,width,height);
		BitmapData bmpdata={0};
		Status status = pimage->LockBits(&rect, ImageLockModeWrite|ImageLockModeRead,
			PixelFormat32bppARGB, &bmpdata);
		if(status != Ok) return;
		pImageBits = bmpdata.Scan0;
		dibObj1.Create(NULL, width, height);
		BYTE* pBits = dibObj1.GetBits();
		memcpy(pBits, pImageBits, width * height * 4);
		pimage->UnlockBits(&bmpdata);	
		pImageObj->Release();
#endif
	}

	if(dibObj2.GetSafeHdc() == NULL)
	{
#ifdef _GDIPLUS_SUPPORT
		void* pImageBits=NULL;
		ImageObj* pImageObj = GetResObj()->LoadImage(_T("#textglow"), false, UIIAMGE_BITMAP);
		if (!pImageObj) return;
		Bitmap* pimage= pImageObj->GetBitmap();
		width= pimage->GetWidth();
		height = pimage->GetHeight();
		Rect rect(0,0,width,height);
		BitmapData bmpdata={0};
		Status status = pimage->LockBits(&rect, ImageLockModeWrite|ImageLockModeRead,
			PixelFormat32bppPARGB, &bmpdata);
		if(status != Ok) return;
		pImageBits = bmpdata.Scan0;
		dibObj2.Create(NULL, width, height);
		BYTE* pBits = dibObj2.GetBits();
		memcpy(pBits, pImageBits, width * height * 4);
		pimage->UnlockBits(&bmpdata);	
		pImageObj->Release();
#endif
	}

	// 自动改变颜色
	DibObj* pdib = NULL;
	BYTE a = (dwColor & 0xFF000000) >> 24;
	if (nFontEffect == UIFONTEFFECT_GLOW_AUTO)
	{
		if (bWhiteHalo)
			dwColor = DUI_ARGB(a, 0, 0, 0);
		else
			dwColor = DUI_ARGB(a, 255, 255, 255);

		if (bWhiteHalo)
			pdib = &dibObj1;
		else
			pdib = &dibObj2;
	}
	else if (nFontEffect == UIFONTEFFECT_GLOW_GOLD)
	{
		pdib = &dibObj1;
	}
	else
	{
		pdib = &dibObj2;
	}

	width = pdib->GetWidth();
	height = pdib->GetHeight();


	SIZE dibSize={0};
	dibSize.cx=rc.right-rc.left;
	dibSize.cy=rc.bottom-rc.top;


	// 计算文字路径，alpha = 0代表有文字
	DibObj dibpath;
	dibpath.Create(NULL,dibSize.cx, dibSize.cy);

	HFONT hOldFont = (HFONT)::SelectObject(dibpath.GetSafeHdc(),hFont);
	int oldMode = ::SetBkMode(dibpath.GetSafeHdc(),TRANSPARENT);
	dibpath.Fill(RGB(255,255,255), 255);
	
	RECT txtRect={0,0,dibSize.cx,dibSize.cy};
	::DrawText(dibpath.GetSafeHdc(),lpszText,_tcslen(lpszText),&txtRect,format);

	::SelectObject(dibpath.GetSafeHdc(), hOldFont);
	::SetBkMode(dibpath.GetSafeHdc(), oldMode);

	float fz = 0.6;

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	bf.SourceConstantAlpha = 80 * fz * a / 255;
	//if(!bWhiteHalo) bf.SourceConstantAlpha = 80 * fz;
	//else 
	//{
	//	double tmp = fabs(0.5 - L) * 100;
	//	if(tmp > 40) bf.SourceConstantAlpha = 255 * fz;
	//	else if(tmp > 30) bf.SourceConstantAlpha = 215 * fz;
	//	else if(tmp > 20) bf.SourceConstantAlpha = 175 * fz;
	//	else if(tmp > 10) bf.SourceConstantAlpha = 135 * fz;
	//	else bf.SourceConstantAlpha = 95 * fz;
	//}

	

	unsigned char aaa = 255;
	int nWideBytes = dibpath.GetWidth() * 4;
	BYTE* ptr = dibpath.GetBits();
	for ( int j = 0;j < dibSize.cy-1;j++ )
	{
		BYTE* ptr1  = ptr + nWideBytes * j;
		for ( int i = 0;i < dibSize.cx-1;i++ )
		{
			if (ptr1[ i * 4 + 3] != aaa)
			{
				//::AlphaBlend(hdc,rc.left + i-2,rc.top + j-2,width,height,pdib->GetSafeHdc(),width - 4,height - 4,4,4,bf);
				//::AlphaBlend(hdc,rc.left + i-4,rc.top + j-4,width - 3,height - 3,pdib->GetSafeHdc(),0,0,width-3,height-3,bf);
				::AlphaBlend(hdc, rc.left + i - width / 2, rc.top + j - width / 2, width, height, pdib->GetSafeHdc(), 0, 0, width, height, bf);
				//::AlphaBlend(dib2.GetSafeHdc(),i-3,j-3,width-4,height-4,dib1.GetSafeHdc(),2,2,width-4,height-4,bf);
			}
		}
	}
	DrawText(pRenderDC, rc, lpszText, dwColor, format, UIFONTEFFECT_NORMAL, nFontType);
}

void FontObj::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{
	if (equal_icmp(lpszName, _T("face")))
	{
		SetFaceName(lpszValue);
	} 
	else if (equal_icmp(lpszName, _T("height")))
	{
		SetFontSize(_ttoi(lpszValue));
	} 
	else if (equal_icmp(lpszName, _T("bold")))
	{
		SetBold(!!_ttoi(lpszValue));
	} 
	else if (equal_icmp(lpszName, _T("underline")))
	{
		SetUnderline(!!_ttoi(lpszValue));
	} 
	else if (equal_icmp(lpszName, _T("italic")))
	{
		SetItalic(!!_ttoi(lpszValue));
	} 
	else if (equal_icmp(lpszName, _T("strikeout")))
	{
		SetStrikeOut(!!_ttoi(lpszValue));
	} 
	else if (equal_icmp(lpszName, _T("TextRenderHint")))
	{
		int nRenderIndex = _ttoi(lpszValue);
		m_nTextRenderingHint = nRenderIndex % (TextRenderingHintClearTypeGridFit + 1);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

bool FontObj::IsCached()
{
	return m_bCached;
}

void FontObj::SetCached( bool bCached )
{
	m_bCached = bCached;
}