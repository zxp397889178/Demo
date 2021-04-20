/*********************************************************************
 *       Copyright (C) 2013,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UITimer.h"
#include "EngineObj.h"

/////////////////////////////////////////////////////////////////////////////////////////////
static CStdPtrArray g_aTimers;
static UINT	g_uTimerID = 0;
static int BASE_TIMER_INTERVAL = 10;
int get_new_base_interval()
{
	int count =  g_aTimers.GetSize();
	if (count == 0) return 0;
	int min_interval = ((CUITimerBase*)g_aTimers[0])->GetInterval();
	for (int i = 1; i < g_aTimers.GetSize() ; i++)
	{
		CUITimerBase* pTimer = (CUITimerBase*)g_aTimers[i];
		if (pTimer->GetInterval() < min_interval)
			min_interval = pTimer->GetInterval();
	}
	if (min_interval < 50)
		min_interval = 10;
	else if (min_interval <100)
		min_interval = 50;
	else if (min_interval < 500)
		min_interval = 100;
	else if (min_interval < 1000)
		min_interval = 500;
	return min_interval;

}
void CALLBACK CUITimerBase::TimerWndProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTimer)
{
	int nCount = g_aTimers.GetSize();
	if (nCount == 0) return;

	for (int i = nCount - 1; i >= 0; i--)
	{
		CUITimerBase* pTimerUI = (CUITimerBase*)g_aTimers[i];

		pTimerUI->CalInterval();

	}
}
/////////////////////////////////////////////////////////////////////////////////////
CUITimerBase::CUITimerBase()
{
	Init();
}

CUITimerBase::CUITimerBase(void* pObject, void* pFn)
{
	Init();
	m_pObject = pObject;
	m_pFn = pFn;
}

CUITimerBase::CUITimerBase(const CUITimerBase& rhs)
{
	Init();
	m_pObject = rhs.m_pObject;
	m_pFn = rhs.m_pFn; 
}

void CUITimerBase::Init()
{
	m_iInterval		= 0;
	m_iTotalTimer	= 0;
	m_bLoop			= false;
	m_lpUserData = 0;
	m_dwLastTickcount = 0;
	m_iCurFrame = 0;
	m_iTotalFrame = 0;
	m_bDone = false;
	m_bPaused = false;
	m_pObject = NULL;
	m_pFn = NULL;
	m_uId = 0;
	m_bByStep = true;
	m_bReverse = false;
	m_bAsc = true;
	m_bRunning = false;
	m_bRebound = false;
}

CUITimerBase::~CUITimerBase()
{
	Stop();
}


CUITimerBase* CUITimerBase::Copy()
{
	return new CUITimerBase(*this);
}


void CUITimerBase::SetTimerParam(UINT iInterval,UINT iTotalTimer, bool bLoop)
{
	m_iTotalFrame = 0;
	m_bLoop			= bLoop;
	m_iInterval		= iInterval < 1 ? 10 : iInterval;
	m_iTotalTimer	= iTotalTimer;
	if (m_iTotalTimer > 0)
	{
		m_iTotalTimer = (long)(m_iTotalTimer / m_iInterval) * m_iInterval;
		if (m_iTotalTimer < iTotalTimer) m_iTotalTimer += m_iInterval;
		 m_iTotalFrame = m_iTotalTimer / m_iInterval;
		// 需要自动反向，时间要双倍
	}
}

void CUITimerBase::SetParam(UINT iInterval,UINT iTotalFrame, bool bLoop)
{
	SetTimerParam(iInterval, iTotalFrame * iInterval, bLoop);
}

void CUITimerBase::SetTimerCallback(TimerCallback fn)
{
	m_pFn = fn;
}

void CUITimerBase::OnTimer()
{

}

void CUITimerBase::Start()
{
	Stop();
	
	m_bDone = false;
	m_bRunning = true;

	m_dwLastTickcount = ::GetTickCount();
	g_aTimers.Add(this);
	// 防止定时器过分频繁
	int nNewInterval = get_new_base_interval();
	if (BASE_TIMER_INTERVAL != nNewInterval)
	{
		if (g_uTimerID > 0)
			::KillTimer(GetEngineObj()->GetMsgHwnd(), g_uTimerID);
		BASE_TIMER_INTERVAL = nNewInterval;
		g_uTimerID = 0;
	}

	if (g_uTimerID == 0)
		g_uTimerID = ::SetTimer(GetEngineObj()->GetMsgHwnd(),1, BASE_TIMER_INTERVAL, TimerWndProc);
}

void CUITimerBase::Stop()
{
	g_aTimers.Remove(this);
	m_bDone = true;
	m_bRunning = false;
	if (g_aTimers.GetSize() == 0 &&  g_uTimerID > 0)
	{
		::KillTimer(GetEngineObj()->GetMsgHwnd(), g_uTimerID);
		g_uTimerID = 0;
		return;
	}
	
	// 防止定时器过分频繁
	int nNewInterval = get_new_base_interval();
	if (BASE_TIMER_INTERVAL != nNewInterval)
	{
		if (g_uTimerID > 0 )
			::KillTimer(GetEngineObj()->GetMsgHwnd(), g_uTimerID);
		BASE_TIMER_INTERVAL = nNewInterval;
		g_uTimerID = ::SetTimer(GetEngineObj()->GetMsgHwnd(),1, BASE_TIMER_INTERVAL, TimerWndProc);
	}
}

void CUITimerBase::Pause()
{
	m_bPaused = true;
}

void CUITimerBase::Resume()
{
	m_bPaused = false;
}

bool  CUITimerBase::IsDone()
{
	return  m_bDone;
};

bool CUITimerBase::IsRunning()
{
	return m_bRunning;
}

bool CUITimerBase::IsReverse()
{
	return m_bReverse;
}

void CUITimerBase::SetReverse(bool bReverse)
{
	m_bReverse = bReverse;
	m_bAsc = !m_bReverse;
}



void CUITimerBase::CalInterval()
{
	if (m_bDone) return;
	DWORD dwTickcount = ::GetTickCount();
	// 判断时间是否到了
	DWORD iInterval = dwTickcount - m_dwLastTickcount;
	if (iInterval < m_iInterval)
		return;
	m_dwLastTickcount = dwTickcount;
	if (m_bPaused ) return;

	// 增加或减少计数
	if (!m_bByStep)
	{
		int nCount = iInterval / m_iInterval;
		for (int i = 0; i < nCount; i++)
			Count();

	}	
	else
	{
		Count();
	}

	if (!m_pObject && m_pFn)
	{
		TimerCallback fn = (TimerCallback)m_pFn;
		fn(this);
	}
	else OnTimer();

}


void CUITimerBase::Count()
{
	if (m_iTotalTimer <= 0)
	{
		m_iCurFrame++;
		return;
	}

	// 非反弹的情况情况下
	if (!m_bRebound)
	{
		if (!m_bReverse)
		{
			m_iCurFrame++;
			if (m_iCurFrame >= m_iTotalFrame)
			{
				m_iCurFrame = 0;
				if (!m_bLoop) m_iCurFrame = m_iTotalFrame; // 这个判断是特殊的,是为了判断SetCurFrame(m_iTotalFrame, false)
			}
		}
		else
		{
			m_iCurFrame--;
			if (m_iCurFrame < 0)
			{
				m_iCurFrame = m_iTotalFrame;
				if (!m_bLoop) m_iCurFrame = 0;// 这个判断是特殊的,是为了判断SetCurFrame(0, true)
			}
		}
	}
	else //反弹的情况下
	{
		if (m_iCurFrame >= m_iTotalFrame)
			m_bAsc = false;
		else if (m_iCurFrame <= 0)
			m_bAsc = true;

		if (m_bAsc)
			m_iCurFrame++;	
		else
			m_iCurFrame--;
	}

	
	if (FinishLoop())
	{
		if (!m_bLoop) Stop();
	}

}

bool CUITimerBase::FinishLoop()
{
	bool bFinished = false;
	if (!m_bRebound)
	{
		if (m_iCurFrame == m_iTotalFrame && !m_bReverse)
			bFinished = true;
		else if (m_iCurFrame == 0 && m_bReverse)
			bFinished = true;
	}
	else
	{
		if (m_iCurFrame == m_iTotalFrame && m_bReverse)
			bFinished = true;
		else if (m_iCurFrame == 0 && !m_bReverse)//反弹情况下，3,2,1,0,3,2,1,0
			bFinished = true;
	}
	return bFinished;
}

void CUITimerBase::SetUserData(LPVOID lpUserData)
{
	m_lpUserData = lpUserData;
}

LPVOID  CUITimerBase::GetUserData()
{
	return m_lpUserData;
};

void CUITimerBase::SetTimerId(UINT uId)
{
	m_uId = uId;

}

UINT CUITimerBase::GetTimerId()
{
	return m_uId;

}

bool  CUITimerBase::IsLoop()
{
	return m_bLoop;
}

bool CUITimerBase::IsRebound()
{
	return m_bRebound;
}

void CUITimerBase::SetRebound(bool bRebound)
{
	m_bRebound = bRebound;
}

void CUITimerBase::SetByStep(bool bByStep)
{
	m_bByStep = bByStep;
}

LONG   CUITimerBase::GetInterval()
{
	return m_iInterval;
};
LONG   CUITimerBase::GetTotalTimer()
{
	return m_iTotalTimer;
}

LONG  CUITimerBase::GetTotalFrame()
{
	return m_iTotalFrame;
}

LONG  CUITimerBase::GetCurFrame()
{
	return m_iCurFrame;
}

void CUITimerBase::SetCurFrame(LONG nCurFrame)
{
	if (nCurFrame <= 0) nCurFrame = 0;
	else if (nCurFrame >= m_iTotalFrame)
		nCurFrame = m_iTotalFrame;
	m_iCurFrame = nCurFrame;
}
//////////////////////////////////////////////////////////////////////////////////////////////

CTimerSource::CTimerSource()
{
	m_pTimerBase = NULL;

}
CTimerSource::~CTimerSource()
{
	if (m_pTimerBase) delete m_pTimerBase;
}

void CTimerSource::operator+= (CUITimerBase& d)
{
	if (m_pTimerBase) delete m_pTimerBase;
	m_pTimerBase =  d.Copy();
}


CUITimerBase* CTimerSource::GetTimerBase()
{
	return m_pTimerBase;
}

void CTimerSource::Start()
{
	if (m_pTimerBase) m_pTimerBase->Start();
}

void CTimerSource::Stop()
{
	if (m_pTimerBase) m_pTimerBase->Stop();
}

void CTimerSource::SetTimerParam(UINT iInterval,UINT iTotalTimer, bool bLoop)
{
	if (m_pTimerBase) m_pTimerBase->SetTimerParam(iInterval, iTotalTimer, bLoop);
}
