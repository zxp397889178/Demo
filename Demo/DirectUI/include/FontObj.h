#pragma once

//#define DT_CALC_NOSPACE			 0x10000000 //GDI+计算字体长度时会在末尾加上一个空格，导致计算不准确，加上这个标志可以去掉空格
#define DT_CALC_SINGLELINE		 DT_SINGLELINE| DT_LEFT_EX|DT_NOPREFIX  //计算单行
#define DT_CALC_MULTILINE		 DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT_EX | DT_NOPREFIX // 计算多行
#define DT_DRAW_SINGLELINE		 DT_SINGLELINE|DT_VCENTER|DT_LEFT_EX|DT_NOPREFIX   //画单行
#define DT_DRAW_MULTILINE	     DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT_EX|DT_NOPREFIX   //画多行

class DUI_API FontObj : public BaseObject
{
	//{{
public:
	FontObj();
	FontObj(FontObj& obj);
	virtual ~FontObj();
	//}}
public:
	static bool IsExistFontName(LPCTSTR lpszFontName);
	// 计算字符串宽度高度
	void CalcText(HDC hdc, RECT& rc, LPCTSTR lpszText, UINT format = DT_CALC_SINGLELINE, UITYPE_FONT nFontType = UIFONT_DEFAULT,  int c = -1);
	// 输出单行文字
	void TextOut(IRenderDC* pRenderDC,  int x, int y, LPCTSTR lpszText, int c, DWORD dwColor, DWORD* pBkColor = NULL, UITYPE_FONT nFontType = UIFONT_DEFAULT);
	// 多行文字
	void DrawText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor = 0, UINT format = DT_DRAW_SINGLELINE, UITYPE_FONTEFFECT nFontEffect = UIFONTEFFECT_NORMAL, UITYPE_FONT nFontType = UIFONT_DEFAULT);
	//光晕效果文字显示，发光范围uiShadowSize，一般设置12-32,
	void DrawShadowText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwFontColor, UINT format, DWORD dwShadowColor, UINT uiShadowSize);

	HFONT GetFont();
	void  GetLogFont(LPLOGFONT lpLogFont);
	void  SetLogFont(const LPLOGFONT lpLogFont);
	TEXTMETRIC GetTextMetric(HDC hDC);
	// 获取GDI+字体
	Gdiplus::Font* GetGdiplusFont();

	void SetFaceName(LPCTSTR lpszFace);
	LPCTSTR GetFaceName();
	bool IsUsedDefaultFaceName(); // 如果使用默认字体，字体将随着默认字体改变而改变
	void SetUsedDefaultFaceName(bool bUsedDefaultFaceName); 

	void SetFontSize(int nSize);
	int GetHeight();

	void SetBold(bool bBold);
	bool IsBold();

	void SetUnderline(bool bUnderline);
	bool IsUnderline();

	void SetItalic(bool bItalic);
	bool IsItalic();

	bool IsStrikeOut();
	void SetStrikeOut(bool bStrikeOut);

	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	bool IsCached();
	void SetCached(bool bCached);

	void Release();

	void RebuildFont();
	//{{
private:

	void DrawGrowText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format, UITYPE_FONT nFontType, UITYPE_FONTEFFECT nFontEffect);
	void DrawGDIPlusText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format, UITYPE_FONTEFFECT nFontEffect);
	LOGFONT m_lfFont;
	HFONT	m_hFont;
	/// TextRenderingHint for gdi+ render
	int m_nTextRenderingHint;
	bool m_bCached;
	Gdiplus::Font* m_pGdiplusFont;
	bool m_bUsedDefalultFontFace;
	//}}
};
