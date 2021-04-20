#ifndef __UIDOCKTOOL_H__
#define __UIDOCKTOOL_H__

class DUI_API CChildDockTool
{
public:
	CChildDockTool();
	~CChildDockTool();

public:
	void Dock(HWND hParent, HWND hChild, const Coordinate& coordinate);
	void Dock(HWND hParent, HWND hChild, const RECT& coordinateValue, UITYPE_ANCHOR anchor);
	void Dock(CControlUI* pControl, HWND hChild);
	void UnDock();

	HWND GetParent();
	HWND GetChild();
private:
	HWND m_hParent;
	HWND m_hChild;
	CWindowUI* m_pWindowChild;
	Coordinate m_coordinate;
	WNDPROC m_oldProcParent;      //原窗体过程
	void MoveChild(LPRECT lpRect = NULL);
	void CloseChild();
	static LRESULT CALLBACK ParentWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // __UIDOCKTOOL_H__