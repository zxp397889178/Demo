// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>
#include "DirectUI.h"
#include "EventBus/EventBus.h"
#include "Utils/Utils.h"

#include <ucsdk.h>
#include <sqlite3.h>
#include <sqlite3pp.h>
#include <NDhttp.h>

#include <rapidjson_wrapper.hpp>
#include <rapidjson_stream.hpp>

using namespace rapidjson;
using namespace rjwrapper;

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#ifdef _DEBUG
#pragma comment( lib,"DirectUI_ud.lib" ) 
#pragma comment(lib, "EventBus_ud.lib")
#pragma comment( lib,"Utils_d.lib" )

#else
#pragma comment( lib,"DirectUI_u.lib" ) 
#pragma comment(lib, "EventBus_u.lib")
#pragma comment( lib,"Utils.lib" )
#endif

#ifdef _DEBUG
#pragma comment(lib, "NDHttp_ud.lib")
#else
#pragma comment(lib, "NDHttp_u.lib" ) 
#endif

#ifdef _DEBUG
#pragma comment(lib, "UCSDK_d.lib")
#else
#pragma comment(lib, "UCSDK.lib")
#endif

#ifdef _DEBUG
#pragma comment( lib,"sqlite_ud.lib" ) 
#else
#pragma comment( lib,"sqlite_u.lib" ) 
#endif

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "../thirdparty/openssl/include/openssl/ssleay32.lib")
#pragma comment(lib, "../thirdparty/openssl/include/openssl/libeay32.lib")
