#ifndef __DIRECTUI_STYLE_H_
#define __DIRECTUI_STYLE_H_

/*!
    \brief    样式基类
*****************************************/
class DUI_API StyleObj : public CViewUI
{
	UI_STYLE_DECLARE(StyleObj, _T("Base"))
public:
	StyleObj(CControlUI* pOwner);
	virtual ~StyleObj(void);

public:
	void SetRect(RECT& rectRegion);

	bool HitTest(POINT point);

	//继承函数
	CControlUI*	GetOwner();
	virtual void Invalidate(bool bUpdateWindow = false);
	virtual bool Resize();

	virtual void ReleaseResource();
	virtual void OnLanguageChange();
	virtual void SetVisible(bool bVisible);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);
	virtual void Copy(StyleObj* pStyleObj);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint){};
	
protected:
	UITYPE_STATE GetDrawState(); // 如果样式有设置状态，以设置的状态为主，否则从控件获取状态
	int GetDrawAlpha(); // 如果样式有设置透明值，以设置的值为主，否则从控件获取透明值

	bool MaskState(UITYPE_STATE typeState);
	UITYPE_STATE GetMaskState();
private:
	CControlUI* m_pOwner;
	UINT m_uMaskState;
	//}}
};

/*!
    \brief    图片样式
*****************************************/
class DUI_API ImageStyle:public StyleObj
{
	UI_STYLE_DECLARE(ImageStyle, _T("Image"))
public:
	ImageStyle(CControlUI* pOwner);
	virtual ~ImageStyle(void);

public:
	ImageObj* GetImageObj();
	LPCTSTR	  GetImageFile();
	void SetImage(LPCTSTR lpszPath);
	void SetImage(LPVOID lpData, long nSize);
	void SetImage(ImageObj* pImg);

	bool LoadImage();
	bool IsImageValid();
	bool IsImageCache();
	
	int  GetImgSeq() const;
	void SetImgSeq(int val);
	void SetAngle(bool bAngle, int nAngle = 0);
	void SetRoate(bool bRoate, int nAngle = 0);
	void SetPaintMode(UITYPE_PAINTMODE PaintMode);
	UITYPE_PAINTMODE GetPaintMode();
	PaintParams* GetPaintParams();

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);
	virtual void ReleaseResource();
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void Copy(StyleObj* pStyleObj);

protected:
	int GetDrawImgSeq();  // 如果样式有SetImgSeq，以设置的值为主，否则从控件状态或imqseq
private:
	ImageObj* m_pImageObj;
	tstring m_strImageFile;
	uint8	m_arrStateImgSeq[5];
	int		m_nImgSeq;
	bool	m_bCache;
	UITYPE_PAINTMODE	m_ePaintMode;
	UITYPE_IMAGE		m_eImageType;
	PaintParams			m_paintParams;
	//}}
};

/*!
    \brief    文字样式
*****************************************/
class DUI_API TextStyle:public StyleObj
{
	UI_STYLE_DECLARE(TextStyle, _T("Text"))
public:
	TextStyle(CControlUI* pOwner);
	~TextStyle();

public:
	FontObj* GetFontObj();
	void SetFontObj(FontObj* pFontObj);

	UINT GetAlign();
	void SetAlign(UINT uAlignStyle);

	void SetText(LPCTSTR lpszText);
	LPCTSTR GetText();

	UITYPE_FONTEFFECT GetFontEffect();
	void SetFontEffect(UITYPE_FONTEFFECT eFontEffect);

	UITYPE_FONT GetFontType();
	void SetFontType(UITYPE_FONT eFontType);

	void SetTextColor(DWORD dwColor, int nIndex = 0);
	DWORD GetTextColor(int nIndex = 0);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue); //重载基类
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void ReleaseResource();
	virtual void OnLanguageChange();
	virtual void Copy(StyleObj* pStyleObj);

protected:
	LPCTSTR GetDrawText();  // 如果样式有设置文本，以设置的值为主，否则从控件获取文本
private:
	FontObj*  m_pFontObj;
	UINT   m_uAlignStyle;
	LPTSTR m_lpszText;
	LPTSTR m_lpszTextKey;
	UITYPE_FONT		m_eFontType;
	UITYPE_FONTEFFECT m_nFontEffect;              //文字样式特效索引，配置参见xml
	ColorObjEx* m_pArrColorObj[5];
};

/*!
    \brief    填充样式
*****************************************/
class DUI_API FillStyle:public StyleObj
{
	UI_STYLE_DECLARE(FillStyle, _T("Fill"))
public:
	FillStyle(CControlUI* pOwner);
	~FillStyle();

public:
	void SetFillColor(DWORD dwColor);
	DWORD GetFillColor();

	void SetBorderColor(DWORD dwColor);
	DWORD GetBorderColor();

	void SetBorderWidth(int lineWidth);
	int  GetBorderWidth();

	void SetCornerRadius(int cornerRadius);
	int  GetCornerRadius();

	void SetGradient(bool bGradient);
	bool IsGradient();

	void SetGradientVert(bool bVert);
	bool IsGradientVert();

	void SetGradientColor1(DWORD dwColor1);
	DWORD GetGradientColor1();

	void SetGradientColor2(DWORD dwColor2);
	DWORD GetGradientColor2();

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void Copy(StyleObj* pStyleObj);

private:
	ColorObjEx* m_pFillColor; 
	ColorObjEx* m_pBorderColor;
	ColorObjEx* m_pGradientColor1;
	ColorObjEx* m_pGradientColor2;
	
	int m_nBorderLineWidth;
	int m_nCornerRadius;
	bool m_bGradient;
	bool m_bGradientVert;
};

#define RectStyle FillStyle

/*!
    \brief    
*****************************************/

class DUI_API LineStyle : public StyleObj
{
	UI_STYLE_DECLARE(LineStyle, _T("Line"))
public:
	LineStyle(CControlUI* pOwner);
	~LineStyle();

public:
	void SetStyle(LineStyleType lineStyle);
	LineStyleType GetStyle();

	void SetWidth(UINT LineWidth);
	UINT GetWidth();

	void SetColor(DWORD lineColor);
	DWORD GetColor();

	void SetVert(bool bVert);
	bool IsVert();

	virtual void SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue );
	virtual void Render( IRenderDC* pRenderDC, RECT& rcPaint );
	virtual void Copy(StyleObj* pStyleObj);
private:
	bool m_bVert;
	LineStyleType m_uLineStyle;
	UINT m_uLineWidth;
	ColorObjEx* m_pColorObj;
	//}}
};

#endif //  __DIRECTUI_STYLE_H_