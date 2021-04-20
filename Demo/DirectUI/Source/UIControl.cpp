/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************/

#include "stdafx.h"
#include "UIControl.h"

CControlUI* Invoke( TEventUI& event, CControlUI* pControl)
{
	CControlUI* pParent = pControl->GetParent();
	if (pParent && pParent->EventToParent() && pControl->EventToParent())
	{
		return Invoke(event, pParent);
	}
	else
	{
		pControl->OnEvent(&event);
		return pControl;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
CControlUI::CControlUI()
:m_bAsyncNotify(false)
,m_bInterVisible(true)
,m_bEnable(true)
,m_bFocused(false)
,m_pParent(NULL)
,m_pOwner(NULL)
,m_pManager(NULL)
,m_bMouseEnable(false)
, m_bGestureEnable(false)
,m_bMouseWhellEnable(false)
,m_bShowToolTips(true)
,m_hToolTipWnd(NULL)
,m_bNeedScroll(true)
,m_uControlFlags(UICONTROLFLAG_SETFOCUS)
,m_bAutoSize(false)
,m_bSelect(false)
,m_uUserData(0)
,m_pStyleList(NULL)
,m_nIndex(0)
,m_pINotifySafe(NULL)
,m_strStyleName(_T(""))
,m_lpszTextKey(NULL)
,m_lpszClassName(NULL)
,m_bEventToParent(false)
,m_lpszToolTipsKey(NULL)
,m_pCursorObj(ResObj::ms_pDefaultCursorObj)
,m_nZOrder(0)
,m_nZOrders(0)
, m_bSizeWithContent(false)
, m_pDefTextStyle(NULL)
, m_pUIControlDestory(NULL)
,m_bOnSizeCallback(false)
,m_bExceptGroup(false)
, m_bNeedMouseInOutEvent(false)
{	
	SetState(UISTATE_NORMAL);
	//SetId(_T("control"));
	::SetRect(&m_rcInset, 0, 0, 0, 0);
}

CControlUI::~CControlUI()
{
	if (GetFlags() & UICONTROLFLAG_ENABLE_DESTORY_EVENT)
	{
		TEventUI event = {0};
		event.nType = UIEVENT_DESTORY;
		this->SendEvent(event);
	}

	if (GetFlags() & UICONTROLFLAG_CHECK_CTR_PTR_VALID)
		GetEngineObj()->RemoveControlPtrToCheck(this);

	SetStyle(NULL);
	SetINotifyUI(NULL);

	CWindowUI* pWindow = GetWindow();
	if (pWindow)
		pWindow->ReapObjects(this);
	if (m_lpszTextKey)
	{
		delete [] m_lpszTextKey;
		m_lpszTextKey = NULL;
	}

	if (m_lpszToolTipsKey)
	{
		delete [] m_lpszToolTipsKey;
		m_lpszToolTipsKey = NULL;
	}

	if (m_lpszClassName)
	{
		delete [] m_lpszClassName;
		m_lpszClassName = NULL;
	}

	if (m_hToolTipWnd && IsWindow(m_hToolTipWnd))
	{
		CWindowUI* pToolTipWindow = GetEngineObj()->GetWindow(m_hToolTipWnd);
		if (pToolTipWindow) pToolTipWindow->CloseWindow();
	}

	if (m_pUIControlDestory)
		m_pUIControlDestory->OnControlDestory(this);
}


void CControlUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	RenderStyle(pRenderDC, rcPaint);
}

void CControlUI::RenderStyle( IRenderDC* pRenderDC, RECT& rcPaint, int nBegin /*= 0*/, int nEnd /*= -1*/ )
{
	int nCnt = nEnd;
	if (nEnd == -1)
		nCnt = GetStyleCount();
	//StyleObj* pStyleObj = NULL;
	for (int i = nBegin; i < nCnt; i++)
	{
		StyleObj* pStyleObj = GetStyle(i);
		if (pStyleObj && pStyleObj->IsVisible())
			pStyleObj->Render(pRenderDC, rcPaint);
	}
}

LPCTSTR CControlUI::GetTooltipKey()
{
	if (!m_lpszToolTipsKey)
	{
		return _T("");
	}

	return m_lpszToolTipsKey;
}

void CControlUI::ReleaseImage()
{
	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
		if (pStyleObj)
			pStyleObj->ReleaseResource();
	}
}


RECT CControlUI::GetClientRect()
{
	return GetRect();
}

RECT CControlUI::GetInterRect()
{
	RECT rc = GetRect();
	RECT rcInset = GetInset();
	DPI_SCALE(&rcInset);
// 	if (GetUIEngine()->IsRTLLayout())
// 	{
// 		rc.left += rcInset.right;
// 		rc.right -= rcInset.left;
// 		rc.top += rcInset.top;
// 		rc.bottom -= rcInset.bottom;
// 	}
// 	else
	{
		rc.left += rcInset.left;
		rc.right -= rcInset.right;
		rc.top += rcInset.top;
		rc.bottom -= rcInset.bottom;
	}
	return rc;
}

inline void CControlUI::FlipRect()
{
	if (!IsRTLLayoutEnable())
		return;

	if (GetUIEngine()->IsRTLLayout())
	{
		CControlUI* pParent = GetParent();
		if (pParent)
		{
			RECT rcParent;
			if (IsNeedScroll())
				rcParent = pParent->GetScrollRect();
			else
				rcParent = pParent->GetRect();

			RtlRect(rcParent, GetRectPtr());
		}

	}
}


void CControlUI::SetInset(RECT& rect)
{
	m_rcInset = rect;
}

RECT& CControlUI::GetInset()
{
	return m_rcInset;
}

UINT CControlUI::GetFlags()
{
	return m_uControlFlags;
}

void CControlUI::ModifyFlags(UINT uAddFlags, UINT uDelFlags)
{
	if (uAddFlags)
	{
		m_uControlFlags |= uAddFlags;
		if (uAddFlags & UICONTROLFLAG_CHECK_CTR_PTR_VALID)
			GetEngineObj()->AddControlPtrToCheck(this);
	}
	if (uDelFlags)
	{
		m_uControlFlags &= ~uDelFlags;
		if (uDelFlags & UICONTROLFLAG_CHECK_CTR_PTR_VALID)
			GetEngineObj()->RemoveControlPtrToCheck(this);
	}
}

void CControlUI::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
	/*if (IsSizeWithContent())
		Resize();*/
	Invalidate();
}

LPCTSTR CControlUI::GetTextKey()
{
	if (!m_lpszTextKey)
	{
		return _T("");
	}

	return m_lpszTextKey;
}

LPCTSTR CControlUI::GetText()
{
	return m_strText.c_str();
}


LPCTSTR CControlUI::GetToolTip()
{
	return m_strToolTips.c_str();
}

void CControlUI::SetToolTip(LPCTSTR lpszTips)
{
	m_strToolTips = lpszTips;
}



bool CControlUI::IsInternVisible()
{
	return m_bInterVisible;
}


void CControlUI::SetVisible(bool bVisible)
{
	if (IsVisible() == bVisible) return;
	__super::SetVisible(bVisible);
	CWindowUI* pWindow = GetWindow();
	if (pWindow)
	{
		if (bVisible && IsRectEmpty(GetRectPtr()))
		{
			if (!IsAutoSize())
			{
				Resize();
			}
			//Resize(); //这句话可能会陷入死循环，比如在父控件的OnSize中调用子控件的SetVisible
		}
		pWindow->Invalidate(GetRectPtr());
	}
}
void CControlUI::SetInternVisible( bool bVisible )
{
	m_bInterVisible = bVisible;
}
int CControlUI::GetIndex()
{
	return m_nIndex;
}
void CControlUI::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
}
bool CControlUI::IsAutoSize()
{
	return m_bAutoSize;
}

void CControlUI::SetAutoSize(bool bAutoSize)
{
	m_bAutoSize = bAutoSize;
}

bool CControlUI::IsSizeWithContent()
{
	return m_bSizeWithContent;
}

void CControlUI::SetSizeWithContent(bool bSizeWidthContent)
{
	m_bSizeWithContent = bSizeWidthContent;
}

bool CControlUI::IsEnabled()
{
	return m_bEnable;
}

void CControlUI::SetEnabled(bool bEnable)
{
	if (m_bEnable == bEnable) return;
	m_bEnable = bEnable;
	Invalidate();
}

bool CControlUI::IsFocused()
{
	return m_bFocused;
}

void CControlUI::SetFocus()
{
	CWindowUI* pWindow = GetWindow();
	if (pWindow)
		pWindow->SetFocus(this);
}

void CControlUI::KillFocus()
{
	CWindowUI* pWindow = GetWindow();
	if (pWindow)
		pWindow->KillFocus(this);
}

bool CControlUI::IsSelected()
{
	return m_bSelect;
}

void CControlUI::Select( bool bSelect)
{
	if (m_bSelect == bSelect)
		return;
	m_bSelect = bSelect;
	Invalidate();
}

CControlUI*	CControlUI::GetOwner()
{
	if (m_pOwner)
	{
		return static_cast<CControlUI*>(m_pOwner);
	}
	else
	{
		return GetParent();
	}
}

void CControlUI::SetOwner(CControlUI* pOwner)
{
	m_pOwner = pOwner;
}

uint8 CControlUI::GetZOrder()
{
	return m_nZOrder;

}

void CControlUI::SetZOrder(uint8 nZOrder)
{
	m_nZOrder = nZOrder;
	if (GetParent() && GetParent()->m_nZOrders < GetZOrder())
		GetParent()->m_nZOrders = GetZOrder();
}

bool CControlUI::IsContainer()
{
	return false;
}

INotifyUI* CControlUI::GetINotifyUI()
{
	if (m_pINotifySafe && m_pINotifySafe->IsValid())
	{
		return m_pINotifySafe->GetCountedOwner();
	}
	return NULL;
}

void CControlUI::SetINotifyUI(INotifyUI* pINotify)
{
	if (m_pINotifySafe)
		m_pINotifySafe->Release();
	m_pINotifySafe = NULL;
	if (pINotify)
	{
		m_pINotifySafe = pINotify->GetINotifySafe();

		m_pINotifySafe->AddRef();
	}
}

void CControlUI::SetControlDestoryNotify(IUIControlDestory* pControlDestory)
{
	m_pUIControlDestory = pControlDestory;
}

bool CControlUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_SETCURSOR:
		{
			::SetCursor(GetCursor());
		}
		break;
	case UIEVENT_SETFOCUS:
		{
			m_bFocused = true;
			Invalidate();
		}
		break;

	case UIEVENT_KILLFOCUS:
		{
			m_bFocused = false;
			Invalidate();
		}
		break;

	case UIEVENT_DRAG_DROP_GROUPS:
		{	
			if (event.wParam == UIEVENT_BEGIN_DRAG)
			{
				RECT rect = GetClientRect();
				POINT pt = {(int)(short)LOWORD(event.lParam),(int)(short)HIWORD(event.lParam) };
				if (PtInRect(&rect, pt)) 
					SendNotify(UINOTIFY_DRAG_DROP_GROUPS, event.wParam, event.lParam);
			}
			else 
			{
				SendNotify(UINOTIFY_DRAG_DROP_GROUPS, event.wParam, event.lParam);
			}

			if (event.wParam == UIEVENT_DROP)
			{
				DragDropParams* pNotifyUI = (DragDropParams*)event.lParam;
				if (pNotifyUI->bFiles)
				{
					IDataObject* pDataObject = pNotifyUI->pDataObject;
					STGMEDIUM stgMedium;
					FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
					HRESULT ret = pDataObject->GetData(&fmt, &stgMedium);
					if (ret == S_OK && stgMedium.hGlobal)
					{
						SendNotify(UINOTIFY_DROPFILES, (WPARAM)stgMedium.hGlobal, NULL);
						*(pNotifyUI->pdwEffect) = DROPEFFECT_COPY;
					}
				}
			}
		}
		break;
	case UIEVENT_GESTURENOTIFY:
	{
		/*	GESTURECONFIG gc = { 0, GC_ALLGESTURES, 0 };
			UINT uiGcs = 1;*/
		DWORD dwPanWant = GC_PAN_WITH_SINGLE_FINGER_VERTICALLY | GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY | GC_PAN_WITH_INERTIA;
		DWORD dwPanBlock = GC_PAN_WITH_GUTTER;
		// set the settings in the gesture configuration
		GESTURECONFIG gc[] = { { GID_ZOOM, GC_ZOOM, 0 },
		{ GID_ROTATE, GC_ROTATE, 0 },
		{ GID_PAN, dwPanWant, dwPanBlock },
		{ GID_TWOFINGERTAP, GC_TWOFINGERTAP, 0 },
		{ GID_PRESSANDTAP, GC_PRESSANDTAP, 0 }
		};
		UINT uiGcs = 5;
		if (set_gesture_config)
			set_gesture_config(GetWindow()->GetHWND(), 0, uiGcs, gc, sizeof(GESTURECONFIG));
	}
	break;
	}

	// 101ppt新增的不确定代码
	if (event.nType == UIEVENT_SCROLLWHEEL
		&& m_pParent)
	{
		m_pParent->Event(event);
	}

	return true;
}

bool CControlUI::ChildEvent(TEventUI& event)
{
	return true;
}

CControlUI* CControlUI::GetParent()
{
	return m_pParent;
}

void CControlUI::SetParent(CControlUI* pParent)
{
	m_pParent = pParent;
}

void CControlUI::Init()
{

}

void CControlUI::OnDestroy()
{
	ReleaseImage();
}

void CControlUI::OnLanguageChange()
{
	if (m_lpszTextKey)
		SetAttribute(_T("text"), m_lpszTextKey);

	if ( m_lpszToolTipsKey)
		SetAttribute(_T("tooltip"), m_lpszToolTipsKey);


	for (int i = 0; i < GetStyleCount(); i++)
	{
		StyleObj* pStyleObj = GetStyle(i);
		if (pStyleObj)
			pStyleObj->OnLanguageChange();
	}
}

CWindowUI* CControlUI::GetWindow()
{
	if (!m_pManager)
	{
		CControlUI* pParent = GetParent();
		CWindowUI* pWindow = NULL;
		while (!pWindow)
		{
			if (pParent)
			{
				pWindow = pParent->GetWindow();
				if (pWindow)
					break;
				else
					pParent = pParent->GetParent();
			}
			else 
			{
				break;
			}

		}
		m_pManager = pWindow;
	}
	return m_pManager;
}

void CControlUI::SetManager(CWindowUI* pManager, CControlUI* pParent)
{
	/*if (this == pManager->GetRoot())*/ //这句话如果不注销掉，就无法登陆，过后测试
	m_pManager = pManager;
	m_pParent = pParent;
	if (GetParent() && GetParent()->m_nZOrders < GetZOrder())
		GetParent()->m_nZOrders = GetZOrder();
}

bool CControlUI::Notify(TNotifyUI* msg)
{
	if (OnNotify.GetCount() > 0)
	{
		OnNotify(msg);
	}
	return false;
}

bool CControlUI::HitTest(POINT& point)
{
	if(!IsVisible()) return false;
	if (PtInRect(GetRectPtr(),point))
		return true;
	return false;
}

bool CControlUI::HitClientTest(POINT& point)
{
	if(!IsVisible()) return false;
	if (PtInRect(&GetClientRect(),point))
		return true;
	return false;
}

void CControlUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (GetStyle())
	{
		tstring strStyle = lpszName;
		int i = strStyle.find(_T('.'));
		if (i > 0)
		{
			tstring strStyleName = strStyle.substr(0, i);
			tstring strStyleAttrName = strStyle.substr(i + 1);
			int nCnt = GetStyleCount();
			for (int j = 0; j < nCnt; j++)
			{
				StyleObj* pStyleObj = (StyleObj*)GetStyle(j);
				if (pStyleObj && equal_icmp(pStyleObj->GetId(), strStyleName.c_str()))
					pStyleObj->SetAttribute(strStyleAttrName.c_str(), lpszValue);
			}
			return;
		}
	}
	if (equal_icmp(lpszName, _T("Inset")))
	{
		tstring spliter = lpszValue;
		short pos[4] = {2, 2, 2, 2};
		int i = spliter.find_first_of(_T(','));
		unsigned char index = 0; 
		while (i >= 0)
		{
			pos[index] = _ttoi(spliter.substr(0, i).c_str());
			spliter = spliter.substr(i + 1);
			i = spliter.find_first_of(_T(','));
			index++;
		}
		pos[index] = _ttoi(spliter.c_str());
		RECT rcInset = { pos[0], pos[1], pos[2], pos[3] };
		SetInset(rcInset);
	}
	else if (equal_icmp(lpszName, _T("text")))
	{
		bool bFind = false;
		m_strText = I18NSTR(lpszValue, &bFind);
		if (lpszValue[0] == _T('#') /*&& bFind*/)
			copy_str(m_lpszTextKey, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("style")))
	{
		SetStyle(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("classname")))
	{
		copy_str(m_lpszClassName, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("ToolTip")))
	{
		bool bFind = false;
		m_strToolTips = I18NSTR(lpszValue, &bFind);
		if (lpszValue[0] == _T('#')/* && bFind*/)
			copy_str(m_lpszToolTipsKey, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("cursor")))
	{
		m_pCursorObj = GetUIRes()->GetCursor(lpszValue);
		ModifyFlags(UICONTROLFLAG_SETCURSOR, 0);
	}
	else if (equal_icmp(lpszName, _T("Scroll")))
	{
		m_bNeedScroll = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("AutoSize")))
	{
		SetAutoSize((bool)!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("EnableMouse")))
	{
		m_bMouseEnable = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("gesture")))
	{
		m_bGestureEnable = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("mousewhell")))
	{
		m_bMouseWhellEnable = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("Enable")))
	{
		m_bEnable = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("UserData")))
	{
		m_uUserData = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("async")))
		m_bAsyncNotify = !!_ttoi(lpszValue);
	else if (equal_icmp(lpszName, _T("ZOrder")))
	{
		SetZOrder(_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("tabstop")))
	{
		if ((bool)!!_ttoi(lpszValue))
			ModifyFlags(UICONTROLFLAG_TABSTOP, 0);
		else
			ModifyFlags(0, UICONTROLFLAG_TABSTOP);
	}
	else if (equal_icmp(lpszName, _T("check_ptr")))
	{
		if ((bool)!!_ttoi(lpszValue))
			ModifyFlags(UICONTROLFLAG_CHECK_CTR_PTR_VALID, 0);
		else
			ModifyFlags(0, UICONTROLFLAG_CHECK_CTR_PTR_VALID);
	}
	else if (equal_icmp(lpszName, _T("sizewithcontent")))
	{
		SetSizeWithContent(!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("maxsize")))
	{
		tstring spliter = lpszValue;
		int i = spliter.find_first_of(_T(','));
		if (i >= 0)
		{
			SIZE sz;
			sz.cx = _ttoi(spliter.substr(0, i).c_str());
			spliter = spliter.substr(i + 1);
			sz.cy = _ttoi(spliter.c_str());
			this->SetMaxSize(sz);
		}
	}
	else if (equal_icmp(lpszName, _T("minsize")))
	{
		tstring spliter = lpszValue;
		int i = spliter.find_first_of(_T(','));
		if (i >= 0)
		{
			SIZE sz;
			sz.cx = _ttoi(spliter.substr(0, i).c_str());
			spliter = spliter.substr(i + 1);
			sz.cy = _ttoi(spliter.c_str());
			SetMinSize(sz);
		}
	}
	else if (equal_icmp(lpszName, _T("ExceptGroup")))
	{
		m_bExceptGroup = ((bool)!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("tag")))
	{		
	 	m_strTag = I18NSTR(lpszValue);
    }
    else if (equal_icmp(lpszName, _T("NeedMouseInOut")))
    {
        m_bNeedMouseInOutEvent = (bool)!!_ttoi(lpszValue);
    }
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

bool CControlUI::IsClass(LPCTSTR lpszName)
{
	if (m_lpszClassName)
		return equal_icmp(m_lpszClassName, lpszName);
	return equal_icmp(GetClassName(), lpszName);
}


bool CControlUI::Resize()
{
	CWindowUI* pWindow = GetWindow();
	if (GetParent())
	{
		CControlUI* pParent = GetParent();
		//add by hanzp
	/*	if (IsAutoSize() || IsSizeWithContent())
			return pParent->Resize();*/
		// 101ppt修改的不确定代码
		if (IsAutoSize() || IsSizeWithContent())
		{
			if (pParent->IsAutoSize())
			{
				return pParent->Resize();
			}
			else
			{
				return pParent->OnlyResizeChild();
			}
		}

		//end add

		RECT rect = pParent->GetRect();
		if (IsRectEmpty(&rect))
			return false;

		if (IsNeedScroll())
		{
			RECT rcScrollRect = pParent->GetScrollRect();
			if (!IsRectEmpty(&rcScrollRect))
				OnSize(rcScrollRect);
			else
				SetRect(GetRect());
		}
		else
		{
			OnSize(rect);
		}
	}
	else if (pWindow && this == pWindow->GetRoot() && pWindow->GetHWND())
	{
		RECT rect;
		::GetClientRect(m_pManager->GetHWND(), &rect);
		if (!IsRectEmpty(&rect))
			OnSize(rect);
		else
			return false;
	}
	return true;
}

bool CControlUI::OnlyResizeChild()
{
	return false;
}

void CControlUI::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);
	
	RECT rcItem = GetRect();
	if (IsSizeWithContent())
	{
		TextStyle* pStyle = GetDefaultTextStyle();
		if (pStyle)
		{
			pStyle->OnSize(rectRegion);
			RECT rectStyle = pStyle->GetRect();
			int marginLeft = rectStyle.left - rcItem.left;
			int marginTop = rectStyle.top - rcItem.top;
			int marginRight = rcItem.right - rectStyle.right;
			int marginBottom = rcItem.bottom - rectStyle.bottom;

			RECT rectText;
			rectText.left = 0;
			rectText.top = 0;
			rectText.right = rectStyle.right - rectStyle.left;
			rectText.bottom = rectStyle.bottom - rectStyle.top;

			pStyle->GetFontObj()->CalcText(GetWindow()->GetPaintDC(), rectText, m_strText.c_str(), pStyle->GetAlign());

			if (pStyle->GetAlign() & DT_SINGLELINE)
				rcItem.right = rcItem.left + marginLeft + marginRight + rectText.right - rectText.left;
			else if ((pStyle->GetAlign() & (DT_WORDBREAK | DT_EDITCONTROL)) == (DT_WORDBREAK | DT_EDITCONTROL))
				rcItem.bottom = rcItem.top + marginTop + marginBottom + rectText.bottom - rectText.top;

			SIZE maxSize = GetMaxSize();
			DPI_SCALE(&maxSize);
			if (maxSize.cx != 0 && rcItem.right - rcItem.left > maxSize.cx)
			{
				rcItem.right = rcItem.left + maxSize.cx;
			}
			if (maxSize.cy != 0 && rcItem.bottom - rcItem.top > maxSize.cy)
			{
				rcItem.bottom = rcItem.top + maxSize.cy;
			}

			SIZE minSize = GetMinSize();
			DPI_SCALE(&minSize);
			if (minSize.cx != 0 && rcItem.right - rcItem.left < minSize.cx)
			{
				rcItem.right = rcItem.left + minSize.cx;
			}
			if (minSize.cy != 0 && rcItem.bottom - rcItem.top < minSize.cy)
			{
				rcItem.bottom = rcItem.top + minSize.cy;
			}

			CViewUI::SetRect(rcItem);
		}
	}

	FlipRect();
	rcItem = GetRect();
	int nCnt = GetStyleCount();
	//StyleObj* pStyleObj = NULL;
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = GetStyle(i);
		if (pStyleObj)
			pStyleObj->OnSize(rcItem);
	}
}

void CControlUI::Invalidate(bool bUpdateWindow /*= false*/)
{
	CWindowUI* pWindow = GetWindow();
	if( pWindow != NULL && IsInternVisible()) 
	{
		RECT rc = GetRect();
		pWindow->Invalidate(&rc, bUpdateWindow);
	}
}

int CControlUI::SetTimer( UINT nTimerID, UINT uElapse )
{
	CWindowUI* pWindow = GetWindow();
	if (pWindow)
		return pWindow->SetTimer(this, nTimerID, uElapse);
	return 0;
}

bool CControlUI::KillTimer(UINT nTimerID)
{
	CWindowUI* pWindow = GetWindow();
	if (pWindow)
		return pWindow->KillTimer(this, nTimerID);
	return false;
}

bool CControlUI::Add(CControlUI* pControlChild)
{
	return false;
}

bool CControlUI::Insert(CControlUI* pControlChild, CControlUI* pControlAfter)
{
	return false;
}

bool CControlUI::Remove(CControlUI* pControlChild)
{
	return true;
}

int CControlUI::GetCount()
{
	return 0;
}

CControlUI* CControlUI::GetItem(LPCTSTR lpszId)
{
	return NULL;
}

CControlUI* CControlUI::GetItem(int iIndex)
{
	return NULL;
}

CControlUI* CControlUI::FindItem(LPCTSTR lpszId)
{
	return GetWindow()->FindControl(lpszId, this);
}

void CControlUI::SetNeedScroll(bool bNeedScroll)
{
	m_bNeedScroll = bNeedScroll;
}

bool CControlUI::IsNeedScroll()
{
	return m_bNeedScroll;

}

CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
	if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_MOUSE_ENABLED) != 0 && !IsMouseEnabled() ) return NULL;
	if ((uFlags & UIFIND_GESTURE_ENABLED) != 0 && !IsGestureEnabled()) return NULL;
	if ((uFlags & UIFIND_MOUSEWHELL_ENABLED) != 0 && !IsMouseWhellEnabled()) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!HitTest(* static_cast<LPPOINT>(pData)))) return NULL;
	if ((uFlags & UIFIND_MOUSEINOUT) != 0 && !IsNeedMouseInOut()) return NULL;
	return Proc(this, pData);
}

bool CControlUI::Activate()
{
	if( !IsVisible() || !IsEnabled()) return false;
	return true;
}

void CControlUI::SetUserData(UINT uData)
{
	m_uUserData = uData;
}
UINT CControlUI::GetUserData()
{
	return m_uUserData;
}
LPCTSTR CControlUI::GetTag()
{
	return m_strTag.c_str();
}

void CControlUI::SetTag(LPCTSTR lpstrTag)
{
	m_strTag = lpstrTag;
}
ImageStyle* CControlUI::GetImageStyle(LPCTSTR lpszId)
{
	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		ImageStyle* pStyleObj = dynamic_cast<ImageStyle*>(GetStyle(i));
		if (pStyleObj && equal_icmp(pStyleObj->GetId(), lpszId))
			return pStyleObj;
	}
	return NULL;
}

TextStyle* CControlUI::GetTextStyle(LPCTSTR lpszId)
{
	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		TextStyle* pStyleObj = dynamic_cast<TextStyle*>(GetStyle(i));
		if (pStyleObj && equal_icmp(pStyleObj->GetId(), lpszId))
			return pStyleObj;
	}
	return NULL;
}

#define  DEFAULT_TEXT_STYLE_NAME _T("txt")
TextStyle* CControlUI::GetDefaultTextStyle()
{
	if (m_pDefTextStyle == NULL)
	{
		m_pDefTextStyle = GetTextStyle(DEFAULT_TEXT_STYLE_NAME);
	}
	return m_pDefTextStyle;
}

void CControlUI::SetStyle( LPCTSTR lpszName )
{
	m_pDefTextStyle = NULL;
	if (!lpszName || lpszName[0] == _T('0') || !equal_icmp(m_strStyleName.c_str(), lpszName))
	{
		if (m_pStyleList)
		{
			int nCnt = GetStyleCount();
			for (int i = 0; i < nCnt; i++)
			{
				StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
				if (pStyleObj)
					delete pStyleObj;
			}
			if (m_pStyleList)
				delete 	m_pStyleList;
			m_pStyleList = NULL;
		}
	}
	if (lpszName)
		m_strStyleName = lpszName;
}

LPCTSTR CControlUI::GetStyleName()
{
	return m_strStyleName.c_str();
}

CStdPtrArray* CControlUI::GetStyle()
{
	if (m_pStyleList)
		return m_pStyleList;
	if (m_strStyleName.empty()) return NULL;
	GetResObj()->AddSytleList(this, &m_pStyleList, m_strStyleName.c_str());
	return m_pStyleList;
}


StyleObj* CControlUI::GetStyle(LPCTSTR lpszId)
{
	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
		if (equal_icmp(pStyleObj->GetId(), lpszId))
			return pStyleObj;
	}
	return NULL;
}

int CControlUI::GetStyleIndex( LPCTSTR lpszId )
{
	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
		if (equal_icmp(pStyleObj->GetId(), lpszId))
			return i;
	}
	return -1;
}

StyleObj* CControlUI::GetStyle(int nIndex)
{
	CStdPtrArray* pStdArray = GetStyle();
	if (pStdArray)
		return (StyleObj*)pStdArray->GetAt(nIndex);
	return NULL;
}

int CControlUI::GetStyleCount()
{
	CStdPtrArray* pStdArray = GetStyle();
	if (pStdArray)
		return pStdArray->GetSize();
	return 0;
}

bool CControlUI::SendNotify(int type, WPARAM wParam /*= NULL*/, LPARAM lParam /*= NULL*/)
{
	if (GetWindow())
		return GetWindow()->SendNotify(this, type, wParam, lParam, m_bAsyncNotify);
	return false;
}

bool CControlUI::SendEvent(TEventUI& event) 	// 发送外部事件通知
{
	if (!event.pSender) event.pSender = this;

	CControlUI* pSender = event.pSender;

	event.pSender = this;
	CControlUI* pDelegate = Invoke(event, this); 
	event.pSender = pSender;

	if (event.nType != UIEVENT_UNUSED && pDelegate != this)
		this->OnEvent(&event);

	if (event.nType != UIEVENT_UNUSED)
		return this->Event(event);
	return true;
}

bool CControlUI::IsToolTipShow()
{
	return m_bShowToolTips;
}

void CControlUI::SetToolTipShow( bool bShow )
{
	m_bShowToolTips = bShow;
}

bool CControlUI::IsMouseEnabled()
{
	return m_bMouseEnable;
}

void CControlUI::SetMouseEnabled( bool bEnable )
{
	m_bMouseEnable = bEnable;
}

// 是否启用触摸屏手势
bool CControlUI::IsGestureEnabled()
{
	return m_bGestureEnable;
}

void CControlUI::SetGestureEnabled(bool bEnable)
{
	m_bGestureEnable = bEnable;
}

bool CControlUI::IsMouseWhellEnabled()
{
	return m_bMouseWhellEnable;
}

void CControlUI::SetMouseWhellEnabled(bool bEnable)
{
	m_bMouseWhellEnable = bEnable;
}

bool CControlUI::IsNeedMouseInOut()
{
    return m_bNeedMouseInOutEvent;
}

void CControlUI::SetNeedMouseInOut(bool bEnable)
{
    m_bNeedMouseInOutEvent = bEnable;
}

#define TOOTTIP_OFFSET_X 35
#define TOOLTIP_WIDTH 180
void CControlUI::ShowToolTip( LPCTSTR lpszContent, UITYPE_TOOLTIP type /*= enToolTipTypeInfo */, LPCTSTR lpszAlign /*= NULL*/, LPPOINT lpPoint /*= NULL*/ )
{
	if (!IsVisible())
		return;
	CToolTipUI* pToolTipWindow = NULL;
	if (m_hToolTipWnd && IsWindow(m_hToolTipWnd))
	{
		CWindowUI* pWindow = GetEngineObj()->GetWindow(m_hToolTipWnd);
		pToolTipWindow = static_cast<CToolTipUI*>(pWindow);
	}
	if (pToolTipWindow)
		pToolTipWindow->SetLockTimer(true);

	if (pToolTipWindow == NULL)
	{
		pToolTipWindow = new CToolTipUI(lpszContent, type, GetWindow()->GetHWND());
		m_hToolTipWnd = pToolTipWindow->GetHWND();
	}
	else
	{
		pToolTipWindow->SetToolTipType(type);
		CControlUI* pControl = pToolTipWindow->GetItem(_T("content"));
		if (pControl)
			pControl->SetText(lpszContent);
	}
	RECT rect = GetRect();
	int  tooltipOffsetX = DPI_SCALE(TOOTTIP_OFFSET_X);
	int  tooltipWidth = DPI_SCALE(TOOLTIP_WIDTH);
	POINT pt = { rect.left + tooltipOffsetX, rect.bottom };
	if (lpPoint)
	{
		pt.x = lpPoint->x;
		pt.y = lpPoint->y;
		DPI_SCALE(&pt);
	}
	else
	{
		if (lpszAlign)
		{
			if (equal_icmp(lpszAlign, _T("BL")))
			{
				pt.x = rect.left + tooltipOffsetX; pt.y = rect.top;
			}
			else if (equal_icmp(lpszAlign, _T("TL")))
			{
				pt.x = rect.left + tooltipOffsetX; pt.y = rect.bottom;
			}
			else
			{
				RT_ASSERT(0, _T("CControlUI::ShowToolTip: when set param lpszAlign, must set lppoint param"));
			}
		}
	}
	GetWindow()->Invalidate(NULL, false);
	ClientToScreen(GetWindow()->GetHWND(), &pt);
	if (lpszAlign)
		pToolTipWindow->SetAlign(lpszAlign);
	else
		pToolTipWindow->SetAlign(_T("TL"));
	pToolTipWindow->SetWidth(tooltipWidth);
	pToolTipWindow->SetElapse(1000);
	pToolTipWindow->SetPointTo(pt);
	pToolTipWindow->ShowTootipWindow();
	pToolTipWindow->SetLockTimer(false);
}

void CControlUI::EventToParent( bool val )
{
	m_bEventToParent = val;
	if (EventToParent())
	{
		//CControlUI* pControl = NULL;
		int nCnt = GetCount();
		for (int i = 0; i < nCnt; i++)
		{
			CControlUI* pControl = GetItem(i);
			if (pControl)
			{
				pControl->EventToParent(val);
			}
		}
	}
}


bool CControlUI::EventToParent() const
{
	return m_bEventToParent;
}

HCURSOR CControlUI::GetCursor()
{
	return m_pCursorObj->GetHCursor();
}

RECT CControlUI::GetScrollRect()
{
	return GetClientRect();
}

ImageObj* CControlUI::GenerateImage()
{
	CWindowUI* pWindow = GetWindow();
	if (pWindow)
	{
		RECT rcWindow;
		::GetWindowRect(pWindow->GetHWND(), &rcWindow);
		RECT rcItem = GetRect();

		if (::IsRectEmpty(&rcItem))
			return NULL;

		RECT rcOffset = rcItem;
		::OffsetRect(&rcOffset, -rcOffset.left, -rcOffset.top);

		DibObj* dibobj = pWindow->GetDibObj();
		DibObj dibobjTempBackup;
		dibobjTempBackup.Create(dibobj->GetSafeHdc(), rcOffset.right - rcOffset.left, rcOffset.bottom - rcOffset.top);
		::BitBlt(dibobjTempBackup.GetSafeHdc(), 0, 0, rcOffset.right - rcOffset.left, rcOffset.bottom - rcOffset.top, dibobj->GetSafeHdc(), rcItem.left, rcItem.top, SRCCOPY);
		dibobj->Fill(rcItem, 0, 0);
		IRenderDC renderDC;
		renderDC.SetDevice(dibobj);
		renderDC.SetEnableAlpha(true);
		bool bLastViewVisible = IsVisible();
		bool bLastInterVisible = IsInternVisible();
		bool bLastVisible = IsVisible();
		GetWindow()->LockUpdate();
		SetVisible(true);
		GetWindow()->UnLockUpdate();
	
		// GDI+ 使用TextRenderingHintClearTypeGridFit等 绘制字体时，在透明背景下有问题，故强制采用TextRenderingHintAntiAlias
		UITYPE_FONT eRawType = GetUIRes()->GetDefaultFontType();
		GetUIRes()->SetDefaultFontType(UIFONT_GDIPLUS);
		GetUIRes()->SetDefaultTextRenderingHint(TextRenderingHintAntiAlias);
		Render(&renderDC, rcItem);
		GetUIRes()->SetDefaultTextRenderingHint(TextRenderingHintSystemDefault);
		GetUIRes()->SetDefaultFontType(eRawType);
		GetWindow()->LockUpdate();
		SetVisible(bLastViewVisible);
		SetInternVisible(bLastInterVisible);
		SetVisible(bLastVisible);
		GetWindow()->UnLockUpdate();

		DibObj dibobjTemp;
		dibobjTemp.Create(dibobj->GetSafeHdc(), rcOffset.right - rcOffset.left, rcOffset.bottom - rcOffset.top);
		::BitBlt(dibobjTemp.GetSafeHdc(), 0, 0, rcOffset.right - rcOffset.left, rcOffset.bottom - rcOffset.top, dibobj->GetSafeHdc(), rcItem.left, rcItem.top, SRCCOPY);

		BYTE* imageSrc = static_cast<BYTE*>(malloc(dibobjTemp.GetSize() * sizeof(BYTE)));

		if (imageSrc == NULL)
			return NULL;

		::CopyMemory(imageSrc, dibobjTemp.GetBits(), dibobjTemp.GetSize());
		ImageObj* pImage = new ImageObj(imageSrc, rcOffset.right - rcOffset.left, rcOffset.bottom - rcOffset.top);

		::BitBlt(dibobj->GetSafeHdc(), rcItem.left, rcItem.top, rcOffset.right - rcOffset.left, rcOffset.bottom - rcOffset.top, dibobjTempBackup.GetSafeHdc(), 0, 0, SRCCOPY);
		return pImage;
	}
	return NULL;
}

void CControlUI::OnSize(const RECT& rectParent)
{
	__super::OnSize(rectParent);
	// 101ppt新增的不确定代码
	if (!m_bOnSizeCallback)
	{
		m_bOnSizeCallback = true;
		TEventUI event = { 0 };
		event.nType = UIEVENT_SIZE;
		SendEvent(event);
		m_bOnSizeCallback = false;
	}
	
}

bool CControlUI::IsExceptionGroup()
{
	return m_bExceptGroup;
}

IUIControlDestory* CControlUI::GetControlDestoryNotify()
{
	return m_pUIControlDestory;
}

//////////////////////////////////////////////////////////////////////////
// 通用处理函数

SIZE UICustomControlHelper::LayoutChildNormal(RECT& rcWinow, RECT& rcClient, CStdPtrArray& items, bool bEnableScroll /*= true*/)
{
	int maxRight = min(rcClient.left, 0);   // rcClient的l/r为负数时，lpRectChild的l/r可能为负数
	int maxBottom = min(rcClient.top, 0);   // rcClient的t/b为负数时, lpRectChild的t/b可能为负数
	LPRECT lpRectChild;
	CControlUI* pControl = NULL;
	int nCnt = items.GetSize();
	for( int i = 0;  i < nCnt; i++ )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;

		if (!pControl->IsNeedScroll())
		{
			pControl->OnSize(rcWinow);
		}
		else
		{
			pControl->OnSize(rcClient);
			if (bEnableScroll)
			{
				lpRectChild  = pControl->GetRectPtr();
				if (lpRectChild->bottom > maxBottom)
					maxBottom = lpRectChild->bottom;
				if (lpRectChild->right > maxRight)
					maxRight = lpRectChild->right;
			}
		}
	}

	SIZE sz;
	sz.cx = maxRight - rcClient.left; 
	sz.cy = maxBottom - rcClient.top;
	return sz;	
}



SIZE UICustomControlHelper::LayoutChildLeftToRight( RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& items )
{
	int nBegin = params.nBegin_;
	int nEnd   = params.nEnd_;
	if (nEnd == -1)
		nEnd = items.GetSize();
	SIZE sz = {0, 0};
	if (items.GetSize() == 0)
		return sz;
	int maxRight = INT_MIN;
	int minLeft = INT_MAX;
	int maxBottom = INT_MIN;
	int nWidth = 0;
	int nHeight = 0;
	RECT rcChild;
	CControlUI* pControl = NULL;
	int nStartX = rcClient.left + params.nStartX_;
	int nStartY = params.nStartY_;
	for( int i = nBegin;  i < nEnd; i++ )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;
		pControl->OnSize(rcClient);
		if (pControl->IsNeedScroll())
		{
			rcChild = pControl->GetRect();
			nWidth = rcChild.right - rcChild.left;
			nHeight = rcChild.bottom - rcChild.top;
			rcChild.top = rcChild.top + nStartY;
			//rcChild.bottom = rcChild.top + nHeight;
			rcChild.left = nStartX;
			rcChild.right = rcChild.left + nWidth;

			if (nHeight == 0)
			{
				rcChild.bottom = rcClient.bottom;
			}
			else
			{
				rcChild.bottom = rcChild.top + nHeight;

			}

			// 防止多次SizeToContent计算
			bool isSizeToContent = pControl->IsSizeWithContent();
			pControl->SetSizeWithContent(false);
			pControl->SetRect(rcChild);
			pControl->SetSizeWithContent(isSizeToContent);
			nStartX = rcChild.right + params.nSpaceX_;
		}
		if (params.bEnableScroll_)
		{
			rcChild  = pControl->GetRect();
			if (rcChild.bottom > maxBottom)
				maxBottom = rcChild.bottom;
		
			if (rcChild.left < minLeft)
				minLeft = rcChild.left;

			if (rcChild.right > maxRight)
				maxRight = rcChild.right;
			
		
		}
	}
	if ((params.bEnableScroll_) && maxRight != INT_MIN && minLeft != INT_MAX && maxBottom != INT_MIN)
	{
		sz.cx = maxRight - minLeft;
		sz.cy = maxBottom - rcClient.top;
	}
	else
	{
		sz.cx = rcClient.right - rcClient.left; 
		sz.cy = rcClient.bottom - rcClient.top;
	}
	return sz;
}

SIZE UICustomControlHelper::LayoutChildRightToLeft( RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& items )
{
	int nBegin = params.nBegin_;
	int nEnd   = params.nEnd_;
	if (nEnd == -1)
		nEnd = items.GetSize();
	SIZE sz = {0, 0};
	if (items.GetSize() == 0)
		return sz;
	int maxRight = INT_MIN;
	int maxBottom = INT_MIN;
	int nWidth = 0;
	int nHeight = 0;
	RECT rcChild;
	CControlUI* pControl = NULL;
	int nStartX =  rcClient.right - params.nStartX_;
	int nStartY = params.nStartY_;
	for( int i = nEnd-1;  i >= nBegin ; i-- )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;
		pControl->OnSize(rcClient);
		if (pControl->IsNeedScroll())
		{
			rcChild = pControl->GetRect();
			nWidth = rcChild.right - rcChild.left;
			nHeight = rcChild.bottom - rcChild.top;
			rcChild.top = rcChild.top + nStartY;
			//rcChild.bottom = rcChild.top + nHeight;
			rcChild.left = nStartX - nWidth;
			rcChild.right = nStartX;

			if (nHeight == 0)
			{
				rcChild.bottom = rcClient.bottom;
			}
			else
			{
				rcChild.bottom = rcChild.top + nHeight;

			}

			pControl->SetRect(rcChild);
			nStartX = rcChild.left - params.nSpaceX_;
		}
		if (params.bEnableScroll_)
		{
			rcChild  = pControl->GetRect();
			if (rcChild.bottom > maxBottom)
				maxBottom = rcChild.bottom;
			if (rcChild.right > maxRight)
				maxRight = rcChild.right;
		}
	}
	if ((params.bEnableScroll_) && maxRight != INT_MIN && maxBottom != INT_MIN)
	{
		sz.cx = maxRight - rcClient.left; 
		sz.cy = maxBottom - rcClient.top;
	}
	else
	{
		sz.cx = rcClient.right - rcClient.left; 
		sz.cy = rcClient.bottom - rcClient.top;
	}
	return sz;
}

SIZE UICustomControlHelper::LayoutChildTopToBottom( RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& items )
{
	int nBegin = params.nBegin_;
	int nEnd   = params.nEnd_;
	if (nEnd == -1)
		nEnd = items.GetSize();
	SIZE sz = {0, 0};
	if (items.GetSize() == 0)
		return sz;
	int maxRight = INT_MIN;
	int maxBottom = INT_MIN;
	int nWidth = 0;
	int nHeight = 0;
	RECT rcChild;
	CControlUI* pControl = NULL;
	int nStartX = params.nStartX_;
	int nStartY = rcClient.top + params.nStartY_;
	for( int i = nBegin;  i < nEnd; i++ )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;
		pControl->OnSize(rcClient);
		if (pControl->IsNeedScroll())
		{

			if (GetUIEngine()->IsRTLLayout())// 因为onsize中已经调用setrect,下面又调用setrect，调用两次了
			{
				pControl->FlipRect();
			}

			rcChild = pControl->GetRect();
			nWidth = rcChild.right - rcChild.left;
			nHeight = rcChild.bottom - rcChild.top;
			rcChild.top = nStartY;
			rcChild.bottom = rcChild.top + nHeight;

			rcChild.left = rcChild.left + nStartX;
			/*rcChild.right = rcChild.left + nWidth;*/
			if (nWidth == 0)
			{
				rcChild.right = rcClient.right;
			}
			else
			{
				rcChild.right =  rcChild.left + nWidth;
			}


			pControl->SetRect(rcChild);
			//update the child rect, if it was autofited
			rcChild = pControl->GetRect();
			nStartY = rcChild.bottom + params.nSpaceY_;
		}
		if (params.bEnableScroll_)
		{
			rcChild  = pControl->GetRect();
			if (rcChild.bottom > maxBottom)
				maxBottom = rcChild.bottom;
			if (rcChild.right > maxRight)
				maxRight = rcChild.right;
		}
	}
	if ((params.bEnableScroll_) && maxRight != INT_MIN && maxBottom != INT_MIN)
	{
		sz.cx = maxRight - rcClient.left; 
		sz.cy = maxBottom - rcClient.top;
	}
	else
	{
		sz.cx = rcClient.right - rcClient.left; 
		sz.cy = rcClient.bottom - rcClient.top;
	}
	return sz;
}

SIZE UICustomControlHelper::LayoutChildBottomToTop( RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& items )
{
	int nBegin = params.nBegin_;
	int nEnd   = params.nEnd_;
	if (nEnd == -1)
		nEnd = items.GetSize();
	SIZE sz = {0, 0};
	if (items.GetSize() == 0)
		return sz;
	int maxRight = INT_MIN;
	int maxBottom = INT_MIN;
	int nWidth = 0;
	int nHeight = 0;
	RECT rcChild;
	CControlUI* pControl = NULL;
	int nStartX =  params.nStartX_;
	int nStartY =  rcClient.bottom - params.nStartY_;
	for( int i = nEnd-1;  i >= nBegin ; i-- )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;
		pControl->OnSize(rcClient);
		if (pControl->IsNeedScroll())
		{

			if (GetUIEngine()->IsRTLLayout())// 因为onsize中已经调用setrect,下面又调用setrect，调用两次了
			{
				pControl->FlipRect();
			}

			rcChild = pControl->GetRect();
			nWidth = rcChild.right - rcChild.left;
			nHeight = rcChild.bottom - rcChild.top;
			rcChild.top = nStartY - nHeight;
			rcChild.bottom = nStartY;
			rcChild.left = rcChild.left + nStartX;
			//rcChild.right = rcChild.left + nWidth;

			if (nWidth == 0)
			{
				rcChild.right = rcClient.right;
			}
			else
			{
				rcChild.right =  rcChild.left + nWidth;
			}

			pControl->SetRect(rcChild);
			nStartY = rcChild.top - params.nSpaceY_;
		}
		if (params.bEnableScroll_)
		{
			rcChild  = pControl->GetRect();
			if (rcChild.bottom > maxBottom)
				maxBottom = rcChild.bottom;
			if (rcChild.right > maxRight)
				maxRight = rcChild.right;
		}
	}
	if ((params.bEnableScroll_) && maxRight != INT_MIN && maxBottom != INT_MIN)
	{
		sz.cx = maxRight - rcClient.left; 
		sz.cy = maxBottom - rcClient.top;
	}
	else
	{
		sz.cx = rcClient.right - rcClient.left; 
		sz.cy = rcClient.bottom - rcClient.top;
	}
	return sz;
}


SIZE UICustomControlHelper::LayoutChildHorz( RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& items )
{
	int nBegin = params.nBegin_;
	int nEnd   = params.nEnd_;
	if (nEnd == -1)
		nEnd = items.GetSize();
	SIZE sz = {0, 0};
	if (items.GetSize() == 0)
		return sz;
	CControlUI* pControl = NULL;
	RECT rcChild;
	int nCount = 0;
	int nAdjust = 0;
	int cxFix = 0;
	for( int i = nBegin;  i < nEnd; i++ )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;
		if (pControl->IsNeedScroll())
		{
			//pControl->OnSize(rcClient);
			nCount++;
			if (pControl->IsAutoSize() && !pControl->IsSizeWithContent())
			{
				nAdjust++;
			}
			else
			{
				pControl->OnSize(rcClient);
				rcChild  = pControl->GetRect();
				cxFix += rcChild.right - rcChild.left;
			}
		}
		else
			pControl->OnSize(rcWinow);
	}

	int maxRight = INT_MIN;
	int minLeft = rcClient.right;
	int maxBottom = INT_MIN;
	if (nAdjust == 0)
		nAdjust = 1;

	int nTotalAdjust = rcClient.right - rcClient.left - cxFix - params.nStartX_ - ((nCount-1)*params.nSpaceX_);
	int nCellWidth = (int)(floor((double)nTotalAdjust / (double)nAdjust));
	if (nCellWidth <= 1)
		nCellWidth = 1;
	int nTotalAdjustLeave = nTotalAdjust - nCellWidth*nAdjust;
	if (nTotalAdjustLeave < 0)
		nTotalAdjustLeave = 0;
	double nStartX = rcClient.left + params.nStartX_;
	double nStartY = params.nStartY_;

	for( int i = nBegin;  i < nEnd; i++ )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;
		if (pControl->IsNeedScroll())
		{
			if (pControl->IsAutoSize() && !pControl->IsSizeWithContent())
			{
				rcChild =  pControl->GetCoordinate()->rect;
				DPI_SCALE(&rcChild);
				GetAnchorPos(pControl->GetCoordinate()->anchor, &rcClient, &rcChild);
				int nHeight = rcChild.bottom - rcChild.top;

				rcChild.left = (LONG)nStartX;
				rcChild.right = rcChild.left + (LONG)nCellWidth;
				rcChild.top = rcChild.top + (LONG)nStartY;
				
				
				if (rcChild.top == rcChild.bottom)
				{
					rcChild.bottom = rcClient.bottom;
				}
				else
				{
					rcChild.bottom = rcChild.top + nHeight;
				}
				if (nTotalAdjustLeave > 0)
				{
					rcChild.right++;
					nTotalAdjustLeave--;
				}

				pControl->SetRect(rcChild);
				nStartX += rcChild.right - rcChild.left + params.nSpaceX_;
			}
			else
			{
				rcChild = pControl->GetRect();
				int nWidth = rcChild.right - rcChild.left;
				int nHeight = rcChild.bottom - rcChild.top;
				rcChild.left = (LONG)nStartX;
				rcChild.right = rcChild.left + (LONG)nWidth;
				rcChild.top = rcChild.top + (LONG)nStartY;
				if (rcChild.top == rcChild.bottom)
				{
					rcChild.bottom = rcClient.bottom;
				}
				else
				{
					rcChild.bottom = rcChild.top + nHeight;
				}
				pControl->SetRect(rcChild);
				nStartX += nWidth + params.nSpaceX_;
			}
		}
		if (params.bEnableScroll_)
		{
			rcChild  = pControl->GetRect();
			if (rcChild.bottom > maxBottom)
				maxBottom = rcChild.bottom;
			if (rcChild.right > maxRight)
				maxRight = rcChild.right;
			if (rcChild.left < minLeft)
				minLeft = rcChild.left;
		}
	}
	if ((params.bEnableScroll_) && maxRight != INT_MIN && maxBottom != INT_MIN)
	{
		sz.cx = maxRight - minLeft;
		sz.cy = maxBottom - rcClient.top;
	}
	else
	{
		sz.cx = rcClient.right - rcClient.left; 
		sz.cy = rcClient.bottom - rcClient.top;
	}
	return sz;
}

SIZE UICustomControlHelper::LayoutChildVert( RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& items )
{
	int nBegin = params.nBegin_;
	int nEnd   = params.nEnd_;
	if (nEnd == -1)
		nEnd = items.GetSize();
	SIZE sz = {0, 0};
	if (items.GetSize() == 0)
		return sz;
	CControlUI* pControl = NULL;
	RECT rcChild;
	int nCount = 0;
	int nAdjust = 0;
	int cyFix = 0;
	for( int i = nBegin;  i < nEnd; i++ )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;
		if (pControl->IsNeedScroll())
		{
			//pControl->OnSize(rcClient);
			nCount++;
			if (pControl->IsAutoSize())
			{
				nAdjust++;
			}
			else
			{
				pControl->OnSize(rcClient);
				rcChild  = pControl->GetRect();
				cyFix += rcChild.bottom - rcChild.top;
			}
		}
		else
			pControl->OnSize(rcWinow);

	}
	if (nCount == 0)
	{
		sz.cx = 0;
		sz.cy = 0;
		return sz;
	}
	int maxRight = INT_MIN;
	int maxBottom = INT_MIN;
	if (nAdjust == 0)
		nAdjust = 1;
	int nTotalAdjust = rcClient.bottom - rcClient.top - params.nStartY_ - cyFix - ((nCount-1)*params.nSpaceY_);
	int nCellHeight = (int)(floor((double)nTotalAdjust / (double)nAdjust));
	if (nCellHeight <= 1.0)
		nCellHeight = 1;
	int nTotalAdjustLeave = nTotalAdjust - nCellHeight * nAdjust;
	if (nTotalAdjustLeave < 0)
		nTotalAdjustLeave = 0;
	int nStartX = params.nStartX_;
	double nStartY = rcClient.top + params.nStartY_;
	int nWidth = 0;
	int nHeight = 0;
	for( int i = nBegin;  i < nEnd; i++ )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;

		if (pControl->IsNeedScroll())
		{

			if (GetUIEngine()->IsRTLLayout())// 因为onsize中已经调用setrect,下面又调用setrect，调用两次了
			{
				pControl->FlipRect();
			}

			rcChild  = pControl->GetRect();
			nWidth = rcChild.right - rcChild.left;
			nHeight = rcChild.bottom - rcChild.top;
			if (pControl->IsAutoSize())
			{
				rcChild =  pControl->GetCoordinate()->rect;
				DPI_SCALE(&rcChild);
				GetAnchorPos(pControl->GetCoordinate()->anchor, &rcClient, &rcChild);
				int nWidth = rcChild.right - rcChild.left;
				//int nHeight = rcChild.bottom - rcChild.top;

				rcChild.left = rcChild.left + nStartX;
				if (rcChild.left == rcChild.right)
				{
					rcChild.right = rcClient.right;
				}
				else
				{
					rcChild.right = rcChild.left + nWidth;
				}
				rcChild.top = (LONG)nStartY;
				rcChild.bottom = rcChild.top + nCellHeight;
				if (nTotalAdjustLeave > 0)
				{
					rcChild.bottom++;
					nTotalAdjustLeave--;
				}
				pControl->SetRect(rcChild);
				nStartY += rcChild.bottom - rcChild.top + params.nSpaceY_;
			}
			else
			{

				rcChild.left = rcChild.left + nStartX;
				if (rcChild.left == rcChild.right)
				{
					rcChild.right = rcClient.right;
				}
				else
				{
					rcChild.right = rcChild.left + nWidth;
				}

				rcChild.top = (LONG)nStartY;
				rcChild.bottom = rcChild.top + nHeight;
				pControl->SetRect(rcChild);

				nStartY += nHeight + params.nSpaceY_;
			}
		}
		if (params.bEnableScroll_)
		{
			rcChild  = pControl->GetRect();
			if (rcChild.bottom > maxBottom)
				maxBottom = rcChild.bottom;
			if (rcChild.right > maxRight)
				maxRight = rcChild.right;
		}
	}
	if ((params.bEnableScroll_) && maxRight != INT_MIN && maxBottom != INT_MIN)
	{
		sz.cx = maxRight - rcClient.left; 
		sz.cy = maxBottom - rcClient.top;
	}
	else
	{
		sz.cx = rcClient.right - rcClient.left; 
		sz.cy = rcClient.bottom - rcClient.top;
	}
	return sz;
}

SIZE UICustomControlHelper::LayoutChildGrid( RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& items )
{
	int nBegin = params.nBegin_;
	int nEnd   = params.nEnd_;
	if (nEnd == -1)
		nEnd = items.GetSize();
	SIZE sz = {0, 0};
	if (items.GetSize() == 0)
		return sz;
	CControlUI* pControl = NULL;
	RECT rcChild;
	int nStartX = rcClient.left + params.nStartX_;
	int nStartY = rcClient.top + params.nStartY_;
	int nMaxX = rcClient.right;
	int nMaxY = nStartY;
	int nWidth = 0;
	int nHeight = 0;
	int maxRight = INT_MIN;
	int maxBottom = INT_MIN;
	for( int i = nBegin;  i < nEnd; i++ )
	{
		pControl = static_cast<CControlUI*>(items.GetAt(i));
		if (!pControl)
			continue;
		if (!pControl->IsVisible())
			continue;
		pControl->OnSize(rcClient);
		if (pControl->IsNeedScroll())
		{
			rcChild  = pControl->GetRect();
			nWidth = rcChild.right - rcChild.left;
			nHeight = rcChild.bottom - rcChild.top;
			if (nStartX + nWidth > nMaxX)
			{
				nStartX = rcClient.left + params.nStartX_;
				nStartY = nMaxY + params.nSpaceY_;
			}
			rcChild.left = nStartX;
			rcChild.top = nStartY;
			rcChild.right = nStartX + nWidth;
			rcChild.bottom = nStartY + nHeight;
			pControl->SetRect(rcChild);
			nMaxY = nMaxY>rcChild.bottom?nMaxY:rcChild.bottom;
			nStartX = rcChild.right + params.nSpaceX_;
		}
		if (params.bEnableScroll_)
		{
			rcChild  = pControl->GetRect();
			if (rcChild.bottom > maxBottom)
				maxBottom = rcChild.bottom;
			if (rcChild.right > maxRight)
				maxRight = rcChild.right;
		}
	}
	if ((params.bEnableScroll_) && maxRight != INT_MIN && maxBottom != INT_MIN)
	{
		sz.cx = maxRight - rcClient.left; 
		sz.cy = maxBottom - rcClient.top;
	}
	else
	{
		sz.cx = rcClient.right - rcClient.left; 
		sz.cy = rcClient.bottom - rcClient.top;
	}
	return sz;
}


void UICustomControlHelper::RenderChildControl( IRenderDC* pRenderDC, RECT& rcPaint, RECT& rcParentItem, RECT& rcParentClient, CStdPtrArray& items, int nBegin /*= 0*/, int nEnd /*= -1*/)
{
	CControlUI* pControl = NULL;
	HDC hDestDC = pRenderDC->GetDC();
	int nCnt = nEnd;
	if (nEnd == -1) nCnt = items.GetSize();

	int nZOrders = 0;
	pControl = static_cast<CControlUI*>(items.GetAt(nBegin)); 
	if (pControl)
	{
		CControlUI* pParent = pControl->GetParent();
		if (pParent)
			nZOrders = pParent->m_nZOrders;
	}

	ClipObj clip;
	ClipObj::GenerateClip(hDestDC, rcParentItem, clip);

	RECT rcTemp = {0};
	if(::IntersectRect(&rcTemp, &rcPaint, &rcParentClient))
	{
		//在客户区中
	/*	ClipObj childClip;
		ClipObj::GenerateClip(hDestDC, rcTemp, childClip);*/
		int nZIndex = 0;
		while (nZIndex <= nZOrders)
		{
			for( int i = nBegin;  i < nCnt; i++ )
			{
				pControl = static_cast<CControlUI*>(items.GetAt(i)); 
				if (!pControl->IsVisible())
					continue;
				//不在该轴不绘制
				if (pControl->GetZOrder() != nZIndex)
					continue;

				if (pControl->IsNeedScroll())
				{
					if( !::IntersectRect(&rcTemp, &rcPaint, pControl->GetRectPtr())) 
						continue;
					if( !::IntersectRect(&rcTemp, &rcParentClient, &rcTemp)) 
						continue;

					ClipObj childClip;
					ClipObj::GenerateClip(hDestDC, rcTemp, childClip);

					pControl->Render(pRenderDC, rcTemp);
				}
				else
				{
					//ClipObj::UseOldClipBegin(hDestDC, childClip);

					if( !::IntersectRect(&rcTemp, &rcPaint, pControl->GetRectPtr())) 
						continue;
					pControl->Render(pRenderDC, rcTemp);
					
					//ClipObj::UseOldClipEnd(hDestDC, childClip);
				}
			}
			nZIndex++;
		}
	}
	else
	{
		//非刷新区域
		for( int i = nBegin;  i < nCnt; i++ )
		{
			pControl = static_cast<CControlUI*>(items.GetAt(i));
			if (!pControl->IsVisible() || pControl->IsNeedScroll())
				continue;

			if( !::IntersectRect(&rcTemp, &rcParentItem, pControl->GetRectPtr())) 
				continue;

			if( !::IntersectRect(&rcTemp, &rcPaint, pControl->GetRectPtr())) 
				continue;

			pControl->Render(pRenderDC, rcTemp);
		}
	}
}

void UICustomControlHelper::RenderChildControl(IRenderDC* pRenderDC, RECT& rcPaint, RECT& rcParentItem, RECT& rcParentClient, CControlUI* pControlChild)
{
	if (!pControlChild || !pControlChild->IsVisible())
		return;

	RECT rcTemp = {0};
	if( !::IntersectRect(&rcTemp, &rcPaint, pControlChild->GetRectPtr())) 
		return;

	if (pControlChild->IsNeedScroll())
	{
		if( !::IntersectRect(&rcTemp, &rcParentClient, &rcTemp)) 
			return;

		ClipObj clip;
		ClipObj::GenerateClip(pRenderDC->GetDC(), rcParentClient, clip);

		pControlChild->Render(pRenderDC, rcTemp);

	}
	else
	{
		if( !::IntersectRect(&rcTemp, &rcParentItem, &rcTemp)) 
			return;

		ClipObj clip;
		ClipObj::GenerateClip(pRenderDC->GetDC(), rcParentItem, clip);

		pControlChild->Render(pRenderDC, rcTemp);
	}

}

bool UICustomControlHelper::AddChildControl( CControlUI* pControl, CControlUI* pParent, CStdPtrArray& items )
{
	if (pControl == NULL) return false;

	ZeroMemory(pControl->GetRectPtr(), sizeof(RECT));//pControl->SetRect(rcEmpty, false);// xqb去掉SetRect重载
	int nCount = items.GetSize();
	items.Add(pControl);
	pControl->SetIndex(nCount);
	pControl->SetManager(pParent->GetWindow(), pParent);

	if (pControl->GetINotifyUI() == NULL && pParent->GetINotifyUI())
		pControl->SetINotifyUI(pParent->GetINotifyUI());


	if (pParent->EventToParent())
		pControl->EventToParent(pParent->EventToParent());

    bool bParentVisible = pParent->IsVisible();
    bParentVisible = bParentVisible && pParent->IsInternVisible();
    pControl->SetInternVisible(bParentVisible);

	TEventUI event = {0};
	event.nType = UIEVENT_ADD_CHILD;
	event.wParam = (WPARAM)pControl;
	pParent->SendEvent(event);
	
	return true;
}

bool UICustomControlHelper::InsertChildControl( CControlUI* pControl, CControlUI* pAfter, CControlUI* pParent, CStdPtrArray& items )
{
	if (pControl == NULL) return false;
	ZeroMemory(pControl->GetRectPtr(), sizeof(RECT));//pControl->SetRect(rcEmpty, false);// xqb去掉SetRect重载
	int nIndex = 0;if (pAfter)	nIndex = pAfter->GetIndex() + 1;
	pControl->SetIndex(nIndex);
	pControl->SetManager(pParent->GetWindow(), pParent);
	items.InsertAt(nIndex, pControl);

	//CControlUI* pSub = NULL;
	for (int i = nIndex + 1; i < items.GetSize(); i++)
	{
		CControlUI* pSub = pParent->GetItem(i);
		pSub->SetIndex(i);
	}

	if (pControl->GetINotifyUI() == NULL && pParent->GetINotifyUI())
		pControl->SetINotifyUI(pParent->GetINotifyUI());


	if (pParent->EventToParent())
		pControl->EventToParent(pParent->EventToParent());

    bool bParentVisible = pParent->IsVisible();
    bParentVisible = bParentVisible && pParent->IsInternVisible();
    pControl->SetInternVisible(bParentVisible);

	TEventUI event = {0};
	event.nType = UIEVENT_ADD_CHILD;
	event.wParam = (WPARAM)pControl;
	pParent->SendEvent(event);

	return true;
}

bool UICustomControlHelper::RemoveChildControl(CControlUI* pControl,  CStdPtrArray& items )
{
	if (!pControl)
		return false;
	int i = -1;
	for( i = 0;  i < items.GetSize(); i++ )
	{
		if( items[i] == pControl ) 
		{
			pControl->OnDestroy();
			delete pControl;
			break;

		}
	}
	for (int j = i + 1; j < items.GetSize(); j++)
		((CControlUI*)items.GetAt(j))->SetIndex(j - 1);
	return items.Remove(i);
}

void UICustomControlHelper::RemoveAllChild(CStdPtrArray& items )
{
	int nCnt = items.GetSize();
	for( int i = 0;  i < nCnt; i++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(items[i]);
		if (!pControl)
			continue;
		pControl->OnDestroy();
		delete pControl;
	}
	items.Empty();
}


CControlUI* UICustomControlHelper::FindChildControl( CStdPtrArray& items, FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, int nBegin /*= 0*/, int nEnd /*= -1*/ )
{
	CControlUI* pControl = NULL;
	int nCnt = nEnd;

	//List控件，Item索引控制有问题，这里统一校验
	int nItemCount = items.GetSize();
	if (nEnd == -1)
	{
		nCnt = nItemCount;
	}
	else
	{
		nCnt = min(nItemCount, nEnd);
	}

	if( (uFlags & UIFIND_TOP_FIRST) != 0 )
	{
		for( int it = nCnt - 1;  it >= nBegin; it-- )
		{
			pControl = static_cast<CControlUI*>(items[it]);
			if ( NULL == pControl ) continue;
			
			pControl = pControl->FindControl(Proc, pData, uFlags);
			if( pControl != NULL)
				return pControl;
		}
	}
	else
	{
		for( int it = nBegin;  it < nCnt; it++ )
		{
			pControl = static_cast<CControlUI*>(items[it]);
			pControl = pControl->FindControl(Proc, pData, uFlags);
			if( pControl != NULL)
				return pControl;
		}
	}
	return NULL;
}

void UICustomControlHelper::AddEventDelegate(CControlUI* pControl, CDelegateBase& d)
{
	pControl->OnEvent += d;
	for (int i = 0; i < pControl->GetCount(); i++)
	{
		UICustomControlHelper::AddEventDelegate(pControl->GetItem(i), d);
	}

}