/*********************************************************************
*       Copyright (C) 2018,Ӧ���������
*********************************************************************
*   Author:     xsm
*   Created:    2018/04/13
*   Describe:   �����ֿɼ��Ա��޸�ʱ��ͨ��Event�¼�ʵʱ��ȡ��״̬       
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