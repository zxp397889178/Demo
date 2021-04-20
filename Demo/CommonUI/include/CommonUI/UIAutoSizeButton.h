#pragma once

/*!
\brief    �Զ�������ť�ؼ�
*****************************************/
class CUI_API CAutoSizeButtonUI : public CButtonUI
{
	UI_OBJECT_DECLARE(CAutoSizeButtonUI, _T("AutoSizeButton"))
public:
	//{{
	CAutoSizeButtonUI();
	//}}

	// �̳к���
	virtual void SetRect(RECT& rectRegion);
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

private:
	bool         m_bAutoSizeEvenHideTxt = true;

};