/************************************************************************** 
    *  @Copyright (c) 2017, LBQ, All rights reserved. 
 
    *  @file     : ControlDecorate.h 
    *  @version  : ver 1.0 
 
    *  @author   : LBQ 
    *  @date     : 2017/07/25 14:28 
    *  @brief    : 所有修饰控件功能类的基类
**************************************************************************/
#pragma once

class CUI_API CControlDecorate
{
public:
	CControlDecorate();
	virtual ~CControlDecorate(void);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void	Init(); 
	void SetLayoutBase(CControlUI *pControl);
	virtual void SetVisible(bool bShow);
	virtual void SetInternVisible(bool bShow);
	virtual void SetRect(RECT& rectRegion);
	virtual void SetEnabled(bool bEnable);
	virtual bool Activate();
protected:
	CControlUI * m_pControl;
};