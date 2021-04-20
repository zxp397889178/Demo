#pragma once

class IWindowPtrMap;
class CComplusWnd;

class CUI_API CIdentifyWnd :
	public CWindowUI,
	public CWindowPtrRef
{
public:
	CIdentifyWnd();
	virtual ~CIdentifyWnd(void);

public:
	void			SetIdentifyReference(HWND hIdentifyWnd, std::shared_ptr<IWindowPtrMap> spWndMap);
	virtual	HWND	GetIdentifyWnd();
	virtual CComplusWnd*
					GetComplusWnd(){ return NULL; };
	virtual  bool   IsLeftContentVisible(){ return true; };

protected:
	//override
	virtual void	OnCreate();
};