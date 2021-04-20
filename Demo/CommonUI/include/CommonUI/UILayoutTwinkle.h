/************************************************************************** 
    *  @Copyright (c) 2017, ChenCY, All rights reserved. 
 
    *  @file     : UILayoutButton.h 
    *  @version  : ver 1.0 
 
    *  @author   : ChenCY 
    *  @date     : 2017/08/07 11:40 
    *  @brief    : 带子布局的按钮控件 
**************************************************************************/
#pragma once

class CUI_API CUILayoutTwinkle : public CLayoutUI
{

	UI_OBJECT_DECLARE(CUILayoutTwinkle, _T("LayoutTwinkle"))
public:
	CUILayoutTwinkle();
	virtual ~CUILayoutTwinkle();

    // 继承函数
	virtual void		Render(IRenderDC* pRenderDC, RECT& rcPaint) override;
	virtual void		StartTwinkleRect();
	virtual void		StopTwinkleRect();
	virtual void		ShowDocRect(bool bShow);
	virtual void		Init()override;
protected:
	bool				Event(TEventUI& event);
	vector <ImageStyle *> GetVisibleOperImages();
	void				ShowImageStyles(bool bShow);
private:
	bool				m_bShowDotRect = false;
	bool				m_bColorChange = false;
	static const int	N_TIMER_ID;
};