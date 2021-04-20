/*********************************************************************
 *       Copyright (C) 2010,Ӧ���������
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-22       xqb
*********************************************************************/
#pragma once
#ifndef __DIRECTUI_CHECKBOX_H_
#define __DIRECTUI_CHECKBOX_H_

/*!
    \brief    ��ѡ��ؼ�
*****************************************/
class DUI_API CCheckBoxUI : public CButtonUI
{
	UI_OBJECT_DECLARE(CCheckBoxUI, _T("CheckBox"))
public:
	//{{
	CCheckBoxUI();


	/*!
	   \brief    �Ƿ�ѡ��
	   \return   bool 
	 ************************************/
	bool IsChecked();

	/*!
	   \brief    ѡ��
	   \param    bool bChecked 
	   \param    UINT uFlags UIITEMFLAG_CHECKED�Ƿ�ѡ��/UIITEMFLAG_NOTIFY�Ƿ�֪ͨ
	   \return   void 
	 ************************************/
	virtual void SetChecked(bool bChecked, UINT uFlags = UIITEMFLAG_CHECKED|UIITEMFLAG_NOTIFY);

	/*!
	   \brief    ����ȫѡ��̬
	   \param    bool bIndef 
	   \return   void 
	 ************************************/

	bool Indef() const;
	/*!
	   \brief    ����ȫѡ��̬
	   \param    bool val 
	   \return   void 
	 ************************************/
	void Indef(bool val);

	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue); //���ػ���
protected:
	/// �̳к���
	virtual bool Event(TEventUI& event);

	
	//{{
private:
	bool m_bPartialChecked;
	//}}
};

#endif //__DIRECTUI_CHECKBOX_H_