/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UISlider.h"

#define _Style_SliderBar_Bk 0
#define _Style_SliderBar_ForeBk 1
#define _Style_SliderBar_PosBk 2
#define _Style_SliderBar_Thumb 3
CSliderUI::CSliderUI()
{	
	m_bHorizontal = true;
	SetMouseEnabled(true);
	SetEnabled(true);
	m_nRange = 100;
	m_nPos = 50;
	m_nForePos = 0;
	m_bMovingScroll = false;
	m_bProcess = false;
	SetStyle(_T("SliderBar"));
	ModifyFlags(UICONTROLFLAG_SETFOCUS, 0);
}

CSliderUI::~CSliderUI()
{
}


void CSliderUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)//重载基类
{
	if (equal_icmp(lpszName, _T("Horizontal")))
	{
		m_bHorizontal = (bool)!!_ttoi(lpszValue);
	}
	else
		CControlUI::SetAttribute(lpszName, lpszValue);
}

bool CSliderUI::Event(TEventUI& event)
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
void CSliderUI::CalcRect(POINT ptMouse)
{
	int nOffset = 0, nWidth = 0, nHeight = 0;
	RECT rc = this->GetRect();
	if (m_bHorizontal)
	{
		ImageObj* pImageObj = NULL;
		ImageStyle* pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_SliderBar_Bk));
		if (!(pStyle && pStyle->LoadImage())) return;
		pImageObj = pStyle->GetImageObj();
		//int nBkWidth = pImageObj->GetCellWidth();
		int nBkHeight = DPI_SCALE(pImageObj->GetCellHeight());
		pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_SliderBar_Thumb));
		if (!(pStyle && pStyle->LoadImage())) return;
		pImageObj = pStyle->GetImageObj();
		int nThumbWidth = DPI_SCALE(pImageObj->GetCellWidth());
		int nThumbHeight = DPI_SCALE(pImageObj->GetCellHeight());
		//nWidth = nBkWidth;
		nHeight = nBkHeight;

		m_rcImageRender.left = rc.left ;
		m_rcImageRender.top  = (rc.bottom - rc.top < nHeight?rc.top:rc.top + (rc.bottom - rc.top - nHeight)/2);
		m_rcImageRender.right= rc.right;
		m_rcImageRender.bottom = m_rcImageRender.top + nHeight;

		int totalWidth = m_rcImageRender.right - m_rcImageRender.left - nThumbWidth;
		if (totalWidth <= 0)
			totalWidth = 1;

		if (ptMouse.x == -1)
		{
			ptMouse.x = m_rcImageRender.left + m_nPos* totalWidth / m_nRange;
			nOffset = ptMouse.x;
			if (nOffset < m_rcImageRender.left)
				nOffset = m_rcImageRender.left;
			else if (nOffset > m_rcImageRender.right - nThumbWidth)
				nOffset = m_rcImageRender.right - nThumbWidth;
		}
		else
		{
			nOffset = ptMouse.x;
			if (nOffset < m_rcImageRender.left)
				nOffset = m_rcImageRender.left;
			else if (nOffset > m_rcImageRender.right - nThumbWidth)
				nOffset = m_rcImageRender.right - nThumbWidth;
			m_nPos = (nOffset - m_rcImageRender.left)*m_nRange / totalWidth;
		}

		double percent = (double)(m_nForePos/* > m_nPos? m_nPos:m_nForePos*/)/(m_nRange);
		//计算进度条前景图绘制区域
		m_rcForeImage.left = m_rcImageRender.left;
		m_rcForeImage.top = m_rcImageRender.top;
		m_rcForeImage.bottom = m_rcImageRender.bottom;
		m_rcForeImage.right = m_rcForeImage.left + (int)(totalWidth * percent);

		nWidth = nThumbWidth;
		nHeight = nThumbHeight;
		m_rcThumb.top = (rc.bottom - rc.top < nHeight?rc.top:rc.top + (rc.bottom - rc.top - nHeight)/2);
		m_rcThumb.bottom = m_rcThumb.top + nHeight;
		m_rcThumb.left = nOffset;
		m_rcThumb.right = m_rcThumb.left + nWidth;

		m_rcPosImageRender.top = m_rcImageRender.top;
		m_rcPosImageRender.left = m_rcImageRender.left;
		m_rcPosImageRender.bottom = m_rcImageRender.bottom;
		m_rcPosImageRender.right = m_rcThumb.left + nWidth / 2;
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
		m_rcImageRender.left = (rc.right - rc.left < nWidth ? rc.left : rc.left + (rc.right - rc.left - nWidth)/2);
		m_rcImageRender.top  = rc.top;
		m_rcImageRender.right= m_rcImageRender.left + nWidth;
		m_rcImageRender.bottom = rc.bottom;

		int totalHeight = m_rcImageRender.bottom - m_rcImageRender.top - nThumbHeight;
		if (totalHeight <= 0)
			totalHeight = 1;

		if (ptMouse.y == -1)
		{
			ptMouse.y = m_rcImageRender.bottom - m_nPos* totalHeight  / m_nRange;
			nOffset = ptMouse.y;
			if (nOffset < m_rcImageRender.top + nThumbHeight)
				nOffset = m_rcImageRender.top + nThumbHeight;
			else if (nOffset > m_rcImageRender.bottom)
				nOffset = m_rcImageRender.bottom;
		}
		else
		{
			nOffset = ptMouse.y;
			if (nOffset < m_rcImageRender.top + nThumbHeight)
				nOffset = m_rcImageRender.top + nThumbHeight;
			else if (nOffset > m_rcImageRender.bottom)
				nOffset = m_rcImageRender.bottom;
		
			m_nPos = (m_rcImageRender.bottom - nOffset)*m_nRange / totalHeight;
		}

		double percent = (double)(m_nForePos/* > m_nPos? m_nPos:m_nForePos*/)/(m_nRange);
		//计算进度条前景图绘制区域
		m_rcForeImage.left = m_rcImageRender.left;
		m_rcForeImage.bottom = m_rcImageRender.bottom;
		m_rcForeImage.right = m_rcImageRender.right;
		m_rcForeImage.top = m_rcForeImage.bottom - (int)(totalHeight * percent);

		nWidth = nThumbWidth;
		nHeight = nThumbHeight;
		m_rcThumb.left = rc.right - rc.left < nWidth ? rc.left : rc.left + (rc.right - rc.left - nWidth)/2;
		m_rcThumb.top  = nOffset - nHeight;
		m_rcThumb.right= m_rcThumb.left + nWidth;
		m_rcThumb.bottom = m_rcThumb.top + nHeight;

		m_rcPosImageRender.bottom = m_rcImageRender.bottom;
		m_rcPosImageRender.left = m_rcImageRender.left;
		m_rcPosImageRender.right = m_rcImageRender.right;
		m_rcPosImageRender.top = m_rcThumb.bottom - nHeight / 2;
	}
}
void CSliderUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	HDC hDestDC = pRenderDC->GetDC();
	SetPos(m_nPos, false);
	ImageStyle* pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_SliderBar_Bk));
	LPRECT lpRect;
	if (pStyle)
	{
		lpRect = pStyle->GetRectPtr();
		*lpRect = m_rcImageRender;
		/*pStyle->SetState(GetState());*/
		pStyle->Render(pRenderDC, rcPaint);
	}

	pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_SliderBar_ForeBk));
	if (pStyle)
	{
		lpRect = pStyle->GetRectPtr();
		*lpRect = m_rcImageRender;
		ClipObj clip;
		ClipObj::GenerateClip(hDestDC, m_rcForeImage, clip);
		/*pStyle->SetState(GetState());*/
		pStyle->Render(pRenderDC, rcPaint);
	}

	pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_SliderBar_PosBk));
	if (pStyle)
	{
		lpRect = pStyle->GetRectPtr();
		*lpRect = m_rcImageRender;
		ClipObj clip;
		ClipObj::GenerateClip(hDestDC, m_rcPosImageRender, clip);
		/*pStyle->SetState(GetState());*/
		pStyle->Render(pRenderDC, rcPaint);
	}

	pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_SliderBar_Thumb));
	if (pStyle)
	{
		lpRect = pStyle->GetRectPtr();
		*lpRect = m_rcThumb;
	/*	pStyle->SetState(GetState());*/
		pStyle->Render(pRenderDC, rcPaint);
	}
	RenderStyle(pRenderDC, rcPaint, _Style_SliderBar_Thumb+1);
}
void CSliderUI::SetPos(int nPos,bool bRedraw/*=true*/)
{
	m_nPos = nPos;
	POINT pt;
	pt.x = -1;
	pt.y = -1;
	CalcRect(pt);
	if (bRedraw)
		this->Invalidate();
}
int CSliderUI::GetPos()
{
	return m_nPos;
}
void CSliderUI::SetRange(int nRange,bool bRedraw/*=true*/)
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
int CSliderUI::GetRange()
{
	return m_nRange;
}

int CSliderUI::GetForePos()
{
	return m_nForePos;
}
void CSliderUI::SetForePos(int nPos, bool bRedraw/* = true*/)
{
	m_nForePos = nPos;
	POINT pt;
	pt.x = -1;
	pt.y = -1;
	CalcRect(pt);
	if (bRedraw)
		this->Invalidate();
}