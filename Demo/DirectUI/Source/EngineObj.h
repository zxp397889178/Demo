/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-25       xqb
*********************************************************************/
#ifndef _ENGINEOBJ_H
#define  _ENGINEOBJ_H
#include <Richedit.h>
#include <Gesture.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 内部用的全局函数
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CControlUI* __stdcall __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
CControlUI* __stdcall __FindControlFromTab(CControlUI* pThis, LPVOID pData);
CControlUI* __stdcall __FindControlEnableDrop(CControlUI* pThis, LPVOID pData);
CControlUI* __stdcall __FindControlEnableDrag(CControlUI* pThis, LPVOID pData);
CControlUI* __stdcall __FindControlById(CControlUI* pThis, LPVOID pData);

bool  __IsRectIntersectScreen(RECT rc); //
bool  __IsRectIntersectScreenLR(RECT rc); //
bool _CalcClipBox( __in IRenderDC* pRenderDC, __in RECT& rcIn, __out RECT& rcOut);
//字符串比较
bool equal_icmp(LPCTSTR lpszValue1, LPCTSTR lpszValue2);
bool equal_strcmp(const char* lpszValue1, const char*  lpszValue2);
bool equal_tstrcmp(LPCTSTR lpszValue1, LPCTSTR  lpszValue2);
bool equal_wstricmp(LPCWSTR lpszValue1, LPCWSTR lpszValue2);
void copy_str(LPTSTR& lpszDest, LPCTSTR lpszSrc);
void InitWebbowserVersion();
bool WriteWebbowserRegKey(LPCTSTR lpKey,LPCTSTR lpSetKey,DWORD dwValue);
bool IsAlreadyWrittenReg(LPCTSTR lpSetKey);
//

void InitDefaultCharFormat(CControlUI* pControl, CHARFORMAT2* pcf, DWORD dwColor,  bool bRTLReading = true);
void InitDefaultParaFormat(CControlUI* pControl, PARAFORMAT2* ppf, bool bRTLReading = true);

#define HIMETRIC_PER_INCH 2540
#define LY_PER_INCH 1440
const LONG cInitTextMax = (32 * 1024) - 1;

extern HDC hdcGrobal;
extern Graphics* graphicsGlobal;
class  EngineObj:public IUIEngine
{
public:
	EngineObj();
	~EngineObj();
public:
	virtual void InitSkin(bool bRTLayout = false);
	virtual void UnInitSkin();
	virtual void SetInstanceHandle(HINSTANCE hInstance);
	virtual HINSTANCE GetInstanceHandle();
	virtual void SetIconResource(UINT uId, bool bBigIcon);
	virtual UINT GetIconResource(bool bBigIcon);
	virtual void GetOSVersion(DWORD& dwOSVer, DWORD& dwOSMinorVer);
	virtual tstring  GetRunDir();
	virtual bool IsRTLLayout();

	virtual int GetWindowCount();
	virtual CWindowUI* GetWindow(int nIndex);
	virtual CWindowUI* GetWindow(HWND hWnd);
	virtual CWindowUI* GetWindow(LPCTSTR lpszId);
	virtual CWindowUI* GetWindow(LPCTSTR lpszId, LPCTSTR lpszSubId);

	virtual void RegisterControl(LPCTSTR lpszType, CONTROLCREATEPROC lfCreateFn);
	virtual bool  AddControl(CControlUI* pParent, CControlUI* pNext, LPCTSTR lpszId);
	CControlUI* CreateControl(LPCTSTR lpszType);
	virtual void RegisterPlugin(LPCTSTR guid, CONTROLCREATEPROC lfCreateFn);
	CControlUI* CreatePlugin(LPCTSTR guid);
	virtual void RegisterStyle(LPCTSTR lpszType, STYLECREATEPROC lfCreateFn);
	StyleObj* CreateStyle(LPCTSTR lpszType, CControlUI* pOwner);
	
	/// Alpha
	virtual void SetAlpha(int nAlpha);
	virtual int  GetAlpha();

	virtual void ChangeSkin(LPCTSTR lpszSkinImage, LPCOLORREF lpSkinColor = NULL, bool bAnimation = true);
	virtual LPCTSTR GetSkinImage();
	virtual DWORD GetSkinColor();

	virtual bool PumpMessage();
	virtual int  MessageLoop();
	virtual bool TranslateMessage(const LPMSG pMsg);
	virtual void AddPreMessageFilter(IPreMessageFilterUI* pIMessageFilter);
	virtual void RemovePreMessageFilter(IPreMessageFilterUI* pIMessageFilter);
	virtual void SetPreNotify(INotifyUI* pNotify);

	virtual void RegisterUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule = NULL);
	virtual void UnRegisterUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule = NULL);
	virtual void UnRegisterUserEvent(UINT uEventID, HMODULE hModule = NULL);
	virtual void FireUserEvent(UINT uEventID, WPARAM wParam = NULL, LPARAM lParam = NULL, bool bImmediately = true, HMODULE hModule = NULL);

	virtual bool CheckControlPtrValid(CControlUI* pControl);
	void AddControlPtrToCheck(CControlUI* pControl);
	void RemoveControlPtrToCheck(CControlUI* pControl);

	INotifyUI* GetPreNotify();

	void InitOle();
	void InitCom();
	void InitGesture();
	void SkinWindow(CWindowUI* pWindow);
	void UnSkinWindow(CWindowUI* pWindow);
	void UnSkinWindow(HWND hWnd);
	HWND GetMsgHwnd(){return m_hWndMsg;};
	DWORD GetMainThreadId(){ return m_dwMainThreadId; };

	virtual  void EnableHDPI();
	virtual  bool IsEnableHDPI();
	virtual  int GetDPI();
	virtual  void SetDPI(int nDPI = 96, bool bAutoResizeWindow = true);
	virtual  CDPI* GetDPIObj();

	virtual  void SetDPIName(LPCTSTR lpszDPIName = _T(""));
	virtual  LPCTSTR GetDPIName();
	virtual bool SetLogPath(LPCTSTR lpszPath);

	static EngineObj* m_pUIEngine;
private:
	void RegisterModuleUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule);
	void UnRegisterModuleUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule);
	void UnRegisterModuleUserEvent(UINT uEventID, HMODULE hModule);
	CStdPtrArray  m_aPreMessageFilterList;		
	CStdPtrArray  m_aWindowList;	//保存皮肤化的窗口列表
	std::map<tstring, CONTROLCREATEPROC> m_mapControlList; //保存创建控件的接口
	std::map<tstring, CONTROLCREATEPROC> m_mapPluginList; //保存创建控件的接口
	std::map<tstring, STYLECREATEPROC> m_mapStyleList; //保存创建控件的接口
	INotifyUI* m_pPreNotify;
	HINSTANCE m_hInstance;
	UINT m_uBigIconId;
	UINT m_uSmallIconId;
	int m_nAlpha;	//透明度（0：全透，255：不透)
	COLORREF m_clrSkinColor;	
	ImageObj* m_pSkinImageObj;					//背景图片
	DWORD m_dwOSVersion;
	DWORD m_dwOSMinorVersion;
	bool m_bInitCom;
	bool m_bInitOle;
	HWND m_hWndMsg;
	bool m_bRTLLayout;
	DWORD m_dwMainThreadId;
	CDPI* m_pDPI;
	tstring m_strDPIName;
};

EngineObj* GetEngineObj();
CDPI* GetDPIObj();

using pfunc_get_gesture_info = BOOL(WINAPI *)(_In_ HGESTUREINFO hGestureInfo, _Out_ PGESTUREINFO pGestureInfo);
using pfunc_close_gesture_info_handle = BOOL(WINAPI *)(_In_ HGESTUREINFO hGestureInfo);
using pfunc_set_gesture_config = BOOL(WINAPI*)(__in HWND hwnd, __in DWORD dwReserved, __in UINT cIDs, __in_ecount(cIDs) PGESTURECONFIG pGestureConfig, __in UINT cbSize);

extern DUI_API pfunc_get_gesture_info get_gesture_info;
extern DUI_API pfunc_close_gesture_info_handle close_gesture_info_handle;
extern DUI_API pfunc_set_gesture_config set_gesture_config;



#endif // _ENGINEOBJ_H

