/************************************************************************** 
    *  @Copyright (c) 2017, ChenCY, All rights reserved. 
 
    *  @file     : UILayoutButton.h 
    *  @version  : ver 1.0 
 
    *  @author   : ChenCY 
    *  @date     : 2017/08/07 11:40 
    *  @brief    : 带子布局的按钮控件 
**************************************************************************/
#pragma once

class CUI_API CUILayoutButton : public CLayoutUI
{

    UI_OBJECT_DECLARE(CButtonUI, _T("LayoutButton"))
public:
    CUILayoutButton();
    virtual ~CUILayoutButton();

    // 继承函数
    virtual bool Activate();
    virtual bool Event(TEventUI& event);
};