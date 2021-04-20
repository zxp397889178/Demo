#pragma once

//{{
#define MESSAGEBOX_CLOSE_ALL	9898
#include <memory>
#
#ifndef COMMONUI_STATICLIB
#ifdef COMMONUI_EXPORTS
#define CUI_API __declspec(dllexport)
#else
#define CUI_API __declspec(dllimport)
#define CUI_INCLUDE
#endif
#else
#define CUI_API 
#define CUI_INCLUDE
#endif

#ifdef CUI_INCLUDE

#include "CommonUI/UIDragImage.h"
#include "CommonUI/UITrackerBox.h"
#include "CommonUI/UIFigurePic.h"
#include "CommonUI/UIClassifyTabLayout.h"
#include "CommonUI/UIEditEx.h"
#include "CommonUI/UIEditLayout.h"
#include "CommonUI/UIAnimationCtrl.h"
#include "CommonUI/UIAutoSizeButton.h"
#include "CommonUI/UILoadingButton.h"
#include "CommonUI/UIMaskButton.h"
#include "CommonUI/UIRotateAnimation.h"
#include "CommonUI/UIShadowOptionLayout.h"
#include "CommonUI/UIRoundProgressBar.h"
#include "CommonUI/Toast.h"
#include "CommonUI/IconToast.h"
#include "CommonUI/UIAutoSizeCheckBox.h"
#include "CommonUI/UIMarqueeLabel.h"
#include "CommonUI/UIProgressBarEx.h"
#include "CommonUI/DecorateBaseWindow.h"
#include "CommonUI/DecorateBaseLayout.h"
#include "CommonUI/WindowPtrHolder.h"
#include "CommonUI/DecorateBaseButton.h"
#include "CommonUI/WindowPtrRef.h"
#include "CommonUI/UIIndentifyWnd.h"
#include "CommonUI/UIScreenToolGroup.h"
#include "CommonUI/UIAssociateVisibleButton.h"
#include "CommonUI/UIWrapContentLayout.h"
#include "CommonUI/UIHelper.hpp"
#include "CommonUI/UILayoutTwinkle.h"
#include "CommonUI/UIPosAnimationButton.h"
#include "CommonUI/UILayerControl.h"
#include "CommonUI/UIInteractOptionLayout.h"
#include "CommonUI/UIMediator.h"
#include "CommonUI/UISuspended.h"
#include "CommonUI/UILayoutButton.h"
#include "CommonUI/UIFadeLayout.h"
#include "CommonUI/UIVisibleComposite.h"
#include "CommonUI/UIGesture.h"
#include "CommonUI/UIAnimationButton.h"
#include "CommonUI/UINewComboBoxEx.h"
#include "CommonUI/UISearchEdit.h"
#include "CommonUI/WndDecorate.h"
#include "CommonUI/ControlDecorate.h"
#endif

//}}