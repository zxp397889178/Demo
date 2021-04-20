/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/

#include "stdAfx.h"
#include "EngineObj.h"
#include "UICaret.h"
#include "Gesture.h"

///////////////////////////////////////////////////////////////////////
#ifdef _GDIPLUS_SUPPORT
GdiplusStartupInput mGdiplusStartupInput;
ULONG_PTR mGdiplusToken;
#endif

LONG xPerInchScreenDC; 					// Pixels per inch used for conversions ...
LONG yPerInchScreenDC;					// ... and determining whether screen or ...

HDC hdcGrobal = NULL;
Graphics* graphicsGlobal;

typedef std::map<UINT, CEventSource*> USER_EVENT_LIST;
USER_EVENT_LIST g_mapUserEventList; // 全局事件
std::map<HMODULE, USER_EVENT_LIST*>g_mapModuleList; // 模块事件

map<CControlUI*, CControlUI*>g_mapCheckControlPtrList; //用于监测只是是否有效
EngineObj* GetEngineObj()
{
	if (!EngineObj::m_pUIEngine)
		EngineObj::m_pUIEngine = new EngineObj;
	return EngineObj::m_pUIEngine;
}

CDPI* GetDPIObj()
{
	return GetEngineObj()->GetDPIObj();
}

EngineObj* EngineObj::m_pUIEngine = NULL;
EngineObj::EngineObj()
{
	m_dwMainThreadId = 0;
	m_bRTLLayout = false;
	m_pPreNotify = NULL;
	m_dwOSVersion = 0;
	m_dwOSMinorVersion = 0;
	m_nAlpha = 255;
	
	m_clrSkinColor = _BASE_SKIN_COLOR;	
	m_pSkinImageObj = NULL;
	m_bInitCom = false;
	m_bInitOle = false;
	m_hInstance = NULL;
	m_uBigIconId = 0;
	m_uSmallIconId = 0;
	m_hWndMsg = NULL;
	m_hInstance = ::GetModuleHandle(NULL);

	HDC hdc = ::GetDC(NULL);
	xPerInchScreenDC = GetDeviceCaps(hdc, LOGPIXELSX); 
	yPerInchScreenDC = GetDeviceCaps(hdc, LOGPIXELSY);
	::ReleaseDC(NULL, hdc);
	m_pDPI = new CDPI;
}
EngineObj::~EngineObj()
{
	delete m_pDPI;
	CCaretUI::ReleaseInstance();
	ResObj::ReleaseInstance();
}

void EngineObj::UnSkinWindow(HWND hWnd)
{
	if(!hWnd)return;
	CWindowUI* pIw=(CWindowUI*)GetWindow(hWnd);
	CWindowBase *p91UWnd = CWindowBase::FromHandle(hWnd);
	if (p91UWnd)
		p91UWnd->UnsubclassWindow();
	if (pIw) 
		m_aWindowList.Remove(pIw);
}

void  EngineObj::SetAlpha(int iAlpha )
{
	if (m_nAlpha == iAlpha)
		return;
	m_nAlpha = iAlpha;

	for (int i = 0; i < m_aWindowList.GetSize(); i++)
	{
		CWindowUI* pWindow = (CWindowUI*)m_aWindowList[i];
		pWindow->SetAlpha(iAlpha);
	}
}


int EngineObj::GetAlpha()
{ 
	return m_nAlpha;
}

int EngineObj::GetWindowCount()
{
	return m_aWindowList.GetSize();
}


CWindowUI* EngineObj::GetWindow(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetWindowCount())
		return NULL;
	return (CWindowUI*)m_aWindowList[nIndex];
}

CWindowUI* EngineObj::GetWindow(HWND hWnd)
{
	for (int it =0; it < m_aWindowList.GetSize(); it++)
	{
		CWindowUI* pIw=(CWindowUI*)m_aWindowList[it];
		if(hWnd==pIw->GetHWND())return pIw;
	}
	return NULL;
}


CWindowUI* EngineObj::GetWindow(LPCTSTR lpszId, LPCTSTR lpszSubId)
{
	for (int it =0; it < m_aWindowList.GetSize(); it++)
	{
		CWindowUI* pIw=(CWindowUI*)m_aWindowList[it];
		if(equal_icmp(pIw->GetId(), lpszId) && equal_icmp(pIw->GetSubId(), lpszSubId))	return pIw;
	}
	return NULL;
}

CWindowUI* EngineObj::GetWindow(LPCTSTR lpszId)
{
	if(!lpszId)return NULL;

	for (int it =0; it < m_aWindowList.GetSize(); it++)
	{
		CWindowUI* pIw=(CWindowUI*)m_aWindowList[it];
		if(equal_icmp(pIw->GetId(), lpszId))
			return pIw;
	}
	return NULL;
}

void EngineObj::SkinWindow(CWindowUI* pWindow)
{	
	m_aWindowList.Add(pWindow);
}

void EngineObj::UnSkinWindow(CWindowUI* pWindow)
{
	int nCnt = m_aWindowList.GetSize();
	for (int i =0; i < nCnt; i++)
	{
		CWindowUI* pIw=(CWindowUI*)m_aWindowList.GetAt(i);
		if(pIw == pWindow)
		{
			m_aWindowList.Remove(i);
			break;
		}
	}

}

void EngineObj::EnableHDPI()
{

	HINSTANCE hUser32 = LoadLibrary(L"user32.dll");
	if (hUser32)
	{
		typedef BOOL(WINAPI* LPSetProcessDPIAware)(void);
		LPSetProcessDPIAware pSetProcessDPIAware = (LPSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");
		if (pSetProcessDPIAware)
		{
			pSetProcessDPIAware();
			
		}
		FreeLibrary(hUser32);
	}
	HDC hdc = ::GetDC(NULL);
	xPerInchScreenDC = GetDeviceCaps(hdc, LOGPIXELSX);
	yPerInchScreenDC = GetDeviceCaps(hdc, LOGPIXELSY);
	::ReleaseDC(NULL, hdc);
}

bool EngineObj::IsEnableHDPI()
{
	bool bEnableHDPI = false;
	HINSTANCE hUser32 = LoadLibrary(L"user32.dll");
	if (hUser32)
	{
		typedef BOOL(WINAPI* LPIsProcessDPIAware)(void);
		LPIsProcessDPIAware pIsProcessDPIAware = (LPIsProcessDPIAware)GetProcAddress(hUser32, "IsProcessDPIAware");
		if (pIsProcessDPIAware)
		{
			bEnableHDPI = pIsProcessDPIAware();
		}
		FreeLibrary(hUser32);
	}
	return bEnableHDPI; 
}

CDPI* EngineObj::GetDPIObj()
{
	return m_pDPI;
}

int EngineObj::GetDPI()
{
	return GetDPIObj()->GetDPI();
}

void EngineObj::SetDPIName(LPCTSTR lpszDPIName)
{
	m_strDPIName = lpszDPIName;

	ResObj::GetInstance()->ReLoadImage();
}

bool EngineObj::SetLogPath(LPCTSTR lpszPath)
{
	if (!lpszPath)
		return false;

	string strDir;
	/// 使用默认配置  
	el::Configurations defaultConf;
	defaultConf.setToDefault();

	//tstring tstr = lpszPath;
	//strDir = Charset::UnicodeTochar(lpszPath);
	//string strLogFile = strDir + "\\dui_log_%datetime{%Y%M%d}.log";
	//defaultConf.setGlobally(el::ConfigurationType::Filename, strLogFile);
	
	/*strLogFile = strDir + "\\dui_trace_log_%datetime{%Y%M%d}.log";
	defaultConf.set(el::Level::Info, el::ConfigurationType::Filename, strLogFile);*/

	string strLogFile = Charset::UnicodeTochar(lpszPath);
	defaultConf.setGlobally(el::ConfigurationType::Filename, strLogFile);
	defaultConf.setGlobally(el::ConfigurationType::Enabled, "true");
	/// 重新设置配置  
	el::Loggers::reconfigureLogger("default", defaultConf);
	return true;
}

LPCTSTR EngineObj::GetDPIName()
{
	return m_strDPIName.c_str();
}

static void hdpi_send_event(CControlUI* pParent)
{
	if (!pParent)
		return;
	int nCount = pParent->GetCount();
	if (nCount <= 0)
		return;
	for (int i = 0; i < nCount; i++)
	{
		CControlUI* pControl = pParent->GetItem(i);
		TEventUI event = { 0 };
		event.nType = UIEVENT_DPI_CHANGED;
		if (pControl)
			pControl->SendEvent(event);
		hdpi_send_event(pControl);
	}
}

void EngineObj::SetDPI(int nDPI, bool bAutoResizeWindow)
{
	int nPreHDPI = GetUIEngine()->GetDPI();
	int nNewHDPI = nDPI;

	GetDPIObj()->SetScale(nDPI);
	GetResObj()->ReBulidFont();

	if (bAutoResizeWindow)
	{
		for (int i = 0; i < GetUIEngine()->GetWindowCount(); i++)
		{
			RECT rect;
			HWND hWnd = GetUIEngine()->GetWindow(i)->GetHWND();
			::GetWindowRect(hWnd, &rect);
			int nWidth = (rect.right - rect.left)  * nNewHDPI / nPreHDPI;
			int nHeight = (rect.bottom - rect.top) * nNewHDPI / nPreHDPI;
			hdpi_send_event(GetUIEngine()->GetWindow(i)->GetRoot());
			::SetWindowPos(hWnd, NULL, rect.left, rect.top, nWidth, nHeight, SWP_NOACTIVATE | SWP_NOZORDER);
		}

	}
	else
	{
		for (int i = 0; i < GetUIEngine()->GetWindowCount(); i++)
		{
			hdpi_send_event(GetUIEngine()->GetWindow(i)->GetRoot());
			GetUIEngine()->GetWindow(i)->GetRoot()->Resize();
			GetUIEngine()->GetWindow(i)->Invalidate();
		}
	}
	
}


typedef BOOL (WINAPI *_ChangeWindowMessageFilter)(UINT, DWORD);  
static void call_user_event(TUserEventUI* pUserEvent, HMODULE hModule)
{
	if (hModule == NULL)
	{
		std::map<UINT, CEventSource*>::iterator it = g_mapUserEventList.find(pUserEvent->uEventID);
		if (it != g_mapUserEventList.end())
		{
			CEventSource* pEventSource = it->second;
			(*pEventSource)(pUserEvent);
		}
	}
	else
	{
		std::map<HMODULE, USER_EVENT_LIST*>::const_iterator it_list = g_mapModuleList.find(hModule);
		if (it_list == g_mapModuleList.end())
		{
			return;
		}

		USER_EVENT_LIST* event_list = it_list->second;
		std::map<UINT, CEventSource*>::iterator it = event_list->find(pUserEvent->uEventID);
		if (it != (*event_list).end())
		{
			CEventSource* pEventSource = it->second;
			(*pEventSource)(pUserEvent);
		}
	}
}

WNDPROC _MsgOldProc = NULL;      //原窗体过程
LRESULT CALLBACK _MsgWndWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_USER + 100://WM_ASNYC_NOTIFY
			{
				TNotifyUI* pNotifyUI = (TNotifyUI*)wParam;
				HWND hWndWindow = (HWND)lParam;
				CWindowUI* pWindow = GetEngineObj()->GetWindow(hWndWindow);
				if (pWindow)
					pWindow->SendNotify(pNotifyUI->pSender, pNotifyUI->nType, pNotifyUI->wParam, pNotifyUI->lParam);
				delete pNotifyUI;
			}
			return S_OK;

		case WM_USER + 101://WM_MENU_NOTIFY
			{
				TNotifyUI* pNotifyUI = (TNotifyUI*)wParam;
				RefCountedThreadSafe<INotifyUI>* pNotifyFuncSafe =  (RefCountedThreadSafe<INotifyUI>*)lParam;

				if (pNotifyUI && pNotifyFuncSafe && pNotifyFuncSafe->IsValid())
				{
					INotifyUI* pNotifyFunc = pNotifyFuncSafe->GetCountedOwner();
					if (pNotifyFunc)
					{
						INotifyUI* pPreNotify = GetEngineObj()->GetPreNotify();
						if (pPreNotify)
							pPreNotify->ProcessNotify(pNotifyUI);

						pNotifyFunc->ProcessNotify(pNotifyUI);
					}
				}
				if (pNotifyUI)
					delete pNotifyUI;
				pNotifyUI = NULL;
			}
			return S_OK;

		case WM_USER + 102://WM_USEREVENT sendmessage
			{
				TUserEventUI* pUserEvent = (TUserEventUI*)wParam;
				HMODULE hModule = (HMODULE)lParam;
				call_user_event(pUserEvent, hModule);
			}
			return S_OK;

		case WM_USER + 103://WM_USEREVENT postmessage
		{
			TUserEventUI* pUserEvent = (TUserEventUI*)wParam;
			HMODULE hModule = (HMODULE)lParam;
			call_user_event(pUserEvent, hModule);
			delete pUserEvent;
		}
		return S_OK;

	}
	return	CallWindowProc(_MsgOldProc, hWnd, message, wParam, lParam); 
}

static CControlUI*  create_plugin()
{
	return NULL;
}

void EngineObj::InitSkin(bool bRTLayout)
{	
#ifdef _GDIPLUS_SUPPORT
	GdiplusStartup(&mGdiplusToken, &mGdiplusStartupInput, NULL);
#endif
	m_bRTLLayout = bRTLayout;
	m_dwMainThreadId = ::GetCurrentThreadId();

	hdcGrobal = CreateCompatibleDC(NULL);
	graphicsGlobal = Graphics::FromHDC(hdcGrobal);
	// 判断操作系统版本,解决UAC无法拖动文件问题
	DWORD dwOSVer = 0, dwOSMinorVer = 0;
	GetEngineObj()->GetOSVersion(dwOSVer, dwOSMinorVer);
	if (dwOSVer >= 6)
	{
		HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));    
		if (hFuncInst)
		{
			_ChangeWindowMessageFilter pChangeWindowMessageFilter =(_ChangeWindowMessageFilter)GetProcAddress(hFuncInst, "ChangeWindowMessageFilter");
			if (pChangeWindowMessageFilter)
			{
				pChangeWindowMessageFilter(WM_DROPFILES, 1/*MSGFLT_ADD*/);
				pChangeWindowMessageFilter(WM_COPYDATA, 1/*MSGFLT_ADD*/);
				pChangeWindowMessageFilter(0x0049, 1/*MSGFLT_ADD*/);

			}
			FreeLibrary(hFuncInst);
		}
	}

	InitOle();
	InitCom();
	InitGesture();

	m_hWndMsg = ::CreateWindow(_T("Message"), NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, 0);
	_MsgOldProc = (WNDPROC)::SetWindowLong(m_hWndMsg, GWL_WNDPROC, (LONG) _MsgWndWindowProc);
	InitWebbowserVersion();
	ActionManager::GetInstance();

	UI_STYLE_REGISTER(ImageStyle);
	UI_STYLE_REGISTER(TextStyle);
	UI_STYLE_REGISTER(FillStyle);
	UI_STYLE_REGISTER_EX(_T("Rect"), FillStyle);
	UI_STYLE_REGISTER(LineStyle);

	UI_OBJECT_REGISTER(CControlUI);
	UI_OBJECT_REGISTER(CContainerUI);
	UI_OBJECT_REGISTER(CLayoutUI);
	UI_OBJECT_REGISTER(COptionLayoutUI);
	UI_OBJECT_REGISTER(CScrollbarUI);
	UI_OBJECT_REGISTER(CAnimationUI);
	UI_OBJECT_REGISTER(CButtonUI);
	UI_OBJECT_REGISTER(CLabelUI);
	UI_OBJECT_REGISTER(CCheckBoxUI);
	UI_OBJECT_REGISTER(CRadioUI);
	UI_OBJECT_REGISTER(CEditArUI);
	UI_OBJECT_REGISTER(CEditUI);
	UI_OBJECT_REGISTER(CMultiLineEditUI);
	UI_OBJECT_REGISTER(CProgressBarUI);
	UI_OBJECT_REGISTER(CMenuUI);
	UI_OBJECT_REGISTER(CComboBoxUI);
	UI_OBJECT_REGISTER(CListUI);
	UI_OBJECT_REGISTER(CListItemUI);
	UI_OBJECT_REGISTER(CTableUI);
	UI_OBJECT_REGISTER(CTableItemUI);
	UI_OBJECT_REGISTER(CListHeaderItemUI);
	UI_OBJECT_REGISTER(CListHeaderUI);
	UI_OBJECT_REGISTER(CListContainerItemUI);
	UI_OBJECT_REGISTER(CTreeUI);
	UI_OBJECT_REGISTER(CTreeNodeUI);
	UI_OBJECT_REGISTER(CTreeContainerNodeUI);
	UI_OBJECT_REGISTER(CTabButtonUI);
	UI_OBJECT_REGISTER(CTabHeaderUI);
	UI_OBJECT_REGISTER(CTabUI);
	UI_OBJECT_REGISTER(CSliderUI);
	UI_OBJECT_REGISTER(CSplitterUI);
	UI_OBJECT_REGISTER(CCompositeUI);
	UI_OBJECT_REGISTER(CControlWndUI);
	UI_OBJECT_REGISTER(CHtmlLabelUI);
	UI_OBJECT_REGISTER(CWebBrowserUI);
	UI_OBJECT_REGISTER(CFlashUI);
	UI_OBJECT_REGISTER(CRichEditUI);
	UI_OBJECT_REGISTER(CCssEditUI);
}

void EngineObj::UnInitSkin()
{
	ActionManager::ReleaseInstance();
	if (m_hWndMsg) 
		::DestroyWindow(m_hWndMsg);
	m_hWndMsg = NULL;

	vector<HWND>vecWindow;
	for (int i = 0; i < m_aWindowList.GetSize(); i++)
	{
		CWindowUI* pIw = GetEngineObj()->GetWindow(i);
		vecWindow.push_back(pIw->GetHWND());
	}
	m_aWindowList.Empty();

	for(size_t i = 0; i < vecWindow.size(); i++)
	{
		HWND hWnd = vecWindow[i];
		if (::IsWindow(hWnd))
		{
			::ShowWindow(hWnd, SW_HIDE);
			::DestroyWindow(hWnd);
			//::PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		}
	}

	map<UINT, CEventSource*>::iterator it;
	for (it = g_mapUserEventList.begin(); it != g_mapUserEventList.end(); ++it)
	{
		delete it->second;
	}
	g_mapUserEventList.clear();

	if (m_pSkinImageObj != NULL)
		m_pSkinImageObj->Release();
	m_pSkinImageObj = NULL;
	m_mapControlList.clear();

	if (m_bInitOle)
		::CoUninitialize();
	if (m_bInitCom) 
		::OleUninitialize();

	if (graphicsGlobal)
		delete graphicsGlobal;
	graphicsGlobal = NULL;

	if (hdcGrobal)
		::DeleteDC(hdcGrobal);
	hdcGrobal = NULL;

	if (m_pUIEngine)
		delete m_pUIEngine;
	m_pUIEngine = NULL;

#ifdef _GDIPLUS_SUPPORT
	GdiplusShutdown(mGdiplusToken);
#endif
}

bool EngineObj::PumpMessage()
{
	MSG msg;
	if (GetMessage(&msg, NULL, 0, 0))
	{
		if (!GetEngineObj()->TranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return true;
	}
	return false;
}

int EngineObj::MessageLoop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!GetEngineObj()->TranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return msg.lParam;
}

// 这些消息预处理有问题，导致浏览器快捷键处理可能有问题，以后改进参考duilib
bool EngineObj::TranslateMessage(const LPMSG pMsg)
{
	int nCount = m_aPreMessageFilterList.GetSize();
	if (nCount > 0)
	{
		for (int i = 0; i < nCount; i++)
		{
			IPreMessageFilterUI* pMessageFilter = (IPreMessageFilterUI*)m_aPreMessageFilterList.GetAt(i);
			if (pMessageFilter->PreMessageFilter(pMsg))
				return true;
		}
	}
	
	// 判断是否本身窗口
	LRESULT lRes = 0;
	CWindowBase* pWindow = NULL;
	UINT uStyle = GetWindowLong(pMsg->hwnd, GWL_STYLE);
	if ((uStyle & WS_CHILD) != 0)
	{
		HWND hwndParent = ::GetParent(pMsg->hwnd);
		while (hwndParent)
		{
			pWindow = CWindowBase::FromHandle(hwndParent);
			if (pWindow)
			{
				if (pWindow->PreMessageHandler(pMsg, lRes)) return true;
			}
			hwndParent = ::GetParent(hwndParent);
		}
	}
	else
	{
		pWindow = CWindowBase::FromHandle(pMsg->hwnd);
		if (pWindow)
		{
			if (pWindow->PreMessageHandler(pMsg, lRes)) return true;
		}
	}
	return false;
}

void EngineObj::AddPreMessageFilter(IPreMessageFilterUI* pIMessageFilter)
{
	if (m_aPreMessageFilterList.Find(pIMessageFilter) < 0)
		m_aPreMessageFilterList.Add(pIMessageFilter);
}

void EngineObj::RemovePreMessageFilter(IPreMessageFilterUI* pIMessageFilter)
{
	m_aPreMessageFilterList.Remove(pIMessageFilter);
}

void EngineObj::SetInstanceHandle(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
}

HINSTANCE EngineObj::GetInstanceHandle()
{
	return m_hInstance;
}


void EngineObj::SetIconResource(UINT uId, bool bBigIcon)
{
	if (bBigIcon)
	{
		m_uBigIconId = uId;
	}
	else
	{
		m_uSmallIconId = uId;
	}
}

UINT EngineObj::GetIconResource(bool bBigIcon)
{
	return bBigIcon ? m_uBigIconId : m_uSmallIconId;
}

void EngineObj::RegisterControl(LPCTSTR lpszName, CONTROLCREATEPROC lfCreateFn){
	m_mapControlList[lpszName] = (CONTROLCREATEPROC)lfCreateFn;
}

void EngineObj::RegisterStyle(LPCTSTR lpszType, STYLECREATEPROC lfCreateFn)
{
	m_mapStyleList[lpszType] = (STYLECREATEPROC)lfCreateFn;
	
}

void EngineObj::RegisterPlugin(LPCTSTR guid, CONTROLCREATEPROC lfCreateFn)
{
	/*TCHAR buf[128] = { 0 };
	_sntprintf(buf, BUFSIZ
		, _T("{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);*/
	m_mapPluginList[guid] = lfCreateFn;
}

CControlUI* EngineObj::CreateControl(LPCTSTR lpszType)
{
	std::map<tstring, CONTROLCREATEPROC>::iterator pos = m_mapControlList.find(lpszType);	
	if (pos != m_mapControlList.end())
		return pos->second();
	return NULL;
}

StyleObj* EngineObj::CreateStyle(LPCTSTR lpszType, CControlUI* pOwner)
{
	std::map<tstring, STYLECREATEPROC>::iterator pos = m_mapStyleList.find(lpszType);
	if (pos != m_mapStyleList.end())
		return pos->second(pOwner);
	return NULL;
}
CControlUI* EngineObj::CreatePlugin(LPCTSTR guid)
{
	/*TCHAR buf[128] = { 0 };
	_sntprintf(buf, BUFSIZ
		, _T("{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
*/
	std::map<tstring, CONTROLCREATEPROC>::iterator pos = m_mapPluginList.find(guid);
	if (pos != m_mapPluginList.end())
		return pos->second();
	return NULL;
}

bool EngineObj::AddControl(CControlUI* pParent, CControlUI* pNext, LPCTSTR lpszId)
{
	return GetResObj()->AddControl(pParent, pNext, lpszId);
}


COLORREF CalcAvgColor(LPBYTE pBitsSrc, int width, int height, int &nR, int &nG, int &nB)
{
	if (!pBitsSrc)
		return RGB(0,0,0);
	const int pitch  = width * 4;
	int nRed = 0, nRedAvg = 0;
	int nGreen = 0, nGreenAvg = 0;
	int nBlue = 0, nBlueAvg = 0;

	for (int y = 0; y < height; y++)
	{
		LPBYTE pRowSrc = pBitsSrc;

		for (int x = 0; x < width; x++)
		{
			nRed += pRowSrc[2];
			nGreen += pRowSrc[1];
			nBlue += pRowSrc[0];
			pRowSrc += 4;
		}
		nRedAvg += nRed / width;
		nGreenAvg += nGreen / width;
		nBlueAvg += nBlue / width;
		nRed = 0;
		nGreen = 0;
		nBlue = 0;
		pBitsSrc += pitch;
	}
	nRedAvg /= height;
	nGreenAvg /= height;
	nBlueAvg /=height;
	nR = nRedAvg;
	nG = nGreenAvg;
	nB = nBlueAvg;
	return RGB(nRedAvg, nGreenAvg, nBlueAvg);
}

void DoAlphaAtImageEdge(ImageObj* imageObj)
{
	if (!imageObj)
		return;
	int w = imageObj->GetWidth();
	int h = imageObj->GetHeight();
	int nBegin = min(h,w) > 100 ? 100 : 0 ;
	if (nBegin == 0)
		return;

	LPVOID lpLockData;
	LPBYTE piexlsSrc = imageObj->LockBits(&lpLockData);
	if (!piexlsSrc) return;
	
	for (int y=0; y < h; ++y)
	{
		LPBYTE pRowSrc = piexlsSrc;
		if (y > h - nBegin)
		{
			for (int x=0; x < w; ++x)
			{
				pRowSrc[3] = 255* (h - y) / nBegin;
				pRowSrc[0] = pRowSrc[0] * pRowSrc[3] / 255;
				pRowSrc[1] = pRowSrc[1] * pRowSrc[3] / 255;
				pRowSrc[2] = pRowSrc[2] * pRowSrc[3] / 255;
				pRowSrc += 4; // Move to the next pixel
			}
		}
		else
		{
			pRowSrc = pRowSrc + (w - nBegin)*4;
			for (int x=w - nBegin; x < w; ++x)
			{
				pRowSrc[3] = 255 * (w-x) / nBegin;
				pRowSrc[0] = pRowSrc[0] * pRowSrc[3] / 255;
				pRowSrc[1] = pRowSrc[1] * pRowSrc[3] / 255;
				pRowSrc[2] = pRowSrc[2] * pRowSrc[3] / 255;
				pRowSrc += 4; // Move to the next pixel
			}
		}
		piexlsSrc += w * 4;
	}
	imageObj->UnLockBits(lpLockData);
}

void EngineObj::ChangeSkin(LPCTSTR lpszSkinImage, LPCOLORREF lpSkinColor, bool bAnimation)
{
	tstring strImagePath = _T("");
	if (lpszSkinImage) strImagePath = lpszSkinImage;
	if (m_pSkinImageObj != NULL)
		m_pSkinImageObj->Release();

	m_pSkinImageObj = GetUIRes()->LoadImage(strImagePath.c_str());

	if (m_pSkinImageObj)
	{
		if (!lpSkinColor)
		{
			LPVOID lpLock = NULL;
			BYTE* pixels = m_pSkinImageObj->LockBits(&lpLock);
			int R = 0, G = 0, B = 0;
			m_clrSkinColor = CalcAvgColor(pixels, m_pSkinImageObj->GetWidth(), m_pSkinImageObj->GetHeight(), R, G, B);
			m_clrSkinColor = DUI_RGB2ARGB(m_clrSkinColor);
			m_pSkinImageObj->UnLockBits(lpLock);
		}
		else m_clrSkinColor = *lpSkinColor;
	
		// 将图片边缘处理成透明
		DoAlphaAtImageEdge(m_pSkinImageObj);
	}
	else if (lpSkinColor) m_clrSkinColor = *lpSkinColor;

	// 改变图片的色调（对比度和亮度不需要改变）
	GetResObj()->ChangeImageHSL(GetSkinColor());

	// 改变窗口的背景
	for (int i = 0; i < GetWindowCount(); i++)
	{
		CWindowUI* pWindow = GetWindow(i);
		pWindow->OnSkinChange(bAnimation);
	}
}



void EngineObj::GetOSVersion(DWORD& dwOSVer, DWORD& dwOSMinorVer)
{
	if (m_dwOSVersion == 0)
	{
		OSVERSIONINFO   osvi;   
		memset(&osvi,   0,   sizeof(OSVERSIONINFO));   
		osvi.dwOSVersionInfoSize   =   sizeof   (OSVERSIONINFO);   
		GetVersionEx   (&osvi);   
		m_dwOSVersion = osvi.dwMajorVersion;
		m_dwOSMinorVersion = osvi.dwMinorVersion;
	}
	dwOSVer =  m_dwOSVersion;
	dwOSMinorVer = m_dwOSMinorVersion;
}

tstring  EngineObj::GetRunDir()
{
	static tstring strRunDir = _T("");
	if (strRunDir.empty())
	{
		TCHAR rtfPath[MAX_PATH];
		GetModuleFileName(NULL, rtfPath, MAX_PATH);
		TCHAR *pLastBias = NULL;
		if (pLastBias = wcsrchr(rtfPath, '\\'))
			*(pLastBias + 1) = '\0';
		strRunDir = rtfPath;
	}
	return strRunDir;
}


bool EngineObj::IsRTLLayout()
{
//#ifndef DIRECTUI_LAYOUT_RTL
//	return false;
//#else
//	return true;
//#endif
	return m_bRTLLayout;
}


LPCTSTR EngineObj::GetSkinImage(){
	if (m_pSkinImageObj) return m_pSkinImageObj->GetFilePath();
	return _T("");
}

DWORD EngineObj::GetSkinColor()
{
	return m_clrSkinColor;
}


void EngineObj::InitOle()
{
	if (m_bInitOle) return;
	HRESULT hr = ::OleInitialize(NULL);
	if (SUCCEEDED(hr)) m_bInitOle = true;

}
void EngineObj::InitCom()
{
	if (m_bInitCom) return;
	HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(hr)) m_bInitCom = true;

}

pfunc_get_gesture_info get_gesture_info = nullptr;
pfunc_close_gesture_info_handle close_gesture_info_handle = nullptr;
pfunc_set_gesture_config set_gesture_config = nullptr;
void EngineObj::InitGesture()
{
	if (get_gesture_info == nullptr)
	{
		HINSTANCE hUser32 = LoadLibrary(L"user32.dll");
		get_gesture_info = (pfunc_get_gesture_info)::GetProcAddress(hUser32, "GetGestureInfo");
		close_gesture_info_handle = (pfunc_close_gesture_info_handle)::GetProcAddress(hUser32, "CloseGestureInfoHandle");
		set_gesture_config = (pfunc_set_gesture_config)::GetProcAddress(hUser32, "SetGestureConfig");
	}
}

void EngineObj::RegisterUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule)
{
	if (hModule != NULL)
	{
		RegisterModuleUserEvent(uEventID, d, hModule);
		return;
	}
	std::map<UINT, CEventSource*>::iterator it = g_mapUserEventList.find(uEventID);
	if (it != g_mapUserEventList.end())
	{
		CEventSource* pEventSource = it->second;
		*pEventSource += d;
	}
	else
	{
		CEventSource* pEventSource = new CEventSource;
		*pEventSource += d;
		g_mapUserEventList[uEventID] = pEventSource;
	}
}

void EngineObj::RegisterModuleUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule)
{
	USER_EVENT_LIST* event_list = NULL;
	std::map<HMODULE, USER_EVENT_LIST*>::const_iterator it_list = g_mapModuleList.find(hModule);
	if (it_list != g_mapModuleList.end())
	{
		event_list = it_list->second;
	}
	else
	{
		event_list = new USER_EVENT_LIST();
		g_mapModuleList[hModule] = event_list;
	}

	CEventSource* pEventSource = NULL;
	std::map<UINT, CEventSource*>::iterator it = event_list->find(uEventID);
	if (it != event_list->end())
	{
		pEventSource = it->second;
		*pEventSource += d;
	}
	else
	{
		CEventSource* pEventSource = new CEventSource;
		*pEventSource += d;
		(*event_list)[uEventID] = pEventSource;
	}

}

void EngineObj::UnRegisterUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule)
 {
	 if (hModule != NULL)
	 {
		 UnRegisterModuleUserEvent(uEventID, d, hModule);
		 return;
	 }

	std::map<UINT, CEventSource*>::iterator it = g_mapUserEventList.find(uEventID);
	if (it != g_mapUserEventList.end())
	{
		CEventSource* pEventSource = it->second;
		*pEventSource -= d;
		if (pEventSource->GetCount() == 0)
		{
			delete pEventSource;
			g_mapUserEventList.erase(it);
		}
	}
}

void EngineObj::UnRegisterModuleUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule)
{
	USER_EVENT_LIST* event_list = NULL;
	std::map<HMODULE, USER_EVENT_LIST*>::const_iterator it_list = g_mapModuleList.find(hModule);
	if (it_list == g_mapModuleList.end())
	{
		return;
	}
	event_list = it_list->second;

	CEventSource* pEventSource = NULL;
	std::map<UINT, CEventSource*>::iterator it = event_list->find(uEventID);
	if (it != event_list->end())
	{
		pEventSource = it->second;
		*pEventSource -= d;
		if (pEventSource->GetCount() == 0)
		{
			delete pEventSource;
			event_list->erase(it);

			if (event_list->size() <= 0)
			{
				delete event_list;
				g_mapModuleList.erase(it_list);
			}
		}
	}
}

void EngineObj::UnRegisterUserEvent(UINT uEventID, HMODULE hModule)
{
	if (hModule != NULL)
	{
		UnRegisterModuleUserEvent(uEventID, hModule);
		return;
	}

	std::map<UINT, CEventSource*>::iterator it = g_mapUserEventList.find(uEventID);
	if (it != g_mapUserEventList.end())
	{
		CEventSource* pEventSource = it->second;
		delete pEventSource;
		g_mapUserEventList.erase(it);
	}
}

void EngineObj::UnRegisterModuleUserEvent(UINT uEventID, HMODULE hModule)
{
	USER_EVENT_LIST* event_list = NULL;
	std::map<HMODULE, USER_EVENT_LIST*>::const_iterator it_list = g_mapModuleList.find(hModule);
	if (it_list == g_mapModuleList.end())
	{
		return;
	}
	event_list = it_list->second;

	CEventSource* pEventSource = NULL;
	std::map<UINT, CEventSource*>::iterator it = event_list->find(uEventID);
	if (it != event_list->end())
	{
		pEventSource = it->second;
		delete pEventSource;
		event_list->erase(it);
		if (event_list->size() <= 0)
		{
			delete event_list;
			g_mapModuleList.erase(it_list);
		}
	}


}

void EngineObj::FireUserEvent(UINT uEventID, WPARAM wParam, LPARAM lParam, bool bImmediately, HMODULE hModule)
{
	if (bImmediately)
	{
		TUserEventUI userevent;
		userevent.uEventID = uEventID;
		userevent.wParam = wParam;
		userevent.lParam = lParam;
		::SendMessage(GetEngineObj()->GetMsgHwnd(), WM_USER + 102/*WM_USEREVENT*/, (WPARAM)&userevent, (LPARAM)hModule);
	}
	else
	{
		TUserEventUI* pUserEvent = new TUserEventUI;
		pUserEvent->uEventID = uEventID;
		pUserEvent->wParam = wParam;
		pUserEvent->lParam = lParam;
		if (!::PostMessage(GetEngineObj()->GetMsgHwnd(), WM_USER + 103/*WM_USEREVENT*/, (WPARAM)pUserEvent, (LPARAM)hModule))
		{
			delete pUserEvent;
		}
	}

}

bool EngineObj::CheckControlPtrValid(CControlUI* pControl)
{
	map<CControlUI*, CControlUI*>::iterator it = g_mapCheckControlPtrList.find(pControl);
	if (it != g_mapCheckControlPtrList.end())
		return true;
	return false;
}

void  EngineObj::AddControlPtrToCheck(CControlUI* pControl)
{
	g_mapCheckControlPtrList[pControl] = pControl;
	//pControl->ModifyFlags(0, UICONTROLFLAG_CHECK_CTR_PTR_VALID);
}

void  EngineObj::RemoveControlPtrToCheck(CControlUI* pControl)
{
	map<CControlUI*, CControlUI*>::iterator it = g_mapCheckControlPtrList.find(pControl);
	if (it != g_mapCheckControlPtrList.end())
		g_mapCheckControlPtrList.erase(it);

	//pControl->ModifyFlags(UICONTROLFLAG_CHECK_CTR_PTR_VALID, 0);
	
}



void EngineObj::SetPreNotify( INotifyUI* pNotify )
{
	m_pPreNotify = pNotify;
}

INotifyUI* EngineObj::GetPreNotify()
{
	return m_pPreNotify;
}

IUIEngine*  GetUIEngine()
{
	return GetEngineObj();
}

void GetAnchorPos(UITYPE_ANCHOR nAnchor, const RECT* lprcParent, RECT* lprcChild)
{
	switch (nAnchor)
	{
	case UIANCHOR_LTRT:
		{
			lprcChild->left = lprcParent->left + lprcChild->left;
			lprcChild->top = lprcParent->top + lprcChild->top;
			lprcChild->right = lprcParent->right - lprcChild->right;
			lprcChild->bottom = lprcParent->top + lprcChild->bottom;
			break;
		}
	case UIANCHOR_LBRB:
		{
			lprcChild->left = lprcParent->left + lprcChild->left;
			lprcChild->top = lprcParent->bottom - lprcChild->top;
			lprcChild->right = lprcParent->right - lprcChild->right;
			lprcChild->bottom = lprcParent->bottom - lprcChild->bottom;
			break;
		}
	case UIANCHOR_LTLB:
		{
			lprcChild->left = lprcParent->left + lprcChild->left;
			lprcChild->top = lprcParent->top + lprcChild->top;
			lprcChild->right = lprcParent->left + lprcChild->right;
			lprcChild->bottom = lprcParent->bottom - lprcChild->bottom;
			break;
		}
	case UIANCHOR_RTRB:
		{
			lprcChild->left = lprcParent->right - lprcChild->left;
			lprcChild->top = lprcParent->top + lprcChild->top;
			lprcChild->right = lprcParent->right - lprcChild->right;
			lprcChild->bottom = lprcParent->bottom - lprcChild->bottom;
			break;
		}

	case UIANCHOR_LTRB:
		{
			lprcChild->left = lprcParent->left + lprcChild->left;
			lprcChild->top = lprcParent->top + lprcChild->top;
			lprcChild->right = lprcParent->right - lprcChild->right;
			lprcChild->bottom = lprcParent->bottom - lprcChild->bottom;
			break;
		}
	case UIANCHOR_LEFTBOTTOM:
		{
			lprcChild->left = lprcParent->left + lprcChild->left;
			lprcChild->top = lprcParent->bottom - lprcChild->top;
			lprcChild->right = lprcParent->left + lprcChild->right;
			lprcChild->bottom = lprcParent->bottom - lprcChild->bottom;
			break;
		}
	case UIANCHOR_RIGHTTOP:
		{
			lprcChild->left = lprcParent->right - lprcChild->left;
			lprcChild->top = lprcParent->top + lprcChild->top;
			lprcChild->right = lprcParent->right - lprcChild->right;
			lprcChild->bottom = lprcParent->top + lprcChild->bottom;
			break;
		}
	case UIANCHOR_RIGHTBOTTOM:
		{
			lprcChild->left=lprcParent->right-lprcChild->left;
			lprcChild->top=lprcParent->bottom-lprcChild->top;
			lprcChild->right = lprcParent->right - lprcChild->right;
			lprcChild->bottom = lprcParent->bottom - lprcChild->bottom;
			break;

		}
	case UIANCHOR_LEFTTOP:
		{
			lprcChild->left = lprcParent->left + lprcChild->left;
			lprcChild->top = lprcParent->top + lprcChild->top;
			lprcChild->right = lprcParent->left + lprcChild->right;
			lprcChild->bottom = lprcParent->top + lprcChild->bottom;
			break;
		}
	case UIANCHOR_LT: //中间对齐
		{
			if (lprcChild->left < 0)
			{
				int offsetx = (lprcParent->right - lprcParent->left - lprcChild->right) / 2;
				lprcChild->left = lprcParent->left + offsetx;// + lprcChild->left;
				lprcChild->right = lprcChild->left + lprcChild->right;
			}
			else
			{
				if (lprcChild->right < 0)
				{
					lprcChild->left = lprcParent->right - lprcChild->left;
					lprcChild->right = lprcChild->left - lprcChild->right;
				}
				else
				{
					lprcChild->left = lprcParent->left + lprcChild->left;
					lprcChild->right = lprcChild->left + lprcChild->right;
				}	
			}

			if (lprcChild->top < 0)
			{
				int offsety = (lprcParent->bottom - lprcParent->top - lprcChild->bottom) / 2;
				lprcChild->top = lprcParent->top + offsety;// + lprcChild->top;
				lprcChild->bottom = lprcChild->top + lprcChild->bottom;
			}
			else
			{
				if (lprcChild->bottom < 0)
				{
					lprcChild->top = lprcParent->bottom - lprcChild->top;
					lprcChild->bottom = lprcChild->top - lprcChild->bottom;
				}
				else
				{
					lprcChild->top = lprcParent->top + lprcChild->top;
					lprcChild->bottom = lprcChild->top + lprcChild->bottom;
				}	
			}
			break;
		}

		case UIANCHOR_RT: //中间对齐
			{
				if (lprcChild->left < 0)
				{
					int offsetx = (lprcParent->right - lprcParent->left - lprcChild->right) / 2;
					lprcChild->left = lprcParent->left + offsetx;// + lprcChild->left;
					lprcChild->right = lprcChild->left + lprcChild->right;
				}
				else
				{
				
					
					lprcChild->left = lprcParent->right - lprcChild->left - lprcChild->right;
					lprcChild->right = lprcChild->left + lprcChild->right;
					
				}

				if (lprcChild->top < 0)
				{
					int offsety = (lprcParent->bottom - lprcParent->top - lprcChild->bottom) / 2;
					lprcChild->top = lprcParent->top + offsety;// + lprcChild->top;
					lprcChild->bottom = lprcChild->top + lprcChild->bottom;
				}
				else
				{
					lprcChild->top = lprcParent->top + lprcChild->top;
					lprcChild->bottom = lprcChild->top + lprcChild->bottom;
				}
			}
			break;

		case UIANCHOR_RB: //中间对齐
			{
				if (lprcChild->left < 0)
				{
					int offsetx = (lprcParent->right - lprcParent->left - lprcChild->right) / 2;
					lprcChild->left = lprcParent->left + offsetx;// + lprcChild->left;
					lprcChild->right = lprcChild->left + lprcChild->right;
				}
				else
				{

					lprcChild->left = lprcParent->right - lprcChild->left - lprcChild->right;
					lprcChild->right = lprcChild->left + lprcChild->right;

				}


				if (lprcChild->top < 0)
				{
					int offsety = (lprcParent->bottom - lprcParent->top - lprcChild->bottom) / 2;
					lprcChild->top = lprcParent->top + offsety;// + lprcChild->top;
					lprcChild->bottom = lprcChild->top + lprcChild->bottom;
				}
				else
				{
					lprcChild->top = lprcParent->bottom - lprcChild->top - lprcChild->bottom;
					lprcChild->bottom = lprcChild->top + lprcChild->bottom;
				}

			}
			break;

		case UIANCHOR_LB: //中间对齐
			{
				if (lprcChild->left < 0)
				{
					int offsetx = (lprcParent->right - lprcParent->left - lprcChild->right) / 2;
					lprcChild->left = lprcParent->left + offsetx;// + lprcChild->left;
					lprcChild->right = lprcChild->left + lprcChild->right;
				}
				else
				{
					lprcChild->left = lprcParent->left + lprcChild->left;
					lprcChild->right = lprcChild->left + lprcChild->right;
				}

				if (lprcChild->top < 0)
				{
					int offsety = (lprcParent->bottom - lprcParent->top - lprcChild->bottom) / 2;
					lprcChild->top = lprcParent->top + offsety;// + lprcChild->top;
					lprcChild->bottom = lprcChild->top + lprcChild->bottom;
				}
				else
				{
					lprcChild->top = lprcParent->bottom - lprcChild->top - lprcChild->bottom;
					lprcChild->bottom = lprcChild->top + lprcChild->bottom;
				}
			}
			break;

		case UIANCHOR_TOP: 
		{
			lprcChild->left = lprcParent->left + lprcChild->left;
			lprcChild->right = lprcParent->right - lprcChild->right;
			if (lprcChild->top < 0)
			{
				int offsety = (lprcParent->bottom - lprcParent->top - lprcChild->bottom) / 2;
				lprcChild->top = lprcParent->top + offsety;
				lprcChild->bottom = lprcChild->top + lprcChild->bottom;
			}
			else
			{
				lprcChild->top = lprcParent->top + lprcChild->top;
				lprcChild->bottom = lprcChild->top + lprcChild->bottom;
			}

		}
		break;

		case UIANCHOR_BOTTOM:
		{
			lprcChild->left = lprcParent->left + lprcChild->left;
			lprcChild->right = lprcParent->right - lprcChild->right;

			if (lprcChild->top < 0)
			{
				int offsety = (lprcParent->bottom - lprcParent->top - lprcChild->bottom) / 2;
				lprcChild->top = lprcParent->top + offsety;// + lprcChild->top;
				lprcChild->bottom = lprcChild->top + lprcChild->bottom;
			}
			else
			{
				lprcChild->top = lprcParent->bottom - lprcChild->top - lprcChild->bottom;
				lprcChild->bottom = lprcChild->top + lprcChild->bottom;
			}

		}
		break;

		case UIANCHOR_LEFT:
		{
			lprcChild->top = lprcParent->top + lprcChild->top;
			lprcChild->bottom = lprcParent->bottom - lprcChild->bottom;

			if (lprcChild->left < 0)
			{
				int offsetx = (lprcParent->right - lprcParent->left - lprcChild->right) / 2;
				lprcChild->left = lprcParent->left + offsetx;// + lprcChild->left;
				lprcChild->right = lprcChild->left + lprcChild->right;
			}
			else
			{
				
				lprcChild->left = lprcParent->left + lprcChild->left;
				lprcChild->right = lprcChild->left + lprcChild->right;
			}
		}
		break;

		case UIANCHOR_RIGHT:
		{
			lprcChild->top = lprcParent->top + lprcChild->top;
			lprcChild->bottom = lprcParent->bottom - lprcChild->bottom;

			if (lprcChild->left < 0)
			{
				int offsetx = (lprcParent->right - lprcParent->left - lprcChild->right) / 2;
				lprcChild->left = lprcParent->left + offsetx;// + lprcChild->left;
				lprcChild->right = lprcChild->left + lprcChild->right;
			}
			else
			{


				lprcChild->left = lprcParent->right - lprcChild->left - lprcChild->right;
				lprcChild->right = lprcChild->left + lprcChild->right;

			}
		}
		break;

	}
}

bool __IsRectIntersectScreen(RECT rc)
{
	POINT pt = {rc.left, rc.top};
	HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &monitorInfo);
	RECT rcMonitor;
	rcMonitor.left = monitorInfo.rcMonitor.left;
	rcMonitor.top = monitorInfo.rcMonitor.top;
	rcMonitor.right = monitorInfo.rcMonitor.right;//GetSystemMetrics(SM_CXSCREEN);
	rcMonitor.bottom = monitorInfo.rcMonitor.bottom;//GetSystemMetrics(SM_CYSCREEN);
	if ( (rc.left > rcMonitor.left && rc.top > rcMonitor.top)
		&& (rc.left < rcMonitor.right && rc.right < rcMonitor.right/* || rc.left > nWidth && rc.left < nScreenX && rc.right < nScreenX*/) 
		&& rc.top < rcMonitor.bottom && rc.bottom < rcMonitor.bottom/* || rc.top > nHeight && rc.top < nScreenY && rc.bottom < nScreenY*/)
		return true;
	return false;
}

bool __IsRectIntersectScreenLR(RECT rc)
{
	POINT pt = {rc.left, rc.top};
	HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &monitorInfo);
	RECT rcMonitor;
	rcMonitor.left = monitorInfo.rcMonitor.left;
	rcMonitor.top = monitorInfo.rcMonitor.top;
	rcMonitor.right = monitorInfo.rcMonitor.right;//GetSystemMetrics(SM_CXSCREEN);
	rcMonitor.bottom = monitorInfo.rcMonitor.bottom;//GetSystemMetrics(SM_CYSCREEN);
	if ( (rc.left > rcMonitor.left && rc.top > rcMonitor.top)
		&& (rc.left < rcMonitor.right && rc.right < rcMonitor.right/* || rc.left > nWidth && rc.left < nScreenX && rc.right < nScreenX*/) 
		/*&& rc.top < rcMonitor.bottom && rc.bottom < rcMonitor.bottom || rc.top > nHeight && rc.top < nScreenY && rc.bottom < nScreenY*/)
		return true;
	return false;
}



bool _CalcClipBox( __in IRenderDC* pRenderDC, __in RECT& rcIn, __out RECT& rcOut)
{
	ZeroMemory(&rcOut, sizeof(RECT));
	DibObj* pDibObj = pRenderDC->GetDibObj();
	if (!pDibObj) return false;

	RECT rectInvalidate = pRenderDC->GetUpdateRect();
	if (!IsRectEmpty(&rectInvalidate))
	{
		if (!IntersectRect(&rcOut, &rcIn, &rectInvalidate))
			return false;
	}
	else
		rcOut = rcIn;

	RECT clipBox = {0, 0, pDibObj->GetWidth(), pDibObj->GetHeight()};
	IntersectRect(&rcOut, &rcOut, &clipBox);
	return  !IsRectEmpty(&rcOut);
}


bool equal_icmp( LPCTSTR lpszValue1, LPCTSTR lpszValue2 )
{
	if (_tcslen(lpszValue1) == _tcslen(lpszValue2))
	{
		return !_tcsicmp(lpszValue1, lpszValue2);
	}
	return false;
}

bool equal_tstrcmp(LPCTSTR lpszValue1, LPCTSTR  lpszValue2)
{
	if (lpszValue1[0] != lpszValue2[0]) return false;
	return !_tcscmp(lpszValue1, lpszValue2);
}

bool equal_strcmp(const char* lpszValue1, const char*  lpszValue2)
{
	if (lpszValue1[0] != lpszValue2[0]) return false;
	return !strcmp(lpszValue1, lpszValue2);
}

bool equal_wstricmp( LPCWSTR lpszValue1, LPCWSTR lpszValue2 )
{
	if (wcslen(lpszValue1) == wcslen(lpszValue2))
	{
		return !_wcsicmp(lpszValue1, lpszValue2);
	}
	return false;
}

void copy_str(LPTSTR& lpszDest, LPCTSTR lpszSrc)
{
	if (lpszDest == lpszSrc) return;
	if (lpszDest) delete [] lpszDest;
	lpszDest = NULL;
	if (!lpszSrc) return;
	int nLen = _tcslen(lpszSrc);
	if (nLen > 0)
	{
		lpszDest = new TCHAR[nLen + 1];
		_tcsncpy(lpszDest, lpszSrc, nLen);
		lpszDest[nLen] = '\0';
	}
}

// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric)
{
	return (LONG) MulDiv(xHimetric, xPerInchScreenDC, HIMETRIC_PER_INCH);
}

// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric)
{
	return (LONG) MulDiv(yHimetric, yPerInchScreenDC, HIMETRIC_PER_INCH);
}

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx)
{
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInchScreenDC);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy)
{
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInchScreenDC);
}

inline UITYPE_ANCHOR StringToAnchor(LPCTSTR lpszValue)
{
	if (equal_tstrcmp(lpszValue, _T("LT")))
		return UITYPE_ANCHOR::UIANCHOR_LT;
	else if (equal_tstrcmp(lpszValue, _T("RT")))
		return UITYPE_ANCHOR::UIANCHOR_RT;
	else if (equal_tstrcmp(lpszValue, _T("RB")))
		return UITYPE_ANCHOR::UIANCHOR_RB;

	else if (equal_tstrcmp(lpszValue, _T("LB")))
		return UITYPE_ANCHOR::UIANCHOR_LB;

	else if (equal_tstrcmp(lpszValue, _T("LTRT")))
		return UITYPE_ANCHOR::UIANCHOR_LTRT;

	else if (equal_tstrcmp(lpszValue, _T("RTRB")))
		return UITYPE_ANCHOR::UIANCHOR_RTRB;

	else if (equal_tstrcmp(lpszValue, _T("LBRB")))
		return UITYPE_ANCHOR::UIANCHOR_LBRB;

	else if (equal_tstrcmp(lpszValue, _T("LTLB")))
		return UITYPE_ANCHOR::UIANCHOR_LTLB;

	else if (equal_tstrcmp(lpszValue, _T("LTRB")))
		return UITYPE_ANCHOR::UIANCHOR_LTRB;

	else if (equal_tstrcmp(lpszValue, _T("TOP")))
		return UITYPE_ANCHOR::UIANCHOR_TOP;

	else if (equal_tstrcmp(lpszValue, _T("RIGHT")))
		return UITYPE_ANCHOR::UIANCHOR_RIGHT;

	else if (equal_tstrcmp(lpszValue, _T("BOTTOM")))
		return UITYPE_ANCHOR::UIANCHOR_BOTTOM;

	else if (equal_tstrcmp(lpszValue, _T("LEFT")))
		return UITYPE_ANCHOR::UIANCHOR_LEFT;

	return UITYPE_ANCHOR::UIANCHOR_LTRB;
}

void PrasePosString(LPCTSTR lpszPosStr, int& l, int& t, int& r, int& b, UITYPE_ANCHOR& anchor)
{
	tstring spliter = lpszPosStr;
	anchor = UIANCHOR_LTRB;
	int pos[4] = {0, 0, 0, 0};
	short i = spliter.find_first_of(_T('|'));
	if (i > 0)
	{
		TCHAR c = spliter.c_str()[0];
		if (c >= _T('0') && c <= _T('9'))
		{
			anchor = (UITYPE_ANCHOR)_ttoi(spliter.substr(0, i).c_str());
		}
		else
		{
			anchor = StringToAnchor(spliter.substr(0, i).c_str());

		}
		
		spliter = spliter.substr(i + 1);
	}
	i = spliter.find_first_of(_T(','));
	unsigned char index = 0; 
	while (i >= 0)
	{
		pos[index] = _ttoi(spliter.substr(0, i).c_str());
		spliter = spliter.substr(i + 1);
		i = spliter.find_first_of(_T(','));
		index++;
	}
	pos[index] = _ttoi(spliter.c_str());
	l = pos[0];
	t = pos[1];
	r = pos[2];
	b = pos[3];

}
//////////////////////////////////////////////////////////////////////////

INotifyUI::INotifyUI()
{
	m_INotifySafe = new RefCountedThreadSafe<INotifyUI>;
	m_INotifySafe->SetValid(true);
	m_INotifySafe->SetCountedOwner(this);
	m_INotifySafe->AddRef();
}

INotifyUI::~INotifyUI()
{
	m_INotifySafe->SetValid(false);
	m_INotifySafe->SetCountedOwner(NULL);
	m_INotifySafe->Release();
}

RefCountedThreadSafe<INotifyUI>* INotifyUI::GetINotifySafe()
{
	return m_INotifySafe; 
}

//////////////////////////////////////////////////////////////////////////

VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
{
	if (NULL==lpSystemInfo)    return;
	typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	HMODULE hModule = GetModuleHandle(_T("kernel32"));
	if (hModule)
	{
		LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress( hModule, "GetNativeSystemInfo");;
		if (NULL != fnGetNativeSystemInfo)
		{
			fnGetNativeSystemInfo(lpSystemInfo);
		}
		else
		{
			GetSystemInfo(lpSystemInfo);
		}
	}
}
int GetSystemBits()
{
	static int s_nBits = 0;
	if (s_nBits == 0)
	{
		SYSTEM_INFO si;
		SafeGetNativeSystemInfo(&si);
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
		{
			s_nBits = 64;
		}
		else
		{
			s_nBits = 32;
		}
	}
	return s_nBits;
}

const wchar_t* GetCurrentIEVersion()
{
	static std::wstring s_strCurrentIEVersion = L"";
	if (s_strCurrentIEVersion.empty())
	{
		int nSysBits = GetSystemBits();
		WCHAR str[MAX_PATH];
		if (nSysBits == 64)
		{
			wsprintf(str,(L"SOFTWARE\\Wow6432Node\\Microsoft\\Internet Explorer"));
		}
		else if (nSysBits == 32)
		{
			wsprintf(str,(L"SOFTWARE\\Microsoft\\Internet Explorer"));
		}
		else
		{
			wsprintf(str,(L""));
		}
		if (wcslen(str) > 0)
		{
			HKEY hRegKey = NULL;
			RegOpenKeyW(HKEY_LOCAL_MACHINE, str, &hRegKey);
			if (hRegKey)
			{
				DWORD dwType = REG_SZ;
				DWORD dwLen = MAX_PATH;
				RegQueryValueExW(hRegKey, L"Version", NULL, &dwType, (LPBYTE)&str, &dwLen);
				if (dwLen > 0)
				{
					s_strCurrentIEVersion = str;
				}
			}
		}
	}
	return s_strCurrentIEVersion.c_str();
}
void InitWebbowserVersion()
{
	static bool s_initWebbowserVersion = false;
	if (s_initWebbowserVersion)
		return;
	WCHAR strOpenKey[MAX_PATH];
	WCHAR strSetKey[MAX_PATH] = {0};
	DWORD dwType = REG_DWORD;
	DWORD dwValue = 0;
	DWORD dwLen = sizeof(DWORD);
	{
		int nSysBits = GetSystemBits();
		if (nSysBits == 64)
		{
			wsprintf(strOpenKey,(L"SOFTWARE\\Wow6432Node\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION"));
		}
		else if (nSysBits == 32)
		{
			wsprintf(strOpenKey,(L"SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION"));
		}
		else
		{
			return;
		}
	}

	{
		WCHAR rtfPath[MAX_PATH];
		::GetModuleFileNameW(NULL, rtfPath, MAX_PATH);
		int nLen = wcslen(rtfPath);
		for (int i = nLen - 1; i >= 0; i--)
		{
			if ( rtfPath[i]=='\\' || rtfPath[i]=='/' )
			{
				wcscpy(strSetKey, rtfPath + (i + 1));
				break;
			}
		}
		if (wcslen(strSetKey) == 0)
		{
			wsprintf(strSetKey,(L"91uExternal.exe"));
		}
	}

	int nIEVersion = 0;
	{
		std::wstring strIEVersion = GetCurrentIEVersion();
		int nPos = strIEVersion.find_first_of('.');
		if (nPos == std::wstring::npos)
			nPos = strIEVersion.length();
		std::wstring strSub = strIEVersion.substr(0, nPos);
		nIEVersion = _wtoi(strSub.c_str());
		if (nIEVersion <= 7)
		{
			dwValue = 7000;  //Webpages containing standards-based !DOCTYPE directives are displayed in IE7 Standards mode.
		}
		else if (nIEVersion == 8)
		{
			dwValue = 8000; //Webpages containing standards-based !DOCTYPE directives are displayed in IE8 mode.
		}
		else if (nIEVersion == 9)
		{
			dwValue = 9000; //Webpages containing standards-based !DOCTYPE directives are displayed in IE9 mode.
		}
		else if (nIEVersion >= 10)
		{
			dwValue = 10000 ; //Webpages containing standards-based !DOCTYPE directives are displayed in IE10 Standards mode. 
		}

	}


	HKEY hRegKey = NULL;
	RegOpenKeyW(HKEY_LOCAL_MACHINE, strOpenKey, &hRegKey);
	if (hRegKey)
	{
		if (RegSetValueExW( hRegKey, strSetKey, 0, dwType, (LPBYTE)&dwValue, dwLen) == ERROR_SUCCESS)
			s_initWebbowserVersion = true;
	}

	if (nIEVersion >= 7)
	{

		WriteWebbowserRegKey(_T("FEATURE_SCRIPTURL_MITIGATION"),strSetKey,1); //IE7 UP
		
		if (nIEVersion >= 8)  //IE8 以上版本
		{
			WriteWebbowserRegKey(_T("FEATURE_LOCALMACHINE_LOCKDOWN"), strSetKey, 0);

			if (IsAlreadyWrittenReg(strSetKey)) //判断是否已修改注册表
			{
				return;
			}
			WriteWebbowserRegKey(_T("FEATURE_BROWSER_EMULATION"),strSetKey,7000);


		}

	}
}

bool WriteWebbowserRegKey(LPCTSTR lpKey,LPCTSTR lpSetKey,DWORD dwValue)
{
	HKEY hk = NULL;
	DWORD dwLen = sizeof(DWORD);
	tstring str = _T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\");
	str += lpKey;
	if (RegCreateKeyW(HKEY_CURRENT_USER,str.c_str(),&hk)!=0)return false;

	if (RegSetValueExW(hk,lpSetKey,NULL,REG_DWORD,(LPBYTE)&dwValue,dwLen)!=0)
	{
		RegCloseKey(hk);
		return false ;
	}

	RegCloseKey(hk);
	return true;
}

bool IsAlreadyWrittenReg(LPCTSTR lpSetKey)
{
	HKEY hk = NULL;
	tstring str = _T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION");

	if (RegOpenKeyExW(HKEY_CURRENT_USER,str.c_str(),0,KEY_READ,&hk) != ERROR_SUCCESS) return false;

	DWORD dwType = REG_DWORD;
	DWORD size = MAX_PATH;
	char result[256] = {0};
	if (ERROR_SUCCESS != RegQueryValueExW(hk,lpSetKey,0,&dwType,(LPBYTE)result,&size)) //获取系统IE版本
	{
		RegCloseKey(hk);
		return false;
	}

	RegCloseKey(hk);
	return true;

}
//////////////////////////////////////////////////////////////////////////

bool FindControlTest(CControlUI* pControl,  LPVOID pData, UINT uFlags)
{
	if( (uFlags & UIFIND_VISIBLE) != 0 && !pControl->IsVisible() ) return false;
	if( (uFlags & UIFIND_ENABLED) != 0 && !pControl->IsEnabled() ) return false;
	if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->HitTest(* static_cast<LPPOINT>(pData))) return false;
	return true;
}

void CalcParentPosWithAnchor(IN UITYPE_ANCHOR eAnchor, IN const RECT* lprcCoordinate, IN const RECT* lprcChild, OUT RECT* lprcParent)
{
	switch (eAnchor)
	{
	case UIANCHOR_LEFTTOP:
		{
			lprcParent->left = lprcChild->left - lprcCoordinate->left;
			lprcParent->top = lprcChild->top - lprcCoordinate->top;
			lprcParent->right = lprcChild->right;
			lprcParent->bottom = lprcChild->bottom;
		}
		break;
	case UIANCHOR_RIGHTTOP:
		{
			lprcParent->left = lprcChild->left;
			lprcParent->top = lprcChild->top - lprcCoordinate->top;
			lprcParent->right = lprcChild->right + lprcCoordinate->right;
			lprcParent->bottom = lprcChild->bottom;
		}
		break;
	case UIANCHOR_RIGHTBOTTOM:
		{
			lprcParent->left = lprcChild->left;
			lprcParent->top = lprcChild->top;
			lprcParent->right = lprcChild->right + lprcCoordinate->right;
			lprcParent->bottom = lprcChild->bottom + lprcCoordinate->bottom;
		}
		break;
	case UIANCHOR_LEFTBOTTOM:
		{
			lprcParent->left = lprcChild->left - lprcCoordinate->left;
			lprcParent->top = lprcChild->top;
			lprcParent->right = lprcChild->right;
			lprcParent->bottom = lprcChild->bottom + lprcCoordinate->bottom;
		}
		break;
	case UIANCHOR_LTRT:
		{
			lprcParent->left = lprcChild->left - lprcCoordinate->left;
			lprcParent->top = lprcChild->top - lprcCoordinate->top;
			lprcParent->right = lprcChild->right + lprcCoordinate->right;
			lprcParent->bottom = lprcChild->bottom;
		}
		break;
	case UIANCHOR_RTRB:
		{
			lprcParent->left = lprcChild->left;
			lprcParent->top = lprcChild->top - lprcCoordinate->top;
			lprcParent->right = lprcChild->right + lprcCoordinate->right;
			lprcParent->bottom = lprcChild->bottom + lprcCoordinate->bottom;
		}
		break;
	case UIANCHOR_LBRB:
		{
			lprcParent->left = lprcChild->left - lprcCoordinate->left;
			lprcParent->top = lprcChild->top;
			lprcParent->right = lprcChild->right + lprcCoordinate->right;
			lprcParent->bottom = lprcChild->bottom + lprcCoordinate->bottom;
		}
		break;
	case UIANCHOR_LTLB:
		{
			lprcParent->left = lprcChild->left - lprcCoordinate->left;
			lprcParent->top = lprcChild->top - lprcCoordinate->top;
			lprcParent->right = lprcChild->right;
			lprcParent->bottom = lprcChild->bottom + lprcCoordinate->bottom;
		}
		break;
	case UIANCHOR_LTRB:
		{
			lprcParent->left = lprcChild->left - lprcCoordinate->left;
			lprcParent->top = lprcChild->top - lprcCoordinate->top;
			lprcParent->right = lprcChild->right + lprcCoordinate->right;
			lprcParent->bottom = lprcChild->bottom + lprcCoordinate->bottom;
		}
		break;
	case UIANCHOR_LT:
		{
			if (lprcCoordinate->left < 0)
			{
				lprcParent->left = lprcChild->left;
				lprcParent->right = lprcChild->right;
			}
			else
			{
				if (lprcCoordinate->right < 0)
				{
					lprcParent->left = lprcChild->left;
					lprcParent->right = lprcChild->right + (-lprcCoordinate->right);
				}
				else
				{
					lprcParent->left = lprcChild->left - lprcCoordinate->left;
					lprcParent->right = lprcChild->right;
				}
			}


			if (lprcCoordinate->top < 0)
			{
				lprcParent->top = lprcChild->top;
				lprcParent->bottom = lprcChild->bottom;
			}
			else
			{
				if (lprcCoordinate->bottom < 0)
				{
					lprcParent->top = lprcChild->top;
					lprcParent->bottom = lprcChild->bottom + (-lprcCoordinate->bottom);
				}
				else
				{
					lprcParent->top = lprcChild->top - lprcCoordinate->top;
					lprcParent->bottom = lprcChild->bottom;
				}
			}
		}
		break;
	}

}
/////////////////////////////////////////////////////////////////////////////////
static map<CControlUI*, ItemHighlightMessageFilter*> m_mapMessageFilterList;
ItemHighlightMessageFilter::ItemHighlightMessageFilter(CControlUI* pOwner)
{
	m_pHighlight = NULL;
	m_pOwner = pOwner;
	CWindowUI* pWindow = m_pOwner->GetWindow();
	if (pWindow)
		pWindow->AddMessageFilter(this);
}

ItemHighlightMessageFilter::~ItemHighlightMessageFilter()
{
	CWindowUI* pWindow = m_pOwner->GetWindow();
	if (pWindow)
		pWindow->RemoveMessageFilter(this);

	if (m_pHighlight)
	{
		m_pHighlight->OnEvent -= MakeDelegate(this, &ItemHighlightMessageFilter::OnHighlightCtlEvent);
	}
}

CControlUI* ItemHighlightMessageFilter::GetOwner()
{
	return m_pOwner;
}


void ItemHighlightMessageFilter::AddToMessageFilterList(CControlUI* pOwner, ItemHighlightMessageFilter* pItemHighlightMessageFilter)
{
	map<CControlUI*, ItemHighlightMessageFilter*>::iterator it = m_mapMessageFilterList.find(pOwner);
	if (it != m_mapMessageFilterList.end()) return;

	if (pItemHighlightMessageFilter == NULL)
		pItemHighlightMessageFilter = new ItemHighlightMessageFilter(pOwner);

	m_mapMessageFilterList[pOwner] = pItemHighlightMessageFilter;
}

void ItemHighlightMessageFilter::RemoveFromMessageFilterList(CControlUI* pOwner)
{
	map<CControlUI*, ItemHighlightMessageFilter*>::iterator it = m_mapMessageFilterList.find(pOwner);
	if (it != m_mapMessageFilterList.end())
	{
		delete it->second;
		m_mapMessageFilterList.erase(it);
	}

}

LRESULT ItemHighlightMessageFilter::MessageFilter( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) 
{
	if (m_pOwner)
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			{
				if (!m_pOwner->IsEnabled() || !m_pOwner->IsVisible())
					break;

				POINT pt = {(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)};
				if (m_pOwner->HitTest(pt))
				{
					CControlUI* pOld = m_pHighlight;
					if (!pOld || !pOld->HitTest(pt))
					{
						CControlUI* pNew = ItemFromPos(pt);
						m_pHighlight = pNew;

						if (pOld)
						{
							pOld->OnEvent -= MakeDelegate(this, &ItemHighlightMessageFilter::OnHighlightCtlEvent);
							pOld->ModifyFlags(0, UICONTROLFLAG_ENABLE_DESTORY_EVENT);
						}
						if (pNew)
						{
							pNew->OnEvent += MakeDelegate(this, &ItemHighlightMessageFilter::OnHighlightCtlEvent);
							pNew->ModifyFlags(UICONTROLFLAG_ENABLE_DESTORY_EVENT, 0);
						}


						TEventUI event = {0};
						event.wParam = (WPARAM)pOld;
						event.lParam = (LPARAM)pNew;
						event.ptMouse = pt;
						event.nType = UIEVENT_ITEM_MOUSEHOVER_CHANGE;
						m_pOwner->SendEvent(event);

					}
				}
				else
				{
					CControlUI* pOld = m_pHighlight;
					if (pOld)
					{
						if (pOld)
						{
							pOld->OnEvent -= MakeDelegate(this, &ItemHighlightMessageFilter::OnHighlightCtlEvent);
							pOld->ModifyFlags(0, UICONTROLFLAG_ENABLE_DESTORY_EVENT);
						}

						m_pHighlight = NULL;
						TEventUI event = {0};
						event.wParam = (WPARAM)pOld;
						event.lParam = (LPARAM)NULL;
						event.ptMouse = pt;
						event.nType = UIEVENT_ITEM_MOUSEHOVER_CHANGE;
						m_pOwner->SendEvent(event);
					}
				}
			}

			break;
		}
	}
	return S_OK;
}

bool ItemHighlightMessageFilter::OnHighlightCtlEvent(TEventUI& event)
{
	if (event.nType == UIEVENT_DESTORY && m_pHighlight == event.pSender)
	{
		CControlUI* pOld = m_pHighlight;
		if (pOld)
		{
			if (pOld)
			{
				pOld->OnEvent -= MakeDelegate(this, &ItemHighlightMessageFilter::OnHighlightCtlEvent);
				pOld->ModifyFlags(0, UICONTROLFLAG_ENABLE_DESTORY_EVENT);
			}

			m_pHighlight = NULL;
			TEventUI e = {0};
			e.wParam = (WPARAM)pOld;
			e.lParam = (LPARAM)NULL;
			e.nType = UIEVENT_ITEM_MOUSEHOVER_CHANGE;
			m_pOwner->SendEvent(e);
		}
		m_pHighlight = NULL;
	}
	return true;
}

CControlUI* ItemHighlightMessageFilter::ItemFromPos(POINT& pt)
{
	int nCount = m_pOwner->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CControlUI* pControl = m_pOwner->GetItem(i);
		if (pControl && pControl->HitTest(pt) && pControl->IsVisible())
			return pControl;
	}
	return NULL;

}

UINT GET_DT_LEFT_EX_VALUE()
{
	if (GetUIEngine()->IsRTLLayout())
		return DT_RIGHT;
	return DT_LEFT;

}

UINT GET_DT_RIGHT_EX_VALUE()
{
	if (GetUIEngine()->IsRTLLayout())
		return DT_LEFT;
	return DT_RIGHT;
}