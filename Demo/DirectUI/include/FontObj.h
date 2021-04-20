#pragma once

//#define DT_CALC_NOSPACE			 0x10000000 //GDI+�������峤��ʱ����ĩβ����һ���ո񣬵��¼��㲻׼ȷ�����������־����ȥ���ո�
#define DT_CALC_SINGLELINE		 DT_SINGLELINE| DT_LEFT_EX|DT_NOPREFIX  //���㵥��
#define DT_CALC_MULTILINE		 DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT_EX | DT_NOPREFIX // �������
#define DT_DRAW_SINGLELINE		 DT_SINGLELINE|DT_VCENTER|DT_LEFT_EX|DT_NOPREFIX   //������
#define DT_DRAW_MULTILINE	     DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT_EX|DT_NOPREFIX   //������

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
	// �����ַ�����ȸ߶�
	void CalcText(HDC hdc, RECT& rc, LPCTSTR lpszText, UINT format = DT_CALC_SINGLELINE, UITYPE_FONT nFontType = UIFONT_DEFAULT,  int c = -1);
	// �����������
	void TextOut(IRenderDC* pRenderDC,  int x, int y, LPCTSTR lpszText, int c, DWORD dwColor, DWORD* pBkColor = NULL, UITYPE_FONT nFontType = UIFONT_DEFAULT);
	// ��������
	void DrawText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwColor = 0, UINT format = DT_DRAW_SINGLELINE, UITYPE_FONTEFFECT nFontEffect = UIFONTEFFECT_NORMAL, UITYPE_FONT nFontType = UIFONT_DEFAULT);
	//����Ч��������ʾ�����ⷶΧuiShadowSize��һ������12-32,
	void DrawShadowText(IRenderDC* pRenderDC, RECT& rc, LPCTSTR lpszText, DWORD dwFontColor, UINT format, DWORD dwShadowColor, UINT uiShadowSize);

	HFONT GetFont();
	void  GetLogFont(LPLOGFONT lpLogFont);
	void  SetLogFont(const LPLOGFONT lpLogFont);
	TEXTMETRIC GetTextMetric(HDC hDC);
	// ��ȡGDI+����
	Gdiplus::Font* GetGdiplusFont();

	void SetFaceName(LPCTSTR lpszFace);
	LPCTSTR GetFaceName();
	bool IsUsedDefaultFaceName(); // ���ʹ��Ĭ�����壬���彫����Ĭ������ı���ı�
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
