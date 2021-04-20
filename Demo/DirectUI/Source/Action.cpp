#include "stdafx.h"
#include "Action.h"


int ActionInterval::FadeInterval( CViewUI* pView, CurveObj* pCurveObj, IActionInterval* pInterval, int startAlpha, int endAlpha )
{
	int nAlpha = startAlpha;
	if (pCurveObj && pInterval)
		nAlpha = (int)BezierAlgorithm::BezierValue(pCurveObj, pInterval->GetCurFrame(), startAlpha, endAlpha - startAlpha, pInterval->GetTotalFrame());

	if (pView)
	{
		pView->SetAlpha(nAlpha);
		pView->Invalidate();
	}
	return nAlpha;
}

RECT ActionInterval::PosInterval( CViewUI* pView, CurveObj* pCurveObj, IActionInterval* pInterval, RECT& rcStart, RECT& rcEnd )
{
	RECT rcDest = rcStart;
	if (pCurveObj)
		rcDest = BezierAlgorithm::BezierRect(pCurveObj, pInterval->GetCurFrame(), rcStart, rcEnd, pInterval->GetTotalFrame());

	if (pView)
	{
		// 先把以前的区域刷掉
		pView->SetVisible(false);
		pView->Invalidate();

		// 再刷新
		pView->SetVisible(true);
		pView->SetRect(rcDest);
		pView->Invalidate();
	}
	return rcDest;
}


//////////////////////////////////////////////////////////////////////////
CAction::CAction()
{
	m_pActionInterval = new CUITimerBase;
	m_pActionInterval->SetUserData(this);
	m_pActionInterval->SetTimerCallback(CAction::ActionTimerCallback);
	m_pActionListener = NULL;
	m_pActionTarget   = NULL;
	m_bFinishedOfRelease = true;
	SetState(CAction::ActionState_Ready);
	SetCurveID(_T(""));
}

CAction::~CAction()
{
	if (m_pActionInterval)
		delete m_pActionInterval;
	m_pActionInterval = NULL;
}

void CAction::startWithTarget( BaseObject* pActionTarget )
{
	ActionTarget(pActionTarget);
	SetState(CAction::ActionState_Running);
}

void CAction::Resume()
{
	SetState(ActionState_Running);
}

void CAction::Paused()
{
	SetState(ActionState_Paused);
}

void CAction::update( IActionInterval* pActionInterval )
{
	if (GetState() == CAction::ActionState_Running)
	{
		step(pActionInterval);
		if (m_pActionListener)
			m_pActionListener->OnActionStep(this, pActionInterval);
	}
	if (pActionInterval->IsDone())
	{
		SetState(ActionState_Finished);
	}
}

void CALLBACK CAction::ActionTimerCallback(CUITimerBase* pTimer)
{
	CAction* pAction = (CAction*)pTimer->GetUserData();
	pAction->update(pTimer);

}

void CAction::SetParam( UINT uInterval, UINT uTotalFrame, bool bLoop /*= false*/, bool bRebound /*= false*/, bool bByStep /*= true*/ )
{
	m_pActionInterval->SetParam(uInterval, uTotalFrame, bLoop);
	m_pActionInterval->SetRebound(bRebound);
	m_pActionInterval->SetByStep(bByStep);
}

CAction::ActionState CAction::GetState() const
{
	return m_stateAction;
}

void CAction::SetState( ActionState val )
{
	m_stateAction = val;
	bool bFinished = false;

	switch (val)
	{
	case ActionState_Ready:
		{

		}
		break;
	case ActionState_Running:
		{
			if (m_pActionInterval->IsRunning())
				m_pActionInterval->Resume();
			else
				m_pActionInterval->Start();
		}
		break;
	case ActionState_Paused:
		{
			m_pActionInterval->Pause();
		}
		break;
	case ActionState_Stop:
		{
			m_pActionInterval->Stop();
			onStop();
		}
		break;
	case ActionState_Finished:
		{
			m_pActionInterval->Stop();
			onFinished();
			bFinished = true;
		}
		break;
	case ActionState_Unknown:
		{
			m_pActionInterval->Stop();
		}
		break;
	}
	if (m_pActionListener)
		m_pActionListener->OnActionStateChanged(this);
	if (bFinished && m_bFinishedOfRelease)
	{
		delete this;
	}
}

void CAction::SetCurveID( LPCTSTR val )
{
	m_pCurveObj = GetResObj()->GetCurveObj(val);
}

CurveObj* CAction::GetCurveObj()
{
	return m_pCurveObj;
}

BaseObject* CAction::ActionTarget() const
{
	return m_pActionTarget;
}

void CAction::ActionTarget( BaseObject* val )
{
	m_pActionTarget = val;
}

void CAction::SetActionListener( IActionListener* val )
{
	m_pActionListener = val;
}

void CAction::SetCurFrame( DWORD dwCurFrame, bool bReverse /*= false*/ )
{
	m_pActionInterval->SetCurFrame(dwCurFrame);
	m_pActionInterval->SetReverse(bReverse);
}

DWORD CAction::GetCurFrame()
{
	if (m_pActionInterval)
		return m_pActionInterval->GetCurFrame();
	return 0;
}

DWORD CAction::GetTotalFrame()
{
	if (m_pActionInterval)
		return m_pActionInterval->GetTotalFrame();
	return 0;
}

bool CAction::IsLoop() const
{
	if (m_pActionInterval)
		return m_pActionInterval->IsLoop();
	return false;
}

bool CAction::IsReverse() const
{
	if (m_pActionInterval)
		return m_pActionInterval->IsReverse();
	return false;
}

void CAction::FinishedOfRelease( bool val )
{
	m_bFinishedOfRelease = val;
}

bool CAction::IsFinishedOfRelease()
{
	return m_bFinishedOfRelease;
}

void CAction::Stop()
{
	SetState(ActionState_Stop);
}
