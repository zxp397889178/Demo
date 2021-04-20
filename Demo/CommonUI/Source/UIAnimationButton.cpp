#include "stdafx.h"
#include "UIAnimationButton.h"

#define TIMER_ID_HIDE 1

void CUIAnimationMouseEnter::SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId)
{
    __super::SetAnimationControl(pControl, lpszStyleId);
    if (!m_pControl)
        return;

    m_pImageStyle = m_pControl->GetStyle(lpszStyleId);

    if (!m_pImageStyle)
        return;

    m_pControl->OnEvent += MakeDelegate(this, &CUIAnimationMouseEnter::EventMouse);

    m_spViewAnimation = std::make_shared<ImageSequenceAnimation>();
    if (m_spViewAnimation)
    {
        m_spViewAnimation->SetCurveID(_T("curve.liner"));
        m_spViewAnimation->SetParam(83, 0, false);
        m_spViewAnimation->FinishedOfRelease(false);
        ActionManager::GetInstance()->addViewAnimation(m_spViewAnimation.get(), m_pImageStyle, true);
        m_spViewAnimation->Paused();
    }
}

bool CUIAnimationMouseEnter::EventMouse(TEventUI& event)
{
    if (!m_pControl
        || !m_spViewAnimation
        || !m_pImageStyle)
    {
        return true;
    }

    switch (event.nType)
    {
    case UIEVENT_MOUSEENTER:
        if (!m_pControl->IsSelected())
        {
            m_spViewAnimation->SetCurFrame(m_spViewAnimation->GetCurFrame(), false);
            m_spViewAnimation->Resume();
        }
        break;
    case UIEVENT_MOUSELEAVE:
    case UIEVENT_BUTTONDOWN:
        m_spViewAnimation->Stop();
        break;
    }
    return true;
}

void CUIAnimationMouseEnter::Resume()
{
	if (m_spViewAnimation)
	{
		m_spViewAnimation->Resume();
	}
}

void CUIAnimationMouseEnter::SetCurFrame(DWORD dwCurFrame, bool bReverse /*= false*/)
{
	if (m_spViewAnimation)
	{
		m_spViewAnimation->SetCurFrame(dwCurFrame, bReverse);
	}
}

CUIAnimationMouseEnter::~CUIAnimationMouseEnter()
{
}

void CUIAnimationMouseEnter::SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop /*= false*/, bool bRebound /*= false*/, bool bByStep /*= true*/)
{
	if (m_spViewAnimation)
	{
		m_spViewAnimation->SetParam(uInterval, uTotalFrame, bLoop, bRebound, bByStep);
	}
}

void CUITreeNodeAnimation::SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId)
{
	__super::SetAnimationControl(pControl, lpszStyleId);
	if (!m_pControl)
		return;

	m_pImageStyle = m_pControl->GetStyle(lpszStyleId);

	if (!m_pImageStyle)
		return;

	m_spViewAnimation = std::make_shared<ImageRotateAnimation>();
	if (m_spViewAnimation)
	{
		m_spViewAnimation->SetCurveID(_T("curve.liner"));
		m_spViewAnimation->SetParam(83, 10, true);
		m_spViewAnimation->FinishedOfRelease(true);
		ActionManager::GetInstance()->addViewAnimation(m_spViewAnimation.get(), m_pImageStyle, true);
		m_spViewAnimation->Paused();
	}
}

void CUITreeNodeAnimation::Pause()
{
	m_spViewAnimation->Paused();
}

void CUITreeNodeAnimation::Resume()
{
	m_spViewAnimation->Resume();
}


void CUITreeNodeAnimation::OnStop()
{
	m_spViewAnimation->Stop();
}

CUITreeNodeAnimation::~CUITreeNodeAnimation()
{
}

CInsertResourseAnimation::CInsertResourseAnimation()
{

}

CInsertResourseAnimation::~CInsertResourseAnimation()
{

}

void CInsertResourseAnimation::Init()
{
    __super::Init();

    m_pAnimationControl = FindSubControl(_T("Item_ResultAnimation"));
    if (m_pAnimationControl)
    {
        m_pAnimationControl->SetVisible(false);
        ImageStyle* pObj = m_pAnimationControl->GetImageStyle(L"bk");
        if (pObj)
        {
            int nFrameCount = 1;
            ImageObj* pImageObj = pObj->GetImageObj();
            if (pImageObj)
                nFrameCount = pImageObj->GetCellNum();

            m_spImageAnimation = std::make_shared<ImageSequenceAnimation>();
            m_spImageAnimation->SetCurveID(_T("curve.liner"));
            m_spImageAnimation->SetParam(83, nFrameCount, false);
            m_spImageAnimation->FinishedOfRelease(false);
            m_spImageAnimation->SetActionListener(this);
            ActionManager::GetInstance()->addViewAnimation(m_spImageAnimation.get(), pObj, false);
            m_spImageAnimation->Paused();
        }
    }

    m_pResultLabel = FindSubControl(_T("Item_ResultLabel"));
    if (m_pResultLabel)
        m_pResultLabel->SetVisible(false);
}

void CInsertResourseAnimation::ShowResult(ResAnimation resAniType)
{
    SetVisible(true);
    if (m_pResultLabel)
    {
		m_pResultLabel->SetAttribute(_T("txtsuccessed.visible"),  (resAniType == ResAnimation::InsertSuccess) ? _T("1") : _T("0"));
		m_pResultLabel->SetAttribute(_T("txtfailed.visible"),	  (resAniType == ResAnimation::InsertFail) ? _T("1") : _T("0"));
		m_pResultLabel->SetAttribute(_T("txtsavesuccessed.visible"), (resAniType == ResAnimation::SaveSuccess) ? _T("1") : _T("0"));
		m_pResultLabel->SetAttribute(_T("txtsavefailed.visible"), (resAniType == ResAnimation::SaveFail) ? _T("1") : _T("0"));
		m_pResultLabel->SetAttribute(_T("txtdownfailed.visible"), (resAniType == ResAnimation::DownloadFail) ? _T("1") : _T("0"));
        m_pResultLabel->SetVisible(false);
    }

	tstring strImage;
	switch (resAniType)
	{
	case InsertSuccess:
	case SaveSuccess:
		strImage = _T("#item_insert_successed");
		break;
	case InsertFail:
	case SaveFail:
		strImage = _T("#item_insert_failed");
		break;
	case DownloadFail:
		strImage = _T("#item_down_failed");
		break;	
	default:
		strImage = _T("#item_insert_successed");
		break;
	}

    if (m_pAnimationControl)
    {
		m_pAnimationControl->SetAttribute(_T("bk.image"), strImage.c_str());
        m_pAnimationControl->SetVisible(true);
    }

    Resize();
    Invalidate();
    if (m_spImageAnimation)
    {
        m_spImageAnimation->SetCurFrame(1, false);
        m_spImageAnimation->Resume();
    }
}

void CInsertResourseAnimation::OnActionStateChanged(CAction* pAction)
{
    if (pAction
        && pAction->GetState() == CAction::ActionState_Finished)
    {
        if (m_pResultLabel)
            m_pResultLabel->SetVisible(true);
        Resize();
        Invalidate();

        SetTimer(TIMER_ID_HIDE, 1000);
    }
}

bool CInsertResourseAnimation::Event(TEventUI& event)
{
    if (event.nType == UIEVENT_TIMER
        && event.wParam == TIMER_ID_HIDE)
    {
        SetVisible(false);
        KillTimer(TIMER_ID_HIDE);
    }
    return __super::Event(event);
}

void CUIGifAnimation::SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId)
{
    __super::SetAnimationControl(pControl, lpszStyleId);
    if (!m_pControl)
        return;

    m_pImageStyle = m_pControl->GetImageStyle(lpszStyleId);

    if (!m_pImageStyle)
        return;

    m_spViewAnimation = std::make_shared<ImageSequenceAnimation>();
    if (m_spViewAnimation)
    {
        m_spViewAnimation->SetCurveID(_T("curve.liner"));
        m_spViewAnimation->SetParam(83, 0, true);
        m_spViewAnimation->FinishedOfRelease(false);
        ActionManager::GetInstance()->addViewAnimation(m_spViewAnimation.get(), m_pImageStyle, true);
        m_spViewAnimation->Paused();
    }
}

void CUIGifAnimation::Pause()
{
    if (m_spViewAnimation)
    {
        m_spViewAnimation->Paused();
    }
}

void CUIGifAnimation::Resume()
{
    if (m_spViewAnimation)
    {
        m_spViewAnimation->Resume();
    }
}

void CUIGifAnimation::SetFrameCount(int nCount)
{
    if (m_spViewAnimation)
    {
        m_spViewAnimation->SetParam(83, nCount, true);
    }
}

void CUIGifAnimation::SetAnimationId(LPCTSTR lpctId)
{
	if (lpctId
		&& m_spViewAnimation)
	{
		m_spViewAnimation->SetId(lpctId);
	}
}

void CUIGifAnimation::SetCurFrame(DWORD dwCurFrame, bool bReverse /*= false*/)
{
	if (m_spViewAnimation)
	{
		m_spViewAnimation->SetCurFrame(dwCurFrame, bReverse);
	}
}

void CUIGifAnimation::SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop /*= false*/, bool bRebound /*= false*/, bool bByStep /*= true*/)
{
	if (m_spViewAnimation)
	{
		m_spViewAnimation->SetParam(uInterval, uTotalFrame, bLoop, bRebound, bByStep);
	}
}

void CUIGifAnimation::SetActionListener(IActionListener* val)
{
	if (m_spViewAnimation && val)
	{
		m_spViewAnimation->SetActionListener(val);
	}
}

void CUIGifAnimation::Stop()
{
	if (m_spViewAnimation)
	{
		m_spViewAnimation->Stop();
	}
}

CUIMarqueeAnimation::CUIMarqueeAnimation() : m_spPosAnimation(NULL)
{
	m_pTextStyle = NULL;
	
	SetRectEmpty(&m_rtTxtOriginal);

	m_nTxtWidth     = 0;
	m_nLastTxtWidth = 0;

	m_fMoveRate		= 1.07f;
	m_uInterval     = 20;
}

void CUIMarqueeAnimation::SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId)
{
	if (!pControl
		|| !lpszStyleId)
	{
		return;
	}

	__super::SetAnimationControl(pControl, lpszStyleId);

	m_pTextStyle = m_pControl->GetTextStyle(lpszStyleId);

	if (!m_pTextStyle)
		return;

	m_rtTxtOriginal = m_pTextStyle->GetRect();
}


void CUIMarqueeAnimation::InitPosAnimation()
{
	if (!m_pTextStyle
		|| !m_pControl)
	{
		return;
	}

	do 
	{
		if (m_spPosAnimation)
		{
			break;
		}

		m_spPosAnimation = std::make_shared<PosChangeAnimation>();		
		m_spPosAnimation->SetCurveID(_T("curve.liner"));
		m_spPosAnimation->FinishedOfRelease(false);		
		ActionManager::GetInstance()->addViewAnimation(m_spPosAnimation.get(), m_pTextStyle, true);

	} while (false);	

	if (m_nLastTxtWidth != m_nTxtWidth)
	{
		m_nLastTxtWidth = m_nTxtWidth;		

		RECT rcItem = m_pControl->GetRect();
		DPI_SCALE_BACK(&rcItem);
		RECT rcTxt = m_rtTxtOriginal;
		DPI_SCALE_BACK(&rcTxt);

		Coordinate stBeginCoordinate;
		stBeginCoordinate.anchor	  = UIANCHOR_LTLB;
		stBeginCoordinate.rect.left	  = rcItem.right - rcItem.left; //头从最右边开始移动
		stBeginCoordinate.rect.right  = stBeginCoordinate.rect.left + m_nTxtWidth;
		stBeginCoordinate.rect.top	  = rcTxt.top - rcItem.top;
		stBeginCoordinate.rect.bottom = rcItem.bottom - rcTxt.bottom;

		Coordinate stEndCoordinate;
		stEndCoordinate.anchor	      = UIANCHOR_RTRB;
		stEndCoordinate.rect.right	  = rcItem.right - rcItem.left;//尾移动到最左边结束移动
		stEndCoordinate.rect.left     = stEndCoordinate.rect.right + m_nTxtWidth;
		stEndCoordinate.rect.top	  = rcTxt.top - rcItem.top;
		stEndCoordinate.rect.bottom	  = rcItem.bottom - rcTxt.bottom;

		int nTotalFrame = (int)(m_nTxtWidth * m_fMoveRate);
		m_spPosAnimation->SetParam(m_uInterval, nTotalFrame, true);
		m_spPosAnimation->SetKeyFrameCoordinate(&stBeginCoordinate, &stEndCoordinate);
		m_spPosAnimation->Stop();
	}	
}

void CUIMarqueeAnimation::Pause()
{
	if (m_spPosAnimation)
	{
		m_spPosAnimation->Paused();
	}
}

void CUIMarqueeAnimation::Resume()
{
	if (m_spPosAnimation)
	{
		m_spPosAnimation->Resume();
	}
}

void CUIMarqueeAnimation::SetFrameCount(int nCount)
{
	if (m_spPosAnimation)
	{
		m_spPosAnimation->SetParam(83, nCount, true);
	}
}

void CUIMarqueeAnimation::SetAnimationInfo(int nTxtWidth)
{
	m_nTxtWidth = nTxtWidth;
}

bool CUIMarqueeAnimation::ShowAnimation()
{
	InitPosAnimation();

	if (m_spPosAnimation)
	{
		m_spPosAnimation->SetCurFrame(0, false);
		m_spPosAnimation->Resume();
	}

	return true;
}

bool CAnimationManagerEx::RemoveAnimationEx(CControlUI* pControl)
{
	auto spAnimation = m_mapAnimation.find(pControl);
	if (spAnimation != m_mapAnimation.end())
	{
		spAnimation->second->Pause();
		m_mapAnimation.erase(spAnimation);
	}

	return true;
}

std::shared_ptr<CUITreeNodeAnimation> CAnimationManagerEx::GetTreeAnimationEx(CControlUI* pControl)
{
	auto spAnimation = m_mapAnimation.find(pControl);
	if (spAnimation != m_mapAnimation.end())
	{
		return spAnimation->second;
	}
	return nullptr;
}