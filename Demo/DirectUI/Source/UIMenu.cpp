/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UIMenu.h"

#define _Style_MenuItem_Bk 0
#define _Style_MenuItem_Check 1
#define _Style_MenuItem_Txt 2
#define _Style_MenuItem_Key 3
#define _Style_MenuItem_Arrow 4

UINT GetItemIDFromStr(LPCTSTR lpszText);
//////////////////////////////////////////////////////////////////////////
class CMenuWindowUI : public CNoActiveWindowUI, public IPreMessageFilterUI
{
public:
	CMenuWindowUI();
	virtual ~CMenuWindowUI();

	void SetPopupType( int iPopupType ){ m_iPopupType = iPopupType;}
	int GetPopupType( ){return m_iPopupType;}

	void SetINotify(INotifyUI* pNotify){
		if (m_pINotifySafe)
			m_pINotifySafe->Release();
		m_pINotifySafe = NULL;
		if (pNotify)
		{
			m_pINotifySafe = pNotify->GetINotifySafe();
			m_pINotifySafe->AddRef();
		}
	}
	RefCountedThreadSafe<INotifyUI>* GetINotify(){return m_pINotifySafe;}
	
	static CMenuWindowUI* ms_pMenuWindowInstance;
protected:
	virtual void SetFocusOnWnd(){};
	virtual void OnCreate();
	virtual void OnClose();
	virtual bool PreMessageFilter(const LPMSG pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	int m_iPopupType;
	RefCountedThreadSafe<INotifyUI>* m_pINotifySafe;
};

CMenuWindowUI* CMenuWindowUI::ms_pMenuWindowInstance = NULL;
//////////////////////////////////////////////////////////////////////////
CMenuWindowUI::CMenuWindowUI()
:m_pINotifySafe(NULL)
,m_iPopupType(0)
{
	RECT rcBorder = {0, 0, 0, 0};
	SetBorder(rcBorder);
	SetWndStyle(GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_CONTROLPARENT);

	SetLayerWindow(true);
	SetWindowClassName(_T("CMenuWindowUI"));
}

CMenuWindowUI::~CMenuWindowUI()
{
	GetEngineObj()->RemovePreMessageFilter(this);
	if (m_pINotifySafe)
		m_pINotifySafe->Release();
	m_pINotifySafe = NULL;
}

void CMenuWindowUI::OnCreate()
{
	HWND hParent = ::GetParent(GetHWND());
	// 判断是否是跟菜单
	if (hParent)
	{
		TCHAR ClassName[MAX_PATH];
		GetClassName(hParent, ClassName, sizeof(ClassName)/sizeof(TCHAR) - 1);
		if(!equal_icmp(ClassName,_T("CMenuWindowUI")))
			GetEngineObj()->AddPreMessageFilter(this);
	}
	
}

void CMenuWindowUI::OnClose()
{
	GetEngineObj()->RemovePreMessageFilter(this);
}

LRESULT CMenuWindowUI::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATEAPP:
		{
			if (!(BOOL)wParam)
			{
				CMenuUI::ReleaseInstance();
			}
		}
		break;
	case WM_KILLFOCUS:
		{
 			CMenuUI::ReleaseInstance();
		}
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}

bool CMenuWindowUI::PreMessageFilter(const LPMSG pMsg)
{
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_SETFOCUS:
		{
			if (pMsg->hwnd != GetHWND())
			{
				bool bChildMenuActive = false;
				HWND hParent = GetParent(pMsg->hwnd);
				while (hParent)
				{
					if (hParent == GetHWND())
					{
						bChildMenuActive = true;
						break;
					}
					hParent = GetParent(hParent);
				}
				if (!bChildMenuActive)
				{
					CMenuUI::ReleaseInstance();
				}
			}
		}
		break;
	}
	return false;
}


CMenuUI::CMenuUI()
{
	m_nSelectItem = -1;
	m_szItem.cx = 120;
	m_szItem.cy = 20;
	m_nBaseItemWidth = 50;
	m_szMenuWindow.cx = 0;
	m_szMenuWindow.cy = 0;
	m_pMenuWindow = NULL;
	m_pMenuInfo = new UIMENUINFO;
	m_strBackGround = _T("");
	m_bShowActive = false;	
	RECT rcInset = {MENU_INSET, MENU_INSET, MENU_INSET, MENU_INSET};
	SetInset(rcInset);
	SetEnabled(true);
	SetRectEmpty(&m_rcMenuItem);
	SetMouseEnabled(true);
	SetStyle(_T("MenuItem"));

	ModifyFlags(UICONTROLFLAG_SETFOCUS | UICONTROLFLAG_SETCURSOR, 0);
}
CMenuUI::~CMenuUI()
{
	m_pMenuWindow = NULL;

	//bool bDel = true;
	CMenuUI* pSub = NULL;
	for( int i = 0;  i < GetCount(); i++ )
	{
		bool bDel = true;
		CMenuUI* pControl = static_cast<CMenuUI*>(m_MenuItems[i]);
		if (m_pMenuInfo)
		{
			int nCnt = m_pMenuInfo->MppSubWindow.GetSize();
			for (int j = 0; j < nCnt; j++)
			{
				pSub = static_cast<CMenuUI*>(m_pMenuInfo->MppSubWindow.GetAt(j));
				if (pSub && pSub == pControl)
				{
					bDel = false;
					m_pMenuInfo->MppSubWindow.Remove(j);
					break;
				}
			}
		}
		if (bDel)
			delete pControl;
	}
	m_MenuItems.Empty();

	if (m_pMenuInfo)
		delete m_pMenuInfo;
	m_pMenuInfo = NULL;

	UICustomControlHelper::RemoveAllChild(m_SubControls);
}


void CMenuUI::ReleaseInstance()
{
	CMenuWindowUI * pWindow = CMenuWindowUI::ms_pMenuWindowInstance;
	if (pWindow)
	{
		CMenuWindowUI::ms_pMenuWindowInstance = NULL;
		pWindow->ShowWindow(SW_HIDE);
		pWindow->CloseWindow();	
	}
}

CMenuUI* CMenuUI::Load(UINT uMenuID)
{
	CMenuUI* pMenu = new CMenuUI;
	bool bRet = GetResObj()->AttachMenu(pMenu, uMenuID);
	if (bRet)
		return pMenu;
	return NULL;
}

HWND CMenuUI::Show( HWND hWndParent, LPPOINT lpPoint, INotifyUI* pNotify )
{
	POINT pt;
	if( NULL != lpPoint )
	{
		pt.x = lpPoint->x;
		pt.y = lpPoint->y;
	}
	else
		GetCursorPos(&pt);
	return ShowEx(hWndParent, lpPoint, pNotify, UIMENU_NOHANDLE);
}

HWND CMenuUI::ShowEx( HWND hWndParent, LPPOINT lpPoint, INotifyUI* pNotify, UINT uFlags )
{
	POINT pt;
	if( NULL != lpPoint )
	{
		pt.x = lpPoint->x;
		pt.y = lpPoint->y;
	}
	else
		GetCursorPos(&pt);
	return Popup(hWndParent, pt.x, pt.y, pNotify, uFlags);
}

HWND CMenuUI::Popup( HWND hParent, int x /*= -1*/, int y /*= -1*/, INotifyUI* pNotify /*= NULL*/, UINT uFlags /*= UIMENU_NOHANDLE*/ )
{
	bool bRootMenu = false;
	//bool bWndParentActive = false;
	TCHAR ClassName[MAX_PATH];
	::GetClassName(hParent, ClassName, sizeof(ClassName)/sizeof(TCHAR) - 1);
	if(!equal_icmp(ClassName,_T("CMenuWindowUI")))
		bRootMenu = true;

	if (bRootMenu)
	{
		CMenuUI::ReleaseInstance();
		//if (GetActiveWindow() == hParent)
		//	bWndParentActive = true;
	}

	CMenuWindowUI* pWindow = new CMenuWindowUI;
	pWindow->Create(hParent, _T("menu"));
	if (!m_strBackGround.empty())
	{
		CControlUI* pRoot = pWindow->GetRoot();
		if (pRoot)
		{
			CControlUI* pBk = pRoot->GetItem(_T("bk"));
			if (pBk)
				pBk->SetAttribute(_T("bk.image"), m_strBackGround.c_str());
		}
	}
	this->SetAttribute(_T("id"),_T("defaultMenuRoot"));
	pWindow->GetRoot()->Add(this);
	this->SetMenuWindow(pWindow);
	this->Init();

	if (bRootMenu)
	{
		CMenuWindowUI::ms_pMenuWindowInstance = pWindow;
		pWindow->SetINotify(pNotify);
	}
	else
	{ 
		CMenuWindowUI * pParentWindow = NULL;
		pParentWindow = dynamic_cast<CMenuWindowUI*>(CWindowBase::FromHandle(hParent));
		if (pParentWindow)
		{
			CMenuUI* pParentMenu = dynamic_cast<CMenuUI*>(pParentWindow->GetItem(_T("defaultMenuRoot")));
			//CMenuUI* pSub = NULL;
			if (pParentMenu->m_pMenuInfo)
			{
				bool bInit = false;
				int nCnt = pParentMenu->m_pMenuInfo->MppSubWindow.GetSize();
				for (int i = 0; i < nCnt; i++)
				{
					CMenuUI* pSub = static_cast<CMenuUI*>(pParentMenu->m_pMenuInfo->MppSubWindow.GetAt(i));
					if (pSub && pSub == this)
					{
						bInit = true;
						break;
					}
				}
				if (!bInit)
					pParentMenu->m_pMenuInfo->MppSubWindow.Add(this);
			}
		}
	}


	SIZE sz = this->GetWindowSize();

	if (x == -1 && y == -1)
	{
		uFlags = UIMENU_NOHANDLE;
		RECT rcWnd;
		int nInset = 8;
		::GetWindowRect(hParent, &rcWnd);
		RECT rcItem = this->GetMenuItemRect();
		RECT rcMenuR = {rcWnd.right - nInset, rcWnd.top + rcItem.top - nInset, rcWnd.right + sz.cx - nInset, rcWnd.top + rcItem.top + sz.cy - nInset};
		RECT rcMenuL = {rcWnd.left - sz.cx + nInset, rcWnd.top + rcItem.top - nInset, rcWnd.left + nInset, rcWnd.top + rcItem.top + sz.cy - nInset};

		if (GetUIEngine()->IsRTLLayout())
		{
			if (__IsRectIntersectScreen(rcMenuL))
			{
				x = rcMenuL.left;
				y = rcMenuL.top;
			}
			else
			{
				x = rcMenuR.left;
				y = rcMenuR.top;
			}

		}
		else
		{
			if (__IsRectIntersectScreenLR(rcMenuR))
			{
				x = rcMenuR.left;
				y = rcMenuR.top;
			}
			else
			{
				x = rcMenuL.left;
				y = rcMenuL.top;
			}
		}
	}

	//int xoff = 0;
	RECT rcTemp = { 0 };
	RECT rcMenuRB = {x, y, x + sz.cx, y + sz.cy};
	RECT rcMenuLB = {x - sz.cx, y, x, y + sz.cy};
	RECT rcMenuRT = {x, y - sz.cy, x + sz.cx, y};
	RECT rcMenuLT = {x - sz.cx, y - sz.cy, x, y};
	if (!bRootMenu)
	{
		//BUG #64280 修复  *2的话偏移量太大
		OffsetRect(&rcMenuRT, 0, DPI_SCALE(m_szItem.cy));
		OffsetRect(&rcMenuLT, 0, DPI_SCALE(m_szItem.cy));
	}

	RECT rcDest;
	if (uFlags == UIMENU_NOHANDLE) 
	{

		if (GetUIEngine()->IsRTLLayout())
		{
			if (bRootMenu)
			{
				if( __IsRectIntersectScreen(rcMenuLB) )
					rcDest = rcMenuLB;
				else if ( __IsRectIntersectScreen(rcMenuLT) )
					rcDest = rcMenuLT;
				else if ( __IsRectIntersectScreen(rcMenuRB) )
					rcDest = rcMenuRB;
				else if ( __IsRectIntersectScreen(rcMenuRT) )
					rcDest = rcMenuRT;
				else
					rcDest = rcMenuLB;

			}
			else
			{
				if( __IsRectIntersectScreen(rcMenuRB) )
					rcDest = rcMenuRB;
				else if ( __IsRectIntersectScreen(rcMenuLB) )
					rcDest = rcMenuLB;
				else if ( __IsRectIntersectScreen(rcMenuRT) )
					rcDest = rcMenuRT;
				else if ( __IsRectIntersectScreen(rcMenuLT) )
					rcDest = rcMenuLT;
				else
					rcDest = rcMenuRB;

			}

		}
		else
		{
			if( __IsRectIntersectScreen(rcMenuRB) )
				rcDest = rcMenuRB;
			else if ( __IsRectIntersectScreen(rcMenuLB) )
				rcDest = rcMenuLB;
			else if ( __IsRectIntersectScreen(rcMenuRT) )
				rcDest = rcMenuRT;
			else if ( __IsRectIntersectScreen(rcMenuLT) )
				rcDest = rcMenuLT;
			else
				rcDest = rcMenuRB;

		}

		pWindow->SetWindowPos(HWND_TOPMOST, rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top, SWP_NOACTIVATE);
	
	}
	else
	{
		rcDest = rcMenuRB;
		if (uFlags & UIMENU_CENTERALIGN)
		{
			::OffsetRect(&rcDest, -sz.cx/2, 0);
		}
		else if (uFlags & UIMENU_LEFTALIGN)
		{
			
		}
		else if (uFlags & UIMENU_RIGHTALIGN)
		{
			::OffsetRect(&rcDest, -sz.cx, 0);
		}

		if (uFlags & UIMENU_VCENTERALIGN)
		{
			::OffsetRect(&rcDest, 0, sz.cy/2);
		}
		else if (uFlags & UIMENU_TOPALIGN)
		{
			::OffsetRect(&rcDest, 0, -sz.cy);
		}
		else if (uFlags & UIMENU_BOTTOMALIGN)
		{
			
		}
		pWindow->SetWindowPos(HWND_TOPMOST, rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top, SWP_NOACTIVATE);
	}
	
	HWND hMenuWindow = pWindow->GetHWND();
	if (!IsWindow(hMenuWindow))
		return hMenuWindow;


	/// 用于修正另外一个进程窗体作为本进程子窗体时，
	/// 其弹出菜单，点击本进程窗体无法将菜单释放的问题
	if (m_bShowActive && bRootMenu)
	{
 		pWindow->ShowWindow(SW_SHOW);
	}
	else
	{
		pWindow->ShowWindow(SW_SHOWNOACTIVATE);
	}
	

	return hMenuWindow;
}

bool CMenuUI::Insert(UINT uid, /*const wchar_t**/LPCTSTR lpszText, UINT uFlag/* = BY_LAST*/, UINT nIndex/* = -1*/)
{
	CMenuUI* pMenu = new CMenuUI;
	pMenu->SetUID(uid);
	pMenu->SetText(lpszText);
	return this->Insert(pMenu, uFlag, nIndex);
}

bool CMenuUI::Insert(CMenuUI* pMenu, UINT uFlag/* = BY_LAST*/, UINT nIndex/* = -1*/)
{
	switch(uFlag)
	{
	case BY_COMMAND:
		{
			CMenuUI* tempMenu = NULL;
			for (int i = 0; i < GetCount(); i++)
			{
				tempMenu = GetItem(i);
				if (tempMenu->GetUID() == nIndex)
					tempMenu->InsertAt(GetIndex(nIndex), pMenu);
			}
			for (int i = 0; i < GetCount(); i++)
			{
				tempMenu = GetItem(i);
				if (tempMenu->GetCount() > 0 && tempMenu->Insert(pMenu, uFlag, nIndex))
					return true;
			}
			return false;
		}
		break;
	case BY_POSITION:
		this->InsertAt(nIndex,pMenu);
		break;
	case BY_FIRST:
		this->InsertAt(0,pMenu);
		break;
	case BY_LAST:
		this->InsertAt(GetCount(),pMenu);
		break;
	default:
		this->InsertAt(GetCount(),pMenu);
		break;
	}
	return true;
}
bool CMenuUI::InsertAt(int iIndex, CMenuUI* pMenu)
{
	return m_MenuItems.InsertAt(iIndex, pMenu);
}
bool CMenuUI::InsertSep(UINT nIndex/* = -1*/,UINT uFlag/* = BY_LAST*/)
{
	CMenuUI* pMenu = new CMenuUI;
	pMenu->Separator();
	pMenu->SetUID(1);
	return Insert(pMenu, uFlag, nIndex);
}
bool CMenuUI::Add(CControlUI* pControl)
{
	if(pControl->IsClass(_T("MenuItem")))
	{
		Insert(static_cast<CMenuUI*>(pControl));
	}
	else
	{
		UICustomControlHelper::AddChildControl(pControl, this, m_SubControls);
	}
	return true;
}
void CMenuUI::DrawItem(IRenderDC* pRenderDC, RECT& rcPaint)
{
	ImageStyle* pStyle = NULL;
	if (IsSeparator())
	{
		pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_MenuItem_Bk));
		pStyle->SetState(UISTATE_OVER);
		pStyle->Render(pRenderDC, rcPaint);
	}
	else
	{
		TextStyle* pText = NULL;
		COLORREF clr = -1;
		if (IsEnableItem())
		{
			pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_MenuItem_Bk));
			if (pStyle) 
			{
				pStyle->SetState((UITYPE_STATE)IsSelected());
				pStyle->Render(pRenderDC, rcPaint);
			}
		}
		else
		{
			clr = DUI_RGB2ARGB(GetSysColor(COLOR_GRAYTEXT));
		}

		pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_MenuItem_Check));
		if (pStyle) 
		{
			pStyle->SetState((UITYPE_STATE)IsSelected());
			pStyle->Render(pRenderDC, rcPaint);
		}

		pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(_Style_MenuItem_Arrow));
		if (pStyle) 
		{
			pStyle->SetState((UITYPE_STATE)(GetCount()>0));
			pStyle->Render(pRenderDC, rcPaint);
		}

		pText = dynamic_cast<TextStyle*>((StyleObj*)GetStyle(_Style_MenuItem_Txt));
		if (pText) 
		{
			if (clr != -1)
				pText->SetTextColor(clr);
			pText->SetState(GetState());
			pText->Render(pRenderDC,rcPaint);
		}

		if (m_pMenuInfo->uKey != 0)
		{
			pText = dynamic_cast<TextStyle*>((StyleObj*)GetStyle(_Style_MenuItem_Key));
			if (pText) 
			{
				if (_tcsclen(pText->GetText()) == 0)
					pText->SetText(m_pMenuInfo->szKey.c_str());
				if (clr != -1)
					pText->SetTextColor(clr);
				pText->SetState(GetState());
				pText->Render(pRenderDC,rcPaint);
			}
		}

		RECT rcMenu = GetMenuItemRect();
		UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcMenu, rcMenu, m_SubControls, 0, -1);
	}
}
bool CMenuUI::SelectItem(int nIndex)
{
	if (nIndex >= GetCount())
		nIndex = GetCount() - 1;
	else if (nIndex <= -1)
		nIndex = -1;
	if (m_nSelectItem == -1 && nIndex != -1)
	{
		static_cast<CMenuUI*>(m_MenuItems[nIndex])->Select(true);
		m_nSelectItem = nIndex;
		return true;
	}
	else if (nIndex == -1 && m_nSelectItem != -1)
	{
		static_cast<CMenuUI*>(m_MenuItems[m_nSelectItem])->Select(false);
		m_nSelectItem = nIndex;
		return true;
	}
	if (m_nSelectItem != nIndex)
	{
		static_cast<CMenuUI*>(m_MenuItems[m_nSelectItem])->Select(false);
		static_cast<CMenuUI*>(m_MenuItems[nIndex])->Select(true);
		m_nSelectItem = nIndex;
		return true;
	}
	return false;
}

CMenuUI g_controlForMenu;
bool CMenuUI::Event(TEventUI& event)
{
	bool bExpand = false;
	bool bClick = false;
	tstring strSubID = _T("");
	switch(event.nType)
	{
	case UIEVENT_BUTTONUP:
		{
			bClick = true;

			int i = HitItem(event.ptMouse);
			CMenuUI* pMenu = GetItem(i);
			if (pMenu)
			{
				int nSubCnt = pMenu->m_SubControls.GetSize();
				CControlUI* pControl = NULL;
				for (int j = 0; j < nSubCnt; j++)
				{
					pControl = static_cast<CControlUI*>(pMenu->m_SubControls.GetAt(j));
					if (pControl)
					{
						if (pControl->IsVisible() && pControl->IsEnabled() && pControl->HitTest(event.ptMouse))
						{
							strSubID = pControl->GetId();
							break;
						}
					}
				}
			}
		}
		break;
	case UIEVENT_MOUSEENTER:
	case UIEVENT_MOUSEMOVE:
		{
			int i = HitItem(event.ptMouse);
			SelectItem(i);
			bExpand = true;
		}
		break;
	case UIEVENT_SETCURSOR:
		{
			int i = HitItem(event.ptMouse);
			CMenuUI* pMenu = GetItem(i);
			if (pMenu)
			{
				HCURSOR hCursor = pMenu->GetCursor();
				int nSubCnt = pMenu->m_SubControls.GetSize();
				CControlUI* pControl = NULL;
				for (int j = 0; j < nSubCnt; j++)
				{
					pControl = static_cast<CControlUI*>(pMenu->m_SubControls.GetAt(j));
					if (pControl)
					{
						if (FindControlTest(pControl,(LPVOID)(&event.ptMouse), UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_HITTEST))
						{
							if ((pControl->GetFlags() & UICONTROLFLAG_SETCURSOR) != 0)
								hCursor = pControl->GetCursor();
						}
					}
				}
				::SetCursor(hCursor);
			}
		}
		break;
	case UIEVENT_MOUSELEAVE:
		{
			SelectItem(-1);
			Invalidate();
		}
		break;
	default:
		return CControlUI::Event(event);
	}
	if (bExpand)
	{
		if (m_nSelectItem != -1)
			UnLoadWindow();

		HWND hwnd = NULL;
		if (m_pMenuWindow)
			hwnd = m_pMenuWindow->GetHWND();
		if (m_nSelectItem != -1)
		{
			CMenuUI* pMenu = GetItem(m_nSelectItem);
			if (pMenu && pMenu->GetCount()>0 && pMenu->IsEnableItem())
			{
				CMenuWindowUI* pWindow = pMenu->GetMenuWindow();
				if (pWindow == NULL)
				{
					pMenu->Popup(m_pMenuWindow->GetHWND());
				}
				else if (!IsWindowVisible(pWindow->GetHWND()))
				{
					pWindow->ShowWindow(SW_SHOWNOACTIVATE);
				}
			}
		}
		if(hwnd && IsWindow(hwnd) && IsWindowVisible(hwnd) && IsWindowEnabled(hwnd))
			Invalidate();
		return true;
	}
	if (bClick)
	{
		if (m_nSelectItem != -1)
		{
			CMenuUI* pMenu = GetItem(m_nSelectItem);
			if (pMenu && pMenu->GetCount() <= 0)
			{
				if (pMenu->IsEnableItem() && !pMenu->IsSeparator())
				{
					CMenuWindowUI* pMenuWindow = CMenuWindowUI::ms_pMenuWindowInstance;
					if (pMenuWindow)
					{
						CMenuUI* pRootMenu = dynamic_cast<CMenuUI*>(pMenuWindow->GetItem(_T("defaultMenuRoot")));
						if (pRootMenu)
						{
							g_controlForMenu.m_pMenuInfo->Copy(*pMenu->m_pMenuInfo);
							g_controlForMenu.SetParam(pRootMenu->GetWParam(), pRootMenu->GetLParam());
							g_controlForMenu.m_pMenuInfo->strSubID = strSubID;

							RefCountedThreadSafe<INotifyUI>* pWindowNotify = pMenuWindow->GetINotify();
							TNotifyUI* pNotify = new TNotifyUI;
							pNotify->pSender = &g_controlForMenu;
							pNotify->nType = UINOTIFY_CLICK;
							pNotify->wParam = (WPARAM)(g_controlForMenu.m_pMenuInfo);
							pNotify->lParam = pRootMenu->GetMenuInfo().uID;

							if (!::PostMessage(GetEngineObj()->GetMsgHwnd(), WM_USER+101/*WM_MENU_NOTIFY*/, (WPARAM)pNotify, (LPARAM)pWindowNotify))
							{
								delete pNotify;
								CMenuUI::ReleaseInstance();
								return false;
							}
							CMenuUI::ReleaseInstance();
						}
					}
				}
			}
		}
		return true;
	}
	return true;
}


void CMenuUI::UnLoadWindow()
{
	int i, nCount = GetCount();
	CMenuUI* pMenu = NULL;
	CMenuWindowUI* pWindow = NULL;
	for (i = 0; i < nCount; i++)
	{
		if (i != m_nSelectItem)
		{
			pMenu = GetItem(i);
			if (pMenu) 
				pWindow = pMenu->GetMenuWindow();
			if (pMenu && pWindow && IsWindowVisible(pWindow->GetHWND()))
			{
				pMenu->UnLoadWindow();
				::ShowWindow(pWindow->GetHWND(), SW_HIDE);
			}
		}
	}
}
void CMenuUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	//CMenuUI* pMenu = NULL;
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
	{
		CMenuUI* pMenu = static_cast<CMenuUI*>(m_MenuItems[i]);
		if (pMenu)	
			pMenu->DrawItem(pRenderDC, rcPaint);
	}
}

int  CMenuUI::HitItem(POINT ptMouse)
{
	RECT rc;
	CMenuUI* pMenu = NULL;
	for (int i = 0; i < GetCount(); i++)
	{
		pMenu = static_cast<CMenuUI*>(m_MenuItems[i]);
		if (pMenu)
			rc = pMenu->GetMenuItemRect();
		if (PtInRect(&rc,ptMouse))
			return i;
	}
	return -1;
}

bool CMenuUI::Remove(int nIndex, int uFlag/* = BY_POSITION*/)
{
	switch(uFlag)
	{
	case BY_POSITION:
		{
			if( nIndex < 0 || nIndex >= GetCount() )
				return false;
			return Remove(static_cast<CMenuUI*>(m_MenuItems[nIndex]));
		}
		break;
	case BY_COMMAND:
		{
			for (int i = 0; i < GetCount(); i++)
			{
				if ( (static_cast<CMenuUI*>(m_MenuItems[i]))->GetUID() == nIndex )
					return Remove(static_cast<CMenuUI*>(m_MenuItems[i]));
			}
			for (int i = 0; i < GetCount(); i++)
			{
				if ((static_cast<CMenuUI*>(m_MenuItems[i]))->GetCount() > 0)
				{
					if ((static_cast<CMenuUI*>(m_MenuItems[i]))->Remove(nIndex, uFlag))
					{
						return true;
					}
				}
			}
			return false;
		}
		break;
	}
	return true;
}

CMenuUI* CMenuUI::GetItem(int nIndex, int uFlag/* = BY_POSITION*/)
{
	switch(uFlag)
	{
	case BY_COMMAND:
		{
			for (int i = 0; i < GetCount(); i++)
			{
				if ( (static_cast<CMenuUI*>(m_MenuItems[i]))->GetUID() == nIndex )
					return static_cast<CMenuUI*>(m_MenuItems[i]);
			}
			for (int i = 0; i < GetCount(); i++)
			{
				if ((static_cast<CMenuUI*>(m_MenuItems[i]))->GetCount() > 0)
				{
					CMenuUI* tempMenu = (static_cast<CMenuUI*>(m_MenuItems[i]))->GetItem(nIndex, uFlag);
					if (tempMenu != NULL)
						return tempMenu;
				}
			}
			return NULL;
		}
		break;
	case BY_POSITION:
		{
			if( nIndex < 0 || nIndex >= GetCount() ) return NULL;
			return static_cast<CMenuUI*>(m_MenuItems[nIndex]);
		}
		break;
	}
	return NULL;
}
int CMenuUI::GetIndex(UINT uID)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if ( static_cast<CMenuUI*>(m_MenuItems[i])->GetUID() == uID )
			return i;
	}
	return -1;
}
UIMENUINFO& CMenuUI::GetMenuInfo()
{
	return *m_pMenuInfo;
}
void CMenuUI::SetUID(UINT uID)
{
	m_pMenuInfo->uID = uID; 
	if (uID==1) 
		Separator();
}
UINT CMenuUI::GetUID()
{
	return m_pMenuInfo->uID;
}
bool CMenuUI::IsEnableItem()
{
	return m_pMenuInfo->bEnableItem;
}
void CMenuUI::EnableItem(bool bEnable/* = true*/)
{
	m_pMenuInfo->bEnableItem = bEnable;
	if (!m_pMenuInfo->bEnableItem)
		SetTextColor(DUI_RGB2ARGB(GetSysColor(COLOR_GRAYTEXT))/*DUI_ARGB(255, 116, 136, 148)*/);
	else
		SetTextColor();
}
bool CMenuUI::Remove(CControlUI* pControl)
{
	int it = 0;
	for( it = 0;  it < GetCount(); it++ )
	{
		if( m_MenuItems[it] == pControl ) {
			delete pControl;
			return m_MenuItems.Remove(it);
		}
	}
	return false;
}
void CMenuUI::SetTextColor(DWORD dwTextColor)
{
	if (GetStyleCount() > 0)
	{
		TextStyle * pText = dynamic_cast<TextStyle*>(GetStyle(_Style_MenuItem_Txt));
		if (pText)
			pText->SetTextColor(dwTextColor);
		pText = dynamic_cast<TextStyle*>(GetStyle(_Style_MenuItem_Key));
		if (pText)
			pText->SetTextColor(dwTextColor);
	}
}
bool CMenuUI::IsCheck()
{
	return m_pMenuInfo->bCheck;
}
void CMenuUI::Check(bool bCheck/* = true*/)
{
	m_pMenuInfo->bCheck = bCheck;
}
bool CMenuUI::IsSeparator()
{
	return m_pMenuInfo->bSeparator;
}
void CMenuUI::Separator(bool bSeparator/* = true*/)
{
	m_pMenuInfo->bSeparator = bSeparator;
}

void CMenuUI::SetMenuAttribute(int nIndex, LPCTSTR lpszName, LPCTSTR lpszValue, int uFlag/* = BY_POSITION*/)
{
	switch (uFlag)
	{
	case BY_POSITION:
	case BY_COMMAND:
		{
			CMenuUI* pMenu = GetItem(nIndex, uFlag);
			if (pMenu)
				pMenu->SetAttribute(lpszName, lpszValue);
		}
		break;
	}
}

void CMenuUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("separator")))
	{
		m_pMenuInfo->bSeparator = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("UID")))
	{
		m_pMenuInfo->uID = _ttoi(lpszValue);
		if (equal_icmp(lpszValue, _T("-")))
			m_pMenuInfo->bSeparator = true;
	}
	else if (equal_icmp(lpszName, _T("check")))
	{
		m_pMenuInfo->bCheck = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("text")))
	{
		m_pMenuInfo->szText = I18NSTR(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("Image")))
	{
		m_pMenuInfo->szIconFile = lpszValue;
	}
	else if (equal_icmp(lpszName, _T("background")))
	{
		m_strBackGround = lpszValue;
	}
	else if (equal_icmp(lpszName, _T("enable")))
	{
		m_pMenuInfo->bEnableItem = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("ImageSize")))
	{
		m_pMenuInfo->uImageSize = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("Key")))
	{
		m_pMenuInfo->uKey = GetItemIDFromStr(lpszValue);
		if (m_pMenuInfo->uKey != 0)
			m_pMenuInfo->szKey = lpszValue;
	}
	else if (equal_icmp(lpszName, _T("hotkey")))
	{
		m_strHotKey = lpszValue;
		SetAttribute(_T("txt.PREFIX"), _T("1"));
	}
	else if (equal_icmp(lpszName, _T("MenuWidth")))
	{
		m_szItem.cx = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("MenuHeight")))
	{
		m_szItem.cy = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("BaseWidth")))
	{
		m_nBaseItemWidth = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("ShowActive")))
	{
		m_bShowActive = _ttoi(lpszValue);
	}
	else
		CControlUI::SetAttribute(lpszName,lpszValue);
}
void CMenuUI::SetRect(RECT& rect)
{
	CViewUI::SetRect(rect);

	CMenuUI* pMenu = NULL;
	StyleObj* pStyleObj = NULL;
	int l, t, r, b;
	UITYPE_ANCHOR a;
	RECT rcStyle;
	for (int i = 0; i < GetCount(); i++)
	{
		pMenu = static_cast<CMenuUI*>(m_MenuItems.GetAt(i));
		if (pMenu)
		{
			if (pMenu->GetStyleCount() > 0)
			{
				pStyleObj = (StyleObj*)pMenu->GetStyle(_Style_MenuItem_Check);
				if (!pMenu->m_pMenuInfo->szIconFile.empty())
				{
					if (pStyleObj)
					{
						pStyleObj->SetCoordinate(2 + (20 - pMenu->m_pMenuInfo->uImageSize) / 2, -1, pMenu->m_pMenuInfo->uImageSize, pMenu->m_pMenuInfo->uImageSize, UIANCHOR_LT);
						pStyleObj->SetAttribute(_T("image"), pMenu->m_pMenuInfo->szIconFile.c_str());
					}
				}
				else if (pMenu->IsCheck())
				{
					if (pStyleObj)
					{
						pStyleObj->SetCoordinate(2, -1, 16, 16, UIANCHOR_LT);
					}
				}

				RECT rcMenu = pMenu->GetMenuItemRect();
				if (pMenu->IsSeparator())
				{
					pStyleObj = (StyleObj*)pMenu->GetStyle(_Style_MenuItem_Bk);
					if (pStyleObj)
					{
						pStyleObj->SetAttribute(_T("image"), _T("#menuseperator"));
						pStyleObj->SetAttribute(_T("pos"), _T("8|28,1,0,0"));
						//pStyleObj->OnSize(rcMenu); // 用这句话阿拉伯语不行，因为FlipRect()中pOwer->GetRect() 为空

						pStyleObj->GetCoordinate(l, t, r, b);
						a = pStyleObj->GetAnchor();
						rcStyle.left = l;
						rcStyle.top = t;
						rcStyle.right = r;
						rcStyle.bottom = b;
						DPI_SCALE(&rcStyle);
						GetAnchorPos(a, &rcMenu, &rcStyle);
						RtlRect(rcMenu, &rcStyle);
						LPRECT lpRect= pStyleObj->GetRectPtr();
						*lpRect = rcStyle;
					}
				}
				else
				{
					int nCnt = pMenu->GetStyleCount();
					for (int j = 0; j < nCnt; j++)
					{
						pStyleObj = (StyleObj*)pMenu->GetStyle(j);
						if (pStyleObj)
						{
							pStyleObj->GetCoordinate(l, t, r, b);
							a = pStyleObj->GetAnchor();

							rcStyle.left = l;
							rcStyle.top = t;
							rcStyle.right = r;
							rcStyle.bottom = b;
							DPI_SCALE(&rcStyle);
							GetAnchorPos(a, &rcMenu, &rcStyle);
							RtlRect(rcMenu, &rcStyle);
							LPRECT lpRect= pStyleObj->GetRectPtr();
							*lpRect = rcStyle;
						}
					}
					UICustomControlHelper::LayoutChildNormal(rcMenu, rcMenu, pMenu->m_SubControls, false);
				}
			}
		}
	}
}
void CMenuUI::Init()
{
	CControlUI::Init();
	//LPCTSTR lpszText = NULL;
	int nCount = GetCount();
	
	if (this->GetWindow())
	{
		int nWidth = 0;
		int l, t, r, b;
		CMenuUI* pMenu = NULL;
		for (int i = 0; i < nCount; i++)
		{
			pMenu = static_cast<CMenuUI*>(m_MenuItems[i]);
			LPCTSTR lpszText = pMenu->GetText();
			RECT rect = {0, 0, 0, 0};
			if (GetStyleCount() > 0)
			{
				TextStyle* pText = dynamic_cast<TextStyle*>((StyleObj*)GetStyle(_Style_MenuItem_Txt));
				if (pText) 
				{
					pText->GetFontObj()->CalcText(GetWindow()->GetPaintDC(), rect, lpszText);
				}
			}

			if (pMenu->m_pMenuInfo->uKey != 0)
			{
				RECT rectKey = {0, 0, 0, 0};

				if (GetStyleCount() > 0)
				{
					TextStyle* pText = dynamic_cast<TextStyle*>((StyleObj*)GetStyle(_Style_MenuItem_Txt));
					if (pText) 
					{
						pText->GetFontObj()->CalcText(GetWindow()->GetPaintDC(), rectKey, pMenu->m_pMenuInfo->szKey.c_str());
					}
				}

				StyleObj* pStyle = (StyleObj*)pMenu->GetStyle(_Style_MenuItem_Key);
				if (pStyle)
				{
					pStyle->GetCoordinate(l, t, r, b);
					if (nWidth < rect.right + rectKey.right + r)
						nWidth = rect.right + rectKey.right + r;
					pStyle->SetCoordinate(rectKey.right - rectKey.left + r, 0, rectKey.left + r, 0, UIANCHOR_RTRB);
				}
			}
			else
			{
				if (nWidth < rect.right)
					nWidth = rect.right;
			}
		}
		if (DPI_SCALE(m_szItem.cx) < nWidth + DPI_SCALE(m_nBaseItemWidth))
			m_szItem.cx = DPI_SCALE_BACK(nWidth) + DPI_SCALE(m_nBaseItemWidth);

		RECT rcInset = GetInset();
		nWidth = DPI_SCALE(m_szItem.cx) + DPI_SCALE(rcInset.right);
		int nHeight = DPI_SCALE(rcInset.top);
		RECT rcItem;
		for (int i = 0; i < GetCount(); i++)
		{
			pMenu = static_cast<CMenuUI*>(m_MenuItems[i]);
			if (pMenu->IsSeparator())
			{
				rcItem.left = rcInset.left;
				rcItem.top = nHeight;
				rcItem.right = nWidth;
				rcItem.bottom = nHeight + MENUSEP_HEIGHT;
				nHeight += MENUSEP_HEIGHT;
			}
			else
			{
				rcItem.left = rcInset.left;
				rcItem.top = nHeight;
				rcItem.right = nWidth;
				rcItem.bottom = nHeight + DPI_SCALE(m_szItem.cy);
				nHeight += DPI_SCALE(m_szItem.cy);
			}
			pMenu->SetMenuItemRect(rcItem);
		}
		nHeight += rcInset.bottom;
		nWidth += rcInset.right;
		m_szMenuWindow.cx = nWidth;
		m_szMenuWindow.cy = nHeight;
	}
}

void CMenuUI::SetMenuItemRect(RECT& rc)
{
	m_rcMenuItem = rc;
}

RECT CMenuUI::GetMenuItemRect()
{
	return m_rcMenuItem;
}

void CMenuUI::SetText(LPCTSTR lpszText)
{
	this->m_pMenuInfo->szText = lpszText;
}

LPCTSTR CMenuUI::GetText()
{
	if (m_strHotKey.empty())
	{
		return m_pMenuInfo->szText.c_str();
	}
	else
	{
		tstring strHotKey = m_pMenuInfo->szText;

		strHotKey += _T("(&");
		strHotKey += m_strHotKey;
		strHotKey += _T(")");
		m_strResultText = strHotKey;
		return m_strResultText.c_str();
	}
}

void CMenuUI::SetParam(WPARAM wParam, LPARAM lParam)
{
	m_pMenuInfo->wParamNotify = wParam;
	m_pMenuInfo->lParamNotify = lParam;
}

WPARAM CMenuUI::GetWParam()
{
	return m_pMenuInfo->wParamNotify;
}

LPARAM CMenuUI::GetLParam()
{
	return m_pMenuInfo->lParamNotify;
}

int CMenuUI::GetCount()
{
	return m_MenuItems.GetSize();
}

void CMenuUI::SetVisible( bool bVisible)
{
	int nCnt = m_SubControls.GetSize();
	for( int i = 0; i < nCnt; i++ ) 
		static_cast<CControlUI*>(m_SubControls.GetAt(i))->SetInternVisible(bVisible);
	__super::SetVisible(bVisible);
}

void CMenuUI::SetInternVisible( bool bVisible )
{
	int nCnt = m_SubControls.GetSize();
	for( int i = 0; i < nCnt; i++ ) 
		static_cast<CControlUI*>(m_SubControls.GetAt(i))->SetInternVisible(bVisible);
	__super::SetInternVisible(bVisible);
}

UINT GetItemIDFromStr(LPCTSTR lpszText)
{
	UINT lRes = 0;
	if (lpszText != NULL)
	{
		int nLen = _tcslen(lpszText);
		if (nLen >= 2)
		{
			if (*lpszText == 'F')
			{
				lRes = VK_F1 + _ttoi(lpszText + 1) -1;
			}
			else
			{
				lRes = 0xffff;
			}
		}
		else if (nLen == 1)
		{
			lRes = *lpszText;
		}
	}
	return lRes;
}