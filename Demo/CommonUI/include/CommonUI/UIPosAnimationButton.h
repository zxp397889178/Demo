/************************************************************************** 
    *  @Copyright (c) 2017, Linbq, All rights reserved. 
 
    *  @file     : UIAnimationButton.h 
    *  @version  : ver 1.0 
 
    *  @author   : Linbq
    *  @date     : 2017/09/05 15:32
    *  @brief    : 平移效果动画按钮 
**************************************************************************/

#pragma once

class CControlUI;
class CUI_API CUIPosAnimationButton : public CAutoSizeButtonUI
{
public:
	UI_OBJECT_DECLARE(CUIPosAnimationButton, _T("PosAnimationButton"))
	CUIPosAnimationButton();
	~CUIPosAnimationButton();
	virtual void SetVisible(bool bVisible)override;
	virtual	void SetInternVisible(bool bVisible)override;
	void		 StartAnimation();
	void		 StopAnimation();
	virtual void Init()override;
protected:
	PosChangeAnimation*  m_spViewAnimation;
};
