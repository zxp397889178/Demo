#include "StdAfx.h"
#include "DPI.h"
#include "VersionHelpers.h"

//96 DPI = 100% scaling
//120 DPI = 125% scaling
//144 DPI = 150% scaling
//168 DPI = 175% scaling
//192 DPI = 200% scaling

typedef HRESULT (WINAPI *LPSetProcessDpiAwareness)(
	_In_ PROCESS_DPI_AWARENESS value
	);

typedef HRESULT (WINAPI *LPGetProcessDpiAwareness)(
	_In_  HANDLE                hprocess,
	_Out_ PROCESS_DPI_AWARENESS *value
	);


typedef HRESULT (WINAPI *LPGetDpiForMonitor)(
	_In_  HMONITOR         hmonitor,
	_In_  MONITOR_DPI_TYPE dpiType,
	_Out_ UINT             *dpiX,
	_Out_ UINT             *dpiY
	);

int CDPI::m_nAppliedDPI = -2;
CDPI::CDPI()
{
	m_nScaleFactor = 0;
	m_nScaleFactorSDA = 0;
	m_Awareness = PROCESS_PER_MONITOR_DPI_AWARE;

	SetScale(96);

}

int CDPI::GetDPIOfMonitor(HMONITOR hMonitor)
{
	UINT dpix = 96, dpiy = 96;
	if (IsWindows8Point1OrGreater()) {
		HRESULT  hr = E_FAIL;
		HMODULE hModule =::LoadLibrary(_T("Shcore.dll"));
		if(hModule != NULL) {
			LPGetDpiForMonitor GetDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
			if (GetDpiForMonitor != NULL && GetDpiForMonitor(hMonitor,MDT_EFFECTIVE_DPI, &dpix, &dpiy) != S_OK) {
				MessageBox(NULL, _T("GetDpiForMonitor failed"), _T("Notification"), MB_OK);
				return 96;
			}
		}
	}
	else {
		HDC screen = GetDC(0);
		dpix = GetDeviceCaps(screen, LOGPIXELSX);
		ReleaseDC(0, screen);
	}
	return dpix;
}

int CDPI::GetDPIOfMonitorNearestToPoint(POINT pt)
{
	HMONITOR hMonitor;
	hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	return GetDPIOfMonitor(hMonitor);
}

int CDPI::GetAppliedDPI()
{
	if (m_nAppliedDPI == -2)
	{
		m_nAppliedDPI = -1;
		HKEY hKey;

		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Control Panel\\Desktop\\WindowMetrics"),
			0,
			KEY_ALL_ACCESS,
			&hKey) == ERROR_SUCCESS)
		{
			DWORD dwValue;
			DWORD dwSize;
			DWORD dwType = REG_DWORD;
			if (RegQueryValueEx(hKey, _T("AppliedDPI"), NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
			{
				m_nAppliedDPI = dwValue;
				if (m_nAppliedDPI < 96 || m_nAppliedDPI > 1000)
					m_nAppliedDPI = -1;

			}
			RegCloseKey(hKey);
		}

	}
	return m_nAppliedDPI;
}
int CDPI::GetMainMonitorDPI()
{
	POINT    pt;
	// Get the DPI for the main monitor
	pt.x = 1;
	pt.y = 1;
	return GetDPIOfMonitorNearestToPoint(pt);
}

PROCESS_DPI_AWARENESS CDPI::GetDPIAwareness()
{
	if (IsWindows8Point1OrGreater()) {
		HMODULE hModule =::LoadLibrary(_T("Shcore.dll"));
		if(hModule != NULL) {
			LPGetProcessDpiAwareness GetProcessDpiAwareness = (LPGetProcessDpiAwareness)GetProcAddress(hModule, "GetProcessDpiAwareness");
			if(GetProcessDpiAwareness != NULL) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
				if(GetProcessDpiAwareness(hProcess, &m_Awareness) == S_OK) {
				}
			}
		}
	}

	return m_Awareness;
}

BOOL CDPI::SetDPIAwareness(PROCESS_DPI_AWARENESS Awareness)
{
	BOOL bRet = FALSE;
	if (IsWindows8Point1OrGreater()) {
		HMODULE hModule =::LoadLibrary(_T("Shcore.dll"));
		if(hModule != NULL) {
			LPSetProcessDpiAwareness SetProcessDpiAwareness = (LPSetProcessDpiAwareness)GetProcAddress(hModule, "SetProcessDpiAwareness");
			if (SetProcessDpiAwareness != NULL && SetProcessDpiAwareness(Awareness) == S_OK) {
				m_Awareness = Awareness;
				bRet = TRUE;
			}
		}
	}
	else {
		m_Awareness = Awareness;
	}
	return bRet;
}

UINT CDPI::GetDPI()
{
	if (m_Awareness == PROCESS_DPI_UNAWARE) {
		return 96;
	}

	if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
		return MulDiv(m_nScaleFactorSDA, 96, 100);
	}

	return MulDiv(m_nScaleFactor, 96, 100);
}

UINT CDPI::GetScale()
{
	if (m_Awareness == PROCESS_DPI_UNAWARE) {
		return 100;
	}
	if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
		return m_nScaleFactorSDA;
	}
	return m_nScaleFactor;
}


void CDPI::SetScale(UINT uDPI)
{
	m_nScaleFactor = MulDiv(uDPI, 100, 96);
	if (m_nScaleFactorSDA == 0) {
		m_nScaleFactorSDA = m_nScaleFactor;
	}
}

int  CDPI::Scale(int iValue)
{
	if (m_Awareness == PROCESS_DPI_UNAWARE) {
		return iValue;
	}
	if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
		return MulDiv(iValue, m_nScaleFactorSDA, 100);
	}
	return MulDiv(iValue, m_nScaleFactor, 100);
}

int  CDPI::ScaleBack(int iValue) {

	if (m_Awareness == PROCESS_DPI_UNAWARE) {
		return iValue;
	}
	if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
		return MulDiv(iValue,  100, m_nScaleFactorSDA);
	}
	return MulDiv(iValue, 100, m_nScaleFactor);
}

RECT CDPI::Scale(RECT rcRect)
{
	RECT rcScale = rcRect;
	int sw = Scale(rcRect.right - rcRect.left);
	int sh = Scale(rcRect.bottom - rcRect.top);
	rcScale.left = Scale(rcRect.left);
	rcScale.top = Scale(rcRect.top);
	rcScale.right = rcScale.left + sw;
	rcScale.bottom = rcScale.top + sh;
	return rcScale;
}

void CDPI::Scale(RECT *pRect)
{
	int sw = Scale(pRect->right - pRect->left);
	int sh = Scale(pRect->bottom - pRect->top);
	pRect->left = Scale(pRect->left);
	pRect->top = Scale(pRect->top);
	pRect->right = pRect->left + sw;
	pRect->bottom = pRect->top + sh;
}

void CDPI::ScaleBack(RECT *pRect)
{
	int sw = ScaleBack(pRect->right - pRect->left);
	int sh = ScaleBack(pRect->bottom - pRect->top);
	pRect->left = ScaleBack(pRect->left);
	pRect->top = ScaleBack(pRect->top);
	pRect->right = pRect->left + sw;
	pRect->bottom = pRect->top + sh;
}

void CDPI::ScaleBack(SIZE *pSize)
{
	pSize->cx = ScaleBack(pSize->cx);
	pSize->cy = ScaleBack(pSize->cy);
}

void CDPI::Scale(POINT *pPoint)
{
	pPoint->x = Scale(pPoint->x);
	pPoint->y = Scale(pPoint->y);
}

POINT CDPI::Scale(POINT ptPoint)
{
	POINT ptScale = ptPoint;
	ptScale.x = Scale(ptPoint.x);
	ptScale.y = Scale(ptPoint.y);
	return ptScale;
}

void CDPI::Scale(SIZE *pSize)
{
	pSize->cx = Scale(pSize->cx);
	pSize->cy = Scale(pSize->cy);
}

SIZE CDPI::Scale(SIZE szSize)
{
	SIZE szScale = szSize;
	szScale.cx = Scale(szSize.cx);
	szScale.cy = Scale(szSize.cy);
	return szScale;
}




int  CDPI::Scale(int iValue, int nScaleFactor)
{
	return MulDiv(iValue, nScaleFactor, 100);
}

int  CDPI::ScaleBack(int iValue, int nScaleFactor) 
{
	return MulDiv(iValue, 100, nScaleFactor);
}

RECT CDPI::Scale(RECT rcRect, int nScaleFactor)
{
	RECT rcScale = rcRect;
	int sw = Scale(rcRect.right - rcRect.left, nScaleFactor);
	int sh = Scale(rcRect.bottom - rcRect.top, nScaleFactor);
	rcScale.left = Scale(rcRect.left, nScaleFactor);
	rcScale.top = Scale(rcRect.top, nScaleFactor);
	rcScale.right = rcScale.left + sw;
	rcScale.bottom = rcScale.top + sh;
	return rcScale;
}

void CDPI::Scale(RECT *pRect, int nScaleFactor)
{
	int sw = Scale(pRect->right - pRect->left, nScaleFactor);
	int sh = Scale(pRect->bottom - pRect->top, nScaleFactor);
	pRect->left = Scale(pRect->left, nScaleFactor);
	pRect->top = Scale(pRect->top, nScaleFactor);
	pRect->right = pRect->left + sw;
	pRect->bottom = pRect->top + sh;
}

void CDPI::ScaleBack(RECT *pRect, int nScaleFactor)
{
	int sw = ScaleBack(pRect->right - pRect->left, nScaleFactor);
	int sh = ScaleBack(pRect->bottom - pRect->top, nScaleFactor);
	pRect->left = ScaleBack(pRect->left, nScaleFactor);
	pRect->top = ScaleBack(pRect->top, nScaleFactor);
	pRect->right = pRect->left + sw;
	pRect->bottom = pRect->top + sh;
}

void CDPI::Scale(POINT *pPoint, int nScaleFactor)
{
	pPoint->x = Scale(pPoint->x, nScaleFactor);
	pPoint->y = Scale(pPoint->y, nScaleFactor);
}

POINT CDPI::Scale(POINT ptPoint, int nScaleFactor)
{
	POINT ptScale = ptPoint;
	ptScale.x = Scale(ptPoint.x, nScaleFactor);
	ptScale.y = Scale(ptPoint.y, nScaleFactor);
	return ptScale;
}

void CDPI::Scale(SIZE *pSize, int nScaleFactor)
{
	pSize->cx = Scale(pSize->cx, nScaleFactor);
	pSize->cy = Scale(pSize->cy, nScaleFactor);
}

SIZE CDPI::Scale(SIZE szSize, int nScaleFactor)
{
	SIZE szScale = szSize;
	szScale.cx = Scale(szSize.cx, nScaleFactor);
	szScale.cy = Scale(szSize.cy, nScaleFactor);
	return szScale;
}
