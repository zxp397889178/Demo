#pragma once

typedef std::map<tstring, CWindowUI*> WindowMap;

class CUI_API IWindowPtrMap
{
public:
	virtual ~IWindowPtrMap(){};

public:
	virtual CWindowUI*
		FindWnd(void* hWnd) = 0;
	virtual void
		AddWnd(void* hWnd, CWindowUI* pWnd) = 0;
	virtual void
		RemoveWnd(void* hWnd) = 0;

	virtual CWindowUI*
		FindWnd(LPCTSTR lptcsKey) = 0;
	virtual void
		AddWnd(LPCTSTR lptcsKey, CWindowUI* pWnd) = 0;
	virtual void
		RemoveWnd(LPCTSTR lptcsKey) = 0;

	virtual WindowMap*
		GetWindowMap() = 0;
};
