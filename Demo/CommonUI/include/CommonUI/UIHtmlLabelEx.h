/************************************************************************** 
    *  @Copyright (c) 2017, ChenCY, All rights reserved. 
 
    *  @file     : UIHtmlLabelEx.h 
    *  @version  : ver 1.0 
 
    *  @author   : FengC 
    *  @date     : 2017/10/18 11:40 
    *  @brief    : 带子布局的按钮控件 
**************************************************************************/
#pragma once

class CUI_API CHtmlLabelExUI : public CHtmlLabelUI
{

	UI_OBJECT_DECLARE(CHtmlLabelExUI, _T("HtmlLabelEx"))
public:
	CHtmlLabelExUI();
	virtual ~CHtmlLabelExUI();

	int CalcWidth(int nHeight);
};