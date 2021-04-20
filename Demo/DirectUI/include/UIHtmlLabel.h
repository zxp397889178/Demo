#pragma once
#ifndef __DIRECTUI_HTMLLABEL_H_
#define __DIRECTUI_HTMLLABEL_H_

/*!
    \brief    使用html描述的文本控件,可以嵌套使用

	\Bold:			{b}text{/b}
	\Color:			{c #xxxxxx}text{/c}
	\Font:			{f x}text{/f}
	\Italic:		{i}text{/i}
	\Underline:		{u}text{/u}
	\Selected:		{s}text{/s}
	\Link:			{a x}text{/a}
	\NewLine		{n}
	\Paragraph:		{p x}text{/p}
	\Raw Text:		{r}text{/r}
	\X Indent:		{x i}
	\Y Indent:		{y i}
	\Image:			{i x y z} 无效
*****************************************/

class DUI_API CHtmlLabelUI:public CControlUI
{
	//{{
	UI_OBJECT_DECLARE(CHtmlLabelUI, _T("HtmlLabel"))
public:
	CHtmlLabelUI();
	~CHtmlLabelUI();
	//}}

	//继承函数
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual bool Event(TEventUI& event);
	int CalcHeight(int nWidth);
	bool IsEndEllipsis();
	void SetTextColor(DWORD dwColor);
	DWORD GetTextColor();
	//{{
protected:
	TextStyle* GetHtmlLabelStyle();
	DWORD GetLinkTextColor();
	DWORD GetLinkOverTextColor();

	enum {MAXLINK = 8};
	int m_iLinks;
	int m_iHoverLink;
	RECT m_rcLinks[MAXLINK];
	tstring m_strLinks[MAXLINK];
	TextStyle* m_pTextStyle;
	tstring m_sHoverLink;
	DWORD m_dwClrLinkHover;
	DWORD m_uAlignStyle;
	bool  m_bEndEllipsis;
	int		m_nLastCalcWidth;
	DibObj	m_dib;
	ColorObjEx* m_pTextColorObj;
	ColorObjEx* m_pLinkTextColorObj;
	ColorObjEx* m_pLinkOverTextColorObj;

	//DWORD m_dwVAlign;
	//DWORD m_dwSingleLine;
	//}}
};

#endif //__DIRECTUI_HTMLLABEL_H_
