#pragma once

#ifndef UTILS_STATICLIB
	#ifdef UTILS_EXPORTS
		#define UTILS_API __declspec(dllexport)
	#else
		#define UTILS_API __declspec(dllimport)
	#define UTILS_INCLUDE
	#endif
#else
	#define UTILS_API 
	#define UTILS_INCLUDE
#endif

#ifndef new_nothrow
#define new_nothrow  new(std::nothrow)
#endif

//call on applicaion init
UTILS_API
bool
WINAPI
UtilsStartup();

//call on applicaion exit
UTILS_API
void
WINAPI
UtilsShutdown();


#ifdef UTILS_INCLUDE
#include "Utils/StlDefine.h"
#include "Utils/StringDefine.h"
#include "Utils/StringEx.h"
#include "Utils/StringConvert.hpp"
#include "Utils/CharsetConvert.h"
#include "Utils/Hash.h"
#include "Utils/Encrypt.h"
#include "Utils/WinApiEx.h"
#include "Utils/Url.h"
#endif
