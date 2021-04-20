// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once

// Change these values to use different versions

//#define WINVER  0x0601
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0501
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define WIN32_LEAN_AND_MEAN


#include "DirectUI.h"
#include "macros.h"
#include "EngineObj.h"
#include "ResObj.h"
#include "AlphaBlendEx.h"
#include "TextHostImpl.h"
#include "DragDropImpl.h"
#include "TemplateObj.h"
#include "sse.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <WinInet.h>
#include <utility>
#include <shlwapi.h >
#include <shlobj.h>
#include <algorithm>
#include <ShellAPI.h>
#include <CommCtrl.h>
#include <Psapi.h>
#include "InterDef.h"
#include "Log/easylogging++.h"
#include "Gesture.h"
#pragma comment(lib,"Imm32.lib")
//#pragma comment(lib,"riched20.lib") 
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"Msimg32.lib")
#pragma comment(lib,"comctl32.lib")


