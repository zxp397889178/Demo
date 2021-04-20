#pragma once

//����DUI�������õ�stl���ģ���࣬�򵥽��õ����ྯ��
#pragma warning( disable: 4251 )

#include "DirectUI.h"
#include "UIDef.h"
#include "CommonUI.h"

#ifdef _DEBUG
#pragma comment( lib,"DirectUI_ud.lib" ) 
#else
#pragma comment( lib,"DirectUI_u.lib" ) 
#endif

#ifndef new_nothrow
#define new_nothrow  new(std::nothrow)
#endif


void _tcssplit(std::vector<tstring>& vctrDest, LPCTSTR lptcsSrc, LPCTSTR lptcsPattern);
bool equal_icmp(LPCTSTR lpszValue1, LPCTSTR lpszValue2);
string  AnsiToUtf8(string ansi);
string	UnicodeToUtf8(wstring unicode);
string  Str2Utf8(const tstring& str);