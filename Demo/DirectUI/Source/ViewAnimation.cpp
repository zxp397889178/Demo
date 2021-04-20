#include "stdafx.h"
#include "ViewAnimation.h"

ViewAnimation::~ViewAnimation()
{
	ActionManager::GetInstance()->removeViewAnimation(this);
}

ViewAnimation::ViewAnimation()
{

}

void AlphaChangeAnimation::step(IActionInterval* pActionInterval)
{
	//TCHAR sz[100];
	//wsprintf(sz, _T("\r\n%d,%d=================================\r\n"), pActionInterval->GetCurFrame(), pActionInterval->GetTotalFrame());
	//OutputDebugString(sz);
	CViewUI* pView = static_cast<CViewUI*>(ActionTarget());
	if (pView)
	{
		ActionInterval::FadeInterval(pView, GetCurveObj(), pActionInterval, m_nStartAlpha, m_nEndAlpha);
	}
}

LPCTSTR AlphaChangeAnimation::GetClass()
{
	return _T("AlphaChangeAnimation");
}

void AlphaChangeAnimation::SetKeyFrameAlpha( int startAlpha, int endAlpha )
{
	m_nStartAlpha = startAlpha;
	m_nEndAlpha = endAlpha;
}

void AlphaChangeAnimation::startWithTarget( BaseObject* pActionTarget )
{
	RT_CLASS_ASSERT(pActionTarget, CViewUI, _T("AlphaChangeAnimation 必须绑定CViewUI"));
	__super::startWithTarget(pActionTarget);
}

void AlphaChangeAnimation::onStop()
{
	AlphaChangeAnimation::onFinished();
}

void AlphaChangeAnimation::onFinished()
{
	CViewUI* pView = static_cast<CViewUI*>(ActionTarget());
	if (pView)
	{
		pView->SetAlpha(IsReverse()?m_nStartAlpha:m_nEndAlpha);
		pView->Invalidate();
	}
}
//////////////////////////////////////////////////////////////////////////
LPCTSTR PosChangeAnimation::GetClass()
{
	return _T("PosChangeAnimation");
}

void PosChangeAnimation::step( IActionInterval* pActionInterval )
{
	CViewUI* pView = static_cast<CViewUI*>(ActionTarget());
	if (pView)
	{
		RECT rcParent = {0};
		StyleObj* pStyleObj = dynamic_cast<StyleObj*>(pView);
		CControlUI* pControl = NULL;
		if (pStyleObj)
		{
			CControlUI* pParent = pStyleObj->GetOwner();
			if (pParent)
				rcParent= pParent->GetRect();
		}
		else
		{
			pControl = dynamic_cast<CControlUI*>(pView);
			if (pControl)
			{
				rcParent = pControl->GetParent()->GetRect();
			}
		}
		if (!IsRectEmpty(&rcParent))
		{
			RECT rcStart = m_startCoordinate.rect;
			DPI_SCALE(&rcStart);
			GetAnchorPos(m_startCoordinate.anchor, &rcParent, &rcStart);

			RECT rcEnd   = m_endCoordinate.rect;
			DPI_SCALE(&rcEnd);
			GetAnchorPos(m_endCoordinate.anchor, &rcParent, &rcEnd);

			ActionInterval::PosInterval(pView, GetCurveObj(), pActionInterval, rcStart, rcEnd);
		}
	}
}

void PosChangeAnimation::startWithTarget( BaseObject* pActionTarget )
{
	RT_CLASS_ASSERT(pActionTarget, CViewUI, _T("PosChangeAnimation 必须绑定CViewUI"));
	__super::startWithTarget(pActionTarget);
}

void PosChangeAnimation::onStop()
{
// 	CViewUI* pView = static_cast<CViewUI*>(ActionTarget());
// 	if (pView)
// 	{
// 		pView->Invalidate();
// 		pView->SetCoordinate(IsReverse()?m_startCoordinate:m_endCoordinate);
// 		pView->Resize();
// 		pView->Invalidate();
// 	}
	PosChangeAnimation::onFinished();
}

void PosChangeAnimation::onFinished()
{
	CViewUI* pView = static_cast<CViewUI*>(ActionTarget());
	if (pView)
	{
		pView->Invalidate();
		pView->SetCoordinate(IsReverse()?m_startCoordinate:m_endCoordinate);
		pView->Resize();
		pView->Invalidate();
	}
}

void PosChangeAnimation::SetKeyFrameCoordinate( const Coordinate* pStartCoordinate, const Coordinate* pEndCoordinate )
{
	m_startCoordinate = *pStartCoordinate;
	m_endCoordinate = *pEndCoordinate;
}
//////////////////////////////////////////////////////////////////////////
LPCTSTR ImageSequenceAnimation::GetClass()
{
	return _T("SeqFrameAnimation");
}

void ImageSequenceAnimation::step( IActionInterval* pActionInterval )
{
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(ActionTarget());
	if (pImageStyle)
	{
		ImageObj* pImageObj = pImageStyle->GetImageObj();
		if (pImageObj)
		{
			if (!pActionInterval->IsLoop()
				&& pActionInterval->GetCurFrame() > pImageObj->GetCellNum())
			{
				pActionInterval->Stop();
				return;
			}
		}
		
		pImageStyle->SetImgSeq(pActionInterval->GetCurFrame());
		pImageStyle->Invalidate();
	}
}

void ImageSequenceAnimation::startWithTarget( BaseObject* pActionTarget )
{
	RT_CLASS_ASSERT(pActionTarget, ImageStyle, _T("SeqFrameAnimation 必须绑定ImageStyle"));
	__super::startWithTarget(pActionTarget);
}

void ImageSequenceAnimation::onStop()
{
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(ActionTarget());
	if (pImageStyle)
	{
		pImageStyle->SetImgSeq(IsReverse()?0:GetTotalFrame());
		pImageStyle->Invalidate();
	}
}

void ImageSequenceAnimation::onFinished()
{
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(ActionTarget());
	if (pImageStyle)
	{
		pImageStyle->SetImgSeq(GetCurFrame());
		pImageStyle->Invalidate();
	}
}
//////////////////////////////////////////////////////////////////////////
LPCTSTR ImageRotateAnimation::GetClass()
{
	return _T("ImageRotateAnimation");
}

void ImageRotateAnimation::step(IActionInterval* pActionInterval)
{
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(ActionTarget());
	if (pImageStyle)
	{
		int nAngle = 360.0f * pActionInterval->GetCurFrame() / pActionInterval->GetTotalFrame();
		pImageStyle->SetRoate(true, nAngle);
		pImageStyle->Invalidate();
	}
}

void ImageRotateAnimation::startWithTarget(BaseObject* pActionTarget)
{
	RT_CLASS_ASSERT(pActionTarget, ImageStyle, _T("SeqFrameAnimation 必须绑定ImageStyle"));
	__super::startWithTarget(pActionTarget);
}

void ImageRotateAnimation::onStop()
{
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(ActionTarget());
	if (pImageStyle)
	{
		int nAngle = 360.0f * GetCurFrame() / GetTotalFrame();
		pImageStyle->SetRoate(false, nAngle);
		pImageStyle->Invalidate();
	}
}

void ImageRotateAnimation::onFinished()
{
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(ActionTarget());
	if (pImageStyle)
	{
		pImageStyle->SetRoate(false, 0);
		pImageStyle->Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////////
TransitionAnimation::TransitionAnimation()
{
	m_pOutView = NULL;
	m_pInView  = NULL;
}

CViewUI* TransitionAnimation::GetOut()
{
	return m_pOutView;
}

CViewUI* TransitionAnimation::GetIn()
{
	return m_pInView;
}

void TransitionAnimation::BindObj( CViewUI* pOut, CViewUI* pIn )
{
	m_pOutView = pOut;
	m_pInView = pIn;
}

void TransitionAnimation::startWithTarget( BaseObject* pActionTarget )
{
	CViewUI* pOutView = GetOut();
	CViewUI* pInView = GetIn();
	if (pOutView)
		ActionManager::GetInstance()->addViewAnimationPair(this, pOutView);
	if (pInView)
		ActionManager::GetInstance()->addViewAnimationPair(this, pInView);
	__super::startWithTarget(pActionTarget);
}
//////////////////////////////////////////////////////////////////////////
LPCTSTR TurnTransition::GetClass()
{
	return _T("TurnObjectAnimation");
}

void TurnTransition::step( IActionInterval* pActionInterval )
{
	ImageStyle* pOutImage = dynamic_cast<ImageStyle*>(GetOut());
	ImageStyle* pInImage = dynamic_cast<ImageStyle*>(GetIn());
	int nAngle = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, 180, pActionInterval->GetTotalFrame());
	bool bFront = true;
	if (pInImage)
		bFront = nAngle < 90;
	if (pOutImage)
	{
		pOutImage->SetVisible(bFront);
		if (pInImage)
			pOutImage->SetAngle(true, nAngle);
		else
			pOutImage->SetAngle(true, nAngle > 90 ? 180 - nAngle : nAngle);
		pOutImage->Invalidate();
	}
	if (pInImage)
	{
		pInImage->SetVisible(!bFront);
		pInImage->SetAngle(true, 180 - nAngle);
		pInImage->Invalidate();
	}
}

void TurnTransition::onStop()
{
	ImageStyle* pOutImage = dynamic_cast<ImageStyle*>(GetOut());
	ImageStyle* pInImage = dynamic_cast<ImageStyle*>(GetIn());
	bool bFront = (pOutImage == NULL) ? true : false;

	if (pOutImage)
	{
		pOutImage->SetVisible(bFront);
		pOutImage->SetAngle(false, 0);
		pOutImage->Invalidate();
	}
	if (pInImage)
	{
		pInImage->SetVisible(!bFront);
		pInImage->SetAngle(false, 0);
		pInImage->Invalidate();
	}
}

void TurnTransition::onFinished()
{
	ImageStyle* pOutImage = dynamic_cast<ImageStyle*>(GetOut());
	ImageStyle* pInImage = dynamic_cast<ImageStyle*>(GetIn());
	bool bFront = true;
	if (pInImage)
	{
		if (GetCurFrame() == GetTotalFrame())
			bFront = false;
		else
			bFront = true;
	}
	else
	{
		bFront = true;
	}

	if (pOutImage)
	{
		pOutImage->SetVisible(bFront);
		pOutImage->SetAngle(false, 0);
		pOutImage->Invalidate();
	}
	if (pInImage)
	{
		pInImage->SetVisible(!bFront);
		pInImage->SetAngle(false, 0);
		pInImage->Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////////
SlideTransition::SlideTransition()
{
	m_SlideType = SlideAnimation_Left;
}

LPCTSTR SlideTransition::GetClass()
{
	return _T("SlideAnimation");
}

void SlideTransition::step( IActionInterval* pActionInterval )
{
	CControlUI* pControl = dynamic_cast<CControlUI*>(ActionTarget());
	if (!pControl)
		return;
	RECT rcStart = pControl->GetRect();
	int nOffsetX = 0;
	int nOffsetY = 0;
	bool bOffsetX = false;
	bool bOffsetY = false;
	bool bChangeX = false;
	bool bChangeY = false;
	switch (m_SlideType)
	{
	case SlideAnimation_Left:
		{
			bOffsetX = true;
			bChangeX = false;
		}
		break;
	case SlideAnimation_Right:
		{
			bOffsetX = true;
			bChangeX = true;
		}
		break;
	case SlideAnimation_Top:
		{
			bOffsetY = true;
			bChangeY = false;
		}
		break;
	case SlideAnimation_Bottom:
		{
			bOffsetY = true;
			bChangeY = true;
		}
		break;
	case SlideAnimation_LeftTop:
		{
			bOffsetX = true;
			bOffsetY = true;
		}
		break;
	case SlideAnimation_RightTop:
		{
			bOffsetX = true;
			bOffsetY = true;
			bChangeX = true;
		}
		break;
	case SlideAnimation_LeftBottom:
		{
			bOffsetX = true;
			bOffsetY = true;
			bChangeY = true;
		}
		break;
	case SlideAnimation_RightBottom:
		{
			bOffsetX = true;
			bOffsetY = true;
			bChangeX = true;
			bChangeY = true;
		}
		break;
	}
	if (bOffsetX)
	{
		nOffsetX = rcStart.right - rcStart.left;
		if (!bChangeX)
			nOffsetX = -nOffsetX;
	}
	if (bOffsetY)
	{
		nOffsetY = rcStart.bottom - rcStart.top;
		if (!bChangeY)
			nOffsetY = -nOffsetY;
	}

	RECT rcEnd = rcStart;
	::OffsetRect(&rcEnd, nOffsetX, nOffsetY);

	RECT rcDest = ActionInterval::PosInterval(GetOut(), GetCurveObj(), pActionInterval, rcStart, rcEnd);

	CViewUI* pInView = GetIn();
	if (pInView)
	{
		::OffsetRect(&rcDest, -nOffsetX, -nOffsetY);

		CViewUI* pView = pInView;
		if (pView)
		{
			pView->SetVisible(true);
			pView->SetRect(rcDest);
			pView->Invalidate();
		}
	}
	pControl->Invalidate();
}

void SlideTransition::onStop()
{
	CControlUI* pControl = dynamic_cast<CControlUI*>(ActionTarget());
	if (!pControl)
		return;
	RECT rcStart = pControl->GetRect();

	CViewUI* pOutView = GetOut();
	CViewUI* pInView = GetIn();
	bool bFront = (pOutView == NULL) ? true : false;

	pControl->GetWindow()->LockUpdate();
	if (pOutView)
	{
		if (bFront)
			pOutView->SetRect(rcStart);
		pOutView->SetVisible(bFront);
	}
	if (pInView)
	{
		if (!bFront)
			pInView->SetRect(rcStart);
		pInView->SetVisible(!bFront);
	}
	pControl->GetWindow()->UnLockUpdate();
	pControl->Invalidate();
}

void SlideTransition::onFinished()
{
	CControlUI* pControl = dynamic_cast<CControlUI*>(ActionTarget());
	if (!pControl)
		return;
	RECT rcStart = pControl->GetRect();

	CViewUI* pOutView = GetOut();
	CViewUI* pInView = GetIn();
	bool bFront = true;
	if (pInView)
	{
		if (GetCurFrame() == GetTotalFrame())
			bFront = false;
		else
			bFront = true;
	}
	else
	{
		bFront = true;
	}

	pControl->GetWindow()->LockUpdate();
	if (pOutView)
	{
		if (bFront)
			pOutView->SetRect(rcStart);
		pOutView->SetVisible(bFront);
	}
	if (pInView)
	{
		if (!bFront)
			pInView->SetRect(rcStart);
		pInView->SetVisible(!bFront);
	}
	pControl->GetWindow()->UnLockUpdate();
	pControl->Invalidate();
}

void SlideTransition::SetSlideType( SlideType slideType )
{
	m_SlideType = slideType;
}

//////////////////////////////////////////////////////////////////////////
LPCTSTR FadeTransition::GetClass()
{
	return _T("FadeTransition");
}

void FadeTransition::step( IActionInterval* pActionInterval )
{
	ActionInterval::FadeInterval(GetOut(), GetCurveObj(), pActionInterval, 255, 0);
	ActionInterval::FadeInterval(GetIn(), GetCurveObj(), pActionInterval, 0, 255);
}

void FadeTransition::onStop()
{
	CViewUI* pOutView = GetOut();
	CViewUI* pInView = GetIn();
	bool bFront = (pOutView == NULL) ? true : false;

	if (pOutView)
	{
		pOutView->SetAlpha(255);
		pOutView->SetVisible(bFront);
	}
	if (pInView)
	{
		pInView->SetAlpha(255);
		pInView->SetVisible(!bFront);
	}
}

void FadeTransition::onFinished()
{
	CViewUI* pOutView = GetOut();
	CViewUI* pInView = GetIn();
	bool bFront = true;
	if (pInView)
	{
		if (GetCurFrame() == GetTotalFrame())
			bFront = false;
		else
			bFront = true;
	}
	else
	{
		bFront = true;
	}

	if (pOutView)
	{
		pOutView->SetAlpha(255);
		pOutView->SetVisible(bFront);
	}
	if (pInView)
	{
		pInView->SetAlpha(255);
		pInView->SetVisible(!bFront);
	}
}