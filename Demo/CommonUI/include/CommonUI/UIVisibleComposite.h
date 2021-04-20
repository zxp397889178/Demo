/*********************************************************************
*       Copyright (C) 2018,应用软件开发
*********************************************************************
*   Author:     xsm
*   Created:    2018/04/13
*   Describe:   当布局可见性被修改时，通过Event事件实时获取该状态       
*********************************************************************/
#pragma once

class CUI_API CVisibleComposite:
    public CCompositeUI
{
    UI_OBJECT_DECLARE(CVisibleComposite, _T("LayoutVisible"));
public:
    CVisibleComposite();
    virtual ~CVisibleComposite();
    CEventSource OnVisible;
	virtual void SetVisible(bool bVisible);
    virtual void SetInternVisible(bool bVisible);
};