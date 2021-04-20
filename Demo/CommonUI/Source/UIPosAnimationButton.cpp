#include "stdafx.h"
#include "UIAutoSizeButton.h"
#include "UIPosAnimationButton.h"


#define TIMER_ID_HIDE 1

CUIPosAnimationButton::CUIPosAnimationButton() :m_spViewAnimation(nullptr)
{
	SetStyle(_T("PostAinmationButton"));
}

CUIPosAnimationButton::~CUIPosAnimationButton()
{

}

void CUIPosAnimationButton::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);
	if (!m_spViewAnimation)
	{
		return;
	}
	if (bVisible && IsInternVisible())
	{
		m_spViewAnimation->Resume();
	}
	else
	{
		m_spViewAnimation->Stop();
	}
}

void CUIPosAnimationButton::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);
	//if (!m_spViewAnimation)
	//{
	//	return;
	//}
	//if (bVisible && IsVisible())
	//{
	//	m_spViewAnimation->Resume();
	//}
	//else
	//{
	//	m_spViewAnimation->Stop();
	//}

}

void CUIPosAnimationButton::StartAnimation()
{
	if (!m_spViewAnimation)
	{
		return;
	}
	auto InitializeCoordinate = [&](Coordinate* coordinate, int nCoordinate, int left, int top, int right, int bottom)
	{
		coordinate->anchor = (UITYPE_ANCHOR)nCoordinate;
		coordinate->rect.left = left;
		coordinate->rect.top = top;
		coordinate->rect.right = right;
		coordinate->rect.bottom = bottom;

	};
	auto pImageStyle = GetImageStyle(_T("ani_bk"));
	if (pImageStyle)
	{
		RECT rcStart = pImageStyle->GetRect();
		RECT rcThis = GetRect();
		int nThisWidth = rcThis.right - rcThis.left;
		int nWidth = rcStart.right - rcStart.left;
		int nHeight = rcStart.bottom - rcStart.top;
		Coordinate stEndCoordinate;
		InitializeCoordinate(&stEndCoordinate, UIANCHOR_LT, nThisWidth + nWidth, 0, nWidth, nHeight);
		Coordinate stStartCoordinate;
		InitializeCoordinate(&stStartCoordinate, UIANCHOR_RT, nThisWidth + nWidth, 0, nWidth, nHeight);
		m_spViewAnimation->SetKeyFrameCoordinate(&stStartCoordinate, &stEndCoordinate);
		m_spViewAnimation->Resume();
	}
}

void CUIPosAnimationButton::StopAnimation()
{
	if (m_spViewAnimation)
	{
		m_spViewAnimation->Paused();
	}
}

void CUIPosAnimationButton::Init()
{
	__super::Init();
	
	auto pImageStyle = GetImageStyle(_T("ani_bk"));
	if (pImageStyle)
	{
		m_spViewAnimation = new PosChangeAnimation;
		m_spViewAnimation->SetParam(1, 95, true);
		m_spViewAnimation->SetCurveID(_T("curve.circ.in"));
		m_spViewAnimation->FinishedOfRelease(false);
		ActionManager::GetInstance()->addViewAnimation(m_spViewAnimation, pImageStyle, false);
	}
}
