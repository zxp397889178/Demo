/*********************************************************************
 *       Copyright (C) 2011,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UIProgressBar.h"

#define _Style_ProgressBar_Bk 0
#define _Style_ProgressBar_ForeBk 1
CProgressBarUI::CProgressBarUI()
:m_bClipRender(true)
{
	SetStyle(_T("ProgressBar"));
	m_ulfirstPos = 0;
	m_ullastPos = 100;
	m_ulcurPos = 0;
	m_bHorizontal = true;
	SetMouseEnabled(true);
}

CProgressBarUI::~CProgressBarUI()
{
}


//设置进度条的控制范围(eg: SetRange(0,100))
void CProgressBarUI::SetRange(const unsigned long long& firstpos,const unsigned long long& lastpos)
{
	m_ulfirstPos = firstpos;
	m_ullastPos = lastpos;
	if (firstpos == lastpos)
		m_ullastPos = m_ulfirstPos+1;
}

//设置当前进度(ge: Setpos(50))
void CProgressBarUI::SetPos(const unsigned long long& curpos, bool bUpdate/* = false*/)
{
	//double prev_perncent,cur_percent;
	if((curpos < m_ulfirstPos) || (curpos > m_ullastPos))
	{
		return;
	}
	//prev_perncent = (double)(m_ulcurPos - m_ulfirstPos)/(m_ullastPos - m_ulfirstPos);
	//cur_percent = (double)(curpos - m_ulfirstPos)/(m_ullastPos - m_ulfirstPos);
	m_ulcurPos = curpos;

	Invalidate(bUpdate);
}

void CProgressBarUI::SetPos(const float fPercent, bool bUpdate /*= false*/)
{
	m_ulcurPos = (int)(fPercent * m_ullastPos);
	Invalidate(bUpdate);
}

void CProgressBarUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)//重载基类
{
	if (equal_icmp(lpszName, _T("Horizontal")))
	{
		m_bHorizontal = (bool)!!_ttoi(lpszValue);
		SetStyle(m_bHorizontal?_T("ProgressBar"):_T("VProgressBar"));
	}
	else if (equal_icmp(lpszName, _T("ClipRender")))
	{
		ClipRender(!!_ttoi(lpszValue));
	}
	else
		CControlUI::SetAttribute(lpszName, lpszValue);
}

void CProgressBarUI::GetRange(unsigned long long& firstpos ,unsigned long long& lastpos)
{
	firstpos = m_ulfirstPos;
	lastpos = m_ullastPos;
}

void CProgressBarUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	StyleObj* pStyleObj = GetStyle(_Style_ProgressBar_Bk);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T(" Style ProgressBar Bk"));
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle)
	{
		pImageStyle->SetRect(GetRect());
		pImageStyle->SetState(GetState());
		pImageStyle->Render(pRenderDC, rcPaint);
	}

	pStyleObj = GetStyle(_Style_ProgressBar_ForeBk);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T(" Style ProgressBar ForeBk"));
	pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle)
	{
		pImageStyle->SetRect(GetRect());

		RECT rect = GetRect();
		RECT drawrect;
		double percent = (double)(m_ulcurPos - m_ulfirstPos)/(m_ullastPos - m_ulfirstPos);

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
		}
		else
		{
			pImageStyle->SetRect(drawrect);
			RECT rcTemp = {0};
			if (!!IntersectRect(&rcTemp, &drawrect, &rcPaint))
			{
				pImageStyle->SetState(GetState());
				pImageStyle->Render(pRenderDC, rcPaint);
			}
		}
	}
	RenderStyle(pRenderDC, rcPaint, _Style_ProgressBar_ForeBk + 1);
}

unsigned long long CProgressBarUI::GetPos()
{
	return m_ulcurPos;
}

void CProgressBarUI::ClipRender( bool val )
{
	m_bClipRender = val;
}

bool CProgressBarUI::IsClipRender() const
{
	return m_bClipRender;
}