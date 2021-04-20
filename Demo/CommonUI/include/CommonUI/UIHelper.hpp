#pragma once
#include <commctrl.h>


namespace ui_common{

	static bool IsChildControl(CControlUI* pControl, CControlUI* pChild)
	{
		if (!pChild)
		{
			return false;
		}

		CControlUI* pParent = pChild->GetParent();
		while (pParent)
		{
			if (pParent == pControl)
			{
				return true;
			}

			pParent = pParent->GetParent();
		}

		return false;
	}

	static void HideOtherCtrl(CContainerUI* pCtn, LPCTSTR lpszCtrl)
	{
		if (!pCtn)
		{
			return;
		}

		CControlUI* pCtrl = NULL;
		int nCount = pCtn->GetCount();
		for (int i = 0; i < nCount; ++i)
		{
			pCtrl = pCtn->GetItem(i);
			if (NULL == pCtrl) return;

			if (0 == _tcscmp(lpszCtrl, pCtrl->GetId()))
			{
				continue;
			}

			if (pCtrl->IsVisible())
			{
				pCtrl->SetVisible(false);
			}
		}
	}

	static HWND FindPopupWnd(HWND hOwner)
	{
		HWND hPopup = ::GetWindow(hOwner, GW_HWNDPREV);
		HWND hTopPopup = hOwner;

		for (;
			hPopup;
			hPopup = ::GetWindow(hPopup, GW_HWNDPREV))
		{
			HWND hOwnerPossible = ::GetWindow(hPopup, GW_OWNER);
			if (hOwner != hOwnerPossible)
			{
				continue;
			}

			DWORD dwStyle = ::GetWindowLong(hPopup, GWL_STYLE);
			if ((dwStyle & WS_VISIBLE) != WS_VISIBLE)
			{
				continue;
			}

			DWORD dwExStyle = ::GetWindowLong(hPopup, GWL_EXSTYLE);
			if ((dwExStyle & WS_EX_NOACTIVATE) == WS_EX_NOACTIVATE)
			{
				continue;
			}

			
			TCHAR szBuf[MAX_CLASS_NAME + 1];
			if (GetClassName(hPopup, szBuf, _countof(szBuf)) != 0)
			{
				if (_tcscmp(szBuf, TOOLTIPS_CLASS) == 0)
				{
					continue;
				}
			}

			hTopPopup = hPopup;
		}


		return hTopPopup;
	}

	static HWND GetTopPopupWnd(HWND h)
	{
		HWND hPopup = h;
		HWND hTmp;
		while ((hTmp = FindPopupWnd(hPopup))
			&& hTmp != hPopup)
		{
			hPopup = hTmp;
		}

		return hPopup;
	}

	static void ActiveWindow(HWND hWnd)
	{
		if (!hWnd)
		{
			return;
		}

		if (!IsWindow(hWnd))
		{
			return;
		}

		if (IsIconic(hWnd))
		{
			::ShowWindow(hWnd, SW_RESTORE);
			::SetForegroundWindow(hWnd);
		}
		else
		{
			HWND hFrgWnd = ::GetForegroundWindow();
			AttachThreadInput(GetWindowThreadProcessId(hFrgWnd, NULL), GetCurrentThreadId(), TRUE);
			::SetForegroundWindow(hWnd);
			AttachThreadInput(GetWindowThreadProcessId(hFrgWnd, NULL), GetCurrentThreadId(), FALSE);
		}
	}
}


