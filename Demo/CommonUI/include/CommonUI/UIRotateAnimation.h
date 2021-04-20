#pragma once

class CUI_API CRotateAnimationUI : public CAnimationCtrlUI
{
	UI_OBJECT_DECLARE(CRotateAnimationUI, _T("RotateAnimation"))
public:
	CRotateAnimationUI();
	virtual ~CRotateAnimationUI();

protected:
	virtual ViewAnimation*
				AllocViewAnimation();
	virtual int	
				GetDefaultFrameCount();

};
