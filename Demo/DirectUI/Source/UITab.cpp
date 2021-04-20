/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UITab.h"

//////////////////////////////////////////////////////////////////////////
CTabButtonUI::CTabButtonUI(void)
{
	m_bShowClose = true;
	SetEnabled(true);
	SetAnchor(UIANCHOR_LEFTTOP);
	SetStyle(_T("TabButton"));
	SetId(_T("TabButton"));
	Select(false);
	m_nCloseState = UISTATE_NORMAL;
	m_pBkStyle = NULL;
	m_pIconStyle = NULL;
	m_pTextStyle = NULL;
	m_pCloseStyle = NULL;
}

CTabButtonUI::~CTabButtonUI(void)
{

}

StyleObj* CTabButtonUI::GetBkStyle()
{
	if (!m_pBkStyle)
		m_pBkStyle = GetStyle(_T("bk"));
	return m_pBkStyle;

}

StyleObj* CTabButtonUI::GetIconStyle()
{
	if (!m_pIconStyle)
		m_pIconStyle = GetStyle(_T("icon"));
	return m_pIconStyle;

}

StyleObj* CTabButtonUI::GetTextStyle()
{
	if (!m_pTextStyle)
		m_pTextStyle = GetStyle(_T("txt"));
	return m_pTextStyle;
}

StyleObj* CTabButtonUI::GetCloseStyle()
{
	if (!m_pCloseStyle)
		m_pCloseStyle = GetStyle(_T("close"));
	return m_pCloseStyle;
}

void CTabButtonUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	//HDC hDestDC = pRenderDC->GetDC();
	StyleObj* pStyle = NULL;
	pStyle = GetBkStyle();
	if (pStyle)
	{
		pStyle->SetState(GetState());
	}

	pStyle = GetIconStyle();
	if (pStyle)
	{
		pStyle->SetState(IsSelected()?UISTATE_FOCUS:GetState());
	}

	pStyle = GetTextStyle();
	if (pStyle)
	{
		pStyle->SetState(GetState());
	}

	pStyle = GetCloseStyle();
	if (pStyle && IsShowClose())
	{
		pStyle->SetState(m_nCloseState);
	}
	__super::Render(pRenderDC, rcPaint);
}

void CTabButtonUI::ShowClose(bool bShow)
{
	m_bShowClose = bShow;
	StyleObj* pStyle = GetCloseStyle();
	if (pStyle)
		pStyle->SetVisible(bShow);
}

void CTabButtonUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("ShowExt")))
		m_bShowClose = (bool)!!_ttoi(lpszValue);
	else
		__super::SetAttribute(lpszName, lpszValue);
}

#define __Default_Tab_Left_ DPI_SCALE(70)

CTabHeaderUI::CTabHeaderUI()
{
	SetAttribute(_T("mode"), _T("horzfill"));
	m_pCurSel = NULL;
	SetStyle(_T("TabHeader"));
	SetId(_T("TabHeader"));
}

CTabHeaderUI::~CTabHeaderUI()
{
}


bool CTabHeaderUI::SelectItem(CControlUI* pControl)
{
	CTabUI* pTab = dynamic_cast<CTabUI*>(GetParent());
	if (!pTab) return false;

	if( pControl == m_pCurSel ) return true;
	if( m_pCurSel) 
	{
		m_pCurSel->Select(false);
		m_pCurSel->SetState(UISTATE_NORMAL);
		pTab->GetItem(m_pCurSel->GetIndex())->SetVisible(false);
		m_pCurSel = NULL;
	}

	if (pControl != NULL)
	{
		m_pCurSel = pControl;
		m_pCurSel->Select(true);
		m_pCurSel->SetState(UISTATE_DOWN);
		pTab->GetItem(m_pCurSel->GetIndex())->SetVisible(true);
		
	}
	this->Invalidate(true);
	return true;
}
void CTabHeaderUI::RemoveAll()
{
	m_pCurSel = NULL;
	__super::RemoveAll();
}
bool CTabHeaderUI::Remove(CControlUI* pControl)
{
	if (m_pCurSel == pControl)
		m_pCurSel = NULL;
	return __super::Remove(pControl);
}

bool CTabHeaderUI::Add(CControlUI* pControl)
{
	pControl->OnEvent += MakeDelegate(this, &CTabHeaderUI::ChildEvent);
	return __super::Add(pControl);
}

bool CTabHeaderUI::ChildEvent(TEventUI& event)
{
	CControlUI* pButton = event.pSender;
	if (pButton == NULL)
		return false;
	CWindowUI* pManager = pButton->GetWindow();
	CTabUI* pTab = NULL;
	if (pButton->GetParent())
		pTab = (CTabUI*)(pButton->GetParent()->GetParent());
	int nIndex = pButton->GetIndex();
	switch (event.nType)
	{
	case UIEVENT_BUTTONUP:
		{
			if (pButton->IsClass(_T("TabButton")))
			{
				CTabButtonUI* pTabButton = dynamic_cast<CTabButtonUI*>(pButton);
				if (pTabButton->IsShowClose() && pTabButton->GetStyle())
				{
					StyleObj* pStyle = pTabButton->GetCloseStyle();
					if (pStyle && pStyle->IsVisible())
					{
						RECT rcStyle = pStyle->GetRect();
						if (PtInRect(&rcStyle, event.ptMouse))
						{
							if (pTab)
								pTab->SendNotify(UINOTIFY_TAB_CLOSE, 0, (LPARAM)pTab->GetItem(nIndex));
							break;
						}
					}
				}
			}
			if (pTab)
			{
				CControlUI* pPage = pTab->GetItem(pButton->GetIndex());
				if (pPage)
					dynamic_cast<CTabUI*>(pTab)->SelectItem(pPage->GetIndex());
			}
		}
		break;
	
	case UIEVENT_BUTTONDOWN:
		{
			bool bUpdate = false;
			if (pButton->IsClass(_T("TabButton")))
			{
				CTabButtonUI* pTabButton = dynamic_cast<CTabButtonUI*>(pButton);
				if (pTabButton && pTabButton->GetStyle() && pTabButton->IsShowClose())
				{
					StyleObj* pStyle = pTabButton->GetCloseStyle();
					if (pStyle && pStyle->IsVisible())
					{
						RECT rcStyle = pStyle->GetRect();
						if (PtInRect(&rcStyle, event.ptMouse))
						{
							if (pTabButton->GetCloseState() != UISTATE_DOWN)
							{
								pTabButton->SetCloseState(UISTATE_DOWN);
								bUpdate = true;
							}
						}
						else
						{
							if (pTabButton->GetCloseState() != UISTATE_NORMAL)
							{
								pTabButton->SetCloseState(UISTATE_NORMAL);
								bUpdate = true;
							}
						}
					}
				}
			}
			if (pButton->GetState() != UISTATE_DOWN)
			{
				pButton->SetState(UISTATE_DOWN);
				bUpdate = true;
			}
			if (bUpdate)
				pButton->Invalidate();

		}
		break;

	case UIEVENT_MOUSEMOVE:
		{
			bool bUpdate = false;
			if (pButton->IsClass(_T("TabButton")))
			{
				CTabButtonUI* pTabButton = dynamic_cast<CTabButtonUI*>(pButton);
				if (pTabButton && pTabButton->GetStyle() && pTabButton->IsShowClose())
				{
					StyleObj* pStyle = pTabButton->GetCloseStyle();
					if (pStyle && pStyle->IsVisible())
					{
						RECT rcStyle = pStyle->GetRect();
						if (PtInRect(&rcStyle, event.ptMouse))
						{
							if (pTabButton->GetCloseState() != UISTATE_OVER)
							{
								pTabButton->SetCloseState(UISTATE_OVER);
								bUpdate = true;
							}
						}
						else
						{
							if (pTabButton->GetCloseState() != UISTATE_NORMAL)
							{
								pTabButton->SetCloseState(UISTATE_NORMAL);
								bUpdate = true;
							}
						}
					}
				}
			}
			if (bUpdate)
				pButton->Invalidate();

		}
		break;
	case UIEVENT_MOUSEENTER:
		{
			bool bUpdate = false;
			if (pButton->IsClass(_T("TabButton")))
			{
				CTabButtonUI* pTabButton = dynamic_cast<CTabButtonUI*>(pButton);
				if (pTabButton && pTabButton->GetStyle() && pTabButton->IsShowClose())
				{
					StyleObj* pStyle = pTabButton->GetCloseStyle();
					if (pStyle && pStyle->IsVisible())
					{
						RECT rcStyle = pStyle->GetRect();
						if (PtInRect(&rcStyle, event.ptMouse))
						{
							if (pTabButton->GetCloseState() != UISTATE_OVER)
							{
								pTabButton->SetCloseState(UISTATE_OVER);
								bUpdate = true;
							}
						}
						else
						{
							if (pTabButton->GetCloseState() != UISTATE_NORMAL)
							{
								pTabButton->SetCloseState(UISTATE_NORMAL);
								bUpdate = true;
							}
						}
					}
				}
			}
			if (!pButton->IsSelected() && pButton->GetState() != UISTATE_OVER)
			{
				pButton->SetState(UISTATE_OVER);
				bUpdate = true;
			}
			if (bUpdate)
				pButton->Invalidate();

		}
		break;

	case UIEVENT_MOUSELEAVE:
		{
			if (pButton->IsSelected())
				pButton->SetState(UISTATE_DOWN);
			else
				pButton->SetState(UISTATE_NORMAL);
			if (pButton->IsClass(_T("TabButton")))
			{
				CTabButtonUI* pTabButton = dynamic_cast<CTabButtonUI*>(pButton);
				if (pTabButton)
					pTabButton->SetCloseState(UISTATE_NORMAL);
			}
			pButton->Invalidate();

		}
		break;

	case UIEVENT_KILLFOCUS:
		if (pButton->IsClass(_T("TabButton")))
			pButton->CControlUI::Event(event);
		else
			pButton->Event(event);
		break;
	default:
		return true;
	}
	event.nType = UIEVENT_UNUSED;
	return true;
}
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CTabUI::CTabUI(void)
{
	SetEnabled(true);
	SetStyle(_T("TabCtrl"));
	m_pHeaderList = NULL;
	m_bHeaderHide = false;
	m_bHeaderVisible = true;
	m_szCurSel = _T("");

	m_cdHeader.rect.left = 0;
	m_cdHeader.rect.top = 0;
	m_cdHeader.rect.right = 0;
	m_cdHeader.rect.bottom = 24;
	m_cdHeader.anchor = UIANCHOR_LTRT;

	m_cdBody.rect.left = 0;
	m_cdBody.rect.top = 24;
	m_cdBody.rect.right = 0;
	m_cdBody.rect.bottom = 0;
	m_cdBody.anchor = UIANCHOR_LTRB;

	m_nMaxWidth = 0;
	m_nMinWidth = 0;

	if (m_pHeaderList == NULL)
	{
		m_pHeaderList = new CTabHeaderUI;
		m_pHeaderList->SetCoordinate(m_cdHeader);
	}
}

CTabUI::~CTabUI(void)
{
	if (m_pHeaderList)
		delete m_pHeaderList;
	m_pHeaderList = NULL;
}

bool CTabUI::SelectItem(int nIndex)
{
	if (!m_pHeaderList)
		return false;
	int nLastIndex = GetCurSel();
	CControlUI* pControl = GetItem(nIndex);
	if (pControl)
	{
		m_pHeaderList->SelectItem(m_pHeaderList->GetItem(pControl->GetIndex()));

		if (!IsRectEmpty(GetRectPtr()))
		{
			this->Resize();
			this->Invalidate();
		}

		if (pControl)
			this->SendNotify(UINOTIFY_SELCHANGE, (WPARAM)pControl->GetIndex(), (LPARAM)nLastIndex);
		return true;
	}
	return false;
}

bool CTabUI::SelectItem(LPCTSTR szID)
{
	if (!m_pHeaderList)
		return false;
	CControlUI* pControl = GetItem(szID);
	if (pControl)
	{
		SelectItem(pControl->GetIndex());
		return true;
	}
	return false;
}

int  CTabUI::GetCurSel()
{
	if (m_pHeaderList->GetCurSel())
	{
		return m_pHeaderList->GetCurSel()->GetIndex();
	}
	return -1;
}

void CTabUI::Init()
{
	 __super::Init();

	 if (m_pHeaderList)
	 {
		 m_pHeaderList->SetManager(GetWindow(), this);
		 m_pHeaderList->Init();
	 }
	
	 if (!m_szCurSel.empty())
	 {
		 int nCnt = GetCount();
		// CControlUI* pControl = NULL;
		 for (int i = 0; i < nCnt; i++)
		 {
			  CControlUI* pControl = GetItem(i);
			 if (m_szCurSel != pControl->GetId())
				 pControl->SetVisible(false);
			 else
				 pControl->SetVisible(true);
		 }
	 }
	 if (!m_szCurSel.empty())
		SelectItem(m_szCurSel.c_str());
}

bool CTabUI::SetTabHeaderVisible(LPCTSTR lpszPageID, bool bVisible/* = true*/)
{
	CControlUI* pControl = GetItem(lpszPageID);
	if (pControl)
	{
		pControl = m_pHeaderList==NULL?NULL:m_pHeaderList->GetItem(pControl->GetIndex());
		if (pControl)
		{
			pControl->SetVisible(bVisible);
			return true;
		}
	}
	return false;
}

bool CTabUI::IsHeaderItemVisible(int nIndex)
{
	CControlUI* pControl = m_pHeaderList==NULL?NULL:m_pHeaderList->GetItem(nIndex);
	if (pControl)
		return pControl->IsVisible();
	return false;
}

bool CTabUI::SetTabHeaderVisible(int nIndex, bool bVisible/* = true*/)
{
	CControlUI* pControl = m_pHeaderList==NULL?NULL:m_pHeaderList->GetItem(nIndex);
	if (pControl)
	{
		pControl->SetVisible(bVisible);
		return true;
	}
	return false;
}

void CTabUI::SetRect(RECT& rectRegion)
{
	if (m_pHeaderList == NULL)
	{
		__super::SetRect(rectRegion);
		return;
	}

	CControlUI* pControl = NULL;
	int nVisible = 0;
	for (int i = 0; i < m_pHeaderList->GetCount(); i++)
	{
		pControl = m_pHeaderList->GetItem(i);
		if (pControl)
			nVisible += pControl->IsVisible();
	}
	if (!m_bHeaderVisible || (nVisible<=1 && !m_bHeaderHide))
	{
		m_pHeaderList->SetCoordinate(0,0,0,0,UIANCHOR_LEFTTOP);
		RECT rcInset = {0, 0, 0, 0};
		SetInset(rcInset);
	}
	else
	{
		m_pHeaderList->SetCoordinate(m_cdHeader);
		int nCount = GetCount();
		for (int i = 0; i < nCount; i++)
		{
			GetItem(i)->SetCoordinate(m_cdBody);
		}
	}

	__super::SetRect(rectRegion);
	{
		if (m_nMaxWidth || m_nMinWidth)
		{
			int nHeaderItems = m_pHeaderList->GetCount();
			RECT rcCalc = {0,0,0,0};
			int nWidth = 0;
			FontObj* pFont = NULL;
			for (int i = 0; i < nHeaderItems; i++)
			{
				rcCalc.left = rcCalc.right = rcCalc.top = rcCalc.bottom = 0;
				pControl = m_pHeaderList->GetItem(i);
				if (pControl)
				{
					TextStyle* pText = pControl->GetTextStyle(_T("txt"));
					if (pText)
						pFont = pText->GetFontObj();
					else
						pFont = GetUIRes()->GetDefaultFont();
					if (pFont)
						pFont->CalcText(GetWindow()->GetPaintDC(), rcCalc, pControl->GetText());
					nWidth = rcCalc.right + 21;//预留关闭按钮的位置
					pControl->SetCoordinate(0, 0, max(min(m_nMaxWidth, nWidth), m_nMinWidth), 0, UIANCHOR_LTLB);
				}
			}
		}
		RECT rc = GetRect();
		m_pHeaderList->OnSize(rc);
	}
}
 
bool CTabUI::Add(CControlUI* pControl)
{
	if (pControl->IsClass(_T("TabButton")))
	{
		m_pHeaderList->Add(pControl);
		return true;
	}
	else if (pControl->IsClass(_T("TabHeader")))
	{
		if (m_pHeaderList)
			delete m_pHeaderList;
		m_pHeaderList = NULL;
		m_pHeaderList = static_cast<CTabHeaderUI*>(pControl);
		if (m_pHeaderList)
		{
			m_pHeaderList->SetManager(GetWindow(), this);
			m_pHeaderList->Init();
			return true;
		}			
	}
	else
	{
		
		bool bRet =  __super::Add(pControl);
		if (m_pHeaderList->GetCount() < GetCount())
		{
			CTabButtonUI* pTabButton = new CTabButtonUI;
			m_pHeaderList->Add(pTabButton);
			pTabButton->SetText(pControl->GetText());
			pTabButton->SetAutoSize(pControl->IsAutoSize());
			int l,t,r,b;
			pControl->GetCoordinate(l,t,r,b);
			pTabButton->SetCoordinate(l,0,r,0,UIANCHOR_LTLB);
			pControl->SetCoordinate(0, 0, 0, 0, UIANCHOR_LTRB);
		}
		return bRet;
	}

	return false;
 }

 bool CTabUI::Remove(CControlUI* pControl)
 {
	 if (m_pHeaderList == NULL || pControl == NULL)
		 return false;
	 if (pControl)
	 {
		 if (m_pHeaderList)
		 {
			 CControlUI* pButton = m_pHeaderList->GetItem(pControl->GetIndex());
			 if (m_pHeaderList && pButton)
				 m_pHeaderList->Remove(pButton);
		 }
		 __super::Remove(pControl);
	 }
	 return true;
 }

void CTabUI::RemoveAll()
{
	if (m_pHeaderList)
		m_pHeaderList->RemoveAll();
	__super::RemoveAll();
}

void CTabUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("headerVisible")))
		m_bHeaderVisible = (bool)!!_ttoi(lpszValue);
	else if (equal_icmp(lpszName, _T("HeaderHeight")))
		m_cdHeader.rect.bottom = _ttoi(lpszValue);
	else if (equal_icmp(lpszName, _T("body_pos")))
	{
		int l, t, r, b;
		UITYPE_ANCHOR anchor;
		PrasePosString(lpszValue, l, t, r, b, anchor);
		SetBodyInitPos(l, t, r, b, anchor);
	}
	else if (equal_icmp(lpszName, _T("header_pos")))
	{
		int l, t, r, b;
		UITYPE_ANCHOR anchor;
		PrasePosString(lpszValue, l, t, r, b, anchor);
		SetHeaderInitPos(l, t, r, b, anchor);
	}
	else if (equal_icmp(lpszName, _T("MaxWidth")))
		m_nMaxWidth = _ttoi(lpszValue);
	else if (equal_icmp(lpszName, _T("MinWidth")))
		m_nMinWidth = _ttoi(lpszValue);
	else if (equal_icmp(lpszName, _T("HeaderAutoHide")))
		m_bHeaderHide = (bool)!!_ttoi(lpszValue);
	else if (equal_icmp(lpszName, _T("Sel")))
		m_szCurSel = lpszValue;
	else
		__super::SetAttribute(lpszName, lpszValue);
}

CControlUI* CTabUI::GetHeader()
{
	return m_pHeaderList;
}


void CTabUI::SetHeaderInitPos(int l, int t, int r, int b, UITYPE_ANCHOR a)
{
	m_cdHeader.rect.left = l;
	m_cdHeader.rect.top = t;
	m_cdHeader.rect.right = r;
	m_cdHeader.rect.bottom = b;
	m_cdHeader.anchor = a;
}


void CTabUI::SetBodyInitPos(int l, int t, int r, int b, UITYPE_ANCHOR a)
{
	m_cdBody.rect.left = l;
	m_cdBody.rect.top = t;
	m_cdBody.rect.right = r;
	m_cdBody.rect.bottom = b;
	m_cdBody.anchor = a;
}

void CTabUI::SetTabHeaderAttribute(LPCTSTR lpszPageID, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (m_pHeaderList == NULL || lpszPageID == NULL)
		return;
	CControlUI* pControl = GetItem(lpszPageID);
	if (pControl)
	{
		int nCnt = pControl->GetIndex();
		pControl = m_pHeaderList->GetItem(nCnt);
		pControl->SetAttribute(lpszName, lpszValue);
	}
}

void CTabUI::SetTabHeaderAttribute(int nIndex, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (m_pHeaderList == NULL)
		return;
	CControlUI* pControl = GetItem(nIndex);
	if (pControl)
	{
		pControl = m_pHeaderList->GetItem(nIndex);
		pControl->SetAttribute(lpszName, lpszValue);
	}
}

void  CTabUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	HDC hDestDC = pRenderDC->GetDC();
	__super::Render(pRenderDC, rcPaint);
	RECT rect = GetRect();

	RECT rcTemp = { 0 };
	if( m_pHeaderList != NULL && m_pHeaderList->IsVisible() ) 
	{
		RECT rcChild = m_pHeaderList->GetRect();
		if(::IntersectRect(&rcTemp, &rcPaint, &rcChild)) 
		{
			ClipObj childClip;
			ClipObj::GenerateClip(hDestDC, rcTemp, childClip);
			m_pHeaderList->Render(pRenderDC, rcPaint);
		}		
	}
	CControlUI::Render(pRenderDC, rcPaint);
}

CControlUI* CTabUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	CControlUI* pControl = __super::FindControl(Proc, pData, uFlags);
	if (pControl != NULL)
		return pControl;

	if (m_pHeaderList && m_pHeaderList->IsVisible())
		pControl = m_pHeaderList->FindControl(Proc, pData, uFlags);
	return pControl;
}

void  CTabUI::OnLanguageChange()
{
	__super::OnLanguageChange();
	if (m_pHeaderList) ((CControlUI*)m_pHeaderList)->OnLanguageChange();
}