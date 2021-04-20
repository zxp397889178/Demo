#pragma once
#ifndef __DIRECTUI_COMBOBOX_H_
#define __DIRECTUI_COMBOBOX_H_

class CComboBoxDropDownWnd;

/*!
    \brief    组合框控件
*****************************************/
class DUI_API CComboBoxUI : public CEditUI
{
	//{{
	friend class CComboBoxDropDownWnd;
	UI_OBJECT_DECLARE(CComboBoxUI, _T("ComboBox"))
public:
	CComboBoxUI();
	virtual ~CComboBoxUI();
	//}}

	/*!
	   \brief    获取第一个文字匹配的控件
	   \param    LPCTSTR lpszText 
	   \return   CControlUI* 
	 ************************************/
	CControlUI* FindFirstItem(LPCTSTR lpszText);

	/*!
	   \brief    获取当前选中控件
	   \return   CControlUI* 
	 ************************************/
	CControlUI* GetCurSel();

	/*!
	   \brief    选中索引
	   \param    int index 
	   \return   void 
	 ************************************/
	virtual void SelectItem(int index);

	/*!
	   \brief    选中控件
	   \param    CControlUI * pControl 
	   \return   void 
	 ************************************/
	virtual void SelectItem(CControlUI* pControl);

	/*!
	   \brief    是否开启check
	   \return   bool 
	 ************************************/
	bool IsEnableCheck();

	/*!
	   \brief    是否为drop类型
	   \return   bool 
	 ************************************/
	bool IsDrop();

	/*!
	   \brief    是否失焦
	   \return   bool 
	 ************************************/
	bool IsKillFocus();

	/*!
	   \brief    check选中
	   \param    bool bCheck 
	   \param    int nIndex 
	   \return   void 
	 ************************************/
	void CheckItem(bool bCheck, int nIndex = -1);//-1为全选

	/*!
	   \brief    check是否选中
	   \param    int nIndex 
	   \return   bool 
	 ************************************/
	bool IsCheckItem(int nIndex);

	/*!
	   \brief    获取列表指针
	   \return   CListUI* 
	 ************************************/
	CListUI* GetList();

	/*!
	   \brief    移除所有
	   \return   bool 
	 ************************************/
	bool RemoveAll();

	tstring &GetWndStyle();

	// 继承函数
	virtual bool Add(CControlUI* pControl);
	virtual bool Remove(CControlUI* pControl);
	virtual CControlUI* GetItem(int index);
	virtual int GetCount();
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void SetRect(RECT& rectRegion);
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void OnLanguageChange();
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);

	HWND GetDropWndHandle();

	void HideDropList();
	//{{
protected:
	/*!
	   \brief    显示下拉框，并设置位置等
	   \return   void 
	 ************************************/
	virtual void ShowDrop();

	/*!
	   \brief    选中
	   \param    int index 
	   \param    bool bTextChange 文字是否改变
	   \return   void 
	 ************************************/
	virtual void SelectItem(int index, bool bTextChange);

	/*!
	   \brief    获取下拉图标的位置
	   \return   RECT 
	 ************************************/
	RECT GetArrowRect();

	/*!
	   \brief    显示下拉框
	   \return   void 
	 ************************************/
	void Show();

	/*!
	   \brief    隐藏下拉框
	   \return   void 
	 ************************************/
	void Hide();

	/*!
	   \brief    下拉框是否显示
	   \return   bool 
	 ************************************/
	bool IsShow();

	CWindowUI* m_pToolWindow;
	CListUI* m_pList;
	int m_nItemsEnsureVisible;
	UITYPE_STATE m_nArrowIndex;
	bool m_bDropList;
	bool m_bEnableCheck;
	tstring m_strWndStyle;
	RECT	m_rcPadding;
	int		m_nItemsHeight;
	//}}
};	

#endif //__DIRECTUI_COMBOBOX_H_