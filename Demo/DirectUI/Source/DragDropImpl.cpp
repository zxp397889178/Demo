/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "DragDropImpl.h"

//////////////////////////////////////////////////////////////////////////////////////////////
CDropSource::CDropSource(IDataObject* pDataObject, CControlUI* pControl)
{
	m_lRefCount = 1;
	cancel_drag_ =false;
	m_pControl = pControl;
	m_pDataObject = pDataObject;
}

CDropSource::~CDropSource()
{

}


HRESULT __stdcall CDropSource::QueryInterface (REFIID iid, void ** ppvObject)
{
	// Simplified version...
	// TEST_ NULL 
	if(ppvObject == NULL)
		return E_POINTER;  

	// Supports IID_IUnknown and IID_IDropTarget 
	if(iid == IID_IUnknown || 
		iid == IID_IDropSource)
	{
		*ppvObject = (void *) this;
		AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE; 

}

ULONG   __stdcall CDropSource::AddRef (void)
{
	return ++m_lRefCount;
}
ULONG   __stdcall CDropSource::Release (void)
{
	ULONG c_Refs = --m_lRefCount;
	if (c_Refs == 0)
	{
		delete this;
	}
	return c_Refs;

}

void CDropSource::CancelDrag()
{
	cancel_drag_ = true;
}

HRESULT __stdcall CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) 
{ 
	if(cancel_drag_)
	{
		return DRAGDROP_S_CANCEL;
	}

	if(fEscapePressed)
	{
		OnDragSourceCancel();
		return DRAGDROP_S_CANCEL;
	}

	if((grfKeyState & MK_LBUTTON) == 0) 
	{
		OnDragSourceDrop();
		return DRAGDROP_S_DROP; 
	}
	
	OnDragSourceMove();
	return S_OK;
} 

HRESULT __stdcall CDropSource::GiveFeedback (DWORD dwEffect)
{ 
	return DRAGDROP_S_USEDEFAULTCURSORS;
} 

void CDropSource::OnDragSourceCancel()
{

}

void CDropSource::OnDragSourceDrop()
{
	if (m_pControl)
	{
		((CDataObject*)m_pDataObject)->m_pDragControl = m_pControl;
		
	}
}

void CDropSource::OnDragSourceMove()
{

}

////////////////////////////////////////////////////////////////////////////////////////
CDropTarget::CDropTarget(CWindowUI* pWindow, DWORD dwAcceptKeyState)
{
	cRefs = 0;
	m_pDataObject = NULL;
	m_pControl = NULL;
	memset(&m_objParams, 0, sizeof(DragDropParams));
	if(IsWindow(pWindow->GetHWND()))
		::RegisterDragDrop (pWindow->GetHWND(),this);
	m_AcceptKeyState = dwAcceptKeyState;
	m_pWindow = pWindow;
}


CDropTarget::~CDropTarget()
{
	if (m_pWindow)
	{
		RevokeDragDrop(m_pWindow->GetHWND());
	}

}

// get a pointer to our interface
HRESULT CDropTarget::QueryInterface(REFIID iid, void ** ppvObject)
{
	// Simplified version...
	// TEST_ NULL 
	if(ppvObject == NULL)
		return E_POINTER;  

	// Supports IID_IUnknown and IID_IDropTarget 
	if(iid == IID_IUnknown || 
		iid == IID_IDropTarget)
	{
		*ppvObject = (void *) this;
		AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE; 
}

ULONG CDropTarget::AddRef(void)
{
	return ++cRefs;
}

ULONG CDropTarget::Release(void)
{
	ULONG c_Refs = --cRefs;
	if (c_Refs == 0)
	{
		delete this;
	}
	return c_Refs;
}

// Interface IDropTarget
// the drag action continues
HRESULT CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	m_objParams.pdwEffect = pdwEffect;
	m_objParams.grfKeyState = grfKeyState;
	m_objParams.pt = pt;

	HRESULT hr = S_OK;
	POINT point = {pt.x, pt.y};
	ScreenToClient(m_pWindow->GetHWND(), &point);
	CControlUI * pControl = m_pWindow->GetRoot()->FindControl(__FindControlEnableDrop, &point, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	if (pControl != m_pControl)
	{
		if (m_pControl)
		{
			TEventUI event = {0};
			event.nType = UIEVENT_DRAG_DROP_GROUPS;
			event.pSender = m_pControl;
			event.wParam = (WPARAM)UIEVENT_DRAG_LEAVE;
			event.lParam = (LPARAM)(&m_objParams);
			m_pControl->SendEvent(event);
		}

		m_pControl = pControl;
		if (m_pControl)
		{
			TEventUI event;
			event.nType = UIEVENT_DRAG_DROP_GROUPS;
			event.pSender = m_pControl;
			event.wParam = (WPARAM)UIEVENT_DRAG_ENTER;
			event.lParam = (LPARAM)(&m_objParams);
			m_pControl->SendEvent(event);
		}
	}
	else if (m_pControl)
	{
		TEventUI event;
		event.nType = UIEVENT_DRAG_DROP_GROUPS;
		event.pSender = m_pControl;
		event.wParam = (WPARAM)UIEVENT_DRAG_OVER;
		event.lParam = (LPARAM)(&m_objParams);
		m_pControl->SendEvent(event);
	}
	if (!m_pControl)
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}
	return hr;
}

// the drag action leaves your window - no dropping
HRESULT CDropTarget::DragLeave()
{
	HRESULT hr = S_OK;
	if (m_pControl)
	{
		TEventUI event = {0};
		event.nType = UIEVENT_DRAG_DROP_GROUPS;
		event.pSender = m_pControl;
		event.wParam = (WPARAM)UIEVENT_DRAG_LEAVE;
		event.lParam = (LPARAM)(&m_objParams);
		m_pControl->SendEvent(event);
		m_pControl = NULL;
	}
	return hr;
}

// The drag action enters your window - get the file
HRESULT CDropTarget::DragEnter( IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	// 判断是否是文件
	m_objParams.pDataObject = pDataObject;
	m_objParams.pt = pt;
	m_objParams.pdwEffect = pdwEffect;
	m_objParams.grfKeyState = grfKeyState;

	m_objParams.bFiles = false;
	if (pDataObject)
	{
		STGMEDIUM stgMedium;
		FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
		HRESULT ret = pDataObject->GetData(&fmt, &stgMedium);
		if (ret == S_OK && stgMedium.hGlobal)
		{
			TCHAR szFile[MAX_PATH] = {0};
			int count = DragQueryFile((HDROP)stgMedium.hGlobal,0xFFFFFFFF, szFile, MAX_PATH);
			if (count > 0)
			{
				m_objParams.bFiles = true;
			}
		}

	}

	HRESULT hr = S_OK;
	POINT point = {pt.x, pt.y};
	ScreenToClient(m_pWindow->GetHWND(), &point);
	m_pControl = m_pWindow->GetRoot()->FindControl(__FindControlEnableDrop, &point, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	if (!m_pControl)
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}

	TEventUI event;
	event.nType = UIEVENT_DRAG_DROP_GROUPS;
	event.pSender = m_pControl;
	event.wParam = (WPARAM)UIEVENT_DRAG_ENTER;
	event.lParam = (LPARAM)(&m_objParams);
	m_pControl->SendEvent(event);
	return hr;
}


// the data have been dropped here, so process it
HRESULT CDropTarget::Drop(IDataObject * pDataObject,	DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	m_objParams.pDataObject = pDataObject;
	m_objParams.pt = pt;
	m_objParams.pdwEffect = pdwEffect;
	m_objParams.grfKeyState = grfKeyState;

	HRESULT hr = S_OK;
	POINT point = {pt.x, pt.y};
	ScreenToClient(m_pWindow->GetHWND(), &point);
	CControlUI* pControl = m_pWindow->GetRoot()->FindControl(__FindControlEnableDrop, &point, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	if (pControl)
	{
		TEventUI event;
		event.nType = UIEVENT_DRAG_DROP_GROUPS;
		event.pSender = m_pControl;
		event.wParam = (WPARAM)UIEVENT_DROP;
		event.lParam = (LPARAM)(&m_objParams);
		m_pControl->SendEvent(event);
	}
	return hr;
}