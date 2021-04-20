#include "stdafx.h"
#include "Toast.h"
#include "..\Source\EngineObj.h"

#define TIMER_HIDE_ID 1

#define MIN_WIDTH   240
#define MIN_HEIGHT  90

#define MAX_WIDTH   320
#define MAX_HEIGHT  120

CToast::CToast()
{
	m_iMinWidth = MIN_WIDTH;
	m_iMinHeight = MIN_HEIGHT;
	m_iMaxWidth = MAX_WIDTH;
	m_iMaxHeight = MAX_HEIGHT;
}

CToast::~CToast()
{

}

void CToast::Init()
{
    __super::Init();

    CControlUI* pControl = FindSubControl(_T("ctl_toast"));
    if (pControl)
        pControl->SetMouseEnabled(false);
}

void CToast::SetToastMsg(const tstring& wstrMsg)
{
	if (wstrMsg.empty())
		return;
	
    if (!IsInternVisible())
        return;
   
    SetCoordinate(-1, -1, MIN_WIDTH, MIN_HEIGHT);
    SetVisible(true);
    Resize();
    do 
    {
        CControlUI* pControl = FindSubControl(_T("ctl_toast"));
        if (!pControl)
            break;

        TextStyle *pTextStyle = pControl->GetTextStyle(_T("txt"));
        if (!pTextStyle)
            break;

        pTextStyle->SetAttribute(_T("singleline"), _T("1"));
        pControl->SetAttribute(L"text", wstrMsg.c_str());
		pControl->Resize();

        //文字较长时，增大toast框
        {
            pTextStyle->SetAttribute(_T("endellipsis"), _T("0"));
            RECT rectBtn = pTextStyle->GetRect();
            RECT rect = rectBtn;
            OffsetRect(&rectBtn, -rectBtn.left, -rectBtn.top);
            pTextStyle->GetFontObj()->CalcText(GetWindow()->GetPaintDC(), rectBtn, wstrMsg.c_str(), pTextStyle->GetAlign());
            int nOffset = (rectBtn.right - rectBtn.left) - (rect.right - rect.left);
            if (nOffset > 0)
            {
				SetCoordinate(-1, -1, m_iMaxWidth, m_iMaxHeight);
                Resize();
            }
            pTextStyle->SetAttribute(_T("endellipsis"), _T("1"));
        }

        pTextStyle->SetAttribute(_T("singleline"), _T("0"));

        if (GetParent())
            GetParent()->Invalidate();

        KillTimer(TIMER_HIDE_ID);
        SetTimer(TIMER_HIDE_ID, 2000);

    } while (false);
}

void CToast::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (!lpszName || !lpszValue)
		return;

	if (_tcsicmp(lpszName, _T("max_width")) == 0)
	{
		m_iMaxWidth = _ttoi(lpszValue);
	}
	else if (_tcsicmp(lpszName, _T("max_height")) == 0)
	{
		m_iMaxHeight = _ttoi(lpszValue);
	}
	else if (_tcsicmp(lpszName, _T("min_width")) == 0)
	{
		m_iMinWidth = _ttoi(lpszValue);
	}
	else if (_tcsicmp(lpszName, _T("min_height")) == 0)
	{
		m_iMinHeight = _ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

bool CToast::Event(TEventUI& event)
{
    if (event.nType == UIEVENT_TIMER)
    {
        SetVisible(false);
        KillTimer(TIMER_HIDE_ID);
    }
    __super::Event(event);
    return true;
}

void CToast::SetRect(RECT& rectRegion)
{

    CControlUI* pControl = FindSubControl(_T("ctl_toast"));
    TextStyle *pTextStyle = nullptr;
    if (pControl)
        pTextStyle = pControl->GetTextStyle(_T("txt"));

    if (pTextStyle)
    {
        pTextStyle->SetAttribute(_T("endellipsis"), _T("0"));
        pTextStyle->SetAttribute(_T("singleline"), _T("1"));
    }

    __super::SetRect(rectRegion);

    if (pTextStyle)
    {
        pTextStyle->SetAttribute(_T("endellipsis"), _T("1"));
        pTextStyle->SetAttribute(_T("singleline"), _T("0"));
    }
}

