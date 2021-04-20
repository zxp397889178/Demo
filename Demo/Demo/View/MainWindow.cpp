// 91uClient.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include <shellapi.h>
#include "Utils/macros.h"
#include "MainWnd.h"

#include "View\UIEditLayout.h"
#include <Crtdbg.h>

#include <DbgHelp.h>
#include "UIEditEx.h"
#include "UIEditLayout.h"
#include "Util/Common.h"
#include "MainViewWnd.h"

#pragma comment(lib, "dbghelp.lib")

#define MUTEX_DEMO		_T("_Mutex_Demo")

void LoadUIPlugin(LPTSTR lpCmdLine);
LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo);
void _SetProcessDPIAware();

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{

// 	HANDLE hMutex = CreateMutex(NULL, TRUE, MUTEX_DEMO);
// 	DWORD dwRet = GetLastError();
// 
// 	if (hMutex
// 		&& dwRet == ERROR_ALREADY_EXISTS)
// 	{
// 		CloseHandle(hMutex);
// 		return -1;
// 	}
	SetUnhandledExceptionFilter(TopLevelExceptionFilter);
	_CrtSetReportMode(_CRT_ASSERT, 0);

	LoadUIPlugin(lpCmdLine);	

	CMainWnd::RegisterWnd(hInstance);
	CMainWnd cMainWnd;
	EventbusAssociateWnd(cMainWnd.CreateWnd(), Wm_User_EventBus);

	CMainViewWnd* pMainViewWnd = new CMainViewWnd();
	if (pMainViewWnd)
	{
		HWND hwnd = pMainViewWnd->CreateWnd(NULL);
		pMainViewWnd->SetAutoDel(true);
		pMainViewWnd->CenterWindow();
		pMainViewWnd->ShowWindow(SW_SHOW);
	}

	cMainWnd.LoopMessage();

	EventbusShutdown();
	GetUIEngine()->UnInitSkin();

// 	if (hMutex)
// 		CloseHandle(hMutex);

	return 0;
}

void LoadUIPlugin(LPTSTR lpCmdLine)
{
	GetUIEngine()->InitSkin(/*true*/);
	GetUIRes()->SetLanguage(_T("zh_CN"));
	GetUIRes()->SetDefaultImageType(UITYPE_IMAGE::UIIAMGE_BITMAP);

	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	TCHAR *pLastBias = NULL;
	if (pLastBias = _tcsrchr(szPath, '\\'))
		*(pLastBias + 1) = '\0';

	tstring strFrameworkPath = szPath;
	strFrameworkPath += _T("framework\\");
	GetUIRes()->InitResDir(strFrameworkPath.c_str());

	tstring strPath = szPath;
	strPath += _T("Skins\\");
	GetUIRes()->InitResDir(strPath.c_str(), true);
	
	if (!EventbusStartup())
	{
		return;
	}

 	UI_OBJECT_REGISTER(CEditExUI);
 	UI_OBJECT_REGISTER(CEditLayoutUI);
// 	UI_OBJECT_REGISTER(CAnimationCtrlUI);
// 	UI_OBJECT_REGISTER(CAutoSizeCheckBoxUI);

}

LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
	TCHAR wszAppDataPath[MAX_PATH * 2] = { 0 };
	GetEnvironmentVariable(_T("APPDATA"), wszAppDataPath, sizeof(wszAppDataPath));
	std::wstring strPath = wszAppDataPath;
	strPath += _T("\\101PPT\\Dump\\");

	common::CreateDirectories(strPath);
	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);

	TCHAR szFileName[MAX_PATH] = { 0 };
	wsprintf(szFileName, _T("%s%04d%02d%02d-%02d%02d%02d-Record.dmp"), strPath.c_str(),
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = false;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);

		CloseHandle(hFile);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void _SetProcessDPIAware()
{
	HINSTANCE hUser32 = LoadLibrary(_T("user32.dll"));
	if (hUser32)
	{
		typedef BOOL(WINAPI* pFnSetProcessDPIAware)(void);
		pFnSetProcessDPIAware pSetProcessDPIAware = (pFnSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");
		if (pSetProcessDPIAware)
		{
			pSetProcessDPIAware();
		}

		FreeLibrary(hUser32);
	}
}