#pragma once

class CListLayoutUI : public CLayoutUI
{
	UI_OBJECT_DECLARE(CListLayoutUI, _T("ListLayout"))
public:
	CListLayoutUI();
	virtual ~CListLayoutUI();
    
	virtual void Init();
	virtual void EnsureVisible(CControlUI* pControl);
};


class CNewComboBoxExUI;
class CNewComboBoxDropDownWnd :
	public CNoActiveWindowUI, 
	public IMessageFilterUI
{
public:
	CNewComboBoxDropDownWnd(CNewComboBoxExUI* pOwner);
	virtual ~CNewComboBoxDropDownWnd();

	UIBEGIN_MSG_MAP
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("NewComboBoxListItem"), OnClickItem)
	UIEND_MSG_MAP

public:

	bool  OnClickItem(TNotifyUI* pNotify);
	bool  IsShow();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
protected:
	virtual LRESULT MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

private:
	CNewComboBoxExUI* m_pOwner;
};


/*!
    \brief    组合框控件
*****************************************/
class CUI_API CNewComboBoxExUI : public CButtonUI
{
	
	friend class CNewComboBoxDropDownWnd;
	UI_OBJECT_DECLARE(CNewComboBoxExUI, _T("NewComboBoxEx"))
public:
	CNewComboBoxExUI();
	virtual ~CNewComboBoxExUI();

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
	   \brief    是否失焦
	   \return   bool 
	 ************************************/
	bool IsKillFocus();

	/*!
	   \brief    获取列表指针
	   \return   CListUI* 
	 ************************************/
	CListLayoutUI* GetList();

	/*!
	   \brief    移除所有
	   \return   bool 
	 ************************************/
	bool RemoveAll();

	// 继承函数
	virtual bool Add(CControlUI* pControl);
	virtual bool Remove(CControlUI* pControl);
	virtual CControlUI* GetItem(int index);
	virtual int GetCount();
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void SetRect(RECT& rectRegion);
	virtual bool Event(TEventUI& event);
	virtual void OnLanguageChange();

	//{{
protected:
	/*!
	   \brief    显示下拉框，并设置位置等
	   \return   void 
	 ************************************/
	virtual void ShowDrop();

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

	CWindowUI*	   m_pToolWindow;
	CListLayoutUI* m_pList;

	int		m_nItemsEnsureVisible;
	RECT	m_rcPadding;
	int		m_nItemsHeight;
	bool    m_bUseSelectItemTag; //选中之后是否显示item的tag数据，默认显示item的text数据
};	
