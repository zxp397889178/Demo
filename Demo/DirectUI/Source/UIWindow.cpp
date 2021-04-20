/***************************************************************************
  Copyright    : 2005, NetDragon Websoft Inc
  Program ID   : NDDialog.cpp: implementation of the CWindowUI class.
  Description  : 对话框
  Version      : 91USkin 2.0
  Modification Log:
       DATE         AUTHOR          DESCRIPTION
 --------------------------------------------------------------------------
     2005-01-29     ZhouJS			create 
	 2010-10-25     xqb				Add CSkinWindow
***************************************************************************/

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWindow.h"
#include "UICaret.h"
#include "Gesture.h"
//////////////////////////////////////////////////////////////////////
#define  _ROUND_RGN_R   5  //窗口圆角半径大小
#define DRAG_DX 5
#define AREA_SELECTION_DX 5
typedef struct tagFINDTABINFO
{
	CControlUI* pFocus;
	CControlUI* pLast;
	bool bForward;
	bool bNextIsIt;
} FINDTABINFO;

CControlUI* __stdcall __FindControlFromPoint(CControlUI* pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	if (pThis->IsNeedScroll() && pThis->GetParent() != NULL)
	{
		CControlUI* pParent = pThis->GetParent();
		if (!pParent->HitClientTest(*pPoint))
			return NULL;
	}
	return  pThis->HitTest(*pPoint) ? pThis : NULL;
}

CControlUI* __stdcall __FindControlFromTab(CControlUI* pThis, LPVOID pData)
{
	FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
	if( pInfo->pFocus == pThis ) {
		if( pInfo->bForward ) pInfo->bNextIsIt = true;
		return pInfo->bForward ? NULL : pInfo->pLast;
	}
	if( (pThis->GetFlags() & UICONTROLFLAG_TABSTOP) == 0 ) return NULL;
	pInfo->pLast = pThis;
	if( pInfo->bNextIsIt ) return pThis;
	if( pInfo->pFocus == NULL ) return pThis;
	return NULL;  // Examine all controls
}

CControlUI* __stdcall __FindControlEnableDrop(CControlUI* pThis, LPVOID pData)
{
	if( (pThis->GetFlags() & UICONTROLFLAG_ENABLE_DROP) == 0 ) return NULL;
	return pThis;
}

CControlUI* __stdcall __FindControlEnableDrag(CControlUI* pThis, LPVOID pData)
{
	if( (pThis->GetFlags() & UICONTROLFLAG_ENABLE_DRAG) == 0 ) return NULL;
	return pThis;
}

CControlUI* __stdcall __FindControlEnableAreaSelection(CControlUI* pThis, LPVOID pData)
{
	if((pThis->GetFlags() & UICONTROLFLAG_ENABLE_AREAS)) 
	{
		LPPOINT pPoint = static_cast<LPPOINT>(pData);
		if (pThis->HitClientTest(*pPoint))
			return pThis;
	}
	return NULL;
}

CControlUI* __stdcall __FindControlById(CControlUI* pThis, LPVOID pData)
{
	if (equal_icmp(pThis->GetId(), LPCTSTR(pData)))
		return pThis;
	return NULL;
}

extern UITYPE_FONT  GetFontTypeValueFromStr(LPCTSTR lpszValue);
LRESULT OnHitTest(HWND hWnd, WPARAM   wParam,   LPARAM   lParam);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CChangeSkinAnimation
{
public:
	 CChangeSkinAnimation(CWindowUI* pWindow)
	{
		m_pWindow = pWindow;
		m_objTimerSource += MakeDelegate(this, &CChangeSkinAnimation::OnTimer);
	}
	void Start()
	{
		m_objTimerSource.SetTimerParam(40, 400);
		m_objTimerSource.Start();

	}
protected:
	void OnTimer(CUITimerBase* pTimer)
	{
		CControlUI* pControlUI = m_pWindow->GetItem(_T("background"));
		if (!pControlUI)
		{			
			delete this;
			return;
		}
		
		int iAlpha = 0;
		int nCurFrame = pTimer->GetCurFrame();
		if( nCurFrame <= 5 )
		{
			iAlpha = (int)TweenAlgorithm::Quad( nCurFrame, 255, -255, 5 );
		}
		else if ( nCurFrame == 10 )
		{
			iAlpha = 255;
		}
		else
		{
			iAlpha = (int)TweenAlgorithm::Quad( nCurFrame-5, 10, 245, 5);
			
		}
		if ( nCurFrame == 6 )
		{
			m_pWindow->SetBkImage(GetEngineObj()->GetSkinImage());
			m_pWindow->SetBkColor(GetEngineObj()->GetSkinColor());
		}
		pControlUI->SetAlpha( iAlpha );
		m_pWindow->Invalidate( );
		
		if (pTimer->IsDone())
			delete this;
	}

protected:
	CTimerSource m_objTimerSource;
	CWindowUI* m_pWindow;

};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CControlUI* ms_pLastClickEvent = NULL;
CWindowUI::CWindowUI()
{	
	SetWindowClassName(_T("NDGuiFoundation"));
	m_hIcon = NULL;
	m_hBigIcon = NULL;
	m_nAlpha = 255;
	m_bLayerWindow = false;
	m_bEnableMove = true;
	m_bModalDlg = false;
	m_pDibObj = new DibObj;
	m_bEnableResize = true;
	m_bMouseCapture =false;
	m_uTimerID = DUI_TIMERID_BASE;
	m_pRoot = new CContainerUI();
	m_pRoot->SetManager(this, NULL);
	m_pRoot->SetCoordinate(0,0,0,0,UIANCHOR_LTRB);
	m_hWnd = NULL;
	m_hDcPaint = NULL;
	m_pFocus = NULL;
	m_pEventHover = NULL;
    m_pEventMouseInOut = NULL;
	m_pEventClick = NULL;
	m_pEventKey = NULL;
	m_pEventDrag = NULL;
	m_pControlDrag = NULL;
	m_pGuesture = NULL;
	m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
	m_ptLastMousePosClick.x = m_ptLastMousePosClick.y = -1;
	m_bMouseTracking = false;
	m_bFocusNeeded = false;
	m_bUpdateNeeded = false;
	m_bSizeChanged = true;
	m_bUnFirstPaint = true;
	m_bMouseWheel = false;
	m_iMinWidth = m_iMinHeight = m_iMaxWidth = m_iMaxHeight =  m_iOrgWidth = m_iOrgHeight = 0;
	m_bMaximized = false;
	m_rcBorder.left = m_rcBorder.top = m_rcBorder.right = m_rcBorder.bottom=4;//GetSystemMetrics(SM_CXFRAME);
	m_strSubId = _T("0");
	m_pDragTarget = NULL;

	m_hImc = NULL;
	m_bDraging = false;
	m_bEnableDragDrop = false;
	m_lpszTitleKey = NULL;
    m_bMinimized = false;
	ZeroMemory(&m_rectWork, sizeof(RECT));
	ZeroMemory(&m_rectUpdate, sizeof(RECT));
	m_pShadowWindow = NULL;
	m_bShadow = false;

	m_bAreaSelecting = false;
	m_uModalResult = 0;
	m_pTipWindow = NULL;
	m_hTipWindow = NULL;
	m_bLockUpdate = false;
    m_bVKTabEnable = false;
	m_eFontType = GetUIRes()->GetDefaultFontType();
	m_nTextRenderingHint = GetUIRes()->GetDefaultTextRenderingHint();
}

void CWindowUI::MaximizeWindow()
{	
	::PostMessage(this->GetHWND(),WM_SYSCOMMAND, SC_MAXIMIZE, 0);
}

void CWindowUI::MinimizeWindow()
{
	::PostMessage(this->GetHWND(),WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CWindowUI::RestoreWindow()
{
	::PostMessage(this->GetHWND(),WM_SYSCOMMAND, SC_RESTORE, 0);
}

void CWindowUI::ActiveWindow()
{
	if (::IsIconic(GetHWND()))
		ShowWindow(SW_RESTORE);
	SetForegroundWindow(GetHWND());
	SetActiveWindow(GetHWND());
}

void CWindowUI::ShowAndActiveWindow()
{
	ShowWindow(SW_SHOW);
	ActiveWindow();
}

CWindowUI::~CWindowUI()
{	
	CCaretUI::GetInstance()->DestroyCaret(this);
	for( int it = 0; it < m_aTimers.GetSize(); it++) {
		TIMERINFO* pTimer =  (TIMERINFO*)m_aTimers.GetAt(it);
		::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
		delete pTimer;
	}
	m_aTimers.Empty();
	SetEnableDragDrop(false);

	if( GetHWND() != NULL && m_hDcPaint != NULL ) 
		::ReleaseDC(GetHWND(), m_hDcPaint);
	m_hDcPaint = NULL;

	if (m_hTipWindow && ::IsWindow(m_hTipWindow))
		::PostMessage(m_hTipWindow, WM_CLOSE, NULL, NULL);
	m_hTipWindow = NULL;
	m_pTipWindow;

	if (m_pDibObj)
		delete m_pDibObj;
	m_pDibObj = NULL;

	GetEngineObj()->UnSkinWindow(this);

 	if (m_pRoot != NULL)
 		delete  m_pRoot;
	m_pRoot = NULL;

	if (m_hIcon)
		::DestroyIcon(m_hIcon);
	m_hIcon = NULL;

	if (m_hBigIcon)
		::DestroyIcon(m_hBigIcon);
	m_hBigIcon = NULL;

	if (m_lpszTitleKey)
		delete [] m_lpszTitleKey;
	m_lpszTitleKey = NULL;

	ActionManager::GetInstance()->removeWindowAnimationByTarget(this);
}


CControlUI * CWindowUI::GetItem(LPCTSTR lpszName)
{
	if (m_pRoot)
		return m_pRoot->GetItem(lpszName);
	return NULL;
}

void CWindowUI::SetAlpha(int nAlpha)
{	
	m_nAlpha = nAlpha;
	if (!IsLayerWindow())
	{
		::SetLayeredWindowAttributes(GetHWND(), RGB(0,0,0), nAlpha, LWA_ALPHA);
	}
	else
	{
		Invalidate();
	}
}

int  CWindowUI::GetAlpha()
{
	return m_nAlpha;
}

void CWindowUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue) //重载基类
{
	if (!lpszName || !lpszValue)
		return;

	if (equal_icmp(lpszName, _T("id")))
	{
		SetId(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("resize")))
	{
		EnableResize(!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("move")))
	{
		EnableMove(!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("minWidth")))
	{
		m_iMinWidth = _ttoi(lpszValue);
		m_iMinWidth = DPI_SCALE(m_iMinWidth);
	}
	else if (equal_icmp(lpszName, _T("minHeight")))
	{
		m_iMinHeight = _ttoi(lpszValue);
		m_iMinHeight = DPI_SCALE(m_iMinHeight);
	}
	else if (equal_icmp(lpszName, _T("maxWidth")))
	{
		m_iMaxWidth = _ttoi(lpszValue);
		m_iMaxWidth = DPI_SCALE(m_iMaxWidth);
	}
	else if (equal_icmp(lpszName, _T("maxHeight")))
	{
		m_iMaxHeight = _ttoi(lpszValue);
		m_iMaxHeight = DPI_SCALE(m_iMaxHeight);
	}
	else if (equal_icmp(lpszName, _T("Width")))
	{
		m_iOrgWidth = _ttoi(lpszValue);
		m_iOrgWidth = DPI_SCALE(m_iOrgWidth);
	}
	else if (equal_icmp(lpszName, _T("Height")))
	{
		m_iOrgHeight = _ttoi(lpszValue);
		m_iOrgHeight = DPI_SCALE(m_iOrgHeight);
	}
	else if (equal_icmp(lpszName, _T("title")))
	{
		bool bFind = false;
		m_strTitle = I18NSTR(lpszValue, &bFind);
		SetTitle(m_strTitle.c_str());
		if (lpszValue[0] == _T('#')/* && bFind*/)
			copy_str(m_lpszTitleKey, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("icon")))
	{
		m_strSmallIcon = lpszValue;
	}
	else if (equal_icmp(lpszName, _T("bigicon")))
	{
		m_strBigIcon = lpszValue;
	}
	else if (equal_icmp(lpszName, _T("shadow")))
	{
		SetShadow(!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("ok")))
	{
		m_strOkControl = lpszValue;
	}
	else if (equal_icmp(lpszName, _T("layer")))
	{
		SetLayerWindow(!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("fonttype")))
	{
		m_eFontType = GetFontTypeValueFromStr(lpszValue);
	}
	
}


void CWindowUI::SetUserAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (lpszName && lpszValue)
		m_mapUserAttribute[lpszName] = lpszValue;
}

LPCTSTR CWindowUI::GetUserAttribute(LPCTSTR lpszName)
{
	if (!lpszName)
		return _T("");
	map<tstring, tstring>::iterator it = m_mapUserAttribute.find(lpszName);
	if (it != m_mapUserAttribute.end())
		return it->second.c_str();
	return _T("");
}

LPCTSTR CWindowUI::GetId()
{
	return m_strId.c_str();
}

LPCTSTR CWindowUI::GetSubId()
{
	return m_strSubId.c_str();
}
void CWindowUI::SetSubId(LPCTSTR lpszSubId)
{
	m_strSubId = lpszSubId;
}

///////////////////////////////////////////////////////////////////

CContainerUI* CWindowUI::GetRoot()
{
	return m_pRoot;
}


void CWindowUI::SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if (IsWindow(m_hWnd))
	{
		RECT rect = {X, Y, X + cx, Y + cy};
		if (m_bMaximized && !::IsRectEmpty(&rect))
		{
			RECT rcWindow;
			::GetWindowRect(this->GetHWND(), &rcWindow);
			RECT rcTemp = {0};
			//要设置的窗口大小不得小于或等于目前窗口
			if (EqualRect(&rect, &rcWindow))
				return;
			if (!!IntersectRect(&rcTemp, &rect, &rcWindow))
			{
				if (!!EqualRect(&rcTemp, &rect))
				{
					return;
				}
			}
		}
		::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
	}
}

bool CWindowUI::PreMessageHandler(const LPMSG pMsg, LRESULT& lRes)//消息
{
	UINT uMsg = pMsg->message;
	WPARAM wParam = pMsg->wParam;
	LPARAM lParam = pMsg->lParam;
	switch( uMsg ) 
	{
	//case WM_KEYDOWN:
	//	{
	//		switch (wParam)
	//		{
	//		case VK_TAB:
	//			{
	//				SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
	//				return true;
	//			}
	//			break;
	//		}
	//		break;
	//	}
	case WM_SYSKEYDOWN:
		{
			if( m_pFocus != NULL ) {
				TEventUI event = { 0 };
				event.nType = UIEVENT_SYSKEY;
				event.ptMouse = m_ptLastMousePos;
				event.wParam = wParam;
				event.lParam = lParam;
				m_pFocus->Event(event);
			}
		}
		break;
	}
	return false;

}

CControlUI* CWindowUI::FindControl(POINT& pt)
{
	if (m_pRoot == NULL)
		return NULL;
	return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_ENABLED | UIFIND_MOUSE_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

CControlUI* CWindowUI::FindControl(POINT& pt, UINT uFlags)
{
	if (m_pRoot == NULL)
		return NULL;
	return m_pRoot->FindControl(__FindControlFromPoint, &pt, uFlags);

}

CControlUI* CWindowUI::FindControl(LPCTSTR lpszId, CControlUI* pParent)
{
	if (m_pRoot == NULL)
		return NULL;

	if (pParent == NULL)
		pParent = m_pRoot;
	return pParent->FindControl(__FindControlById, (LPVOID)lpszId, UIFIND_ALL);
}


void CWindowUI::ReapObjects(CControlUI* pControl)//控件pControl删除时，进行处理否则会崩溃
{
	int it = 0;
	while (it < m_aTimers.GetSize())
	{
		TIMERINFO* pTimer =  (TIMERINFO*)m_aTimers.GetAt(it);
		if( pTimer->pSender == pControl
			&& pTimer->hWnd == m_hWnd)
		{
			::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
			delete pTimer;
			m_aTimers.Remove(it);
		}
		else
		{
			it++;
		}
	}

	if (m_pFocus == pControl)
		m_pFocus = NULL;

	if (m_pEventHover == pControl)
		m_pEventHover = NULL;

    if (m_pEventMouseInOut == pControl)
        m_pEventMouseInOut = NULL;

	if (m_pEventClick == pControl)
		m_pEventClick = NULL;

	if (m_pEventKey == pControl)
		m_pEventKey = NULL;

	if (m_pEventDrag == pControl)
		m_pEventDrag = NULL;

	if (ms_pLastClickEvent == pControl)
		ms_pLastClickEvent = NULL;

	if (m_pControlDrag == pControl)
		m_pControlDrag = NULL;

	if (m_pGuesture == pControl)
		m_pGuesture = NULL;
}

LRESULT CWindowUI::HitTest(WPARAM   wParam,   LPARAM   lParam)
{
	UINT uHit = HTCLIENT;
	POINT point;
	point.x = (int)(short)LOWORD(lParam); 
	point.y = (int)(short)HIWORD(lParam);
	ScreenToClient(m_hWnd, &point);

	RECT rectWnd;
	GetClientRect(GetHWND(), &rectWnd); 	
	RECT rect;		

	if (m_bMaximized) return uHit;
	//客户区
	if( ::SetRect(&rect, rectWnd.left+m_rcBorder.left, rectWnd.top + m_rcBorder.top,
		rectWnd.right-m_rcBorder.right, rectWnd.bottom -  m_rcBorder.bottom)
		, ::PtInRect(&rect, point))
	{
		return uHit;
	}
	//处于右下角			
	if( ::SetRect(&rect, rectWnd.right-m_rcBorder.right - _ROUND_RGN_R, rectWnd.bottom-m_rcBorder.bottom - _ROUND_RGN_R,
		rectWnd.right, rectWnd.bottom)
		, ::PtInRect(&rect, point))
	{
		uHit = HTBOTTOMRIGHT;
	}
	//处于左上角
	else if( ::SetRect(&rect, rectWnd.left, rectWnd.top,
		rectWnd.left+m_rcBorder.left+ _ROUND_RGN_R, rectWnd.top+m_rcBorder.top+ _ROUND_RGN_R)
		, ::PtInRect(&rect, point))
	{
		uHit = HTTOPLEFT;
	}

	//处于右上角
	else if( ::SetRect(&rect, rectWnd.right-m_rcBorder.right -  _ROUND_RGN_R, rectWnd.top,
		rectWnd.right, rectWnd.top+m_rcBorder.top+ _ROUND_RGN_R)
		, ::PtInRect(&rect, point))
	{
		uHit = HTTOPRIGHT;
	}
	//处于左下角
	else if( ::SetRect(&rect, rectWnd.left, rectWnd.bottom-m_rcBorder.bottom- _ROUND_RGN_R,
		rectWnd.left+m_rcBorder.left+ _ROUND_RGN_R, rectWnd.bottom)
		, ::PtInRect(&rect, point))
	{
		uHit = HTBOTTOMLEFT;
	}

	//处于右边框
	else if( ::SetRect(&rect, rectWnd.right-m_rcBorder.right, rectWnd.top+m_rcBorder.top,
		rectWnd.right, rectWnd.bottom-m_rcBorder.bottom)
		, ::PtInRect(&rect, point))
	{
		uHit = HTRIGHT;
	}
	//处于下边框
	else if( ::SetRect(&rect, rectWnd.left+m_rcBorder.left, rectWnd.bottom-m_rcBorder.bottom,
		rectWnd.right-m_rcBorder.right, rectWnd.bottom)
		, ::PtInRect(&rect, point))
	{
		uHit = HTBOTTOM;
	}

	//处于上边框
	else if( ::SetRect(&rect, rectWnd.left+m_rcBorder.left, rectWnd.top,
		rectWnd.right-m_rcBorder.right, rectWnd.top+m_rcBorder.top)
		, ::PtInRect(&rect, point))
	{
		uHit = HTTOP;
	}
	//处于左边框
	else if( ::SetRect(&rect, rectWnd.left, rectWnd.top+m_rcBorder.top,
		rectWnd.left+m_rcBorder.left, rectWnd.bottom-m_rcBorder.bottom)
		, ::PtInRect(&rect, point))
	{
		uHit = HTLEFT;
	}

	return uHit;
}


LRESULT OnNcLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UINT nHitTest = wParam;
	if (nHitTest == HTCAPTION) 
	{
		return 0;
	}
	if(nHitTest == HTTOP)
		::DefWindowProc(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP ,lParam);
		//::SendMessage(hWnd,WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, lParam);
	else if(nHitTest == HTBOTTOM)
		::SendMessage(hWnd,WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, lParam);
	else if(nHitTest == HTLEFT)
		::DefWindowProc(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT ,lParam);
		//::SendMessage(hWnd,WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, lParam);
	else if(nHitTest == HTRIGHT)
		::SendMessage(hWnd,WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, lParam);
	else if(nHitTest == HTTOPLEFT)
		::SendMessage(hWnd,WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, lParam);
	else if(nHitTest == HTTOPRIGHT)
		::SendMessage(hWnd,WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, lParam);
	else if(nHitTest == HTBOTTOMLEFT)
		::SendMessage(hWnd,WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, lParam);
	else if(nHitTest == HTBOTTOMRIGHT)
		::SendMessage(hWnd,WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, lParam);
	else if (nHitTest == HTSYSMENU)
		::SendMessage(hWnd,WM_SYSCOMMAND, SC_MOUSEMENU,lParam);
	return 1;

}


void CWindowUI::AddNotifier(INotifyUI* pNotifyUI)
{
	if (pNotifyUI)
	{
		RefCountedThreadSafe<INotifyUI>* pNotifySafe = pNotifyUI->GetINotifySafe();
		if (pNotifySafe)
		{
			pNotifySafe->AddRef();
			m_aNotifiers.Add(pNotifySafe);
		}
	}
}
void CWindowUI::RemoveNotifier(INotifyUI* pNotifyUI)
{
	int nCnt = m_aNotifiers.GetSize();
	for( int i = 0; i < nCnt; i++ ) 
	{
		RefCountedThreadSafe<INotifyUI>* pNotifySafe = static_cast<RefCountedThreadSafe<INotifyUI>*>(m_aNotifiers.GetAt(i));
		if( pNotifySafe && pNotifySafe->IsValid() && pNotifySafe->GetCountedOwner() == pNotifyUI ) {
			m_aNotifiers.Remove(i);
			pNotifySafe->Release();
			break;
		}
	}

}
void CWindowUI::FlushToBackBuffer(RECT rcPaint, RECT rcClient)
{
	
	if (!m_pDibObj->IsValid() || rcClient.right - rcClient.left != m_pDibObj->GetWidth() || rcClient.bottom - rcClient.top !=  m_pDibObj->GetHeight())
		m_pDibObj->Create(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	IRenderDC renderDC;
	renderDC.SetDevice(m_pDibObj);

	Draw(&renderDC, &rcPaint);
		
}

void CWindowUI::Draw(IRenderDC* pRenderDC, LPRECT lpClipRect)
{
	DibObj* pDibObj = pRenderDC->GetDibObj();
	if (!pDibObj) return;
	RECT rcCanvas = {0, 0, pDibObj->GetWidth(), pDibObj->GetHeight()};
	RECT rcClip = rcCanvas;
	if (lpClipRect)
		rcClip = *lpClipRect;

	RECT rectRoot = m_pRoot->GetRect();
	if (m_bSizeChanged || !EqualRect(&rectRoot, &rcCanvas))
	{
		m_bSizeChanged = false;
		m_pRoot->OnSize(rcCanvas);
	}

	pRenderDC->SetUpdateRect(rcClip);

	if (!::IntersectRect(&rcClip, &rcCanvas, &rcClip)) return;

	if (IsLayerWindow() && pRenderDC->GetDibObj())
		pRenderDC->GetDibObj()->Fill(rcClip, RGB(0,0,0), 0);
	pRenderDC->SetEnableAlpha(IsLayerWindow());

	if( m_bFocusNeeded ) {
		SetNextTabControl();
	}


	HDC hMemDC = pRenderDC->GetDC();
	int iSaveDC = ::SaveDC(hMemDC);
	//留给用户画背景

	
	UITYPE_FONT eFontType = GetUIRes()->GetDefaultFontType();
	GetUIRes()->SetDefaultFontType(GetDefaultFontType());
	int nTextRenderingHint = GetUIRes()->GetDefaultTextRenderingHint();
	GetUIRes()->SetDefaultTextRenderingHint(GetDefaultTextRenderingHint());

	OnDrawBefore(pRenderDC, rcCanvas, rcClip);
	//留给皮肤画
	m_pRoot->Render(pRenderDC, rcClip); 

	//留给用户画前景
	OnDrawAfter(pRenderDC, rcCanvas, rcClip);
	GetUIRes()->SetDefaultFontType(eFontType);
	GetUIRes()->SetDefaultTextRenderingHint(nTextRenderingHint);

	::RestoreDC(hMemDC, iSaveDC);

}

void CWindowUI::UpdateLayeredWindow(LPRECT lpRect)
{
	if (!IsWindow(GetHWND())) return;
	POINT ptSrc = {0, 0};
	POINT ptDst = {lpRect->left, lpRect->top};
	SIZE  sz = { lpRect->right - lpRect->left, lpRect->bottom - lpRect->top};
	BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	if ((sz.cx != m_pDibObj->GetWidth()) ||  (sz.cy != m_pDibObj->GetHeight()))
	{
		m_pDibObj->Create(GetPaintDC(), sz.cx, sz.cy);
		IRenderDC dc;
		dc.SetDevice(m_pDibObj);
		Draw(&dc);

	}

	::UpdateLayeredWindow(GetHWND(), GetPaintDC(),
		&ptDst,
		&sz, 
		GetMemDC(), 
		&ptSrc, 
		0,
		&pixelblend, 2);
}


typedef BOOL (WINAPI *MYFUNC)(HWND, UPDATELAYEREDWINDOWINFO CONST *);          
MYFUNC pUpdateLayeredWindowIndirect = NULL;
void CWindowUI::FlushToForeBuffer(RECT rcPaint, HDC hDC)
{
	HDC hMemDC = m_pDibObj->GetSafeHdc();
	if (!IsLayerWindow())
	{
		::BitBlt(hDC, 
			rcPaint.left, 
			rcPaint.top, 
			rcPaint.right - rcPaint.left,
			rcPaint.bottom - rcPaint.top,
			hMemDC,
			rcPaint.left,
			rcPaint.top,
			SRCCOPY);
		
	}
	else
	{
	
		RECT rcWin;
		GetWindowRect(GetHWND(), &rcWin);
		POINT ptSrc = {0, 0};
		POINT ptDst = {rcWin.left, rcWin.top};
		SIZE  sz = { rcWin.right - rcWin.left, rcWin.bottom - rcWin.top};
		BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, m_nAlpha, AC_SRC_ALPHA};

		DWORD dwOSVer = 0, dwOSMinorVer = 0;
		GetEngineObj()->GetOSVersion(dwOSVer, dwOSMinorVer);
		if (dwOSVer < 6) //vista以下的版本
		{
			::UpdateLayeredWindow(m_hWnd, hDC,
				&ptDst,
				&sz, 
				m_pDibObj->GetSafeHdc(), 
				&ptSrc, 
				0,
				&pixelblend, 2);
		}
		else
		{
			if (pUpdateLayeredWindowIndirect == NULL)
			{
				HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));        
				if (hFuncInst)
					pUpdateLayeredWindowIndirect = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindowIndirect");
			}
			if (pUpdateLayeredWindowIndirect)
			{
				UPDATELAYEREDWINDOWINFO info = {};
				info.cbSize = sizeof(UPDATELAYEREDWINDOWINFO);
				info.pptSrc = &ptSrc;
				info.pptDst = &ptDst;
				info.psize = &sz;
				info.pblend = &pixelblend;
				info.dwFlags = ULW_ALPHA;
				info.hdcSrc = m_pDibObj->GetSafeHdc();
				info.hdcDst = hDC;
				info.prcDirty = &rcPaint;
				pUpdateLayeredWindowIndirect(m_hWnd, &info);
			}
		}
	}

}

void CWindowUI::HideToolTips()
{
	TipWindow* pTipWnd = GetToolTipWnd();
	if (pTipWnd)
	{
		pTipWnd->ShowTip(false);
	}
}

void CWindowUI::AddMessageFilter(IMessageFilterUI* pFilter)
{
	for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
		if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
			return;
		}
	}
	m_aMessageFilters.Add(pFilter);
}

void CWindowUI::RemoveMessageFilter(IMessageFilterUI* pFilter)
{
	for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
		if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
			m_aMessageFilters.Remove(i);
		}
	}
}

void CWindowUI::OnDrawBefore(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint)
{

}

void CWindowUI::OnDrawAfter(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint)
{

}

bool CWindowUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
	for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
	{
		bool bHandled = false;
		IMessageFilterUI* pIMessageFilterUI = static_cast<IMessageFilterUI*>(m_aMessageFilters[i]);
		LRESULT lResult = pIMessageFilterUI->MessageFilter(uMsg, wParam, lParam, bHandled);
		if( bHandled ) {
			lRes = lResult;
			return true;
		}
	}

	m_bMsgHandled = false;
	switch( uMsg ) {
		case WM_MOUSEMOVE:
		 {
			 //解决BUG#52909 和BUG#47126
			 POINT pt = { (int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };
			 if ( m_ptLastMousePos.x == pt.x &&  m_ptLastMousePos.y == pt.y && !m_bMouseWheel)
				 break;
			 m_bMouseWheel = false;

			 if( !m_bMouseTracking ) {
				 TipWindow* pTipWnd = GetToolTipWnd();
				 TRACKMOUSEEVENT tme = { 0 };
				 tme.cbSize = sizeof(TRACKMOUSEEVENT);
				 tme.dwFlags = TME_HOVER | TME_LEAVE;
				 tme.hwndTrack = m_hWnd;
				 tme.dwHoverTime = pTipWnd == NULL ? 400UL : (DWORD)pTipWnd->Send(TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
				 _TrackMouseEvent(&tme);
				 m_bMouseTracking = true;
			 }
			 // 添加拖拉代码
			  if (wParam==MK_LBUTTON)
			  {
				  if (!m_bDraging && (abs(m_ptLastMousePosClick.x - pt.x) > DRAG_DX || abs(m_ptLastMousePosClick.y - pt.y) > DRAG_DX))
				  {
					  CControlUI * pControl = m_pRoot->FindControl(__FindControlEnableDrag, &m_ptLastMousePosClick,  UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_HITTEST | UIFIND_TOP_FIRST | UIFIND_MOUSE);
					  if (pControl)
					  {
						  m_bDraging = true;
						  TEventUI event = { 0 };
						  event.nType = UIEVENT_DRAG_DROP_GROUPS;
						  event.wParam = UIEVENT_BEGIN_DRAG;
						  event.lParam = MAKELPARAM(m_ptLastMousePosClick.x, m_ptLastMousePosClick.y);//(LPARAM)(&m_bDraging);
						  event.ptMouse = pt;
						  pControl->SendEvent(event);
						  m_bDraging = false;
						  m_pControlDrag = pControl;
					  }
				  }
			  }

			  // 添加框选代码
			  if (wParam==MK_LBUTTON)
			  {
				  if (abs(m_ptLastMousePosClick.x - pt.x) > DRAG_DX || abs(m_ptLastMousePosClick.y - pt.y) > DRAG_DX)
				  {
					  CControlUI * pControl = m_pRoot->FindControl(__FindControlEnableAreaSelection, &m_ptLastMousePosClick,  UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_HITTEST | UIFIND_TOP_FIRST);
					  if (pControl)
					  {
						  m_bAreaSelecting = true;
						  TEventUI event = { 0 };
						  event.nType = UIEVENT_AREA_SELECT;
						  event.wParam = MAKEWPARAM(m_ptLastMousePosClick.x, m_ptLastMousePosClick.y);
						  event.lParam = lParam;
						  event.ptMouse = m_ptLastMousePosClick;
						  pControl->SendEvent(event);
					  }
				  }
			  }	
			 m_ptLastMousePos = pt;

             CControlUI* pNewMouseIn = FindControl(pt, UIFIND_ENABLED | UIFIND_MOUSEINOUT | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST | UIFIND_MOUSE);
             if (pNewMouseIn != m_pEventMouseInOut && m_pEventMouseInOut != NULL) {
                 TEventUI event = { 0 };
                 event.nType = UIEVENT_MOUSELEAVERECT;
                 event.wParam = wParam;
                 event.lParam = (LPARAM)pNewMouseIn;
                 m_pEventMouseInOut->SendEvent(event);
                 m_pEventMouseInOut = NULL;
             }
             if (pNewMouseIn != m_pEventMouseInOut && pNewMouseIn != NULL) {
                 m_pEventMouseInOut = pNewMouseIn;
                 TEventUI event = { 0 };
                 event.nType = UIEVENT_MOUSEENTERRECT;
                 event.wParam = wParam;
                 event.lParam = lParam;
                 pNewMouseIn->SendEvent(event);
             }

			 // 改回原来，碰到控件重叠会出问题
			 CControlUI* pNewHover = FindControl(pt, UIFIND_ENABLED | UIFIND_MOUSE_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST | UIFIND_MOUSE);

			 TEventUI event = { 0 };
			 event.ptMouse = m_ptLastMousePos;
			 if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
				 event.nType = UIEVENT_MOUSELEAVE;
				 event.wParam = wParam;
				 event.lParam = lParam/*(LPARAM)pNewHover*/;
				 m_pEventHover->SendEvent(event);
				 if (::IsWindow(m_hTipWindow) && IsWindowVisible(m_hTipWindow))
					HideToolTips();
				 m_pEventHover = NULL;
			 }
			 if( pNewHover != m_pEventHover && pNewHover != NULL ) {
				 event.nType = UIEVENT_MOUSEENTER;
				 event.wParam = wParam;
				 event.lParam = lParam/*(LPARAM)m_pEventHover*/;
				 pNewHover->SendEvent(event);
				 m_pEventHover = pNewHover;
			 }
			 if( m_pEventClick != NULL ) {
				 event.nType = UIEVENT_MOUSEMOVE;
				 event.wParam = wParam;
				 event.lParam = lParam;
				 m_pEventClick->SendEvent(event);

			 }
			 else if( pNewHover != NULL ) {
				 event.nType = UIEVENT_MOUSEMOVE;
				 event.ptMouse = m_ptLastMousePos;
				 event.wParam = wParam;
				 event.lParam = lParam;
				 pNewHover->SendEvent(event);
			 }

			 if (pNewHover == NULL && wParam==MK_LBUTTON && m_bEnableMove && !IsMaximized())
			 {
				::PostMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
			 }
		 }
		 break;

		 case WM_SHOWWINDOW:
			 {
				 // 合共OnWindowInit调用时机调整（窗口创建完后，只调用一次）
				/* if (wParam == FALSE)
					 m_bUnFirstPaint = true;*/
				BOOL bShow = (BOOL)wParam;
				if (!bShow)
				{
					this->HideToolTips();
				}

				 if (!m_pShadowWindow) break;
				
				 if (bShow != ::IsWindowVisible(m_pShadowWindow->GetHWND()))
					 ::ShowWindow(m_pShadowWindow->GetHWND(), bShow ? SW_SHOWNOACTIVATE : SW_HIDE);

				 if (bShow && lParam == SW_PARENTOPENING)
				 {
					 HWND hParent = ::GetParent(GetHWND());
					 if (hParent)
					 {
						 RECT rect;
						 ::GetWindowRect(m_pShadowWindow->GetHWND(), &rect);
						 POINT pt = {rect.left, rect.top};
						 ScreenToClient(hParent, &pt);
						 int w = rect.right - rect.left;
						 int h = rect.bottom - rect.top;
						 rect.left = pt.x;
						 rect.top = pt.y;
						 rect.right = rect.left + w;
						 rect.bottom = rect.top + h;
						 ::RedrawWindow(hParent, &rect, NULL, RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN);
					 }
					
				 }
			 }
			 break;
		 case WM_PAINT:
			 {
				 m_bUpdateNeeded = false;
				 PAINTSTRUCT ps = { 0 };
				 ::BeginPaint(m_hWnd, &ps);

				bool bDraw = true;
				RECT rcPaint;
				if (IsLayerWindow())
				{
					rcPaint = m_rectUpdate;
					if (IsRectEmpty(&rcPaint))
						bDraw = false;
				}
				else
				{
					rcPaint = ps.rcPaint;
				}
				ZeroMemory(&m_rectUpdate, sizeof(RECT));

				if (bDraw)
				{
					RECT rcClient;
					GetClientRect(GetHWND(), &rcClient);
					FlushToBackBuffer(rcPaint, rcClient);
					FlushToForeBuffer(rcPaint, ps.hdc);
				}	
				::EndPaint(m_hWnd, &ps);

				 if( m_bUpdateNeeded )
					 Invalidate();
				 if (m_bUnFirstPaint)
				 {
					 m_bUnFirstPaint = false;
					 SetShadow(m_bShadow);
					OnWindowInit();
				 }
			 }
			 return true;

	 case WM_MOUSEHOVER:
		 {
			 m_bMouseTracking = false;
			 POINT pt = {(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };
			 CControlUI* pHover = FindControl(pt, UIFIND_MOUSE_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST | UIFIND_MOUSE);
		
			 if( pHover == NULL ) break;
			 if( m_pEventHover != NULL && m_pEventHover == pHover) {
				 TEventUI event = { 0 };
				 event.ptMouse = pt;
				 event.nType = UIEVENT_MOUSEHOVER;
				 event.wParam = wParam;
				 event.lParam = lParam;
				 m_pEventHover->SendEvent(event);
				 
			 }
			
			 if(IsCaptured())
				 return true;
			 // 解决BUG #48829
			 if(/* pHover->GetToolTip() && _tcslen(pHover->GetToolTip()) == 0 ||*/ !pHover->IsToolTipShow()) 
			 {
		
				 return true;
			 }

			 if (!GetToolTipWnd())
			 {
				 m_pTipWindow = new TipWindow;
				 m_pTipWindow->CreateTip(GetHWND());
				 m_hTipWindow = m_pTipWindow->GetHWND();
			 }
			 m_pTipWindow->ShowTip(true, pHover->GetToolTip(), &pHover->GetRect());
		 }
		 break;

	 case WM_MOUSELEAVE:
		 {
			 //解决BUG#52909 和BUG#47126
			 if( m_bMouseTracking ) 
			 {	
				  HideToolTips();
				 ::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM) -1);
			 }
			 m_bMouseTracking = false;

			 if( m_pEventHover) 
			 {   
				 TEventUI event = { 0 };
				 event.ptMouse = m_ptLastMousePos;
				 event.nType = UIEVENT_MOUSELEAVE;
				 event.wParam = wParam;
				 event.lParam = lParam;
				 m_pEventHover->SendEvent(event);
				 m_pEventHover = NULL;
			 }
		 }
		 break;
	 case WM_LBUTTONDOWN:
		 {
			 if (m_bAreaSelecting)
			 {
				 POINT pt = {(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };
				 CControlUI * pControl = m_pRoot->FindControl(__FindControlEnableAreaSelection, &pt,  UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_HITTEST | UIFIND_TOP_FIRST);
				 if (pControl)
				 {
					 m_bAreaSelecting = true;
					 TEventUI event = { 0 };
					 event.nType = UIEVENT_AREA_SELECT_END;
					 event.wParam = wParam;
					 event.lParam = lParam;
					 event.ptMouse = m_ptLastMousePosClick;
					 pControl->SendEvent(event);
				 }
				 m_bAreaSelecting = false;
			 }
			 if (m_pControlDrag)
			 {
				 TEventUI event = { 0 };
				 event.nType = UIEVENT_DRAG_DROP_GROUPS;
				 event.wParam = UIEVENT_END_DRAG;
				 event.lParam = lParam;
				 event.ptMouse = m_ptLastMousePosClick;
				 m_pControlDrag->SendEvent(event);
				 m_bDraging = false;
				 m_pControlDrag = NULL;
			 }
			 //add by hanzp. 防误操作
			 if (wParam & MK_RBUTTON)
				 break;

			 //add by hanzp.
			 HideToolTips();
			 //end add by hanzp.
			SetFocusOnWnd();
			 POINT pt = {(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };
			 m_ptLastMousePos = pt;
			 m_ptLastMousePosClick = pt;
			 CControlUI* pControl = FindControl(pt);
			 // SetCapture移动到这个位置飞，防止点击空白地方没调用到
		     //SetCapture();
			 if( pControl == NULL )
			 {
				// !m_pFocus 判断原因是：当编辑框获取焦点时，点击其他空白地方时失去焦点，否则比如聊天框个性签名签名获取焦点时，点击其他地方焦点都不会失去
				if (!m_pFocus)
					  SetFocus(NULL); 
				break;
			 }
		
			 // 如果控件不能获取焦点则不获取焦点
			 if ((pControl->GetFlags() & UICONTROLFLAG_SETFOCUS) != 0)	
				 pControl->SetFocus();

			 // We always capture the mouse
			  m_pEventClick = pControl;
			 SetCapture();
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_BUTTONDOWN;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 event.ptMouse = pt;
			 pControl->SendEvent(event);
	
			
		 }
		break;

	 case WM_RBUTTONDOWN:
		 {
			 //add by hanzp. 防误操作
			 if (wParam & MK_LBUTTON)
				 break;

			 //add by hanzp.
			 HideToolTips();
			 //end add by hanzp.

			SetFocusOnWnd();
			 POINT pt = { (int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam)};
			 m_ptLastMousePos = pt;
			 CControlUI* pControl = FindControl(pt);
			 if( pControl == NULL )
			 {
				 SetFocus(NULL);
				 break;
			 }

			 m_pEventClick = pControl;
			 pControl->SetFocus();
			 // We always capture the mouse
			 SetCapture();
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_RBUTTONDOWN;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 event.ptMouse = pt;
			 pControl->SendEvent(event);
		 }
		 break;

	 case WM_LBUTTONUP:
		 {
			 if (m_pControlDrag)
			 {
				 TEventUI event = { 0 };
				 event.nType = UIEVENT_DRAG_DROP_GROUPS;
				 event.wParam = UIEVENT_END_DRAG;
				 event.lParam = lParam;
                 event.ptMouse = { (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) };
				 m_pControlDrag->SendEvent(event);
				 m_bDraging = false;
				 m_pControlDrag = NULL;
			 }
			 if (m_bAreaSelecting)
			 {
				 CControlUI * pControl = m_pRoot->FindControl(__FindControlEnableAreaSelection, &m_ptLastMousePosClick,  UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_HITTEST | UIFIND_TOP_FIRST);
				 if (pControl)
				 {
					 m_bAreaSelecting = true;
					 TEventUI event = { 0 };
					 event.nType = UIEVENT_AREA_SELECT_END;
					 event.wParam = wParam;
					 event.lParam = lParam;
					 event.ptMouse = m_ptLastMousePosClick;
					 pControl->SendEvent(event);
				 }
				 m_bAreaSelecting = false;
				 {
					 //m_bDraging = false;
					 POINT pt = { (int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };
					 m_ptLastMousePos = pt;
					 ReleaseCapture();
					 m_pEventClick = NULL;
					 break;
				 }
			 }
			 //add by hanzp. 防误操作
			 if (wParam & MK_RBUTTON)
				 break;
			 m_bDraging = false;
			 POINT pt = { (int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };
			 m_ptLastMousePos = pt;
			 // 这句话要提前到前面来，否则在down,或dbclick中可能析构 m_pEventClick，导致ReleaseCapture()没有调用到
			 ReleaseCapture();
			 ms_pLastClickEvent = m_pEventClick;
			 if( m_pEventClick == NULL )  break;
			 CControlUI* pClickControl = m_pEventClick;
			 m_pEventClick = NULL;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_BUTTONUP;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 event.ptMouse = pt;
			 pClickControl->SendEvent(event);
		 }
		 break;

	 case WM_RBUTTONUP:
		 {
			 //add by hanzp. 防误操作
			 if (wParam & MK_LBUTTON)
				 break;

			 POINT pt = {(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };
			 m_ptLastMousePos = pt;
			 if( m_pEventClick == NULL )
			 {
				 m_ptLastMousePos = pt;
				 CControlUI* pControl = FindControl(pt);
				 if( pControl == NULL )
				 {
					 SetFocus(NULL);
					 break;
				 }
				 m_pEventClick = pControl;
			 }
			 ReleaseCapture();
			 CControlUI* pClickControl = m_pEventClick;
			 m_pEventClick = NULL;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_RBUTTONUP;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 event.ptMouse = pt;
			 pClickControl->SendEvent(event);
		 }
		 break;

	 case WM_LBUTTONDBLCLK:
		 {
			 //add by hanzp. 防误操作
			 if (wParam & MK_RBUTTON)
				 break;
			 POINT pt = {  (int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };

			 //有控件不需要处理
			 m_ptLastMousePos = pt;
			 CControlUI* pControl = ms_pLastClickEvent;
			 if (pControl != NULL)
			 {
				 // modify by hanzp.
				 SetCapture();

				 // 修改一下这句话，这句应该放在这个位置，不能放到最后，因为有可能dbclick的时候会把本身控件删除掉
				 m_pEventClick = pControl;

				 TEventUI event = { 0 };
				 event.nType = UIEVENT_LDBCLICK;
				 event.wParam = wParam;
				 event.lParam = lParam;
				 event.ptMouse = pt;
				 pControl->SendEvent(event);

				 break;
			 }

			 if (IsEnableResize()) 
			 {

				if (!m_pRoot->FindControl(__FindControlEnableAreaSelection, &pt,  UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_HITTEST | UIFIND_TOP_FIRST))
				{
					 if (IsMaximized())
						 RestoreWindow();
					 else
						 MaximizeWindow();
				}
			 }

			 
			 // modify by hanzp.
// 			 SetCapture();
// 
// 			 // 修改一下这句话，这句应该放在这个位置，不能放到最后，因为有可能dbclick的时候会把本身控件删除掉
// 			  m_pEventClick = pControl;
// 			 TEventUI event = { 0 };
// 			 event.nType = UIEVENT_LDBCLICK;
// 			 event.wParam = wParam;
// 			 event.lParam = lParam;
// 			 event.ptMouse = pt;
// 			 pControl->SendEvent(event);

		 }
		 break;

	 case WM_RBUTTONDBLCLK:
		 {
			 //add by hanzp. 防误操作
			 if (wParam & MK_LBUTTON)
				 break;

			 POINT pt = { (int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam) };
			 m_ptLastMousePos = pt;
			 CControlUI* pControl = FindControl(pt);
			 if( pControl == NULL ) break;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_RDBCLICK;
			 event.ptMouse = pt;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 pControl->SendEvent(event);
			 m_pEventClick = pControl;
			 // We always capture the mouse
			 // modify by hanzp.
			 //SetCapture();
		 }
		 break;
	 case WM_CHAR:
		 {
			 if( m_pFocus == NULL ) break;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_CHAR;
			 event.ptMouse = m_ptLastMousePos;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 m_pFocus->SendEvent(event);
		 }
		 break;

	 case WM_IME_CHAR:
		 {
			 if( m_pFocus == NULL ) break;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_IME_CHAR;
			 event.ptMouse = m_ptLastMousePos;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 m_pFocus->SendEvent(event);
		 }
		 break;

	 case WM_IME_STARTCOMPOSITION:
		 {
			 if( m_pFocus == NULL ) break;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_IME_START;
			 event.ptMouse = m_ptLastMousePos;
			 // event.dwTimestamp = ::GetTickCount();
			 event.wParam = wParam;
			 event.lParam = lParam;
			  m_pFocus->SendEvent(event);
		 }
		 break;
	 case WM_IME_ENDCOMPOSITION:
		 {
			 if( m_pFocus == NULL ) break;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_IME_END;
			 event.ptMouse = m_ptLastMousePos;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 m_pFocus->SendEvent(event);
		 }
		 break;
	 case WM_IME_COMPOSITION:
		 {
			 if( m_pFocus == NULL ) break;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_IME_ING;
			 event.ptMouse = m_ptLastMousePos;
			 event.wParam = wParam;
			 event.lParam = lParam;
			  m_pFocus->SendEvent(event);
			 if (m_bMsgHandled)
			 {
				 lRes = S_OK;
				 return true;
			 }
		 }
		 break;

	 case WM_KEYDOWN:
		 {
			 if( m_pFocus
				 && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
			 {
				 TEventUI event = { 0 };
				 event.nType = UIEVENT_KEYDOWN;
				 event.ptMouse = m_ptLastMousePos;
				 event.wParam = wParam;
				 event.lParam = lParam;
				 m_pFocus->SendEvent(event);
				 m_pEventKey = m_pFocus;
			 }

             switch (wParam)
             {
             case VK_RETURN:
             {
                 if (m_pFocus == NULL || (m_pFocus->GetFlags() & UICONTROLFLAG_WANTRETURN) == 0)
                 {
                     // 按默认回车需要处理
                     if (!m_strOkControl.empty())
                     {
                         CControlUI* pOkControl = GetItem(m_strOkControl.c_str());
                         if (pOkControl && pOkControl != m_pFocus)
                         {
                             pOkControl->Activate();
                             break;
                         }
                     }
                     OnOk();
                 }
             }
             break;
             case VK_ESCAPE:
             {
                 if (m_pFocus == NULL || (m_pFocus->GetFlags() & UICONTROLFLAG_WANTESCAPE) == 0)
                     OnCancel();
             }
             break;
             case VK_TAB:
             {
                 if (m_bVKTabEnable)
                 {
                     SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
                 }
             }
             break;
             default:
                 break;
             }
     }
         break;

	 case WM_KEYUP:
		 {
			 if( m_pEventKey == NULL ) break;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_KEYUP;
			 event.ptMouse = m_ptLastMousePos;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 m_pEventKey->SendEvent(event);
			 m_pEventKey = NULL;
		 }
		 break;

	 case WM_SETCURSOR:
		 {
			 if (LOWORD(lParam) != HTCLIENT ) break;
			 if( m_bMouseCapture ) return true;

			 POINT pt = { 0 };
			 ::GetCursorPos(&pt);
			 ::ScreenToClient(m_hWnd, &pt);
			 CControlUI* pControl = FindControl(pt);
			 if( pControl == NULL ) break;
			 if( (pControl->GetFlags() & UICONTROLFLAG_SETCURSOR) == 0 ) break;
			 TEventUI event = { 0 };
			 event.nType = UIEVENT_SETCURSOR;
			 event.wParam = wParam;
			 event.lParam = lParam;
			 event.ptMouse = pt;
			 pControl->SendEvent(event);

		 }
		 return true;

	 case WM_ERASEBKGND:
		 {
			 lRes = 1;
		 }
		 return true;

	 case WM_TIMER:
		 {
			 for( int it = 0; it < m_aTimers.GetSize(); it++) {
				 TIMERINFO* pTimer =  (TIMERINFO*)m_aTimers.GetAt(it);
				 if( pTimer->hWnd == m_hWnd && pTimer->uWinTimer == LOWORD(wParam) ) 
				 {
					 TEventUI event = { 0 };
					 event.nType = UIEVENT_TIMER;
					 event.wParam = pTimer->nLocalID;
					 pTimer->pSender->SendEvent(event);
					 break;
				 }
			 }
		 }
		 break;

	 case WM_NOTIFY:
		 {
			 LPNMHDR lpNMHDR = (LPNMHDR) lParam;
			 if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
			 if (lRes)
				 return true;
		 }
		 break;
	 case WM_COMMAND:
		 {
			 if( lParam == 0 ) break;
			 HWND hWndChild = (HWND) lParam;
			 lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			 if (lRes)
				 return true;
		 }
		 break;

	 case WM_NCHITTEST:
		 {
			 if (IsEnableResize())
				 lRes = HitTest(wParam, lParam);
			 else
				 return false;
		 }
		 return true;

	 case WM_NCLBUTTONDOWN:
		 {
			 if (IsEnableResize() && (lRes = OnNcLButtonDown(m_hWnd, wParam, lParam)))
			 {
				 return true;
			 }
			 else
			 {
				 return false;
			 }
		 }
		 return true;
	 case WM_DROPFILES:
		 {
			 POINT pt = { 0 };
			 ::GetCursorPos(&pt);
			 ::ScreenToClient(m_hWnd, &pt);
			 CControlUI * pControl = m_pRoot->FindControl(__FindControlEnableDrop, &pt,  UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
			 if (pControl)
			 {
				 pControl->SendNotify(UINOTIFY_DROPFILES, wParam, lParam);
			 }
		 }
		 return true;

	 case WM_HOTKEY:
		 {
			GetRoot()->SendNotify(UINOTIFY_HOTKEY, wParam, lParam);	
		 }
		 break;
	 case WM_CLOSE:
		 {
			 m_uModalResult = wParam;
			 if (!OnClosing()) return true;
			 bool bFocus = (::GetFocus() == m_hWnd);
			 TEventUI event = { 0 };
			 event.ptMouse = m_ptLastMousePos;
			 if( m_pEventHover != NULL ) {
				 event.nType = UIEVENT_MOUSELEAVE;
				 m_pEventHover->SendEvent(event);
			 }
			 if( m_pEventClick != NULL ) {
				 event.nType = UIEVENT_BUTTONUP;
				 m_pEventClick->SendEvent(event);
			 }
			 SetFocus(NULL);

			 if (bFocus)
			 {
				 HWND hwndParent = GetWindow(m_hWnd, GW_OWNER);
				 if( hwndParent != NULL ) 
					 ::SetFocus(hwndParent);
			 }
			 if (IsModal())
			 {
				 EnableWindow(GetWindow(m_hWnd, GW_OWNER), TRUE);
			 }

		 }
		 break;
	 case WM_KILLFOCUS:
		 {
			 this->KillFocus(m_pFocus);
		 }
		 break;

	 case WM_SIZE:
		 {
			 // 最大化
			 if (wParam == SIZE_MAXIMIZED)
			 {	 
				 if (!m_bMaximized)
				 {
					 CControlUI* pControlUI = GetItem(_T("sysbutton.maximize"));
					 if (pControlUI) pControlUI->Select(true);	
				 }
				 lRes = CWindowBase::WindowProc(uMsg, wParam, lParam);
				 m_bMaximized = true;
				 m_bMinimized = false;
				  m_bSizeChanged = true;
				  m_bUpdateNeeded = true;
				 OnMaximize();		
			 }
			 else if (wParam == SIZE_MINIMIZED) // 最小化
			 {
				 MONITORINFO oMonitor = {};
				 oMonitor.cbSize = sizeof(oMonitor);
				 ::GetMonitorInfo(::MonitorFromWindow(GetHWND(), MONITOR_DEFAULTTONEAREST), &oMonitor);

				 RECT rcWork = oMonitor.rcWork;
				 lRes = CWindowBase::WindowProc(uMsg, wParam, lParam);
				 m_bMaximized = false;
				 m_bMinimized = true;
				 OnMinimize();
			
			 }
			 else if (wParam == SIZE_RESTORED) // 还原
			 {
				 if (m_bMaximized)
				 {
					 CControlUI* pControlUI = GetItem(_T("sysbutton.maximize"));
					 if (pControlUI) pControlUI->Select(false);	
					 lRes = CWindowBase::WindowProc(uMsg, wParam, lParam);
					 OnRestored();
					  m_bSizeChanged = true;
				 }
				 else
				 {
					  lRes = CWindowBase::WindowProc(uMsg, wParam, lParam);
					   OnRestored();
				 }
				 m_bMaximized = false;
				 m_bMinimized = false;
				 m_bUpdateNeeded = true;
			 }
			 else
			 {
				 lRes = CWindowBase::WindowProc(uMsg, wParam, lParam);
				 m_bMaximized = false;
				 m_bMinimized = false;
				 m_bSizeChanged = true;
				 m_bUpdateNeeded = true;
			 }

			
			
			 if (!IsLayerWindow())
			 {
				OnSetWindowRgn();
			 }
			 // 如果没有这一句，WINDOWS7 下可能会导致窗口没刷新
			 MoveShadowWindow();
			 if (m_bUpdateNeeded)
				 Invalidate(NULL, false);
		 }
		 return true;

	 case WM_MOVE:
		 {
			 lRes = CWindowBase::WindowProc(uMsg, wParam, lParam);
			 MoveShadowWindow();
		 }
		 return true;

	 case WM_NCDESTROY: 
		 { 
			 CCaretUI::GetInstance()->DestroyCaret(this);
			 OnClose();
		 }
		 break;

	 case WM_GETMINMAXINFO:
		 {		
			 LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
			 if(m_iMinWidth>0)lpMMI->ptMinTrackSize.x =m_iMinWidth;
			 if(m_iMinHeight>0)lpMMI->ptMinTrackSize.y = m_iMinHeight;

			 MONITORINFO oMonitor = {};
			 oMonitor.cbSize = sizeof(oMonitor);
			 ::GetMonitorInfo(::MonitorFromWindow(GetHWND(), MONITOR_DEFAULTTONEAREST), &oMonitor);
			 lpMMI->ptMaxPosition.x	= abs(oMonitor.rcMonitor.left  - oMonitor.rcWork.left);
			 lpMMI->ptMaxPosition.y	= abs(oMonitor.rcMonitor.top - oMonitor.rcWork.top);
			 DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
			 if (dwStyle & WS_MAXIMIZE)
			 {
				 RECT rcWork = oMonitor.rcWork;
				 // 窗口最小化后最大化 rcwork会不准确，因此必须使用最小化之前的数据
				 if (m_bMinimized)
					rcWork = m_rectWork;
				 lpMMI->ptMaxTrackSize.x  = abs(rcWork.right - rcWork.left);
				 lpMMI->ptMaxTrackSize.y  = abs(rcWork.bottom - rcWork.top);
			 }
			 else if (dwStyle & WS_MINIMIZE)
			 {
				 m_rectWork =  oMonitor.rcWork;
			 }
			 else
			 {
				 if (m_iMaxWidth > 0)
					  lpMMI->ptMaxTrackSize.x  = m_iMaxWidth;
				 if (m_iMaxHeight > 0)
					  lpMMI->ptMaxTrackSize.y  = m_iMaxHeight;
			 }
			 break;
		 }
	
	 //// 如图窗口带有WS_THICKFRAME风格的属性可以去掉边框
	  case WM_NCCALCSIZE:
		  {
			  lRes = 0;
			  NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;
			  if((BOOL)wParam)
			  {
				  lpncsp->rgrc[2] = lpncsp->rgrc[1];
				  lpncsp->rgrc[1] = lpncsp->rgrc[0];
			  }
			  return true;
		  }
		  break;

	  case WM_NCACTIVATE:
		  {
			  bool bHandled = TRUE;
			  if( ::IsIconic(m_hWnd) ) bHandled = FALSE;
			  lRes = (wParam == 0) ? TRUE : FALSE;
			  return bHandled;
		  }
		  break;
	  case WM_NCPAINT:
		  {
			  lRes = 0;
			  return true;
		  }
		  break;
	  case WM_MOUSEWHEEL:
		  {
			  if (m_bAreaSelecting && IsCaptured())
				  break;

			  POINT pt = { (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) };
			  ::ScreenToClient(GetHWND(), &pt);

			  CControlUI* pControl = FindControl(pt, UIFIND_MOUSEWHELL_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
			  if (pControl)
			  {
				  m_bMouseWheel = true;
				  TEventUI event = { 0 };
				  event.nType = UIEVENT_SCROLLWHEEL;
				  event.wParam = wParam;
				  event.lParam = lParam;
				  event.ptMouse = pt;
				  event.pSender = pControl;
				  pControl->SendEvent(event);
				  ::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
				  return true;
			  }

			  if (m_pFocus || m_pEventHover)
			  {
				  m_bMouseWheel = true;
				  TEventUI event = { 0 };
				  event.nType = UIEVENT_SCROLLWHEEL;
				  event.wParam = wParam;
				  event.lParam = lParam;
				  event.ptMouse = pt;
				  if (m_pFocus != m_pEventHover && m_pEventHover)
				  {
					  event.pSender = m_pEventHover;
					  m_pEventHover->SendEvent(event);

				  }
				  else if (m_pFocus)
				  {
					  event.pSender = m_pFocus;
					  m_pFocus->SendEvent(event);
				  }
				  ::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
				  return true;
			  }
		  }
		  break;
	  case WM_GESTURENOTIFY:
	  {
		  GESTURENOTIFYSTRUCT* pGns = (GESTURENOTIFYSTRUCT*)lParam;
		  POINT pt = { pGns->ptsLocation.x, pGns->ptsLocation.y };
		  ::ScreenToClient(GetHWND(), &pt);
		  CControlUI* pControl =  FindControl(pt, UIFIND_ENABLED | UIFIND_GESTURE_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
		  if (pControl)
		  {
			  TEventUI event = { 0 };
			  event.nType = UIEVENT_GESTURENOTIFY;
			  event.wParam = wParam;
			  event.lParam = lParam;
			  event.ptMouse = pt;
			  pControl->SendEvent(event);
		  }
		  else
		  {
			  GESTURECONFIG gc = { 0, 0, GC_ALLGESTURES };
			  UINT uiGcs = 1;
			  if (set_gesture_config)
				  set_gesture_config(GetHWND(), 0, uiGcs, &gc, sizeof(GESTURECONFIG)); 

		  }
	  }
	  break;
	  case WM_GESTURE:
	  {
		  lRes = DecodeGesture(wParam, lParam);
		   return true;
	  }
	  break;
	}
	return false;
}


LRESULT CWindowUI::DecodeGesture(WPARAM wParam, LPARAM lParam)
{
	if (!get_gesture_info)
	{
		return __super::WindowProc(WM_GESTURE, wParam, lParam);
	}

	HWND hWnd = GetHWND();
	GESTUREINFO gi = {};
	gi.cbSize = sizeof(GESTUREINFO);

	bool bHandled = false;
	if (get_gesture_info((HGESTUREINFO)lParam, &gi))
	{
		POINT pt = { gi.ptsLocation.x, gi.ptsLocation.y };
		::ScreenToClient(hWnd, &pt);
		TEventUI event = { 0 };
		event.nType = UIEVENT_GESTURE;
		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = pt;
		switch (gi.dwID)
		{
		case GID_BEGIN:
		{
			m_pGuesture = FindControl(pt, UIFIND_ENABLED | UIFIND_GESTURE_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
			if (m_pGuesture)
			{
				m_pGuesture->SendEvent(event);
			}
		}
		break;

		case GID_END:
		{
			if (m_pGuesture)
			{
				m_pGuesture->SendEvent(event);
			}
			m_pGuesture = NULL;
		}
		break;
		default:
		{
			
			if (gi.dwFlags & GF_BEGIN)
			{
				if (!m_pGuesture)
					m_pGuesture = FindControl(pt, UIFIND_ENABLED | UIFIND_GESTURE_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
				if (m_pGuesture)
				{
					m_pGuesture->SendEvent(event);
				}
			}
			else if (gi.dwFlags & GF_END)
			{
				if (m_pGuesture)
				{
					m_pGuesture->SendEvent(event);
				}
			}
			else
			{
				if (m_pGuesture)
				{
					m_pGuesture->SendEvent(event);
				}
			}
		}
		break;
		}
	}
	if (m_bMsgHandled)
	{
		if (close_gesture_info_handle)
			close_gesture_info_handle((HGESTUREINFO)lParam);
		return S_OK;
	}
	return __super::WindowProc(WM_GESTURE, wParam, lParam);
}

HDC  CWindowUI::GetMemDC()
{
	return m_pDibObj->GetSafeHdc();
}


CControlUI * CWindowUI::GetEventClickControl()
{
	return m_pEventClick;
}

bool  CWindowUI::SetNextTabControl(bool bForward)
{
	// If we're in the process of restructuring the layout we can delay the
	// focus calulation until the next repaint.
	if( m_bUpdateNeeded && bForward ) {
		m_bFocusNeeded = true;
		Invalidate();
		return true;
	}
	// Find next/previous tabbable control
	FINDTABINFO info1 = { 0 };
	info1.pFocus = m_pFocus;

	//为了防止滚动条占用焦点导致tab顺序发生错误，添加该判断
	/*if (m_pFocus != NULL && (m_pFocus->GetFlags() & UICONTROLFLAG_TABSTOP == 0))
	info1.pFocus = m_pFocus->GetParent();*/

	info1.bForward = bForward;
	CControlUI* pControl = NULL;

	bool bNeedFindAgain = true;
	do
	{
		pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_ENABLED | UIFIND_ME_FIRST);

		info1.pFocus = pControl;
		info1.bNextIsIt = false;

		if (pControl == NULL)
		{
			if (!info1.bForward)
			{
				pControl = info1.pLast;
				break;
			}

			if (!bNeedFindAgain)
			{
				break;
			}

			bNeedFindAgain = false;
			continue;
		}


		if (!pControl->IsVisible()
			|| !pControl->IsInternVisible())
		{
			continue;
		}

		RECT& rt = pControl->GetRect();
		if (rt.right - rt.left == 0
			|| rt.bottom - rt.top == 0)
		{
			continue;
		}

		break;

	} while (true);

// 	info1.bForward = bForward;
// 	CControlUI* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
// 	if( pControl == NULL ) {  
// 		if( bForward ) {
// 			// Wrap around
// 			FINDTABINFO info2 = { 0 };
// 			info2.pFocus = bForward ? NULL : info1.pLast;
// 			info2.bForward = bForward;
// 			pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
// 		}
// 		else {
// 			pControl = info1.pLast;
// 		}
// 	}
	if( pControl != NULL ) SetFocus(pControl);
	m_bFocusNeeded = false;
	return true;
}

CControlUI* CWindowUI::GetFocus()
{
	return m_pFocus;
}

void CWindowUI::KillFocus(CControlUI* pControl)
{
	if( pControl != m_pFocus ) return;
	if( m_pFocus != NULL ) 
	{
		CControlUI* pFocus = m_pFocus;
		m_pFocus = NULL;
		TEventUI event = { 0 };
		event.nType = UIEVENT_KILLFOCUS;
		if (pFocus)
			 pFocus->SendEvent(event);
	}
}

void CWindowUI::SetFocus(CControlUI* pControl)
{
	if( pControl == m_pFocus ) return;
	//添加判断，防止点击编辑框滚动条时失去焦点
	if( pControl && pControl->GetParent() == m_pFocus) 
	{
		if ((pControl->GetFlags() & UICONTROLFLAG_SETFOCUS) == 0)
			return;
	}

	if( ::GetFocus() != m_hWnd) 
		SetFocusOnWnd();
	if( m_pFocus != NULL ) 
	{
		CControlUI* pFocus = m_pFocus;
		m_pFocus = NULL;
		TEventUI event = { 0 };
		event.nType = UIEVENT_KILLFOCUS;
        event.lParam = (LPARAM)pControl;
		if (pFocus)
			pFocus->SendEvent(event);
	}

	if (pControl == NULL)
	{
		m_pFocus = NULL;
		return;
	}
	if( pControl != NULL && pControl->IsVisible() && pControl->IsEnabled() ) 
	{
		m_pFocus = pControl;
		TEventUI event = { 0 };
		event.nType = UIEVENT_SETFOCUS;
		if (m_pFocus)
			m_pFocus->SendEvent(event);
	}

}


void CWindowUI::SetCapture()
{
	if (GetCapture() != m_hWnd)
		::SetCapture(m_hWnd);

	m_bMouseCapture = true;
}

void CWindowUI::ReleaseCapture()
{
	if (GetCapture() == m_hWnd)
		::ReleaseCapture();
	m_bMouseCapture = false;
}
bool CWindowUI::IsCaptured()
{
	return m_bMouseCapture;
}


int CWindowUI::SetTimer( CControlUI* pControl, UINT nTimerID, UINT uElapse )
{
	if (!pControl/* && uElapse < 0*/)
		return FALSE;

	for( int it = 0; it < m_aTimers.GetSize(); it++) {
		TIMERINFO* pTimer =  (TIMERINFO*)m_aTimers.GetAt(it);
		if( pTimer->pSender == pControl
			&& pTimer->hWnd == m_hWnd
			&& pTimer->nLocalID == nTimerID)
		{
			return 2;
		}
	}

	m_uTimerID = (m_uTimerID + 1) % DUI_TIMERID_BASE + DUI_TIMERID_BASE;
	UINT uRet = ::SetTimer(m_hWnd, m_uTimerID, uElapse, NULL);
	if( !uRet )
	{
		return FALSE;
	}
	TIMERINFO* pTimer = new TIMERINFO;
	pTimer->hWnd = m_hWnd;
	pTimer->pSender = pControl;
	pTimer->nLocalID = nTimerID;
	pTimer->uWinTimer = m_uTimerID;
	m_aTimers.Add(pTimer);
	return TRUE;
}

bool CWindowUI::KillTimer(CControlUI* pControl, UINT nTimerID)
{
	if (!pControl)
		return false;
	for( int it = 0; it < m_aTimers.GetSize(); it++) {
		TIMERINFO* pTimer =  (TIMERINFO*)m_aTimers.GetAt(it);
		if( pTimer->pSender == pControl
			&& pTimer->hWnd == m_hWnd
			&& pTimer->nLocalID == nTimerID )
		{
			::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
			delete pTimer;
			m_aTimers.Remove(it);
			return true;
		}
	}
	return false;
}


// 锁定/解锁绘制
bool CWindowUI::LockUpdate()
{
	bool bOriginalLock = m_bLockUpdate;
	m_bLockUpdate = true;
	return bOriginalLock;
}

void CWindowUI::UnLockUpdate()
{
	m_bLockUpdate = false;
}

void CWindowUI::Invalidate(RECT* lpRect, bool bUpdate)
{
	if (m_bLockUpdate) return;

	if (!::IsWindowVisible(GetHWND()))
		return;
	if (m_hWnd)
	{
		RECT rcClient;
		GetClientRect(GetHWND(), &rcClient);
		RECT rcItem = rcClient;
		if (lpRect)
			IntersectRect(&rcItem, &rcClient, lpRect);
		if (IsRectEmpty(&rcItem))
			return;
		
		m_bUpdateNeeded = true;
		//判断是否采用层叠窗口
		if (IsLayerWindow())
		{
			if (IsRectEmpty(&m_rectUpdate))
				m_rectUpdate = rcItem;
			else
				UnionRect(&m_rectUpdate, &m_rectUpdate, &rcItem);

			IntersectRect(&m_rectUpdate, &rcClient, &m_rectUpdate);
		}

		::InvalidateRect(m_hWnd, &rcItem, false);
		if (bUpdate) UpdateWindow(m_hWnd);
	}
}

void CWindowUI::SetLayerWindow(bool bLayer, bool bRedraw)
{
	m_bLayerWindow = bLayer;
	if (!IsWindow(m_hWnd)) 
		return;
	if (bLayer)
	{
		ModifyWndStyle(GWL_EXSTYLE, 0, WS_EX_LAYERED);
		OnSetWindowRgn();
		if (bRedraw)
		{
			RECT rcItem;
			GetClientRect(GetHWND(), &rcItem);
			m_rectUpdate = rcItem;
			SendMessage(m_hWnd, WM_PAINT, (WPARAM)m_hDcPaint, (LPARAM)0);
		}
	}
	else
	{
		ModifyWndStyle(GWL_EXSTYLE, WS_EX_LAYERED, 0);
		OnSetWindowRgn();
		RECT rcItem;
		GetClientRect(GetHWND(), &rcItem);
		::RedrawWindow(m_hWnd, &rcItem, NULL, RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN|RDW_UPDATENOW);
	}

}

bool CWindowUI::IsLayerWindow()
{
	return m_bLayerWindow;
}

LRESULT CWindowUI::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (!IsWindow(m_hWnd))
		return S_FALSE;

	LRESULT lRes = S_OK;
	if (MessageHandler(message, wParam, lParam, lRes))
		return lRes;

	return __super::WindowProc(message, wParam, lParam);
	
}

void CWindowUI::CenterWindow( HWND hAlternateOwner /*= NULL*/)
{
	if (m_hWnd && IsWindow(m_hWnd))
	{
		int nStartp_x = 0;
		int nStartp_y = 0;
		RECT rcWnd;
		GetWindowRect(GetHWND(), &rcWnd);
		int nWnd_w = rcWnd.right - rcWnd.left;
		int nWnd_h = rcWnd.bottom - rcWnd.top;

		{
			RECT rcParent;
			if ( hAlternateOwner == NULL )
			{
				hAlternateOwner = ::GetDesktopWindow();
				HMONITOR hMonitor = ::MonitorFromWindow(GetHWND(), MONITOR_DEFAULTTONEAREST);
				if (hMonitor)
				{
					MONITORINFOEX monitorInfo;
					monitorInfo.cbSize = sizeof(MONITORINFOEX);
					::GetMonitorInfo(hMonitor, &monitorInfo);

					rcParent = monitorInfo.rcWork;
				}
			}
			else
			{
				::GetWindowRect(hAlternateOwner, &rcParent);
			}
			int nCenter_X = rcParent.left + (rcParent.right - rcParent.left)/2;
			int nCenter_Y = rcParent.top + (rcParent.bottom - rcParent.top)/2;
			nStartp_x = nCenter_X - nWnd_w/2;
			nStartp_y = nCenter_Y - nWnd_h/2;
			if (nStartp_x == rcParent.left && nStartp_y == rcParent.top)
			{
				nStartp_x += 20;
				nStartp_y += 20;
			}
		}

		//超过屏幕
		RECT rcScreen;
		rcScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		rcScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		rcScreen.right = rcScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
		rcScreen.bottom = rcScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
		if ((nStartp_x+nWnd_w>=rcScreen.right)||(nStartp_y+nWnd_h>=rcScreen.bottom)
			||(nStartp_x+nWnd_w<=rcScreen.left)||(nStartp_y+nWnd_h<=rcScreen.top))
		{
			RECT rcDesktopWindow;
			::GetWindowRect(::GetDesktopWindow(), &rcDesktopWindow);
			int nCenter_X = rcDesktopWindow.left + (rcDesktopWindow.right - rcDesktopWindow.left)/2;
			int nCenter_Y = rcDesktopWindow.top + (rcDesktopWindow.bottom - rcDesktopWindow.top)/2;
			nStartp_x = nCenter_X - nWnd_w/2;
			nStartp_y = nCenter_Y - nWnd_h/2;
			if ((nStartp_x>=rcDesktopWindow.right)||(nStartp_y>=rcDesktopWindow.bottom)
				||(nStartp_x<=rcDesktopWindow.left)||(nStartp_y<=rcDesktopWindow.top))
			{
				nStartp_x = rcDesktopWindow.left + 20;
				nStartp_y = rcDesktopWindow.top + 20;
			}
		}
		UINT uFlags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE;
		SetWindowPos(HWND_DESKTOP, nStartp_x, nStartp_y, 0, 0, uFlags);
	}
}
bool CWindowUI::ShowWindow(int nCmdShow)
{
	if (!IsWindow(m_hWnd)) 
		return false;

	::ShowWindow(m_hWnd, nCmdShow);

	if (IsLayerWindow() && (SW_HIDE != nCmdShow))
	{
		if (!(GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED))
			SetLayerWindow(IsLayerWindow());
	}
	return true;
}

void CWindowUI::OnShowFristSide()
{
	RECT rect = {0, 0, m_pDibObj->GetWidth(), m_pDibObj->GetHeight()};
	FlushToBackBuffer(rect, rect);
}

void CWindowUI::OnShowSecondSide()
{
	RECT rect = {0, 0, m_pDibObj->GetWidth(), m_pDibObj->GetHeight()};
	GetRoot()->OnSize(rect);
	FlushToBackBuffer(rect, rect);
}

void CWindowUI::SetTitle(LPCTSTR lpszText)
{
    if (!lpszText)
		return;
	TCHAR chTitle[MAX_PATH];
	::GetWindowText(GetHWND(), chTitle, MAX_PATH-1);

	if (_tcscmp(chTitle, lpszText) != 0)
	{
		m_strTitle = lpszText;
		SetWindowText(m_hWnd, lpszText);

		CControlUI* pControl = GetItem(_T("title"));
		if (pControl) 
		{
			pControl->SetText(m_strTitle.c_str());
			pControl->Resize();
			pControl->Invalidate();
		}
	}
	
}

LPCTSTR CWindowUI::GetTitle()
{
	return m_strTitle.c_str();
}

void  CWindowUI::SetIcon(LPCTSTR lpszIcon, bool bBigIcon)
{
	if (!bBigIcon)
	{
		if (m_hIcon != NULL)
			::DestroyIcon(m_hIcon);
		ImageObj* pImageObj = GetUIRes()->LoadImage(lpszIcon, false, UIIAMGE_HICON);
		if (pImageObj)
		{
			m_hIcon = pImageObj->GetHIcon();
			pImageObj->Detach();
			pImageObj->Release();
		}

		if (m_hWnd)
		{
			::SendMessage(m_hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)m_hIcon);
			if (!m_hBigIcon)
			::SendMessage(m_hWnd, WM_SETICON, (WPARAM)ICON_BIG,   (LPARAM)m_hIcon);
		}


		if (lpszIcon && _tcslen(lpszIcon) !=0)
		{
			CControlUI* pControl = GetItem(_T("icon"));
			if (pControl)
			{
				pControl->SetAttribute(_T("bk.image"), lpszIcon);
				pControl->Invalidate();
			}
		}
		else
		{
			CControlUI* pControl = GetItem(_T("icon"));
			if (pControl) 
			{
				pControl->ReleaseImage();
				pControl->Invalidate();
			}
		}
	}
	else
	{
		if (m_hBigIcon)
			::DestroyIcon(m_hBigIcon);
		ImageObj* pImageObj = GetUIRes()->LoadImage(lpszIcon, false, UIIAMGE_HICON);
		if (pImageObj)
		{
			m_hBigIcon = pImageObj->GetHIcon();
			pImageObj->Detach();
			pImageObj->Release();
		}
		if (m_hWnd)
		{
			::SendMessage(m_hWnd, WM_SETICON, (WPARAM)ICON_BIG,   (LPARAM)m_hBigIcon);
		}
	}
}

HICON CWindowUI::GetIcon()
{
	return m_hIcon;
}

HICON CWindowUI::GetBigIcon()
{
	return m_hBigIcon;
}

void CWindowUI::SetBkColor(DWORD dwColor)
{
	CControlUI* pControlUI = GetItem(_T("background"));
	if (pControlUI)
	{
		FillStyle* pStyleObj = (FillStyle*)pControlUI->GetStyle(_T("fill"));
		if (pStyleObj)
		{
			pStyleObj->SetFillColor(dwColor);
			pStyleObj->SetVisible(true);
		}
	}
	
}

void CWindowUI::SetBkImage(LPCTSTR lpszFileName)
{
	CControlUI* pControlUI = GetItem(_T("background"));
	if (pControlUI)
	{
		ImageStyle* pImageStyle = dynamic_cast<ImageStyle*>((StyleObj*)pControlUI->GetStyle(_T("bk")));
		if (pImageStyle)
		{
			pImageStyle->ReleaseResource();
			pImageStyle->SetAttribute(_T("image"), lpszFileName);
		}
	}
}

void CWindowUI::EnableResize(bool bEnableResize)
{
	m_bEnableResize = bEnableResize;
	if (m_bEnableResize)
		ModifyWndStyle(GWL_STYLE, 0, WS_THICKFRAME|WS_MAXIMIZEBOX);
	else
		ModifyWndStyle(GWL_STYLE, WS_THICKFRAME|WS_MAXIMIZEBOX, 0);
}

bool CWindowUI::IsEnableResize()
{
	return m_bEnableResize;
}

// 窗口是否能够移动
void CWindowUI::EnableMove(bool bEnableMove)
{
	m_bEnableMove = bEnableMove;
}

bool CWindowUI::IsEnableMove()
{
	return m_bEnableMove;
}

UINT CWindowUI::DoModal(int nCmdShow /*= SW_SHOW*/)
{
	m_uModalResult = 0;
	if (!IsWindow(m_hWnd))
		return m_uModalResult;
	m_bModalDlg = true;
	this->SetAutoDel(false);

	HWND hWndParent = GetWindow(m_hWnd, GW_OWNER);

	CWindowUI* pParentWindow = GetEngineObj()->GetWindow(hWndParent);
	if (pParentWindow && pParentWindow->IsCaptured())
		pParentWindow->ReleaseCapture();

	::ShowWindow(m_hWnd, nCmdShow);
	::EnableWindow(hWndParent, FALSE);

	HWND hWnd = m_hWnd;
	MSG msg = { 0 };
	while(::IsWindow(hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) 
	{
		if( ( msg.message == WM_CLOSE 
			|| (msg.message == WM_SYSCOMMAND && msg.wParam == SC_CLOSE)) && msg.hwnd == hWnd ) {
			m_uModalResult = msg.wParam;
			::EnableWindow(hWndParent, TRUE);
			::SetFocus(hWndParent);
		}


		if (!GetEngineObj()->TranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_QUIT ) 
			break;
	}

	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);

	if( msg.message == WM_QUIT ) 
		::PostQuitMessage(msg.wParam);
	return m_uModalResult;
}

bool CWindowUI::IsModal()
{
	return m_bModalDlg;
}

void CWindowUI::CloseWindow(UINT nRet)
{
	WindowAnimation* pWindowAnition = ActionManager::GetInstance()->getCloseWindowAnimation(this);
	if (pWindowAnition)
	{
		pWindowAnition->SetAnimationState(WindowAnimation::WindowAnimationState_Close);
		ActionManager::GetInstance()->addWindowAnimation(pWindowAnition, this);
	}
	else
	{
		if (::IsWindow(m_hWnd)) 
			::PostMessage(m_hWnd, WM_CLOSE, (WPARAM)nRet, NULL);
	}
}

UINT CWindowUI::GetCloseCode()
{
	return m_uModalResult;
}

void CWindowUI::EndDialog(UINT nRet)
{
	if (!IsWindow(m_hWnd)) return;
	::PostMessage(m_hWnd, WM_CLOSE, (WPARAM)nRet, 0L);
}

HWND CWindowUI::Create(HWND hParent, LPCTSTR lpszId ,  int x/* = 0*/, int y/* = 0*/, int nWidth/* = 0*/, int nHeight/* = 0*/)
{
	// 获取窗口属性
	TemplateObj* pTempObj =GetResObj()->GetWindowTemplateObj(lpszId);
	/*if (!pTempObj)
	{
		return NULL;
	}*/
	
	if (pTempObj)
	{
		// 如果有继承基类，先读取基类属性

		MAPAttr* mapAttr = pTempObj->GetAttr();
		if (mapAttr)
		{
			for (MAPAttr::iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
				SetAttribute(pos->first.c_str(), pos->second.c_str());
		}
	}

	// 创建窗口
	nWidth = (nWidth == 0 ? m_iOrgWidth:nWidth);
	nHeight = (nHeight == 0 ? m_iOrgHeight:nHeight);

	nWidth = (nWidth == 0 ? m_iMinWidth:nWidth);
	nHeight = (nHeight == 0 ? m_iMinHeight:nHeight);

	//add by hanzp
	nWidth = (nWidth == CW_USEDEFAULT) ? 0 : nWidth;
	nHeight = (nHeight == CW_USEDEFAULT) ? 0 : nHeight;
	EnableResize(m_bEnableResize);
	HWND hWnd = CWindowBase::Create(hParent, _T(""), GetWndStyle(GWL_STYLE), GetWndStyle(GWL_EXSTYLE), x, y, nWidth, nHeight);
	GetEngineObj()->SkinWindow(this);

	::SetWindowRgn(GetHWND(), NULL, FALSE);
	
	SetEnableDragDrop(m_bEnableDragDrop);

	m_hDcPaint = ::GetDC(m_hWnd); 
	m_pDibObj->Create(m_hDcPaint, nWidth, nHeight); // 注如果创建了，估计就重复了， 不创建layer窗口有问题，这里可以优化
	m_bUnFirstPaint = true;
	m_bUpdateNeeded = true;

	OnSetWindowRgn();

	// 加载控件
	if (pTempObj)
	{
		GetResObj()->TemplateObjToControl(GetRoot(), pTempObj);
		if (!pTempObj->IsStore())
		{
			GetResObj()->DelWindowTemplateObj(lpszId);
		}
	}
	// 设置背景颜色和图片
	SetBkColor(GetEngineObj()->GetSkinColor());
	SetBkImage(GetEngineObj()->GetSkinImage());
	// 设置标题
	SetTitle(m_strTitle.c_str());
	// 设置透明度
	SetAlpha(GetEngineObj()->GetAlpha());
	// 设置图标
	if (!m_strBigIcon.empty()) SetIcon(m_strBigIcon.c_str(), true);
	if (!m_strSmallIcon.empty()) 
	{
		SetIcon(m_strSmallIcon.c_str());
	}
	else
	{
		CControlUI* pControl = GetItem(_T("icon"));
		if (pControl)
		{
			ImageStyle* pImageStyle = pControl->GetImageStyle(_T("bk"));
			if (pImageStyle)
				SetIcon(pImageStyle->GetImageFile());
		}
		pControl = GetItem(_T("windowbigicon"));
		if (pControl)
		{
			ImageStyle* pImageStyle = pControl->GetImageStyle(_T("bk"));
			if (pImageStyle)
				SetIcon(pImageStyle->GetImageFile(), true);
		}
	}

	
	GetRoot()->Resize();
	OnCreate();

	return hWnd;
}

void CWindowUI::SkinWindow(HWND  hWnd, LPCTSTR lpszId,  int x, int y, int nWidth, int nHeight)
{
	// 获取窗口属性
	TemplateObj* pTempObj =GetResObj()->GetWindowTemplateObj(lpszId);
	if (pTempObj)
	{
		// 如果有继承基类，先读取基类属性

		MAPAttr* mapAttr = pTempObj->GetAttr();
		if (mapAttr)
		{
			for (MAPAttr::iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
				SetAttribute(pos->first.c_str(), pos->second.c_str());
		}
	}

	// 创建窗口
	nWidth = (nWidth == 0 ? m_iOrgWidth:nWidth);
	nHeight = (nHeight == 0 ? m_iOrgHeight:nHeight);

	nWidth = (nWidth == 0 ? m_iMinWidth:nWidth);
	nHeight = (nHeight == 0 ? m_iMinHeight:nHeight);

	//add by hanzp
	nWidth = (nWidth == CW_USEDEFAULT) ? 0 : nWidth;
	nHeight = (nHeight == CW_USEDEFAULT) ? 0 : nHeight;
	EnableResize(m_bEnableResize);

	m_hWnd = hWnd;
	SetWindowPos(NULL, x, y, nWidth, nHeight, SWP_NOACTIVATE|SWP_NOZORDER);
	SubclassWindow(m_hWnd);

	SetWndStyle(GWL_STYLE, GetWndStyle(GWL_STYLE));
	SetWndStyle(GWL_EXSTYLE, GetWndStyle(GWL_EXSTYLE));
	
	::SetWindowRgn(GetHWND(), NULL, FALSE);

	SetEnableDragDrop(m_bEnableDragDrop);

	m_hDcPaint = ::GetDC(m_hWnd); 
	m_pDibObj->Create(m_hDcPaint, nWidth, nHeight); // 注如果创建了，估计就重复了， 不创建layer窗口有问题，这里可以优化
	m_bUnFirstPaint = true;
	m_bUpdateNeeded = true;
	OnSetWindowRgn();

	// 加载控件
	if (pTempObj)
	{
		GetResObj()->TemplateObjToControl(GetRoot(), pTempObj);
		if (!pTempObj->IsStore())
		{
			GetResObj()->DelWindowTemplateObj(lpszId);
		}
	}

	//改变背景颜色和图片
	SetBkColor(GetEngineObj()->GetSkinColor());
	SetBkImage(GetEngineObj()->GetSkinImage());
	GetEngineObj()->SkinWindow(this);


	//////////////////////////////
	SetAlpha(GetEngineObj()->GetAlpha());
	if (!m_strBigIcon.empty()) SetIcon(m_strBigIcon.c_str(), true);
	if (!m_strSmallIcon.empty()) 
	{
		SetIcon(m_strSmallIcon.c_str());
	}
	else
	{
		CControlUI* pControl = GetItem(_T("icon"));
		if (pControl)
		{
			ImageStyle* pImageStyle = pControl->GetImageStyle(_T("bk"));
			if (pImageStyle)
				SetIcon(pImageStyle->GetImageFile());
		}
		pControl = GetItem(_T("windowbigicon"));
		if (pControl)
		{
			ImageStyle* pImageStyle = pControl->GetImageStyle(_T("bk"));
			if (pImageStyle)
				SetIcon(pImageStyle->GetImageFile(), true);
		}
	}

	if (!m_strTitle.empty())
		SetTitle(m_strTitle.c_str());

	GetRoot()->Resize();
	OnCreate();

}

bool CWindowUI::SendNotify(CControlUI* pControl, int nType, WPARAM wParam, LPARAM lParam, bool bAsyn)
{
	if (bAsyn)
	{
		TNotifyUI* pNotify = new TNotifyUI;
		pNotify->pSender = pControl;
		pNotify->nType = nType;
		pNotify->wParam = wParam;
		pNotify->lParam = lParam;
		pNotify->ptMouse = m_ptLastMousePos;
		if (!PostMessage(GetEngineObj()->GetMsgHwnd(), WM_USER + 100/*WM_ASNYC_NOTIFY*/, (WPARAM)pNotify, (LPARAM)GetHWND()))
		{
			delete pNotify;
			return false;
		}
		return true;
	}
	else if (GetCurrentThreadId() != GetEngineObj()->GetMainThreadId())
	{
		TNotifyUI* pNotify = new TNotifyUI;
		pNotify->pSender = pControl;
		pNotify->nType = nType;
		pNotify->wParam = wParam;
		pNotify->lParam = lParam;
		pNotify->ptMouse = m_ptLastMousePos;
		SendMessage(GetEngineObj()->GetMsgHwnd(), WM_USER + 100/*WM_ASNYC_NOTIFY*/, (WPARAM)pNotify, (LPARAM)GetHWND());
		return true;
	}

	TNotifyUI notify;
	notify.pSender = pControl;
	notify.nType = nType;
	notify.wParam = wParam;
	notify.lParam = lParam;
	notify.ptMouse = m_ptLastMousePos;



	INotifyUI* pPreNotify = GetEngineObj()->GetPreNotify();
	if (pPreNotify)
		pPreNotify->ProcessNotify(&notify);
	//让发送者先做出反应

	if (notify.pSender->Notify(&notify))
		return true;

	if (pControl->GetINotifyUI())
	{
		if (pControl->GetINotifyUI()->ProcessNotify(&notify)) return true;
	}

	//让外面的程序做出反应
	if (nType == UINOTIFY_CLICK)
	{
		if (equal_icmp(pControl->GetId(), _T("close")))
		{
			if (IsModal())
			{
				EndDialog(IDCANCEL);
			}
			else
			{
				CloseWindow(IDCANCEL);
			}
			return true;

		}
		else if (equal_icmp(pControl->GetId(), _T("minimize")))
		{
			MinimizeWindow();
			return true;

		}
		else if (equal_icmp(pControl->GetId(), _T("maximize")))
		{
			if (IsMaximized())
				RestoreWindow();
			else
				MaximizeWindow();
			return true;
		}
	}

	if (!ProcessNotify(&notify))
	{
		int nCnt = m_aNotifiers.GetSize();
		for( int i = 0; i < nCnt; i++ ) 
		{
			RefCountedThreadSafe<INotifyUI>* pNotifySafe = static_cast<RefCountedThreadSafe<INotifyUI>*>(m_aNotifiers.GetAt(i));
			if( pNotifySafe && pNotifySafe->IsValid()) {
				INotifyUI* pNotifyFunc = pNotifySafe->GetCountedOwner();
				if (pNotifyFunc && pNotifyFunc->ProcessNotify(&notify))
					return true;
			}
		}
		this->OnControlNotify(&notify);
	}
	return true;
}

void CWindowUI::OnControlNotify(TNotifyUI* pNotify)
{
}

void CWindowUI::OnCreate()
{
}


void CWindowUI::OnCancel()
{
	CloseWindow(IDCANCEL);
}

void CWindowUI::OnOk()
{

}

void CWindowUI::OnClose()
{

}

bool CWindowUI::OnClosing()
{
	return true;
}

void CWindowUI::OnMinimize()
{

}

void CWindowUI::OnMaximize()
{

}

void CWindowUI::OnRestored()
{

}

bool CWindowUI::IsMaximized()
{
	return m_bMaximized;
	
}
bool CWindowUI::IsMinimized()
{
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	if (dwStyle &  WS_MINIMIZE)
		return true;
	return false;

}

void SetWindowEllispeFrame(HWND hwnd, int nWidthEllipse, int nHeightEllipse)
{
	HRGN hRgn;
	RECT rect;
	int nHeight,nWidth;

	GetWindowRect(hwnd, &rect);
	nHeight = rect.bottom - rect.top;    // 策画高度
	nWidth = rect.right - rect.left;        // 策画宽度

	POINT point[8] = {
		{0, nHeightEllipse},              // left-left-top
		{nWidthEllipse, 0},                 // left-top-left
		{nWidth - nWidthEllipse, 0},
		{nWidth, nHeightEllipse},        // right-top
		{nWidth, nHeight - nHeightEllipse}, // right-bottom-right
		{nWidth - nWidthEllipse, nHeight},  // right-bottom-bottom
		{nWidthEllipse, nHeight},              // left-bottom
		{0, nHeight - nHeightEllipse},
	};

	hRgn = CreatePolygonRgn(point, 8, WINDING);
	if (hRgn)
	{
		SetWindowRgn(hwnd,hRgn,TRUE);
		::DeleteObject(hRgn);
	}
}


void CWindowUI::OnSetWindowRgn()
{
	if (!IsLayerWindow())
		SetWindowEllispeFrame(m_hWnd, 2, 2);
}

void CWindowUI::OnLanguageChange()
{
	if (m_lpszTitleKey)
		SetAttribute(_T("title"), m_lpszTitleKey);
}

void CWindowUI::OnSkinChange(bool bAnimation)
{
	if (!bAnimation)
	{
		SetBkImage(GetEngineObj()->GetSkinImage());
		SetBkColor(GetEngineObj()->GetSkinColor());
		Invalidate();
	}
	else
	{
		CChangeSkinAnimation* pAnimation = new CChangeSkinAnimation(this);
		pAnimation->Start();
	}
}

void CWindowUI::SetBorder(RECT& rect)
{
	m_rcBorder = rect;
}

void CWindowUI::SetEnableDragDrop(bool bDragDrop)
{
	m_bEnableDragDrop = bDragDrop;
	if (!IsWindow(GetHWND())) return;
	if (bDragDrop)
	{
		if (!m_pDragTarget)
			m_pDragTarget = new CDropTarget(this);
	}
	else if (m_pDragTarget)
	{
		m_pDragTarget->Release();
		m_pDragTarget = NULL;
	}
}



void CWindowUI::EnableIME()
{
	if (m_hWnd && IsWindow(m_hWnd))
	{
		if (m_hImc)
			ImmAssociateContext(m_hWnd, m_hImc);
		m_hImc = NULL;
	}
}
void CWindowUI::DisableIME()
{
	if (m_hWnd && IsWindow(m_hWnd))
	{
		HIMC hImc = ImmGetContext(m_hWnd);
		if (hImc)
		{
			m_hImc = hImc;
			ImmAssociateContext(m_hWnd, NULL);
		}
		ImmReleaseContext(m_hWnd, hImc);
	}
}


HRESULT  CWindowUI::DoDrag(CControlUI* pControl, IN LPDATAOBJECT pDataObj,  IN DWORD dwOKEffects, OUT LPDWORD pdwEffect)
{
	CDropSource* pdsrc = new CDropSource(pDataObj, pControl);
	m_pEventDrag = pControl;
	HRESULT hr = ::DoDragDrop(pDataObj, pdsrc, dwOKEffects, pdwEffect);
	m_pEventDrag = NULL;
	pdsrc->Release();
	return hr;
}

void CWindowUI::SetMsgHandled(bool bMsgHanled)
{
	m_bMsgHandled =  bMsgHanled;
}

HDC CWindowUI::GetPaintDC()
{
	return m_hDcPaint;
}

CControlUI* CWindowUI::GetEventHoverControl()
{
	return m_pEventHover;
}

bool CWindowUI::IsWindowRender()
{
	return !m_bUnFirstPaint;
}

void CWindowUI::SetShadow(bool bShadow)
{
	m_bShadow = bShadow;
	if (!bShadow)
	{
		if (m_pShadowWindow)
			m_pShadowWindow->CloseWindow();
		m_pShadowWindow = NULL;
		
	}
	else
	{
		if (!m_pShadowWindow)
		{
			if (IsWindow(GetHWND()))
			{
				RECT rcShdowEdge = GetResObj()->GetWindowShadowEdgeRect();
				m_pShadowWindow = new CNoActiveWindowUI();
				m_pShadowWindow->SetShadow(false);
				m_pShadowWindow->SetLayerWindow(true);
				m_pShadowWindow->ModifyWndStyle(GWL_EXSTYLE, 0,  WS_EX_TRANSPARENT |WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
				RECT rect;
				GetWindowRect(GetHWND(), &rect);
				rect.left -= rcShdowEdge.left;
				rect.top -= rcShdowEdge.top;
				rect.right += rcShdowEdge.right;
				rect.bottom += rcShdowEdge.bottom;

				m_pShadowWindow->Create(GetHWND(), _T("back_shadow_window"), rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
				GetEngineObj()->UnSkinWindow(m_pShadowWindow);

				//防止个别机器阴影问题，最大化和最小化时隐藏shadow
				DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
				if (dwStyle & WS_MAXIMIZE || dwStyle & WS_MINIMIZE)
				{
					m_pShadowWindow->ShowWindow(SW_HIDE);
					m_pShadowWindow->SetLayerWindow(true);
				}
				else
				{
					m_pShadowWindow->SetLayerWindow(true);
					this->MoveShadowWindow();
				}
			}
		}
	}
}

void CWindowUI::OnWindowInit()
{

}

void CWindowUI::MoveShadowWindow()
{
	if (!m_pShadowWindow) return;
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	if (dwStyle & WS_MAXIMIZE || dwStyle & WS_MINIMIZE)
	{
		::ShowWindow(m_pShadowWindow->GetHWND(), SW_HIDE);
	}
	else
	{
		::ShowWindow(m_pShadowWindow->GetHWND(), SW_SHOWNOACTIVATE);
	}

	RECT rect;
	GetWindowRect(GetHWND(), &rect);
	RECT rcShdowEdge = GetResObj()->GetWindowShadowEdgeRect();
	rect.left -= rcShdowEdge.left;
	rect.top -= rcShdowEdge.top;
	rect.right += rcShdowEdge.right;
	rect.bottom += rcShdowEdge.bottom;


	m_pShadowWindow->UpdateLayeredWindow(&rect);
}

DibObj* CWindowUI::GetDibObj()
{
	return m_pDibObj;
}

void CWindowUI::ShowToolTip(LPCTSTR lpszText, LPRECT lpRect)
{
	if(!GetToolTipWnd())
	{
		m_pTipWindow = new TipWindow;
		m_pTipWindow->CreateTip(GetHWND());
		m_hTipWindow = m_pTipWindow->GetHWND();
	}
	
	m_pTipWindow->ShowTip(true, lpszText, lpRect);

}

void CWindowUI::SetVKTabEnable(bool bEnable)
{
    m_bVKTabEnable = bEnable;
}

bool CWindowUI::IsVKTabEnable()
{
    return m_bVKTabEnable;
}

void CWindowUI::SetDefaultFontType(UITYPE_FONT eFontType)
{
	m_eFontType = eFontType;
}

UITYPE_FONT CWindowUI::GetDefaultFontType()
{
	return m_eFontType;
}

void CWindowUI::SetDefaultTextRenderingHint(int textRenderingHint)
{
	m_nTextRenderingHint = textRenderingHint;
}

int CWindowUI::GetDefaultTextRenderingHint()
{
	return m_nTextRenderingHint;
}


void CWindowUI::SetMinWidth(int nMinWidth)
{
	m_iMinWidth = nMinWidth;
}
int CWindowUI::GetMinWidth()
{
	return m_iMinWidth;
}
void CWindowUI::SetMinHeight(int nMinHeight)
{
	m_iMinHeight = nMinHeight;
}
int CWindowUI::GetMinHeight()
{
	return m_iMinHeight;
}

void CWindowUI::SetMaxWidth(int nMaxWidth)
{
	m_iMaxWidth = nMaxWidth;
}

int CWindowUI::GetMaxWidth()
{
	return m_iMaxWidth;
}

void CWindowUI::SetMaxHeight(int nMaxHeight)
{
	m_iMaxHeight = nMaxHeight;
}

int CWindowUI::GetMaxHeight()
{
	return m_iMaxHeight;
}

TipWindow* CWindowUI::GetToolTipWnd()
{
	//access to m_pToolWindow directly is not safe.
	return (TipWindow*)FromHandle(m_hTipWindow);
}

//////////////////////////////////////////////////////////////////////////
CNoActiveWindowUI* CNoActiveWindowUI::ms_pModalWindow = NULL;
CNoActiveWindowUI::CNoActiveWindowUI()
{
	EnableMove(false);
	SetWndStyle(GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
}

CNoActiveWindowUI::~CNoActiveWindowUI()
{
	if (this == ms_pModalWindow)
		ms_pModalWindow = NULL;
}


LRESULT CNoActiveWindowUI::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case WM_ACTIVATE:
		return S_OK;
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	case WM_SYSKEYDOWN:
		if (wParam == VK_F4)
		{
			return FALSE;
		}
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}