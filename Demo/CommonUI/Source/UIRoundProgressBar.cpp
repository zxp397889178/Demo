#include "stdafx.h"

#include "UIRoundProgressBar.h"

#include "boost\format.hpp"

#define _Round_ProgressBar_Bk 0
#define _Round_ProgressBar_ForeBk 1

#define PI 3.141592653
CRoundProgressBar::CRoundProgressBar() : m_bShwValue(true)
{
	SetRange(0,100);
	m_fPercent = 0;
	m_ulcurPos = 0;
}

void CRoundProgressBar::Init()
{
	__super::Init();

	TextStyle *pStyle = new TextStyle(this);
	pStyle->SetAttribute(_T("id"), _T("progress_num"));
	pStyle->SetAttribute(_T("Text"), _T("0%"));
	pStyle->SetAttribute(_T("halign"), _T("center"));
	pStyle->SetAttribute(_T("font"), _T("font14"));
	pStyle->SetAttribute(_T("textcolor"), _T("#Color_White"));
	pStyle->SetAttribute(_T("pos"), _T("LTRB|0,0,0,0"));
	pStyle->SetAttribute(_T("state"), _T("non"));
    if (GetStyle())
    {
        GetStyle()->Add(pStyle);
    }
}

void CRoundProgressBar::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	StyleObj* pStyleObj = GetStyle(_Round_ProgressBar_Bk);
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle)
	{
		pImageStyle->SetRect(GetRect());
		pImageStyle->SetState(GetState());
		pImageStyle->Render(pRenderDC, rcPaint);
	}

	pStyleObj = GetStyle(_Round_ProgressBar_ForeBk);
	pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle)
	{
		pImageStyle->SetRect(GetRect());

		//double percent = (double)(m_ulcurPos - m_ulfirstPos) / (m_ullastPos - m_ulfirstPos);
		
		if (m_bShwValue)
		{
			int nProgress = (int)(m_fPercent * 100);
			tstring strProgress = str(boost::wformat(_T("%d%s")) % nProgress % _T("%"));
			SetAttribute(_T("progress_num.Text"), strProgress.c_str());
		}
		else
		{
			SetAttribute(_T("progress_num.Text"), _T(""));
		}
		

		if (m_ulcurPos == m_ullastPos)
		{
			__super::Render(pRenderDC, rcPaint);
		}
		else if (m_ulcurPos != 0)
		{
			RECT rect = GetRect();
			//ÍâÀ©Ò»¸öÏñËØ,·ñÔò±ßÔµÓÐ¾â³Ý
			--rect.left;
			--rect.top;
			++rect.bottom;
			++rect.right;

			int iRadius = (rect.right - rect.left) / 2;
			int nOffsetY = (int)(iRadius - cos(2 * PI * m_fPercent) * iRadius);

			int nOffsetX = (int)((sin(2 * PI * m_fPercent) + 1) * iRadius);
			POINT ptArcEnd = { rect.left + iRadius, rect.top };
			POINT ptArcStart = { rect.left + nOffsetX, rect.top + nOffsetY };
			POINT ptArcCenter = { rect.left + iRadius, rect.top + iRadius };
			HDC hDestDC = pRenderDC->GetDC();

			::BeginPath(hDestDC);
			::MoveToEx(hDestDC, ptArcCenter.x, ptArcCenter.y, nullptr);
			::LineTo(hDestDC, ptArcStart.x, ptArcStart.y);
			::ArcTo(hDestDC, rect.left, rect.top, rect.right, rect.bottom,
				ptArcStart.x, ptArcStart.y, ptArcEnd.x, ptArcEnd.y);
			::LineTo(hDestDC, ptArcCenter.x, ptArcCenter.y);
			::CloseFigure(hDestDC);
			::EndPath(hDestDC);
			ClipObj clip;
			RECT rcTemp;
			if (::IntersectRect(&rcTemp, &rect, &rcPaint))
			{
				ClipObj::GenerateClipWithPath(hDestDC, rcTemp, clip);

				pImageStyle->SetState(GetState());
				pImageStyle->Render(pRenderDC, rcPaint);
			}
		}
	}
	RenderStyle(pRenderDC, rcPaint, _Round_ProgressBar_ForeBk + 1);
}

void CRoundProgressBar::SetPos(float fPercent, bool bUpdate)
{
	m_fPercent = fPercent;
	m_ulcurPos = (int)(m_fPercent * m_ullastPos);
	GetParent()->Invalidate();
}

void CRoundProgressBar::SetShowValue(bool bShowValue)
{
	m_bShwValue = bShowValue;
	
}


