#include "stdafx.h"
#include "UIWebbrowser.h"
#include "activex/activex_inner.h"

//////////////////////////////////////////////////////////////////////////
#define GET_SAFE_DISP_BSTR(_val) ((_val.pvarVal && VT_BSTR == _val.pvarVal->vt) ? _val.pvarVal->bstrVal : NULL)
#define GET_SAFE_DISP_I4(_val) ((_val.pvarVal && VT_I4 == _val.pvarVal->vt) ? _val.pvarVal->intVal : 0)

#define WEBBROWSER_DISPID_EXTBASE 0x1000

class WebbrowserExternal_STL_IMPL
{
public:
	int m_nLastFuncID;
	int m_nLastDispID;
	std::map<tstring, int> m_FuncNameToID;
	std::map<int, Webbrowser_Client_Callback*> m_IDToFuncCallback;
	std::map<tstring, int> m_IDispNameToID;
	std::map<int, IDispatch*> m_IDToDispatch;
};

//////////////////////////////////////////////////////////////////////////
WebbrowserExternal::WebbrowserExternal()
{
	m_pOwner = NULL;
	_refNum = 0;
	map_impl = new WebbrowserExternal_STL_IMPL;
	map_impl->m_nLastFuncID = WEBBROWSER_DISPID_EXTBASE;
	map_impl->m_nLastDispID = 2*WEBBROWSER_DISPID_EXTBASE;
}
WebbrowserExternal::~WebbrowserExternal(void)
{
	if (map_impl)
		delete map_impl;
	map_impl = NULL;
}

STDMETHODIMP WebbrowserExternal::QueryInterface(REFIID iid,void**ppvObject)
{
	*ppvObject = NULL;
	if (iid == IID_IUnknown)	*ppvObject = this;
	else if (iid == IID_IDispatch)	*ppvObject = (IDispatch*)this;
	if( *ppvObject != NULL ) AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) WebbrowserExternal::AddRef()
{
	return ::InterlockedIncrement(&_refNum);
}

STDMETHODIMP_(ULONG) WebbrowserExternal::Release()
{
	return ::InterlockedDecrement(&_refNum);
}
// IDispatch Methods

STDMETHODIMP WebbrowserExternal::GetTypeInfoCount(
	unsigned int * pctinfo) 
{
	return E_NOTIMPL;
}

STDMETHODIMP WebbrowserExternal::GetTypeInfo(
									 unsigned int iTInfo,
									 LCID lcid,
									 ITypeInfo FAR* FAR* ppTInfo) 
{
	return E_NOTIMPL;
}

STDMETHODIMP WebbrowserExternal::GetIDsOfNames(
									   REFIID riid, 
									   OLECHAR FAR* FAR* rgszNames, 
									   unsigned int cNames, 
									   LCID lcid, 
									   DISPID FAR* rgDispId 
									   )
{

	if (cNames > 0)
	{
		std::map<tstring, int>::iterator posFuncNameToID = map_impl->m_FuncNameToID.find(tstring(rgszNames[0]));
		if (posFuncNameToID != map_impl->m_FuncNameToID.end())
		{
			// It is.
			*rgDispId = posFuncNameToID->second;
			return S_OK;
		}
		else
		{
			{
				std::map<tstring, int>::iterator posIDispNameToID = map_impl->m_IDispNameToID.find(tstring(rgszNames[0]));
				if (posIDispNameToID != map_impl->m_IDispNameToID.end())
				{
					*rgDispId = posIDispNameToID->second;
					return S_OK;
				}
			}
			// It isn't.
			*rgDispId = DISPID_UNKNOWN;
			return DISP_E_UNKNOWNNAME;
		}
	}
	return E_NOTIMPL;
}

STDMETHODIMP WebbrowserExternal::Invoke(
								DISPID dispIdMember,
								REFIID riid,
								LCID lcid,
								WORD wFlags,
								DISPPARAMS* pDispParams,
								VARIANT* pVarResult,
								EXCEPINFO* pExcepInfo,
								unsigned int* puArgErr
								)
{
	std::map<int, Webbrowser_Client_Callback*>::iterator pos = map_impl->m_IDToFuncCallback.find(dispIdMember);
	if (pos != map_impl->m_IDToFuncCallback.end())
	{
		pos->second(pDispParams, pVarResult, m_pOwner);
		return S_OK;
	}
	std::map<int, IDispatch*>::iterator posDisp = map_impl->m_IDToDispatch.find(dispIdMember);
	if (posDisp != map_impl->m_IDToDispatch.end())
	{
		VariantInit(pVarResult);
		V_VT(pVarResult) = VT_DISPATCH;
		V_DISPATCH(pVarResult) = posDisp->second;
		return S_OK;
	}
	return DISP_E_MEMBERNOTFOUND;
}

void WebbrowserExternal::AddExtDisp( LPCTSTR lpszDispName, IDispatch* extDisp )
{
	tstring strDispName = lpszDispName;
	map_impl->m_IDToDispatch[map_impl->m_nLastDispID] = extDisp;
	map_impl->m_IDispNameToID[strDispName] = map_impl->m_nLastDispID;
	map_impl->m_nLastDispID++;
}

IDispatch* WebbrowserExternal::GetExtDisp(LPCTSTR lpszDispName)
{
	std::map<tstring, int>::iterator posName = map_impl->m_IDispNameToID.find(lpszDispName);

	if ( posName != map_impl->m_IDispNameToID.end() )
	{
		std::map<int, IDispatch*>::iterator posDisp = map_impl->m_IDToDispatch.find(posName->second);
		if ( posDisp != map_impl->m_IDToDispatch.end() )
		{
			return posDisp->second;
		}
	}
	return NULL;
}

void WebbrowserExternal::AddExtFunc(LPCTSTR lpszMethodName, Webbrowser_Client_Callback* extMethod)
{
	tstring strMethodName = lpszMethodName;
	map_impl->m_IDToFuncCallback[map_impl->m_nLastFuncID] = extMethod;
	map_impl->m_FuncNameToID[strMethodName] = map_impl->m_nLastFuncID;
	map_impl->m_nLastFuncID++;
}

Webbrowser_Client_Callback* WebbrowserExternal::GetExtFunc( LPCTSTR lpszMethodName )
{
	std::map<tstring, int>::iterator posName = map_impl->m_FuncNameToID.find(lpszMethodName);

	if ( posName != map_impl->m_FuncNameToID.end() )
	{
		std::map<int, Webbrowser_Client_Callback*>::iterator posDisp = map_impl->m_IDToFuncCallback.find(posName->second);
		if ( posDisp != map_impl->m_IDToFuncCallback.end() )
		{
			return posDisp->second;
		}
	}
	return NULL;
}

void WebbrowserExternal::SetOwner( CWebBrowserUI* pOwner )
{
	m_pOwner = pOwner;
}

//////////////////////////////////////////////////////////////////////////
class WebbrowserTopWnd : public CWindowBase
{
public:
	WebbrowserTopWnd(CWebBrowserUI* pOwner){m_pOwner = pOwner;}
	virtual ~WebbrowserTopWnd(){m_pOwner = NULL;}
	bool PreMessageHandler(const LPMSG pMsg, LRESULT& lRes)
	{
		bool bRet = false;
		HWND hWnd = pMsg->hwnd;
		if (hWnd)
		{
			bRet = m_pOwner->PreMessageHandler(pMsg);
			if (!bRet)
			{
				UINT uMsg = pMsg->message;
				if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
				{
					if (!bRet)
					{
						IWebBrowser2* pWeb2 = m_pOwner->GetWebBrowser2();
						if (pWeb2)
						{
							util_activex::ScopedComPtr<IOleInPlaceActiveObject> oleActiveObj;
							oleActiveObj.QueryFrom(pWeb2);
							if (oleActiveObj.get())
							{
								HRESULT hr = oleActiveObj->TranslateAccelerator(pMsg);
								if (hr == S_OK)
								{
									bRet = true;
								}
								else
								{
									bRet = false;
								}
							}
						}
					}
				}
			}
		}
		return bRet;
	}
protected:
private:
	CWebBrowserUI* m_pOwner;
};
//////////////////////////////////////////////////////////////////////////
class Webbrowser_Ctrl : 
	public util_activex::AxHost,
	public IDocHostUIHandler, 
	public IOleCommandTarget
{
public:
	Webbrowser_Ctrl(CWebBrowserImpl* pOwnerImpl);
	void SetOwner(CWebBrowserImpl* pOwner){m_pOwner = pOwner;}
	CWebBrowserImpl* m_pOwner;

	//IUnknown
	STDMETHOD_(ULONG, AddRef)()
	{ return AxHost::AddRef(); }
	STDMETHOD_(ULONG, Release)()
	{ return AxHost::Release(); }
	STDMETHOD(QueryInterface)(REFIID iid,void**ppvObject)
	{
		*ppvObject = NULL;
		if (iid == IID_IDocHostUIHandler)	
			*ppvObject = static_cast<IDocHostUIHandler*>(this);
		else if (iid == IID_IOleCommandTarget)	
			*ppvObject = static_cast<IOleCommandTarget*>(this);

		if( *ppvObject != NULL ) 
		{
			AxHost::AddRef();
			return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
		}
		else
			return __super::QueryInterface(iid, ppvObject);
	}

	//IOleCommandTarget
	STDMETHOD(QueryStatus)( 
		const GUID *pguidCmdGroup,
		ULONG cCmds,
		OLECMD prgCmds[  ],
		OLECMDTEXT *pCmdText )
	{ return OLECMDERR_E_NOTSUPPORTED; }
	STDMETHOD(Exec)( 
		const GUID *pguidCmdGroup,
		DWORD nCmdID,
		DWORD nCmdexecopt,
		VARIANT *pvaIn,
		VARIANT *pvaOut);

	//IDocHostUIHandler
	STDMETHOD(ShowContextMenu)(DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved);
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame,IOleInPlaceUIWindow *pDoc)
	{ return E_NOTIMPL; }
	STDMETHOD(HideUI)(void)
	{ return E_NOTIMPL; }
	STDMETHOD(UpdateUI)(void)
	{ return E_NOTIMPL; }
	STDMETHOD(EnableModeless)(BOOL fEnable)
	{ return E_NOTIMPL; }
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate)
	{ return E_NOTIMPL; }
	STDMETHOD(OnFrameWindowActivate)( BOOL fActivate)
	{ return E_NOTIMPL; }
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder,IOleInPlaceUIWindow *pUIWindow,BOOL fRameWindow)
	{ return E_NOTIMPL; }
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID)
	{ return E_NOTIMPL; }
	STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey,DWORD dw)
	{ return E_NOTIMPL; }
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget,IDropTarget **ppDropTarget)
	{ return E_NOTIMPL; }
	STDMETHOD(GetExternal)( IDispatch **ppDispatch); // 连接控件和WebbrowserExternal的函数
	STDMETHOD(TranslateUrl)(DWORD dwTranslate,OLECHAR *pchURLIn,OLECHAR **ppchURLOut);
	STDMETHOD(FilterDataObject)(IDataObject *pDO,IDataObject **ppDORet)
	{ return E_NOTIMPL; }
protected:
private:
};

class DWebBrowserEvents2Impl : public DWebBrowserEvents2
{
public:
	DWebBrowserEvents2Impl(CWebBrowserUI* pOwner):m_pOwner(pOwner){}
	virtual ~DWebBrowserEvents2Impl()
	{
		if (m_pOwner) 
			m_pOwner->SetDWebBrowserEvents2(NULL);
		m_pOwner = NULL;
	}
	CWebBrowserUI* m_pOwner;

	//IUnknown
	STDMETHOD_(ULONG, AddRef)()
	{ return 1; }
	STDMETHOD_(ULONG, Release)()
	{ return 1; }
	STDMETHOD(QueryInterface)(REFIID iid,void**ppvObject)
	{
		*ppvObject = NULL;
		if (iid == DIID_DWebBrowserEvents2)
			*ppvObject = static_cast<DWebBrowserEvents2*>(this);
		if( *ppvObject != NULL ) 
			AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}

	//IDispatch
	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo)
	{
		if(pctinfo == NULL)  
			return E_POINTER; 
		*pctinfo = 1;
		return S_OK;
	}
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
	{ return E_NOTIMPL; }
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
	{ return E_NOTIMPL; }
	STDMETHOD(Invoke)(
		DISPID dispIdMember, 
		REFIID riid, 
		LCID lcid, 
		WORD wFlags, 
		DISPPARAMS FAR *pDispParams, 
		VARIANT FAR *pVarResult, 
		EXCEPINFO FAR * pExcepInfo, 
		unsigned int FAR *puArgErr)
	{
		CWebBrowserUI* pWeb = m_pOwner;
		if (pWeb)
		{
			switch (dispIdMember)
			{ 
			case DISPID_BEFORENAVIGATE2:
				pWeb->OnBeforeNavigate( pDispParams->rgvarg[6].pdispVal, GET_SAFE_DISP_BSTR(pDispParams->rgvarg[5]),
					GET_SAFE_DISP_I4(pDispParams->rgvarg[4]), GET_SAFE_DISP_BSTR(pDispParams->rgvarg[3]),
					pDispParams->rgvarg[2].pvarVal, GET_SAFE_DISP_BSTR(pDispParams->rgvarg[1]), pDispParams->rgvarg[0].pboolVal);
				break;
			case DISPID_NAVIGATEERROR:
				{
					VARIANT * vt_statuscode = pDispParams->rgvarg[1].pvarVal;
					DWORD  dwStatusCode =  vt_statuscode->lVal;
					if (dwStatusCode == 200)
					{
						*pDispParams->rgvarg[0].pboolVal = VARIANT_TRUE;
						break;
					}
					pWeb->OnNavigateError(pDispParams->rgvarg[4].pdispVal, GET_SAFE_DISP_BSTR(pDispParams->rgvarg[3]), 
						GET_SAFE_DISP_BSTR(pDispParams->rgvarg[2]), GET_SAFE_DISP_I4(pDispParams->rgvarg[1]), pDispParams->rgvarg[0].pboolVal);
				}
				break;
			case DISPID_NAVIGATECOMPLETE2:
				pWeb->OnNavigateComplete(pDispParams->rgvarg[1].pdispVal, GET_SAFE_DISP_BSTR(pDispParams->rgvarg[0]));
				break;
			case DISPID_DOCUMENTCOMPLETE:
				pWeb->OnDocumentComplete(pDispParams->rgvarg[1].pdispVal, GET_SAFE_DISP_BSTR(pDispParams->rgvarg[0]));
				break;
			case DISPID_FILEDOWNLOAD:
				pWeb->OnFileDownload(pDispParams->rgvarg[1].pboolVal, pDispParams->rgvarg[0].pboolVal);
				break;
			case DISPID_NEWWINDOW2:
				pWeb->OnNewWindow2(pDispParams->rgvarg[1].ppdispVal, pDispParams->rgvarg[0].pboolVal);
				break;
			case DISPID_PROGRESSCHANGE:
				pWeb->OnProgressChange(pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0].lVal);
				break;
			case DISPID_STATUSTEXTCHANGE:
				pWeb->OnStatusTextChange(GET_SAFE_DISP_BSTR(pDispParams->rgvarg[0]));
				break;
			case DISPID_COMMANDSTATECHANGE:
				pWeb->OnCommandStateChange(pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0].boolVal == VARIANT_TRUE?true:false);
				break;
			case DISPID_TITLECHANGE:
				pWeb->OnTitleChange(GET_SAFE_DISP_BSTR(pDispParams->rgvarg[0]));
				break;
			case DISPID_WINDOWCLOSING:
				pWeb->OnWindowClose();
				//非js创建的窗口会提示
				if (pDispParams->rgvarg[1].boolVal == VARIANT_FALSE)
				{
					//设置为不关闭
					*(pDispParams->rgvarg[0].pboolVal) = VARIANT_TRUE;
					//执行关闭
					VARIANT vNull;
					VariantInit(&vNull);
					pWeb->ExecWB(OLECMDID_CLOSE, OLECMDEXECOPT_DONTPROMPTUSER, &vNull, &vNull);
				}
				break;
			default:
				return pWeb->DWebBrowserEvents2Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
			}
			return S_OK;
		}
		return DISP_E_MEMBERNOTFOUND;
	}
};

class CWebBrowserImpl : public util_activex::AxHostDelegate
{
public:
	CWebBrowserImpl(CWebBrowserUI* pOwner);
	~CWebBrowserImpl()
	{
		if (m_pOwner)
			m_pOwner->m_pBrowserImpl = NULL;
		if (ax_host_.get())
			ax_host_->m_pOwner = NULL;
	}
	void CreateControl()
	{
		ax_host_->CreateControl(__uuidof(WebBrowser));
	}
	CWebBrowserUI* GetUI()
	{
		return m_pOwner;
	}
	IWebBrowser2* GetWebBrowser2()
	{
		return webbrowser_.get();
	}
	void SetOwner(CWebBrowserUI* pOwner)
	{
		m_pOwner = pOwner;
	}
	IDispatch* GetExtendedControl()
	{
		IDispatch* pDispatch = NULL;
		ax_host_->GetExtendedControl(&pDispatch);
		return pDispatch;
	}
	void SetRect( RECT& rectRegion )
	{
		ax_host_->SetRect(rectRegion);
	}
	void SetVisible(bool bVisible)
	{
		if (m_pOwner)
		{
			HWND hWnd = m_pOwner->GetHostWindow();
			if (!IsWindowVisible(hWnd) && bVisible)
			{
				m_bLastVisible = bVisible;
				return;
			}
		}
		
		m_bLastVisible = bVisible;
		if(!ax_host_.get())
		{
			return ;
		}
		if(!webbrowser_.get())
		{
			return ;
		}

		util_activex::ScopedComPtr<IOleWindow> ole_window;
		ole_window.QueryFrom(ax_host_->activex_control());
		if(!ole_window)
		{
			return ;
		}

		HWND window = NULL;
		ole_window->GetWindow(&window);
		if(!window)
		{
			return ;
		}

		ShowWindow(window, bVisible ? SW_SHOW : SW_HIDE);
	}

	// Overridden from AxHostDelegate:
	virtual HWND GetAxHostWindow() const
	{
		if (m_pOwner)
		{
			return m_pOwner->GetHostWindow();
		}
		return HWND_DESKTOP;
	}
	virtual void OnAxCreate(util_activex::AxHost* host)
	{
		if (ax_host_.get() != host)
			return;

		webbrowser_.QueryFrom(ax_host_->activex_control());
	}
	virtual void OnAxInvalidate(const RECT& rect){}
public:
	util_activex::scoped_ptr<Webbrowser_Ctrl> ax_host_;
	util_activex::ScopedComPtr<IWebBrowser2> webbrowser_;
	CWebBrowserUI* m_pOwner;
	/// 窗口未显示前设置了网页显示
	bool m_bLastVisible;
};




HRESULT Webbrowser_Ctrl:: ShowContextMenu(
	DWORD dwID,
	POINT *ppt,
	IUnknown *pcmdtReserved,
	IDispatch *pdispReserved
	)
{
	HRESULT result	= S_FALSE; //Dont Interfere
	bool bHandled	= false;

	CWebBrowserUI* pWeb = NULL;
	if (m_pOwner)
		pWeb = m_pOwner->GetUI();
	if (pWeb && pWeb->GetMenuCall())
	{
		util_activex::ScopedComPtr<IHTMLElement> pDestElement;
		pDestElement.QueryFrom(pdispReserved);
		if (pDestElement.get())
		{
			BSTR bstrID;
			BSTR bstrTag;
			pDestElement->get_id(&bstrID);
			pDestElement->get_tagName(&bstrTag);
			bHandled = pWeb->GetMenuCall()->OnShowContentMenu(dwID, ppt, bstrID, bstrTag, pdispReserved, pWeb);
		}
	}
	if (bHandled)
		result	= S_OK;
	else if (pWeb)
	{
		switch ( pWeb->GetContextMenuMode() )
		{
		case CWebBrowserUI::WebbrowserMenuMode_DefaultMenuSupport:
			break;

		case CWebBrowserUI::WebbrowserMenuMode_NoContextMenu:
			result	= S_OK;
			bHandled= true;
			break;

		case CWebBrowserUI::WebbrowserMenuMode_TextSelectionOnly:
			if (dwID != CONTEXT_MENU_TEXTSELECT)
			{
				result	= S_OK;
				bHandled= true;
			}
			break;

		case CWebBrowserUI::WebbrowserMenuMode_AllowAllButViewSource:
			if (dwID == CONTEXT_MENU_DEFAULT)
			{
				//result	= ModifyContextMenu(dwID, ppt, pcmdtReserved);
				bHandled= true;
			}
			break;

		case CWebBrowserUI::WebbrowserMenuMode_CustomMenuSupport:
			if (dwID == CONTEXT_MENU_DEFAULT)
			{
				//result = CustomContextMenu(ppt, pcmdtReserved);
				bHandled= true;
			}
			break;
		}
	}

	if (!bHandled)
	{
		return E_NOTIMPL;
	}

	return result;

}

HRESULT Webbrowser_Ctrl::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	if (m_pOwner && m_pOwner->GetUI())
		pInfo->dwFlags = m_pOwner->GetUI()->GetDocHostFlags();
	return S_OK;
}

STDMETHODIMP Webbrowser_Ctrl::GetExternal( IDispatch **ppDispatch)
{
	if (m_pOwner)
	{
		CWebBrowserUI* pWebBrowser = m_pOwner->GetUI();
		if (pWebBrowser && pWebBrowser->IsEnableExternal())
		{
			IDispatch* pDispatch = pWebBrowser->GetExternalCall();
			if (pDispatch)
			{
				*ppDispatch = pDispatch;
				(*ppDispatch)->AddRef();
				return S_OK;
			}
		}
	}
	return E_NOTIMPL;
}

STDMETHODIMP Webbrowser_Ctrl::TranslateUrl(
	DWORD dwTranslate,
	OLECHAR *pchURLIn,
	OLECHAR **ppchURLOut
	)
{
	if (m_pOwner && m_pOwner->GetUI())
	{
		m_pOwner->GetUI()->m_strTranslateUrl = pchURLIn;
	}
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////


CWebBrowserImpl::CWebBrowserImpl( CWebBrowserUI* pOwner ) 
:ax_host_( new Webbrowser_Ctrl(this))// 生成web控件
,m_pOwner(pOwner)
{
	m_bLastVisible = true;
}
//////////////////////////////////////////////////////////////////////////
Webbrowser_Ctrl::Webbrowser_Ctrl( CWebBrowserImpl* pOwnerImpl ) :util_activex::AxHost(dynamic_cast<util_activex::AxHostDelegate*>(pOwnerImpl))
,m_pOwner(pOwnerImpl)
{

}

//IOleCommandTarget
STDMETHODIMP Webbrowser_Ctrl::Exec( 
				const GUID *pguidCmdGroup,
				DWORD nCmdID,
				DWORD nCmdexecopt,
				VARIANT *pvaIn,
				VARIANT *pvaOut)
{
	HRESULT hr = S_OK;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{

		switch (nCmdID) 
		{

		case OLECMDID_SHOWSCRIPTERROR:
			{
				if (m_pOwner && m_pOwner->GetUI() && !m_pOwner->GetUI()->IsEnableScriptErrorsSuppressed())
				{
					hr = OLECMDERR_E_NOTSUPPORTED;
					break;
				}
				IHTMLDocument2*             pDoc = NULL;
				IHTMLWindow2*               pWindow = NULL;
				IHTMLEventObj*              pEventObj = NULL;
				BSTR                        rgwszNames[5] = 
				{ 
					SysAllocString(L"errorLine"),
					SysAllocString(L"errorCharacter"),
					SysAllocString(L"errorCode"),
					SysAllocString(L"errorMessage"),
					SysAllocString(L"errorUrl")
				};
				DISPID                      rgDispIDs[5];
				VARIANT                     rgvaEventInfo[5];
				DISPPARAMS                  params;
				bool                        fContinueRunningScripts = false;
				int	                        i;

				params.cArgs = 0;
				params.cNamedArgs = 0;

				// Get the document that is currently being viewed.
				pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc);				
				// Get document.parentWindow.
				pDoc->get_parentWindow(&pWindow);
				pDoc->Release();
				// Get the window.event object.
				pWindow->get_event(&pEventObj);
				// Get the error info from the window.event object.
				for (i = 0; i < 5; i++) 
				{  
					// Get the property's dispID.
					hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1, 
						LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
					if (SUCCEEDED(hr))
					{
						// Get the value of the property.
						hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
							LOCALE_SYSTEM_DEFAULT,
							DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
							NULL, NULL);
						SysFreeString(rgwszNames[i]);
					}
				}

				// At this point, you would normally alert the user with 
				// the information about the error, which is now contained
				// in rgvaEventInfo[]. Or, you could just exit silently.

				(*pvaOut).vt = VT_BOOL;
				if (fContinueRunningScripts)
				{
					// Continue running scripts on the page.
					(*pvaOut).boolVal = VARIANT_TRUE;
				}
				else
				{
					// Stop running scripts on the page.
					(*pvaOut).boolVal = VARIANT_FALSE;			
				} 
				break;
			}
		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}
	else
	{
		hr = OLECMDERR_E_UNKNOWNGROUP;
	}
	return (hr);
}

wstring CWebBrowserUI::m_strDefaultCookiesPath = _T("");
wstring CWebBrowserUI::ms_strDefaultCachePath = _T("");

CWebBrowserUI::CWebBrowserUI()
:m_bCreated(false),
m_bEnableScroll(true),
m_bEnableSelect(true),
m_bEnableNewWindow(true),
m_bEnableDownload(false),
m_bEnableExternal(false),
m_bEnableRedirect(false),
m_bChangedDirect(false),
m_strCookiesPath(_T("")),
m_strCachePath(_T("")),
m_bEnableNativeWeb(false),
m_bEnableDrop(false),
m_bEnableLoading(false),
m_pLoadingWindow(NULL),
m_contextMenuMode(WebbrowserMenuMode_DefaultMenuSupport),
m_pDocumentCompleteDispatch(NULL),
m_strTranslateUrl(_T("")),
m_bEnableBack(false),
m_bEnableNext(false),
m_dwCookie_DWebBrowserEvents2(0),
m_pBrowserImpl(NULL),
m_pWindowExternal(NULL),
m_pWebMenu(NULL),
m_dwDocHostUIFlag(DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_NO3DBORDER),
m_hWebBrowser(NULL),
m_bEnableScriptErrorsSuppressed(true),
m_pDWebBrowserEvents2(NULL)
{
	InitControl();
}

CWebBrowserUI::~CWebBrowserUI()
{
}

void CWebBrowserUI::EnableContextMenu(bool bEnable)
{
	if (!bEnable)
	{
		SetContextMenuMode(WebbrowserMenuMode_NoContextMenu);
	}
	else
	{
		SetContextMenuMode(WebbrowserMenuMode_DefaultMenuSupport);

	}

}

bool CWebBrowserUI::IsEnableContextMenu()
{
	return GetContextMenuMode() != WebbrowserMenuMode_NoContextMenu;
}
double CWebBrowserUI::GetZoomLevel()
{
	VARIANT vtZoom;
	vtZoom.vt = VT_EMPTY;
	ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, NULL, &vtZoom);
	if (vtZoom.vt == VT_I4)
	{
		LONG lZoom = V_I4(&vtZoom);
		return lZoom / (double)100;
	}
	return 1.0;
}

void CWebBrowserUI::SetZoomLevel(double zoomLevel)
{
	VARIANT vtZoom;
	vtZoom.vt = VT_I4;
	vtZoom.lVal = zoomLevel * 100;
	this->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, &vtZoom, NULL);
}

void CWebBrowserUI::Show( LPCTSTR lpszUrl, LPCTSTR lpszPostData /*= NULL*/, UINT flags /*= 0*/, LPCTSTR lpszHeaders /*= NULL*/ )
{
	DoCreateControl();
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		if (m_pBrowserImpl)
			m_pBrowserImpl->SetVisible(IsVisible());
		OffsetBrowser();

		VARIANT vURL;
		VariantInit(&vURL);
		V_VT(&vURL) = VT_BSTR;
		V_BSTR(&vURL) = ::SysAllocString(lpszUrl);
		if (lpszPostData)
		{
			VARIANT vFlags;
			VARIANT vTargetFrameName;
			VARIANT vPostData;
			VARIANT vHeaders;
			VariantInit(&vFlags);
			VariantInit(&vTargetFrameName);
			VariantInit(&vPostData);
			VariantInit(&vHeaders);

			V_VT(&vFlags) = VT_I4;
			V_I4(&vFlags) = flags;

			V_VT(&vTargetFrameName) = VT_BSTR;
			V_BSTR(&vTargetFrameName) = ::SysAllocString(_T("_self"));

			{
				std::string strPostData = Charset::NativeToUTF8(lpszPostData);

				SAFEARRAYBOUND bound;
				bound.cElements = (ULONG)strPostData.length();
				bound.lLbound = 0;
				SAFEARRAY FAR* sfPost = SafeArrayCreate(VT_UI1, 1, &bound);

				const char* pChar = strPostData.c_str();
				for (long lIndex = 0; lIndex < (signed)bound.cElements; lIndex++) {
					SafeArrayPutElement(sfPost, &lIndex, (void*)((pChar++)));
				}
				pChar = NULL;

				V_VT(&vPostData) = VT_ARRAY | VT_UI1;
				V_ARRAY(&vPostData) = sfPost;
			}

			V_VT(&vHeaders) = VT_BSTR;
			if (!lpszHeaders)
				V_BSTR(&vHeaders) = ::SysAllocString(_T("Content-Type: application/x-www-form-urlencoded\r\n"));
			else
				V_BSTR(&vHeaders) = ::SysAllocString(lpszHeaders);

			pWB2->Navigate2(&vURL, &vFlags, &vTargetFrameName, &vPostData, &vHeaders);
		}
		else
		{
			VARIANT vNull;
			VariantInit(&vNull);
			pWB2->Navigate2(&vURL, &vNull, &vNull, &vNull, &vNull);
		}

	}
}

void CWebBrowserUI::SetRect( RECT& rectRegion )
{
	__super::SetRect(rectRegion);
	OffsetBrowser();
}

void CWebBrowserUI::InitControl()
{
	// 初始化Activex控件，web控件还未创建
	if (!m_pBrowserImpl)
	{
		m_pBrowserImpl = new CWebBrowserImpl(this);
	}

	// 执行javascript
	if (!GetExternalCall())
	{
		WebbrowserExternal* pExternal = new WebbrowserExternal;
		pExternal->SetOwner(this);
		SetExternalCall(pExternal);
	}

	// 加载网页事件响应
	if (!m_pDWebBrowserEvents2)
	{
		m_pDWebBrowserEvents2 = new DWebBrowserEvents2Impl(this);
	}
}

// 第一次显示页面时调用
bool CWebBrowserUI::DoCreateControl()
{
	if (m_bCreated)
	{
		if (m_pBrowserImpl)
			return (m_pBrowserImpl->GetWebBrowser2() != NULL);
	}
	InitControl();
	if (m_pBrowserImpl)
	{
		m_bCreated = true;
		m_pBrowserImpl->CreateControl();
		RegisterEventHandler(true);

		IWebBrowser2* pWeb = m_pBrowserImpl->GetWebBrowser2();

		HWND hWebBrowserTopWnd = NULL;
		m_pBrowserImpl->ax_host_->inplace_object_windowless_->GetWindow(&hWebBrowserTopWnd);
		if (hWebBrowserTopWnd)
		{
			WebbrowserTopWnd* pTop = new WebbrowserTopWnd(this);
			pTop->Attach(hWebBrowserTopWnd);
			m_hWebBrowser = hWebBrowserTopWnd;
		}

		// 设置脚本错误提示和拖拉属性
		if (pWeb)
		{
			if (IsEnableScriptErrorsSuppressed())
				pWeb->put_Silent(VARIANT_TRUE);

			pWeb->put_RegisterAsDropTarget(IsEnableDrop()?VARIANT_TRUE:VARIANT_FALSE);
		}
		m_pBrowserImpl->SetVisible(false);
		OffsetBrowser();
		SendNotify(UINOTIFY_ACTIVEX_CREATE, NULL, NULL);
		return (pWeb != NULL);
	}
	return false;
}

void CWebBrowserUI::ReleaseControl()
{
	m_bCreated = false;
	m_dwCookie_DWebBrowserEvents2 = 0;
	RegisterEventHandler(false);

	if (m_hWebBrowser)
	{
		CWindowBase* pWnd = CWindowBase::FromHandle(m_hWebBrowser);
		if (pWnd)
		{
			pWnd->Detach();
			delete pWnd;
		}
	}
	m_hWebBrowser = NULL;

	if (m_pBrowserImpl)
	{
		m_pBrowserImpl->SetOwner(NULL);
		delete m_pBrowserImpl;
	}
	m_pBrowserImpl = NULL;

	if (m_pWindowExternal)
	{
		WebbrowserExternal* pExternal = dynamic_cast<WebbrowserExternal*>(m_pWindowExternal);
		if (pExternal)
		{
			pExternal->SetOwner(NULL);
			delete pExternal;
		}
		else
		{
			delete m_pWindowExternal;
		}
	}
	m_pWindowExternal = NULL;

	if (m_pDWebBrowserEvents2)
	{
		DWebBrowserEvents2Impl* pDWebBrowserEvents2Impl = dynamic_cast<DWebBrowserEvents2Impl*>(m_pDWebBrowserEvents2);
		if (pDWebBrowserEvents2Impl)
		{
			m_pDWebBrowserEvents2 = NULL;
			delete pDWebBrowserEvents2Impl;
		}
		else
		{
			DWebBrowserEvents2* pDWebBrowserEvents2 = m_pDWebBrowserEvents2;
			delete pDWebBrowserEvents2;
		}
	}
	m_pDWebBrowserEvents2 = NULL;
}

HRESULT CWebBrowserUI::RegisterEventHandler( bool inAdvise )
{
	//利用DIID_DWebBrowserEvents2接口接收WebBrowser事件 http://blog.csdn.net/breeze_vickie/article/details/4270297
	DWebBrowserEvents2* pWebBrowserEvents2 = m_pDWebBrowserEvents2;
	if (pWebBrowserEvents2 == NULL)
		return S_FALSE;
	IWebBrowser2* pWebBrowser = GetWebBrowser2();
	if (pWebBrowser == NULL)
		return S_FALSE;
	util_activex::ScopedComPtr<IConnectionPointContainer> conn_Cont;
	conn_Cont.QueryFrom(pWebBrowser);
	if (conn_Cont.get())
	{
		util_activex::ScopedComPtr<IConnectionPoint> conn_Point;
		conn_Cont->FindConnectionPoint(DIID_DWebBrowserEvents2, conn_Point.Receive());
		if (conn_Point.get())
		{
			if (inAdvise)
			{
				conn_Point->Advise(m_pDWebBrowserEvents2, &m_dwCookie_DWebBrowserEvents2);
			}
			else
			{
				conn_Point->Unadvise(m_dwCookie_DWebBrowserEvents2);
				m_dwCookie_DWebBrowserEvents2 = 0;
			}
		}
	}
	return S_OK; 
}

void CWebBrowserUI::SetVisible( bool bVisible)
{
	__super::SetVisible(bVisible);
	if (!m_pBrowserImpl)
		return;
	m_pBrowserImpl->SetVisible(IsVisible());
}

void CWebBrowserUI::SetInternVisible( bool bVisible )
{
	__super::SetInternVisible(bVisible);
	if (!m_pBrowserImpl)
		return;
	m_pBrowserImpl->SetVisible(IsVisible() && bVisible);
}

void CWebBrowserUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("enableExternal"))) 
	{
		SetEnableExternal((bool)!!_ttoi(lpszValue));
	}
	else if(equal_icmp(lpszName, _T("enableScroll"))) 
	{
		SetEnableScroll((bool)!!_ttoi(lpszValue));
		SetDocHostFlags(!IsEnableScroll() ? (GetDocHostFlags() | DOCHOSTUIFLAG_SCROLL_NO) : (GetDocHostFlags() & ~DOCHOSTUIFLAG_SCROLL_NO));
	}
	else if(equal_icmp(lpszName, _T("enableTextSelect"))) 
	{
		SetEnableSelect((bool)!!_ttoi(lpszValue));
		SetDocHostFlags(!IsEnableSelect() ? (GetDocHostFlags() | DOCHOSTUIFLAG_DIALOG) : (GetDocHostFlags() & ~DOCHOSTUIFLAG_DIALOG));
	}
	else if(equal_icmp(lpszName, _T("menumode"))) 
	{
		SetContextMenuMode((WebbrowserContextMenuMode)_ttoi(lpszValue));
	}
	else if(equal_icmp(lpszName, _T("enableDownload"))) 
	{
		SetEnableDownload((bool)!!_ttoi(lpszValue));
	}
	else if(equal_icmp(lpszName, _T("enableDrop"))) 
	{
		SetEnableDrop((bool)!!_ttoi(lpszValue));
	}
	else if(equal_icmp(lpszName, _T("enableNewWindow"))) 
	{
		SetEnableNewWindow((bool)!!_ttoi(lpszValue));
	}
	else if(equal_icmp(lpszName, _T("enableNativeWeb")))
	{
		SetEnableNativeWeb((bool)!!_ttoi(lpszValue));
	}
	else if(equal_icmp(lpszName, _T("enableLoading"))) 
	{
		SetEnableLoading((bool)!!_ttoi(lpszValue));
	}
	else if(equal_icmp(lpszName, _T("enableRedirect"))) 
	{
		SetEnableRedirect((bool)!!_ttoi(lpszValue));
	}
	else if(equal_icmp(lpszName, _T("enableScriptErrors"))) 
	{
		SetEnableScriptErrorsSuppressed((bool)!!_ttoi(lpszValue));
	}
	else 
		__super::SetAttribute(lpszName, lpszValue);
}

void CWebBrowserUI::OnBeforeNavigate(IDispatch *pDispatch, LPCWSTR pszURL, DWORD dwFlags, LPCWSTR pszTargetFrameName, VARIANT *vtPostData, LPCWSTR pszHeaders, VARIANT_BOOL *Cancel)
{
	SendNotify(UINOTIFY_ACTIVEX_NAVIGATE_BEFORE, (WPARAM)Cancel, (LPARAM)pszURL);
	if (*Cancel == VARIANT_FALSE)
	{
		/*if (GetUIEngine()->IsEnableHDPI())
		{
		double oldZoomLevel =  GetZoomLevel();
		double zoomLevel = GetUIEngine()->GetDPI() / (double)96;
		zoomLevel = zoomLevel * zoomLevel;
		if (abs(oldZoomLevel - zoomLevel) > 0.0001)
		SetZoomLevel(zoomLevel);
		}*/
		
		if (IsEnableLoading())
		{
			OnDocumentLode(false);
		}

		if (!m_bChangedDirect && IsEnableRedirect())
		{
			GetRegDefaultValue();
			if(!m_strCookiesPath.empty() && !m_strCachePath.empty())
			{
				SetRegValue(m_strCookiesPath.c_str(), m_strCachePath.c_str());
				//改变通知
				InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
				//广播
				SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
				Sleep(50);
			}
		}
	}
}

void CWebBrowserUI::OnNavigateError(IDispatch *pDispatch, LPCWSTR pszURL, LPCWSTR pszTargetFrameName, INT nStatusCode, VARIANT_BOOL *Cancel)
{
	SendNotify(UINOTIFY_ACTIVEX_NAVIGATE_ERROR, nStatusCode, NULL);
}

void CWebBrowserUI::OnNavigateComplete(IDispatch *pDispatch, LPCWSTR pszURL)
{
	if (!m_bChangedDirect && IsEnableRedirect())
	{
		SetRegValue(CWebBrowserUI::m_strDefaultCookiesPath.c_str(), CWebBrowserUI::ms_strDefaultCachePath.c_str());
		m_bChangedDirect = true;
	}

	if (m_pDocumentCompleteDispatch == NULL)
		m_pDocumentCompleteDispatch = pDispatch;
	SendNotify(UINOTIFY_ACTIVEX_NAVIGATE_COMPLETE, NULL, (LPARAM)pszURL);

}

void CWebBrowserUI::OnDocumentComplete(IDispatch *pDisp, LPCWSTR pszURL)
{
	if (IsEnableLoading())
	{
		OnDocumentLode(true);
	}
	if (m_pDocumentCompleteDispatch && m_pDocumentCompleteDispatch == pDisp)
	{
		m_pDocumentCompleteDispatch = NULL;
		SendNotify(UINOTIFY_ACTIVEX_SHOWCOMPLETE, NULL, (LPARAM)pszURL);

	}
}

void CWebBrowserUI::OnFileDownload(VARIANT_BOOL *ActiveDocument, VARIANT_BOOL *Cancel)
{
	if (IsEnableDownload())
		*Cancel = VARIANT_FALSE;
	else
		*Cancel = VARIANT_TRUE;
}

IDispatch* DisplayNativeWeb(LPCTSTR lpszUrl, HWND hWndParent);
void CWebBrowserUI::OnNewWindow2(IDispatch **ppDisp, VARIANT_BOOL *Cancel)
{

	if (m_strTranslateUrl.empty())
	{
		*Cancel = VARIANT_TRUE;
		return;
	}

	if (IsEnableNewWindow())
	{
		if (IsEnableNativeWeb())
		{
 			*ppDisp = DisplayNativeWeb(m_strTranslateUrl.c_str(), GetHostWindow());
 			m_strTranslateUrl = _T("");
 			*Cancel = VARIANT_TRUE;
		}
		else
		{
			/*int ret = (int)ShellExecute(NULL, _T("open"), _T("iexplore.exe"),  m_strTranslateUrl.c_str(), NULL, SW_SHOWNORMAL);
			if  (ret<32)*/
			{
				ShellExecute(NULL, NULL, m_strTranslateUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);//使用默认浏览器打开固定网址
			}
			m_strTranslateUrl = _T("");
			*Cancel = VARIANT_TRUE;
			//*Cancel = VARIANT_FALSE;
		}
	}
	else
	{
		this->LoadUrl(m_strTranslateUrl.c_str());
		*Cancel = VARIANT_TRUE;
	}
}

void CWebBrowserUI::OnProgressChange(long Progress, long ProgressMax)
{

}

void CWebBrowserUI::OnStatusTextChange(LPCWSTR pszText)
{

}

void CWebBrowserUI::OnCommandStateChange(long nCommand, bool bEnable)
{
	if (nCommand == CSC_NAVIGATEBACK)
		m_bEnableBack = bEnable;
	if (nCommand == CSC_NAVIGATEFORWARD)
		m_bEnableNext = bEnable;
}

void CWebBrowserUI::OnDocumentLode(bool bDone)
{
	if (bDone)
	{
		if (m_pLoadingWindow)
		{
			m_pLoadingWindow->ShowWindow(SW_HIDE);
		}
		this->SendNotify(UINOTIFY_ACTIVEX_LODE_DONE, NULL, NULL);
	} 
	else
	{
		if (m_pLoadingWindow && !IsWindowVisible(m_pLoadingWindow->GetHWND()))
		{
			RECT rcParent;
			::GetWindowRect(m_pBrowserImpl->GetAxHostWindow(), &rcParent);
			RECT rcWnd;
			::GetWindowRect(m_pLoadingWindow->GetHWND(), &rcWnd);
			int nWnd_w = rcWnd.right - rcWnd.left;
			int nWnd_h = rcWnd.bottom - rcWnd.top;
			if (rcParent.left != rcParent.right && rcParent.top != rcParent.bottom)
			{
				int nStartp_x = rcParent.left + (rcParent.right - rcParent.left)/2 - nWnd_w/2;
				int nStartp_y = rcParent.top + (rcParent.bottom - rcParent.top)/2 - nWnd_h/2;

				m_pLoadingWindow->SetWindowPos(HWND_NOTOPMOST, nStartp_x, nStartp_y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
				m_pLoadingWindow->ShowAndActiveWindow();
			}
		}
		this->SendNotify(UINOTIFY_ACTIVEX_LODING, NULL, NULL);
	}
}

IWebBrowser2* CWebBrowserUI::GetWebBrowser2()
{
	if (m_pBrowserImpl)
	{
		return m_pBrowserImpl->GetWebBrowser2();
	}
	return NULL;
}

IHTMLDocument2* CWebBrowserUI::GetDocument2()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		util_activex::ScopedComPtr<IDispatch> dispatch_Document;
		pWB2->get_Document(dispatch_Document.Receive());
		if (dispatch_Document.get())
		{
			IHTMLDocument2 *pHtmlDoc2 = NULL;
			dispatch_Document.QueryInterface(IID_IHTMLDocument2,(void**)&pHtmlDoc2);
			return pHtmlDoc2;
		}
	}
	return NULL;
}

IHTMLDocument3* CWebBrowserUI::GetDocument3()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		util_activex::ScopedComPtr<IDispatch> DispatchDocument;
		pWB2->get_Document(DispatchDocument.Receive());
		if (DispatchDocument.get())
		{
			IHTMLDocument3 *pHtmlDoc3 = NULL;
			DispatchDocument.QueryInterface(IID_IHTMLDocument3,(void**)&pHtmlDoc3);
			return pHtmlDoc3;
		}
	}
	return NULL;
}

IHTMLDocument4* CWebBrowserUI::GetDocument4()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		util_activex::ScopedComPtr<IDispatch> DispatchDocument;
		pWB2->get_Document(DispatchDocument.Receive());
		if (DispatchDocument.get())
		{
			IHTMLDocument4 *pHtmlDoc4 = NULL;
			DispatchDocument.QueryInterface(IID_IHTMLDocument4,(void**)&pHtmlDoc4);
			return pHtmlDoc4;
		}
	}
	return NULL;
}

IDispatch* CWebBrowserUI::GetHtmlWindow()
{
	IDispatch *pHtmlWindown = NULL;
	util_activex::ScopedComPtr<IHTMLDocument2> html_doc2(GetDocument2());
	if (html_doc2.get())
	{
		util_activex::ScopedComPtr<IHTMLWindow2> html_wnd2;
		html_doc2->get_parentWindow(html_wnd2.Receive());
		if (html_wnd2.get())
		{
			html_wnd2.QueryInterface(IID_IDispatch, (void**)&pHtmlWindown);
		}
	}
	return pHtmlWindown;
}

HRESULT CWebBrowserUI::GetControl( const IID iid, LPVOID* ppRet )
{
	if (m_pBrowserImpl && m_pBrowserImpl->ax_host_.get())
		return m_pBrowserImpl->ax_host_->QueryInterface(iid, ppRet);
	return S_FALSE;
}

void CWebBrowserUI::GetRegDefaultValue()
{
	if (!CWebBrowserUI::m_strDefaultCookiesPath.empty() && !CWebBrowserUI::ms_strDefaultCachePath.empty())
		return;
	WCHAR str[MAX_PATH];
	HKEY hRegKey; 
	unsigned long dwType = REG_SZ; 
	unsigned long dwSize; 

	wsprintf(str,(L"SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\EXPLORER\\User Shell Folders"));
	RegOpenKey(HKEY_CURRENT_USER, str, &hRegKey);

	if (CWebBrowserUI::m_strDefaultCookiesPath.empty())
	{
		WCHAR szCookiesPath[MAX_PATH]={0}; 
		wsprintf(str,(L"Cookies"));
		RegQueryValueEx(hRegKey,str, NULL, &dwType, (unsigned char far*)szCookiesPath, &dwSize);
		CWebBrowserUI::m_strDefaultCookiesPath = szCookiesPath;
	}

	if (CWebBrowserUI::ms_strDefaultCachePath.empty())
	{
		WCHAR szCachePath[MAX_PATH]={0};
		wsprintf(str,(L"Cache"));
		RegQueryValueEx(hRegKey,str, NULL, &dwType, (unsigned char far*)szCachePath, &dwSize);
		CWebBrowserUI::ms_strDefaultCachePath = szCachePath;
	}
}

void CWebBrowserUI::SetRegValue( wstring strCookiesPath, wstring strCachePath )
{
	WCHAR str[MAX_PATH];
	HKEY hRegKey;  
	wsprintf(str,(L"SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\EXPLORER\\User Shell Folders"));
	RegOpenKey(HKEY_CURRENT_USER, str, &hRegKey);
	wsprintf(str,L"Cookies");
	RegSetValueEx( hRegKey,str,0,REG_SZ,(const unsigned char *)strCookiesPath.c_str(),MAX_PATH);
	wsprintf(str,L"Cache");
	RegSetValueEx( hRegKey,str,0,REG_SZ,(const unsigned char *)strCachePath.c_str(),MAX_PATH);
}

DISPID CWebBrowserUI::FindId(IDispatch *pObj, LPOLESTR pName)
{
	DISPID id = 0;
	if (pObj == NULL)
		return -1;
	if(FAILED(pObj->GetIDsOfNames(IID_NULL,&pName,1,LOCALE_SYSTEM_DEFAULT,&id))) id = -1;
	return id;
}

HRESULT CWebBrowserUI::InvokeMethod(IDispatch *pObj, LPOLESTR pName, VARIANT *pVarResult, VARIANT *p, int cArgs)
{
	if (pObj == NULL) return E_FAIL;
	DISPID dispid = FindId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = cArgs;
	ps.rgvarg = p;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &ps, pVarResult, NULL, NULL);
}

bool CWebBrowserUI::GetBusy()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		VARIANT_BOOL vtBool;
		pWB2->get_Busy(&vtBool);
		return !(vtBool == VARIANT_FALSE);
	}
	return false;
}

tstring CWebBrowserUI::GetLocationName()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		BSTR bstrText;
		pWB2->get_LocationName(&bstrText);
		return bstrText;
	}
	return _T("");
}

tstring CWebBrowserUI::GetLocationURL()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		BSTR bstrText;
		pWB2->get_LocationURL(&bstrText);
		return bstrText;
	}
	return _T("");
}

void CWebBrowserUI::LoadUrl(LPCTSTR lpszUrl)
{
	this->Show(lpszUrl);
}

void CWebBrowserUI::LoadUrl(LPCTSTR lpszUrl, LPCTSTR lpszPostData, UINT flags, LPCTSTR lpszHeaders)
{
	this->Show(lpszUrl, lpszPostData, flags, lpszHeaders);
}

void CWebBrowserUI::Forward()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		pWB2->GoForward();
	}
}

bool CWebBrowserUI::CanForward()
{
	return m_bEnableNext;
}

void CWebBrowserUI::GoBack()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		pWB2->GoBack();
	}
}

bool CWebBrowserUI::CanGoBack()
{
	return m_bEnableBack;
}

void CWebBrowserUI::Refresh()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		pWB2->Refresh();
	}
}

void CWebBrowserUI::Stop()
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		pWB2->Stop();
	}
}

void CWebBrowserUI::OnTitleChange( LPCWSTR pszText )
{

}

bool CWebBrowserUI::HaveSelection()
{
	bool bReturnValue = true;
	util_activex::ScopedComPtr<IHTMLDocument2> html_doc2(GetDocument2());
	if (html_doc2.get())
	{
		util_activex::ScopedComPtr<IHTMLSelectionObject> html_Selection;
		html_doc2->get_selection(html_Selection.Receive());
		if (html_Selection.get())
		{
			_bstr_t bstrText;
			html_Selection->get_type(bstrText.GetAddress());
			if (equal_icmp((TCHAR*)bstrText, _T("None")))
				bReturnValue = false;
		}
	}
	return bReturnValue;
}

bool CWebBrowserUI::GetSelectionHtml(tstring& strText)
{
	_bstr_t bstrText;
	util_activex::ScopedComPtr<IHTMLDocument2> html_doc2(GetDocument2());
	if (html_doc2.get())
	{
		util_activex::ScopedComPtr<IHTMLSelectionObject> html_Selection;
		html_doc2->get_selection(html_Selection.Receive());
		if (html_Selection.get())
		{
			util_activex::ScopedComPtr<IDispatch> dispatch_range;
			html_Selection->createRange(dispatch_range.Receive());
			if (dispatch_range.get())
			{
				util_activex::ScopedComPtr<IHTMLTxtRange> html_textrange;
				html_textrange.QueryFrom(dispatch_range.get());
				if (html_textrange.get())
				{
					html_textrange->get_htmlText(bstrText.GetAddress());
					if ( 0 != bstrText.length() && (NULL != (const wchar_t*)bstrText) )
					{
						strText = (const wchar_t*)bstrText;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool CWebBrowserUI::GetSelectionText(tstring& strText)
{
	_bstr_t bstrText;
	util_activex::ScopedComPtr<IHTMLDocument2> html_doc2(GetDocument2());
	if (html_doc2.get())
	{
		util_activex::ScopedComPtr<IHTMLSelectionObject> html_Selection;
		html_doc2->get_selection(html_Selection.Receive());
		if (html_Selection.get())
		{
			util_activex::ScopedComPtr<IDispatch> dispatch_range;
			html_Selection->createRange(dispatch_range.Receive());
			if (dispatch_range.get())
			{
				util_activex::ScopedComPtr<IHTMLTxtRange> html_textrange;
				html_textrange.QueryFrom(dispatch_range.get());
				if (html_textrange.get())
				{
					html_textrange->get_text(bstrText.GetAddress());
					if ( 0 != bstrText.length() && (NULL != (const wchar_t*)bstrText) )
					{
						strText = (const wchar_t*)bstrText;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool CWebBrowserUI::SetElementInnerHtml(LPOLESTR lpszID, LPOLESTR lpszHtml)
{
	util_activex::ScopedComPtr<IHTMLDocument3> html_doc3(GetDocument3());
	if (html_doc3.get())
	{
		util_activex::ScopedComPtr<IHTMLElement> html_element;
		html_doc3->getElementById(lpszID, html_element.Receive());
		if (html_element.get())
		{
			html_element->put_innerHTML(lpszHtml);
		}
	}
	return true;
}

bool CWebBrowserUI::GetElementInnerHtml(LPOLESTR lpszID, tstring& strText)
{
	_bstr_t bstrText;
	util_activex::ScopedComPtr<IHTMLDocument3> html_doc3(GetDocument3());
	if (html_doc3.get())
	{
		util_activex::ScopedComPtr<IHTMLElement> html_element;
		html_doc3->getElementById(lpszID, html_element.Receive());
		if (html_element.get())
		{
			html_element->get_innerHTML(bstrText.GetAddress());
			if ( 0 != bstrText.length() && (NULL != (const wchar_t*)bstrText) )
			{
				strText = (const wchar_t*)bstrText;
				return true;
			}
		}
	}

	return false;
}

bool CWebBrowserUI::GetElementInnerText(LPOLESTR lpszID, tstring& strText)
{
	_bstr_t bstrText;
	util_activex::ScopedComPtr<IHTMLDocument3> html_doc3(GetDocument3());
	if (html_doc3.get())
	{
		util_activex::ScopedComPtr<IHTMLElement> html_element;
		html_doc3->getElementById(lpszID, html_element.Receive());
		if (html_element.get())
		{
			html_element->get_innerText(bstrText.GetAddress());
			if ( 0 != bstrText.length() && (NULL != (const wchar_t*)bstrText) )
			{
				strText = (const wchar_t*)bstrText;
				return true;
			}
		}
	}

	return false;
}

IDispatch* CWebBrowserUI::GetElement(LPOLESTR lpszID)
{
	IDispatch* pDispatch = NULL;
	util_activex::ScopedComPtr<IHTMLDocument3> html_doc3(GetDocument3());
	if (html_doc3.get())
	{
		util_activex::ScopedComPtr<IHTMLElement> html_element;
		html_doc3->getElementById(lpszID, html_element.Receive());
		if (html_element.get())
		{
			html_element.QueryInterface(IID_IDispatch, (void**)&pDispatch);
		}
	}
	return pDispatch;
}

bool CWebBrowserUI::ExecWB( /* [in] */ OLECMDID cmdID, /* [in] */ OLECMDEXECOPT cmdexecopt, /* [optional][in] */ __RPC__in VARIANT *pvaIn, /* [optional][in][out] */ __RPC__inout VARIANT *pvaOut )
{
	IWebBrowser2* pWB2 = GetWebBrowser2();
	if (pWB2)
	{
		return (pWB2->ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut) == S_OK);
	}
	return false;
}

void CWebBrowserUI::AddExtFunc( LPCTSTR lpszMethodName, Webbrowser_Client_Callback* extMethod )
{
	WebbrowserExternal* pExternal = dynamic_cast<WebbrowserExternal*>(GetExternalCall());
	if (pExternal)
	{
		pExternal->AddExtFunc(lpszMethodName, extMethod);
	}
}

Webbrowser_Client_Callback* CWebBrowserUI::GetExtFunc( LPCTSTR lpszMethodName )
{
	WebbrowserExternal* pExternal = dynamic_cast<WebbrowserExternal*>(GetExternalCall());
	if (pExternal)
	{
		return pExternal->GetExtFunc(lpszMethodName);
	}
	return NULL;
}

void CWebBrowserUI::AddExtDisp( LPCTSTR lpszDispName, IDispatch* extDisp )
{
	WebbrowserExternal* pExternal = dynamic_cast<WebbrowserExternal*>(GetExternalCall());
	if (pExternal)
	{
		pExternal->AddExtDisp(lpszDispName, extDisp);
	}
}

IDispatch* CWebBrowserUI::GetExtDisp( LPCTSTR lpszDispName )
{
	WebbrowserExternal* pExternal = dynamic_cast<WebbrowserExternal*>(GetExternalCall());
	if (pExternal)
	{
		return pExternal->GetExtDisp(lpszDispName);
	}
	return NULL;
}

HWND CWebBrowserUI::GetHostWindow()
{
	CWindowUI* pWindow = GetWindow();
	if (pWindow)
	{
		return pWindow->GetHWND();
	}
	return HWND_DESKTOP;
}

void CWebBrowserUI::OnWindowClose()
{
	SendNotify(UINOTIFY_ACTIVEX_CLOSE);
}

void CWebBrowserUI::Render( IRenderDC* pRenderDC, RECT& rcPaint )
{
	__super::Render(pRenderDC, rcPaint);
	HWND hWnd = GetHostWindow();
	if (hWnd && ::IsWindowVisible(hWnd))
	{
		CWindowUI* pWindow = GetWindow();
		if (!pWindow->IsWindowRender())
		{
			m_pBrowserImpl->SetVisible(m_pBrowserImpl->m_bLastVisible);
		}
	}
}

void CWebBrowserUI::SetDWebBrowserEvents2( DWebBrowserEvents2* pEvents2 )
{
	if (m_pDWebBrowserEvents2)
		delete m_pDWebBrowserEvents2;
	m_pDWebBrowserEvents2 = pEvents2;
}

STDMETHODIMP CWebBrowserUI::DWebBrowserEvents2Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, unsigned int* puArgErr )
{
	return DISP_E_MEMBERNOTFOUND;
}

void CWebBrowserUI::OffsetBrowser()
{
	if (m_pBrowserImpl)
	{
		m_pBrowserImpl->SetRect(GetRect());
	}
}

void CWebBrowserUI::OnDestroy()
{
	ReleaseControl();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ResizeBrowser(HWND hwnd, DWORD width, DWORD height);
void UnEmbedBrowserObject(HWND hwnd);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HICON getCurExeIcon();

// 本地浏览器最早出现，父浏览器先关闭后会崩溃，后来解决方案是父浏览器光标把子浏览器也光标
class CNativeWebBrowserUI : public CWebBrowserUI
{
public:
	CNativeWebBrowserUI(){m_hNativeWebHostWnd = NULL;}
	virtual HWND GetHostWindow(){return m_hNativeWebHostWnd;} // 通过GetHostWindow把NativeWebBrowser窗口和webbrowser控件结合起来
	virtual void OnWindowClose(){::PostMessage(m_hNativeWebHostWnd, WM_CLOSE, NULL, NULL);}
	virtual void LoadUrl(LPCTSTR lpszUrl)
	{
		__super::LoadUrl(lpszUrl);

		::ShowWindow(m_hNativeWebHostWnd, SW_SHOW);
		if (::IsIconic(m_hNativeWebHostWnd))
			::ShowWindow(m_hNativeWebHostWnd, SW_RESTORE);
		::SetForegroundWindow(m_hNativeWebHostWnd);
		::SetActiveWindow(m_hNativeWebHostWnd);
	}
	IDispatch* CreateHostWindow(HWND hWndParent)
	{
		IDispatch* pRet = NULL;
		WNDCLASSEX wc;

		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hInstance = GetEngineObj()->GetInstanceHandle();
		wc.lpfnWndProc = WindowProc;
		wc.lpszClassName = _T("NativeWebBrowser");
		RegisterClassEx(&wc);

		m_hNativeWebHostWnd = CreateWindowEx(0, _T("NativeWebBrowser"), _T(""), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
			hWndParent, NULL, GetEngineObj()->GetInstanceHandle(), (LPVOID)this);

		DoCreateControl();

		if (m_hNativeWebHostWnd)
		{
			pRet = m_pBrowserImpl->GetExtendedControl();

			HICON hIcon = getCurExeIcon();
			if (hIcon)
			{
				::SendMessage(m_hNativeWebHostWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);
				::SendMessage(m_hNativeWebHostWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIcon);
			}
		}
		return pRet;
	}
	STDMETHOD(DWebBrowserEvents2Invoke)( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, unsigned int* puArgErr )
	{
		switch (dispIdMember)
		{
		case DISPID_WINDOWSETLEFT:
			{
				RECT rcWindow;
				GetWindowRect(m_hNativeWebHostWnd, &rcWindow);
				::SetWindowPos(m_hNativeWebHostWnd, HWND_DESKTOP, pDispParams->rgvarg[0].intVal, rcWindow.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			break;
		case DISPID_WINDOWSETTOP:
			{
				RECT rcWindow;
				GetWindowRect(m_hNativeWebHostWnd, &rcWindow);
				::SetWindowPos(m_hNativeWebHostWnd, HWND_DESKTOP, rcWindow.left, pDispParams->rgvarg[0].intVal, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			break;
		case DISPID_WINDOWSETWIDTH:
			{
				RECT rcWindow;
				GetWindowRect(m_hNativeWebHostWnd, &rcWindow);
				::SetWindowPos(m_hNativeWebHostWnd, HWND_DESKTOP, 0, 0, pDispParams->rgvarg[0].intVal, rcWindow.bottom - rcWindow.top, SWP_NOZORDER);
			}
			break;
		case DISPID_WINDOWSETHEIGHT:
			{
				RECT rcWindow;
				GetWindowRect(m_hNativeWebHostWnd, &rcWindow);
				::SetWindowPos(m_hNativeWebHostWnd, HWND_DESKTOP, 0, 0, rcWindow.right - rcWindow.left, pDispParams->rgvarg[0].intVal, SWP_NOZORDER);
			}
			break;
		default:
			return DISP_E_MEMBERNOTFOUND;
		}
		return S_OK;
	}
protected:
private:
	HWND m_hNativeWebHostWnd;
};


void ResizeBrowser(HWND hwnd, DWORD width, DWORD height)
{
	CNativeWebBrowserUI* pWeb = (CNativeWebBrowserUI*)GetWindowLong(hwnd, GWL_USERDATA);
	if (pWeb == NULL)
		return;

	pWeb->SetVisible(true);
	RECT rcWindowClient;
	::GetClientRect(hwnd, &rcWindowClient);
	pWeb->SetRect(rcWindowClient);
}

void UnEmbedBrowserObject(HWND hwnd)
{
	CNativeWebBrowserUI* pWeb = (CNativeWebBrowserUI*)GetWindowLong(hwnd, GWL_USERDATA);
	if (pWeb == NULL)
		return;
	SetWindowLong(hwnd, GWL_USERDATA, (LONG)NULL);
	pWeb->OnDestroy();
	delete pWeb;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		{
			ResizeBrowser(hwnd, LOWORD(lParam), HIWORD(lParam));
			return(0);
		}
	case WM_CREATE:
		{
			LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam;
			if (lpcs->lpCreateParams)
			{
				SetWindowLong(hwnd, GWL_USERDATA, (LONG)lpcs->lpCreateParams);
			}
			return(0);
		}
	case WM_DESTROY:
		{
			UnEmbedBrowserObject(hwnd);
			return(TRUE);
		}
	}
	return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}

HICON getCurExeIcon()
{
	SHFILEINFO info;
	TCHAR rtfPath[MAX_PATH];
	GetModuleFileName(NULL, rtfPath, MAX_PATH);
	SHGetFileInfo(rtfPath, 0, &info, sizeof(info), SHGFI_ICON);
	return info.hIcon;
}

IDispatch* DisplayNativeWeb( LPCTSTR lpszUrl, HWND hWndParent )
{
	CNativeWebBrowserUI* pWeb = new CNativeWebBrowserUI;
	IDispatch* pRet = pWeb->CreateHostWindow(hWndParent);
	pWeb->LoadUrl(lpszUrl);
	return pRet;
}