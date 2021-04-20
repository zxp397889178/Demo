#pragma once

//{{

#ifndef DIRECTUI_STATICLIB
#ifdef DIRECTUI_EXPORTS
#define DUI_API __declspec(dllexport)
#else
#define DUI_API __declspec(dllimport)
#endif
#else
#define DUI_API 
#endif

#ifndef _GDIPLUS_SUPPORT
#define _GDIPLUS_SUPPORT //是否支持GDI+
#endif

#ifndef _COLOR_ALPHA_SUPPORT
#define _COLOR_ALPHA_SUPPORT // 颜色是否支持透明
#endif

#include <tchar.h>
#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;
#include <comdef.h>

#include <windows.h>
#include "UIDef.h"
#include "DPI.h"
#include "UIView.h"
#include "Utility.h"
#include "UIDelegate.h"
#include "UITimer.h"
#include "ImageLib.h"
#include "DataObjectImpl.h"
#include "ImageObj.h"
#include "FontObj.h"
#include "ColorObj.h"
#include "CursorObj.h"
#include "RenderObj.h"
#include "StyleObj.h"
#include "Action.h"
#include "ViewAnimation.h"
#include "WindowAnimation.h"
#include "ActionManager.h"
#include "UIControl.h"
#include "UIScrollBar.h"
#include "UIContainer.h"
#include "UILayout.h"
#include "UITransition.h"
#include "WindowBase.h"
#include "UIWindow.h"
#include "UILabel.h"
#include "UIButton.h"
#include "UICheckBox.h"
#include "UIRichEdit.h"
#include "UIEdit.h"

#include "UIList.h"
#include "UIComboBox.h"
#include "UIMessageBox.h"
#include "UIAnimation.h"
#include "UIRadio.h"
#include "UISlider.h"
#include "UISliderV2.h"
#include "UISplitter.h"
#include "UITab.h"
#include "UITree.h"
#include "UIMenu.h"
#include "UIWebbrowser.h"
#include "UIFlash.h"
#include "UIProgressBar.h"
#include "UIComposite.h"
#include "UIControlWnd.h"
#include "UIToolTip.h"
#include "UIHtmlLabel.h"
#include "RdbParser.h"

//}}