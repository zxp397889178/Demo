#pragma once

/*!
    \brief    ÎÄ×ÖÑùÊ½
*****************************************/
class CUI_API ShadowTextStyle :public TextStyle
{
	UI_STYLE_DECLARE(ShadowTextStyle, _T("ShadowText"))

public:
	ShadowTextStyle(CControlUI* pOwner);
	virtual ~ShadowTextStyle();

	virtual void	SetShadowTextColor(DWORD dwColor, int nIndex = 0);
	virtual DWORD	GetShadowTextColor(int nIndex = 0);

protected:
	virtual void	Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);
	
private:
	ColorObjEx*		m_pArrShadowColorObj[5];
};

