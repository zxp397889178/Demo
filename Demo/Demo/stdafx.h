// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

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

// TODO:  在此处引用程序需要的其他头文件
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
