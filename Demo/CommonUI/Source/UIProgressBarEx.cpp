#include "stdafx.h"
#include "UIProgressBarEx.h"
#include "boost\format.hpp"


CProgressBarUIEx::CProgressBarUIEx()
	: m_sFinishImage(_T(""))
	, m_sForeImage(_T(""))
	, m_bFinish(false)
{
	SetStyle(_T("ProgressBarEx"));
}


void CProgressBarUIEx::SetPos(const float fPercent, bool bUpdate /*= false*/)
{
	if (fPercent >= 1.0)
	{
		if (!m_bFinish &&m_sFinishImage != _T(""))
		{
			auto pImageStyle = GetImageStyle(_T("forebk"));
			auto pTextStyle = GetTextStyle(_T("txt"));
			if (pImageStyle && pTextStyle)
			{
				m_sForeImage = pImageStyle->GetImageFile();
				SetAttribute(_T("forebk.image"), m_sFinishImage.c_str());

				m_dwForeTextColor = pTextStyle->GetTextColor();
				pTextStyle->SetTextColor(m_dwFinishTextColor);
				m_bFinish = true;
				Invalidate();
			}
		}
	}
	else
	{
		if (m_bFinish &&m_sForeImage != _T(""))
		{		
			auto pImageStyle = GetImageStyle(_T("forebk"));
			auto pTextStyle = GetTextStyle(_T("txt"));
			if (pImageStyle && pTextStyle)
			{
				SetAttribute(_T("forebk.image"), m_sForeImage.c_str());
				pTextStyle->SetTextColor(m_dwForeTextColor);
				m_bFinish = false;
				Invalidate();
			}
		}
	}
	__super::SetPos(fPercent, bUpdate);
}


void CProgressBarUIEx::SetPos(const unsigned long long& curpos, bool bUpdate /*= false*/)
{
	
	if (curpos == m_ullastPos)
	{
		if (!m_bFinish &&m_sFinishImage != _T(""))
		{
			auto pImageStyle = GetImageStyle(_T("forebk"));
			auto pTextStyle = GetTextStyle(_T("txt"));
			if (pImageStyle && pTextStyle)
			{
				m_sForeImage = pImageStyle->GetImageFile();
				SetAttribute(_T("forebk.image"), m_sFinishImage.c_str());

				m_dwForeTextColor = pTextStyle->GetTextColor();
				pTextStyle->SetTextColor(m_dwFinishTextColor);
				m_bFinish = true;
				Invalidate();
			}
		}
	}
	else
	{
		if (m_bFinish &&m_sForeImage != _T(""))
		{
			auto pImageStyle = GetImageStyle(_T("forebk"));
			auto pTextStyle = GetTextStyle(_T("txt"));
			if (pImageStyle && pTextStyle)
			{
				SetAttribute(_T("forebk.image"), m_sForeImage.c_str());
				pTextStyle->SetTextColor(m_dwForeTextColor);
				m_bFinish = false;
				Invalidate();
			}
		}
	}
	__super::SetPos(curpos, bUpdate);
}

void CProgressBarUIEx::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	ImageStyle* pImageStyle = GetImageStyle(_T("bk"));
	if (pImageStyle)
	{
		pImageStyle->SetRect(GetRect());
		pImageStyle->SetState(GetState());
		pImageStyle->Render(pRenderDC, rcPaint);
	}
	pImageStyle = GetImageStyle(_T("forebk"));
	ImageStyle* pDecorateImage = GetImageStyle(_T("foredecorate"));
	if (pImageStyle && pDecorateImage)
	{
		pImageStyle->SetRect(GetRect());
		pDecorateImage->SetRect(GetRect());

		RECT rect = GetRect();
		RECT drawrect;
		bool bVisible = pDecorateImage->IsVisible();
		bool b = GetTextStyle(_T("txt"))->IsVisible();
		double percent = (double)(m_ulcurPos - m_ulfirstPos) / (m_ullastPos - m_ulfirstPos);
		int nProgress = (int)(percent * 100);
		tstring strProgress = _T("");
		if (m_bTextVisible)
		{
			strProgress = str(boost::wformat(_T("%d%s")) % nProgress% _T("%"));
		}
		SetAttribute(_T("txt.text"), strProgress.c_str());

		if (m_bHorizontal)
		{
			drawrect.left = rect.left;
			drawrect.top = rect.top;
			drawrect.bottom = rect.bottom;
			drawrect.right = drawrect.left + (int)((rect.right - rect.left)*percent);
		}
		else
		{
			drawrect.left = rect.left;
			drawrect.right = rect.right;
			drawrect.bottom = rect.bottom;
			drawrect.top = drawrect.bottom - (int)((rect.bottom - rect.top)*percent);
		}

		RtlRect(m_rcItem, &drawrect);
		if (IsClipRender())
		{
			HDC hDestDC = pRenderDC->GetDC();
			ClipObj clip;
			ClipObj::GenerateClipWithAnd(hDestDC, drawrect, clip);
			pImageStyle->SetState(GetState());
			pImageStyle->Render(pRenderDC, rcPaint);
			pDecorateImage->SetState(GetState());
			pDecorateImage->Render(pRenderDC, rcPaint);
		}
		else
		{
			pImageStyle->SetRect(drawrect);
			pDecorateImage->SetRect(drawrect);
			RECT rcTemp = { 0 };
			if (!!IntersectRect(&rcTemp, &drawrect, &rcPaint))
			{
				pImageStyle->SetState(GetState());
				pImageStyle->Render(pRenderDC, rcPaint);
				pDecorateImage->SetState(GetState());
				pDecorateImage->Render(pRenderDC, rcPaint);
			}
		}
	}
	RenderStyle(pRenderDC, rcPaint, 3);
}

void CProgressBarUIEx::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(_T("finishimage"), lpszName) == 0)
	{
		m_sFinishImage = lpszValue;
		return;
	}
	else if (_tcsicmp(_T("finishtextcolor"), lpszName) == 0)
	{
		long longTemp = _tcstol(lpszValue, NULL, 16);     //先将有用的字符串转换成长整型赋给变量
		int R = (longTemp & 0xFF0000) >> 16;    //先取出表示颜色分量R，移位得到真实的颜色分量值
		int G = (longTemp & 0x00FF00) >> 8;
		int B = longTemp & 0x0000FF;
		m_dwFinishTextColor = RGB(R, G, B);
		return;
	}
	else if (_tcsicmp(_T("EnableText"), lpszName) == 0)
	{
		m_bTextVisible = !!_ttoi(lpszValue);
	}
	__super::SetAttribute(lpszName, lpszValue);
}

void CProgressBarUIEx::Init()
{
	__super::Init();
	auto pTextStyle = GetTextStyle(_T("txt"));
	if (pTextStyle)
	{
		if (m_dwFinishTextColor == 1)
		{
			m_dwFinishTextColor = pTextStyle->GetTextColor();
		}
		
	}
}
