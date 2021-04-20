#pragma once

//目前只考虑横向布局

class CUI_API CInteractOptionLayoutUI:
	public CLayoutUI
{
	UI_OBJECT_DECLARE(CInteractOptionLayoutUI, _T("InteractOptionLayout"))
public:
	//{{
	CInteractOptionLayoutUI();
	virtual ~CInteractOptionLayoutUI();

public:
	virtual bool	Add(CControlUI* pControlChild);

	virtual bool	Insert(CControlUI* pControlChild, CControlUI* pControlAfter);

	virtual bool	SelectControl(CControlUI* pItem);
	virtual void
					MoveItem(int nIndx, int nAfterIndex);

	virtual bool	Remove(CControlUI* pControlChild);

	virtual void	RemoveAll();

	virtual	int		GetItemMaxWidth();

protected:
	virtual	int		GetItemWidth(CControlUI* pItem);

	virtual	bool	LayoutChild();

	virtual void	ResetChildWidth();

	void			ClearMap();

	bool			LayoutChildInternal(int nParentWidth, int nParentHeight, int nItemDefaultWidth, int nModWidth);

protected:
	virtual void	Init();

	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	virtual void	Render(IRenderDC* pRenderDC, RECT& rcPaint);

	virtual CControlUI*
					FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	virtual bool	Event(TEventUI& event);

	virtual void	SetRect(RECT& rectRegion);

private:
	int			m_nOverlappadX;
	int			m_nItemMaxWidth;
	bool		m_bItemAutoSize;
	//组item和非组item交叉时是否生效OverlappadX属性
	bool		m_bCrossOverlappad;
	std::map<CControlUI*, HRGN>
				m_mapCtrlsRgn;
};

