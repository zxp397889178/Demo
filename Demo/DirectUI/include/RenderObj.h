/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-22       xqb
*********************************************************************/
#pragma once

//namespace dui{

class  DUI_API ClipObj
{
	//{{
public:
	ClipObj();
    ~ClipObj();
public:
    static void GenerateClip(HDC hDC, RECT& rcItem, ClipObj& clip);
	static void GenerateClipWithAnd(HDC hDC, RECT& rcItem, ClipObj& clip);
	static void GenerateClipWithPath(HDC hDC, RECT& rcItem, ClipObj& clip);
    static void UseOldClipBegin(HDC hDC, ClipObj& clip);
    static void UseOldClipEnd(HDC hDC, ClipObj& clip);

private:
	HDC hDC;
	HRGN hRgn;
	HRGN hOldRgn;
};


//////////////////////////////////////////////////////////////////////
class DUI_API DibObj
{
	//{{
public:
	DibObj();
	virtual ~DibObj(void);
	//}}
public:
	BOOL Create(HDC hdc, int cx, int cy, UINT usage = DIB_RGB_COLORS);
	void Clear();
	BOOL IsValid(){return m_hMemDC != NULL;}
	HDC GetSafeHdc(){return m_hMemDC;}
	HBITMAP GetHBitmap(){return m_hMemBmp;}
	BYTE * GetBits(){return m_pBits;}
	int GetSize();
	int GetWidth();
	int GetHeight();
	void Fill(DWORD dwColor, DWORD dwAlpha = 255);
	void Fill(RECT& rect, COLORREF dwColor, DWORD dwAlpha);
	void DoAlpha(RECT &rect);
	HBITMAP Detach();
	//{{
private:
	HDC m_hMemDC;
	HBITMAP m_hMemBmp;
	HBITMAP m_hOldBmp;
	BITMAPINFOHEADER m_bmpHeader;
	BYTE *m_pBits;
	//}}
};
////////////////////////////////////////////////////////////
struct DUI_API PaintParams
{
	//{{
	UITYPE_SMOOTHING eSmoothingMode;
	bool bAlpha;
	int nAlpha;
	int nActiveFrame;//针对gif
	bool bHole;
	int nAngle;
	int nAngleHeight;
	bool bAngle;
	int nRotateAngle;
	bool bRotate;
	bool bEnableHDPI;
	int eGDIPLUSWarpMode;
	PaintParams()
	{
		eSmoothingMode = UISMOOTHING_HighSpeed;
		eGDIPLUSWarpMode = WrapModeClamp;
		bAlpha = true;
		bHole = false;
		nAlpha = 255;
		nActiveFrame = 0;
		bAngle = false;
		nAngle = 0;
		nAngleHeight = 0;
		nRotateAngle = 0;
		bRotate = false;
		bEnableHDPI = true;
		eGDIPLUSWarpMode = -1;
	}
	//}}
};

struct DUI_API HtmlPaintParams
{
	int nLineNum;
	int nHeight;
	HtmlPaintParams() : nLineNum(0), nHeight(0)
	{
	}
};


enum LineStyleType
{
	LineStyleSolid = 0,          // 0
	LineStyleDash,           // 1
	LineStyleDot,            // 2
	LineStyleDashDot,        // 3
	LineStyleDashDotDot,     // 4
	LineStyleCustom          // 5
};


class  DUI_API IRenderDC
{
public:
	//{{
	IRenderDC();
	~IRenderDC();
	//}}
	void SetDevice(HDC hDC);
	void SetDevice(DibObj* pDibObj);

	HDC GetDC();
	DibObj* GetDibObj();

	void SetUpdateRect(const RECT& rect);
	RECT GetUpdateRect();

	void SetEnableAlpha(bool bEnableAlpha);
	bool GetEnableAlpha();
public:
	void TextOut(FontObj* pFontObj,  int x, int y, LPCTSTR lpszText, int c, DWORD dwColor, DWORD* pBkColor = NULL, UITYPE_FONT nFontType = UIFONT_DEFAULT);

	void DrawText(FontObj* pFontObj, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format, UITYPE_FONTEFFECT nFontEffect = UIFONTEFFECT_NORMAL, UITYPE_FONT nFontType = UIFONT_DEFAULT);
	void DrawShadowText(FontObj* pFontObj, RECT& rc, LPCTSTR lpszText, DWORD dwColor, UINT format, DWORD dwShadowColor, UINT uiShadowSize);
	void DrawHtmlText(FontObj* pFontObjDefault, RECT& rcDst, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, tstring* sLinks, int& nLinkRects, tstring sHoverLink,
		DWORD dwLinkColor, DWORD dwLinkHoverColor, UINT uStyle, HtmlPaintParams &paintParams, bool &bEndEllipsis, UITYPE_FONT nFontType = UIFONT_DEFAULT, int alpha = 255);

	void DrawImage(ImageObj* pImageObj, int xDst, int yDst, int nImageIndex = 0, PaintParams* params = NULL);
	void DrawImageWithDpiScale(ImageObj* pImageObj, int xDst, int yDst, int nImageIndex = 0, PaintParams* params = NULL);
	void DrawImage(ImageObj* pImageObj, RECT& rcDst, int nImageIndex = 0, PaintParams* params = NULL);
	void DrawImage(ImageObj* pImageObj, RECT& rcDst, RECT& rcSrc,  PaintParams* params = NULL);

	void DrawRect(const RECT& rcDst, DWORD color);
	void DrawRoundRect(const RECT& dstRect, DWORD fillColor, DWORD borderColor, int lineWidth, int cornerX, int cornerY);
	void DrawGradientRoundRect(const RECT& dstRect, DWORD gradientColor1, DWORD gradientColor2, DWORD borderColor, int lineWidth, int cornerX, int cornerY, bool bVert);
	void Line(LPPOINT ptBegin, LPPOINT ptEnd, LineStyleType lineStyle, int lineWidth, DWORD lineColor);
	//{{
protected:
	void DrawImageAngle(ImageObj* pImageObj, RECT& rcDst, RECT& rcSrc,  PaintParams* params = NULL);
	void DrawImageRotate(ImageObj* pImageObj, RECT& rcDst, RECT& rcSrc, PaintParams* params = NULL);
	void DrawNinePatchImage(ImageObj* pImageObj, RECT& rcDst, RECT& rectImage, PaintParams* params);
	void DrawThreePatchImageH(ImageObj* pImageObj, RECT& rcDst, RECT& rectImage, PaintParams* params);
	void DrawThreePatchImageV(ImageObj* pImageObj, RECT& rcDst, RECT& rectImage, PaintParams* params);

private:
	DibObj* m_pDibObj;
	HDC m_hDC;
	RECT m_rectUpdate;
	bool m_bEnableAlpha;
	//}}
};

///////////////////////////////////////////////////

//} //dui命名空间

