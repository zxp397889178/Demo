#include "stdafx.h"

//���ھ�̬��İ汾�ж�
std::string g_strVersion = "V0.3.0.324396";

const char* GetDirectUIVersion()
{
	return g_strVersion.c_str();
}

void ShowDirectUIVersion()
{
	MessageBoxA(HWND_DESKTOP, g_strVersion.c_str(), "version info", MB_OK);
}
