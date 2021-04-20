#include "stdafx.h"
#include "ShadowTextStyleObj.h"
#pragma comment(lib, "MSIMG32.LIB")

///////////////////////////////////////////////////////////////////////////////////////////////
class ColorObjExHelper
{
public:
	static DWORD GetColor(ColorObjEx* colorObj)
	{
		if (colorObj == NULL)
			return GetUIRes()->GetDefaultColor()->GetColor();
		return colorObj->GetColor();
	}

	static void SetColor(ColorObjEx** colorObj, LPCTSTR lpszValue)
	{
		if (lpszValue == NULL || _tcslen(lpszValue) == 0 || _tcsicmp(lpszValue, _T("-1")) == 0)
		{
			ClearColor(colorObj);
			return;
		}
		if (*colorObj == NULL)
		{
			*colorObj = new ColorObjEx();
		}
		(*colorObj)->SetColor(lpszValue);
	}

	static void SetColor(ColorObjEx** colorObj, DWORD dwValue)
	{
		if (*colorObj == NULL)
		{
			*colorObj = new ColorObjEx();
		}
		(*colorObj)->SetColor(dwValue);
	}

	static void SetColor(ColorObjEx** colorObj, ColorObj* objValue)
	{
		if (*colorObj == NULL)
		{
			*colorObj = new ColorObjEx();
		}
		(*colorObj)->SetColorObj(objValue);
	}

	static void ClearColor(ColorObjEx** colorObj)
	{
		if (*colorObj != NULL)
		{
			delete (*colorObj);
			*colorObj = NULL;
		}
	}

	static void CopyColor(ColorObjEx** dest, ColorObjEx** src)
	{
		if (*src == NULL)
		{
			if (*dest != NULL)
			{
				delete (*dest);
				*dest = NULL;
			}
		}
		else
		{
			if (*dest == NULL)
				*dest = new ColorObjEx();
			memcpy(*dest, *src, sizeof(ColorObjEx));
		}
	}
};

void DrawGrowText(FontObj* fontObj, IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format);

////////////////////////////////////////////////////////////////////////////////////////////////
ShadowTextStyle::ShadowTextStyle(CControlUI* pOwner) :TextStyle(pOwner)
{
	memset(m_pArrShadowColorObj, 0, sizeof(m_pArrShadowColorObj));
}

ShadowTextStyle::~ShadowTextStyle(void)
{
	for (int i = 0; i < _countof(m_pArrShadowColorObj); i++)
	{
		if (m_pArrShadowColorObj[i])
		{
			delete m_pArrShadowColorObj[i];
			m_pArrShadowColorObj[i] = NULL;
		}
			
	}
}

void ShadowTextStyle::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	UITYPE_STATE uState = GetDrawState();
	if (!MaskState((UITYPE_STATE)uState))
	{
		return;
	}

	if (GetFontEffect() == UIFONTEFFECT_NORMAL)
	{
		__super::Render(pRenderDC, rcPaint);
		return;
	}

	int nIndex = uState - UISTATE_NORMAL;
	if (uState >= UISTATE_CKNORMAL)
		nIndex = uState - UISTATE_CKNORMAL;

	DWORD dwColor = GetShadowTextColor(nIndex);
	
	//pRenderDC->DrawShadowText(GetFontObj(), GetRect(), GetDrawText(), dwColor, GetAlign(), 0x02000000, 20);
	DrawGrowText(GetFontObj(), pRenderDC, GetRect(), GetDrawText(), dwColor, GetAlign());
}

void ShadowTextStyle::SetShadowTextColor(DWORD dwColor, int nIndex /*= 0*/)
{
	if (nIndex >= 0 && nIndex < _countof(m_pArrShadowColorObj))
	{
		ColorObjExHelper::SetColor(&m_pArrShadowColorObj[nIndex], dwColor);
	}
}
DWORD ShadowTextStyle::GetShadowTextColor(int nIndex /*= 0*/)
{
	if (nIndex >= 0 && nIndex < 5)
	{
		if (m_pArrShadowColorObj[nIndex] == NULL)
			nIndex = 0;
		return ColorObjExHelper::GetColor(m_pArrShadowColorObj[nIndex]);
	}
	return GetUIRes()->GetDefaultColor()->GetColor();
}

void ShadowTextStyle::SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue)
{
	if (_tcsicmp(lpszName, _T("shadow_textcolor")) == 0)
	{
		tstring strSpliter = lpszValue;
		int i = strSpliter.find_first_of(_T('|'));
		int index = 0;
		while (i >= 0 && index < 5)
		{
			tstring strValue = strSpliter.substr(0, i);
			if (!strValue.empty())
			{
				ColorObjExHelper::SetColor(&m_pArrShadowColorObj[index], strValue.c_str());
			}
			else if (index > 0)
			{
				ColorObjExHelper::CopyColor(&m_pArrShadowColorObj[index], &m_pArrShadowColorObj[index - 1]); // 不配置使用上次颜色
			}
			strSpliter = strSpliter.substr(i + 1);
			i = strSpliter.find_first_of(_T('|'));
			index++;
		}
		if (index < 5)
		{
			tstring strValue = strSpliter.substr(0, i);
			if (!strValue.empty())
			{
				ColorObjExHelper::SetColor(&m_pArrShadowColorObj[index], strValue.c_str());
			}
			else if (index > 0)
			{
				ColorObjExHelper::CopyColor(&m_pArrShadowColorObj[index], &m_pArrShadowColorObj[index - 1]); // 不配置使用上次颜色
			}
		}
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}



void DrawGrowText(FontObj* fontObj, IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format)
{
	HDC hdc = pRenderDC->GetDC();
	HFONT hFont = fontObj->GetFont();

	int width = 0;
	int height = 0;


	static DibObj dibObj;
	if (dibObj.GetSafeHdc() == NULL)
	{
		void* pImageBits = NULL;
		ImageObj* pImageObj = GetUIRes()->LoadImage(_T("#textglow"), false, UIIAMGE_BITMAP);
		if (!pImageObj) return;
		Bitmap* pimage = pImageObj->GetBitmap();
		width = pimage->GetWidth();
		height = pimage->GetHeight();
		Rect rect(0, 0, width, height);
		BitmapData bmpdata = { 0 };
		Status status = pimage->LockBits(&rect, ImageLockModeWrite | ImageLockModeRead,
			PixelFormat32bppPARGB, &bmpdata);
		if (status != Ok) return;
		pImageBits = bmpdata.Scan0;
		dibObj.Create(NULL, width, height);
		BYTE* pBits = dibObj.GetBits();
		memcpy(pBits, pImageBits, width * height * 4);
		pimage->UnlockBits(&bmpdata);
		pImageObj->Release();
	}

	// 自动改变颜色
	DibObj* pdib = NULL;
	BYTE a = (dwColor & 0xFF000000) >> 24;
	
	pdib = &dibObj;
	width = pdib->GetWidth();
	height = pdib->GetHeight();


	SIZE dibSize = { 0 };
	dibSize.cx = rc.right - rc.left;
	dibSize.cy = rc.bottom - rc.top;


	// 计算文字路径，alpha = 0代表有文字
	DibObj dibpath;
	dibpath.Create(NULL, dibSize.cx, dibSize.cy);

	HFONT hOldFont = (HFONT)::SelectObject(dibpath.GetSafeHdc(), hFont);
	int oldMode = ::SetBkMode(dibpath.GetSafeHdc(), TRANSPARENT);
	dibpath.Fill(RGB(255, 255, 255), 255);

	RECT txtRect = { 0, 0, dibSize.cx, dibSize.cy };
	::DrawText(dibpath.GetSafeHdc(), lpszText, _tcslen(lpszText), &txtRect, format);

	::SelectObject(dibpath.GetSafeHdc(), hOldFont);
	::SetBkMode(dibpath.GetSafeHdc(), oldMode);

	float fz = 0.55;

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	bf.SourceConstantAlpha = 80 * fz * a / 255;


	unsigned char aaa = 255;
	int nWideBytes = dibpath.GetWidth() * 4;
	BYTE* ptr = dibpath.GetBits();
	for (int j = 0; j < dibSize.cy - 1; j++)
	{
		BYTE* ptr1 = ptr + nWideBytes * j;
		for (int i = 0; i < dibSize.cx - 1; i++)
		{
			if (ptr1[i * 4 + 3] != aaa)
			{
				::AlphaBlend(hdc, rc.left + i - width / 2, rc.top + j - width / 2, width, height, pdib->GetSafeHdc(), 0, 0, width, height, bf);
			}
		}
	}
	fontObj->DrawText(pRenderDC, rc, lpszText, dwColor, format, UIFONTEFFECT_NORMAL, UIFONT_GDIPLUS);
}
