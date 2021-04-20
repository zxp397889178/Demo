#include "stdafx.h"
#include "UITrackerBox.h"
#include "UIFigurePic.h"


CFigurePicUI::CFigurePicUI(void)
{
	SetStyle(_T("Picture"));
	m_box.SetNotify(this);
	SetMouseEnabled(true);
	ModifyFlags(UICONTROLFLAG_SETCURSOR, 0);
	m_box.LockTracInBound(true);
	m_box.LockTracRatio(true);
	m_box.SetRatio(1);
	m_box.SetMinSize(86, 86);
	m_bTracker = true;
	SetRectEmpty(&m_rcInPic);
	m_uX1 = 0;
	m_uY1 = 0;
	m_uX2 = 0;
	m_uY2 = 0;
	SetRectEmpty(&m_rcPaintLast);
}

CFigurePicUI::~CFigurePicUI(void)
{

}

void CFigurePicUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	HDC hDestDC = pRenderDC->GetDC();
	RECT r = GetRect();

	ImageObj *pImageRender = this->GetImage();

	if (pImageRender)
	{
		/// 画白框
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
		HPEN oldPen = (HPEN)SelectObject(hDestDC, hPen);
		::Rectangle(hDestDC, r.left, r.top, r.right, r.bottom);
		SelectObject(hDestDC, oldPen);
		DeleteObject(hPen);


		RECT rcImg = GetRect();

		PaintParams params;
		params.nAlpha =  GetAlpha();
		params.eSmoothingMode = UITYPE_SMOOTHING::UISMOOTHING_HighQuality;
		if ( IsRectEmpty(&m_rcInPic) )
		{
			/// 画整张图
			::InflateRect(&r, -1, -1);

			int cx1 = GetWidth(r);
			int cy1 = GetHeight(r);
			int cx2 = pImageRender->GetWidth();
			int cy2 = pImageRender->GetHeight();

			if ( ( cx1 > cx2 ) && ( cy1 > cy2 ) )
			{
				/// 图太小时
				
				rcImg.left =  (cx1-cx2)/2 + r.left;
				rcImg.top = (cy1-cy2)/2 + r.top;
				rcImg.right =  (cx1+cx2)/2 + r.left;
				rcImg.bottom = (cy1+cy2)/2 + r.top;
			}
			else
			{
				/// 图太大时
				float fRateDef = GetWidth(r) / (float)GetHeight(r);
				float fRateImg = pImageRender->GetWidth() / (float)pImageRender->GetHeight();

				if ( fRateDef > fRateImg )
				{
					int nWidth = GetHeight(r) * pImageRender->GetWidth() / pImageRender->GetHeight();	/// 得到图片宽
					rcImg.left = r.left + (GetWidth(r) - nWidth) / 2;
					rcImg.right = rcImg.left + nWidth;
					rcImg.top = r.top;
					rcImg.bottom = rcImg.top + GetHeight(r);
				}
				else
				{
					int nHeight = GetWidth(r) * pImageRender->GetHeight() / pImageRender->GetWidth();	/// 得到图片高
					rcImg.left = r.left;
					rcImg.right = rcImg.left + GetWidth(r);
					rcImg.top = r.top + (GetHeight(r) - nHeight) / 2;
					rcImg.bottom = rcImg.top + nHeight;
				}
			}

			/// 画图
			pRenderDC->DrawImage(pImageRender,  rcImg, 0, &params);
		}
		else
		{
			pRenderDC->DrawImage(pImageRender, rcImg, m_rcInPic, &params);
		}
		if ( !m_bTracker )
		{
			return;
		}
		m_box.SetBoundRect(rcImg);

		/// 增加改变控件大小后，box等比例变大的功能。
		RECT rcBox = m_box.GetRect();
		bool bChangeBoxSize = false;
		RECT rcTemp;
		UnionRect(&rcTemp, &rcBox, &rcImg);
		if ( (!IsRectEmpty(&m_rcPaintLast) && !EqualRect(&m_rcPaintLast, &rcImg)) || !EqualRect(&rcTemp, &rcImg) )
		{
			bChangeBoxSize = true;
		}
		else
		{
			int cx = GetWidth(rcImg);
			m_uX1 = (rcBox.left-rcImg.left)*1000/cx;
			m_uY1 = (rcBox.top-rcImg.top)*1000/cx;
			m_uX2 = (rcBox.right-rcImg.left)*1000/cx;
			m_uY2 = (rcBox.bottom-rcImg.top)*1000/cx;
		}

		/// 画box
		if ( m_bOnLoadImg )
		{
			RECT rcBox = rcImg;
			float fRatioImg = pImageRender->GetWidth() / (float)pImageRender->GetHeight();
			int nInset = 15;
			if (rcImg.bottom - rcImg.top <= m_box.GetMinSize().cy
				|| rcImg.right - rcImg.left <= m_box.GetMinSize().cx)
			{
				nInset = 0;
			}

			if ( m_box.GetRatio() < fRatioImg )
			{
				rcBox.top = rcImg.top + nInset;
				rcBox.bottom = rcImg.bottom - nInset;
				int nBoxCx = (int)(GetHeight(rcBox) * m_box.GetRatio());
				rcBox.left = rcImg.left + (GetWidth(rcImg)/2) - (nBoxCx/2);
				rcBox.right = rcBox.left + nBoxCx;
			}
			else
			{
				rcBox.left = rcImg.left + nInset;
				rcBox.right = rcImg.right - nInset;
				int nBoxCy = (int)(GetWidth(rcBox) / m_box.GetRatio());
				rcBox.top = rcImg.top + (GetHeight(rcImg)/2) - (nBoxCy/2);
				rcBox.bottom = rcBox.top + nBoxCy;
			}

			DrawZoomBox(rcBox);
			m_box.SetRect(rcBox);
		}
		/// 增加改变控件大小后，box等比例变大的功能。
 		else if ( bChangeBoxSize )
 		{
 			int cx = GetWidth(rcImg);
 			rcBox.left = m_uX1*cx/1000 + rcImg.left;
 			rcBox.top = m_uY1*cx/1000 + rcImg.top;
 			rcBox.right = m_uX2*cx/1000 + rcImg.left;
 			rcBox.bottom = m_uY2*cx/1000 + rcImg.top;
 			m_box.SetRect(rcBox);
 		}

		RECT rect_box = m_box.GetRect();
		DrawShadow(hDestDC, rect_box);
		m_rcPaintLast = rcImg;
	}	

	StyleObj* pStyleObj = GetStyle(_T("ModifyAvatar_ZoomBox"));
	if (pStyleObj && pStyleObj->IsVisible())
		pStyleObj->Render(pRenderDC, rcPaint);

	if ( m_bOnLoadImg )
	{
		SendNotify(EN_FIGURE_PIC_CHANGE);
		m_bOnLoadImg = false;
	}
}

bool CFigurePicUI::Event(TEventUI& event)
{
	if (!IsEnabled())
		return true;

	switch (event.nType)
	{
	case UIEVENT_BUTTONDOWN:
		{
			POINT point={ (int)(short)LOWORD(event.lParam), (int)(short)HIWORD(event.lParam) };
			m_box.BenginTrack(point);
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			if ( m_box.EndTrack() )
			{
				SendNotify(EN_FIGURE_PIC_CHANGE);
			}
		}
		break;
	case UIEVENT_MOUSEMOVE:
		{
			POINT point={ (int)(short)LOWORD(event.lParam), (int)(short)HIWORD(event.lParam) };
			
			m_box.Track(point);
			RECT rect_box = m_box.GetRect();
			DrawZoomBox(rect_box);
		}
		break;
	case UIEVENT_SETFOCUS:
	case UIEVENT_MOUSEENTER:
		{

		}
		break;
	case UIEVENT_KILLFOCUS:
	case UIEVENT_MOUSELEAVE:
		{
			
		}
		break;
	case  UIEVENT_SETCURSOR:
		{
			m_box.SetCursor(event.ptMouse);
		}
		return true;
	}

	return __super::Event(event);
}

void CFigurePicUI::InvalidateRect(CONST RECT *lpRect, bool bErase)
{
	//__super::Invalidate(bErase);
	::InvalidateRect(GetWindow()->GetHWND(), lpRect, bErase);
}

void CFigurePicUI::SetAttribute(const WCHAR* pszName, const WCHAR* pszValue)
{
	if (_wcsicmp(pszName, L"enable_tracker") == 0)
	{
		m_bTracker = !!_ttoi(pszValue);
	}
	else
	{
		__super::SetAttribute(pszName, pszValue);
	}
}

void CFigurePicUI::SetDrawRect(RECT rcInPic)
{
	m_rcInPic = rcInPic;
}

RECT CFigurePicUI::GetTrackerRectInPic()
{
	RECT rc = {0};

	RECT rcBox = m_box.GetRect();
	RECT rcBound = m_box.GetBoundRect();

	//rcBox.OffsetRect(-rcBound.left, -rcBound.top);
	::OffsetRect(&rcBox, -rcBound.left, -rcBound.top);

	//rcBound.OffsetRect(-rcBound.left, -rcBound.top);
	::OffsetRect(&rcBound, -rcBound.left, -rcBound.top);

	if ( IsRectEmpty(&rcBound) ) return rc;

	ImageObj *pImageRender=NULL;
	pImageRender=this->GetImage();
	if(pImageRender)
	{
		float fRatio = pImageRender->GetWidth() / (float)GetWidth(rcBound);
		rc.left = (LONG)(rcBox.left * fRatio);
		rc.right = (LONG)(rcBox.right * fRatio);

		fRatio = pImageRender->GetHeight() / (float)GetHeight(rcBound);
		rc.top = (LONG)(rcBox.top * fRatio);
		rc.bottom = (LONG)(rcBox.bottom * fRatio);
	}
	return rc;
}

void  CFigurePicUI::Init()
{
	ImageStyle *pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(0));
	if (pStyle)
	{
		pStyle->SetAttribute(_T("imagetype"), _T("bitmap"));
		pStyle->SetAttribute(_T("smooth"), _T("HighQuality"));
	}
}

RECT& CFigurePicUI::GetRcInPic()
{
	return m_rcInPic;
}

void CFigurePicUI::DrawShadow(HDC hDC, RECT& rect)
{
	if (IsRectEmpty(&rect))
		return;

	::SolidBrush BrushOfBackGround(::Color(128, 0, 0, 0));
	::Graphics graphics(hDC);
	graphics.FillRectangle(&BrushOfBackGround, m_rcBackGround.left, m_rcBackGround.top, m_rcBackGround.right - m_rcBackGround.left, rect.top - m_rcBackGround.top);
	graphics.FillRectangle(&BrushOfBackGround, m_rcBackGround.left, rect.bottom + 1, m_rcBackGround.right - m_rcBackGround.left, m_rcBackGround.bottom - rect.bottom);
	graphics.FillRectangle(&BrushOfBackGround, m_rcBackGround.left, rect.top, rect.left - m_rcBackGround.left, rect.bottom + 1 - rect.top);
	graphics.FillRectangle(&BrushOfBackGround, rect.right + 1, rect.top, m_rcBackGround.right - rect.right, rect.bottom + 1 - rect.top);
}

void CFigurePicUI::DrawZoomBox(RECT& rect)
{
	StyleObj* pObj = GetStyle(_T("ModifyAvatar_ZoomBox"));
	if (pObj)
	{
		RECT rectTemp = rect;
		TCHAR szPos[256] = { 0 };
		tstring strPos = _T("LTRB|%d,%d,%d,%d");
		int nLeft = DPI_SCALE_BACK(rectTemp.left - m_rcBackGround.left - 1);
		int nTop = DPI_SCALE_BACK(rectTemp.top - m_rcBackGround.top - 1);
		int nRight = DPI_SCALE_BACK(m_rcBackGround.right - rectTemp.right - 2);
		int nBottom = DPI_SCALE_BACK(m_rcBackGround.bottom - rectTemp.bottom - 2);
		wsprintf(szPos, strPos.c_str(), nLeft - 1, nTop - 1, nRight - 1, nBottom - 1);
		pObj->SetAttribute(_T("pos"), szPos);
		GetParent()->Resize();
		GetParent()->Invalidate();
	}
}

void CFigurePicUI::SetImage(LPCTSTR lpszImage)
{
	ImageStyle *pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(0));
	if (pStyle)
	{
		pStyle->SetImage(lpszImage);
	}
	m_bOnLoadImg = true;
}

ImageObj* CFigurePicUI::GetImage()
{
	ImageStyle *pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(0));
	if (pStyle)
		pStyle->LoadImage();
	return pStyle ? pStyle->GetImageObj() : NULL;
}

LPCTSTR CFigurePicUI::GetImageFileName()
{
	ImageStyle *pStyle=NULL;
	pStyle=dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(0));
	if (pStyle)
	{
		return pStyle->GetImageFile();
	}
	return _T("");
}

void CFigurePicUI::SetImage(ImageObj* pImageRender)
{
	ImageStyle *pStyle = dynamic_cast<ImageStyle*>((StyleObj*)GetStyle(0));
	if (pStyle)
	{
		pStyle->SetImage(pImageRender);
	}
	m_bOnLoadImg = true;
}


HBITMAP CFigurePicUI::CreateHBitBmp()
{
	if (::IsRectEmpty(&m_rcInPic)) return NULL;
	RECT rcItem = GetRect();
	HDC hDC = GetDC(NULL);
	DibObj dib;
	dib.Create(NULL, rcItem.right - rcItem.left, rcItem.bottom - rcItem.top);
	dib.Fill(RGB(255,255,255), 255);
	IRenderDC render;
	render.SetDevice(&dib);
	PaintParams params;
	params.eSmoothingMode = UITYPE_SMOOTHING::UISMOOTHING_HighQuality;
	RECT rcDst = {0, 0, dib.GetWidth(), dib.GetHeight()};
	render.DrawImage(GetImage(), rcDst, m_rcInPic, &params);
	::ReleaseDC(NULL, hDC);
	return dib.Detach();
}

void CFigurePicUI::RotatePhoto90(bool bClockWise/*=true*/)
{
	ImageObj *pImageRender = this->GetImage();
	if (pImageRender != NULL)
	{	
		Bitmap* bitmap = pImageRender->GetBitmap();
		if (!bitmap) return;
		if (bClockWise)
		{
			bitmap->RotateFlip(RotateFlipType::Rotate90FlipNone);
		}
		else
		{
			bitmap->RotateFlip(RotateFlipType::Rotate270FlipNone);
		}
		pImageRender->SetWidth(bitmap->GetWidth());
		pImageRender->SetHeight(bitmap->GetHeight());
		
		m_bOnLoadImg = true;
		this->Invalidate();
		SendNotify(EN_FIGURE_PIC_CHANGE);
	}
}

void CFigurePicUI::SetBackGroundRect(RECT& rect)
{
	m_rcBackGround = rect;
}

