/************************************************************************** 
    *  @Copyright (c) 2017, LBQ, All rights reserved. 
 
    *  @file     : WndDecorate.h 
    *  @version  : ver 1.0 
 
    *  @author   : LBQ 
    *  @date     : 2017/07/25 14:28 
    *  @brief    : 所有修饰窗口功能类的基类
**************************************************************************/
#pragma once

class CUI_API CWndDecorate
{
public:
	CWndDecorate();
	virtual ~CWndDecorate(void);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void	OnCreate(); 
	void SetWindowBase(CWindowUI *pWindow);
protected:
	CWindowUI * m_pWindow;
};