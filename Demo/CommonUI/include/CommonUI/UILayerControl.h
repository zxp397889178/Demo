/************************************************************************** 
    *  @Copyright (c) 2017, LINBQ, All rights reserved. 
 
    *  @file     : UILayerControl.h 
    *  @version  : ver 1.0 
 
    *  @author   : LINBQ
    *  @date     : 2017/09/13 17:46 
    *  @brief    : 透明透传的控件
**************************************************************************/

#pragma once

class CUI_API CLayerControl : public CControlUI
{
	UI_OBJECT_DECLARE(CLayerControl, _T("LayerControl"))
protected:
	void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)override;
private:
	DWORD m_dColor = 0;
};
