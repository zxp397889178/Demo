#pragma once

class IWindowPtrMap;

class CUI_API CWindowPtrHolder
{
public:
	CWindowPtrHolder();
	virtual ~CWindowPtrHolder();

public:
	CWindowUI*
		FindWindowPtr(void* pIdentify);

	void
		AddWindowPtr(void* pIdentify, CWindowUI* pWnd);

	void
		RemoveWindowPtr(void* pIdentify);

	//check null
	std::shared_ptr<IWindowPtrMap>
		GetWindowPtrMap();

private:
	std::shared_ptr<IWindowPtrMap>
					m_spWndMap;

};
