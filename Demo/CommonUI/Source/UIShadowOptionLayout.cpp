#include "stdafx.h"
#include "UIShadowOptionLayout.h"

COptionLayoutUIEx::~COptionLayoutUIEx()
{

}

bool COptionLayoutUIEx::Add(CControlUI* pControl)
{
    pControl->OnNotify += MakeDelegate(this, &COptionLayoutUIEx::OnChildNotify);
    return __super::Add(pControl);
}

bool COptionLayoutUIEx::OnChildNotify(TNotifyUI& msg)
{
    if (msg.nType == UINOTIFY_CLICK)
    {
        SelectControl(msg.pSender);
    }
    return true;
}

bool COptionLayoutUIEx::Event(TEventUI& event)
{
    switch (event.nType)
    {
    case UIEVENT_ITEM_SEL_CHANGE:
    {
                                    this->SendNotify(UINOTIFY_SELCHANGE, event.wParam, event.lParam);
    }
        break;
    }
    return __super::Event(event);
}

void CShadowOptionLayoutUI::SetMaxLines(int nMaxLines)
{
	m_nMaxLines = nMaxLines;
}

bool CShadowOptionLayoutUI::Resize()
{
	auto ret = __super::Resize();

	HandleOutOfLines();

	return ret;
}

void CShadowOptionLayoutUI::OnProcessScrollbar(int cxRequired, int cyRequired)
{
	__super::OnProcessScrollbar(cxRequired, cyRequired);
	bool bNeedVScroll = false;
	if (GetVScrollbar() 
		&& GetVScrollbar()->IsVisible())
	{
		bNeedVScroll = true;
	}
	StyleObj* pObj = GetStyle(_T("bkshadow"));
	if (pObj)
	{
		pObj->SetVisible(bNeedVScroll);
		pObj->Resize();
		
	}
}

void CShadowOptionLayoutUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	__super::Render(pRenderDC, rcPaint);
	if (!this->IsVisible())
		return;
	if (GetCount() > 0)
	{
		RECT rcTemp = { 0 };
		RECT rcItem = GetRect();
		RECT rcClient = GetClientRect();
		CControlUI* pParent = GetParent();
		if (pParent)
		{
			::IntersectRect(&rcItem, pParent->GetRectPtr(), GetRectPtr());
			::IntersectRect(&rcClient, &rcItem, &rcClient);
		}
		if( !::IntersectRect(&rcTemp, &rcPaint, &rcItem)) 
			return;

		HDC hDestDC = pRenderDC->GetDC();
		ClipObj clip;
		ClipObj::GenerateClip(hDestDC, rcTemp, clip);
		CControlUI::Render(pRenderDC, rcPaint);	
	}
}

void CShadowOptionLayoutUI::HandleOutOfLines()
{
	if (m_nMaxLines <= 0)
		return;

	auto nCount = GetCount();
	if (nCount <= 0)
		return;

	//简单点，假定内部容器的item都是一样的
	auto rcItem = GetItem(0)->GetRect();
	auto rc = GetRect();
	auto inset = GetInset();

	auto nItemWidth = rcItem.right - rcItem.left;
	auto nWidth = (rc.right - rc.left) - GetStartX() - (inset.right - inset.left);

	if (nItemWidth >= nWidth)
		return;

	//每行能容纳的item数
	auto nNumPerLine = nWidth / (nItemWidth + GetSpaceX());

	if (nNumPerLine > 0 && ((nNumPerLine * (nItemWidth + GetSpaceX()) + nItemWidth) < nWidth))
	{
		++nNumPerLine;

		auto pVScrollbar = GetVScrollbar();

		if (1 == m_nMaxLines && pVScrollbar)
			pVScrollbar->SetVisible(false);
	}

	int nStartIndex = nNumPerLine * m_nMaxLines;

	for (int i = 0; i < nCount; ++i)
	{
		if (auto pItem = GetItem(i - 1))
			pItem->SetVisible(i < nStartIndex);
	}
}