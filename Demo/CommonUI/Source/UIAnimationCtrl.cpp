/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UIAnimationCtrl.h"

#define _Style_Animation_Bk_ 0
#define enAnimationDefaultTimerID		0

CAnimationCtrlUI::CAnimationCtrlUI()
:m_nFrameCount(0)
,m_nFrameRate(0)
,m_nFrameDelay(0)
,m_bLoop(true)
,m_bAutoStart(false)
{
	SetMouseEnabled(false);
	SetStyle(_T("AnimationCtrl"));
	m_pImageStyle	= NULL;
	m_pAnimation	= NULL;
}

CAnimationCtrlUI::~CAnimationCtrlUI()
{

}

void CAnimationCtrlUI::Init()
{
	__super::Init();

	if (m_bAutoStart)
	{
		StartAnimation();
	}
}

void CAnimationCtrlUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("image")) == 0)
	{
		do
		{
			m_pImageStyle = dynamic_cast<ImageStyle*>(GetStyle(_T("anim")));
			if (!m_pImageStyle)
			{
				break;
			}

			m_pImageStyle->SetAttribute(lpszName, lpszValue);

		} while (false);
	}
// 	else if (equal_icmp(lpszName, _T("framedelay")))
// 		m_nFrameDelay = _ttoi(lpszValue);
	else if (_tcsicmp(lpszName, _T("autostart")) == 0)
	{
		m_bAutoStart = !!_ttoi(lpszValue);

		if (m_bAutoStart)
		{
			StartAnimation();
		}
	}		
	else
		CControlUI::SetAttribute(lpszName, lpszValue);
}

void CAnimationCtrlUI::StopAnimation()
{
	if (!IsAnimationValid())
	{
		return;
	}

	m_pAnimation->Stop();
}

void CAnimationCtrlUI::StartAnimation()
{
	if (!m_pImageStyle)
	{
		return;
	}

	do 
	{
		if (m_pAnimation)
		{
			m_pAnimation->Resume();
			break;
		}

		m_pAnimation = AllocViewAnimation();
		if (!m_pAnimation)
		{
			break;
		}
		m_pAnimation->FinishedOfRelease(false);
		ImageObj* pImageObj = m_pImageStyle->GetImageObj();
		if (!pImageObj)
		{
			break;
		}

		if (m_nFrameCount == 0)
		{
			m_nFrameCount = GetDefaultFrameCount();

			if (pImageObj->GetFrameCount() > 1)
			{
				m_nFrameCount = pImageObj->GetFrameCount();
				m_nFrameRate = pImageObj->GetFrameDelay(0);
			}
			else if (pImageObj->GetCellCol() > 1)
			{
				m_nFrameCount = pImageObj->GetCellCol();
			}
			else if (pImageObj->GetCellRow() > 1)
			{
				m_nFrameCount = pImageObj->GetCellRow();
			}
		}
		else
		{
			m_nFrameCount = 1;
		}

		if (m_nFrameRate == 0)
		{
			m_nFrameRate = 60;
		}

		if (m_pAnimation)
		{
			m_pAnimation->SetParam(m_nFrameRate, m_nFrameCount ? m_nFrameCount : 1, m_bLoop);
			ActionManager::GetInstance()->addViewAnimation(m_pAnimation, m_pImageStyle, false);
		}

	} while (false);
}

void CAnimationCtrlUI::SetLoop(bool bLoop)
{
	m_bLoop = bLoop;

	if (!IsAnimationValid())
	{
		return;
	}

	m_pAnimation->SetParam(m_nFrameRate, m_nFrameCount, bLoop);
}

int CAnimationCtrlUI::GetFrameCount()
{
	if (!IsAnimationValid())
	{
		return 1;
	}

	return m_pAnimation->GetTotalFrame();
}

int CAnimationCtrlUI::GetCurFrame()
{
	if (!IsAnimationValid())
	{
		return 0;
	}

	return m_pAnimation->GetCurFrame();
}

void CAnimationCtrlUI::SetCurFrame(int nCurFrame)
{
	if (!IsAnimationValid())
	{
		return;
	}

	m_pAnimation->SetCurFrame(nCurFrame);
}

void CAnimationCtrlUI::SetFrameRate(int nRate)
{
	m_nFrameRate = nRate;

	if (!IsAnimationValid())
	{
		return;
	}

	m_pAnimation->SetParam(m_nFrameRate, m_nFrameCount, m_bLoop);

}

ViewAnimation* CAnimationCtrlUI::AllocViewAnimation()
{
	return new ImageSequenceAnimation;
}

bool CAnimationCtrlUI::IsAnimationValid()
{
	if (!m_pImageStyle)
	{
		return false;
	}

	if (!m_pAnimation)
	{
		return false;
	}

	return true;
}

int CAnimationCtrlUI::GetDefaultFrameCount()
{
	return 1;
}

void CAnimationCtrlUI::SetAnimatable(bool bStartAndShow)
{
	SetVisible(bStartAndShow);
	if (bStartAndShow)
	{
		StartAnimation();
	}
	else
	{
		StopAnimation();
	}
}
