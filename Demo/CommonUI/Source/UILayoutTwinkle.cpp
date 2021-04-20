#include "stdafx.h"

#include "UILayoutTwinkle.h"
#define TWINKLE_CORNER_X 5  
#define TWINKLE_CORNER_Y 5 
#define TWINKLE_OFFSET_X 5  
#define TWINKLE_OFFSET_Y 7  

const int CUILayoutTwinkle::N_TIMER_ID = 10809;
CUILayoutTwinkle::CUILayoutTwinkle()
{
	SetStyle(_T("TwinkleStyle"));
}

CUILayoutTwinkle::~CUILayoutTwinkle()
{
	KillTimer(CUILayoutTwinkle::N_TIMER_ID);
	m_bColorChange = false;
}

void CUILayoutTwinkle::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	__super::Render(pRenderDC, rcPaint);
	//Graphics g(pRenderDC->GetDC());
	//g.SetSmoothingMode(SmoothingModeHighQuality);	
	////创建一个红色的画笔  
	//Pen *pen = new Pen(Gdiplus::Color{ 0xdd, 0x6a, 0x1c }, 2);
	//pen->SetDashStyle(Gdiplus::DashStyle::DashStyleDash);
	////画矩形上面的边  

	//RECT rc = GetRect();
	//rc.left += TWINKLE_OFFSET_X;
	//rc.right -= TWINKLE_OFFSET_X;
	//rc.top += TWINKLE_OFFSET_Y;
	//rc.bottom -= TWINKLE_OFFSET_Y;

	//g.DrawLine(pen, rc.left + TWINKLE_CORNER_X, rc.top, rc.right - TWINKLE_CORNER_X, rc.top);

	////画矩形下面的边  
	//g.DrawLine(pen, rc.left + TWINKLE_CORNER_X, rc.bottom, rc.right - TWINKLE_CORNER_X, rc.bottom);

	////画矩形左面的边  
	//g.DrawLine(pen, rc.left, rc.top + TWINKLE_CORNER_Y, rc.left, rc.bottom - TWINKLE_CORNER_Y);

	////画矩形右面的边  
	//g.DrawLine(pen, rc.right, rc.top + TWINKLE_CORNER_Y, rc.right, rc.bottom - TWINKLE_CORNER_Y);

	////画矩形左上角的圆角  
	//g.DrawArc(pen, rc.left, rc.top, TWINKLE_CORNER_X * 2, TWINKLE_CORNER_Y * 2, 180, 90);

	////画矩形右下角的圆角  
	//g.DrawArc(pen, rc.right - TWINKLE_CORNER_X * 2, rc.bottom - TWINKLE_CORNER_Y * 2, TWINKLE_CORNER_X * 2, TWINKLE_CORNER_Y * 2, 360, 90);

	////画矩形右上角的圆角  
	//g.DrawArc(pen, rc.right - TWINKLE_CORNER_X * 2, rc.top, TWINKLE_CORNER_X * 2, TWINKLE_CORNER_Y * 2, 270, 90);

	////画矩形左下角的圆角  
	//g.DrawArc(pen, rc.left, rc.bottom - TWINKLE_CORNER_Y * 2, TWINKLE_CORNER_X * 2, TWINKLE_CORNER_Y * 2, 90, 90);

	//delete pen;
}

void CUILayoutTwinkle::StartTwinkleRect()
{
	m_bShowDotRect = true;
	ShowImageStyles(m_bColorChange);
	Invalidate();
	SetTimer(CUILayoutTwinkle::N_TIMER_ID, 500);
}

void CUILayoutTwinkle::StopTwinkleRect()
{
	KillTimer(CUILayoutTwinkle::N_TIMER_ID);
	m_bColorChange = false;
	ShowImageStyles(m_bColorChange);
	Invalidate();
}

void CUILayoutTwinkle::ShowDocRect(bool bShow)
{
	m_bShowDotRect = bShow;
	ShowImageStyles(m_bShowDotRect);
	Invalidate();
}

void CUILayoutTwinkle::Init()
{
	__super::Init();
	ShowImageStyles(false);
}

bool CUILayoutTwinkle::Event(TEventUI& event)
{
	if (event.nType == UIEVENT_TIMER
		&& event.wParam == CUILayoutTwinkle::N_TIMER_ID)
	{
		m_bColorChange = !m_bColorChange;
		ShowImageStyles(m_bColorChange);
		m_bShowDotRect = true;
		Invalidate();
	}
	return __super::Event(event);
}

vector <ImageStyle *> CUILayoutTwinkle::GetVisibleOperImages()
{
	vector<ImageStyle *> vecControls;
	ImageStyle * pImage = GetImageStyle(_T("bk"));
	if (pImage)
	{
		vecControls.push_back(pImage);
	}
	pImage = GetImageStyle(_T("center"));
	if (pImage)
	{
		vecControls.push_back(pImage);
	}
	pImage = GetImageStyle(_T("left"));
	if (pImage)
	{
		vecControls.push_back(pImage);
	}
	pImage = GetImageStyle(_T("right"));
	if (pImage)
	{
		vecControls.push_back(pImage);
	}
	return vecControls;
}

void CUILayoutTwinkle::ShowImageStyles(bool bShow)
{
	auto vecImages = GetVisibleOperImages();
	for (auto pImage : vecImages)
	{
		if (pImage)
		{
			pImage->SetVisible(bShow);
		}
	}
}
