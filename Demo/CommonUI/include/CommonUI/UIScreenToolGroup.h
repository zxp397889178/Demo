#pragma once

/*!
    \brief    全屏放映学科工具/互动工具 group
*****************************************/
class CUI_API CScreenToolGroupUI :
	public CLayoutUI
{
	UI_OBJECT_DECLARE(CScreenToolGroupUI, _T("ScreenToolGroup"))
public:	
	CScreenToolGroupUI();
	~CScreenToolGroupUI();

	LPCTSTR			GetGroupName();
	void            SetGroupName(LPCTSTR lstrGroupName);
	void            SetGroupId(int nCategory); //分类ID
	int             GetGroupId();

	void            AddItem(CControlUI* pItem);

	CLayoutUI*      GetItemList();
	CLayoutUI*      GetEmptyContent();

	void            SetItemListVisible(bool bVisible);
	void            SetEmptyContentVisible(bool bVisible);
protected:
	virtual void	Init();

protected:
	CControlUI*     m_pGroupTypeName;
	CLayoutUI*      m_pItemList;
	CLayoutUI*		m_pLayoutEmpty;
	int             m_nCategoryId;
};

