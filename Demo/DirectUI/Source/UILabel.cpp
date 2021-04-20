/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UILabel.h"

CLabelUI::CLabelUI()
{
	SetMouseEnabled(false);
	SetEnabled(false);
	SetAnchor(UIANCHOR_LEFTTOP);
	SetStyle(_T("Label"));
}
