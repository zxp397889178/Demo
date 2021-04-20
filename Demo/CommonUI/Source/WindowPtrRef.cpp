#include "stdafx.h"
#include "WindowPtrRef.h"
#include "WindowPtrMap.h"
#include "WindowPtrHolder.h"

CWindowPtrRef::~CWindowPtrRef()
{
	std::shared_ptr<IWindowPtrMap>& spWndMap = GetWindowPtrMap();
	if (spWndMap)
	{
		spWndMap->RemoveWnd(m_strIdentify.c_str());
	}
}

void CWindowPtrRef::SetWindowPtrRef(void* pIdentify, std::shared_ptr<IWindowPtrMap> spWndMap)
{
	TCHAR szKey[9] = { 0 };
	_sntprintf_s(szKey, _countof(szKey) - 1, _T("%08x"), pIdentify);
	m_strIdentify = szKey;
	m_spWndMap    = spWndMap;
}

void CWindowPtrRef::SetWindowPtrRef(void* pIdentify, CWindowUI* pWnd, std::shared_ptr<IWindowPtrMap> spWndMap)
{
	if (!pWnd)
	{
		return;
	}

	SetWindowPtrRef(pIdentify, spWndMap);	

	if (spWndMap)
	{
		spWndMap->AddWnd(m_strIdentify.c_str(), pWnd);
	}
}

void CWindowPtrRef::SetWindowPtrRef(LPCTSTR lpszIdentify, CWindowUI* pWnd, std::shared_ptr<IWindowPtrMap> spWndMap)
{
	if (!pWnd)
	{
		return;
	}

	m_strIdentify = lpszIdentify;
	m_spWndMap    = spWndMap;

	if (spWndMap)
	{
		spWndMap->AddWnd(m_strIdentify.c_str(), pWnd);
	}
}

HWND CWindowPtrRef::GetWindowPtrIdentifyAsWnd()
{	
	DWORD dwValue = _tcstoul(m_strIdentify.c_str(), NULL, 16);
	return (HWND)(dwValue);
}

tstring CWindowPtrRef::GetWindowPtrIdentify()
{	
	return m_strIdentify;
}

std::shared_ptr<IWindowPtrMap> CWindowPtrRef::GetWindowPtrMap()
{
	return m_spWndMap.lock();
}
