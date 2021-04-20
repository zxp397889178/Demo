/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UIAnimation.h"

#define _Style_Animation_Bk_ 0
#define enAnimationDefaultTimerID		0

CAnimationUI::CAnimationUI()
:m_nFrameCount(1)
,m_nActiveFrame(0)
,m_nTimerElapse(100)
,m_nFrameDelay(100)
,m_nCurrFrameDelay(100)
,m_bLoop(true)
,m_bAutoStart(true)
,m_bRuning(false)
{
	SetAnchor(UIANCHOR_LEFTTOP);
	SetMouseEnabled(false);
	SetStyle(_T("Picture"));
	m_pImageStyle = NULL;
}

CAnimationUI::~CAnimationUI()
{
	StopAnimation();
}


void CAnimationUI::StopAnimation()
{
	KillTimer(enAnimationDefaultTimerID);
	m_bRuning = false;
}

void CAnimationUI::StartAnimation()
{
	ImageStyle* pStyle = GetAnimationImageStyle();
	if (!pStyle) return;

	if (!pStyle->IsImageValid()) 
	{
		StopAnimation();
		ImageObj* pImageObj = pStyle->GetImageObj();
		if (!pImageObj) return;

		if (pImageObj->GetFrameCount() > 1) // gif
		{
			m_nActiveFrame = 0;
			m_nFrameCount = pImageObj->GetFrameCount();
			m_nCurrFrameDelay = pImageObj->GetFrameDelay(0);

		}
		else //png
		{
			m_nFrameCount = 1;
			if (pImageObj->GetCellRow() > 1)
				m_nFrameCount = pImageObj->GetCellRow();
			else if (pImageObj->GetCellCol() > 1)
				m_nFrameCount = pImageObj->GetCellCol();
			m_nActiveFrame = 0;
		}
	}

	if (!m_bRuning && GetFrameCount() > 1)
	{
		m_bRuning = true;
		SetTimer(enAnimationDefaultTimerID, m_nTimerElapse);
	}
	else
	{
		m_bRuning = true;
	}
}

int CAnimationUI::GetFrameCount()
{
	return m_nFrameCount;
}

void CAnimationUI::SetCurFrame(int nCurFrame)
{
	m_nActiveFrame = nCurFrame;
	if (m_nActiveFrame >= GetFrameCount())
		m_nActiveFrame = GetFrameCount() - 1;
}

int CAnimationUI::GetCurFrame()
{
	return m_nActiveFrame;
}

ImageObj* CAnimationUI::GetImage()
{
	ImageStyle *pStyle = GetAnimationImageStyle();
	if (pStyle)
		pStyle->LoadImage();
	return pStyle ? pStyle->GetImageObj() : NULL;
}

void CAnimationUI::SetImage(LPCTSTR lpszImage)
{
	if(!lpszImage)
		return;
	StopAnimation();
	ImageStyle *pStyle = GetAnimationImageStyle();
	if (pStyle)
	{
		pStyle->SetImage(lpszImage);
	}
	Invalidate();
}

void CAnimationUI::SetImage(ImageObj* pImageRender)
{
	if (!pImageRender)
		return;

	StopAnimation();

	ImageStyle *pStyle = GetAnimationImageStyle();
	if (pStyle)
	{
		pStyle->SetImage(pImageRender);
	}
	Invalidate();
}

ImageStyle* CAnimationUI::GetAnimationImageStyle()
{
	if (!m_pImageStyle)
		m_pImageStyle = GetImageStyle(_T("bk"));
	return m_pImageStyle;
}

void CAnimationUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("image")))
		SetImage(lpszValue);
	else if (equal_icmp(lpszName, _T("framedelay")))
		m_nFrameDelay = _ttoi(lpszValue);
	else if (equal_icmp(lpszName, _T("autostart")))
		m_bAutoStart = (bool)_ttoi(lpszValue);
	else
		CControlUI::SetAttribute(lpszName,lpszValue);
}
bool CAnimationUI::Event(TEventUI& event)
{
	if (event.nType == UIEVENT_TIMER && IsVisible())
	{
		ImageStyle* pImageStyle = GetAnimationImageStyle();
		if (!pImageStyle) return true;

		ImageObj *pImageObj = this->GetImage();
		if (!pImageObj) return true;

		// 用于换肤时中途图片发生变化
		int nCurrentFrameCount = 1;
		if (pImageObj->GetFrameCount() > 1)
		{
			nCurrentFrameCount = pImageObj->GetFrameCount();
			if (nCurrentFrameCount != GetFrameCount())
			{
				m_nFrameCount = nCurrentFrameCount;
				m_nActiveFrame = 0;
				m_nCurrFrameDelay = pImageObj->GetFrameDelay(0);
			}
		}
		else
		{
			if (pImageObj->GetCellRow() > 1)
				nCurrentFrameCount = pImageObj->GetCellRow();
			else if (pImageObj->GetCellCol() > 1)
				nCurrentFrameCount = pImageObj->GetCellCol();
			if (nCurrentFrameCount != GetFrameCount())
			{
				m_nFrameCount = nCurrentFrameCount;
				m_nActiveFrame = 0;

			}
		}

		m_nCurrFrameDelay -= m_nTimerElapse;
		if (m_nCurrFrameDelay <= 0)
		{
			if (m_nActiveFrame + 1 >= GetFrameCount() && !m_bLoop)
			{
				StopAnimation();
				GetWindow()->SendNotify(this, UINOTIFY_ANIMATION_END, 0, 0, true);
			}
			else
			{
				m_nActiveFrame = (m_nActiveFrame + 1) % GetFrameCount();
				if (pImageObj->GetFrameCount() > 1)
					m_nCurrFrameDelay = pImageObj->GetFrameDelay(m_nActiveFrame);
				else
					m_nCurrFrameDelay = m_nFrameDelay;
			}
		}
		Invalidate();
		return true;
	}
	else
		return CControlUI::Event(event);
}
void CAnimationUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	if (m_bAutoStart)
		StartAnimation();

	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = GetStyle(i);
		if (!pStyleObj->IsVisible()) continue;

		ImageStyle* pImageStyle = GetAnimationImageStyle();
		if (pStyleObj == pImageStyle)
		{
			ImageObj *pImageObj = pImageStyle->GetImageObj();
			if (pImageObj)
			{
				PaintParams params = *pImageStyle->GetPaintParams();
				params.nAlpha =  GetAlpha();
				params.nActiveFrame = m_nActiveFrame;
				pRenderDC->DrawImage(pImageObj, GetRect(), m_nActiveFrame, &params);
			}
		}
		else
			pStyleObj->Render(pRenderDC, rcPaint);
	}
}

void CAnimationUI::SetLoop(bool bLoop)
{
	m_bLoop = bLoop;
}

LPCTSTR CAnimationUI::GetImageFileName()
{
	ImageStyle *pStyle = GetAnimationImageStyle();
	if (pStyle)
	{
		return pStyle->GetImageFile();
	}
	return _T("");
}

void CAnimationUI::ReleaseImage()
{
	StopAnimation();
	__super::ReleaseImage();
}