#pragma once


enum EFigurePicNotify
{
	EN_FIGURE_PIC_CHANGE = UINOTIFY__USER	+ 1,
};

class CUI_API CFigurePicUI : public CControlUI, public ITrackerBoxEvent
{
	UI_OBJECT_DECLARE(CFigurePicUI, _T("FigurePic"))
public:
	CFigurePicUI(void);
	~CFigurePicUI(void);
public:

	virtual void		Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual bool		Event(TEventUI& event);
	virtual void		InvalidateRect(CONST RECT *lpRect, bool bErase);
	virtual void		Init();

public:
	RECT				GetTrackerRectInPic();
	void				SetDrawRect(RECT rcInPic);
	HBITMAP				CreateHBitBmp();
	void				SetBoxMinSize(int cx, int cy){ m_box.SetMinSize(cx, cy); }
	void				SetBoxRatio(float fRatio){ m_box.SetRatio(fRatio); }
	void				SetImage(LPCTSTR lpszImage);
	void				SetImage(ImageObj* pImageRender);
	ImageObj*			GetImage();
	LPCTSTR				GetImageFileName();	
	void				RotatePhoto90(bool bClockWise=true);
	RECT&				GetRcInPic();
	void				DrawShadow(HDC hDC, RECT& rect);
	void				DrawZoomBox(RECT& rect);
	void				SetBackGroundRect(RECT& rect);

protected:
	virtual void		SetAttribute(const WCHAR* pszName, const WCHAR* pszValue);

protected:
	CTrackerBox			m_box; //ΩÿÕºøÚ
	bool				m_bTracker;
	RECT				m_rcInPic;
	bool				m_bOnLoadImg;
	RECT				m_rcPaintLast;
	UINT				m_uX1;
	UINT				m_uY1;
	UINT				m_uX2;
	UINT				m_uY2;
	RECT				m_rcBackGround; /// “ı”∞±≥æ∞«¯”Ú¥Û–°
};
