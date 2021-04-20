#pragma once

/*!
    \brief    容器控件
*****************************************/
class  DUI_API CContainerUI : public CControlUI
{
	UI_OBJECT_DECLARE(CContainerUI, _T("Container"))
public:
	//{{
	CContainerUI();
	virtual ~CContainerUI();

public:
	// 操作子控件接口（从CControlUI派生的接口）
	virtual int GetCount();
	virtual CControlUI* GetItem(int iIndex);
	virtual CControlUI* GetItem(LPCTSTR lpszId);
	virtual bool Add(CControlUI* pControlChild);
	virtual bool Insert(CControlUI* pControlChild, CControlUI* pControlAfter);
	virtual bool Remove(CControlUI* pControlChild);
	virtual void RemoveAll();
	// 操作子控件接口
	CStdPtrArray* GetItemArray();
	bool RemoveWithoutDelCtrl(CControlUI* pControlChild);  
	bool MoveItem(CControlUI* pControlChild, CControlUI* pControlAfter); // 移动子控件
	bool Contain(CControlUI* pControlChild);
	virtual bool SelectItem(CControlUI* pControl);
	CControlUI* GetSelectedItem();

	//  滚动条相关接口
	void EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal);
	bool IsEnableVScrollbar();
	bool IsEnableHScrollbar();
	CScrollbarUI* GetVScrollbar();
	CScrollbarUI* GetHScrollbar();

	virtual void SetVisible(bool bShow);
	virtual void SetInternVisible(bool bShow);
	virtual bool IsContainer();

	virtual void SetRect(RECT& rectRegion);
	virtual void SetManager(CWindowUI* pManager, CControlUI* pParent);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	virtual void ReleaseImage();
	virtual RECT GetClientRect();
	virtual RECT GetScrollRect();
	virtual void SetINotifyUI(INotifyUI* pINotify);

	virtual void SetAlpha(int val);

	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual bool Event(TEventUI& event);
	// 这个两个接口是多余的，为了101ppt暂时加上
	CControlUI* FindSubControl(LPCTSTR pstrSubControlName);
	virtual bool OnlyResizeChild();
	virtual void OnlyShowChild(CControlUI *pItem);
protected:
	virtual void OnProcessScrollbar(int cxRequired, int cyRequired);
	virtual SIZE OnChildSize(RECT& rcWinow, RECT& rcClient);
	virtual void OnLanguageChange();
	virtual	void CalcChildRect();
	bool SelectControl(CControlUI* pItem);
	CControlUI* GetCurSelControl();
private:
	void HandleGestureEvent(TEventUI& event);
	CStdPtrArray m_items;
	CControlUI* m_pCurSelItem;
	RECT m_rcClient; // 客户区区域
	bool m_bEnableVertical;
	bool m_bEnableHorizontal;
	bool m_bShowVertical;        //渲染时是否显示垂直滚动条
	bool m_bShowHorizontal;      //渲染时是否显示水平滚动条
	bool m_bFolatVertical;       //垂直滚动条是否占用子控件区域
	bool m_bFolatHorizontal;     //水平滚动条是否占用子控件区域
	CScrollbarUI* m_pVerticalScrollbar;
	CScrollbarUI* m_pHorizontalScrollbar;
	bool m_bAutoFitHeight;
	bool m_bAutoFitWidth;
	/// 防止OnSize循环调用
	bool m_bScrollProcess;
	//}}

};


