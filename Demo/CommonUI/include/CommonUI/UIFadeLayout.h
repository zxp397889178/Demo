/*********************************************************************
*       Copyright (C) 2018,应用软件开发
*********************************************************************
*   Author:     xsm
*   Created:    2018/04/13
*   Describe:   布局支持淡化操作
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