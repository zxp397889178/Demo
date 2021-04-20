#pragma once


class CWebPreviewInterface
{
public:
	virtual ~CWebPreviewInterface(){}

	virtual HWND      GetPreviewHwnd(){ return NULL; }
	virtual void      CloseWebPreview(){}
	virtual void      ReloadCefUrl(LPCTSTR lptcsUrl){}
	virtual HWND      GetWebCefWnd(){ return NULL; }
	virtual LPCTSTR		GetLoadedUrl(){ return _T(""); }
	virtual CControlUI*
		FindWndControl(LPCTSTR lptcsId){ return NULL; };
	virtual void
		AddWndNotifier(INotifyUI* pNotifier){ };
	virtual void
		RemoveWndNotifier(INotifyUI* pNotifier){ };

	virtual void		SetReceiveStatusChange(bool bChange){}

	virtual void		ShowCefUrl(const wstring& wstrUrl,
							int nType,
							int nQuestionType = 0,
							bool bFileReq = false,
							bool bCefHide = false){}

	virtual void        SetErrorLanguage(bool bLocal){}; 
	//cef
	virtual void		ExitCefWnd(){}//退出
	virtual void		BackOffCefWnd(){}//后退
	virtual void		ForwardCefWnd(){}//前进
	virtual void		ReloadCefWnd(){}//刷新
	virtual void		MiniPreviewWnd(){}//最小化
	virtual void		DoExecJs(std::wstring& wstrJs){}//执行Js

};

