/*********************************************************************
*       Copyright (C) 2010,Ӧ���������
*********************************************************************
*   Date             Name                 Description
*   2010-12-2		 hanzp				  Create.
*********************************************************************/
#pragma once

/*!
    \brief    �����ؼ�
*****************************************/
class DUI_API CSliderUI:public CControlUI
{
	UI_OBJECT_DECLARE(CSliderUI, _T("SliderBar"))
public:
	CSliderUI();
	virtual ~CSliderUI();
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
	bool			m_bHorizontal;
	bool			m_bMovingScroll;
	bool			m_bProcess;
	int				m_nRange;
	int				m_nPos;
	int				m_nForePos;
	RECT			m_rcThumb;
	RECT			m_rcImageRender;
	RECT			m_rcForeImage;
	RECT			m_rcPosImageRender;
	//}}
};
