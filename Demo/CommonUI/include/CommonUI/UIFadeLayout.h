/*********************************************************************
*       Copyright (C) 2018,Ӧ���������
*********************************************************************
*   Author:     xsm
*   Created:    2018/04/13
*   Describe:   ����֧�ֵ�������
*********************************************************************/
#pragma once

class CUI_API CFadeLayout :
    public CLayoutUI
{
	UI_OBJECT_DECLARE(CFadeLayout, _T("FadeLayout"));
public:
	CFadeLayout();
	virtual ~CFadeLayout();
	void StartFade();
	void StopFade();
protected:
	virtual bool Event(TEventUI& event);

private:
	int m_curAlpha;
};