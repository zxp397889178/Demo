/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UISliderV2.h"

#define _Style_SliderBar_Bk 0
#define _Style_SliderBar_ForeBk 1
#define _Style_SliderBar_PosBk 2
#define _Style_SliderBar_Thumb 3
CSliderV2UI::CSliderV2UI()
{	
	m_bHorizontal = true;
	SetMouseEnabled(true);
	SetEnabled(true);
	m_nRange = 100;
	m_nPos = 0;
	m_bMovingScroll = false;
	m_bProcess = false;
	SetStyle(_T("SliderBarV2"));
	ModifyFlags(UICONTROLFLAG_SETFOCUS, 0);
	m_pThumbStyle = NULL;
	m_pForeStyle = NULL;
}

CSliderV2UI::~CSliderV2UI()
{
}


void CSliderV2UI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)//重载基类
{
	if (equal_icmp(lpszName, _T("Horizontal")))
	{
		m_bHorizontal = (bool)!!_ttoi(lpszValue);
	}
	else
		CControlUI::SetAttribute(lpszName, lpszValue);
}

bool CSliderV2UI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_BUTTONDOWN:
		{
				if (PtInRect(&m_rcThumb, event.ptMouse) && !m_bMovingScroll)
				{
					m_bMovingScroll = true;
					CalcRect(event.ptMouse);
					SetTimer(enDefaultTimerID, enDefaultTimerElapse);
					this->SendNotify(UINOTIFY_TRBN_BEGINCHANGE);
					this->SendNotify(UINOTIFY_TRBN_POSCHANGE);
				}
				else if (PtInRect(&m_rcImageRender, event.ptMouse))
				{
					CalcRect(event.ptMouse);
					this->SendNotify(UINOTIFY_TRBN_BEGINCHANGE);
					this->SendNotify(UINOTIFY_TRBN_POSCHANGE);
				}
				this->Invalidate();
		}
		break;
	case UIEVENT_TIMER:
		{
				if (m_bMovingScroll && !m_bProcess)
				{
					m_bProcess = true;
					this->SendNotify(UINOTIFY_TRBN_POSCHANGE);
					m_bProcess = false;
				}
				return true;
		}
		break;
	case UIEVENT_BUTTONUP:
		{
				if (m_bMovingScroll)
				{
					m_bMovingScroll = false;
					
					this->KillTimer(enDefaultTimerID);

					this->SendNotify(UINOTIFY_TRBN_ENDCHANGE);
				}
				else
				{
					if (PtInRect(&m_rcThumb, event.ptMouse))
					{
						this->SendNotify(UINOTIFY_TRBN_ENDCHANGE);
					}
					else if (PtInRect(&m_rcImageRender, event.ptMouse))
					{
						this->SendNotify(UINOTIFY_TRBN_ENDCHANGE);
					}
				}
				this->SendNotify(UINOTIFY_TRBN_SELCHANGE);
				this->Invalidate();
		}
		break;
	case UIEVENT_MOUSEMOVE:
	case UIEVENT_MOUSEENTER:
		{
			if (m_bMovingScroll)
			{
				CalcRect(event.ptMouse);
				this->Invalidate();
			}
		}
		break;
	default:
		return CControlUI::Event(event);
	}
	return true;
}
void CSliderV2UI::CalcRect(POINT ptMouse)
{
	ImageStyle* pThumbStyle = GetThumbStyle();
	ImageStyle* pForeStyle = GetForeStyle();
	if (!pThumbStyle || !pForeStyle)
		return;

	int nOffset = 0, nWidth = 0, nHeight = 0;
	RECT rc = pForeStyle->GetRect();
	if (m_bHorizontal)
	{
		int nThumbWidth = pThumbStyle->GetRectPtr()->right - pThumbStyle->GetRectPtr()->left;
		int nThumbHeight = pThumbStyle->GetRectPtr()->bottom - pThumbStyle->GetRectPtr()->top;
	
		if (ptMouse.x == -1)
		{
			ptMouse.x = rc.left + m_nPos*(rc.right - rc.left - nThumbWidth) / m_nRange;
			nOffset = ptMouse.x;
			if (nOffset < rc.left)
				nOffset = rc.left;
			else if (nOffset > rc.right - nThumbWidth)
				nOffset = rc.right - nThumbWidth;
		}
		else
		{
			nOffset = ptMouse.x;
			if (nOffset < rc.left)
				nOffset = rc.left;
			else if (nOffset > rc.right - nThumbWidth)
				nOffset = rc.right - nThumbWidth;
			m_nPos = (nOffset - rc.left)*m_nRange / ((rc.right - rc.left - nThumbWidth) != 0 ? (rc.right - rc.left - nThumbWidth) : 1);
		}

		m_rcThumb = pThumbStyle->GetRect();
		m_rcThumb.left = nOffset;
		m_rcThumb.right = m_rcThumb.left + nThumbWidth;
	}
	else
	{
		ImageObj* pImageObj = NULL;
		ImageStyle* pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_SliderBar_Bk));
		if (!(pStyle && pStyle->LoadImage())) return;
		pImageObj = pStyle->GetImageObj();
		int nBkWidth = DPI_SCALE(pImageObj->GetCellWidth());
		//int nBkHeight = pImageObj->GetCellHeight();
		pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_SliderBar_Thumb));
		if (!(pStyle && pStyle->LoadImage())) return;
		pImageObj = pStyle->GetImageObj();
		int nThumbWidth = DPI_SCALE(pImageObj->GetCellWidth());
		int nThumbHeight = DPI_SCALE(pImageObj->GetCellHeight());
		nWidth = nBkWidth;
		//nHeight = nBkHeight;
		m_rcImageRender.left = (rc.right - rc.left < nWidth ? rc.left : rc.left + (rc.right - rc.left - nWidth) / 2);
		m_rcImageRender.top = rc.top + nThumbHeight / 2 + 1;
		m_rcImageRender.right = m_rcImageRender.left + nWidth;
		m_rcImageRender.bottom = rc.bottom - nThumbHeight / 2 - 1;

		if (ptMouse.y == -1)
		{
			ptMouse.y = m_rcImageRender.bottom - m_nPos*(m_rcImageRender.bottom - m_rcImageRender.top) / m_nRange;
			nOffset = ptMouse.y;
			if (nOffset < m_rcImageRender.top)
				nOffset = m_rcImageRender.top;
			else if (nOffset > m_rcImageRender.bottom)
				nOffset = m_rcImageRender.bottom;
		}
		else
		{
			nOffset = ptMouse.y;
			if (nOffset < m_rcImageRender.top)
				nOffset = m_rcImageRender.top;
			else if (nOffset > m_rcImageRender.bottom)
				nOffset = m_rcImageRender.bottom;
			m_nPos = (m_rcImageRender.bottom - nOffset)*m_nRange / ((m_rcImageRender.bottom - m_rcImageRender.top) != 0 ? m_rcImageRender.bottom - m_rcImageRender.top : 1);
		}

		nWidth = nThumbWidth;
		nHeight = nThumbHeight;
		m_rcThumb.left = rc.right - rc.left < nWidth ? rc.left : rc.left + (rc.right - rc.left - nWidth) / 2;
		m_rcThumb.top = nOffset - nHeight / 2;
		m_rcThumb.right = m_rcThumb.left + nWidth;
		m_rcThumb.bottom = m_rcThumb.top + nHeight;

		m_rcPosImageRender.bottom = m_rcImageRender.bottom;
		m_rcPosImageRender.left = m_rcImageRender.left;
		m_rcPosImageRender.right = m_rcImageRender.right;
		m_rcPosImageRender.top = m_rcThumb.top + (m_rcThumb.bottom - m_rcThumb.top) / 2;
	}
}
void CSliderV2UI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	SetPos(m_nPos, false);
	HDC hDestDC = pRenderDC->GetDC();
	ImageStyle* pThumbStyle = GetThumbStyle();
	ImageStyle* pForeStyle = GetForeStyle();
	if (!pThumbStyle || !pForeStyle)
		return;
	int nStyleCount = GetStyleCount();
	for (int i = 0; i < nStyleCount; i++)
	{
		StyleObj* pStyle = GetStyle(i);
		if (!pStyle->IsVisible())
			continue;
		if (pStyle == pForeStyle)
		{
			RECT rectClip = pForeStyle->GetRect();
			rectClip.right = m_rcThumb.left;
			ClipObj clip;
			ClipObj::GenerateClip(hDestDC, rectClip, clip);
			pStyle->Render(pRenderDC, rcPaint);
		}
		else if (pStyle == pThumbStyle)
		{
			LPRECT lpRect = pStyle->GetRectPtr();
			*lpRect = m_rcThumb;
			pStyle->Render(pRenderDC, rcPaint);
		}
		else
		{

			pStyle->Render(pRenderDC, rcPaint);
		}
	}
}
void CSliderV2UI::SetPos(int nPos, bool bRedraw/*=true*/)
{
	m_nPos = nPos;
	POINT pt;
	pt.x = -1;
	pt.y = -1;
	CalcRect(pt);
	if (bRedraw)
		this->Invalidate();
}
int CSliderV2UI::GetPos()
{
	return m_nPos;
}
void CSliderV2UI::SetRange(int nRange, bool bRedraw/*=true*/)
{
	if (nRange !=0 )
		m_nRange = nRange;
	POINT pt;
	pt.x = -1;
	pt.y = -1;
	CalcRect(pt);
	if (bRedraw)
		this->Invalidate();
}
int CSliderV2UI::GetRange()
{
	return m_nRange;
}

ImageStyle* CSliderV2UI::GetThumbStyle()
{
	if (m_pThumbStyle == nullptr)
	{
		m_pThumbStyle = dynamic_cast<ImageStyle*>(GetStyle(_T("thumb")));
	}
	return m_pThumbStyle;
}

ImageStyle* CSliderV2UI::GetForeStyle()
{
	if (m_pForeStyle == nullptr)
	{
		m_pForeStyle = dynamic_cast<ImageStyle*>(GetStyle(_T("fore")));
	}
	return m_pForeStyle;
}

