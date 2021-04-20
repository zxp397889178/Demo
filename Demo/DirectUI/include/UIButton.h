/*********************************************************************
 *       Copyright (C) 2010,Ӧ���������
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-22       xqb
*********************************************************************/
#pragma once
#ifndef __DIRECTUI_BUTTON_H_
#define __DIRECTUI_BUTTON_H_

/*!
    \brief    ��ť�ؼ�
*****************************************/
class DUI_API CButtonUI : public CControlUI
{
	UI_OBJECT_DECLARE(CButtonUI, _T("Button"))
public:
	CButtonUI();
	virtual ~CButtonUI();

	// �̳к���
	virtual bool Activate();
	virtual bool Event(TEventUI& event);
};


#endif