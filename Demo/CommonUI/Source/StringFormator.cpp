#include "stdafx.h"
#include "StringFormator.h"

CStringFormator::CStringFormator(LPCTSTR lpctOriginal)
{
	if (lpctOriginal)
	{
		m_strGenerate = lpctOriginal;
	}
}

CStringFormator::~CStringFormator()
{

}

void CStringFormator::Bind(LPCTSTR lpctKey, LPCTSTR lpctValue)
{
	do
	{
		if (!lpctKey
			|| !lpctValue
			|| m_strGenerate.empty())
		{
			break;
		}

		if (_tcsicmp(lpctKey, _T("")) == 0
			|| _tcsicmp(lpctValue, _T("")) == 0)
		{
			break;
		}

		size_t npos = m_strGenerate.find(lpctKey);
		if (npos == tstring::npos)
		{
			break;
		}

		m_strGenerate.replace(npos, _tcslen(lpctKey), lpctValue);
	} while (false);
}

void CStringFormator::Bind(LPCTSTR lpctKey, DWORD dwValue)
{
	TCHAR sz[256];
	_stprintf_s(sz, _countof(sz) - 1, _T("%lu"), dwValue);
	Bind(lpctKey, sz);
}

LPCTSTR CStringFormator::Str()
{
	return m_strGenerate.c_str();
}

