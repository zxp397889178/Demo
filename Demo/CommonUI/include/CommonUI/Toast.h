/************************************************************************** 
    *  @Copyright (c) 2017, ChenCY, All rights reserved. 
 
    *  @file     : Toast.h 
    *  @version  : ver 1.0 
 
    *  @author   : ChenCY 
    *  @date     : 2017/07/25 14:28 
    *  @brief    : toast window 
**************************************************************************/

#pragma once

class CUI_API CToast : public CLayoutUI
{
    UI_OBJECT_DECLARE(CToast, _T("ToastLayout"))
public:

    CToast();

    virtual ~CToast();

    virtual void Init();

    void    SetToastMsg(const tstring& wstrMsg);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

    virtual bool Event(TEventUI& event);

    virtual void SetRect(RECT& rectRegion);

	int     m_iMinWidth;

	int     m_iMinHeight;

	int     m_iMaxWidth;

	int     m_iMaxHeight;
};