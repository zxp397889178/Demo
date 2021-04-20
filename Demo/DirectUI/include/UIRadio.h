/*********************************************************************
 *       Copyright (C) 2010,Ӧ���������
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-22       xqb
*********************************************************************/
#pragma once

/*!
    \brief    ��ѡ��ť
*****************************************/
class DUI_API CRadioUI:public CCheckBoxUI
{
	//{{
	UI_OBJECT_DECLARE(CRadioUI, _T("Radio"))
public:
	CRadioUI();
	virtual ~CRadioUI();

public:
	void SetChecked(bool bChecked, UINT uFlags = UIITEMFLAG_CHECKED|UIITEMFLAG_NOTIFY);
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue); //���ػ���

protected:
	virtual bool Event(TEventUI& event);

private:
	tstring m_strGroupName;
	//}}
};
