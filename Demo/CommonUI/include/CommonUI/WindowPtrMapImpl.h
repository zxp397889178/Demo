#pragma once

class CUI_API CWindowPtrMapImpl :
	public IWindowPtrMap
{
	
public:
	CWindowPtrMapImpl();
	virtual ~CWindowPtrMapImpl();

private:
	virtual CWindowUI*	FindWnd(void* hWnd);
	virtual CWindowUI*	FindWnd(LPCTSTR lptcsKey);

	virtual void		AddWnd(void* hWnd, CWindowUI* pWnd);
	virtual void		AddWnd(LPCTSTR lptcsKey, CWindowUI* pWnd);

	virtual void		RemoveWnd(void* hWnd);
	virtual void		RemoveWnd(LPCTSTR lptcsKey);

	virtual WindowMap*  GetWindowMap();

private:
	WindowMap*
		m_pWndMap;
};
