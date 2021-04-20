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

	//���໯�뷴���໯
	BOOL SubclassWindow(HWND hWnd);
	void UnsubclassWindow();

	bool Attach(HWND hWnd);
	void Detach();
	
	void SetAutoDel(bool bAutoDel); //�Ƿ��Զ��ͷ�ָ��
	bool IsAutoDel();

public:
	virtual void    SetFocusOnWnd();
	// ��ϢԤ����
	virtual bool    PreMessageHandler(const LPMSG pMsg, LRESULT& lRes);
	// ���ڹ��̺���
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	HWND m_hWnd;
private:
	bool RegisterWindowClass();
	WNDPROC m_oldProc;      //ԭ�������
	bool m_bAutoDel;
	tstring m_strWndClassName;
	DWORD m_dwExStyle;
	DWORD m_dwStyle;
	//}}
};




















