// 91uSkin2.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"

#ifndef DIRECTUI_STATICLIB

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		//::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif


