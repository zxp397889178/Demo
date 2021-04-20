#pragma once

class DUI_API CWindowBase : public BaseObject
{
public:
	CWindowBase();
	virtual ~CWindowBase();
	static CWindowBase* FromHandle(HWND hWnd);

public:
	HWND Create(HWND hwndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy);

	HWND GetHWND();

	void	SetWndStyle(int index, DWORD dwStyle);
	DWORD	GetWndStyle(int index);
	void	ModifyWndStyle(int index, DWORD dwRemove, DWORD dwAdd);

	LPCTSTR GetWindowClassName();
	void	SetWindowClassName(LPCTSTR lpszName);

	//子类化与反子类化
	BOOL SubclassWindow(HWND hWnd);
	void UnsubclassWindow();

	bool Attach(HWND hWnd);
	void Detach();
	
	void SetAutoDel(bool bAutoDel); //是否自动释放指针
	bool IsAutoDel();

public:
	virtual void    SetFocusOnWnd();
	// 消息预处理
	virtual bool    PreMessageHandler(const LPMSG pMsg, LRESULT& lRes);
	// 窗口过程函数
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	HWND m_hWnd;
private:
	bool RegisterWindowClass();
	WNDPROC m_oldProc;      //原窗体过程
	bool m_bAutoDel;
	tstring m_strWndClassName;
	DWORD m_dwExStyle;
	DWORD m_dwStyle;
	//}}
};




















