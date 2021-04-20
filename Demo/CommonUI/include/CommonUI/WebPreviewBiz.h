#pragma once

class CWebPreviewInterface;
struct WebPreviewInfo;

class  CUI_API CWebPreviewBiz
{
public:
	CWebPreviewBiz();
	virtual ~CWebPreviewBiz();

public:
	virtual void        OnCreateBefore(WebPreviewInfo* pInfo);
	virtual void        ShowCefWnd(const tstring & strUrl);
	virtual void		InitWebWindow(CWebPreviewInterface* pWindow);
	virtual void        UpdateWindowPos(CWindowUI* pWindow, HWND hParent, float fWidthPercent, float fHeightPercent, float fConstraintSspectRatio);
	virtual void		SetReceiveStatusChange(bool bReceive);

	virtual void        RefreshCefWnd(); //刷新
	virtual void        ForwardCefWnd(); //前进
	virtual void        BackOffCefWnd(); //后退
	virtual void        MiniWnd(); //最小化窗口

	virtual void        DestroyWebWindow();

	virtual int			GetLinkPopupType();
	virtual void        ReloadCefUrl(LPCTSTR lptcsUrl);
	//WM_COPYDATA
	virtual void        OnCefInvokeMethodPpt(LPCSTR pszJsonInfo);
	virtual void        OnCefInvokeAsync(LPCSTR pszJsonInfo);
	virtual void        OnCefInvoke(LPCSTR pszJsonInfo);

	virtual void        OnCefUrlPopUp(LPCTSTR lptcsUrl);
	virtual void        OnCefUrlChange(LPCTSTR lptcsUrl);
	virtual bool        isRetryErrorUrl(bool bLocal = false);
	virtual	wstring		OnCefRetryErrorUrl();
	virtual void        OnCefLoaded();
	virtual void        SetErrorLanguage(bool bLocal);
protected:	
	CWebPreviewInterface*   m_pWindow;
};