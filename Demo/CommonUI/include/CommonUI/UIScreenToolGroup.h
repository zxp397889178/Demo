#pragma once

/*!
    \brief    ȫ����ӳѧ�ƹ���/�������� group
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
	void            SetGroupId(int nCategory); //����ID
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

