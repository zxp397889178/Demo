#pragma once


class CUI_API CMaskButtonUI :
	public CButtonUI
{
	UI_OBJECT_DECLARE(CMaskButtonUI, _T("MaskButton"))
public:
	CMaskButtonUI(void);
	~CMaskButtonUI(void);

public:
	ImageStyle* GetImageStyle();
	ImageStyle* GetMaskStyle();

protected:
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
private:
	tstring m_strImageStyleName;
	ImageStyle* m_pImageStyle;

	tstring m_strMaskStyleName;
	ImageStyle* m_pMaskStyle;


};

