#pragma once
#include <random>

__inline
LPCTSTR
GetCurrentProcessPathPtr(){
	LPTSTR lptcsPath = NULL;
	if (_get_tpgmptr(&lptcsPath))
	{
		return _T("");
	}

	return lptcsPath;
}

__inline
tstring
WINAPI
GetCurrentProcessDir()
{
	tstring strPath = GetCurrentProcessPathPtr();
	size_t nPos = strPath.find_last_of(_T('\\'));
	if (nPos != tstring::npos)
	{
		return strPath.substr(0, nPos + 1);
	}

	return strPath;
}

__inline
LPCTSTR
GetCurrentProcessName()
{
	LPCTSTR lpPath = GetCurrentProcessPathPtr();
	LPCTSTR lpPos = _tcsrchr(lpPath, _T('\\'));
	if (!lpPos
		|| *lpPath == NULL)
	{
		return _T("");
	}

	return lpPos + 1;
}

__inline
DWORD
GetRandom32()
{
	std::random_device rd;
	std::mt19937 mt(rd());
	return mt();
}