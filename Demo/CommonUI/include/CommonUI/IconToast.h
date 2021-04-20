/************************************************************************** 
    *  @Copyright (c) 2017, ChenCY, All rights reserved. 
 
    *  @file     : Toast.h 
    *  @version  : ver 1.0 
 
    *  @author   : ChenCY 
    *  @date     : 2017/07/25 14:28 
    *  @brief    : toast window 
**************************************************************************/

#pragma once

class CUI_API CIconToast : public CLayoutUI
{
	UI_OBJECT_DECLARE(CIconToast, _T("IconToastLayout"))
public:

	CIconToast();

	virtual ~CIconToast();

    virtual void Init();

	virtual bool Event(TEventUI& event);



	void		 SetToastMsg(LPCTSTR lpszMsg, LPCTSTR lpszIcon = _T(""), UINT uElapse = 2000);
	bool         SetToastMsgShow(LPCTSTR lpszMsg, LPCTSTR lpszIcon = _T(""));

};