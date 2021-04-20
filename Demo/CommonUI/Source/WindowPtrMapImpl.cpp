#include "stdafx.h"
#include "WindowPtrMap.h"
#include "WindowPtrMapImpl.h"

CWindowPtrMapImpl::CWindowPtrMapImpl()
{
	m_pWndMap = new_nothrow WindowMap;
}

CWindowPtrMapImpl::~CWindowPtrMapImpl()
{
	if (m_pWndMap)
	{
		m_pWndMap->clear();

		delete m_pWndMap;
		m_pWndMap = NULL;
	}
}

CWindowUI* CWindowPtrMapImpl::FindWnd(void* hWnd)
{
	if (!hWnd)
	{
		return NULL;
	}	
	
	TCHAR szKey[9] = { 0 };
	_sntprintf_s(szKey, _countof(szKey) - 1, _T("%08x"), hWnd);

	return FindWnd(szKey);
}

CWindowUI* CWindowPtrMapImpl::FindWnd(LPCTSTR lptcsKey)
{
	if (!lptcsKey
		|| _tcsicmp(lptcsKey, _T("")) == 0)
	{
		return NULL;
	}

	if (!m_pWndMap)
	{
		return NULL;
	}

	CWindowUI* pWnd = NULL;
	WindowMap::iterator itrFind = m_pWndMap->find(lptcsKey);
	if (itrFind != m_pWndMap->end())
	{
		return itrFind->second;
	}

	return NULL;
}

void CWindowPtrMapImpl::AddWnd(void* hWnd, CWindowUI* pWnd)
{
	if (!hWnd)
	{
		return;
	}
	
	TCHAR szKey[9] = { 0 };
	_sntprintf_s(szKey, _countof(szKey) - 1, _T("%08x"), hWnd);

	AddWnd(szKey, pWnd);
}

void CWindowPtrMapImpl::AddWnd(LPCTSTR lptcsKey, CWindowUI* pWnd)
{
	if (!lptcsKey
		|| _tcsicmp(lptcsKey, _T("")) == 0)
	{
		return;
	}

	CWindowUI* pFindWnd = this->FindWnd(lptcsKey);
	if (pFindWnd)
	{
		return;
	}

	if (!m_pWndMap)
	{
		return;
	}

	m_pWndMap->insert(std::make_pair(lptcsKey, pWnd));
}

void CWindowPtrMapImpl::RemoveWnd(void* hWnd)
{
	if (!hWnd)
	{
		return;
	}

	TCHAR szKey[9] = { 0 };
	_snwprintf_s(szKey, _countof(szKey) - 1, L"%08x", hWnd);

	RemoveWnd(szKey);
}

void CWindowPtrMapImpl::RemoveWnd(LPCTSTR lptcsKey)
{
	if (!lptcsKey
		|| _tcsicmp(lptcsKey, _T("")) == 0)
	{
		return;
	}

	if (!m_pWndMap)
	{
		return;
	}

	WindowMap::iterator itrFind = m_pWndMap->find(lptcsKey);
	if (itrFind != m_pWndMap->end())
	{
		m_pWndMap->erase(itrFind);
	}
}

WindowMap* CWindowPtrMapImpl::GetWindowMap()
{
	return m_pWndMap;
}