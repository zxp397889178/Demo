#include "stdafx.h"
#include "Util.h"
#include <direct.h>

namespace Xdelte
{
	std::string Util::Str2Utf8( const wstring& str )
	{
		int utfLen = WideCharToMultiByte(CP_UTF8, NULL, str.c_str(), wcslen(str.c_str()), NULL, 0, NULL, NULL);
		char* szString = new char[utfLen+1];

		WideCharToMultiByte(CP_UTF8, NULL, str.c_str(), wcslen(str.c_str()), szString, utfLen, NULL, NULL);
		szString[utfLen] = '\0';

		string strUtf8 = szString;
		delete szString;

		return strUtf8;
	}

	std::wstring Util::Utf82Str( const string& str )
	{
		// utf8 --> unicode
		int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), str.length(), NULL, 0);
		wchar_t* wszString = new wchar_t[wcsLen + 1];

		MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), str.length(), wszString, wcsLen);
		wszString[wcsLen] = '\0';

		wstring strUnicode = wszString;
		delete wszString;

		return strUnicode;
	}
}

