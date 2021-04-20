// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// CommonUI.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
void _tcssplit(std::vector<tstring>& vctrDest, LPCTSTR lptcsSrc, LPCTSTR lptcsPattern)
{
	vctrDest.clear();
	LPCTSTR lptcsFinded;
	size_t	nPatternLen = _tcslen(lptcsPattern);
	do
	{
		lptcsFinded = _tcsstr(lptcsSrc, lptcsPattern);
		if (!lptcsFinded)
		{
			break;
		}

		vctrDest.push_back(tstring(lptcsSrc, lptcsFinded));
		lptcsSrc = lptcsFinded + nPatternLen;

	} while (true);


	if (_tcsicmp(lptcsSrc, _T("")) != 0)
	{
		vctrDest.push_back(lptcsSrc);
	}
}

bool equal_icmp(LPCTSTR lpszValue1, LPCTSTR lpszValue2)
{
	if (_tcslen(lpszValue1) == _tcslen(lpszValue2))
	{
		return !_tcsicmp(lpszValue1, lpszValue2);
	}
	return false;
}

string Str2Utf8(const tstring& str)
{
#ifdef _UNICODE	
	return UnicodeToUtf8(str);
#else
	return AnsiToUtf8(str);
#endif
}

string		UnicodeToUtf8(wstring unicode)
{
	int utfLen = WideCharToMultiByte(CP_UTF8, NULL, unicode.c_str(), wcslen(unicode.c_str()), NULL, 0, NULL, NULL);
	char* szString = new char[utfLen + 1];

	WideCharToMultiByte(CP_UTF8, NULL, unicode.c_str(), wcslen(unicode.c_str()), szString, utfLen, NULL, NULL);
	szString[utfLen] = '\0';

	string str = szString;
	delete []szString;

	return str;
}

string AnsiToUtf8(string ansi)
{
	// ansi --> unicode
	int wcsLen = MultiByteToWideChar(CP_ACP, NULL, ansi.c_str(), ansi.length(), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	MultiByteToWideChar(CP_ACP, NULL, ansi.c_str(), ansi.length(), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	// unicode --> utf8
	int utfLen = WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
	char* szString = new char[utfLen + 1];

	WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), szString, utfLen, NULL, NULL);
	szString[utfLen] = '\0';

	string str = szString;
	delete []szString;
	delete []wszString;

	return str;
}