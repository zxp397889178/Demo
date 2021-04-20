#include "stdafx.h"
#include "UIFlash.h"
#include "activex/activex_inner.h"

class CFlashImpl : public util_activex::AxHostDelegate
{
public:
	CFlashImpl() : ax_host_(new util_activex::AxHost(this)){m_pOwner = NULL;}

	void CreateControl()
	{
		ax_host_->CreateControl(__uuidof(ShockwaveFlash));
	}
	void SetOwner(CFlashUI* pOwner)
	{
		m_pOwner = pOwner;
	}
	void SetRect( RECT& rectRegion )
	{
		ax_host_->SetRect(rectRegion);
	}

	// Overridden from AxHostDelegate:
	virtual HWND GetAxHostWindow() const
	{
		if (m_pOwner)
			return m_pOwner->GetHostWindow();
		return HWND_DESKTOP;
	}
	virtual void OnAxCreate(util_activex::AxHost* host)
	{
		if (ax_host_.get() != host)
			return;

		/* HRESULT hr =  */flash_.QueryFrom(ax_host_->activex_control());
		if (flash_.get())
		{
			flash_->put_WMode(::SysAllocString(_T("Transparent")));
		}
	}
	virtual void OnAxInvalidate(const RECT& rect)
	{
		if (m_pOwner)
		{
			CWindowUI* pWindow = m_pOwner->GetWindow();
			if (pWindow)
			{
				RECT invalidate = rect;
				pWindow->Invalidate(&invalidate);
			}
		}
	}
public:
	util_activex::scoped_ptr<util_activex::AxHost> ax_host_;
	util_activex::ScopedComPtr<IShockwaveFlash> flash_;
	CFlashUI* m_pOwner;
};

#define DISPID_FLASHEVENT_FLASHCALL	 ( 0x00C5 )
#define DISPID_FLASHEVENT_FSCOMMAND	 ( 0x0096 )
#define DISPID_FLASHEVENT_ONPROGRESS ( 0x07A6 )


class CFlashDispatch : public _IShockwaveFlashEvents
{
public:
	CFlashDispatch(CFlashUI* pOwner):m_pOwner(pOwner),_refNum(0){}
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo );
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo );
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
	virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject );
	virtual ULONG STDMETHODCALLTYPE AddRef( void );
	virtual ULONG STDMETHODCALLTYPE Release( void );
	CFlashUI* m_pOwner;
protected:
private:
	long _refNum;
};


HRESULT STDMETHODCALLTYPE CFlashDispatch::GetTypeInfoCount( __RPC__out UINT *pctinfo )
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashDispatch::GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo )
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashDispatch::GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId )
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashDispatch::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
{
	switch(dispIdMember)
	{
	case DISPID_FLASHEVENT_FLASHCALL:
		{
			if (pDispParams->cArgs != 1 || pDispParams->rgvarg[0].vt != VT_BSTR) 
				return E_INVALIDARG;
			return m_pOwner->FlashCall(_bstr_t(pDispParams->rgvarg[0].bstrVal));
		}
	case DISPID_FLASHEVENT_FSCOMMAND:
		{
			if( pDispParams && pDispParams->cArgs == 2 )
			{
				if( pDispParams->rgvarg[0].vt == VT_BSTR &&
					pDispParams->rgvarg[1].vt == VT_BSTR )
				{
					return m_pOwner->FSCommand(_bstr_t(pDispParams->rgvarg[1].bstrVal), _bstr_t(pDispParams->rgvarg[0].bstrVal));
				}
				else
				{
					return DISP_E_TYPEMISMATCH;
				}
			}
			else
			{
				return DISP_E_BADPARAMCOUNT;
			}
		}
	case DISPID_FLASHEVENT_ONPROGRESS:
		{
			return m_pOwner->OnProgress(*pDispParams->rgvarg[0].plVal);
		}
	case DISPID_READYSTATECHANGE:
		{
			return m_pOwner->OnReadyStateChange(pDispParams->rgvarg[0].lVal);
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFlashDispatch::QueryInterface( REFIID riid, void **ppvObject )
{
	*ppvObject = NULL;

	if( riid == IID_IUnknown)
		*ppvObject = static_cast<LPUNKNOWN>(this);
	else if( riid == IID_IDispatch)
		*ppvObject = static_cast<IDispatch*>(this);
	else if( riid ==  __uuidof(_IShockwaveFlashEvents))
		*ppvObject = static_cast<_IShockwaveFlashEvents*>(this);

	if( *ppvObject != NULL )
		AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

ULONG STDMETHODCALLTYPE CFlashDispatch::AddRef( void )
{
	return ::InterlockedIncrement(&_refNum); 
}

ULONG STDMETHODCALLTYPE CFlashDispatch::Release( void )
{
	return ::InterlockedDecrement(&_refNum);
}
//////////////////////////////////////////////////////////////////////////

CFlashUI::CFlashUI()
:m_pFlashImpl(NULL)
,m_bCreated(false)
,m_dwCookie(0)
,m_pFlashDispatch(NULL)
{
	ModifyFlags(UICONTROLFLAG_SETCURSOR | UICONTROLFLAG_SETFOCUS, 0);
	SetEnabled(true);
	SetMouseEnabled(true);
	InitControl();
	m_bFlashEvent = false;
}

CFlashUI::~CFlashUI()
{
	ReleaseControl();
}


void CFlashUI::InitControl()
{
	if (!m_pFlashImpl)
	{
		m_pFlashImpl = new CFlashImpl;
		m_pFlashImpl->SetOwner(this);
	}
	if (!m_pFlashDispatch)
		m_pFlashDispatch = new CFlashDispatch(this);
}

bool CFlashUI::DoCreateControl()
{
	if (m_bCreated)
		return true;
	if (m_pFlashImpl)
	{
		m_bCreated = true;
		m_pFlashImpl->CreateControl();

		RegisterEventHandler(true);
	}
	return true;
}

void CFlashUI::ReleaseControl()
{
	RegisterEventHandler(false);
	m_bCreated = false;
	if (m_pFlashImpl)
	{
		m_pFlashImpl->SetOwner(NULL);
		delete m_pFlashImpl;
	}
	m_pFlashImpl = NULL;
	if (m_pFlashDispatch)
	{
		m_pFlashDispatch->m_pOwner = NULL;
		delete m_pFlashDispatch;
	}
	m_pFlashDispatch = NULL;
}

HWND CFlashUI::GetHostWindow()
{
	CWindowUI* pWindow = GetWindow();
	if (pWindow)
	{
		return pWindow->GetHWND();
	}
	return HWND_DESKTOP;
}

void CFlashUI::Show( LPCTSTR lpszUrl )
{
	if (m_pFlashImpl && m_pFlashImpl->flash_)
	{
		m_pFlashImpl->flash_->Stop();
		m_pFlashImpl->flash_->put_Movie(_bstr_t(lpszUrl));
		m_pFlashImpl->flash_->Play();
	}
}

void CFlashUI::SetRect( RECT& rectRegion )
{
	__super::SetRect(rectRegion);
	if (m_pFlashImpl)
	{
		m_pFlashImpl->SetRect(rectRegion);
	}
}

void CFlashUI::Init()
{
	DoCreateControl();
}

HRESULT CFlashUI::RegisterEventHandler( bool inAdvise )
{
	if (!(m_pFlashImpl && m_pFlashImpl->flash_.get() && m_pFlashDispatch))
		return S_FALSE;

	util_activex::ScopedComPtr<IConnectionPointContainer> conn_Cont;
	conn_Cont.QueryFrom(m_pFlashImpl->flash_);
	if (conn_Cont.get())
	{
		util_activex::ScopedComPtr<IConnectionPoint> conn_Point;
		conn_Cont->FindConnectionPoint(__uuidof(_IShockwaveFlashEvents), conn_Point.Receive());
		if (conn_Point.get())
		{
			if (inAdvise)
			{
				conn_Point->Advise(m_pFlashDispatch, &m_dwCookie);
			}
			else
			{
				conn_Point->Unadvise(m_dwCookie);
				m_dwCookie = 0;
			}
		}
	}
	return S_OK; 
}

void CFlashUI::Render( IRenderDC* pRenderDC, RECT& rcPaint )
{
	__super::Render(pRenderDC, rcPaint);
	if (m_pFlashImpl)
	{
		RECT rcItem = GetRect();
		
		m_pFlashImpl->ax_host_->Draw(pRenderDC->GetDC(), rcItem);
	}
}

bool CFlashUI::Event( TEventUI& event )
{
	if (!(m_pFlashImpl && m_pFlashImpl->ax_host_.get()))
		return true;

	if(!m_bFlashEvent)
		return true;

 	switch (event.nType)
 	{
 	case UIEVENT_SETCURSOR:
 		{
			//util_activex_gfx::Point point(event.ptMouse);
 			//m_pFlashImpl->ax_host_->OnSetCursor(point);
 		}
 		break;
 	case UIEVENT_BUTTONDOWN:
 	case UIEVENT_BUTTONUP:
 	case UIEVENT_RBUTTONDOWN:
 	case UIEVENT_RBUTTONUP:
 	case UIEVENT_LDBCLICK:
 	case UIEVENT_RDBCLICK:
 	case UIEVENT_MOUSEMOVE:
 		{
 			UINT uMessage = 0;
 			switch(event.nType)
 			{
 			case UIEVENT_BUTTONDOWN:	uMessage = WM_LBUTTONDOWN;		break;
 			case UIEVENT_BUTTONUP:		uMessage = WM_LBUTTONUP;		break;
 			case UIEVENT_RBUTTONDOWN:	uMessage = WM_RBUTTONDOWN;		break;
 			case UIEVENT_RBUTTONUP:		uMessage = WM_RBUTTONUP;		break;
 			case UIEVENT_LDBCLICK:		uMessage = WM_LBUTTONDBLCLK;	break;
 			case UIEVENT_RDBCLICK:		uMessage = WM_RBUTTONDBLCLK;	break;
 			case UIEVENT_MOUSEMOVE:		uMessage = WM_MOUSEMOVE;		break;
 			}
 			m_pFlashImpl->ax_host_->OnWindowlessMouseMessage(uMessage, event.wParam, event.lParam);
 		}
 		break;
	case UIEVENT_SCROLLWHEEL:
		{
			UINT uMessage = 0;
			switch(event.nType)
			{
				case UIEVENT_SCROLLWHEEL:	uMessage = WM_MOUSEWHEEL;		break;
			}

			m_pFlashImpl->ax_host_->OnWindowlessMouseMessage(uMessage,event.wParam,event.lParam);
		}
		break;
 	case UIEVENT_KEYDOWN:
 	case UIEVENT_KEYUP:
 	case UIEVENT_CHAR:
 		{
 			UINT uMessage = 0;
 			switch(event.nType)
 			{
 			case UIEVENT_KEYDOWN:	uMessage = WM_KEYDOWN;	break;
 			case UIEVENT_KEYUP:		uMessage = WM_KEYUP;	break;
 			case UIEVENT_CHAR:		uMessage = WM_CHAR;		break;
 			}
 			m_pFlashImpl->ax_host_->OnWindowMessage(uMessage, event.wParam, event.lParam);
 		}
 		break;
 	}
	return true;
}

HRESULT CFlashUI::OnReadyStateChange (long newState)
{
	return S_OK;
}

HRESULT CFlashUI::OnProgress(long percentDone )
{
	return S_OK;
}

HRESULT CFlashUI::FSCommand (LPCTSTR command, LPCTSTR args)
{
	return S_OK;
}

HRESULT CFlashUI::FlashCall( LPCTSTR request )
{
	return S_OK;
}
