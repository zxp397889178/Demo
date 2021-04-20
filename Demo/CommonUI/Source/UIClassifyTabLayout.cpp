#include "stdafx.h"

#include "UIClassifyTabLayout.h"


CClassifyTabLayoutUI::CClassifyTabLayoutUI() :m_pBtnTabLeft(nullptr), m_pBtnTabRight(nullptr)
{
	m_pOptionLayout = new COptionLayoutUI();
	m_pOptionLayout->SetAttribute(_T("mode"), _T("horzfill"));
	TCHAR szBuf[1024];
	_stprintf_s(szBuf, _countof(szBuf), _T("LTRB|0,0,%d,0"), (51));
	m_pOptionLayout->SetAttribute(_T("pos"), szBuf);
	m_pOptionLayout->SetSpaceX((34));
	m_pOptionLayout->SetAttribute(_T("HScroll"), _T("1"));
	m_pOptionLayout->SetAttribute(_T("showhscroll"), _T("0"));
	m_pOptionLayout->OnEvent += MakeDelegate(this, &CClassifyTabLayoutUI::OnOptionLayoutEvent);
	m_pOptionLayout->OnNotify += MakeDelegate(this, &CClassifyTabLayoutUI::OnOptionLayoutNotify);
	m_pOptionLayout->OnEvent += MakeDelegate(this, &CClassifyTabLayoutUI::OnScrollPosChange);

	__super::Add(m_pOptionLayout);

	m_pTabControlLayout = new CLayoutUI();
	_stprintf_s(szBuf, _countof(szBuf), _T("RTRB|%d,%d,0,%d"), (101), (4), (4));
	m_pTabControlLayout->SetAttribute(_T("pos"), szBuf);
	__super::Add(m_pTabControlLayout);
}

CClassifyTabLayoutUI::~CClassifyTabLayoutUI()
{

}

bool CClassifyTabLayoutUI::Add(CControlUI* pControl)
{
	if (m_pOptionLayout)
	{
		return m_pOptionLayout->Add(pControl);
	}
	
	return true;
}

void CClassifyTabLayoutUI::Init()
{
	__super::Init();

	GetUIEngine()->AddControl(m_pTabControlLayout, nullptr, _T("Frame_b_tab_control"));

	m_pBtnTabLeft = dynamic_cast<CButtonUI *>(FindSubControl(_T("FrameB_Classify_Tab_Left")));
	if (m_pBtnTabLeft)
	{
		m_pBtnTabLeft->OnNotify += MakeDelegate(this, &CClassifyTabLayoutUI::OnTabLeftNotify);
	}
	m_pBtnTabRight = dynamic_cast<CButtonUI *>(FindSubControl(_T("FrameB_Classify_Tab_Right")));
	if (m_pBtnTabRight)
	{
		m_pBtnTabRight->OnNotify += MakeDelegate(this, &CClassifyTabLayoutUI::OnTabRightNotify);
	}
	
}

COptionLayoutUI * CClassifyTabLayoutUI::GetOptionLayout()
{
	return m_pOptionLayout;
}

CLayoutUI * CClassifyTabLayoutUI::GetControlLayout()
{
	return m_pTabControlLayout;
}

bool CClassifyTabLayoutUI::SelectItem(CControlUI* pControl)
{
	return m_pOptionLayout->SelectItem(pControl);
}

void CClassifyTabLayoutUI::SelectItem(int iIndex)
{
    if (m_pOptionLayout 
        && m_pOptionLayout->GetCount()>iIndex)
    {
        CControlUI* pItem = m_pOptionLayout->GetItem(iIndex);
        m_pOptionLayout->SelectItem(pItem);
    }
}

CControlUI* CClassifyTabLayoutUI::GetCurSel()
{
	return m_pOptionLayout->GetCurSel();
}

CControlUI* CClassifyTabLayoutUI::GetAssocicatecurSel()
{
	return m_pOptionLayout->GetAssocicatecurSel();
}

CControlUI* CClassifyTabLayoutUI::GetAssocicate()
{
	return m_pOptionLayout->GetAssocicate();
}

bool CClassifyTabLayoutUI::OnTabLeftNotify(TNotifyUI& msg)
{
	if (msg.nType == UINOTIFY_CLICK)
	{
		Resize();
		m_pOptionLayout->GetHScrollbar()->PageUp();
		UpdateTabButton();
	}
	return true;
}

bool CClassifyTabLayoutUI::OnTabRightNotify(TNotifyUI& msg)
{
	if (msg.nType == UINOTIFY_CLICK)
	{
		Resize();		

		m_pOptionLayout->GetHScrollbar()->PageDown();

		UpdateTabButton();
	}
	return true;
}

bool CClassifyTabLayoutUI::OnOptionLayoutEvent(TNotifyUI& msg)
{
	if (!m_pOptionLayout
		|| !m_pTabControlLayout)
	{
		return true;
	}

	if (msg.nType == UIEVENT_SIZE)
	{
		RECT rcItem = m_pOptionLayout->GetRect();
		//m_rcClient = m_pOptionLayout->GetInterRect();
		RECT rcScroll = m_pOptionLayout->GetScrollRect();


		UICustomControlHelper::LayoutParams params;
		params.nStartX_ = DPI_SCALE(m_pOptionLayout->GetStartX());
		params.nStartY_ = DPI_SCALE(m_pOptionLayout->GetStartY());
		params.nSpaceX_ = DPI_SCALE(m_pOptionLayout->GetSpaceX());
		params.nSpaceY_ = DPI_SCALE(m_pOptionLayout->GetSpaceY());
		params.bEnableScroll_ = (m_pOptionLayout->GetHScrollbar() || m_pOptionLayout->GetVScrollbar());
		CStdPtrArray* pItems = m_pOptionLayout->GetItemArray();

		SIZE sz = UICustomControlHelper::LayoutChildHorz(rcItem, rcScroll, params, *pItems);

		auto AdjustScroll = [&]()//大小改变后要对滚动位置做调整，以防选择的项显示不完全
		{
			//GetParent()->Resize();
			CControlUI * pControl = m_pOptionLayout->GetCurSel();
			if (pControl)
			{
				RECT rc = pControl->GetRect();
				//if (rc.right > rcParent.right - rcCoordinate.right)
				//{
				//	m_pOptionLayout->GetHScrollbar()->PageDown();
				//}
				//else if (rc.left < rcParent.left + rcCoordinate.left)
				//{
				//	m_pOptionLayout->GetHScrollbar()->PageUp();
				//}
				UpdateTabButton();
			}
		};

		RECT rc = GetRect();
		if (sz.cx > (rc.right - rc.left))
		{
			m_pTabControlLayout->SetVisible(true);
			TCHAR szBuf[1024];
			_stprintf_s(szBuf, _countof(szBuf), _T("LTRB|0,0,%d,0"), (51));
			m_pOptionLayout->SetAttribute(_T("pos"), szBuf);
			AdjustScroll();
		}
		else
		{
			m_pTabControlLayout->SetVisible(false);
			m_pOptionLayout->SetAttribute(_T("pos"), _T("LTRB|0,0,0,0"));
			AdjustScroll();
		}
		this->OnlyResizeChild();
		this->Invalidate();
	}
	return true;
}

bool CClassifyTabLayoutUI::OnOptionLayoutNotify(TNotifyUI& msg)
{
	if (msg.nType == UINOTIFY_SELCHANGE || msg.nType == UINOTIFY_TRBN_POSCHANGE || msg.nType == UINOTIFY_TRBN_BEGINCHANGE || msg.nType == UINOTIFY_TRBN_ENDCHANGE)
	{
		do 
		{
			auto pControl = m_pOptionLayout->GetCurSel();
			if (!pControl)
			{
				break;
			}
			auto pHScrollbar = m_pOptionLayout->GetHScrollbar();
			if (!pHScrollbar)
			{
				break;
			}
			RECT rcChild = pControl->GetRect();
			RECT rcThis = m_pOptionLayout->GetRect();
			rcThis.right -= DPI_SCALE(10);//要扣除虚化的部分，避免被虚化挡住看不清
			RECT temp = { 0 };
			if (!::IntersectRect(&temp, &rcChild, &rcThis) || temp.left != rcChild.left || temp.right != rcChild.right)
			{
				int nOffset = rcChild.left - rcThis.left;
				pHScrollbar->OffsetScrollPos(nOffset);
				Invalidate();
			}
		} while (false);
		
		UpdateTabButton();
		SendNotify(msg.nType, msg.wParam, msg.lParam);
	}
	
	return true;
}

bool CClassifyTabLayoutUI::OnScrollPosChange(TEventUI &evt)
{
	UpdateTabButton();
	return true;
}

void CClassifyTabLayoutUI::UpdateTabButton()
{
	if (!m_pBtnTabLeft || !m_pBtnTabRight)
	{
		return;
	}


	CScrollbarUI *pScrollUI = m_pOptionLayout->GetHScrollbar();
	if (!pScrollUI)
	{
		return;
	}
	int nScrollPos = pScrollUI->GetScrollPos();
	int nScrollRange = pScrollUI->GetScrollRange();
	bool bEnablePrePage = (nScrollPos != 0);
	bool bEnableNextPage = (nScrollPos != nScrollRange);
	m_pBtnTabLeft->SetEnabled(bEnablePrePage);
	m_pBtnTabRight->SetEnabled(bEnableNextPage);
	ShowShade(bEnableNextPage);
}

LPCTSTR CClassifyTabLayoutUI::GetResourseType()
{
	LPCTSTR lpszResourseType = nullptr;

	if (m_pOptionLayout->GetCurSel())
	{
		CControlUI* pCurSel = m_pOptionLayout->GetCurSel();
		lpszResourseType = pCurSel->GetId();
	}
	return lpszResourseType;
}

void CClassifyTabLayoutUI::ShowShade(bool bShow)
{
	CControlUI * pControl = FindSubControl(_T("FrameB_Classify_Tab_Shade"));
	if (pControl)
	{
		LPCTSTR sPic = bShow ? _T("#frame_b_tab_shade") : _T("");
		pControl->SetAttribute(_T("bk.image"), sPic);
	}
	
}

//int CTabLayoutUI::GetCount()
//{
//	return m_pOptionLayout->GetCount();
//}

//CControlUI* CTabLayoutUI::GetItem(int iIndex)
//{
//	return m_pOptionLayout->GetItem(iIndex);
//}
//
//CControlUI* CTabLayoutUI::GetItem(LPCTSTR lpszId)
//{
//	return m_pOptionLayout->GetItem(lpszId);
//}
//
bool CClassifyTabLayoutUI::Insert(CControlUI* pControlChild, CControlUI* pControlAfter)
{
	return m_pOptionLayout->Insert(pControlChild, pControlAfter);
}

















