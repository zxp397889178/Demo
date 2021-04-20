#include "stdafx.h"
#include "Event\LoginEvent.hpp"
#include "UcPullDownWnd.h"

CUcPullDownWnd::CUcPullDownWnd()
{
}

CUcPullDownWnd::~CUcPullDownWnd()
{
}

HWND CUcPullDownWnd::CreateWnd(HWND hParent, CLayoutUI *pEdit)
{
	this->SetLayerWindow(true);
	HWND hWnd = this->Create(hParent, _T("PullDownWnd"));

	RECT rtParent = { 0 };
	RECT r;
	//左右两边阴影6
	r = pEdit->FindSubControl(_T("under_line"))->GetRect();
	POINT pt = { r.left - DPI_SCALE(6), r.bottom };
	ClientToScreen(hParent, &pt);
	this->SetWindowPos(NULL, pt.x, pt.y, (r.right - r.left) + DPI_SCALE(6 * 2) , 0, SWP_NOZORDER | SWP_NOACTIVATE);

	//请求下拉框数据
	UcRequestPullDownInfo PdInfo;
	PdInfo.PostInWorkPool();

	return hWnd;
}

void CUcPullDownWnd::OnCreate()
{
	__super::OnCreate();
}

LRESULT CUcPullDownWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KILLFOCUS)
	{
		CloseWindow();
	}
	return __super::WindowProc(message, wParam, lParam);
}

bool CUcPullDownWnd::OnBtnCheckUser(TNotifyUI* pNotify)
{
	CControlUI * pControl = pNotify->pSender;
	if (pControl)
	{
		UcRequestSelectUserInfo selectUserinfo;
		selectUserinfo.wstrUserName = pControl->GetText();
		selectUserinfo.hwnd = GetParent(GetHWND());
		selectUserinfo.PostInWorkPool();

		CloseWindow();
	}

	return true;
}

bool CUcPullDownWnd::OnBtnCheckDel(TNotifyUI* pNotify)
{
	CControlUI * pControl = pNotify->pSender;
	if (pControl)
	{
		CButtonUI *pButton = dynamic_cast<CButtonUI*>((dynamic_cast<CContainerUI*>(pControl->GetParent()))->FindSubControl(_T("pd_user")));
		if (pButton)
		{
			UcRequestDelUserInfo delUserInfo;
			delUserInfo.wstrUserName = pButton->GetText();
			delUserInfo.PostInWorkPool();
		}

		CControlUI *pLayout = pControl->GetParent()->GetParent();
		pControl->GetParent()->GetParent()->Remove(pControl->GetParent());
		RECT rect;
		::GetWindowRect(this->GetHWND(), &rect);
		int num = pLayout->GetCount();
		if (num <= 1)
		{
			SetWindowPos(this->GetHWND(), rect.left, rect.top, (rect.right - rect.left), DPI_SCALE(11 + 46), SWP_NOZORDER | SWP_NOACTIVATE);
		}
		else if (num == 2)
		{
			SetWindowPos(this->GetHWND(), rect.left, rect.top, (rect.right - rect.left), DPI_SCALE(11 + 46 * 2), SWP_NOZORDER | SWP_NOACTIVATE);
		}
		else
		{
			SetWindowPos(this->GetHWND(), rect.left, rect.top, (rect.right - rect.left), DPI_SCALE(11 + 46 * 3), SWP_NOZORDER | SWP_NOACTIVATE);
		}

		pLayout->Resize();
		Invalidate();
	}

	return true;
}

void CUcPullDownWnd::OnEvent(CompletePullDownInfo* pEvent)
{
	if (pEvent)
	{
		RECT rect;
		GetWindowRect(this->GetHWND(), &rect);
		POINT pt = { rect.left, rect.bottom };

		if (pEvent->m_vLoginParam.size() <= 1)
		{
			this->SetWindowPos(NULL, pt.x, pt.y, (rect.right - rect.left), DPI_SCALE(11 + 46), SWP_NOZORDER | SWP_NOACTIVATE);
		}
		else if (pEvent->m_vLoginParam.size() == 2)
		{
			this->SetWindowPos(NULL, pt.x, pt.y, (rect.right - rect.left), DPI_SCALE(11 + 46 * 2), SWP_NOZORDER | SWP_NOACTIVATE);
		}
		else
		{
			this->SetWindowPos(NULL, pt.x, pt.y, (rect.right - rect.left), DPI_SCALE(11 + 46 * 3), SWP_NOZORDER | SWP_NOACTIVATE);
		}

		CContainerUI* pLayout = dynamic_cast<CContainerUI*>(GetItem(_T("pulldowm_layout")));
		if (pEvent->m_vLoginParam.size() > 0)
		{
			int iNum = pEvent->m_vLoginParam.size();
			for (size_t i = iNum; i > 0; i--)
			{
				GetUIEngine()->AddControl(pLayout, nullptr, _T("PullDownStyle"));
				CContainerUI* pSubLayout = dynamic_cast<CContainerUI*>(pLayout->GetItem(pLayout->GetCount() - 1));
				if (pSubLayout)
				{
					CContainerUI *pPdLayout = dynamic_cast<CContainerUI *>(pSubLayout->FindSubControl(_T("pd_layout")));
					if (pPdLayout)
					{
						CButtonUI *pButton = dynamic_cast<CButtonUI *>(pPdLayout->FindSubControl(_T("pd_user")));
						if (pButton)
						{
							pButton->SetAttribute(_T("text"), pEvent->m_vLoginParam[i - 1].strUserName.c_str());
							pButton->SetAttribute(_T("over_txt.text"), pEvent->m_vLoginParam[i - 1].strUserName.c_str());
							pButton->SetAttribute(_T("push_txt.text"), pEvent->m_vLoginParam[i - 1].strUserName.c_str());
						}
					}
				}
			}
		}
	}
}
