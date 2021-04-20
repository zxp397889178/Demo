
#ifndef __view_ax_host_h__
#define __view_ax_host_h__

#pragma once

#include <ocidl.h>
#include <oleidl.h>
#include <objsafe.h> 

#include "scoped_comptr.h"

namespace util_activex_gfx
{
    class Point;
    class Rect;
}

namespace util_activex
{
	class AxHost;

	class AxHostDelegate
	{
	public:
		virtual HWND GetAxHostWindow() const = 0;
		virtual void OnAxCreate(AxHost* host) = 0;
		virtual void OnAxInvalidate(const RECT& rect) = 0;
		virtual HRESULT QueryService(REFGUID guidService,
			REFIID riid, void** ppvObject) { return E_NOINTERFACE; }

	protected:
		virtual ~AxHostDelegate() {}
	};

	class AxHost : public IDispatch,
		public IOleClientSite,
		public IOleContainer,
		public IOleControlSite,
		public IOleInPlaceSiteWindowless,
		public IAdviseSink,
		public IServiceProvider,
		public IObjectSafety
	{
	public:
		AxHost(AxHostDelegate* delegate);
		virtual ~AxHost();

		IUnknown* controlling_unknown();
		IUnknown* activex_control();

		bool CreateControl(const CLSID& clsid);
		bool SetRect(const RECT& rect);
		void Draw(HDC hdc, const RECT& rect);
		LRESULT OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnWindowlessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool OnSetCursor(const POINT& point);

		// IUnknown:
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();
		STDMETHOD(QueryInterface)(REFIID iid, void** object);
		// IDispatch:
		STDMETHOD(GetTypeInfoCount)(UINT* pctinfo);
		STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
		STDMETHOD(GetIDsOfNames)(REFIID riid,
			LPOLESTR* rgszNames,
			UINT cNames,
			LCID lcid,
			DISPID* rgDispId);
		STDMETHOD(Invoke)(DISPID dispIdMember,
			REFIID riid,
			LCID lcid,
			WORD wFlags,
			DISPPARAMS* pDispParams,
			VARIANT* pVarResult,
			EXCEPINFO* pExcepInfo,
			UINT* puArgErr);

		// IOleClientSite:
		STDMETHOD(SaveObject)();
		STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker,
			IMoniker** ppmk);
		STDMETHOD(GetContainer)(IOleContainer** ppContainer);
		STDMETHOD(ShowObject)();
		STDMETHOD(OnShowWindow)(BOOL fShow);
		STDMETHOD(RequestNewObjectLayout)();

		// IOleContainer:
		STDMETHOD(ParseDisplayName)(IBindCtx* pbc,
			LPOLESTR pszDisplayName,
			ULONG* pchEaten,
			IMoniker** ppmkOut);
		STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
		STDMETHOD(LockContainer)(BOOL fLock);

		// IOleControlSite:
		STDMETHOD(OnControlInfoChanged)();
		STDMETHOD(LockInPlaceActive)(BOOL fLock);
		STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);
		STDMETHOD(TransformCoords)(POINTL* pPtlHimetric,
			POINTF* pPtfContainer,
			DWORD dwFlags);
		STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
		STDMETHOD(OnFocus)(BOOL fGotFocus);
		STDMETHOD(ShowPropertyFrame)();

		// IOleWindow:
		STDMETHOD(GetWindow)(HWND* phwnd);
		STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);
		// IOleInPlaceSite:
		STDMETHOD(CanInPlaceActivate)();
		STDMETHOD(OnInPlaceActivate)();
		STDMETHOD(OnUIActivate)();
		STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame,
			IOleInPlaceUIWindow** ppDoc,
			LPRECT lprcPosRect,
			LPRECT lprcClipRect,
			LPOLEINPLACEFRAMEINFO lpFrameInfo);
		STDMETHOD(Scroll)(SIZE scrollExtant);
		STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
		STDMETHOD(OnInPlaceDeactivate)();
		STDMETHOD(DiscardUndoState)();
		STDMETHOD(DeactivateAndUndo)();
		STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);
		// IOleInPlaceSiteEx:
		STDMETHOD(OnInPlaceActivateEx)(BOOL* pfNoRedraw, DWORD dwFlags);
		STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);
		STDMETHOD(RequestUIActivate)();
		// IOleInPlaceSiteWindowless:
		STDMETHOD(CanWindowlessActivate)();
		STDMETHOD(GetCapture)();
		STDMETHOD(SetCapture)(BOOL fCapture);
		STDMETHOD(GetFocus)();
		STDMETHOD(SetFocus)(BOOL fFocus);
		STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
		STDMETHOD(ReleaseDC)(HDC hDC);
		STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
		STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
		STDMETHOD(ScrollRect)(INT dx, INT dy,
			LPCRECT pRectScroll,
			LPCRECT pRectClip);
		STDMETHOD(AdjustRect)(LPRECT prc);
		STDMETHOD(OnDefWindowMessage)(UINT msg,
			WPARAM wParam,
			LPARAM lParam,
			LRESULT* plResult);

		// IAdviseSink:
		STDMETHOD_(void, OnDataChange)(FORMATETC* pFormatetc, STGMEDIUM* pStgmed);
		STDMETHOD_(void, OnViewChange)(DWORD dwAspect, LONG lindex);
		STDMETHOD_(void, OnRename)(IMoniker* pmk);
		STDMETHOD_(void, OnSave)();
		STDMETHOD_(void, OnClose)();

		// IServiceProvider:
		STDMETHOD(QueryService)(REFGUID guidService,
			REFIID riid, void** ppvObject);

		//IObjectSafety
		STDMETHOD (GetInterfaceSafetyOptions)( REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions);
		STDMETHOD (SetInterfaceSafetyOptions)( REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);

	private:
		bool ActivateAx();
		void ReleaseAll();

		AxHostDelegate* delegate_;

		// IOleInPlaceSiteWindowless
		HDC screen_dc_;
		bool dc_released_;

	public:
		// pointers
		util_activex::ScopedComPtr<IUnknown> unknown_;
		util_activex::ScopedComPtr<IOleObject> ole_object_;
		util_activex::ScopedComPtr<IOleInPlaceFrame> inplace_frame_;
		util_activex::ScopedComPtr<IOleInPlaceUIWindow> inplace_uiwindow_;
		util_activex::ScopedComPtr<IViewObjectEx> view_object_;
		util_activex::ScopedComPtr<IOleInPlaceObjectWindowless> inplace_object_windowless_;
	private:

		DWORD view_object_type_;

		// state
		bool inplace_active_;
		bool ui_active_;
		bool windowless_;
		bool capture_;
		bool have_focus_;

		DWORD ole_object_sink_;
		DWORD misc_status_;

		RECT bounds_;

		// Accelerator table
		HACCEL accel_;

		// Ambient property storage
		unsigned long can_windowless_activate_ : 1;
		unsigned long user_mode_ : 1;
	};
}
#endif //__view_ax_host_h__