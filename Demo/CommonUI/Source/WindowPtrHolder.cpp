#include "stdafx.h"
#include "WindowPtrHolder.h"
#include "WindowPtrMap.h"
#include "WindowPtrMapImpl.h"

CWindowPtrHolder::CWindowPtrHolder()
{
	m_spWndMap = std::make_shared<CWindowPtrMapImpl>();
}

CWindowPtrHolder::~CWindowPtrHolder()
{

}

CWindowUI* CWindowPtrHolder::FindWindowPtr(void* pIdentify)
{
	if (!m_spWndMap)
	{
		return NULL;
	}

	return m_spWndMap->FindWnd(pIdentify);
}

void CWindowPtrHolder::AddWindowPtr(void* pIdentify, CWindowUI* pWnd)
{
	if (!m_spWndMap)
	{
		return;
	}

	m_spWndMap->AddWnd(pIdentify, pWnd);
}

void CWindowPtrHolder::RemoveWindowPtr(void* pIdentify)
{
	if (!m_spWndMap)
	{
		return;
	}

	m_spWndMap->RemoveWnd(pIdentify);
}

std::shared_ptr<IWindowPtrMap> CWindowPtrHolder::GetWindowPtrMap()
{
	return m_spWndMap;
}
