#pragma once
class DUI_API IWebViewUI
{
public:
	virtual void LoadUrl(LPCTSTR lpszUrl) = 0;
	virtual void LoadUrl(LPCTSTR lpszUrl, LPCTSTR lpszPostData, UINT flags = 0, LPCTSTR lpszHeaders = NULL) = 0;

	virtual bool CanForward() = 0;
	virtual bool CanGoBack() = 0;
	virtual void Forward() = 0;
	virtual void GoBack() = 0;
	virtual void Refresh() = 0;

	virtual void EnableContextMenu(bool bEnable) = 0;
    virtual void SetEnableNewWindow(bool bEnable) = 0;
	virtual void SetEnableNativeWeb(bool bEnable) = 0;

	virtual double GetZoomLevel() = 0;
	virtual void SetZoomLevel(double zoomLevel) = 0;

	// 这个接口怪怪的，以后会去掉
	virtual void SetLoadingWindow(CWindowUI* pWindow){};

	
};
