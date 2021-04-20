#include "stdafx.h"
#include "WindowPtrMap.h"
#include "WindowPtrRef.h"
#include "UIIndentifyWnd.h"


CIdentifyWnd::CIdentifyWnd()
{
	
}

CIdentifyWnd::~CIdentifyWnd(void)
{

}

void CIdentifyWnd::OnCreate()
{
	__super::OnCreate();

	std::shared_ptr<IWindowPtrMap> spWndMap = GetWindowPtrMap();
	if (spWndMap)
	{
		spWndMap->AddWnd(GetIdentifyWnd(), this);
	}
}

void CIdentifyWnd::SetIdentifyReference(HWND hIdentifyWnd, std::shared_ptr<IWindowPtrMap> spWndMap)
{
	if (!hIdentifyWnd)
	{
		return;
	}

	SetWindowPtrRef(hIdentifyWnd, spWndMap);

	if (GetHWND()
		&& ::IsWindow(GetHWND()))
	{
		std::shared_ptr<IWindowPtrMap> spWndMap = GetWindowPtrMap();
		if (spWndMap)
		{
			spWndMap->AddWnd(GetIdentifyWnd(), this);
		}
	}
}

HWND CIdentifyWnd::GetIdentifyWnd()
{
	return GetWindowPtrIdentifyAsWnd();
}
