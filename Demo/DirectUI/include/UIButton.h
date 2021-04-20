/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-22       xqb
*********************************************************************/
#pragma once
#ifndef __DIRECTUI_BUTTON_H_
#define __DIRECTUI_BUTTON_H_

/*!
    \brief    按钮控件
*****************************************/
class DUI_API CButtonUI : public CControlUI
{
	UI_OBJECT_DECLARE(CButtonUI, _T("Button"))
public:
	CButtonUI();
	virtual ~CButtonUI();

	// 继承函数
	virtual bool Activate();
	virtual bool Event(TEventUI& event);
};


#endif