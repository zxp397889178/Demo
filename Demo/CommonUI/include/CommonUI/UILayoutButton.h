/************************************************************************** 
    *  @Copyright (c) 2017, ChenCY, All rights reserved. 
 
    *  @file     : UILayoutButton.h 
    *  @version  : ver 1.0 
 
    *  @author   : ChenCY 
    *  @date     : 2017/08/07 11:40 
    *  @brief    : ���Ӳ��ֵİ�ť�ؼ� 
**************************************************************************/
#pragma once

class CUI_API CUILayoutButton : public CLayoutUI
{

    UI_OBJECT_DECLARE(CButtonUI, _T("LayoutButton"))
public:
    CUILayoutButton();
    virtual ~CUILayoutButton();

    // �̳к���
    virtual bool Activate();
    virtual bool Event(TEventUI& event);
};