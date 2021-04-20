#pragma once

/*!
\brief    自动增长按钮控件
*****************************************/
class CUI_API CAutoSizeButtonUI : public CButtonUI
{
	UI_OBJECT_DECLARE(CAutoSizeButtonUI, _T("AutoSizeButton"))
public:
	//{{
	CAutoSizeButtonUI();
	//}}

	// 继承函数
	virtual void SetRect(RECT& rectRegion);
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

private:
	bool         m_bAutoSizeEvenHideTxt = true;

};