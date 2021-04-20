#pragma once

class IWindowPtrMap;

class CUI_API CWindowPtrRef
{
public:
	virtual ~CWindowPtrRef();

public:
	void			SetWindowPtrRef(void* pIdentify, std::shared_ptr<IWindowPtrMap> spWndMap);
	void			SetWindowPtrRef(void* pIdentify, CWindowUI* pWnd, std::shared_ptr<IWindowPtrMap> spWndMap);
	void			SetWindowPtrRef(LPCTSTR lpszIdentify, CWindowUI* pWnd, std::shared_ptr<IWindowPtrMap> spWndMap);
	HWND			GetWindowPtrIdentifyAsWnd();
	tstring       	GetWindowPtrIdentify();

	//check null
	std::shared_ptr<IWindowPtrMap>
					GetWindowPtrMap();

private:
	tstring   	   m_strIdentify;
	std::weak_ptr<IWindowPtrMap>
					m_spWndMap;

};
