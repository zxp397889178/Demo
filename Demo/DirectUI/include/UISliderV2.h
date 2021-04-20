/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************
*   Date             Name                 Description
*   2010-12-2		 hanzp				  Create.
*********************************************************************/
#pragma once

/*!
    \brief    滑条控件
*****************************************/
class DUI_API CSliderV2UI:public CControlUI
{
	UI_OBJECT_DECLARE(CSliderV2UI, _T("SliderBarV2"))
public:
	CSliderV2UI();
	virtual ~CSliderV2UI();
	//}}

	enum
	{
		enDefaultTimerID = 10,
		enDefaultTimerElapse = 100
	};

 	int				GetRange();
 	void			SetRange(int nRange,bool bRedraw = true);
 	int				GetPos();
 	void			SetPos(int nPos,bool bRedraw = true);
	int				GetForePos();
	void			SetForePos(int nPos, bool bRedraw = true);
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	//{{
protected:
	virtual bool	Event(TEventUI& event);
	virtual void	Render(IRenderDC* pRenderDC, RECT& rcPaint);
	void			CalcRect(POINT ptMouse);
private:
	ImageStyle* GetThumbStyle();
	ImageStyle* GetForeStyle();
	ImageStyle* m_pThumbStyle;
	ImageStyle* m_pForeStyle;

	bool			m_bHorizontal;
	bool			m_bMovingScroll;
	bool			m_bProcess;
	int				m_nRange;
	int				m_nPos;
	RECT			m_rcThumb;
	RECT			m_rcImageRender;
	RECT			m_rcPosImageRender;
	//}}
};
