#include "stdafx.h"
#include "UIAnimationCtrl.h"
#include "UIRotateAnimation.h"


CRotateAnimationUI::CRotateAnimationUI()
{

}

CRotateAnimationUI::~CRotateAnimationUI()
{

}

ViewAnimation* CRotateAnimationUI::AllocViewAnimation()
{
    return new_nothrow ImageRotateAnimation();
}

int CRotateAnimationUI::GetDefaultFrameCount()
{
	return 16;
}

