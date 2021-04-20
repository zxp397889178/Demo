#include "stdafx.h"
#include "WindowAnimation.h"

//////////////////////////////////////////////////////////////////////////
WindowAnimation::WindowAnimation()
{
	m_AnimationState = WindowAnimationState_Show;
	m_hWnd = NULL;
	m_pWindow = NULL;
	m_BlendFun.BlendOp=0;
	m_BlendFun.BlendFlags=0;
	m_BlendFun.AlphaFormat=1;
	m_BlendFun.SourceConstantAlpha=200;
	m_pDibObj = NULL;
	m_szAnimationDC.cx = 0;
	m_szAnimationDC.cy = 0;
	m_bLayerWindow = false;
	SetParam(30, 8);
}

WindowAnimation::~WindowAnimation()
{
	if ( m_hWnd != NULL )
		m_hWnd = NULL;
	if ( m_pWindow != NULL )
		m_pWindow = NULL;
	if  (m_pDibObj)
		delete m_pDibObj;
	m_pDibObj = NULL;

	ActionManager::GetInstance()->removeWindowAnimation(this);
}

void WindowAnimation::onFinished()
{
	__super::onFinished();

	if (m_pWindow)
	{
		switch (m_AnimationState)
		{
		case WindowAnimationState_Show:
			{
				m_pWindow->SetLayerWindow(m_bLayerWindow, false);
				m_pWindow->ShowWindow(SW_SHOW);
				m_pWindow->GetRoot()->Resize();
				m_pWindow->GetRoot()->Invalidate();
			}
			break;
		case WindowAnimationState_Hide:
			{
				m_pWindow->ShowWindow(SW_HIDE);
				m_pWindow->SetLayerWindow(m_bLayerWindow, false);
			}
			break;
		case WindowAnimationState_Close:
			{
				m_pWindow->CloseWindow(IDCANCEL);
			}
			break;
		default:
			{
				m_pWindow->GetRoot()->Resize();
				m_pWindow->GetRoot()->Invalidate();
			}
			break;
		}
	}
}

void WindowAnimation::SetAnimationState( WindowAnimationState state )
{
	m_AnimationState = state;
}

void WindowAnimation::startWithTarget( BaseObject* pActionTarget )
{
	RT_CLASS_ASSERT(pActionTarget, CWindowUI, _T("WindowAnimation 必须绑定窗口"));
	m_pWindow = static_cast<CWindowUI*>(pActionTarget);

	Start(m_pWindow);

	__super::startWithTarget(pActionTarget);
}

//////////////////////////////////////////////////////////////////////////

CTurnObjectAnimation::CTurnObjectAnimation()
{
	SetParam(50, 11);
	SetCurveID(_T("curve.quad.in"));
}

CTurnObjectAnimation::~CTurnObjectAnimation()
{
}

void CTurnObjectAnimation::Start( CWindowUI *pWindow)
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	m_rcAnimationDC = m_rcWindow;
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;

	m_rcAnimationDC.top = m_rcAnimationDC.top - 100;
	m_rcAnimationDC.bottom = m_rcAnimationDC.bottom + 100;
	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();

	__super::Start(pWindow);
}

void CTurnObjectAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);

	int iBaseY = 100;
	int iOffY = 10;
	int iPosY = 0;
	int iTimer = pActionInterval->GetCurFrame();

	if ( fabs(pActionInterval->GetCurFrame() - ceil(pActionInterval->GetTotalFrame()/2.0f)) < 0.000001 )
	{
		m_pWindow->OnShowSecondSide();
	}

	if ( pActionInterval->GetCurFrame() > floor(pActionInterval->GetTotalFrame()/2.0f) )
	{
		iTimer = pActionInterval->GetTotalFrame() - pActionInterval->GetCurFrame();
		iPosY = iBaseY - iOffY*iTimer;
	}
	else
	{
		iPosY = iBaseY+iOffY*iTimer;
	}

	double dAngle = BezierAlgorithm::BezierValue(GetCurveObj(), iTimer, 0, 90, (long)ceil(pActionInterval->GetTotalFrame()/2.0f));
	double dRad = (double)(dAngle * M_PI / 180) ;
	int iPosX = (int)((m_szAnimationDC.cx/2) * sin( dRad ));

	POINT newpoints1[] = {{iPosX, iPosY},{m_szWindow.cx-iPosX, 200-iPosY},{iPosX, m_szWindow.cy+iPosY}};
	::PlgBlt( m_pDibObj->GetSafeHdc(), newpoints1, 	m_pWindow->GetMemDC(), 0, 0, m_szWindow.cx, m_szWindow.cy, NULL, 0, 0 );

	if ( pActionInterval->GetCurFrame() == pActionInterval->GetTotalFrame() )
	{	
		m_BlendFun.SourceConstantAlpha = 255;

		POINT ptSrc = {0,0};
		POINT ptWinPos = {m_rcWindow.left, m_rcWindow.top};
		UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szWindow, m_pWindow->GetMemDC(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );
	}
	else
	{
		POINT ptSrc = {0,0};
		POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
		UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );
	}

	if ( !IsWindowVisible( m_hWnd ) )
	{
		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}
}

//////////////////////////////////////////////////////////////////////////

CInflateAnimation::CInflateAnimation()
{
	SetParam(40, 8);
	SetCurveID(_T("curve.back.out"));
}

CInflateAnimation::~CInflateAnimation()
{

}

void CInflateAnimation::Start( CWindowUI *pWindow )
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;

	m_rcAnimationDC.left = m_rcWindow.left - 20;
	m_rcAnimationDC.top = m_rcWindow.top - 20;
	m_rcAnimationDC.right = m_rcWindow.right + 20;
	m_rcAnimationDC.bottom = m_rcWindow.bottom + 20;
	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	m_BlendFun.SourceConstantAlpha=100;

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();
}

void CInflateAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);
	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	m_BlendFun.SourceConstantAlpha = (int)TweenAlgorithm::Linear( pActionInterval->GetCurFrame(), 40, 215, pActionInterval->GetTotalFrame() );

	int iPos = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 36, -16, pActionInterval->GetTotalFrame());
	int iDis = (20-iPos)*2;

	::AlphaBlend( m_pDibObj->GetSafeHdc(), iPos, iPos, m_szWindow.cx+iDis,m_szWindow.cy+iDis, m_pWindow->GetMemDC(),
		0,0,m_szWindow.cx, m_szWindow.cy, pixelblend );

	if ( pActionInterval->GetCurFrame() == pActionInterval->GetTotalFrame() )
	{
		m_BlendFun.SourceConstantAlpha = 255;
		POINT ptSrc = {0,0};
		POINT ptWinPos = {m_rcWindow.left, m_rcWindow.top};
		UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szWindow, m_pWindow->GetMemDC(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );
	}
	else
	{
		POINT ptSrc = {0,0};
		POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
		UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );
	}

	if ( !IsWindowVisible(m_hWnd) )
	{
		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}
}

//////////////////////////////////////////////////////////////////////////
CScatterAnimation::CScatterAnimation()
{
	SetParam(30, 6);
	SetCurveID(_T("curve.circ.in"));
}

CScatterAnimation::~CScatterAnimation()
{
}

void CScatterAnimation::Start( CWindowUI *pWindow )
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;

	m_rcAnimationDC = m_rcWindow;
	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	m_ptCenter.x = (int)(m_rcAnimationDC.right + m_rcAnimationDC.left) / 2;
	m_ptCenter.y = (int)(m_rcAnimationDC.bottom + m_rcAnimationDC.top) / 2;
	m_szWnd.cx = (int) m_szAnimationDC.cx / 2;
	m_szWnd.cy = (int) m_szAnimationDC.cy / 2;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	m_BlendFun.SourceConstantAlpha=255;

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true,false);
	}

	m_pWindow->OnShowFristSide();

}

void CScatterAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);

	int disX = (int) BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetTotalFrame()-pActionInterval->GetCurFrame(), 0, m_szWnd.cx, pActionInterval->GetTotalFrame());
	int disY = (int) BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetTotalFrame()-pActionInterval->GetCurFrame(), 0, m_szWnd.cy, pActionInterval->GetTotalFrame());
	int cx = m_szWnd.cx - disX;
	int cy = m_szWnd.cy - disY;
	
	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend( m_pDibObj->GetSafeHdc(), 0, 0, cx, cy, m_pWindow->GetMemDC(), disX, disY, cx, cy, pixelblend );
	::AlphaBlend( m_pDibObj->GetSafeHdc(), m_szWnd.cx+disX, 0, cx, cy, m_pWindow->GetMemDC(), m_szWnd.cx, disY, cx, cy, pixelblend );
	::AlphaBlend( m_pDibObj->GetSafeHdc(), 0, m_szWnd.cy+disY, cx, cy, m_pWindow->GetMemDC(), disX, m_szWnd.cy, cx, cy, pixelblend );
	::AlphaBlend( m_pDibObj->GetSafeHdc(), m_szWnd.cx+disX, m_szWnd.cy+disY, cx, cy, m_pWindow->GetMemDC(), m_szWnd.cx, m_szWnd.cy, cx, cy, pixelblend );	

	POINT ptSrc = {0,0};
	POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
	UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );

	if ( !IsWindowVisible( m_hWnd ) )
	{
		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}
}

//////////////////////////////////////////////////////////////////////////

CBlindAnimation::CBlindAnimation(  BlindType iBlindType /*= BlindTop*/, DWORD dwTimer, bool bActive, bool bIn )
{
	m_iBlindType = iBlindType;
	m_bActive = bActive;
	m_bIn = bIn;
	SetParam(dwTimer, 10);
	if (iBlindType == BlindLeft || iBlindType == BlindRight)
		SetCurveID(_T("curve.circ.out"));
	else
		SetCurveID(_T("curve.circ.in"));
}

CBlindAnimation::~CBlindAnimation()
{
}

void CBlindAnimation::Start( CWindowUI *pWindow )
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;

	m_rcAnimationDC = m_rcWindow;
	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	m_ptCenter.x = (int)(m_rcAnimationDC.right + m_rcAnimationDC.left) / 2;
	m_ptCenter.y = (int)(m_rcAnimationDC.bottom + m_rcAnimationDC.top) / 2;
	m_szWnd.cx = (int) m_szAnimationDC.cx / 2;
	m_szWnd.cy = (int) m_szAnimationDC.cy / 2;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	m_BlendFun.SourceConstantAlpha=255;

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();
}

void CBlindAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);

	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	if ( m_iBlindType==BlindTop)
	{
		int Y = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, m_szAnimationDC.cy, pActionInterval->GetTotalFrame());
		int pos = m_szAnimationDC.cy - Y;
		::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, 0, m_szAnimationDC.cx, Y, m_pWindow->GetMemDC(), 0, pos, m_szAnimationDC.cx, Y, pixelblend);
	}
	else if ( m_iBlindType == BlindBottom )
	{
		int Y = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, m_szAnimationDC.cy, pActionInterval->GetTotalFrame());
		int pos = m_szAnimationDC.cy - Y;
		::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, pos, m_szAnimationDC.cx, Y, m_pWindow->GetMemDC(), 0, 0, m_szAnimationDC.cx, Y, pixelblend);
	}
	else if ( m_iBlindType == BlindLeft)
	{
		if ( m_bIn )
		{
			int X = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, m_szAnimationDC.cx, pActionInterval->GetTotalFrame());
			int pos = m_szAnimationDC.cx - X;		
			::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, 0, X, m_szAnimationDC.cy, m_pWindow->GetMemDC(), pos, 0, X, m_szAnimationDC.cy, pixelblend);
		}
		else
		{
			int X = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), m_szAnimationDC.cx, -m_szAnimationDC.cx, pActionInterval->GetTotalFrame());
			int pos = m_szAnimationDC.cx - X;		
			::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, 0, X, m_szAnimationDC.cy, m_pWindow->GetMemDC(), pos, 0, X, m_szAnimationDC.cy, pixelblend);
		}
	}
	else
	{
		if ( m_bIn )
		{
			int X = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, m_szAnimationDC.cx, pActionInterval->GetTotalFrame());
			int pos = m_szAnimationDC.cx - X;
			::AlphaBlend(m_pDibObj->GetSafeHdc(), pos, 0, X, m_szAnimationDC.cy, m_pWindow->GetMemDC(), 0, 0, X, m_szAnimationDC.cy, pixelblend);
		}
		else
		{
			int X = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), m_szAnimationDC.cx, -m_szAnimationDC.cx, pActionInterval->GetTotalFrame());
			int pos = m_szAnimationDC.cx - X;
			::AlphaBlend(m_pDibObj->GetSafeHdc(), pos, 0, X, m_szAnimationDC.cy, m_pWindow->GetMemDC(), 0, 0, X, m_szAnimationDC.cy, pixelblend);
		}

	}

	if (!IsWindowVisible(m_hWnd))
	{
		::ShowWindow(m_hWnd, m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
		if (m_pWindow->IsLayerWindow())
		{
			m_pWindow->ModifyWndStyle(GWL_EXSTYLE, 0, WS_EX_LAYERED);
		}
	}

	POINT ptSrc = {0,0};
	POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
	UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );//
}

void CBlindAnimation::onFinished()
{
	if ( !m_bIn )
	{
		m_pWindow->ShowWindow( SW_HIDE );
	}

	if ( m_bActive )
	{
		m_pWindow->ActiveWindow();
	}
	__super::onFinished();
}

//////////////////////////////////////////////////////////////////////////
CCloseAnimation::CCloseAnimation()
{
	m_AnimationState = WindowAnimation::WindowAnimationState_Close;
	SetParam(30, 8);
	SetCurveID(_T("curve.back.in"));
}

CCloseAnimation::~CCloseAnimation()
{
}

void CCloseAnimation::Start( CWindowUI *pWindow)
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	m_rcAnimationDC = m_rcWindow;
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;

	m_rcAnimationDC.left = m_rcWindow.left - 25;
	m_rcAnimationDC.top = m_rcWindow.top - 25;
	m_rcAnimationDC.right = m_rcWindow.right + 25;
	m_rcAnimationDC.bottom = m_rcWindow.bottom + 25;
	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();

}

void CCloseAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);

	m_BlendFun.SourceConstantAlpha = (int) TweenAlgorithm::Linear( pActionInterval->GetCurFrame(), 255, -200, pActionInterval->GetTotalFrame());

	int iPos = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 25, -25, pActionInterval->GetTotalFrame());
	int iWidth = (25-iPos)*2;

	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend( m_pDibObj->GetSafeHdc(), iPos, iPos, m_szWindow.cx+iWidth, m_szWindow.cy+iWidth, m_pWindow->GetMemDC(),
			0, 0, m_szWindow.cx, m_szWindow.cy, pixelblend );

	POINT ptSrc = {0,0};
	POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
	UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );

 	if ( !IsWindowVisible( m_hWnd ) )
 	{
 		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
 	}
}
//////////////////////////////////////////////////////////////////////////

CFadeCloseAnimation::CFadeCloseAnimation()
{
	m_AnimationState = WindowAnimation::WindowAnimationState_Close;
	SetParam(30, 8);
	SetCurveID(_T("curve.back.in"));
}

CFadeCloseAnimation::~CFadeCloseAnimation()
{
}

void CFadeCloseAnimation::Start(CWindowUI *pWindow)
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect(m_hWnd, &m_rcWindow);
	m_rcAnimationDC = m_rcWindow;
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;

	m_rcAnimationDC.left = m_rcWindow.left /*- 25*/;
	m_rcAnimationDC.top = m_rcWindow.top /*- 25*/;
	m_rcAnimationDC.right = m_rcWindow.right /*+ 25*/;
	m_rcAnimationDC.bottom = m_rcWindow.bottom/* + 25*/;
	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	if (!m_pDibObj)
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	if (!m_bLayerWindow)
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();

}

void CFadeCloseAnimation::step(IActionInterval* pActionInterval)
{
	m_BlendFun.SourceConstantAlpha = (int)TweenAlgorithm::Linear(pActionInterval->GetCurFrame(), 255, -255, pActionInterval->GetTotalFrame());
	POINT ptSrc = { 0, 0 };
	POINT ptWinPos = { m_rcAnimationDC.left, m_rcAnimationDC.top };
	UpdateLayeredWindow(m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, NULL, NULL, 0, &m_BlendFun, ULW_ALPHA);


	if (!IsWindowVisible(m_hWnd))
	{
		m_pWindow->ShowWindow(m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}

}

//////////////////////////////////////////////////////////////////////////
CMenuAnimation::CMenuAnimation( BlindType iType /*= BlindLT*/ )
{
	m_iBlindType=iType;
	SetParam(10, 12);
	SetCurveID(_T("curve.cubic.in"));
}

CMenuAnimation::~CMenuAnimation()
{
}

void CMenuAnimation::Start( CWindowUI *pWindow )
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;

	m_rcAnimationDC = m_rcWindow;
	if ( m_iBlindType == BlindLT )
	{
		m_rcAnimationDC.right = m_rcAnimationDC.right + 15;
		m_rcAnimationDC.bottom = m_rcAnimationDC.bottom + 15;
	}
	else if ( m_iBlindType == BlindRT )
	{
		m_rcAnimationDC.left = m_rcAnimationDC.left - 15;
		m_rcAnimationDC.bottom = m_rcAnimationDC.bottom + 15;
	}
	else if ( m_iBlindType == BlindLB)
	{
		m_rcAnimationDC.right = m_rcAnimationDC.right + 15;
		m_rcAnimationDC.top = m_rcAnimationDC.top - 15;
	}
	else
	{
		m_rcAnimationDC.left = m_rcAnimationDC.left - 15;
		m_rcAnimationDC.top = m_rcAnimationDC.top - 15;
	}

	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	m_BlendFun.SourceConstantAlpha=255;

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();

}

void CMenuAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);

	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	int iWidth = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, m_szAnimationDC.cx, pActionInterval->GetTotalFrame()-2);
	int iHeight = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, m_szAnimationDC.cy, pActionInterval->GetTotalFrame()-2);
	int iPosX = m_szAnimationDC.cx - iWidth;
	int iPosY = m_szAnimationDC.cy - iHeight;

	int iSrcCX = iWidth < m_szWindow.cx ? iWidth : m_szWindow.cx;
	int iSrcCY = iHeight < m_szWindow.cy ? iHeight : m_szWindow.cy;
	int iSrcPosX = m_szWindow.cx - iSrcCX;
	int iSrcPosY = m_szWindow.cy - iSrcCY;

	if ( pActionInterval->GetCurFrame() == pActionInterval->GetTotalFrame()-1)
	{
		::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, 0, m_szAnimationDC.cx, m_szAnimationDC.cy,
			m_pWindow->GetMemDC(), 0, 0, m_szWindow.cx, m_szWindow.cy, pixelblend);
	}
	else if ( pActionInterval->GetCurFrame() == pActionInterval->GetTotalFrame() )
	{
		::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, 0, m_szWindow.cx, m_szWindow.cy,
			m_pWindow->GetMemDC(), 0, 0, m_szWindow.cx, m_szWindow.cy, pixelblend);
	}
	else
	{
		if ( m_iBlindType == BlindLT )
		{
			::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, 0, iWidth, iHeight, m_pWindow->GetMemDC(), iSrcPosX, iSrcPosY, iSrcCX, iSrcCY, pixelblend);
		}
		else if ( m_iBlindType == BlindRT )
		{
			::AlphaBlend(m_pDibObj->GetSafeHdc(), iPosX, 0, iWidth, iHeight, m_pWindow->GetMemDC(), 0, iSrcPosY, iSrcCX, iSrcCY, pixelblend);
		}
		else if ( m_iBlindType == BlindLB)
		{
			::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, iPosY, iWidth, iHeight, m_pWindow->GetMemDC(), iSrcPosX, 0, iSrcCX, iSrcCY, pixelblend);
		}
		else
		{
			::AlphaBlend(m_pDibObj->GetSafeHdc(), iPosX, iPosY, iWidth, iHeight, m_pWindow->GetMemDC(), 0, 0, iSrcCX, iSrcCY, pixelblend);
		}
	}

	if ( pActionInterval->GetCurFrame() == pActionInterval->GetTotalFrame() )
	{
		//扩大
		POINT ptSrc = {0,0};
		POINT ptWinPos = {m_rcWindow.left, m_rcWindow.top};
		UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szWindow, m_pWindow->GetMemDC(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );
	}
	else
	{
		POINT ptSrc = {0,0};
		POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
		UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );
	}

	if ( !IsWindowVisible(m_hWnd) )
	{
		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}
}

//////////////////////////////////////////////////////////////////////////
CTurnButtonIconAnimation::CTurnButtonIconAnimation()
{
	SetParam(50, 7);
}

CTurnButtonIconAnimation::~CTurnButtonIconAnimation()
{
}

void CTurnButtonIconAnimation::Start( CWindowUI *pWindow)
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	m_rcAnimationDC = m_rcWindow;
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;
	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	m_dAcceleration = m_szAnimationDC.cx / 7.0;

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();
}

void CTurnButtonIconAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);

	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	int iPosX;
	if ( pActionInterval->GetCurFrame() > floor(pActionInterval->GetTotalFrame()/2.0) )
	{
		iPosX = (int)m_dAcceleration * (pActionInterval->GetTotalFrame()-pActionInterval->GetCurFrame());
	}
	else
	{
		iPosX = (int)m_dAcceleration * pActionInterval->GetCurFrame();
	}
	int iWidth = m_szAnimationDC.cx - 2*iPosX;

	if ( fabs(pActionInterval->GetCurFrame() - ceil(pActionInterval->GetTotalFrame()/2.0)) < 0.000001 )
	{
		m_pWindow->OnShowSecondSide();
	}

	::AlphaBlend(m_pDibObj->GetSafeHdc(), iPosX, 0, iWidth, m_szAnimationDC.cy, m_pWindow->GetMemDC(), 0, 0, m_szAnimationDC.cx, m_szAnimationDC.cy, pixelblend);

	POINT ptSrc = {0,0};
	POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
	UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );

	if ( !IsWindowVisible( m_hWnd ) )
	{
		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}
}

//////////////////////////////////////////////////////////////////////////

CPopupAnimation::CPopupAnimation()
{
	SetParam(10, 6);
	SetCurveID(_T("curve.liner"));
}

CPopupAnimation::~CPopupAnimation()
{

}

void CPopupAnimation::Start( CWindowUI *pWindow )
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;
	m_rcAnimationDC = m_rcWindow;
	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();
}

void CPopupAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);
	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	m_BlendFun.SourceConstantAlpha = (int)TweenAlgorithm::Linear( pActionInterval->GetCurFrame(), 20, 225, pActionInterval->GetTotalFrame() );

	int iPos = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 30, -30, pActionInterval->GetTotalFrame());
	int iDis = iPos*2;
	::AlphaBlend( m_pDibObj->GetSafeHdc(), iPos, iPos, m_szWindow.cx-iDis,m_szWindow.cy-iDis, m_pWindow->GetMemDC(),
			0,0,m_szWindow.cx, m_szWindow.cy, pixelblend );

	POINT ptSrc = {0,0};
	POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
	UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );
	
	if ( !IsWindowVisible(m_hWnd) )
	{
		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}
}

//////////////////////////////////////////////////////////////////////////

CPosChangeAnimation::CPosChangeAnimation( int iDisX, int iDisY ): m_iDisX(iDisX), m_iDisY(iDisY)
{
	SetParam(20, 8);
	SetCurveID(_T("curve.sine.out"));
}

CPosChangeAnimation::~CPosChangeAnimation()
{

}

void CPosChangeAnimation::Start( CWindowUI *pWindow)
{
	m_pWindow = pWindow;
	m_hWnd = m_pWindow->GetHWND();
	m_bLayerWindow = m_pWindow->IsLayerWindow();

	::GetWindowRect( m_hWnd, &m_rcWindow );
	// 改变动画算法
	m_rcWindow.top = m_rcWindow.top + m_iDisY;
	m_rcWindow.bottom = m_rcWindow.bottom + m_iDisY;
	//
	m_rcAnimationDC = m_rcWindow;
	m_szWindow.cx = m_rcWindow.right - m_rcWindow.left;
	m_szWindow.cy = m_rcWindow.bottom - m_rcWindow.top;

	if ( m_iDisY > 0 )
	{
		m_rcAnimationDC.top = m_rcAnimationDC.top - m_iDisY;	
	}
	else
	{
		m_rcAnimationDC.bottom = m_rcAnimationDC.bottom - m_iDisY;
	}

	m_szAnimationDC.cx = m_rcAnimationDC.right - m_rcAnimationDC.left;
	m_szAnimationDC.cy = m_rcAnimationDC.bottom - m_rcAnimationDC.top;

	if ( !m_pDibObj )
	{
		m_pDibObj = new DibObj;
		m_pDibObj->Create(pWindow->GetPaintDC(), m_szAnimationDC.cx, m_szAnimationDC.cy);
	}

	if ( !m_bLayerWindow )
	{
		m_pWindow->SetLayerWindow(true, false);
	}

	m_pWindow->OnShowFristSide();
	m_BlendFun.SourceConstantAlpha = 255;
}

void CPosChangeAnimation::step(IActionInterval* pActionInterval)
{
	m_pDibObj->Fill(0,0);

	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	int Y = 0;
	if ( m_iDisY > 0 )
	{
		Y = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, m_iDisY, pActionInterval->GetTotalFrame());
	}
	else
	{
		Y = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), -m_iDisY, m_iDisY, pActionInterval->GetTotalFrame());
	}

	::AlphaBlend(m_pDibObj->GetSafeHdc(), 0, Y, m_szWindow.cx, m_szWindow.cy, m_pWindow->GetMemDC(), 0, 0, m_szWindow.cx, m_szWindow.cy, pixelblend );

	if ( pActionInterval->GetCurFrame() == pActionInterval->GetTotalFrame() )
	{	
		m_BlendFun.SourceConstantAlpha = 255;

		POINT ptSrc = {0,0};
		POINT ptWinPos = {m_rcWindow.left, m_rcWindow.top};
		UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szWindow, m_pWindow->GetMemDC(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );

	}
	else
	{
		POINT ptSrc = {0,0};
		POINT ptWinPos = {m_rcAnimationDC.left, m_rcAnimationDC.top};
		UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );
	}

	if ( !IsWindowVisible( m_hWnd ) )
	{
		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}
}

CZoomAnimation::CZoomAnimation()
{
	SetRectEmpty(&m_rcZoomDest);
	SetParam(40, 10);
	SetCurveID(_T("curve.quad.out"));
}

RECT CZoomAnimation::ZoomDest() const
{
	return m_rcZoomDest;
}

void CZoomAnimation::ZoomDest( const RECT& val )
{
	::CopyRect(&m_rcZoomDest, &val);
}

void CZoomAnimation::step(IActionInterval* pActionInterval)
{
// 	int nMoveLeft = m_rcWindow.left - m_rcZoomDest.left;
// 	int nMoveTop  = m_rcWindow.top - m_rcZoomDest.top;
// 	int nMoveRight= m_rcWindow.right - m_rcZoomDest.right;
// 	int nMoveBottom = m_rcWindow.bottom - m_rcZoomDest.bottom;

	m_pDibObj->Fill(0,0);
	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	m_BlendFun.SourceConstantAlpha = (int)TweenAlgorithm::Linear( pActionInterval->GetCurFrame(), 100, 150, pActionInterval->GetTotalFrame() );

// 	int nOffsetLeft = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, nMoveLeft, pActionInterval->GetTotalFrame());
// 	int nOffsetTop  = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, nMoveTop, pActionInterval->GetTotalFrame());
// 	int nOffsetRight= (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, nMoveRight, pActionInterval->GetTotalFrame());
// 	int nOffsetBottom = (int)BezierAlgorithm::BezierValue(GetCurveObj(), pActionInterval->GetCurFrame(), 0, nMoveBottom, pActionInterval->GetTotalFrame());


	//RECT rcDest = {m_rcWindow.left - nOffsetLeft, m_rcWindow.top - nOffsetTop, m_rcWindow.right - nOffsetRight, m_rcWindow.bottom - nOffsetBottom};

	RECT rcDest = BezierAlgorithm::BezierRect(GetCurveObj(), pActionInterval->GetCurFrame(), m_rcWindow, m_rcZoomDest, pActionInterval->GetTotalFrame());

	::SetWindowPos(m_hWnd, HWND_DESKTOP, rcDest.left, rcDest.top, rcDest.right - rcDest.top, rcDest.bottom - rcDest.top, SWP_NOZORDER);
	::AlphaBlend( m_pDibObj->GetSafeHdc(), 0, 0, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top,
		m_pWindow->GetMemDC(), 0, 0,m_szWindow.cx, m_szWindow.cy, 
		pixelblend );

	POINT ptSrc = {0,0};
	POINT ptWinPos = {rcDest.left, rcDest.top};
	UpdateLayeredWindow( m_hWnd, m_pWindow->GetPaintDC(), &ptWinPos, &m_szAnimationDC, m_pDibObj->GetSafeHdc(), &ptSrc, 0, &m_BlendFun, ULW_ALPHA );


	if ( !IsWindowVisible( m_hWnd ) )
	{
		m_pWindow->ShowWindow( m_pWindow->GetWndStyle(GWL_EXSTYLE) & WS_EX_NOACTIVATE ? SW_SHOWNOACTIVATE : SW_SHOW);
	}
}
