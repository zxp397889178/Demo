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
    \brief    ��Ͽ�ؼ�
*****************************************/
class CUI_API CNewComboBoxExUI : public CButtonUI
{
	
	friend class CNewComboBoxDropDownWnd;
	UI_OBJECT_DECLARE(CNewComboBoxExUI, _T("NewComboBoxEx"))
public:
	CNewComboBoxExUI();
	virtual ~CNewComboBoxExUI();

	/*!
	   \brief    ѡ������
	   \param    int index 
	   \return   void 
	 ************************************/
	virtual void SelectItem(int index);

	/*!
	   \brief    ѡ�пؼ�
	   \param    CControlUI * pControl 
	   \return   void 
	 ************************************/
	virtual void SelectItem(CControlUI* pControl);

	/*!
	   \brief    �Ƿ�ʧ��
	   \return   bool 
	 ************************************/
	bool IsKillFocus();

	/*!
	   \brief    ��ȡ�б�ָ��
	   \return   CListUI* 
	 ************************************/
	CListLayoutUI* GetList();

	/*!
	   \brief    �Ƴ�����
	   \return   bool 
	 ************************************/
	bool RemoveAll();

	// �̳к���
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
	   \brief    ��ʾ�����򣬲�����λ�õ�
	   \return   void 
	 ************************************/
	virtual void ShowDrop();

	/*!
	   \brief    ��ʾ������
	   \return   void 
	 ************************************/
	void Show();

	/*!
	   \brief    ����������
	   \return   void 
	 ************************************/
	void Hide();

	/*!
	   \brief    �������Ƿ���ʾ
	   \return   bool 
	 ************************************/
	bool IsShow();

	CWindowUI*	   m_pToolWindow;
	CListLayoutUI* m_pList;

	int		m_nItemsEnsureVisible;
	RECT	m_rcPadding;
	int		m_nItemsHeight;
	bool    m_bUseSelectItemTag; //ѡ��֮���Ƿ���ʾitem��tag���ݣ�Ĭ����ʾitem��text����
};	
