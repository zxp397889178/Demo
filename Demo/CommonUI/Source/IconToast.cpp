#include "stdafx.h"
#include "IconToast.h"
#include "..\Source\EngineObj.h"

#define TIMER_HIDE_ID 1

CIconToast::CIconToast()
{
}

CIconToast::~CIconToast()
{

}

void CIconToast::Init()
{
    __super::Init();

    CControlUI* pControl = FindSubControl(_T("ctl_toast"));
    if (pControl)
        pControl->SetMouseEnabled(false);
}

void CIconToast::SetToastMsg(LPCTSTR lpszMsg, LPCTSTR lpszIcon /*= _T("")*/, UINT uElapse /*= 2000*/)
{
	bool bRet = SetToastMsgShow(lpszMsg, lpszIcon);
	if (bRet)
	{
		SetTimer(TIMER_HIDE_ID, uElapse);
	}	
}

bool CIconToast::SetToastMsgShow(LPCTSTR lpszMsg, LPCTSTR lpszIcon /*= _T("")*/)
{
	bool bRet = false;	

	do
	{
		if (!lpszMsg
			|| !lpszIcon)
			break;

		if (!IsInternVisible())
			break;

		SetVisible(true);

		CControlUI* pIcon = FindSubControl(_T("ctl_icon"));
		if (pIcon
			&& _tcsicmp(lpszIcon, _T("")) != 0)
		{
			pIcon->SetAttribute(_T("bk.image"), lpszIcon);
		}

		CControlUI* pControl = FindSubControl(_T("ctl_toast"));
		if (!pControl)
			break;

		pControl->SetAttribute(_T("text"), lpszMsg);

		OnlyResizeChild();
		
		bRet = true;

	} while (false);

	KillTimer(TIMER_HIDE_ID);

	return bRet;
}

bool CIconToast::Event(TEventUI& event)
{
    if (event.nType == UIEVENT_TIMER)
    {
        SetVisible(false);
        KillTimer(TIMER_HIDE_ID);
    }
    __super::Event(event);
    return true;
}
