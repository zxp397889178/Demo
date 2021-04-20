#include "stdafx.h"
#include "UILayerControl.h"

void CLayerControl::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	HDC hDestDC = pRenderDC->GetDC();
	ClipObj clip;
	ClipObj::GenerateClipWithAnd(hDestDC, rcPaint, clip);
	::SetBkColor(hDestDC, RGB(0, 0, 0));
	::ExtTextOut(hDestDC, 0, 0, ETO_OPAQUE, &GetRect(), NULL, 0, NULL);
	RECT rc = GetRect();
	BYTE a = LOBYTE((m_dColor) >> 24);
	if (a != 0) //不透明下再画
	{
		Graphics graphics(hDestDC);
		Gdiplus::SolidBrush brush(Gdiplus::Color(a, GetRValue(m_dColor), GetGValue(m_dColor), GetBValue(m_dColor))); // 红色和蓝色要相反
		graphics.FillRectangle(&brush, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}
}

void CLayerControl::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("Color")) == 0)
	{
		int len = _tcslen(lpszValue);
		if (len == 8)
		{
			DWORD color = _tcstoul(lpszValue, NULL, 16);
			DWORD A = color & 0xFF000000;
			BYTE R = GetBValue(color);    //先取出表示颜色分量R，移位得到真实的颜色分量值
			BYTE G = GetGValue(color);
			BYTE B = GetRValue(color);
			m_dColor = A + RGB(R, G, B);
		}
	}
	__super::SetAttribute(lpszName, lpszValue);
}
