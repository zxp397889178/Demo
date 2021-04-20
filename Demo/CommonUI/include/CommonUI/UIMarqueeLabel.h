#pragma once

//跑马灯label控件，支持单行

class CUI_API CMarqueeLabelUI :
	public CLabelUI,
	public IActionListener
{
	UI_OBJECT_DECLARE(CMarqueeLabelUI, _T("MarqueeLabel"))
public:
	CMarqueeLabelUI();
	virtual ~CMarqueeLabelUI();

	void         ShowAnimation();
	void         StopAnimation();
protected:

	virtual void SetRect(RECT& rect);

	virtual void OnActionStateChanged(CAction* pAction);

	virtual void SetVisible(bool bVisible);

	virtual void SetInternVisible(bool bVisible);

	virtual void OnSize(const RECT& rectParent);

	void         InitPosAnimation();

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

protected:

	RECT  m_rectTxt;
	int   m_nTxtWidth;
	int   m_nLastTxtWidth;
	PosChangeAnimation* m_pAlphaChange;
};
