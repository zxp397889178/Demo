/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-22       xqb
*********************************************************************/
#pragma once

/*!
    \brief    单选按钮
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
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue); //重载基类

protected:
	virtual bool Event(TEventUI& event);

private:
	tstring m_strGroupName;
	//}}
};
