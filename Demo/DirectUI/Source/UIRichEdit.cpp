/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UIRichEdit.h"
#include "UICaret.h"
#include <regex>
using namespace std::tr1; 

// richedit内部定义的几个timer id
#define RETID_BGND_RECALC	0x01af
#define RETID_AUTOSCROLL	0x01b0
#define RETID_SMOOTHSCROLL	0x01b1
#define RETID_DRAGDROP		0x01b2
#define RETID_MAGELLANTRACK	0x01b3

#define _RichDraw_Timer_Elapse_ 100
#define COLOR_LINK RGB(0, 144, 193)
#define COLOR_LINK_CLICK RGB(255, 0, 0)
const int g_MaxArraySize_UEdit = 512;


const int URL_COUNT = 6;
TCHAR *FURLValues[]={ 
	_T("http:"),
	_T("file:"),
	_T("mailto:"),
	_T("ftp:"),
	_T("https:"),
	_T("\\\\")
};

static void DetectURLs(LPCTSTR lpszText, CStdPtrArray& arrLinksOut);
/////////////////////////////////////////////////////////////////////////////
//接口定义
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
	0x8d33f740,
	0xcf58,
	0x11ce,
	{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
	0xc5bdd8d0,
	0xd26e,
	0x11ce,
	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};


int CF_HTML = 0;
static int s_tmp_image_count = 0;
typedef struct tagClipboardDataHeader
{
	int cdhOffset;
	int cdhImgCount;
}ClipboardDataHeader,*LPClipboardDataHeader;


typedef struct tagClipboardImgData
{
	WCHAR cidPath[MAX_PATH];
	DWORD cidCharPos;
	uint8 cidImgType;
	WCHAR cidKey[MAX_PATH];
	bool  cdiCache;
}ClipboardImgData,*LPClipboardImgData;

typedef struct tagClipboardData
{
	ClipboardDataHeader cdHeader;
	ClipboardImgData cdImage[1];
}ClipboardData,*LPClipboardData;

////////////////////////////////////////////////////////////////////////////////////////////
class CAnimationHandler
{
public:
	CAnimationHandler()
	{
		m_objTimerSource += MakeDelegate(this, &CAnimationHandler::OnTimer);
		m_objTimerSource.SetTimerParam(100);
	}
	~CAnimationHandler()
	{
		m_objTimerSource.Stop();
	}

	void AddRichedit(CRichEditUI* pRichEdit)
	{
		m_arrayRicheditList.Add(pRichEdit);
		if (m_arrayRicheditList.GetSize() == 1)
			m_objTimerSource.Start();
	}

	void RemoveRichedit(CRichEditUI* pRichEdit)
	{
		m_arrayRicheditList.Remove(pRichEdit);
		if (m_arrayRicheditList.GetSize() == 0)
			m_objTimerSource.Stop();
	}
private:
	void OnTimer(CUITimerBase* pTimer)
	{
		int nCount = m_arrayRicheditList.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CRichEditUI* pRichEdit = (CRichEditUI*)m_arrayRicheditList.GetAt(i);
			pRichEdit->OnDrawAnimation(&pRichEdit->GetClientRect());
		}

	}
	CStdPtrArray m_arrayRicheditList;
	CTimerSource m_objTimerSource;
};
///////////////////////////////////////////////////////////////////////////////////////////////
CAnimationHandler m_objAnimationHandler;
//////////////////////////////////////////////////////////

COleControlObj::COleControlObj(CControlUI* pControl)
{
	m_pControl = pControl;
	ZeroMemory(&m_rcLastBounds, sizeof(RECT));
	m_bAutoSize = false;
}

COleControlObj::~COleControlObj()
{
	if (m_pControl)
		((CControlUI*)GetRichEdit())->Remove(m_pControl);
}

void COleControlObj::SetAutoSize(bool bAutoSize)// 控件随richedit宽度变化而变化
{
	m_bAutoSize = bAutoSize;
}

CControlUI* COleControlObj::GetControl()
{
	return m_pControl;
}


void COleControlObj::SetSize(int nWidth,int nHeight)
{
	ZeroMemory(&m_rcLastBounds, sizeof(m_rcLastBounds));
	__super::SetSize(nWidth, nHeight);
}

void COleControlObj::OnPositonChanged(LPCRECT lpRect)
{
	__super::OnPositonChanged(lpRect);
	if (!EqualRect(&m_rcLastBounds, lpRect) || ::IsRectEmpty(m_pControl->GetRectPtr()) || IsReLayoutOle())
	{
		m_pControl->OnSize(GetBound());

		if (GetUIEngine()->IsRTLLayout())
		{
			RECT rect = m_pControl->GetRect();
			RtlRect(GetBound(), &rect);
			m_pControl->SetRect(rect);

		}

		m_rcLastBounds = *lpRect;
	}

}

void COleControlObj::OnRichEditSizeChanged(RECT& rectParent)
{
	if (!m_bAutoSize) return;

	SIZEL sizel;
	GetExtent(DVASPECT_CONTENT, &sizel);

	long himetric_width = DXtoHimetricX(rectParent.right - rectParent.left);
	if (sizel.cx != himetric_width)
	{
		sizel.cx = himetric_width;
		SetExtent(DVASPECT_CONTENT, &sizel);
		FireViewChange();
	}
}

bool COleControlObj::IsSelected()
{
	if (!GetRichEdit()) return false;
	int nObjCnt = GetRichEdit()->GetIRichEditOle()->GetObjectCount();
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(reobject);
	for (int i=0; i < nObjCnt; i++)
	{
		GetRichEdit()->GetIRichEditOle()->GetObject(i, &reobject, REO_GETOBJ_POLEOBJ);
		if (reobject.poleobj == this)
		{
			reobject.poleobj->Release();
			return reobject.dwFlags & (REO_SELECTED);
		}
		reobject.poleobj->Release();
	}
	return false;
}

HRESULT WINAPI COleControlObj::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
								  HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
								  BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue)
{
	HRESULT hr = __super::Draw(dwDrawAspect, lindex, pvAspect, ptd, hdcTargetDev, hdcDraw, lprcBounds, lprcWBounds, pfnContinue, dwContinue);
	IRenderDC*  pRenderDC = GetRenderDC();
	if (!pRenderDC || !m_pControl) return hr;

	OnPositonChanged((LPCRECT)lprcBounds);

	CControlUI* pOwner = GetRichEdit();
	UICustomControlHelper::RenderChildControl(pRenderDC, pRenderDC->GetUpdateRect(), pOwner->GetRect(), pOwner->GetClientRect(),m_pControl);
	if (IsSelected())
	{
		InvertRect(pRenderDC->GetDC(), (LPCRECT)lprcBounds);
	}
	return hr;
}
///////////////////////////////////////////////////////////////////

ReTextObj::ReTextObj()
{
	m_cpMin = -1;
	m_cpMax = -1;
	m_bIsUpdating = false;


}

ReTextObj::~ReTextObj()
{

}


void ReTextObj::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
}

LPCTSTR ReTextObj::GetText()
{
	return m_strText.c_str();
}

void ReTextObj::GetRange(LONG& cpMin, LONG& cpMax)
{
	cpMin = m_cpMin;
	cpMax = m_cpMax;
}
/////////////////////////////////////////////////////////////////

COleObject::COleObject()
{
	m_dwRef = 0;
	AddRef();
	m_pOleClientSite = NULL;
	m_spAdviseSink = NULL;
	m_pOwner = NULL;
	memset(&m_rcBounds, 0, sizeof(RECT));
	m_dwId = -1;
}

COleObject::~COleObject()
{
	ReleaseResource();
}

void COleObject::SetOwner(CRichEditUI* pOwner)
{
	m_pOwner = pOwner;
}

HRESULT STDMETHODCALLTYPE 
COleObject::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = NOERROR;
	*ppvObject = NULL;
	if(iid == IID_IOleInPlaceSite)
	{
		int i = 0;
	}

	if ( iid == IID_IUnknown ||
		iid == IID_IOleObject)
	{
		*ppvObject = this;
		AddRef();
	}
	else if (iid == IID_IViewObject || iid == IID_IViewObject2)
	{
		*ppvObject = (IViewObject2 *)this;
		AddRef();
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE 
COleObject::AddRef()
{
	 ++m_dwRef;
	 return m_dwRef;
}



ULONG STDMETHODCALLTYPE 
COleObject::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}
	return m_dwRef;
}

void COleObject::LoadResource()
{

}

void COleObject::ReleaseResource()
{
	
}


IRenderDC* COleObject::GetRenderDC()
{
	return m_pOwner->m_pRenderDC;
}

bool COleObject::IsReLayoutOle()
{
	return m_pOwner->m_bReLayoutOle;
}

CRichEditUI* COleObject::GetRichEdit()
{
	return m_pOwner;
}

HRESULT  __stdcall COleObject::SetClientSite( IOleClientSite *pClientSite)
{
	m_pOleClientSite = pClientSite;
	return S_OK;
};
HRESULT  __stdcall COleObject::GetClientSite( IOleClientSite **ppClientSite)
{
	*ppClientSite = m_pOleClientSite;
	return S_OK;
};
HRESULT  __stdcall COleObject::SetHostNames( LPCOLESTR szContainerApp,  LPCOLESTR szContainerObj)
{
	return E_NOTIMPL;
};
HRESULT  __stdcall COleObject::Close( DWORD dwSaveOption)
{
	return S_OK;
};
HRESULT  __stdcall COleObject::SetMoniker( DWORD dwWhichMoniker, IMoniker *pmk)
{
	return E_NOTIMPL;
};
HRESULT  __stdcall COleObject::GetMoniker( DWORD dwAssign,  DWORD dwWhichMoniker,IMoniker **ppmk)
{
	return E_NOTIMPL;
};
HRESULT  __stdcall COleObject::InitFromData( 
										IDataObject *pDataObject,  //Pointer to data object
										BOOL fCreation,            //Specifies how object is created
										DWORD dwReserved           //Reserved
										)
{
	return E_NOTIMPL;
};

HRESULT  __stdcall COleObject::GetClipboardData( 
	DWORD dwReserved,  //Reserved
	IDataObject **ppDataObject
	//Address of output variable that receives the 
	// IDataObject interface pointer
	)
{
	return E_NOTIMPL;
};

HRESULT  __stdcall COleObject::DoVerb( 
								  LONG iVerb,          //Value representing verb to be performed
								  LPMSG lpmsg,         //Pointer to structure that describes Windows 
								  // message
								  IOleClientSite *pActiveSite,
								  //Pointer to active client site
								  LONG lindex,         //Reserved
								  HWND hwndParent,     //Handle of window containing the object
								  LPCRECT lprcPosRect  //Pointer to object's display rectangle
								  )
{
	return E_NOTIMPL;
};
HRESULT  __stdcall COleObject::EnumVerbs( 
									 IEnumOLEVERB **ppEnumOleVerb  //Address of output variable that 
									 // receives the IEnumOleVerb interface 
									 // pointer
									 )
{
	return E_NOTIMPL;
};
HRESULT  __stdcall COleObject::Update()
{
	return E_NOTIMPL;
};
HRESULT  __stdcall COleObject::IsUpToDate()
{
	return E_NOTIMPL;
};
HRESULT  __stdcall COleObject::GetUserClassID( 
	CLSID *pClsid   //Pointer to the class identifier
	)
{
	return E_NOTIMPL;
};

HRESULT  __stdcall COleObject::GetUserType( 
									   DWORD dwFormOfType,  //Specifies form of type name
									   LPOLESTR *pszUserType
									   //Address of output variable that receives a 
									   // pointer to the requested user type string
									   )
{
	return E_NOTIMPL;
};

HRESULT __stdcall  COleObject::SetExtent( 
									 DWORD dwDrawAspect,  //DVASPECT value
									 SIZEL  *psizel       //Pointer to size limit for object
									 )
{
	m_sizeExtent = *psizel;
	return S_OK;
};

HRESULT  __stdcall COleObject::GetExtent( 
									 DWORD dwDrawAspect,  //Value indicating object aspect
									 SIZEL *psizel        //Pointer to storage of object size limit
									 )
{
	*psizel = m_sizeExtent;
	return S_OK;
};

SIZE COleObject::GetSize()
{
	SIZE sz;
	sz.cx = HimetricXtoDX(m_sizeExtent.cx);
	sz.cy = HimetricYtoDY(m_sizeExtent.cy);
	return sz;

}

void COleObject::SetSize(int nWidth,int nHeight)
{
	SIZEL sizel;
	sizel.cx = DXtoHimetricX(nWidth);
	sizel.cy = DYtoHimetricY(nHeight);
	DPI_SCALE(&sizel);
	SetExtent(DVASPECT_CONTENT,&sizel);		
}

HRESULT  __stdcall COleObject::Advise( 
								  IAdviseSink *pAdvSink,  //Pointer to advisory sink
								  DWORD *pdwConnection    //Pointer to a token
								  )
{
	m_spAdviseSink = pAdvSink;
	return S_OK;
};
HRESULT  __stdcall COleObject::Unadvise( 
									DWORD dwConnection  //Token
									)
{
	return E_NOTIMPL;
};

HRESULT  __stdcall COleObject::EnumAdvise( 
									  IEnumSTATDATA **ppenumAdvise  //Address of output variable that 
									  // receives the IEnumSTATDATA interface 
									  // pointer
									  )
{
	return E_NOTIMPL;
};

HRESULT  __stdcall COleObject::GetMiscStatus( 
	DWORD dwAspect,  //Value indicating object aspect
	DWORD *pdwStatus //Pointer to storage of status information
	)
{
	return E_NOTIMPL;
};

HRESULT  __stdcall COleObject::SetColorScheme( 
	LOGPALETTE *pLogpal   //Pointer to a structure
	)
{
	return E_NOTIMPL;
};




HRESULT WINAPI COleObject::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
								   HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
								   BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue)
{
	if (lindex != -1)
		return S_FALSE;
	m_rcBounds.left   = lprcBounds->left;
	m_rcBounds.top    = lprcBounds->top;
	m_rcBounds.right  = lprcBounds->right;
	m_rcBounds.bottom = lprcBounds->bottom;
	LoadResource();
	return S_OK;
}



HRESULT WINAPI COleObject::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
										  DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
{
	return E_NOTIMPL;
}

HRESULT WINAPI COleObject::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI COleObject::Unfreeze(DWORD dwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI COleObject::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
{

	return E_NOTIMPL;
}

HRESULT WINAPI COleObject::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	return E_NOTIMPL;
}

HRESULT WINAPI COleObject::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
{
	return E_NOTIMPL;
}

HRESULT WINAPI COleObject::GetRect(LPRECT lpRect)
{
	if (!lpRect) return S_FALSE;
	*lpRect = m_rcBounds;
	return S_OK;
}

void COleObject::SetId(DWORD dwId)
{
	m_dwId = dwId;
}

DWORD COleObject::GetId()
{
	return m_dwId;
}

long  COleObject::GetCharPos()
{
	if (!m_pOwner) return - 1;
	int nObjCnt = m_pOwner->GetIRichEditOle()->GetObjectCount();
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(reobject);
	for (int i=0; i < nObjCnt; i++)
	{
		m_pOwner->GetIRichEditOle()->GetObject(i, &reobject, REO_GETOBJ_POLEOBJ);
		if (reobject.poleobj == this)
		{
			reobject.poleobj->Release();
			return reobject.cp;
		}
		reobject.poleobj->Release();
	}
	return - 1;

}

RECT COleObject::GetBound()
{
	return m_rcBounds;
}

void COleObject::OnPositonChanged(LPCRECT lpRect)
{
	m_rcBounds = *lpRect;
}

void COleObject::FireViewChange()
{
	if (m_spAdviseSink)
		m_spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
}

void COleObject::OnMouseEnter(TEventUI& event)
{

}

void COleObject::OnMouseLeave(TEventUI& event)
{
}

void COleObject::OnMouseMove(TEventUI& event)
{
}

void COleObject::OnClick(TEventUI& event)
{
	
}
void COleObject::OnRClick(TEventUI& event)
{

}
void COleObject::OnDbClick(TEventUI& event)
{
	
}

void COleObject::OnRichEditSizeChanged(RECT& rectParent)
{

}

////////////////////////////////////////////////////////////
class IExRichEditOleCallback : public IRichEditOleCallback
{
public:
	IExRichEditOleCallback(CRichEditUI* pOwner);
	virtual ~IExRichEditOleCallback();
	LONG m_dwRef;
	virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
		LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
	virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
	virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
	virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
		DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
	virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
	virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
	virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
		HMENU FAR *lphmenu);

	CRichEditUI* m_pOwner;
};



IExRichEditOleCallback::IExRichEditOleCallback(CRichEditUI* pOwner)
{
	m_dwRef = 1;
	m_pOwner = pOwner;
}

IExRichEditOleCallback::~IExRichEditOleCallback()
{
}

HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;

	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE 
IExRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
IExRichEditOleCallback::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
															LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return E_NOTIMPL;
}



HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	COleObject* pOleObject = (COleObject*)lpoleobj;
	if (pOleObject)
	{
		m_pOwner->OnOleRelease(lpoleobj);
		pOleObject->ReleaseResource();
		if (pOleObject == m_pOwner->m_pOleObjHover)
			m_pOwner->m_pOleObjHover = NULL;
	}
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
										DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	if (!fReally) return S_OK;
	switch(reco)  
	{
	case RECO_DROP:
		{
			if (!m_pOwner || m_pOwner->m_bReadOnly) 
				return E_NOTIMPL;
			STGMEDIUM stgMedium;
			FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
			HRESULT ret = lpdataobj->GetData(&fmt, &stgMedium);
			if (ret == S_OK && stgMedium.hGlobal)
			{
				if (m_pOwner)
					m_pOwner->SendNotify(UINOTIFY_DROPFILES, (WPARAM)stgMedium.hGlobal, NULL);
			}
			else
			{
				if (m_pOwner)
					m_pOwner->OnPaste(lpdataobj);
			}
			return S_FALSE;
		}
		break;
	}
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	switch(reco)  
	{
	case RECO_COPY:  
		{
			IDataObject* pDataObject = NULL;
			if (m_pOwner->OnCopy(&pDataObject))
				*lplpdataobj = pDataObject;
			else
			{
				 pDataObject->Release();
			}
			return S_OK;

		}
		break;
	}
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE 
IExRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
														 HMENU FAR *lphmenu)
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTextHost: public CTextHostBase
{
	friend class CRichEditUI;
public:
	CTextHost(	CRichEditUI* pOwner);
	virtual ~CTextHost();

	// 重新改变图片大小需要绘制和滚动条
	void LockScrollBar();
	void UnLockScrollBar();
protected:
	virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
	virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
	virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
	virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
	virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
	virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual HRESULT TxNotify(DWORD iNotify, void *pv);
private:
	bool m_bLockScrollBar;

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTextHost::CTextHost(CRichEditUI* pOwner):CTextHostBase(pOwner)
{
	m_bLockScrollBar = false;

}
CTextHost::~CTextHost()
{

}

BOOL CTextHost::TxShowScrollBar(INT fnBar, BOOL fShow)
{
	if (SB_VERT == fnBar && this->fVerticalScrollBar)
	{
		CScrollbarUI* pScrollBar = ((CRichEditUI*)m_pOwner)->GetVScrollbar();
		if (!pScrollBar) return TRUE;
		pScrollBar->SetVisible(!!fShow);
	}
	return true;
}

BOOL CTextHost::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
{
	if( fuSBFlags == SB_VERT  && this->fVerticalScrollBar )
	{
		CScrollbarUI* pScrollBar = ((CRichEditUI*)m_pOwner)->GetVScrollbar();
		if (!pScrollBar) return TRUE;

		bool bVBarVisible = fuArrowflags != ESB_DISABLE_BOTH;
		if (bVBarVisible != pScrollBar->IsVisible())
		{
			pScrollBar->SetVisible(bVBarVisible);
			m_pOwner->Resize();
		}
	}
	return TRUE;
}

BOOL CTextHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
	if (SB_VERT == fnBar && rcClient.bottom > 0  && this->fVerticalScrollBar )
	{
		CScrollbarUI* pScrollBar = ((CRichEditUI*)m_pOwner)->GetVScrollbar();
		if (!pScrollBar) return TRUE;

		int nRange = nMaxPos - nMinPos - rcClient.bottom + rcClient.top;
		if(nRange <= 0 ) {
			pScrollBar->SetVisible(false);
			pScrollBar->SetScrollRange(0);
		}
		else {
			pScrollBar->SetVisible(true);
			pScrollBar->SetScrollRange(nRange);
		}
	}
	return TRUE;
}

BOOL CTextHost::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
{
	if( fnBar == SB_VERT  && this->fVerticalScrollBar )
	{
		CScrollbarUI* pScrollBar = ((CRichEditUI*)m_pOwner)->GetVScrollbar();
		if (!pScrollBar) return TRUE;

		pScrollBar->SetScrollPos(nPos, fRedraw);
		pScrollBar->Invalidate();
	}
	return TRUE;
}

void CTextHost::LockScrollBar()
{
	m_bLockScrollBar = true;
}

void CTextHost::UnLockScrollBar()
{
	m_bLockScrollBar = false;
}

HRESULT CTextHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
	HRESULT hr = __super::TxGetScrollBars(pdwScrollBar);
	if (m_bLockScrollBar)
	{
		*pdwScrollBar &= ~(WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
			ES_AUTOHSCROLL | ES_DISABLENOSCROLL);
	}
	return hr;
}
void CTextHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	if (m_bLockScrollBar) return;
	__super::TxInvalidateRect(prc, fMode);
}

HRESULT CTextHost::TxNotify(DWORD iNotify, void *pv)
{
	if (iNotify == EN_REQUESTRESIZE)
	{
		REQRESIZE* lpre = (REQRESIZE*)pv;
		if (rcClient.bottom - rcClient.top != lpre->rc.bottom - lpre->rc.top)
		{
			((CRichEditUI*)m_pOwner)->OnRequestResize(&lpre->rc);
		}

		return S_OK;
	}
	return __super::TxNotify(iNotify, pv);

}

///////////////////////////////////////////////////////////////////////////

COleImageObj::COleImageObj()
{
	m_ImageObj = NULL;
	m_nActiveFrame = 0;
	m_nImageType = 0;
	m_bCache = false;
	m_pZoomImage = NULL;
	m_bInThumbnail = false;
}

COleImageObj::~COleImageObj()
{
	ReleaseResource();
}


COleImageObj* COleImageObj::LoadImage(LPCTSTR lpszPath, bool bCache)
{
	ImageObj* pImageRender = GetUIRes()->LoadImage(lpszPath, bCache, UIIAMGE_BITMAP);
	if (!pImageRender)
		return NULL;

	COleImageObj* pImageObj = new COleImageObj;
	pImageObj->m_ImageObj = pImageRender;
	pImageObj->m_bCache = bCache;
	pImageObj->m_strPath = lpszPath;
	if (pImageRender->GetFrameCount() > 1)
		pImageObj->m_nFrameDelay = pImageRender->GetFrameDelay(0);
	return pImageObj;
}

COleImageObj* COleImageObj::LoadImage(HICON hIcon )
{
	ImageObj* pImageRender = new ImageObj(hIcon);
	COleImageObj* pImageObj = new COleImageObj;
	pImageObj->m_ImageObj = pImageRender;
	return pImageObj;
}



ImageObj* COleImageObj::GetImage()
{
	if (m_ImageObj == NULL)
		LoadResource();
	return m_ImageObj;
}

void COleImageObj::SetImage(ImageObj* pImageObj)
{
	if (!pImageObj) return;
	if (m_ImageObj)
		m_ImageObj->Release();
	m_ImageObj = pImageObj->Get();
	m_strPath = pImageObj->GetFilePath();
	m_bCache = m_ImageObj->IsCached();
	if (m_ImageObj->GetFrameCount() > 1)
		m_nFrameDelay = m_ImageObj->GetFrameDelay(0);
	pImageObj->SetRightOrientation();

	if (GetRichEdit())
		OnRichEditSizeChanged(GetRichEdit()->GetTextHost()->rcClient);
}

tstring COleImageObj::GetFilePath()
{
	return m_strPath.c_str();
}

void COleImageObj::SetFilePath(LPCTSTR lpszPath)
{
	m_strPath = lpszPath;
}


int  COleImageObj::GetImageType()
{
	return m_nImageType;
}
void COleImageObj::SetImageType(int nType)
{
	m_nImageType = nType;
}
void COleImageObj::SetKey(LPCTSTR lpszKey)
{
	m_strKey = lpszKey;
}
LPCTSTR COleImageObj::GetKey()
{
   return m_strKey.c_str();
}

void COleImageObj::NextFrame()
{
	if (m_ImageObj)
		m_nActiveFrame = (m_nActiveFrame + 1) % m_ImageObj->GetFrameCount();
}

int COleImageObj::GetFrameDelay()
{
	if (m_ImageObj) return m_ImageObj->GetFrameDelay(m_nActiveFrame);
	return 0;
}

bool COleImageObj::IsThumbnailImage()
{
	if (!m_ImageObj) return false;
	RECT rect = GetBound();
	int nCurWidth = rect.right - rect.left - 2;
	if (nCurWidth <= 0) nCurWidth = m_ImageObj->GetWidth();
	return nCurWidth < m_ImageObj->GetWidth();
}

int  COleImageObj::GetFrameCount()
{
	if (!m_ImageObj) return 0; 
	return m_ImageObj->GetFrameCount();
}

void COleImageObj::LoadResource()
{
	if (m_ImageObj || m_strPath.empty()) return;
	m_ImageObj = GetUIRes()->LoadImage(m_strPath.c_str(), m_bCache, UIIAMGE_BITMAP);
	if (!m_ImageObj) return;

}

void COleImageObj::ReleaseResource()
{
	if (m_ImageObj)
		m_ImageObj->Release();
	m_ImageObj = NULL;
	//m_strPath = _T("");

	if (m_pZoomImage)
		m_pZoomImage->Release();
	m_pZoomImage = NULL;

}

#define OLEIMAGE_RIGHT_MARGIN DPI_SCALE(35)
#define OLEIMAGE_TOP_MARGIN DPI_SCALE(35)

void COleImageObj::OnRichEditSizeChanged(RECT& rectParent)
{
	if (!m_ImageObj) LoadResource();
	if (!m_ImageObj) return;

	int nImageWidth = m_ImageObj->GetWidth() + 2; //图片宽度，边缘留两个像素
	int nImageHeight = m_ImageObj->GetHeight() + 2;

	nImageWidth = /*DPI_SCALE*/(nImageWidth);
	nImageHeight = /*DPI_SCALE*/(nImageHeight);

	if (GetFrameCount() > 1)
	{
		SIZEL oldsizel;
		GetExtent(DVASPECT_CONTENT, &oldsizel);

		SIZEL sizel;
		sizel.cx = DXtoHimetricX(nImageWidth); 
		sizel.cy = DYtoHimetricY(nImageHeight);
		/*DPI_SCALE(&sizel);*/
		SetExtent(DVASPECT_CONTENT, &sizel);

		if (sizel.cx != oldsizel.cx)
			FireViewChange();
		return;
	}

	SIZEL oldsizel;
	GetExtent(DVASPECT_CONTENT, &oldsizel);

	RECT rect = GetBound();
	int nOleWidth = rect.right - rect.left;//ole宽度

	int nThumbnailWidth = rectParent.right - rectParent.left - GetRichEdit()->GetStartIndent() - OLEIMAGE_RIGHT_MARGIN;//扣除8像素否则一部分无分显示出来
	int nThumbnailHeight = rectParent.bottom - rectParent.top - OLEIMAGE_TOP_MARGIN;

	if (nImageWidth / (double)nImageHeight > nThumbnailWidth / (double)nThumbnailHeight)
	{
		if (nImageWidth > nThumbnailWidth) //图片比控件大
		{
			int nNewWidth = nThumbnailWidth;
			if (nNewWidth < 1)
				nNewWidth = 1;
			SIZEL sizel;

			sizel.cx = DXtoHimetricX(nNewWidth);
			sizel.cy = DYtoHimetricY(nImageHeight) * nNewWidth / nImageWidth;
			/*DPI_SCALE(&sizel);*/
			SetExtent(DVASPECT_CONTENT, &sizel);
		}
		else if (nOleWidth !=  nImageWidth) //图片宽度和缩略图宽度不一样
		{
			SIZEL sizel;

			sizel.cx = DXtoHimetricX(nImageWidth);
			sizel.cy = DYtoHimetricY(nImageHeight);
		/*	DPI_SCALE(&sizel);*/
			SetExtent(DVASPECT_CONTENT, &sizel);
		}
	}
	else
	{
		if (nImageHeight > nThumbnailHeight) //图片比控件大
		{
			int nNewHeight = nThumbnailHeight;
			if (nNewHeight < 1)
				nNewHeight = 1;
			SIZEL sizel;

			sizel.cy = DXtoHimetricX(nNewHeight);
			sizel.cx = DYtoHimetricY(nImageWidth) * nNewHeight/ nImageHeight;
			/*DPI_SCALE(&sizel);*/
			SetExtent(DVASPECT_CONTENT, &sizel);
		}
		else if (nOleWidth !=  nImageWidth) //图片宽度和缩略图宽度不一样
		{
			SIZEL sizel;

			sizel.cx = DXtoHimetricX(nImageWidth);
			sizel.cy = DYtoHimetricY(nImageHeight);
	/*		DPI_SCALE(&sizel);*/
			SetExtent(DVASPECT_CONTENT, &sizel);
		}

	}

	SIZEL newsizel;
	GetExtent(DVASPECT_CONTENT, &newsizel);
	if (newsizel.cx != oldsizel.cx || newsizel.cy != oldsizel.cy)
		FireViewChange();
	
}

HRESULT WINAPI COleImageObj::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
									HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
									BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue)
{
	HRESULT hr = __super::Draw(dwDrawAspect, lindex, pvAspect, ptd, hdcTargetDev, hdcDraw, lprcBounds, lprcWBounds, pfnContinue, dwContinue);
	IRenderDC*  pRenderDC = GetRenderDC();
	if (!pRenderDC) return hr;

	if (m_ImageObj)
	{
		RECT rcBounds = GetBound();
		RECT rcImage = {rcBounds.left + 1, rcBounds.top + 1, rcBounds.right - 1, rcBounds.bottom - 1};
		
		PaintParams param;
		param.eSmoothingMode = UISMOOTHING_HighQuality;
		pRenderDC->DrawImage(m_ImageObj, rcImage, m_nActiveFrame, &param);// xqb 临时去掉

		if (IsThumbnailImage())
		{
			if (!m_pZoomImage) m_pZoomImage = GetUIRes()->LoadImage(_T("#pic_zoom"));
			if (m_pZoomImage)
			{
				int nLeft = m_pZoomImage->GetCellWidth();
				int nTop = m_pZoomImage->GetCellHeight();
				nLeft = rcImage.right - nLeft - 5;
				nTop = rcImage.bottom - nTop - 5;
				pRenderDC->DrawImage(m_pZoomImage, nLeft, nTop, (int)m_bInThumbnail);;// xqb 临时去掉
			}
		}
	}

	return hr;
}

void COleImageObj::OnMouseLeave(TEventUI& event)
{
	m_bInThumbnail = false;

}
void COleImageObj::OnMouseMove(TEventUI& event)
{
	if (!m_ImageObj || !IsThumbnailImage())
	{
		GetRichEdit()->SetToolTip(_T(""));
		return;
	}
	if (!m_pZoomImage) return;
	
	RECT rect;
	RECT rcBounds = GetBound();
	rect.left = rcBounds.right - 1 - m_pZoomImage->GetCellWidth() - 5;
	rect.top =  rcBounds.bottom - 1 - m_pZoomImage->GetCellHeight() - 5;
	rect.right = rect.left + m_pZoomImage->GetCellWidth();
	rect.bottom = rect.top + m_pZoomImage->GetCellHeight();
	if (PtInRect(&rect, event.ptMouse))
	{
		if (!m_bInThumbnail)
		{
			m_bInThumbnail = true;
			GetRichEdit()->GetWindow()->Invalidate(&rect);
			TCHAR szToolTips[30];
			_stprintf_s(szToolTips, 30, I18NSTR(_T("#SYS_RICHEDIT_THUMBNAIL_TIPS")), (int)GetScale());
			GetRichEdit()->SetToolTip(szToolTips);
		}
	}
	else
	{
		if (m_bInThumbnail)
		{
			m_bInThumbnail = false;
			GetRichEdit()->GetWindow()->Invalidate(&rect);
			GetRichEdit()->SetToolTip(_T(""));
			GetRichEdit()->GetWindow()->HideToolTips();
		}
	}
}
void COleImageObj::OnClick(TEventUI& event)
{
	if (!m_ImageObj) return;
	if (IsThumbnailImage() && m_pZoomImage)	
	{
		RECT rect;
		RECT rcBounds = GetBound();
		rect.left = rcBounds.right - 1 - m_pZoomImage->GetCellWidth() - 5;
		rect.top =  rcBounds.bottom - 1 - m_pZoomImage->GetCellHeight() - 5;
		rect.right = rect.left + m_pZoomImage->GetCellWidth();
		rect.bottom = rect.top + m_pZoomImage->GetCellHeight();
		if (PtInRect(&rect, event.ptMouse) && GetRichEdit())
		{
			GetRichEdit()->SendNotify(UINOTIFY_RE_CLICK_IMAGE_THUMBNAIL_BTN, (WPARAM)0, (LPARAM)this);
		}
	}
}
void COleImageObj::OnDbClick(TEventUI& event)
{
	
	GetRichEdit()->SendNotify(UINOTIFY_RE_DBCLICK_IMAGE, (WPARAM)0, (LPARAM)this);
}

double COleImageObj::GetScale()
{
	if (!m_ImageObj) return 0;
	RECT rect = GetBound();
	int nCurWidth = rect.right - rect.left - 2;
	if (nCurWidth <= 0) nCurWidth = m_ImageObj->GetWidth();
	return (double)((nCurWidth) * 100) / m_ImageObj->GetWidth();
}


/////////////////////////////////////////////////////////////////////////////////
CLinkObj::CLinkObj()
{
	m_dwType = LINK_URL;
}
CLinkObj::~CLinkObj()
{

}

////////////////////////////////////////////////////////////////////////////
void FillCharFormat(TEXTFORMAT& in, CHARFORMAT2& out)
{
	out.dwMask = in.dwMask | CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE;
	out.dwEffects = in.dwEffects;
	out.lcid = in.lcid;

	if (in.dwMask & CFM_SIZE)
		out.yHeight = in.dwFontSize * 20;
	if (in.dwMask & CFM_COLOR)
		out.crTextColor = in.crTextColor;
	if (in.dwMask & CFM_FACE)
		_tcscpy(out.szFaceName, in.szFaceName);
}


static UINT m_clip_board_format = 0;
HRESULT InitDefaultCharFormat(CRichEditUI* re, CHARFORMAT2* pcf) 
{
	LOGFONT lf;
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfCharSet = DEFAULT_CHARSET;
	memset(pcf, 0, sizeof(CHARFORMAT2));
	pcf->lcid = 0;
	pcf->cbSize = sizeof(CHARFORMAT2);
	pcf->crTextColor = re->GetTextColor();
	pcf->yHeight = /*DPI_SCALE*/(re->GetFontSize()) * 20;
	pcf->yOffset = 0;
	pcf->dwEffects = 0;
	pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
	if(re->IsBold())
		pcf->dwEffects |= CFE_BOLD;
	if(re->IsItalic())
		pcf->dwEffects |= CFE_ITALIC;
	if(re->IsUnderline())
		pcf->dwEffects |= CFE_UNDERLINE;
	_tcscpy(pcf->szFaceName, re->GetFontName());

	if (GetUIEngine()->IsRTLLayout())
	{
		pcf->bCharSet = ARABIC_CHARSET;
	}
	else
	{
		pcf->bCharSet = lf.lfCharSet;
	}

	pcf->bPitchAndFamily = lf.lfPitchAndFamily;
	return S_OK;
}

static CRichEditUI* is_control_in_richeidt(CControlUI* pControl)
{
	CControlUI* pParent = pControl->GetParent();
	if (!pParent)
		return NULL;
	if (dynamic_cast<CRichEditUI*>(pParent))
	{
		return (CRichEditUI*)pParent;
	}
	else 
	{
		return is_control_in_richeidt(pParent);
	}
}

static bool is_control_as_child_of(CControlUI* pParent, CControlUI* pChild)
{
	if (pChild == pParent)
		return true;

	bool bFind = false;
	for (int i = 0; i < pParent->GetCount(); i++)
	{
		if (pParent->GetItem(i) == pChild)
		{
			bFind = true;
			break;
		}
		else
		{
			bFind = is_control_as_child_of(pParent->GetItem(i), pChild);
			if (bFind)
				break;
		}
	}
	return bFind;
}


int get_control_charpos_in_richedit(CRichEditUI* pRichEdit, CControlUI* pControl)
{
	LONG nObjCnt = pRichEdit->GetIRichEditOle()->GetObjectCount();
	REOBJECT struREObj;
	struREObj.cbStruct = sizeof(struREObj);
	for(int i=0; i<nObjCnt; i++)
	{
		pRichEdit->GetIRichEditOle()->GetObject(i, &struREObj, REO_GETOBJ_POLEOBJ);
		COleControlObj* pOleObj = dynamic_cast<COleControlObj*>(struREObj.poleobj);
		if (pOleObj && is_control_as_child_of(pOleObj->GetControl(), pControl))
		{
			struREObj.poleobj->Release();
			return pOleObj->GetCharPos();
		}
		struREObj.poleobj->Release();
	}
	return -1;
}


 CRichEditUI:: CRichEditUI()
{	
	SetEnabled(true);
	SetMouseEnabled(true);
	SetGestureEnabled(true);
	m_bReadOnly = false;
	RECT rcInset = {2, 2, 2, 0};
	SetInset(rcInset);
	
	ModifyFlags(UICONTROLFLAG_SETCURSOR | UICONTROLFLAG_TABSTOP | UICONTROLFLAG_SETFOCUS | UICONTROLFLAG_WANTRETURN|UICONTROLFLAG_ENABLE_DROP, 0);
	SetAttribute(_T("cursor"), _T("IBEAM"));
	m_pTextHost = NULL;
	m_pVerticalScrollbar = NULL;

	m_bUnderline = false;
	m_bBold = false;
	m_bItalic = false;
	m_nFontSize = 9;
	m_strFontName = GetResObj()->GetDefaultFont()->GetFaceName();
	m_bResizing = false;
	m_bImageThumbnail = false;
	m_nStartIndent = 0;
	m_txtColor = 0;
	m_pRichEditOleCallback = NULL;
	m_pOleObjHover = NULL;
	m_bHideSelection = false;
	m_bEnabeImage = true;
	m_pTextHost = new CTextHost(this);
	m_pRenderDC = NULL;	
	m_dwObjectID = 0;
	m_bNeedUpdateOlePositon = true;
	m_bNeedReLayout = false;
	m_bEnableVScrollbar = true;
	m_bEnableCaret = true;
	m_bAutoURLDetect = true;
	m_objAnimationHandler.AddRichedit(this);
	m_bReLayoutOle = false;
	m_nMaxImageWidth = 0;
	m_nMaxImageHeight = 0;
	m_bEnableMenu = false;
}

 CRichEditUI::~CRichEditUI()
{
	m_objAnimationHandler.RemoveRichedit(this);
	if (m_pVerticalScrollbar)
		m_pVerticalScrollbar->SetVisible(false);
	if (m_pRichEditOleCallback)
		m_pRichEditOleCallback->Release();

	if (m_pTextHost != NULL)
		delete m_pTextHost;
	m_pTextHost = NULL;

	RemoveAllLinkObj();
	RemoveAllTextObj();

	if (m_pVerticalScrollbar != NULL)
		delete m_pVerticalScrollbar;
	m_pVerticalScrollbar = NULL;

}


 void CRichEditUI::Init()
 {
	 CControlUI::Init();
	 if (m_clip_board_format == 0)
		 m_clip_board_format = RegisterClipboardFormat(_T("DUI_Richedit_Format"));

	 CHARFORMAT2 cf;
	 InitDefaultCharFormat(this, &cf);
	 PARAFORMAT2 pf;
	 InitDefaultParaFormat(this, &pf);
	 m_pTextHost->fVerticalScrollBar = IsEnableVScrollBar();
	 m_pTextHost->Init(&cf, &pf);
	 m_pRichEditOleCallback = new IExRichEditOleCallback(this);
	 LRESULT lResult;
	 TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)m_pRichEditOleCallback, &lResult);

	 TxSendMessage(EM_AUTOURLDETECT, (WPARAM) false, 0, &lResult);
	 SetReadOnly(IsReadOnly());
	 //SetAutoURLDetect(IsReadOnly());
 }


void CRichEditUI::ReLayoutOle()
{
	m_bReLayoutOle = true;
	Resize();
	Invalidate();
}

void  CRichEditUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	if (m_bNeedUpdateOlePositon)
	{
		m_bNeedUpdateOlePositon = false;
		UpdateOlePosition();
	}


	RECT rcTemp = { 0 };
	RECT rcItem = GetRect();
	RECT rcClient = GetClientRect();
	CControlUI* pParent = GetParent();
	if (pParent)
	{
		::IntersectRect(&rcItem, pParent->GetRectPtr(), GetRectPtr());
		::IntersectRect(&rcClient, &rcItem, &rcClient);
	}
	if( !::IntersectRect(&rcTemp, &rcPaint, &rcItem)) 
		return;


	HDC hDestDC = pRenderDC->GetDC();
	ClipObj clip;
	ClipObj::GenerateClip(hDestDC, rcTemp, clip);
	CControlUI::Render(pRenderDC, rcPaint);

	if( ::IntersectRect(&rcTemp, &rcPaint, &rcClient)) 
	{
		ClipObj clip;
		ClipObj::GenerateClip(hDestDC, rcTemp, clip);

		RECTL rectLong = { m_pTextHost->rcClient.left, m_pTextHost->rcClient.top, m_pTextHost->rcClient.right, m_pTextHost->rcClient.bottom};
		m_pRenderDC = pRenderDC;

		RECT clipBox;
		DibObj* pDibObj = pRenderDC->GetDibObj();
		bool bNeedDoAlpha = pRenderDC->GetEnableAlpha() && pDibObj;
		if (bNeedDoAlpha)
		{
			_CalcClipBox(pRenderDC, m_pTextHost->rcClient, clipBox);
			CSSE::IncreaseAlpha(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(), clipBox);
		}

		GetTextHost()->GetTextServices()->TxDraw(DVASPECT_CONTENT,0,NULL,NULL,(HDC)hDestDC,NULL,&rectLong,NULL,&rcTemp,NULL,NULL,TXTVIEW_ACTIVE);

		if (bNeedDoAlpha)
		{
			CSSE::DecreaseAlpha(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(), clipBox);
		}


		m_pRenderDC = NULL;
		if (IsFocused())
		{
			GetTextHost()->DrawCaret(pRenderDC, rcPaint);
		}
	}

	UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, GetVScrollbar());

	if (m_bNeedReLayout)
	{
		m_bNeedReLayout = false;
		SetScrollPos(GetScrollPos() + 1);
	}

	m_bReLayoutOle = false;
}


void CRichEditUI::SetRect(RECT& rectRegion)
{
	//if (IsRectEmpty(&rectRegion)) //xqb如果不注释掉，刚开始弹出聊天框，排版会有问题，SetSel不准确
	//	return;

	if (GetWindow()->IsMinimized())
		return;

	if (m_bResizing) return;
	m_bResizing = true;

	CControlUI::SetRect(rectRegion);
	RECT rect = GetInterRect();

	CScrollbarUI* pScrollBar = GetVScrollbar();
	if (!pScrollBar)
	{
		m_pTextHost->SetRect(rect);
		m_bResizing = false;
		TEventUI event = { 0 };
		event.nType = UIEVENT_SIZE;
		SendEvent(event);

		return;
	}

	bool bVScrollBarVisiable = pScrollBar->IsVisible();
	if (IsRectEmpty(&m_pTextHost->rcClient))
	{
		LONG lWidth = rect.right -  rect.left;
		LONG lHeight = 0;
		SIZEL szExtent = { -1, -1 };
		GetTextHost()->GetTextServices()->TxGetNaturalSize(
			DVASPECT_CONTENT, 
			GetWindow()->GetPaintDC(), 
			NULL,
			NULL,
			TXTNS_FITTOCONTENT,
			&szExtent,
			&lWidth,
			&lHeight);

		long lRange = lHeight - (rect.bottom -  rect.top);
		if (lRange > 0)
			bVScrollBarVisiable = true;
		else
			bVScrollBarVisiable = false;
	}

	bool bScrollToEnd =pScrollBar->GetScrollPos() >= pScrollBar->GetScrollRange() - 2;


	RECT rcParent = GetRect();
	pScrollBar->OnSize(rcParent);
	if (bVScrollBarVisiable)
	{
		if (GetUIEngine()->IsRTLLayout())
		{
			rect.left += DPI_SCALE(pScrollBar->GetFixedSize().cx);

		}
		else
		{
			rect.right -= DPI_SCALE(pScrollBar->GetFixedSize().cx);
		}
	}


	//if (!IsRectEmpty(&rect)) //xqb如果不注释掉，刚开始弹出聊天框，排版会有问题，SetSel不准确
	{
		m_pTextHost->SetRect(rect);
		ResizeOle(rect);
		UpdateOlePosition();
	}

	if (bScrollToEnd)
		EndDown();

	TxSendMessage(WM_TIMER, RETID_BGND_RECALC, 0, NULL);
	m_bResizing = false;

	TEventUI event = { 0 };
	event.nType = UIEVENT_SIZE;
	SendEvent(event);
}


LONG CRichEditUI::CalcTextHeight(LONG& nWidth)
{
	if (!GetTextHost()) return 0;
	LONG nHeight = 0;
	SIZEL szExtent = { -1, -1 };
	GetTextHost()->GetTextServices()->TxGetNaturalSize(
		DVASPECT_CONTENT, 
		GetWindow()->GetPaintDC(), 
		NULL,
		NULL,
		TXTNS_FITTOCONTENT,
		&szExtent,
		&nWidth,
		&nHeight);
	if (GetUIEngine()->IsEnableHDPI())
	{
		/*HDC hdc = GetDC(NULL);
		int nHDPI = GetDeviceCaps(hdc, LOGPIXELSX);
		ReleaseDC(NULL, hdc);
		nHeight = MulDiv(nHeight, 96, nHDPI);*/
	}
	return nHeight;
}

void CRichEditUI::ResizeOle(RECT rect)
{
	if (!GetImageThumbnail()) return;

	int nObjCnt = GetIRichEditOle()->GetObjectCount();
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(reobject);
	for (int i=0; i < nObjCnt; i++)
	{
		GetIRichEditOle()->GetObject(i, &reobject, REO_GETOBJ_POLEOBJ);
		COleObject* pOleObj = (COleObject*)reobject.poleobj;
		pOleObj->OnRichEditSizeChanged(rect);
		pOleObj->Release();

	}
}

void CRichEditUI::UpdateOlePosition()
{
	POINT pt;
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(reobject);
	int nObjCnt = GetIRichEditOle()->GetObjectCount();
	for (int i=0; i < nObjCnt; i++)
	{
		GetIRichEditOle()->GetObject(i, &reobject, REO_GETOBJ_POLEOBJ);
		
		COleObject* pOleObj = dynamic_cast<COleControlObj*>(reobject.poleobj);
		if (!pOleObj)
		{
			reobject.poleobj->Release();
			continue;
		}

		GetCharPos(reobject.cp, pt);
		SIZE ole_size = pOleObj->GetSize();
		RECT ole_rect = {pt.x, pt.y, pt.x + ole_size.cx, pt.y + ole_size.cy};
		pOleObj->OnPositonChanged(&ole_rect);

		pOleObj->Release();
	}

}

bool CRichEditUI::ChildEvent(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_SCROLLWHEEL:
		{
			if (GetVScrollbar() && GetVScrollbar()->IsVisible())
			{
				// 假设滚动条滚动到最后，点击一张图视图会滚动，但滚动条不滚动，导致最后一点东西看不到，所以要修正一张
				if (GetVScrollbar()->GetScrollPos() >= GetVScrollbar()->GetScrollRange())
				{
					int nScroll = GET_WHEEL_DELTA_WPARAM(event.wParam);
					if (nScroll < 0)
					{
						//this->Resize();
						this->EndDown();
						break;
					}
				}
				GetVScrollbar()->SendEvent(event);
			}
		}
		break;
	}
	return true;

}

bool  CRichEditUI::ProcessNotify(TNotifyUI* pNotify)
{
	UI_EVENT_MENU_ID_HANDLER(UINOTIFY_CLICK, IDR_MENU_UIEDIT,  OnMenuClick)
	return false;
}

bool CRichEditUI::Event(TEventUI& event)
{
	UINT uMsg = 0;
	LRESULT lres=0;
	switch (event.nType)
	{
	case UIEVENT_TIMER:
	{
		int timerId = event.wParam;
		if (timerId == RETID_BGND_RECALC || timerId == RETID_AUTOSCROLL
			|| timerId == RETID_SMOOTHSCROLL || timerId == RETID_DRAGDROP
			|| timerId == RETID_MAGELLANTRACK)
		{
			GetTextHost()->TxSendMessage(WM_TIMER, timerId, 0, NULL);
			return true;
		}
	}
	break;
	case UIEVENT_KEYDOWN:
		{
			if ((GetKeyState(VK_CONTROL) & 0x80))
			{
				if (event.wParam == 'c' || event.wParam == 'C')
					Copy();
				else if (event.wParam == 'x' || event.wParam == 'X')
					Cut();
				else if (event.wParam == 'v' || event.wParam == 'V')
					Paste();
				else uMsg = WM_KEYDOWN;
			}
			else 
			{
				if (event.wParam == VK_UP)
				{
					if (!IsReadOnly() && IsFocused())
					{
						POINT pt;
						CCaretUI::GetInstance()->GetCaretPos(&pt);
						RECT clientRect = this->GetClientRect();
						if (pt.y <= clientRect.top)
						{
							TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
						}
					}
				}
				uMsg = WM_KEYDOWN;
			}
		}
		break;
	case UIEVENT_CHAR:
		uMsg=WM_CHAR;
		break;
	case UIEVENT_SETFOCUS:
		{
			HWND hWnd = GetWindow()->GetHWND();
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}

			GetTextHost()->GetTextServices()->OnTxUIActivate();
			((CTextHost*)GetTextHost())->LockScrollBar();
			TxSendMessage(WM_SETFOCUS, 0, 0, NULL);
			((CTextHost*)GetTextHost())->UnLockScrollBar();
			GetWindow()->EnableIME();
			this->SendNotify(UINOTIFY_SETFOCUS, event.wParam,event.lParam);
		}
		break;
	case UIEVENT_KILLFOCUS:
		{
			CCaretUI::GetInstance()->DestroyCaret(this->GetWindow());
			GetTextHost()->GetTextServices()->OnTxUIDeactivate();
			TxSendMessage(WM_KILLFOCUS, 0, 0, NULL);

			HWND hWnd = GetWindow()->GetHWND();
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}
			GetWindow()->DisableIME();
			this->SendNotify(UINOTIFY_KILLFOCUS, event.wParam,event.lParam);
		}
		
		break;
	case UIEVENT_MOUSEMOVE:
		{
			TxSendMessage(WM_MOUSEMOVE,event.wParam,event.lParam,&lres);
			COleObject* pOleObj = (COleObject*)FindObject(event.ptMouse);
			if (pOleObj != m_pOleObjHover)
			{
				if (m_pOleObjHover)
				{
					((COleObject*)m_pOleObjHover)->OnMouseLeave(event);
					event.nType = UIEVENT_MOUSELEAVE;
					OnOleEvent(event, m_pOleObjHover);
				}

				if (pOleObj)
				{
					pOleObj->OnMouseEnter(event);
					event.nType = UIEVENT_MOUSEENTER;
					OnOleEvent(event, pOleObj);
				}
				m_pOleObjHover = pOleObj;
			}
			if (m_pOleObjHover)
			{
				((COleObject*)m_pOleObjHover)->OnMouseMove(event);
				event.nType = UIEVENT_MOUSEMOVE;
				OnOleEvent(event, m_pOleObjHover);
			}
		}
		break;
	case  UIEVENT_MOUSELEAVE:
		{
			if (m_pOleObjHover)
			{
				((COleObject*)m_pOleObjHover)->OnMouseLeave(event);
				OnOleEvent(event, m_pOleObjHover);
			}
			m_pOleObjHover = NULL;
		}
		break;
	case UIEVENT_BUTTONDOWN:
		{
			HWND hWnd = GetWindow()->GetHWND();
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}

			TxSendMessage(WM_LBUTTONDOWN,event.wParam,event.lParam, NULL);
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			TxSendMessage(WM_LBUTTONUP, event.wParam,event.lParam, NULL);
			CLinkObj* pLinkObj = FindLink(event.ptMouse);
			if (pLinkObj)
			{
				CHARRANGE range;
				GetSel(range);
				if (range.cpMin == range.cpMax)	
				{
					if (pLinkObj->m_dwType == LINK_URL)
					{
						

						CHARFORMAT2 cfOld;
						GetSelectionCharFormat(cfOld);
						CHARFORMAT2 cfNew = cfOld;
						cfNew.dwMask =  CFM_COLOR;
						cfNew.crTextColor = COLOR_LINK_CLICK;
						HideSelection(true);
						SetSel(pLinkObj->m_cpMin, pLinkObj->m_cpMax);
						SetSelectionCharFormat(cfNew);
						SetSel(range.cpMin, range.cpMax);
						SetSelectionCharFormat(cfOld);
						HideSelection(false);

						tstring strLinkText = pLinkObj->m_strLinkTxt;
						if (strLinkText.empty())
						{
							range.cpMin = pLinkObj->m_cpMin;
							range.cpMax = pLinkObj->m_cpMax;
							GetText(range, strLinkText);
						}
						if ((int)::ShellExecute( NULL, _T("open"), strLinkText.c_str(), NULL, NULL, SW_SHOW) <= 32)
						{
							ShellExecute(NULL, _T("open"), _T("iexplore.exe"),  strLinkText.c_str(), NULL, SW_SHOW);
						}
					}
					else
					{
						CHARFORMAT2 cfOld;
						GetSelectionCharFormat(cfOld);
						CHARFORMAT2 cfNew = cfOld;
						cfNew.dwMask = CFM_COLOR;
						cfNew.crTextColor = COLOR_LINK_CLICK;
						HideSelection(true);
						SetSel(pLinkObj->m_cpMin, pLinkObj->m_cpMax);
						SetSelectionCharFormat(cfNew);
						SetSel(range.cpMin, range.cpMax);
						SetSelectionCharFormat(cfOld);
						HideSelection(false);

						this->SendNotify(UINOTIFY_RE_CLICK_LINK, (WPARAM)0, (LPARAM)pLinkObj);	
					}
				}
			}
			else
			{
				COleObject* pOleObj = (COleObject*)FindObject(event.ptMouse);
				if (pOleObj)
				{
					pOleObj->OnClick(event);
					OnOleEvent(event, pOleObj);
				}
			}
				
		}
		break;

	case UIEVENT_RBUTTONUP:
		{

			TxSendMessage(WM_RBUTTONUP, event.wParam,event.lParam,NULL);
			CRichEditUI* pParentRichedit = is_control_in_richeidt(this);
			if (pParentRichedit)
			{
				int charPos = get_control_charpos_in_richedit(pParentRichedit, this);
				if (charPos >= 0)
				{
					CHARRANGE cr;
					pParentRichedit->GetSel(cr);
					if (charPos >= cr.cpMin && charPos <= cr.cpMax)
					{
						event.pSender = pParentRichedit;
						pParentRichedit->Event(event);
						break;

					}
				}
			}

			if (!m_bEnableMenu) break;

			CMenuUI *pMenu = NULL;
			this->SendNotify(UINOTIFY_GET_CONTEXTMENU, (WPARAM)&pMenu, 0);
			if (pMenu) break;

			pMenu = CMenuUI::Load(IDR_MENU_UIEDIT);
			if (pMenu == NULL) break;
			CMenuUI* pSubMenu = NULL;
			if (IsReadOnly())
			{
				pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_PASTE, BY_COMMAND);
				if (pSubMenu) pMenu->Remove(pSubMenu);

				pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_CUT, BY_COMMAND);
				if (pSubMenu) pMenu->Remove(pSubMenu);

			}

			CHARRANGE range;
			GetSel(range);
			if (range.cpMin == range.cpMax)
			{
				pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_COPY, BY_COMMAND);
				if (pSubMenu) pSubMenu->EnableItem(false);

				pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_CUT, BY_COMMAND);
				if (pSubMenu) pSubMenu->EnableItem(false);
			}

			pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_PASTE, BY_COMMAND);
			if (pSubMenu) pSubMenu->EnableItem(CanPaste());

			pMenu->Show(GetWindow()->GetHWND(), NULL, this);

		}
		break;
	case UIEVENT_SCROLLWHEEL:
		{
			if (GetVScrollbar() && GetVScrollbar()->IsVisible())
			{
				// 假设滚动条滚动到最后，点击一张图视图会滚动，但滚动条不滚动，导致最后一点东西看不到，所以要修正一张
				if (GetVScrollbar()->GetScrollPos() >= GetVScrollbar()->GetScrollRange())
				{
					int nScroll = GET_WHEEL_DELTA_WPARAM(event.wParam);
					if (nScroll < 0)
					{
						//this->Resize();
						this->EndDown();
						break;
					}
				}
				GetVScrollbar()->SendEvent(event);
			}
		}
		break;

	case UIEVENT_LDBCLICK:
		{
			COleObject* pOleObj = (COleObject*)FindObject(event.ptMouse);
			if (pOleObj) 
				pOleObj->OnDbClick(event);
			else
				TxSendMessage(WM_LBUTTONDBLCLK,event.wParam,event.lParam, NULL);
		}
		return true;
	case UIEVENT_SETCURSOR:
		{
			if (FindLink(event.ptMouse))
				::SetCursor(::LoadCursor(NULL, IDC_HAND));
			else 
				m_pTextHost->DoSetCursor(NULL, &event.ptMouse);	
		}
		return true;

	case UIEVENT_VSCROLL:
		{
			if (GetVScrollbar())
			{
				int nPos = GetVScrollbar()->GetScrollPos();
				TxSendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nPos), 0, NULL);
				UpdateOlePosition();

			}
			
		}
		break;

	case UIEVENT_IME_START:
		{
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				POINT pt;
				GetCaretPos(&pt);
				COMPOSITIONFORM form;
				ImmGetCompositionWindow(hImc, &form);
				form.dwStyle = CFS_POINT;
				form.ptCurrentPos.x = pt.x;
				form.ptCurrentPos.y = pt.y;
				ImmSetCompositionWindow(hImc, &form);
				FontObj font;
				font.SetBold(m_bBold);
				font.SetFaceName(m_strFontName.c_str());
				font.SetFontSize(MulDiv(m_nFontSize, 96/*GetDeviceCaps(GetWindow()->GetPaintDC(), LOGPIXELSY)*/, 72));
				font.SetItalic(m_bItalic);
				LOGFONT lf;
				font.GetLogFont(&lf);
				::ImmSetCompositionFont(hImc, &lf);
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);

			}

		}
		break;
	case UIEVENT_IME_ING:
		{
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				if (event.lParam & GCS_RESULTSTR)
				{
					if (!IsReadOnly())
					{
						int dwSize = ImmGetCompositionString(hImc, GCS_RESULTSTR, NULL, 0);
						dwSize += sizeof(WCHAR);
						HGLOBAL hstr = GlobalAlloc(GHND,dwSize);
						if (hstr == NULL)
							return S_OK;

						LPVOID lpstr = GlobalLock(hstr);
						if (lpstr)
						{
							ImmGetCompositionString(hImc, GCS_RESULTSTR, lpstr, dwSize);
							if (dwSize !=0 )
								ReplaceSel((LPCTSTR)lpstr, TRUE);
						}

						GlobalUnlock(hstr);
						GlobalFree(hstr);
						GetWindow()->SetMsgHandled(true);
					}
				}//
				else if (event.lParam & GCS_CURSORPOS)
				{
					POINT pt;
					GetCaretPos(&pt);
					COMPOSITIONFORM form;
					ImmGetCompositionWindow(hImc, &form);
					form.dwStyle = CFS_POINT;
					form.ptCurrentPos.x = pt.x;
					form.ptCurrentPos.y = pt.y;
					ImmSetCompositionWindow(hImc, &form);
				
				}
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}

			
		}
		break;

	case UIEVENT_DRAG_DROP_GROUPS:
		{
			if (event.wParam == UIEVENT_DRAG_ENTER)
			{
				IDropTarget* pdt;
				GetDropTarget(&pdt);
				DragDropParams* pNotifyUI = (DragDropParams*)event.lParam;
				if (pdt && pNotifyUI)
				{
					pdt->DragEnter(pNotifyUI->pDataObject, pNotifyUI->grfKeyState, pNotifyUI->pt, pNotifyUI->pdwEffect);
				}
			}
			else if (event.wParam == UIEVENT_DRAG_OVER)
			{
				IDropTarget* pdt;
				GetDropTarget(&pdt);
				DragDropParams* pNotifyUI = (DragDropParams*)event.lParam;
				if (pdt && pNotifyUI)
				{
					pdt->DragOver(pNotifyUI->grfKeyState, pNotifyUI->pt, pNotifyUI->pdwEffect);
				}
			}
			else if (event.wParam == UIEVENT_DRAG_LEAVE)
			{
				IDropTarget* pdt;
				GetDropTarget(&pdt);
				DragDropParams* pNotifyUI = (DragDropParams*)event.lParam;
				if (pdt && pNotifyUI)
				{
					pdt->DragLeave();
				}
			}	
			else if (event.wParam == UIEVENT_DROP)
			{
				IDropTarget* pdt;
				GetDropTarget(&pdt);
				DragDropParams* pNotifyUI = (DragDropParams*)event.lParam;
				if (pNotifyUI)
				{
					IDataObject* pDataObject = pNotifyUI->pDataObject;
					if (pNotifyUI->bFiles && IsReadOnly())
					{
						STGMEDIUM stgMedium;
						FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
						HRESULT ret = pDataObject->GetData(&fmt, &stgMedium);
						if (ret == S_OK && stgMedium.hGlobal)
						{
							SendNotify(UINOTIFY_DROPFILES, (WPARAM)stgMedium.hGlobal, NULL);
							*(pNotifyUI->pdwEffect) = DROPEFFECT_COPY;
							//return TRUE;
						}
					}
				}
				if (pdt && pNotifyUI)
				{
					pdt->Drop(pNotifyUI->pDataObject, pNotifyUI->grfKeyState, pNotifyUI->pt, pNotifyUI->pdwEffect);
					if (!IsReadOnly())
						SetFocus();
				}
			}
		}
		return true;

	case UIEVENT_GESTURE:
	{
		HandleGestureEvent(event);
	}
	break;
	}
	if (uMsg > 0)
	{
		LRESULT lr=0;
		TxSendMessage(uMsg,event.wParam,event.lParam,&lr);
		int scrollPos = GetScrollPos();
		int i = 0;
		i++;
	}

	return CControlUI::Event(event);
}

void CRichEditUI::HandleGestureEvent(TEventUI& event)
{
	static POINT lastPt = { 0, 0 }; // 记录上次的位置
	GESTUREINFO gi = {};
	gi.cbSize = sizeof(GESTUREINFO);
	if (get_gesture_info((HGESTUREINFO)event.lParam, &gi))
	{
		// 默认只处理滑动消息
		if (gi.dwID == GID_PAN)
		{
			if (gi.dwFlags & GF_BEGIN)
			{
				lastPt = event.ptMouse;

			}
			else if (gi.dwFlags & GF_END)
			{
				lastPt.x = lastPt.y = 0;
			}
			else
			{
				int offsetX = event.ptMouse.x - lastPt.x;
				int offsetY = event.ptMouse.y - lastPt.y;
				lastPt = event.ptMouse;
				if (IsEnableVScrollBar())
				{
					CScrollbarUI* pVSCrollBar = GetVScrollbar();
					if (pVSCrollBar && pVSCrollBar->IsVisible())
					{
						int nCurPos = pVSCrollBar->GetScrollPos();
						pVSCrollBar->SetScrollPos(nCurPos - offsetY);
					}
				}
			}
		}
	}
}

void CRichEditUI::OnOleEvent(TEventUI&event, IOleObject* pOleObj)
{

}

void CRichEditUI::OnOleRelease(IOleObject* pOleObj)
{

}

void CRichEditUI::OnRequestResize(LPRECT lpRect)
{
	m_bResizing = false;

}

void CRichEditUI::OnDetectURLs(LPCTSTR lpszText, CStdPtrArray& arrLinksOut)
{
	DetectURLs(lpszText, arrLinksOut);
}

RECT CRichEditUI::GetScrollRect()
{
	RECT rect = {0, 0, 0, 0};
	return rect;
}

CTextHostBase* CRichEditUI::GetTextHost()
{
	return m_pTextHost;
}

IOleObject* CRichEditUI::FindObject(POINT& pt)
{
	if (!PtInRect(&m_pTextHost->rcClient, pt)) return NULL;
	int nObjCnt = GetIRichEditOle()->GetObjectCount();
	if (nObjCnt <= 0)
		return NULL;
	int nChar = CharFromPos(pt);
	if (nChar <= 0)
		return NULL;
	REOBJECT reObj;
	reObj.cbStruct = sizeof(REOBJECT);
	for (int i=(nObjCnt-1); i>=0; i--)
	{
		GetIRichEditOle()->GetObject(i, &reObj, REO_GETOBJ_POLEOBJ);
		COleObject* pOleObj = (COleObject*)reObj.poleobj;
		if ((reObj.cp == nChar || reObj.cp == nChar - 1) && PtInRect(&pOleObj->GetBound(), pt))
		{
			pOleObj->Release();
			return reObj.poleobj;
		}
		pOleObj->Release();
	}
	return NULL;

}

RECT  CRichEditUI::GetClientRect()
{
	return m_pTextHost->rcClient;
}

CScrollbarUI* CRichEditUI::GetVScrollbar()
{
	if (!m_pVerticalScrollbar && IsEnableVScrollBar())
	{
		m_pVerticalScrollbar = new CScrollbarUI;
		m_pVerticalScrollbar->SetOwner(this);
		m_pVerticalScrollbar->SetManager(GetWindow(), this);
		m_pVerticalScrollbar->SetHorizontal(false);
		m_pVerticalScrollbar->SetVisible(false);
		m_pVerticalScrollbar->SetScrollRange(0);
		m_pVerticalScrollbar->OnEvent += MakeDelegate(this, &CRichEditUI::ScrollBarEventDelegate);

	}
	return m_pVerticalScrollbar;
}


CControlUI* CRichEditUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{

	CControlUI* pControl = CControlUI::FindControl(Proc, pData, uFlags);
	if (!pControl)
		return NULL;

	CControlUI* pControlChild = UICustomControlHelper::FindChildControl(m_items, Proc, pData, uFlags);
	if (pControlChild) return pControlChild;

	CScrollbarUI* pVScrollBar = GetVScrollbar();
	if (pVScrollBar && pVScrollBar->IsVisible() && pVScrollBar->FindControl(Proc, pData, uFlags))
		return pVScrollBar;
	return pControl;
}


void CRichEditUI::SetTextColor( DWORD dwTextColor )
{
	m_txtColor = dwTextColor;
	m_pTextHost->SetTextColor(dwTextColor);
}

DWORD CRichEditUI::GetTextColor()
{
	return m_txtColor;
}

void CRichEditUI::SetFontName(LPCTSTR lpszFontName)
{
	m_strFontName = lpszFontName;
	m_pTextHost->SetFontName(lpszFontName);
}

LPCTSTR CRichEditUI::GetFontName()
{
	return m_strFontName.c_str();
}

void CRichEditUI::SetFontSize(LONG size)
{
	size = size < 0?-size:size;
	m_nFontSize = size;
	/*size = DPI_SCALE(size);*/
	m_pTextHost->SetFontSize(size * 20);
}

LONG CRichEditUI::GetFontSize()
{
	return m_nFontSize;
}

void CRichEditUI::SetBold(bool flag)
{
	m_bBold = flag;
	m_pTextHost->SetBold(flag);
}

void CRichEditUI::SetItalic(bool flag)
{
	m_bItalic = flag;
	m_pTextHost->SetItalic(flag);
}

void CRichEditUI::SetUnderline(bool flag)
{
	m_bUnderline = flag;
	m_pTextHost->SetUnderline(flag);
}

bool CRichEditUI::IsBold()
{
	return m_bBold;
}

bool CRichEditUI::IsItalic()
{
	return m_bItalic;
}

bool CRichEditUI::IsUnderline()
{
	return m_bUnderline;
}


void CRichEditUI::SetReadOnly(bool bReadOnly)
{
	m_bReadOnly = bReadOnly;
	m_pTextHost->SetReadOnly(bReadOnly);
    SetAutoURLDetect(IsReadOnly());
}

bool CRichEditUI::IsReadOnly()
{
	return m_bReadOnly;
}

////////////////////////////////////////////////////////////////////////////

int CRichEditUI::GetLineCount()
{
	LRESULT lResult;
	TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
	return (int)lResult; 
}

int CRichEditUI::GetFirstVisibleLine()
{
	LRESULT lResult;
	TxSendMessage(EM_GETFIRSTVISIBLELINE, 0, 0, &lResult);
	return (int)lResult; 
}

int CRichEditUI::LineIndex(int nLine)
{
	LRESULT lResult;
	TxSendMessage(EM_LINEINDEX, (WPARAM)nLine, 0, &lResult);
	return (int)lResult; 
}

void CRichEditUI::GetCharPos(long lChar, POINT& pt)
{
	TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, 0);
}

int CRichEditUI::CharFromPos(POINT& pt)
{
    LRESULT lResult;
	TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&pt, &lResult);
	return (int)lResult; 
}

long CRichEditUI::LineFromChar(long nIndex)
{
	LRESULT lResult;
	TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
	return (long)lResult;
}


void CRichEditUI::GetSel(CHARRANGE &cr)
{
	m_pTextHost->GetSel(cr);
}

void CRichEditUI::GetSel(long& nStartChar, long& nEndChar)
{
	CHARRANGE cr;
	GetSel(cr);
	nStartChar = cr.cpMin;
	nEndChar = cr.cpMax;
}

void CRichEditUI::SetSel(CHARRANGE &cr)
{
	m_pTextHost->SetSel(cr);
}

void CRichEditUI::SetSel(long nStartChar, long nEndChar)
{
	m_pTextHost->SetSel(nStartChar, nEndChar);
}

void CRichEditUI::SetSelAll()
{
	((CTextHost*)GetTextHost())->LockScrollBar();
	SetSel(0, -1);
	((CTextHost*)GetTextHost())->UnLockScrollBar();
	Invalidate();
}

void CRichEditUI::ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo)
{
	CHARRANGE range;
	GetSel(range);

	RemoveLinkObj(range);
	RemoveTextObj(range);
	
	int nOldTextLen = GetTextLength(GTL_DEFAULT);
	int nChangePos = range.cpMax;

	m_pTextHost->ReplaceSel(lpszNewText, bCanUndo);

	//URLDetect(range.cpMin)

	int nTextChangedLen = GetTextLength(GTL_DEFAULT) - nOldTextLen;
	OnCharPosChanged(nChangePos, nTextChangedLen);
	
}

void CRichEditUI::HideSelection(bool bHide)
{
	if (m_bHideSelection == bHide) return;
	m_bHideSelection = bHide;
	TxSendMessage(EM_HIDESELECTION, (WPARAM)bHide, (LPARAM)0, 0); 
}

bool CRichEditUI::IsHideSelection()
{
	return m_bHideSelection;
}

DWORD CRichEditUI::GetDefaultCharFormat(CHARFORMAT2 &cf)
{
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
	return (DWORD)lResult;
}


void  CRichEditUI::GetDefaultCharFormat(TEXTFORMAT &tf)
{
	CHARFORMAT2 cf;
	GetDefaultCharFormat(cf);
	memset(&tf, 0, sizeof(TEXTFORMAT));
	tf.crTextColor = GetTextColor();
	tf.dwFontSize = GetFontSize();
	_tcscpy(tf.szFaceName, GetFontName());
	tf.dwMask = cf.dwMask;
	tf.dwEffects = cf.dwEffects;
}

bool CRichEditUI::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
	return m_pTextHost->SetCharFormat(cf);
}

DWORD CRichEditUI::GetSelectionCharFormat(CHARFORMAT2 &cf)
{
	return m_pTextHost->GetSelectionCharFormat(cf);
}

bool CRichEditUI::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
	return m_pTextHost->SetSelectionCharFormat(cf);
}

DWORD CRichEditUI::GetParaFormat(PARAFORMAT2 &pf)
{
	pf.cbSize = sizeof(PARAFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
	return (DWORD)lResult;
}


bool CRichEditUI::SetParaFormat(PARAFORMAT2 &pf)
{
	return m_pTextHost->SetParaFormat(pf);
}
//////////////////////////////////////////////////

long CRichEditUI::GetTextLength(DWORD dwFlags)
{
	return m_pTextHost->GetTextLength(dwFlags);
}

void CRichEditUI::GetText(tstring& sText)
{
	m_pTextHost->GetText(sText);
}

void CRichEditUI::GetText(CHARRANGE &cr, tstring& sText)
{
	m_pTextHost->GetText(cr, sText);

}

void CRichEditUI::GetSelText(tstring& sText)
{
	m_pTextHost->GetSelText(sText);
}

LPCTSTR CRichEditUI::GetText()
{
	//this->GetText(m_strText);
	return m_strText.c_str();
}

void CRichEditUI::SetText(LPCTSTR lpcszText)
{
	RemoveAllLinkObj();
	RemoveAllTextObj();
	m_pTextHost->SetText(lpcszText);
	if (IsAutoURLDetect())
		this->URLDetect(0);
}

void CRichEditUI::URLDetect(int nStartPos, int nEndPos)	
{
	CHARRANGE cr;
	cr.cpMin = nStartPos;
	cr.cpMax = nEndPos;

	tstring strtmp;
	if (cr.cpMax >= 0)
		GetText(cr, strtmp);
	else
		GetText(strtmp);
	CStdPtrArray arrLinks;
	
	OnDetectURLs(strtmp.c_str(), arrLinks);

	CHARRANGE range;
	for (int i = 0; i < arrLinks.GetSize(); i++)
	{
		CLinkObj* pLinkObj = (CLinkObj*)arrLinks.GetAt(i);
		range.cpMin = pLinkObj->m_cpMin + nStartPos;
		range.cpMax = pLinkObj->m_cpMax + nStartPos;
		SetLink(range);
		delete pLinkObj;
	}
	arrLinks.Empty();
}

void CRichEditUI::InsertText(LPCTSTR lpcszText, int nOffsetPos, bool bReplaceSel, TEXTFORMAT* pcf)
{
	nOffsetPos = DPI_SCALE(nOffsetPos);
	bool bHideSelection = IsHideSelection();
	HideSelection(true);
	if( !bReplaceSel )
	SetSel(-1, -1);
	CHARRANGE range;
	GetSel(range);
	ReplaceSel(lpcszText, !m_bReadOnly);
	CHARRANGE range2;
	GetSel(range2);

	range.cpMax = range2.cpMax;
	SetSel(range);

	CHARFORMAT2 cfOld;
	if (pcf)
	{
		GetSelectionCharFormat(cfOld);
		CHARFORMAT2 cfNew = cfOld;
		FillCharFormat(*pcf, cfNew);
		/*cfNew.yHeight = DPI_SCALE(cfNew.yHeight);*/
		if (!FontObj::IsExistFontName(cfNew.szFaceName))
			_tcscpy(cfNew.szFaceName, GetUIRes()->GetDefaultFontName());
		SetSelectionCharFormat(cfNew);
	}
	

	PARAFORMAT2 pf; 
	memset(&pf, 0, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	if (nOffsetPos != 0)
	{
		pf.dwMask = PFM_STARTINDENT ;
		pf.dxStartIndent = nOffsetPos;
		SetParaFormat(pf);
	}


	if( !bReplaceSel )
	SetSel(-1,-1);
	else SetSel(range2.cpMax,range2.cpMax);

	//恢复默认字体
	if (pcf)
	{
		SetSelectionCharFormat(cfOld);
	}
	
	if (nOffsetPos != 0)
	{
		pf.dwMask = PFM_STARTINDENT;
		pf.dxStartIndent = 0;
		SetParaFormat(pf);
	}
	HideSelection(bHideSelection);

}

void CRichEditUI::InsertTextObj(ReTextObj* pTextObj, int nOffsetPos, bool bReplaceSel, TEXTFORMAT* pcf)
{
	nOffsetPos = DPI_SCALE(nOffsetPos);
	bool bHideSelection = IsHideSelection();
	HideSelection(true);
	if (!bReplaceSel)
		SetSel(-1, -1);
	CHARRANGE range;
	GetSel(range);

	ReplaceSel(pTextObj->GetText(), !m_bReadOnly);

	CHARRANGE range2;
	GetSel(range2);

	range.cpMax = range2.cpMax;
	SetSel(range);

	pTextObj->m_cpMin = range.cpMin;
	pTextObj->m_cpMax = range.cpMax;
	m_listTextObj.Add(pTextObj);

	CHARFORMAT2 cfOld;
	if (pcf)
	{
		GetSelectionCharFormat(cfOld);
		CHARFORMAT2 cfNew = cfOld;
		FillCharFormat(*pcf, cfNew);
		/*cfNew.yHeight = DPI_SCALE(cfNew.yHeight);*/
		if (!FontObj::IsExistFontName(cfNew.szFaceName))
			_tcscpy(cfNew.szFaceName, GetUIRes()->GetDefaultFontName());
		SetSelectionCharFormat(cfNew);
	}


	PARAFORMAT2 pf;
	memset(&pf, 0, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	if (nOffsetPos != 0)
	{
		pf.dwMask = PFM_STARTINDENT;
		pf.dxStartIndent = nOffsetPos;
		SetParaFormat(pf);
	}


	if (!bReplaceSel)
		SetSel(-1, -1);
	else SetSel(range2.cpMax, range2.cpMax);

	//恢复默认字体
	if (pcf)
	{
		SetSelectionCharFormat(cfOld);
	}

	if (nOffsetPos != 0)
	{
		pf.dwMask = PFM_STARTINDENT;
		pf.dxStartIndent = 0;
		SetParaFormat(pf);
	}
	HideSelection(bHideSelection);

}

void CRichEditUI::UpdateTextObj(ReTextObj* pTextObj, TEXTFORMAT* pcf)
{
	if (!FindTextObj(pTextObj))
		return;

	bool bHideSelection = IsHideSelection();
	HideSelection(true);
	CHARRANGE range;
	GetSel(range);
	CHARFORMAT2 cfOld;
	GetSelectionCharFormat(cfOld);
	SetSel(pTextObj->m_cpMin, pTextObj->m_cpMax);

	if (pcf)
	{
		CHARFORMAT2 cfNew = cfOld;
		FillCharFormat(*pcf, cfNew);
		/*cfNew.yHeight = DPI_SCALE(cfNew.yHeight);*/
		if (!FontObj::IsExistFontName(cfNew.szFaceName))
			_tcscpy(cfNew.szFaceName, GetUIRes()->GetDefaultFontName());
		SetSelectionCharFormat(cfNew);
	}
	
	pTextObj->m_bIsUpdating = true;
	ReplaceSel(pTextObj->GetText());
	pTextObj->m_bIsUpdating = false;

	CHARRANGE range2;
	GetSel(range2);
	pTextObj->m_cpMax = range2.cpMax;

	SetSel(range);

	if (pcf)
	{
		SetSelectionCharFormat(cfOld);
	}

	HideSelection(bHideSelection);
	
}

bool CRichEditUI::FindTextObj(ReTextObj* pTextObj)
{
	for (int i = 0; i < m_listTextObj.GetSize(); i++)
	{
		if (pTextObj == (ReTextObj*)m_listTextObj.GetAt(i))
		{
			return true;
		}
	}
	return false;
}

void CRichEditUI::InsertLink(LPCTSTR lpcszText, CLinkObj* pLinkObj, int nOffsetPos, bool bReplaceSel, TEXTFORMAT* pcf)
{
	nOffsetPos = DPI_SCALE(nOffsetPos);
	if (!pLinkObj)
		return;
	bool bHideSelection = IsHideSelection();
	HideSelection(true);
	if( !bReplaceSel )
	SetSel(-1, -1);
	CHARRANGE range;
	GetSel(range);

	ReplaceSel(lpcszText, !m_bReadOnly);
	CHARRANGE range2;
	GetSel(range2);

	range.cpMax = range2.cpMax;
	SetSel(range);

	pLinkObj->m_cpMin = range.cpMin;
	pLinkObj->m_cpMax = range.cpMax;


	m_listLinkObj.Add(pLinkObj);


	CHARFORMAT2 cfOld;
	GetSelectionCharFormat(cfOld);
	CHARFORMAT2 cfNew = cfOld;
	if (pcf)
	{
		FillCharFormat(*pcf, cfNew);
		/*cfNew.yHeight = DPI_SCALE(cfNew.yHeight);*/
		cfNew.dwMask |= CFM_UNDERLINE;
		cfNew.dwEffects |= CFE_UNDERLINE;
		if (!FontObj::IsExistFontName(cfNew.szFaceName))
			_tcscpy(cfNew.szFaceName, GetUIRes()->GetDefaultFontName());
		SetSelectionCharFormat(cfNew);
	}
	else
	{
		cfNew.dwMask |= CFM_UNDERLINE | CFM_COLOR;
		cfNew.dwEffects |= CFE_UNDERLINE;
		cfNew.crTextColor = COLOR_LINK;
		SetSelectionCharFormat(cfNew);
	}


	PARAFORMAT2 pf; 
	if (nOffsetPos != 0)
	{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_STARTINDENT;
		pf.dxStartIndent = nOffsetPos;
		SetParaFormat(pf);
	}

	if( !bReplaceSel ) 
	SetSel(-1,-1);
	else SetSel(range2.cpMax,range2.cpMax);

	//恢复默认字体
	
	SetSelectionCharFormat(cfOld);
	

	if (nOffsetPos != 0)
	{
		pf.dwMask = PFM_STARTINDENT;
		pf.dxStartIndent = 0;
		SetParaFormat(pf);
	}
	HideSelection(bHideSelection);

}


void CRichEditUI::SetLink(CHARRANGE &cr, TEXTFORMAT* pcf)
{
	CHARRANGE range;
	GetSel(range);
	bool bHideSelection = IsHideSelection();
	HideSelection(true);
	SetSel(cr.cpMin, cr.cpMax + 1);

	CLinkObj* pLinkObj = new CLinkObj;
	pLinkObj->m_cpMin = cr.cpMin;
	pLinkObj->m_cpMax = cr.cpMax + 1;
	
	m_listLinkObj.Add(pLinkObj);
	

	CHARFORMAT2 cfOld;
	GetSelectionCharFormat(cfOld);
	CHARFORMAT2 cfNew = cfOld;
	if (pcf)
	{
		FillCharFormat(*pcf, cfNew);
		/*cfNew.yHeight = DPI_SCALE(cfNew.yHeight);*/
		cfNew.dwMask |= CFM_UNDERLINE;
		cfNew.dwEffects |= CFE_UNDERLINE;
		SetSelectionCharFormat(cfNew);
	}
	else
	{
		cfNew.dwMask |= CFM_UNDERLINE | CFM_COLOR;
		cfNew.dwEffects |= CFE_UNDERLINE;
		cfNew.crTextColor = COLOR_LINK;
		SetSelectionCharFormat(cfNew);
	}

	SetSel(range);
	SetSelectionCharFormat(cfOld);
	HideSelection(bHideSelection);

}

void CRichEditUI::CancelLink(CLinkObj* pLinkObj, TEXTFORMAT* pcf)
{
	if (pLinkObj == NULL)
		return;

	bool bHideSelection = IsHideSelection();
	HideSelection(true);
	CHARRANGE range;
	GetSel(range);
	CHARFORMAT2 cfOld;
	GetSelectionCharFormat(cfOld);

	SetSel(pLinkObj->m_cpMin, pLinkObj->m_cpMax);
	CHARFORMAT2 cfNew = cfOld;
	if (pcf)
	{
		FillCharFormat(*pcf, cfNew);
		/*cfNew.yHeight = DPI_SCALE(cfNew.yHeight);*/
		cfNew.dwMask |= CFM_UNDERLINE;
		cfNew.dwEffects &= ~CFE_UNDERLINE;
		SetSelectionCharFormat(cfNew);
	}
	else
	{
		cfNew.dwMask = CFM_UNDERLINE;
		cfNew.dwEffects &= ~CFE_UNDERLINE;
		SetSelectionCharFormat(cfNew);
	}


	SetSel(range);
	SetSelectionCharFormat(cfOld);

	for (int i = 0; i < m_listLinkObj.GetSize(); i++)
	{
		if (pLinkObj == m_listLinkObj.GetAt(i))
		{
			delete pLinkObj;
			m_listLinkObj.Remove(i);
			break;
		}
	}
	HideSelection(bHideSelection);

}


void CRichEditUI::Clear()
{
	TxSendMessage(WM_SETTEXT, 0, LPARAM(_T("")), 0);
	RemoveAllLinkObj();
	RemoveAllTextObj();
}

void CRichEditUI::RemoveLinkObj(CHARRANGE& range)
{
	if (range.cpMin == range.cpMax)
		return;

	vector<int> vecTmp;
	
	for (int i = 0; i < m_listLinkObj.GetSize(); i++)
	{
		CLinkObj* pLinkObj = (CLinkObj*)m_listLinkObj[i];
		if (pLinkObj != NULL)
		{
			if ((pLinkObj->m_cpMin >= range.cpMin && pLinkObj->m_cpMin < range.cpMax) ||
				(pLinkObj->m_cpMax >= range.cpMin && pLinkObj->m_cpMax < range.cpMax))
			{
				vecTmp.push_back(i);
				delete pLinkObj;
			}

		}

	}

	for (int i = vecTmp.size() - 1; i >= 0; i--)
	{
		m_listLinkObj.Remove(vecTmp[i]);

	}
}

void CRichEditUI::RemoveAllLinkObj()
{
	for (int i = 0; i < m_listLinkObj.GetSize(); i++)
	{
		CLinkObj* pLinkObj = (CLinkObj*)m_listLinkObj[i];
		if (pLinkObj != NULL)
		{
			delete pLinkObj;
		}
	}
	m_listLinkObj.Empty();
}

void CRichEditUI::RemoveTextObj(CHARRANGE& range)
{
	if (range.cpMin == range.cpMax)
		return;

	vector<int> vecTmp;
	LONG cpMin = 0, cpMax = 0;
	for (int i = 0; i < m_listTextObj.GetSize(); i++)
	{
		ReTextObj* pTextObj = (ReTextObj*)m_listTextObj[i];
		if (pTextObj != NULL && !pTextObj->m_bIsUpdating)
		{
			pTextObj->GetRange(cpMin, cpMax);
				if ((cpMin >= range.cpMin && cpMin < range.cpMax) ||
					(cpMax >= range.cpMin && cpMax < range.cpMax))
			{
				vecTmp.push_back(i);
				delete pTextObj;
			}

		}

	}

	for (int i = vecTmp.size() - 1; i >= 0; i--)
	{
		m_listTextObj.Remove(vecTmp[i]);

	}
}

void CRichEditUI::RemoveAllTextObj()
{
	for (int i = 0; i < m_listTextObj.GetSize(); i++)
	{
		ReTextObj* pTextObj = (ReTextObj*)m_listTextObj[i];
		if (pTextObj != NULL)
		{
			delete pTextObj;
		}
	}
	m_listTextObj.Empty();
}
//插入图片

void CRichEditUI::InsertImage(COleImageObj* pImgObj, long nCharPos)
{
	if (!pImgObj || !pImgObj->GetImage())
		return;

	pImgObj->GetImage()->SetRightOrientation();
	// 加上这句可以使插入图片是滚动条滚动
	bool bHideSelection = IsHideSelection();
	HideSelection(true);
	SIZEL size;
	int nImageWidth = pImgObj->GetImage()->GetWidth() + 2;
	int nImageHeight = pImgObj->GetImage()->GetHeight() + 2;

	// 显示缩略图计算
	RECT rect = GetClientRect();
	if ((GetImageThumbnail() || m_nMaxImageWidth != 0 || m_nMaxImageHeight != 0) &&  pImgObj->GetFrameCount() <= 1 && !IsRectEmpty(&rect))
	{
		int nThumbnailWidth = rect.right - rect.left - GetStartIndent() - OLEIMAGE_RIGHT_MARGIN;
		int nThumbnailHeight = rect.bottom - rect.top - OLEIMAGE_TOP_MARGIN;

		if (m_nMaxImageWidth != 0)
			nThumbnailWidth = DPI_SCALE(m_nMaxImageWidth);
		if (m_nMaxImageHeight != 0)
			nThumbnailHeight = DPI_SCALE(m_nMaxImageHeight);

		if (nImageWidth / (double)nImageHeight > nThumbnailWidth / (double)nThumbnailHeight)
		{
			if (/*DPI_SCALE*/(nImageWidth) > nThumbnailWidth)
			{
				int nNewWidth = DPI_SCALE_BACK(nThumbnailWidth);
				if (nNewWidth < 1) nNewWidth = 1;
				nImageHeight = nImageHeight * nNewWidth / nImageWidth;
				nImageWidth = nNewWidth;
			}

		}
		else
		{
			if (/*DPI_SCALE*/(nImageHeight) > nThumbnailHeight)
			{
				int nNewHeight = DPI_SCALE_BACK(nThumbnailHeight);
				if (nNewHeight < 1) nNewHeight = 1;
				nImageWidth = nImageWidth * nNewHeight / nImageHeight;
				nImageHeight = nNewHeight;
			}
		}

	}

	size.cx = DXtoHimetricX(nImageWidth);
	size.cy = DYtoHimetricY(nImageHeight);
	//DPI_SCALE(&size);
	pImgObj->SetExtent(DVASPECT_CONTENT, &size);

	InsertObject(pImgObj, nCharPos);

	HideSelection(bHideSelection);
	Invalidate();
}

COleObject* CRichEditUI::InsertControl(CControlUI* pControl, int nWidth, int nHeight, long nCharPos, DWORD dwFlags)
{
	if (!pControl) return NULL;
	// 加上这句可以使插入图片是滚动条滚动

	COleControlObj* pControlObject = new COleControlObj(pControl);
	InsertControl(pControlObject, nWidth, nHeight, nCharPos, dwFlags);
	return pControlObject;
}

void CRichEditUI::InsertControl(COleControlObj* pControlObject, int nWidth, int nHeight, long nCharPos, DWORD dwFlags)
{
	CControlUI* pControl = pControlObject->GetControl();
	// 加上这句可以使插入图片是滚动条滚动
	bool bHideSelection = IsHideSelection();
	HideSelection(true);

	SIZEL size;
	if (nWidth <= 0)
	{
		pControlObject->SetAutoSize(true);
		nWidth = DPI_SCALE_BACK(GetTextHost()->rcClient.right - GetTextHost()->rcClient.left);
	}

	size.cx = DXtoHimetricX(nWidth);
	size.cy = DYtoHimetricY(nHeight);
	DPI_SCALE(&size);
	pControlObject->SetExtent(DVASPECT_CONTENT, &size);

	pControl->OnEvent += MakeDelegate(this, &CRichEditUI::ChildEvent);
	pControl->EventToParent(true);
	pControl->EnableRTLLayout(false);
	//pControl->SetINotifyUI(this);
	UICustomControlHelper::AddChildControl(pControl, this, m_items);
	InsertObject(pControlObject, nCharPos, dwFlags);


	HideSelection(bHideSelection);
	Invalidate();

}

void CRichEditUI::RemoveControl(CControlUI* pControl)
{
	int nCharPos = GetControlCharPos(pControl);
	if (nCharPos < 0) return;
	SetSel(nCharPos, nCharPos + 1);
	ReplaceSel(_T(""));
	Invalidate();
}

int	 CRichEditUI::GetControlCharPos(CControlUI* pControl)
{
	COleObject* pOleObject = GetOleObjectByControl(pControl);
	if (!pOleObject) return -1;
	return pOleObject->GetCharPos();

}

COleObject* CRichEditUI::GetOleObjectByControl(CControlUI* pControl)
{
	LONG nObjCnt = GetIRichEditOle()->GetObjectCount();
	REOBJECT struREObj;
	struREObj.cbStruct = sizeof(struREObj);
	for(int i=0; i<nObjCnt; i++)
	{
		GetIRichEditOle()->GetObject(i, &struREObj, REO_GETOBJ_POLEOBJ);
		COleControlObj* pOleObj = dynamic_cast<COleControlObj*>(struREObj.poleobj);
		if (pOleObj && pOleObj->GetControl() == pControl)
		{
			pOleObj->Release();
			return pOleObj;
		}
		struREObj.poleobj->Release();
	}
	return NULL;
}


CControlUI* CRichEditUI::GetItem(LPCTSTR lpszId)
{
	tstring strID = lpszId;
	int i = strID.find_first_of(_T('.'));
	if (i != tstring::npos)
	{
		CControlUI* pControl = GetItem(strID.substr(0, i).c_str());
		if (pControl)
		{
			strID = strID.substr(i + 1);
			return pControl->GetItem(strID.c_str());
		}
	}
	CControlUI* pControl = NULL;
	int nCnt = m_items.GetSize();
	for( int it = 0;  it < nCnt; it++ )
	{
		pControl = static_cast<CControlUI*>(m_items[it]);
		if (equal_icmp(pControl->GetId(), lpszId))
			return pControl;
	}
	return NULL;	
}

CControlUI* CRichEditUI::GetItem(int iIndex)
{
	if (iIndex < 0 || iIndex >= GetCount())
		return NULL;
	return static_cast<CControlUI*>(m_items[iIndex]);
}

int  CRichEditUI::GetCount()
{
	return m_items.GetSize();
}


bool CRichEditUI::Remove(CControlUI* pControlChild)
{
	return UICustomControlHelper::RemoveChildControl(pControlChild, m_items);

}

void CRichEditUI::InsertObject(COleObject* pOleObject, long nCharPos, DWORD dwFlags)
{
	if (!pOleObject) return;

	LPOLECLIENTSITE lpClientSite;
	GetIRichEditOle()->GetClientSite( &lpClientSite );

	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(REOBJECT);

	reobject.cp = nCharPos;
	pOleObject->SetOwner(this);
	pOleObject->SetClientSite(lpClientSite);
	reobject.dvaspect = DVASPECT_CONTENT;
	reobject.poleobj = pOleObject;
	reobject.polesite = lpClientSite;
	reobject.pstg = NULL;
	pOleObject->SetId(m_dwObjectID);
	reobject.dwUser = m_dwObjectID;
	reobject.dwFlags = dwFlags;
	m_dwObjectID ++;

	int nCharPosTmp = nCharPos;
	if (REO_CP_SELECTION == nCharPosTmp)
	{
		CHARRANGE range;
		GetSel(range);
		nCharPosTmp = range.cpMax;
	}

	// 插入之后告别链接位置
	int nChangePos = nCharPos;
	if (REO_CP_SELECTION == nChangePos)
	{
		CHARRANGE range;
		GetSel(range);
		nChangePos = range.cpMax;
	}
	int nOldTextLen = GetTextLength(GTL_DEFAULT);


	GetIRichEditOle()->InsertObject(&reobject);

	// 改变链接位置
	int nTextChangedLen = GetTextLength(GTL_DEFAULT) - nOldTextLen;
	OnCharPosChanged(nChangePos, nTextChangedLen);
	pOleObject->Release();
	lpClientSite->Release();

	m_bNeedUpdateOlePositon = true;
}


void CRichEditUI::ResizeObject(COleObject* pOleObject, int nWidth, int nHeight)
{
	if (!pOleObject)
		return;

	pOleObject->SetSize(nWidth, nHeight);
	pOleObject->FireViewChange();
}

void CRichEditUI::EndDown()
{
	TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
}

void CRichEditUI::SetScrollPos(int nPos)
{
	if (nPos < 0) nPos = 0;
	CScrollbarUI* pVScrollBar = GetVScrollbar();
	if (!pVScrollBar)
	{
		TxSendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nPos), 0, NULL);
		return;
	}
	
	pVScrollBar->SetScrollPos(nPos, false);
	TxSendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nPos), 0, NULL);
}

int CRichEditUI::GetScrollPos()
{
	CScrollbarUI* pVScrollBar = GetVScrollbar();
	if (!pVScrollBar)
		return 0;
	if (!pVScrollBar || !pVScrollBar->IsVisible()) return 0;
	return pVScrollBar->GetScrollPos();

}

int CRichEditUI::GetScrollRange()
{
	if (!GetTextHost() || !GetTextHost()->GetTextServices()) return 0;
	LONG lWidth = GetTextHost()->rcClient.right -  GetTextHost()->rcClient.left;
	LONG lHeight = 0;
	SIZEL szExtent = { -1, -1 };
	GetTextHost()->GetTextServices()->TxGetNaturalSize(
		DVASPECT_CONTENT, 
		GetWindow()->GetPaintDC(), 
		NULL,
		NULL,
		TXTNS_FITTOCONTENT,
		&szExtent,
		&lWidth,
		&lHeight);

	long lRange = lHeight - (GetTextHost()->rcClient.bottom -  GetTextHost()->rcClient.top);
	if (lRange < 0) lRange = 0;
	return lRange;
}

void CRichEditUI::EnableVScrollBar(bool bEnable)
{
	m_bEnableVScrollbar = bEnable;
	if (!bEnable && m_pVerticalScrollbar)
	{
		delete m_pVerticalScrollbar;
		m_pVerticalScrollbar = NULL;
	}

	if (GetTextHost())
	{
		GetTextHost()->fVerticalScrollBar = bEnable;
		if (GetTextHost()->GetTextServices()) 
			GetTextHost()->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_SCROLLBARCHANGE, TXTBIT_SCROLLBARCHANGE);
	}
}

bool CRichEditUI::IsEnableVScrollBar()
{
	return m_bEnableVScrollbar;
}

void CRichEditUI::EnableCaret(bool bEnable)
{
	m_bEnableCaret = bEnable;
	if (GetTextHost())
		GetTextHost()->fShowCaret = bEnable;
}

bool CRichEditUI::IsEnableCaret()
{
	return m_bEnableCaret;
}

bool CRichEditUI::Redo()
{ 
	LRESULT lResult;
	TxSendMessage(EM_REDO, 0, 0, &lResult);
	return (BOOL)lResult == TRUE; 
}

bool CRichEditUI::Undo()
{
	LRESULT lResult;
	TxSendMessage(EM_UNDO, 0, 0, &lResult);
	return (BOOL)lResult == TRUE; 
}


void CRichEditUI::Copy()
{ 
	if (OpenClipboard(HWND_DESKTOP))
	{
		::EmptyClipboard();
		::CloseClipboard();
	}
	CHARRANGE rg;
	GetSel(rg);
	if(rg.cpMin  == rg.cpMax) 
		return;
	
	IDataObject* pDataObject = NULL;
	if (OnCopy(&pDataObject)) 
	{	
		OleSetClipboard (pDataObject);
		OleFlushClipboard ();
	}
	pDataObject->Release();   
}

static tstring GetCopyText(CControlUI* pControl)
{
	if (!pControl)
		return _T("");
	if (pControl->GetCount() == 0)
	{
		CRichEditUI* pRichEdit = dynamic_cast<CRichEditUI*>(pControl);
		tstring strText;
		if (pRichEdit)
		{
			pRichEdit->GetText(strText);
		}
		else
		{
			strText = pControl->GetText();
		}
		if (!strText.empty())
			strText += _T("\r");
		return strText;
	}

	tstring strAllText;
	for (int i = 0; i < pControl->GetCount(); i++)
	{
		strAllText += GetCopyText(pControl->GetItem(i));
	}
	return strAllText;
}

bool CRichEditUI::OnCopy(IDataObject** ppDataObject)
{
	CDataObject* pDataObject = new CDataObject;
	*ppDataObject = pDataObject;
	CHARRANGE rg;
	GetSel(rg);
	if(rg.cpMin  == rg.cpMax) return false;

	FORMATETC fmtetc = {CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};  
	STGMEDIUM stgmed = {TYMED_HGLOBAL, {0}, 0};  
	
	// 获取图片的个数
	CStdPtrArray allItems;
	int imgCount = 0;
	int ctlCount = 0;
	LONG nObjCnt = GetIRichEditOle()->GetObjectCount();
	REOBJECT struREObj;
	struREObj.cbStruct = sizeof(struREObj);
	for(int i=0; i<nObjCnt; i++)
	{
		GetIRichEditOle()->GetObject(i, &struREObj, REO_GETOBJ_POLEOBJ);
		if(struREObj.cp>=rg.cpMin && struREObj.cp<rg.cpMax)
		{
			COleImageObj* pImgObj = dynamic_cast<COleImageObj*>(struREObj.poleobj);
			if (pImgObj)
			{
				imgCount++;
				allItems.Add(struREObj.poleobj);
			}
			else
			{
				COleControlObj* pCtlOle = dynamic_cast<COleControlObj*>(struREObj.poleobj);
				int length = GetCopyText(pCtlOle->GetControl()).length();
				if (pCtlOle && length > 0)
				{
					ctlCount++;
					allItems.Add(struREObj.poleobj);
				}
			}
			
			
		}
		if (struREObj.poleobj)
			struREObj.poleobj->Release();
	}

	// 复制图片（只选择一个图片的情况）
	if ((rg.cpMin  == rg.cpMax - 1))
	{
		if (imgCount == 1)
		{
			COleImageObj* pImgObj = (COleImageObj*)allItems[0];

			// 复制图片到剪贴板
			ImageObj* imgobj = pImgObj->GetImage();
			if (imgobj)
			{
				int cx = imgobj->GetWidth();
				int cy = imgobj->GetHeight();
				HDC hDC = ::GetDC(NULL);
				HDC hMemDC = CreateCompatibleDC(hDC);
				HBITMAP hBitmapNew = ::CreateCompatibleBitmap(hDC, cx, cy);
				HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hMemDC, hBitmapNew);
				RECT rect = { 0, 0, cx, cy };
				HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 255));
				FillRect(hMemDC, &rect, hbr);
				::DeleteObject(hbr);

				IRenderDC renderDC;
				renderDC.SetDevice(hMemDC);
				renderDC.DrawImage(imgobj, 0, 0, pImgObj->m_nActiveFrame);
				pDataObject->SetHBitmapData(hBitmapNew);
				::SelectObject(hMemDC, hBitmapOld);
				::DeleteObject(hBitmapNew);
				::DeleteDC(hMemDC);
				::ReleaseDC(NULL, hDC);

			}
			
		}
		else if (ctlCount == 1)
		{
			COleControlObj* pCtlOle = (COleControlObj*)allItems[0];
			pDataObject->SetTextData(GetCopyText(pCtlOle->GetControl()).c_str());
			return true;
		}
	
	}


	//  复制文字
	WCHAR* pwszText = new WCHAR[rg.cpMax - rg.cpMin + 2];
	memset(pwszText, 0, (rg.cpMax - rg.cpMin + 2) * 2);
	TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)pwszText, 0);

	tstring wstrText = pwszText;
	delete []pwszText;

	if (ctlCount > 0)
	{
		tstring wstrNewText;
		int nStartPos = 0, nEndPos = 0;
		for (int i = 0; i < allItems.GetSize(); i++)
		{
			COleObject* pOleObj = (COleObject*)allItems.GetAt(i);
			COleControlObj* pCtlOle = dynamic_cast<COleControlObj*>(pOleObj);
			if (pCtlOle)
			{
				nEndPos = pCtlOle->GetCharPos() - rg.cpMin;
				tstring subText = GetCopyText(pCtlOle->GetControl());
				wstrNewText += wstrText.substr(nStartPos, nEndPos - nStartPos) + subText;
				nStartPos = nEndPos + 1;
			}
			
		}
		if (nStartPos < wstrText.length())
			wstrNewText += wstrText.substr(nStartPos);
		wstrText = wstrNewText;
	}
	

	StrUtil::Replace(wstrText, _T("\r"), _T("\r\n"));
	pDataObject->SetTextData(wstrText.c_str());

	// 复制文字和图片（自定义格式和html格式）
	if (imgCount > 0)
	{
		//存储字符数据
		int nOffsetBits = sizeof(ClipboardDataHeader) + imgCount * sizeof(ClipboardImgData);
		int nMsgSize = (rg.cpMax - rg.cpMin + 1) * 2;

		int nBytes = nOffsetBits + nMsgSize;
		LPBYTE  lpByte = new BYTE[nBytes];
		LPClipboardData lpClipData = (LPClipboardData)lpByte; 
		lpClipData->cdHeader.cdhImgCount = imgCount;
		lpClipData->cdHeader.cdhOffset = nOffsetBits;
		//复制文字
		LPBYTE lpData = (LPBYTE)lpClipData + lpClipData->cdHeader.cdhOffset; 
		TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpData, 0);

		//strHTML = _T("<DIV>");
		//复制图片
		int nStartPos = 0, nEndPos = 0;
		if (ctlCount == 0)
		{
			// 设置自定义
			
			for (int i = 0; i < allItems.GetSize(); i++)
			{
				COleObject* pOleObj = (COleObject*)allItems.GetAt(i);
				nEndPos =  pOleObj->GetCharPos() - rg.cpMin;

				COleImageObj* pImgObj = dynamic_cast<COleImageObj*>(pOleObj);
				if (pImgObj)
				{
					//
					lpClipData->cdImage[i].cidCharPos =  nEndPos;
					tstring strPath =  pImgObj->GetFilePath();
					if (strPath.substr(0, 1) == _T("#"))
						strPath = GetUIRes()->GetImageFileFullPath(strPath.c_str());
					_tcscpy(lpClipData->cdImage[i].cidPath, strPath.c_str());
					_tcscpy(lpClipData->cdImage[i].cidKey, pImgObj->GetKey());
					lpClipData->cdImage[i].cidImgType = pImgObj->GetImageType();
					lpClipData->cdImage[i].cdiCache = pImgObj->IsCache();
				}
			
				nStartPos = nEndPos + 1;
			}
			pDataObject->SetUserData(m_clip_board_format, lpByte, nBytes);
		}


		tstring strHTML, strHTMLTemp;
		strHTMLTemp = (LPCTSTR)(LPBYTE)lpData;
		// 设置html
		nStartPos = 0, nEndPos = 0;
		for (int i = 0; i < allItems.GetSize(); i++)
		{
			COleObject* pOleObj = (COleObject*)allItems.GetAt(i);
			nEndPos =  pOleObj->GetCharPos() - rg.cpMin;

			COleImageObj* pImgObj = dynamic_cast<COleImageObj*>(pOleObj);
			if (pImgObj)
			{
		
				tstring strPath =  pImgObj->GetFilePath();
				if (strPath.substr(0, 1) == _T("#"))
					strPath = GetUIRes()->GetImageFileFullPath(strPath.c_str());
				strHTML += strHTMLTemp.substr(nStartPos, nEndPos - nStartPos) + _T("<IMG src=\"file:///") + strPath +  _T("\" >");	

			}
			else
			{
				COleControlObj* pCtlOle = dynamic_cast<COleControlObj*>(pOleObj);
				strHTML += strHTMLTemp.substr(nStartPos, nEndPos - nStartPos) + GetCopyText(pCtlOle->GetControl());	

			}

			nStartPos = nEndPos + 1;
			
		}

		if (nStartPos < (int)strHTMLTemp.length())
			strHTML += strHTMLTemp.substr(nStartPos, strHTMLTemp.length()  - nStartPos);

		StrUtil::Replace(strHTML, _T("\r"), _T("<br>"));
		if (!strHTML.empty())
		{
			pDataObject->SetHTMLData(strHTML.c_str());
		}
	

		delete[] lpByte;
	}
	return true;

}

void CRichEditUI::Cut()
{ 
	if (IsReadOnly())
		return;
	CHARRANGE rg;
	GetSel(rg);
	if(rg.cpMin  == rg.cpMax)
		return;
	Copy();
	ReplaceSel(_T(""), true);
}


void CRichEditUI::Paste()
{ 
	if (IsReadOnly())
		return;
	IDataObject* pDataObject = NULL;
	OleGetClipboard(&pDataObject);
	if (!pDataObject) return;
	OnPaste(pDataObject);
	pDataObject->Release();
}

void CRichEditUI::Paste(IDataObject* pDataObject)
{
	OnPaste(pDataObject);
}

bool CRichEditUI::CanPaste()
{
	if (IsReadOnly())
		return false;

	IDataObject* pDataObject = NULL;
	OleGetClipboard(&pDataObject);
	if (!pDataObject) return false;

	STGMEDIUM stgMedium;
	FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	HRESULT ret = S_OK;

	fmt.cfFormat = m_clip_board_format;
	ret = pDataObject->GetData(&fmt, &stgMedium);
	if (m_bEnabeImage && ret == S_OK && stgMedium.hGlobal)
	{
		pDataObject->Release();
		return true;
	}

	fmt.cfFormat = RegisterClipboardFormat(_T("HTML Format"));
	ret = pDataObject->GetData(&fmt, &stgMedium);
	if (m_bEnabeImage && ret == S_OK && stgMedium.hGlobal)
	{
		pDataObject->Release();
		return true;
	}

	fmt.cfFormat = CF_BITMAP;
	fmt.tymed = TYMED_GDI;
	ret = pDataObject->GetData(&fmt, &stgMedium);
	if (m_bEnabeImage && ret == S_OK && stgMedium.hBitmap)
	{
		pDataObject->Release();
		return true;
	}

	fmt.cfFormat = CF_UNICODETEXT;
	fmt.tymed = TYMED_HGLOBAL;
	ret = pDataObject->GetData(&fmt, &stgMedium);
	if (ret == S_OK && stgMedium.hBitmap)
	{
		pDataObject->Release();
		return true;
	}

	fmt.cfFormat = CF_HDROP;
	fmt.tymed = TYMED_HGLOBAL;
	ret = pDataObject->GetData(&fmt, &stgMedium);
	if (ret == S_OK && stgMedium.hGlobal)
	{
		pDataObject->Release();
		return true;
	}
	return false;
}



void CRichEditUI::OnPaste(IDataObject* pDataObject)
{
	if (!pDataObject) return;

	
	if (IsReadOnly()) return;

	HideSelection(true);
	CHARRANGE rg;
	GetSel(rg);
	int nStartPos = rg.cpMax;

	bool bPaste = false;
	HGLOBAL hMem = NULL;

	STGMEDIUM stgMedium;
	FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	fmt.cfFormat = 0;


	if (!bPaste && m_bEnabeImage)
	{
		fmt.cfFormat = m_clip_board_format;
		HRESULT ret = pDataObject->GetData(&fmt, &stgMedium);
		hMem = stgMedium.hGlobal;
		if (ret == S_OK && hMem)
		{
			// 粘贴文字
			LPClipboardData lpClipData = (LPClipboardData)GlobalLock(hMem); 
			
			int nLastPos = 0;
			int len = 0;
			tstring strText = LPCTSTR((LPBYTE)lpClipData + lpClipData->cdHeader.cdhOffset);
			tstring strSubText;
			LPClipboardImgData lpClipImgData = (LPClipboardImgData)((LPBYTE)lpClipData + sizeof(ClipboardDataHeader));
			for (int i = 0; i <  lpClipData->cdHeader.cdhImgCount; i++)
			{
				len = lpClipImgData[i].cidCharPos - nLastPos;
				strSubText = strText.substr(nLastPos, len );
				nLastPos = lpClipImgData[i].cidCharPos + 1 ;
				if (nLastPos < 0){nLastPos = 0;}

				if (!strSubText.empty())
					InsertText(strSubText.c_str(), 0, true);

				COleImageObj* pImgObj = COleImageObj::LoadImage(lpClipImgData[i].cidPath, lpClipImgData[i].cdiCache);
				if (pImgObj)
				{
					pImgObj->SetImageType(lpClipImgData[i].cidImgType);
					pImgObj->SetKey(lpClipImgData[i].cidKey);
					this->InsertImage(pImgObj);
				}
			}
			if (nLastPos < 0){nLastPos = 0;}
			strSubText = strText.substr(nLastPos);
			if (!strSubText.empty())
				InsertText(strSubText.c_str(), 0, true);
			GlobalUnlock(hMem);
			bPaste = true;

		}

	}


	if (!bPaste && m_bEnabeImage)
	{
		fmt.cfFormat = RegisterClipboardFormat(_T("HTML Format"));
		HRESULT ret = pDataObject->GetData(&fmt, &stgMedium);
		hMem = stgMedium.hGlobal;
		if (ret == S_OK && hMem)
		{
			bPaste = ReadHTMLFromClipboard(hMem);
		}
	}

	if (!bPaste && m_bEnabeImage)
	{
		fmt.cfFormat = CF_BITMAP;
		fmt.tymed = TYMED_GDI;
		HRESULT ret = pDataObject->GetData(&fmt, &stgMedium);
		fmt.tymed = TYMED_HGLOBAL;
		HBITMAP hBitmap = (HBITMAP)stgMedium.hBitmap;
		if (ret == S_OK && hBitmap)
		{
			TCHAR szTempDir[MAX_PATH] = {'\0'};
			DWORD dwLength = GetTempPath(MAX_PATH, szTempDir);
			_tcscat(szTempDir, _T("richole\\"));
			if (GetFileAttributes(szTempDir) != FILE_ATTRIBUTE_DIRECTORY) CreateDirectory(szTempDir, NULL);
			TCHAR szTempFile[30];
			s_tmp_image_count++;
			_stprintf_s(szTempFile, _T("temp_%d_%d.png"), GetCurrentProcessId(), s_tmp_image_count);
			tstring strTempFile = tstring(szTempDir) +  szTempFile;
			Bitmap* bitmap = Bitmap::FromHBITMAP(hBitmap, NULL);
			CImageLib::SaveImage(bitmap, strTempFile.c_str());
			delete bitmap;
			COleImageObj* pImgObj = COleImageObj::LoadImage(strTempFile.c_str(), false);
			if (pImgObj)
			{
				this->InsertImage(pImgObj);
			}
			bPaste = true;
		}
	}

	// 拖拉文件
	if (!bPaste)
	{
		fmt.cfFormat = CF_HDROP;
		fmt.tymed = TYMED_HGLOBAL;
		HRESULT ret = pDataObject->GetData(&fmt, &stgMedium);
		if (ret == S_OK && stgMedium.hGlobal)
		{
			this->SendNotify(UINOTIFY_DROPFILES, (WPARAM)stgMedium.hGlobal, NULL);
			bPaste = true;
		}
	}

	////粘贴普通文字
	if (!bPaste)
	{
		fmt.cfFormat = CF_UNICODETEXT;
		HRESULT ret = pDataObject->GetData(&fmt, &stgMedium);
		hMem = stgMedium.hGlobal;

		if (hMem)
		{
			LPCTSTR lpValue = (LPCTSTR)GlobalLock(hMem);//LPCWSTR
			InsertText(lpValue, 0, true);
			GlobalUnlock(hMem);
			bPaste = true;
		}
	}

	

	HideSelection(false);
}

void CRichEditUI::SetAutoURLDetect(bool bAutoURLDetect)
{
	m_bAutoURLDetect = bAutoURLDetect;

}

bool CRichEditUI::IsAutoURLDetect()
{
	return m_bAutoURLDetect;
}

HRESULT CRichEditUI::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult)
{
	if (m_pTextHost)
	{
		HRESULT hr =  m_pTextHost->GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
		return hr;
	}
	return S_FALSE;
}


CLinkObj* CRichEditUI::FindLink(POINT& pt)
{
	int nChar = this->CharFromPos(pt);
	for (int i = 0; i < m_listLinkObj.GetSize(); i++)
	{
		CLinkObj* pLinkObj = (CLinkObj*)m_listLinkObj.GetAt(i);
		if (pLinkObj->m_cpMin <= nChar && pLinkObj->m_cpMax > nChar)
		{
			if (pLinkObj->m_cpMin == nChar)
			{
				POINT pt1;
				GetCharPos(nChar, pt1);
				if (pt.x < pt1.x) return NULL;
			}
			else if (pLinkObj->m_cpMax - 1 == nChar)
			{
				POINT pt1;
				GetCharPos(nChar + 1, pt1);
				if (pt.x > pt1.x) return NULL;
			}
			return pLinkObj;
		}
	}
	return NULL;
}


void CRichEditUI::OnDrawAnimation(LPRECT lpRectClip)
{
	if (!IsVisible() || !IsInternVisible() || GetWindow()->IsMinimized() || !::IsWindowVisible(GetWindow()->GetHWND()))
		return;

	int nObjCnt = GetIRichEditOle()->GetObjectCount();
	if (nObjCnt <= 0)
		return;

	HDC dc = GetWindow()->GetMemDC();
	ClipObj childClip;
	ClipObj::GenerateClip(dc, *lpRectClip, childClip);


	long nMaxLine = GetLineCount()-1;
	int  nMaxLineH = 0;

	long nStartChar = 0,nEndChar = -1;
	GetVisibleCharRang(nStartChar, nEndChar);

	int nWidth = 0, nHeight = 0;
	REOBJECT reObj;
	reObj.cbStruct = sizeof(REOBJECT);
	COleImageObj* pImgObj = NULL;
	CHARRANGE cr;
	if (nObjCnt > 0)
		GetSel( cr );

	RECT rcTmp;
	RECT rcClient =GetInterRect();

	IRenderDC renderDC;
	renderDC.SetDevice(GetWindow()->GetDibObj());

	for (int i=(nObjCnt-1); i>=0; i--)
	{
		GetIRichEditOle()->GetObject(i, &reObj, REO_GETOBJ_POLEOBJ);
		if (reObj.cp < nStartChar || reObj.cp >= nEndChar)
		{
			reObj.poleobj->Release();
			continue;
		}

		pImgObj = dynamic_cast<COleImageObj*>(reObj.poleobj);
		if (!pImgObj)
		{
			reObj.poleobj->Release();
			continue;

		}
		pImgObj->LoadResource();
		if (!pImgObj->GetImage() || pImgObj->GetImage()->GetFrameCount() < 2)
		{
			reObj.poleobj->Release();
			continue;
		}

		RECT rcImage;
		((COleObject*)reObj.poleobj)->GetRect(&rcImage);
		reObj.poleobj->Release();
		if (!IntersectRect(&rcTmp, &rcImage, &rcClient))
		{
			continue;
		}
		rcImage.left += 1;
		rcImage.top += 1;
		rcImage.right -= 1;
		rcImage.bottom -= 1;

		pImgObj->m_nFrameDelay -= _RichDraw_Timer_Elapse_;
		if( pImgObj->m_nFrameDelay <= 0 )
		{
			pImgObj->NextFrame();
			pImgObj->m_nFrameDelay = pImgObj->GetFrameDelay();
		}


		GetWindow()->Draw(&renderDC, &rcImage);
		PaintParams param;
		param.eSmoothingMode = UITYPE_SMOOTHING::UISMOOTHING_HighQuality;
		renderDC.DrawImage(pImgObj->GetImage(), rcImage, pImgObj->m_nActiveFrame, &param);
			;// xqb 临时去掉
		
		//图像选中状态				
		if ((reObj.cp >= cr.cpMin && (reObj.cp+1) < cr.cpMax)
			||(reObj.cp >  cr.cpMin && (reObj.cp+1) == cr.cpMax))
		{
			::InvertRect(dc, &rcImage );
		}
		
		ClipObj childClip1;
		ClipObj::GenerateClip(GetWindow()->GetPaintDC(), *lpRectClip, childClip1);
		::BitBlt(GetWindow()->GetPaintDC(), rcImage.left, rcImage.top, rcImage.right - rcImage.left, rcImage.bottom - rcImage.top, dc, rcImage.left, rcImage.top, SRCCOPY );
	}

}

void CRichEditUI::OnCharPosChanged(long nChangePos, long nChangeLen)
{
	for (int i = m_listLinkObj.GetSize() - 1; i >= 0; i--)
	{
		CLinkObj* pLinkObj = (CLinkObj*)m_listLinkObj.GetAt(i);
		if (pLinkObj->m_cpMax <= nChangePos)
			continue;
		pLinkObj->m_cpMin += nChangeLen;
		pLinkObj->m_cpMax += nChangeLen;
	}

	for (int i = m_listTextObj.GetSize() - 1; i >= 0; i--)
	{
		ReTextObj* pTextObj = (ReTextObj*)m_listTextObj.GetAt(i);
		if (pTextObj->m_bIsUpdating)
			continue;
		if (pTextObj->m_cpMax <= nChangePos)
			continue;
		pTextObj->m_cpMin += nChangeLen;
		pTextObj->m_cpMax += nChangeLen;
	}
}

void CRichEditUI::GetVisibleCharRang( long& nVFirstChar, long& nVEndChar )
{

	if (GetUIEngine()->IsRTLLayout())
	{
		POINT pt = {m_pTextHost->rcClient.right, m_pTextHost->rcClient.top};
		nVFirstChar = CharFromPos(pt);
		pt.x = m_pTextHost->rcClient.left;
		pt.y = m_pTextHost->rcClient.bottom;
		nVEndChar = CharFromPos(pt);
	}
	else
	{
		POINT pt = {m_pTextHost->rcClient.left, m_pTextHost->rcClient.top};
		nVFirstChar = CharFromPos(pt);
		pt.x = m_pTextHost->rcClient.right;
		pt.y = m_pTextHost->rcClient.bottom;
		nVEndChar = CharFromPos(pt);

	}
}


int CRichEditUI::GetLastLineHeight()
{
	int nMaxH = 0;
	long nMaxLine = GetLineCount()-1;
	REOBJECT struREObj;
	struREObj.cbStruct = sizeof(REOBJECT);
	int nObjCnt = GetIRichEditOle()->GetObjectCount();
	for (int i=(nObjCnt-1); i>=0; i--)
	{
		GetIRichEditOle()->GetObject(i, &struREObj, REO_GETOBJ_NO_INTERFACES);
		if (LineFromChar(struREObj.cp) != nMaxLine)
		{
			if (struREObj.poleobj)
				struREObj.poleobj->Release();
			continue;
		}
		int yPerInch = ::GetDeviceCaps(hdcGrobal, LOGPIXELSY); 
		int nHeight = MulDiv(struREObj.sizel.cy, yPerInch, HIMETRIC_PER_INCH);
		nMaxH = nMaxH>nHeight ? nMaxH:nHeight;
		if (struREObj.poleobj)
			struREObj.poleobj->Release();
	}
	return nMaxH;
}

IRichEditOle*  CRichEditUI::GetIRichEditOle()
{
	return m_pTextHost->GetIRichEditOle();
}


long CRichEditUI::StreamIn(int nFormat, EDITSTREAM &es)
{ 
	LRESULT lResult;
	TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
	return (long)lResult;
}

long CRichEditUI::StreamOut(int nFormat, EDITSTREAM &es)
{ 
	LRESULT lResult;
	TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
	return (long)lResult; 
}


void CRichEditUI::SetMaxChar(int nMax)
{
	GetTextHost()->SetMaxChar(nMax);
}

int CRichEditUI::GetMaxChar()
{
	return GetTextHost()->GetMaxChar();
}

void CRichEditUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("maxchar")))
	{
		SetMaxChar(_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("enableimage")))
	{
		m_bEnabeImage = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("readonly")))
	{
		m_bReadOnly = (bool)!!_ttoi(lpszValue);
		SetReadOnly(m_bReadOnly);
	}
	else if (equal_icmp(lpszName, _T("vscroll")))
	{
		EnableVScrollBar((bool)!!_ttoi(lpszValue));
	
	}
	else if (equal_icmp(lpszName, _T("showcaret")))
	{
		EnableCaret((bool)!!_ttoi(lpszValue));

	}
	else
		CControlUI::SetAttribute(lpszName, lpszValue);
}

void CRichEditUI::SetMaxImageSize(int maxWidth, int maxHeight)
{
	m_nMaxImageWidth = maxWidth;
	m_nMaxImageHeight = maxHeight;
}

bool CRichEditUI::IsMouseWhellEnabled()
{
	if (IsFocused())
	{
		return true;
	}
	return __super::IsMouseWhellEnabled();
}

#define OPER_MAX 10 //转义符个数
const char Operator_Code[OPER_MAX][10]=
{
	"&nbsp;",	"&#160;",
	"&quot;",	"&#34;",
	"&amp;",	"&#38;",
	"&lt;",		"&#60;",
	"&gt;",		"&#62;"
};

const char Operator_Char[255]=
{
	' ',	' ',
	'\"',	'\"',
	'&',	'&',
	'<',	'<',
	'>',	'>'
};

#define EX_OPER_MAX 12 //扩展转义符个数，宽字符
const wchar_t Operator_Code_Unicode[EX_OPER_MAX][10]=
{
	_T("ldquo;"),	_T("#8220;"),
	_T("rdquo;"),	_T("#8221;"),
	_T("lsquo;"),	_T("#8216;"),
	_T("rsquo;"),	_T("#8217;"),
	_T("times;"),	_T("#215;"),
	_T("divide;"),	_T("#247;")
};
const wchar_t Operator_Char_Unicode[EX_OPER_MAX][5]=
{
	_T("“"),	_T("“"),
	_T("”"),	_T("”"),
	_T("‘"),	_T("‘"),
	_T("’"),	_T("’"),
	_T("×"),	_T("×"),
	_T("÷"),	_T("÷")
};
wstring Replace(const wstring& orignStr, const wstring& oldStr, const wstring& newStr) 
{ 
	size_t pos = 0; 
	wstring tempStr = orignStr; 
	wstring::size_type newStrLen = newStr.length(); 
	wstring::size_type oldStrLen = oldStr.length(); 
	while(true) 
	{ 
		pos = tempStr.find(oldStr, pos); 
		if (pos == wstring::npos) break; 
		tempStr.replace(pos, oldStrLen, newStr);         
		pos += newStrLen;
	} 
	return tempStr; 
}

char* urldecode(const char* encd,char* decd)
{
	int j,i;
	char *cd = (char*)encd;
	char p[2];
	unsigned int num;
	j=0;

	int len = strlen(cd);
	for( i = 0; i < len; i++ )
	{
		memset( p, '/0', 2 );
		if( cd[i] != '%' )
		{
			decd[j++] = cd[i];
			continue;
		}

		p[0] = cd[++i];
		p[1] = cd[++i];

		p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0); 
		p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0); 
		decd[j++] = (unsigned char)(p[0] * 16 + p[1]); 

	}
	return decd;
}

bool CRichEditUI::ReadHTMLFromClipboard(HGLOBAL hMem)
{
	char* lpszHTML = (char*)GlobalLock(hMem);
	char *ptr = strstr(lpszHTML, "StartFragment:");
	if (ptr == NULL) return false;
	UINT nStart = strtoul(ptr + 14, NULL, 10);
	ptr = strstr(lpszHTML, "EndFragment:");
	if (ptr == NULL) return false;
	UINT nEnd = strtoul(ptr + 12, NULL, 10);
	char* lpszSelHTML = new char[nEnd - nStart + 1];
	memcpy(lpszSelHTML, lpszHTML + nStart, nEnd - nStart);
	lpszSelHTML[nEnd - nStart] = '\0';
	GlobalUnlock(hMem);

	string strHTML = lpszSelHTML;//Charset::UTF8ToNative(lpszSelHTML);

	char* p = lpszSelHTML;

	std::transform(
		strHTML.begin(),
		strHTML.end(),
		strHTML.begin(),
		tolower);

	// 解析XML
	bool bInTag = false;
	int length = strHTML.length();
	int m = 0; 
	string strContent;
	CHAR szBuf[1025] = {'\0'};

	// 先转换成小写
	// 	for (int i = 0; i < length; i++)
	// 	{
	// 		if(p[i]== ('<'))
	// 		{
	// 			bInTag = true;
	// 		}
	// 		if (bInTag)
	// 		{
	// 			if(p[i]== ('>'))
	// 			{
	// 				bInTag = false;
	// 			}
	// 
	// 		}
	// 		if (bInTag && isupper(p[i]))
	// 		{
	// 			p[i] = tolower(p[i]);
	// 		}
	// 	}


	// 过滤掉<>及脚本样式
	bool bImgTag = false; // 需要保留IMG,BR等标签
	for (int i = 0; i < length; i++)
	{
		CHAR* p2 = p + i;
		if(p[i]== ('<'))
		{
			bInTag = true;
			if (strHTML.compare(i + 1, 3, ("img")) == 0)
			{
				bImgTag = true;
			}
			else if (strHTML.compare(i + 1, 2, ("br")) == 0 || \
				strHTML.compare(i + 1, 1, ("p")) == 0 || strHTML.compare(i + 1, 2, ("/p")) == 0 || 
				strHTML.compare(i + 1, 2, ("tr")) == 0 || strHTML.compare(i + 1, 3, ("/tr")) == 0 || 
				strHTML.compare(i + 1, 2, ("li")) == 0 || strHTML.compare(i + 1, 3, ("/li")) == 0 ||
				strHTML.compare(i+1, 3, ("div")) == 0 || strHTML.compare(i+1, 4, ("/div")) == 0 || 
				strHTML.compare(i + 1, 2, ("dl")) == 0 || strHTML.compare(i + 1, 3, ("dl")) == 0 || 
				strHTML.compare(i + 1, 2, ("dt")) == 0 || strHTML.compare(i + 1, 3, ("dt")) == 0 ||
				strHTML.compare(i + 1, 2, ("dd")) == 0 || strHTML.compare(i + 1, 3, ("dd")) == 0)
			{
				p2 = p + i;
				if (m > 0)
				{
					if (szBuf[m - 1] != ('\n'))
						szBuf[m++] = ('\n');
				}
				else if (strContent.length() > 0)
				{
					if (strContent.at(strContent.length() - 1) != ('\n'))
						szBuf[m++] = ('\n');
				}
				if (m > 1023)
				{
					szBuf[m] = ('\0');
					strContent += szBuf;
					m = 0;
				}	

			}
			else if (strHTML.compare(i + 1, 2, ("td")) == 0)
			{
				szBuf[m++] = ('\t');
				if (m > 1023)
				{
					szBuf[m] = ('\0');
					strContent += szBuf;
					m = 0;
				}	

			}
			else if (strHTML.compare(i+1, 6, ("script")) == 0)
			{
				CHAR* p1 = StrStrIA(p + i + 6, ("</script>"));		
				if (p1)
				{
					i = p1  - p + 8;
					continue;
				}

			}
			else if (strHTML.compare(i+1, 5, ("style")) == 0)
			{
				CHAR* p1 = StrStrIA(p + i + 5, ("</style>"));
				if (p1)
				{
					i = p1  - p + 7;
					continue;
				}
			}
		}
		if (bInTag)
		{
			if (bImgTag)
			{
				if( p[i]==  ('&'))
				{
					for(int k=0;k<OPER_MAX;k++)
					{  
						if (strHTML.compare(i, strlen(Operator_Code[k]), Operator_Code[k]) == 0)
						{   
							szBuf[m++] = Operator_Char[k];
							if (m > 1023)
							{
								szBuf[m] = ('\0');
								strContent += szBuf;
								m = 0;
							}	
							i+= strlen(Operator_Code[k]) - 1;
							break;
						}   
					}  

				}
				else
				{
					szBuf[m++] = p[i];
					if (m > 1023)
					{
						szBuf[m] = ('\0');
						strContent += szBuf;
						m = 0;
					}	

				}


			}
			if(p[i]== _T('>'))
			{
				bInTag = false;
				bImgTag = false;

			}
		}
		else if( p[i]== ('&'))
		{
			for(int k=0;k<OPER_MAX;k++)
			{  
				if (strHTML.compare(i, strlen(Operator_Code[k]), Operator_Code[k]) == 0)
				{   
					szBuf[m++] = Operator_Char[k];
					if (m > 1023)
					{
						szBuf[m] = ('\0');
						strContent += szBuf;
						m = 0;
					}	
					i+= strlen(Operator_Code[k]) - 1;
					break;
				}   
			}  

		}
		else
		{
			if (p[i] == ('\0') || p[i] == ('\ ') || (p[i] == ('\r')) || (p[i] == ('\n')))
			{
				if (m > 0 && (szBuf[m - 1] != ('\ ')) &&  (szBuf[m - 1] != ('\n')))
				{
					szBuf[m++] = ('\ ');
					if (m > 1023)
					{
						szBuf[m] = ('\0');
						strContent += szBuf;
						m = 0;
					}	
				}
				continue;
			}


			int l = p[i];
			szBuf[m++] = p[i];
			if (m > 1023)
			{
				szBuf[m] = ('\0');
				strContent += szBuf;
				m = 0;
			}	
		}
	}
	if (m > 0) 
	{
		szBuf[m] = ('\0');
		strContent += szBuf;
		m = 0;
	}

	// 去掉前面的空格和换行符
	int j = 0;
	length = strContent.length();
	for (int i = 0; i < length; i++)
	{
		TCHAR c = strContent.at(i);
		if (c == ('\n') || c == (' '))
			j++;
		else
			break;
	}
	if (j > 0) strContent.erase(0, j);

	// 去掉后面的空格和换行符

	j = 0;
	length = strContent.length();
	for (int i = length - 1; i >= 0; i--)
	{
		TCHAR c = strContent.at(i);
		if (c == ('\n') || c == _T(' '))
			j++;
		else
			break;
	}
	if (j > 0) strContent.erase(length - j, j);

	// 下载图片
	m = 0;
	j = 0;
	bImgTag = false; // 需要保留IMG,BR等标签
	p = (CHAR*)strContent.c_str();
	length = strContent.length();
	string strCmp = strContent;

	std::transform(
		strCmp.begin(),
		strCmp.end(),
		strCmp.begin(),
		tolower);

	if (strCmp.find("<img") == string::npos)
	{
		delete [] lpszSelHTML;
		return false;
	}

	for (int i = 0; i < length; i++)
	{
		if(p[i]== ('<'))
		{
			if (strCmp.compare(i + 1, 3, ("img")) == 0)
			{
				if (i > j)
				{
					InsertText(Charset::UTF8ToUnicode(strContent.substr(j, i - j).c_str()).c_str(), 0, true);
				}
				bImgTag = true;
				m = i;
			}
		}
		if (bImgTag)
		{
			if(p[i]== _T('>'))
			{
				bImgTag = false;
				j = i + 1;
				string strImg = strContent.substr(m, j - m);
				// 解析图片
				int k = strImg.find(("src"));
				int nImageType = 0;
				m = strImg.find(("type=\""));
				if (m > 0)
				{
					// 获取图片类型
					string strTmp = strImg.substr(m+6);
					m = strTmp.find(("\""));
					strTmp= strTmp.substr(0, m);
					nImageType = atoi(strTmp.c_str());
				}

				m = strImg.find(("key=\""));
				string strKey;
				if (m > 0)
				{
					// 获取图片类型
					string strTmp = strImg.substr(m+5);
					m = strTmp.find(("\""));
					strKey= strTmp.substr(0, m);
				}


				if (k > 0)
				{
					strImg = strImg.substr(k + 3);
					k = strImg.find(("\""));
					if (k >= 0)
					{
						strImg = strImg.substr(k + 1);
						k = strImg.find(("\""));
						if (k > 0)
						{
							strImg = strImg.substr(0, k);
							k = strImg.find(("file:///"));
							if (k < 0 && nImageType == 0)
							{
								DWORD dwEntrySize = 0;
								LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry;
								GetUrlCacheEntryInfo(Charset::UTF8ToUnicode(strImg.c_str()).c_str(), NULL, &dwEntrySize);
								if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
								{
									lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO)new BYTE[dwEntrySize];
									GetUrlCacheEntryInfo(Charset::UTF8ToUnicode(strImg.c_str()).c_str(),lpCacheEntry, &dwEntrySize);
									strImg = Charset::UnicodeToUTF8(lpCacheEntry->lpszLocalFileName);
									delete[] lpCacheEntry;
								}
							}
							else
							{
								strImg = strImg.substr(8);
							}

							char* pszImgTmp = new char[strImg.length() + 1];
							ZeroMemory(pszImgTmp, strImg.length() + 1);
							urldecode(strImg.c_str(), pszImgTmp);
							strImg = pszImgTmp;
							delete []pszImgTmp;

							COleImageObj* pImgObj = COleImageObj::LoadImage(Charset::UTF8ToUnicode(strImg.c_str()).c_str(), nImageType != 0);
							if (pImgObj != NULL)
							{
								pImgObj->SetImageType(nImageType);  //发送图片文件IMAGE_FILE
								if (!strKey.empty())
									pImgObj->SetKey(Charset::UTF8ToUnicode(strKey.c_str()).c_str());
								InsertImage(pImgObj);
							}

						}
					}
				}
			}
		}	
	}
	if (j < (int)strContent.length())
	{
		wstring wstrContent = Charset::UTF8ToUnicode(strContent.substr(j).c_str());
		for(int k=0; k<EX_OPER_MAX; k++)
		{  
			wstrContent = Replace(wstrContent, Operator_Code_Unicode[k], Operator_Char_Unicode[k]);
		}
		InsertText(wstrContent.c_str(), 0, true);
		//InsertText(Charset::UTF8ToUnicode(strContent.substr(j).c_str()).c_str(), 0, true);
	}
	delete [] lpszSelHTML;
	return true;
}

void CRichEditUI::SetImageThumbnail(bool bThumbnail)  //是否显示缩略图
{
	m_bImageThumbnail = bThumbnail;
}

bool CRichEditUI::GetImageThumbnail()
{
	return m_bImageThumbnail;
}

void CRichEditUI::SetStartIndent(int nStartIndent)
{
	int xPerInch = ::GetDeviceCaps(GetWindow()->GetPaintDC(), LOGPIXELSX); 
	m_nStartIndent = MulDiv(nStartIndent, xPerInch, HIMETRIC_PER_INCH);
}

int CRichEditUI::GetStartIndent()
{
	return m_nStartIndent;
}

void CRichEditUI::GetDropTarget(IDropTarget** ppdt)
{
	if (IsReadOnly()) 
	{
		*ppdt = NULL;
		return;
	}
	*ppdt = m_pTextHost->GetIDropTarget();
}

void CRichEditUI::SetRich(BOOL fNew)
{
	m_pTextHost->fRich = fNew;
	m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
		fNew ? TXTBIT_RICHTEXT : 0);
}

bool CRichEditUI::GetZoom(int& nNum, int& nDen)
{
    LRESULT lResult;
    TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetZoom(int nNum, int nDen)
{
    if (nNum < 0) return false;
    if (nDen < 0) return false;

    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
	m_bNeedReLayout = true;
	
    return (BOOL)lResult == TRUE;
}


UINT CRichEditUI::SetUndoLimit(UINT nLimit)
{
	LRESULT lResult;
	TxSendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0, &lResult);
	return (UINT)lResult;
}

void  CRichEditUI::EmptyUndoBuffer()
{
	LRESULT lResult;
	TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, &lResult);
}

bool CRichEditUI::ScrollBarEventDelegate(TEventUI& event)
{
	switch ( event.nType )
	{
	case UIEVENT_SETFOCUS:
		{
		   this->SetFocus();
		   event.nType = UIEVENT_UNUSED;
		}
		return true;
	}

	return true;
}

bool CRichEditUI::OnMenuClick(TNotifyUI* pNotifyUI)
{
	UIMENUINFO* pInfo = (UIMENUINFO*)pNotifyUI->wParam;
	switch (pInfo->uID)
	{
	case IDR_MENU_UIEDIT_CUT:
		{
			Cut();
		}
		break;
	case IDR_MENU_UIEDIT_COPY:
		{
			Copy();
		}
		break;
	case IDR_MENU_UIEDIT_PASTE:
		{
			Paste();
		}
		break;
	case IDR_MENU_UIEDIT_SELALL:
		{
			SetSelAll();
		}
		break;	
	default:
		break;
	}
	return true;
}


const TCHAR URL_Forbid_Char[]=
{
	_T(' '), _T('\r'), _T('\n'), _T('\''), _T('"'),
	_T('“'), _T('”'), _T('‘'), _T('’'), _T(''),_T('　')
};

const int KForbidCharCount = sizeof(URL_Forbid_Char) / sizeof(TCHAR);
bool  IsURLChars(TCHAR value)
{
	for(int i = 0; i < KForbidCharCount; i++ )
	{
		if(value == URL_Forbid_Char[i])
		{
			return false;
		}
	}
	return true;//value < 128;
}


static void DetectURLs(LPCTSTR lpszText, CStdPtrArray& arrLinksOut)
{
	tstring strText = lpszText;

	TCHAR* p = (TCHAR*)strText.c_str();
	int length = strText.length();
	// 先转换成小写
	int i, j , k;
	for (i = 0; i < length; i++)
	{
		if (p[i] >= _T('A') && p[i] < _T('Z'))
		{
			p[i] = tolower(p[i]);
		}
	}

	// 记录url的位置
	vector<int> vecCharPos;
	vector<int> vecURLPos;

	for (i = 0; i < URL_COUNT; i++)
	{
		p = FURLValues[i];
		k = _tcsclen(p);
		j = strText.rfind(p);
		while (j != -1)
		{
			vecCharPos.push_back(j);
			vecURLPos.push_back(i);
			j = j-k;
			if (j <= 0)
			{
				break;
			}
			j = strText.rfind(p, j);
		}
	}

	//url的位置进行排序
	int v = 0;
	int n = vecCharPos.size();
	for (i=n-1; i>0; i=k) /*循环到没有比较范围*/
	{
		for (j=0, k=0; j<i; j++) /*每次预置k=0，循环扫描后更新k*/
		{
			if (vecCharPos[j] > vecCharPos[j + 1])
			{
				v  = vecCharPos[j];
				vecCharPos[j] = vecCharPos[j + 1];
				vecCharPos[j + 1] = v;

				v  = vecURLPos[j];
				vecURLPos[j] = vecURLPos[j + 1];
				vecURLPos[j + 1] = v;
			}
			k = j; /*保存最后下沉的位置。这样k后面的都是排序排好了的。*/
		}
	}

	p = (TCHAR*)strText.c_str();
	int nStart, nEnd = -1;
	int nFurlValuesLen = -1;
	for (i = 0; i < n; i++)
	{
		nStart = vecCharPos[i];
		nFurlValuesLen = _tcsclen(FURLValues[vecURLPos[i]]);
		if (i < n-1)
		{
			int nNextStart = vecCharPos[i+1];
			if (nStart + nFurlValuesLen >= nNextStart)
			{
				continue;
			}
		}

		if (nStart < nEnd)
		{
			continue;
		}

		nEnd = vecCharPos[i] + nFurlValuesLen - 1;
		j = nEnd + 1;
		while (j < length)
		{
			if (IsURLChars(*(p + j)))
			{
				j++;
			}
			else
			{
				break;
			}
		}

		if (j > nEnd + 1)
		{
			nEnd = j - 1;
			CLinkObj* pLinkObj = new CLinkObj;
			pLinkObj->m_cpMin = nStart;
			pLinkObj->m_cpMax = nEnd;
			arrLinksOut.Add(pLinkObj);
		}
	}

}

