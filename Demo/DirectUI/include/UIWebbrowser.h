#pragma once

#include <mshtmhst.h>
#include <mshtml.h>
#include <MsHtmdid.h>
#include <Exdisp.h>
#include <exdispid.h>
#include "UIWebView.h"


/*!
   \brief    js����C++����
   \note     
   \param    DISPPARAMS * pDispParams ����(��������)
   \param    VARIANT * pVarResult ����ֵ
   \param    LPVOID pControlUI �ؼ�ָ��
   \return   typedef STDMETHODIMP 
 ************************************/
//{{
typedef STDMETHODIMP Webbrowser_Client_Callback(DISPPARAMS* pDispParams, VARIANT* pVarResult, LPVOID pControlUI);
//}}
class DUI_API Webbrowser_IMenu
{
public:
	virtual bool OnShowContentMenu(DWORD dwID, POINT *ppt, LPCTSTR lpszID, LPCTSTR lpszTagname, IDispatch* pTarget, CWebBrowserUI* pControl) = 0;
};

class WebbrowserExternal_STL_IMPL;
class DUI_API WebbrowserExternal : public IDispatch
{
	//{{
public:
	WebbrowserExternal();
	virtual ~WebbrowserExternal(void);
	//}}
	void SetOwner(CWebBrowserUI* pOwner);
public:
	//{{
	//IDispatch�̳к���
	STDMETHOD(QueryInterface)(REFIID iid,void**ppvObject);
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());
	STDMETHOD(GetTypeInfoCount)(unsigned int * pctinfo);
	STDMETHOD(GetTypeInfo)(unsigned int iTInfo,LCID lcid,ITypeInfo FAR* FAR* ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames, LCID lcid, DISPID FAR* rgDispId );
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, unsigned int* puArgErr);

public:
	void AddExtFunc(LPCTSTR lpszMethodName, Webbrowser_Client_Callback* extMethod);
	Webbrowser_Client_Callback* GetExtFunc(LPCTSTR lpszMethodName);
	//}}
	void AddExtDisp(LPCTSTR lpszDispName, IDispatch* extDisp);
	IDispatch* GetExtDisp(LPCTSTR lpszDispName);
	//{{
protected:
	CWebBrowserUI* m_pOwner;
	long _refNum;

private:
	WebbrowserExternal_STL_IMPL* map_impl;
	//}}
};


class CWebBrowserImpl;
/*!
    \brief    ��ҳ�ؼ�
*****************************************/
class DUI_API CWebBrowserUI : public CControlUI, public IWebViewUI
{
	//{{
	friend class CWebBrowserImpl;
	//}}
	UI_OBJECT_DECLARE(CWebBrowserUI, _T("Webbrowser"))
public:
	typedef enum
	{
		WebbrowserMenuMode_DefaultMenuSupport = 0,
		WebbrowserMenuMode_NoContextMenu,
		WebbrowserMenuMode_TextSelectionOnly,
		WebbrowserMenuMode_AllowAllButViewSource,
		WebbrowserMenuMode_CustomMenuSupport,
		WebbrowserMenuMode_WebContextMenuModeLimit
	}WebbrowserContextMenuMode;
	//{{
public:
	CWebBrowserUI();
	virtual ~CWebBrowserUI();
	//}}


	//************************************
	// @brief:    ��ȡIWebBrowser2��ָ��
	// @note:     
	// @return:   IWebBrowser2* 
	//************************************
	IWebBrowser2* GetWebBrowser2();
	IHTMLDocument2* GetDocument2();
	IHTMLDocument3* GetDocument3();
	IHTMLDocument4* GetDocument4();
	virtual HWND GetHostWindow();
	void SetDWebBrowserEvents2(DWebBrowserEvents2* pEvents2);

	IDispatch* GetHtmlWindow();
	HRESULT GetControl(const IID iid, LPVOID* ppRet);
	bool GetBusy();
	tstring GetLocationName();
	tstring GetLocationURL();

	// IWebViewUI
	virtual void LoadUrl(LPCTSTR lpszUrl);
	virtual void LoadUrl(LPCTSTR lpszUrl, LPCTSTR lpszPostData, UINT flags = 0, LPCTSTR lpszHeaders = NULL);
	virtual bool CanForward();
	virtual bool CanGoBack();
	virtual void Forward();
	virtual void GoBack();
	virtual void Refresh();

	virtual void EnableContextMenu(bool bEnable);
	virtual bool IsEnableContextMenu();

	virtual double GetZoomLevel();
	virtual void SetZoomLevel(double zoomLevel);

	void Stop();
	bool ExecWB(
		/* [in] */ OLECMDID cmdID,
		/* [in] */ OLECMDEXECOPT cmdexecopt,
		/* [optional][in] */ __RPC__in VARIANT *pvaIn,
		/* [optional][in][out] */ __RPC__inout VARIANT *pvaOut);

	bool HaveSelection();
	bool SetElementInnerHtml(LPOLESTR lpszID, LPOLESTR lpszHtml);
	bool GetElementInnerHtml(LPOLESTR lpszID, tstring& strText);
	bool GetElementInnerText(LPOLESTR lpszID, tstring& strText);
	IDispatch* GetElement(LPOLESTR lpszID);
	bool GetSelectionHtml(tstring& strText);
	bool GetSelectionText(tstring& strText);

	//����falseִ��Ĭ�ϴ�����
	virtual bool PreMessageHandler(const LPMSG pMsg){return false;}
	virtual void OnWindowClose();
	//�̳к���
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void SetRect(RECT& rectRegion);
	virtual void SetVisible(bool bVisible);
	virtual void SetInternVisible(bool bVisible);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void OnDestroy();
//{{
protected:
	void InitControl();
	bool DoCreateControl();
	virtual void ReleaseControl();
	virtual void OffsetBrowser();
	
protected:
	CWebBrowserImpl* m_pBrowserImpl;
	HWND m_hWebBrowser; // ����ؼ��Ķ��㴰��
	bool m_bCreated;
private:
	
	/*!
	   \brief    ����ҳ
	   \param    LPCTSTR lpszUrl Ŀ��url
	   \param    LPCTSTR lpszPostData Ϊ��ʱΪGET����
	   \param    UINT flags ����BrowserNavConstants
	   \param    LPCTSTR lpszHeaders HTTP headers
	   \return   void 
	 ************************************/
	virtual void Show(LPCTSTR lpszUrl, LPCTSTR lpszPostData = NULL, UINT flags = 0, LPCTSTR lpszHeaders = NULL);

	HRESULT RegisterEventHandler(bool inAdvise);
	DWebBrowserEvents2* m_pDWebBrowserEvents2;
	DWORD m_dwCookie_DWebBrowserEvents2;
//}}
public:

	//************************************
	// @brief:    �Ƿ����������ҷ����
	// @note:     
	// @param: 	  bool bEnable 
	// @return:   void 
	//************************************
	void SetEnableDrop(bool bEnable){m_bEnableDrop = bEnable;}
	bool IsEnableDrop(){return m_bEnableDrop;}
	//////////////////////////////////////////////////////////////////////////
	// ���·��������Ի��ڲ������Զ���UIHandler
public:
	//************************************
	// @brief:    �Ƿ�����ʹ��window.external
	// @note:     IDocHostUIHandler GetExternal
	// @param: 	  bool bEnable 
	// @return:   void 
	//************************************
	void SetEnableExternal(bool bEnable){m_bEnableExternal = bEnable;}
	bool IsEnableExternal(){return m_bEnableExternal;}

	//************************************
	// @brief:    �Ƿ�����ѡ������
	// @note:     IDocHostUIHandler GetHostInfo
	// @param: 	  bool bEnable 
	// @return:   void 
	//************************************
	void SetEnableSelect(bool bEnable){m_bEnableSelect = bEnable;}
	bool IsEnableSelect(){return m_bEnableSelect;}

	//************************************
	// @brief:    �Ƿ���ʾ������
	// @note:     IDocHostUIHandler GetHostInfo
	// @param: 	  bool bEnable 
	// @return:   void 
	//************************************
	void SetEnableScroll(bool bEnable){m_bEnableScroll = bEnable;}
	bool IsEnableScroll(){return m_bEnableScroll;}

	//************************************
	// @brief:    ���ò˵�ģʽ
	// @note:     ���Ȼص�MenuCall     IDocHostUIHandler ShowContextMenu
	// @param: 	  WebbrowserContextMenuMode nMode 
	// @return:   void 
	//************************************
	void SetContextMenuMode(WebbrowserContextMenuMode nMode){m_contextMenuMode = nMode;}
	int GetContextMenuMode(){ return m_contextMenuMode; }

	//************************************
	// @brief:    �Ƿ����������ļ�
	// @note:     DWebBrowserEvents2 Invoke
	// @param: 	  bool bEnable 
	// @return:   void 
	//************************************
	void SetEnableDownload(bool bEnable){m_bEnableDownload = bEnable;}
	bool IsEnableDownload(){return m_bEnableDownload;}

	/*!
	   \brief    �Ƿ�������´���
	   \note     DWebBrowserEvents2 Invoke
	   \param    bool bEnable 
	   \return   void 
	 ************************************/
	void SetEnableNewWindow(bool bEnable){m_bEnableNewWindow = bEnable;}
	bool IsEnableNewWindow(){return m_bEnableNewWindow;}

	/*!
	   \brief    �Ƿ�����Ƕ���ڴ��´���
	   \note     DWebBrowserEvents2 Invoke
	   \param    bool bEnable 
	   \return   void 
	 ************************************/
	void SetEnableNativeWeb(bool bEnable){m_bEnableNativeWeb = bEnable;}
	bool IsEnableNativeWeb(){return m_bEnableNativeWeb;}

	/*!
	   \brief    �Ƿ����νű�����
	   \param    bool bEnable 
	   \return   void 
	 ************************************/
	void SetEnableScriptErrorsSuppressed(bool bEnable){m_bEnableScriptErrorsSuppressed = bEnable;}
	bool IsEnableScriptErrorsSuppressed(){return m_bEnableScriptErrorsSuppressed;}

	//************************************
	// @brief:    �Ƿ������Զ�����ҳ����·��
	// @note:     DWebBrowserEvents2 Invoke
	// @param: 	  bool bEnable 
	// @return:   void 
	//************************************
	void SetEnableRedirect(bool bEnable){m_bEnableRedirect = bEnable;}
	bool IsEnableRedirect(){return m_bEnableRedirect;}
	void SetCookiesDir(LPCTSTR lpszText){m_strCookiesPath = lpszText;}
	LPCTSTR GetCookiesDir(){return m_strCookiesPath.c_str();}
	void SetCacheDir(LPCTSTR lpszText){m_strCachePath = lpszText;}
	LPCTSTR GetCacheDir(){return m_strCachePath.c_str();}

	//************************************
	// @brief:    �Ƿ���ʾ���ع��̴���
	// @note:     DWebBrowserEvents2 Invoke
	// @param: 	  bool bEnable 
	// @return:   void 
	//************************************
	void SetEnableLoading(bool bEnable){m_bEnableLoading = bEnable;}
	bool IsEnableLoading(){return m_bEnableLoading;}
	void SetLoadingWindow(CWindowUI* pWindow){m_pLoadingWindow = pWindow;}
	CWindowUI* GetLodingWindow(){return m_pLoadingWindow;}

	//************************************
	// @brief:    �Զ���External
	// @note:     
	// @param: 	  IDispatch * pExternal 
	// @return:   void 
	//************************************
	void SetExternalCall(IDispatch* pExternal){m_pWindowExternal = pExternal;}
	IDispatch* GetExternalCall(){return m_pWindowExternal;}

	//************************************
	// @brief:    �Զ���˵���ʾ�ص�
	// @note:     �����ͷ�
	// @param: 	  Webbrowser_IMenu * pMenu 
	// @return:   void 
	//************************************
	void SetMenuCall(Webbrowser_IMenu* pMenu){m_pWebMenu = pMenu;}
	Webbrowser_IMenu* GetMenuCall(){return m_pWebMenu;}

	void SetDocHostFlags(DWORD uFlags){m_dwDocHostUIFlag = uFlags;}
	DWORD GetDocHostFlags(){return m_dwDocHostUIFlag;}

	//{{
	// ������ҳ����c++����
	void AddExtFunc(LPCTSTR lpszMethodName, Webbrowser_Client_Callback* extMethod);
	Webbrowser_Client_Callback* GetExtFunc(LPCTSTR lpszMethodName);
	//}}
	void AddExtDisp(LPCTSTR lpszDispName, IDispatch* extDisp);
	IDispatch* GetExtDisp(LPCTSTR lpszDispName);

	//{{
	tstring m_strTranslateUrl;
private:
	Webbrowser_IMenu* m_pWebMenu;
	IDispatch* m_pWindowExternal;
	IDispatch* m_pDocumentCompleteDispatch;

private:
	bool m_bEnableBack;
	bool m_bEnableNext;
	bool m_bEnableScroll;		//�Ƿ����������
	bool m_bEnableSelect;		//�Ƿ�����ѡ��
	bool m_bEnableNewWindow;	//�Ƿ������´���
	bool m_bEnableDownload;		//�Ƿ���������
	bool m_bEnableExternal;		//�Ƿ�����window.external����
	bool m_bEnableRedirect;		//�Ƿ�����ı�·��
	bool m_bChangedDirect;		//�Ƿ�ı������·��
	bool m_bEnableNativeWeb;	//�´��ڵ���������web���ڣ���Ҫm_bEnableNewWindowΪtrue
	bool m_bEnableScriptErrorsSuppressed;	//�Ƿ����νű�����
	wstring m_strCookiesPath;
	wstring m_strCachePath;
	bool m_bEnableDrop;			//�Ƿ�����drop
	bool m_bEnableLoading;		//�Ƿ�������ʾ���봰��
	CWindowUI* m_pLoadingWindow;//���봰��
	WebbrowserContextMenuMode m_contextMenuMode;		//�˵�ģʽ
	DWORD m_dwDocHostUIFlag;

public:
	//�����ص���������SetDWebBrowserEvents2�����
	virtual void OnBeforeNavigate(IDispatch *pDispatch, LPCWSTR pszURL, DWORD dwFlags, LPCWSTR pszTargetFrameName, VARIANT *vtPostData, LPCWSTR pszHeaders, VARIANT_BOOL *Cancel);
	virtual void OnNavigateError(IDispatch *pDispatch, LPCWSTR pszURL, LPCWSTR pszTargetFrameName, INT nStatusCode, VARIANT_BOOL *Cancel);
	virtual void OnNavigateComplete(IDispatch *pDispatch, LPCWSTR pszURL);
	virtual void OnDocumentComplete(IDispatch *pDisp, LPCWSTR pszURL);
	virtual void OnFileDownload(VARIANT_BOOL *ActiveDocument, VARIANT_BOOL *Cancel);
	virtual void OnNewWindow2(IDispatch **ppDisp, VARIANT_BOOL *Cancel);
	virtual void OnProgressChange(long Progress, long ProgressMax);
	virtual void OnStatusTextChange(LPCWSTR pszText);
	virtual void OnCommandStateChange(long nCommand, bool bEnable);
	virtual void OnDocumentLode(bool bDone);   //nProgress = -1ʱ���������
	virtual void OnTitleChange(LPCWSTR pszText);
	STDMETHOD(DWebBrowserEvents2Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, unsigned int* puArgErr);

private:
	void GetRegDefaultValue();
	void SetRegValue(wstring strCookiesPath, wstring strCachePath);

	static wstring m_strDefaultCookiesPath;
	static wstring ms_strDefaultCachePath;
	
public:
	static DISPID	FindId(IDispatch *pObj, LPOLESTR pName);
	static HRESULT	InvokeMethod(IDispatch *pObj, LPOLESTR pMethod, VARIANT *pVarResult, VARIANT *ps, int cArgs);

	//}}
};

