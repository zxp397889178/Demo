#pragma once
#ifndef __DIRECTUI_COMBOBOX_H_
#define __DIRECTUI_COMBOBOX_H_

class CComboBoxDropDownWnd;

/*!
    \brief    ��Ͽ�ؼ�
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
	   \brief    ��ȡ��һ������ƥ��Ŀؼ�
	   \param    LPCTSTR lpszText 
	   \return   CControlUI* 
	 ************************************/
	CControlUI* FindFirstItem(LPCTSTR lpszText);

	/*!
	   \brief    ��ȡ��ǰѡ�пؼ�
	   \return   CControlUI* 
	 ************************************/
	CControlUI* GetCurSel();

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
	   \brief    �Ƿ���check
	   \return   bool 
	 ************************************/
	bool IsEnableCheck();

	/*!
	   \brief    �Ƿ�Ϊdrop����
	   \return   bool 
	 ************************************/
	bool IsDrop();

	/*!
	   \brief    �Ƿ�ʧ��
	   \return   bool 
	 ************************************/
	bool IsKillFocus();

	/*!
	   \brief    checkѡ��
	   \param    bool bCheck 
	   \param    int nIndex 
	   \return   void 
	 ************************************/
	void CheckItem(bool bCheck, int nIndex = -1);//-1Ϊȫѡ

	/*!
	   \brief    check�Ƿ�ѡ��
	   \param    int nIndex 
	   \return   bool 
	 ************************************/
	bool IsCheckItem(int nIndex);

	/*!
	   \brief    ��ȡ�б�ָ��
	   \return   CListUI* 
	 ************************************/
	CListUI* GetList();

	/*!
	   \brief    �Ƴ�����
	   \return   bool 
	 ************************************/
	bool RemoveAll();

	tstring &GetWndStyle();

	// �̳к���
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
	   \brief    ��ʾ�����򣬲�����λ�õ�
	   \return   void 
	 ************************************/
	virtual void ShowDrop();

	/*!
	   \brief    ѡ��
	   \param    int index 
	   \param    bool bTextChange �����Ƿ�ı�
	   \return   void 
	 ************************************/
	virtual void SelectItem(int index, bool bTextChange);

	/*!
	   \brief    ��ȡ����ͼ���λ��
	   \return   RECT 
	 ************************************/
	RECT GetArrowRect();

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