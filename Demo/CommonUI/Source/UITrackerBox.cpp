#include "stdafx.h"

#include "UITrackerBox.h"


#pragma warning(disable: 4244)
#define POINTRADIO 2			//截屏矩形框四角深色的宽度
#define POINTLENGTH 15			//截屏矩形框四角深色的长度
#define  OFFSETMAX 2

CTrackerBox::CTrackerBox()
{
	m_cursorArray[0]=LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
	m_cursorArray[1]=LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEALL));
	m_cursorArray[2]=LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS));
	m_cursorArray[3]=LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE));
	m_cursorArray[4]=LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENESW));
	m_cursorArray[5]=LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENWSE));
	m_hPen = CreatePen(PS_SOLID, 1,  RGB(234, 121, 134));
	m_hBrush = CreateSolidBrush(RGB(230, 0, 18));
	Clear();
	m_pEvent = NULL;
	m_bLButtonDown = false;
	m_bLockTracInBounder = false;
	m_bLockTracRatio = false;

	m_fRatio = 1;
	m_szMinSize.cx = 86;
	m_szMinSize.cy = 86;
	m_ptFixLast.x = POINT_DEF_X;
	m_ptFixLast.y = POINT_DEF_Y;
}

CTrackerBox::~CTrackerBox()
{
	for(int i=0;i<6;i++)
	{
		if (m_cursorArray[i]!=NULL)
			DeleteObject(m_cursorArray[i]);
	}
	if (m_hPen)
		DeleteObject(m_hPen);
	if (m_hBrush)
		DeleteObject(m_hBrush);
}

void CTrackerBox::Clear()
{
	m_bLButtonDown = false;
	ZeroMemory(&m_rcItem,sizeof(m_rcItem));
	ZeroMemory(&m_ptMouseInit,sizeof(POINT));
	ZeroMemory(&m_ptStart,sizeof(POINT));
	ZeroMemory(&m_ptEnd,sizeof(POINT));
	m_isExistBox=false;
	m_hitPlace= NOHIT;
}


// void CTrackerBox::Draw(HDC hDC)
// {
// 	if (IsRectEmpty(&m_rcItem))
// 		return;
// 	HPEN hOldPen = (HPEN)::SelectObject(hDC, m_hPen);
	// 画线
// 	MoveToEx(hDC,m_rcItem.left,m_rcItem.top,NULL);
// 	LineTo(hDC,m_rcItem.right,m_rcItem.top);
// 	LineTo(hDC,m_rcItem.right,m_rcItem.bottom);
// 	LineTo(hDC,m_rcItem.left,m_rcItem.bottom);
// 	LineTo(hDC,m_rcItem.left,m_rcItem.top);
// 	// 画四角  顺序为 上左 上右 右上... 依次顺时针 
// 	RECT rect1 = { m_rcItem.left, m_rcItem.top, m_rcItem.left + POINTLENGTH, m_rcItem.top + POINTRADIO };
// 	RECT rect2 = { m_rcItem.right + 1 - POINTLENGTH, m_rcItem.top, m_rcItem.right + 1, m_rcItem.top + POINTRADIO };
// 	RECT rect3 = { m_rcItem.right + 1 - POINTRADIO, m_rcItem.top, m_rcItem.right + 1, m_rcItem.top + POINTLENGTH };
// 	RECT rect4 = { m_rcItem.right + 1 - POINTRADIO, m_rcItem.bottom + 1 - POINTLENGTH, m_rcItem.right + 1, m_rcItem.bottom + 1 };
// 	RECT rect5 = { m_rcItem.right + 1 - POINTLENGTH, m_rcItem.bottom + 1 - POINTRADIO, m_rcItem.right + 1, m_rcItem.bottom + 1 };
// 	RECT rect6 = { m_rcItem.left, m_rcItem.bottom + 1 - POINTRADIO, m_rcItem.left + POINTLENGTH, m_rcItem.bottom + 1 };
// 	RECT rect7 = { m_rcItem.left, m_rcItem.bottom + 1 - POINTLENGTH, m_rcItem.left + POINTRADIO, m_rcItem.bottom + 1 };
// 	RECT rect8 = { m_rcItem.left, m_rcItem.top, m_rcItem.left + POINTRADIO, m_rcItem.top + POINTLENGTH };
// 	FillRect(hDC, &rect1, m_hBrush);
// 	FillRect(hDC, &rect2, m_hBrush);
// 	FillRect(hDC, &rect3, m_hBrush);
// 	FillRect(hDC, &rect4, m_hBrush);
// 	FillRect(hDC, &rect5, m_hBrush);
// 	FillRect(hDC, &rect6, m_hBrush);
// 	FillRect(hDC, &rect7, m_hBrush);
// 	FillRect(hDC, &rect8, m_hBrush);

// 	RequestSetZoomBox requestSetzoombox;
// 	requestSetzoombox.rect = m_rcItem;
// 	requestSetzoombox.Send();

// 	StyleObj* pObj = m_pZoomBox->GetStyle(_T("bk"));
// 	if (pObj)
// 	{
// 		TCHAR szPos[256] = { 0 };
// 		tstring strPos = _T("LTRB|%d,%d,%d,%d");
// 		wsprintf(szPos, strPos.c_str(), m_rcItem.left - m_rcBackGround.left - 1, m_rcItem.top - m_rcBackGround.top - 1, m_rcBackGround.right - m_rcItem.right - 2, m_rcBackGround.bottom - m_rcItem.bottom - 2);
// 		pObj->SetAttribute(_T("pos"), szPos);
// 		m_pZoomBox->GetWindow()->GetRoot()->Resize();
// 		m_pZoomBox->GetWindow()->GetRoot()->Invalidate();
// 		RECT rect = m_pZoomBox->GetRect();
// 	}

	//填充阴影部分
// 	::SolidBrush BrushOfBackGround(::Color(128, 0, 0, 0));
// 	::Graphics graphics(hDC);
// 	graphics.FillRectangle(&BrushOfBackGround, m_rcBackGround.left, m_rcBackGround.top, m_rcBackGround.right - m_rcBackGround.left, m_rcItem.top - m_rcBackGround.top);
// 	graphics.FillRectangle(&BrushOfBackGround, m_rcBackGround.left, m_rcItem.bottom + 1, m_rcBackGround.right - m_rcBackGround.left, m_rcBackGround.bottom - m_rcItem.bottom);
// 	graphics.FillRectangle(&BrushOfBackGround, m_rcBackGround.left, m_rcItem.top, m_rcItem.left - m_rcBackGround.left, m_rcItem.bottom + 1 - m_rcItem.top);
// 	graphics.FillRectangle(&BrushOfBackGround, m_rcItem.right + 1, m_rcItem.top, m_rcBackGround.right - m_rcItem.right, m_rcItem.bottom + 1 - m_rcItem.top);
// 
// 
// 	::SelectObject(hDC, hOldPen);
//}


CTrackerBox::HITPLACE CTrackerBox::HitTest(POINT& point)
{
	RECT rect=GetRect();
	if (!m_isExistBox)
	{
		return NOHIT;
	}
	else if ((rect.left-OFFSETMAX <= point.x) && (point.x <= rect.left+OFFSETMAX) 
		&& (rect.top-OFFSETMAX <= point.y) && (point.y <= rect.top+OFFSETMAX))
	{
		return TOPLEFTBOX;
	}
	else if ((rect.right-OFFSETMAX <= point.x) && (point.x <= rect.right + OFFSETMAX) 
		&& (rect.top-OFFSETMAX <=point.y) && (point.y <= rect.top+OFFSETMAX))
	{
		return  TOPRIGHTBOX;
	}
	else if ((rect.left-OFFSETMAX <= point.x) && (point.x <= rect.left+OFFSETMAX) 
		&& (rect.bottom-OFFSETMAX <= point.y) && (point.y <= rect.bottom+OFFSETMAX))
	{
		return  BUTTOMLEFTBOX;
	}
	else if ((rect.right-OFFSETMAX <= point.x) && (point.x <= rect.right + OFFSETMAX) 
		&& (rect.bottom-OFFSETMAX <=point.y) && (point.y <= rect.bottom+OFFSETMAX))
	{
		return  BUTTOMRIGHTBOX;
	}
	else if ((rect.left-OFFSETMAX <= point.x) && (point.x <= rect.left+OFFSETMAX)
		&& (rect.top+OFFSETMAX <= point.y) && (rect.bottom-OFFSETMAX >= point.y))
	{
		return  LEFTBOX;
	}
	else if ((rect.top-OFFSETMAX <= point.y) && (point.y <= rect.top+OFFSETMAX)
		&& (rect.left+OFFSETMAX <= point.x) && (rect.right-OFFSETMAX >=point.x))
	{
		return  TOPBOX;
	}
	else if ((rect.right-OFFSETMAX <= point.x) && (point.x <= rect.right + OFFSETMAX)
		&& (rect.top+OFFSETMAX <= point.y) && (rect.bottom-OFFSETMAX >= point.y))
	{
		return  RIGHTBOX;
	}
	else if ((rect.bottom-OFFSETMAX <= point.y) && (point.y <= rect.bottom+OFFSETMAX)
		&& (rect.left+OFFSETMAX <= point.x) && (rect.right-OFFSETMAX >=point.x))
	{
		return  BOTTOMBOX;
	}
	else if ((rect.left+OFFSETMAX < point.x) && (rect.top+OFFSETMAX < point.y) 
		&& (rect.right-OFFSETMAX > point.x) && (rect.bottom-OFFSETMAX > point.y))
	{
		return  INBOX;
	}
	else 
	{
		return  NOHIT;
	}
}

bool CTrackerBox::BenginTrack( POINT& point, HWND hWnd /*= NULL*/, bool bRedraw /*= true*/ )
{
	RECT rcTemp = m_rcBound;
	rcTemp.right += 1;
	rcTemp.bottom += 1;
	if (!PtInRect(&rcTemp, point))
		return false;

	SetCursor(point);

	m_bLButtonDown = true;
	m_ptMouseInit = point;
	if (!m_isExistBox)
	{
		m_ptStart = m_ptEnd = point;
	}
	else
	{
		m_ptStart.x = m_rcItem.left;
		m_ptStart.y = m_rcItem.top;
		m_ptEnd.x = m_rcItem.right;
		m_ptEnd.y = m_rcItem.bottom;
		m_hitPlace = HitTest(point);

		RECT rcUnion = GetRect();
		rcUnion.top = -POINTRADIO;
		rcUnion.left = -POINTRADIO;
		rcUnion.right += POINTRADIO;
		rcUnion.bottom += POINTRADIO;

		if (bRedraw)
		{
			if ( hWnd != NULL )
			{
				::InvalidateRect(hWnd, &rcUnion, true);
			}
			else if ( m_pEvent != NULL )
			{
				m_pEvent->InvalidateRect(&rcUnion, true);
			}
		}
	}

	return true;
}


bool CTrackerBox::Track( POINT& point, HWND hWnd /*= NULL*/, bool bRedraw /*= true*/ )
{
	bool bRet = false;
	// 鼠标移动
	if (m_bLButtonDown)
	{
		RECT rcOld = m_rcItem;
		if (!m_isExistBox)
		{
			m_ptEnd = point;
		}
		else
		{
			bool bNeedOffSet = true;
			RECT rc = GetRect();
			if (rc.bottom - rc.top < m_szMinSize.cy
				|| rc.right - rc.left < m_szMinSize.cx)
			{
				if (m_hitPlace != INBOX)
				{
					bNeedOffSet = false;
				}
			}

			if (bNeedOffSet)
				OffSet(point);

		}
		CalcRect();

		if (bRedraw)
		{
			RECT rcUnion;	/// 需要刷新的矩形区域
			UnionRect(&rcUnion, &rcOld, &m_rcItem);
			rcUnion.top = -POINTRADIO;
			rcUnion.left = -POINTRADIO;
			rcUnion.right += POINTRADIO;
			rcUnion.bottom += POINTRADIO;
			if ( hWnd != NULL )
			{
				::InvalidateRect(hWnd, &rcUnion, true);
			}
			else if ( m_pEvent != NULL )
			{
				m_pEvent->InvalidateRect(&rcUnion, true);
			}
		}
		bRet = true;
	}
	m_ptMouseInit = point;

	if ( !m_bLButtonDown )
	{
		SetCursor(point);
	}
	return bRet;
}

bool CTrackerBox::EndTrack( HWND hWnd /*= NULL*/, bool bRedraw /*= true*/ )
{
	if ( m_bLButtonDown )
	{
		m_bLButtonDown = false;
		m_hitPlace = NOHIT;
		m_isExistBox = true;

		RECT rcUnion = GetRect();
		rcUnion.top -= POINTRADIO;
		rcUnion.left -= POINTRADIO;
		rcUnion.right += POINTRADIO;
		rcUnion.bottom += POINTRADIO;

		if (bRedraw)
		{
			if ( hWnd != NULL )
			{
				::InvalidateRect(hWnd, &rcUnion, true);
			}
			else if ( m_pEvent != NULL )
			{
				m_pEvent->InvalidateRect(&rcUnion, true);
			}
		}
		m_bLButtonDown = false;

		m_ptFixLast.x = POINT_DEF_X;
		m_ptFixLast.y = POINT_DEF_Y;
		return true;
	}
	return false;
}


RECT CTrackerBox::GetRect()
{
	return m_rcItem;
}

void CTrackerBox::CalcRect()
{
	m_rcItem.left = min(m_ptStart.x, m_ptEnd.x);
	m_rcItem.right = max(m_ptStart.x, m_ptEnd.x);
	m_rcItem.top = min(m_ptStart.y, m_ptEnd.y);
	m_rcItem.bottom  = max(m_ptStart.y, m_ptEnd.y);
	if (m_rcItem.left < m_rcBound.left)
	OffsetRect(&m_rcItem, m_rcBound.left-m_rcItem.left, 0);
	if (m_rcItem.top < m_rcBound.top)
	OffsetRect(&m_rcItem, 0, m_rcBound.top-m_rcItem.top);
	if (m_rcItem.bottom > m_rcBound.bottom)
	OffsetRect(&m_rcItem, 0, m_rcBound.bottom-m_rcItem.bottom);
	if (m_rcItem.right > m_rcBound.right)
	OffsetRect(&m_rcItem, m_rcBound.right-m_rcItem.right, 0);
}

void CTrackerBox::OffSet(POINT& point)
{
	int x =point.x - m_ptMouseInit.x;
	int y =point.y - m_ptMouseInit.y;
	switch (m_hitPlace)
	{
	case INBOX:	/// 移动box时
		{
			if ( m_bLockTracInBounder )
			{
				POINT ptStartRaw = m_ptStart;
				POINT ptEndRaw = m_ptEnd;
				RECT rc;

				m_ptStart.x = ptStartRaw.x + x;
				m_ptStart.y = ptStartRaw.y + y;
				m_ptEnd.x = ptEndRaw.x + x;
				m_ptEnd.y = ptEndRaw.y + y;

				rc.left = min(m_ptStart.x, m_ptEnd.x);
				rc.right = max(m_ptStart.x, m_ptEnd.x);
				rc.top = min(m_ptStart.y, m_ptEnd.y);
				rc.bottom  = max(m_ptStart.y, m_ptEnd.y);
				if (rc.left <= 0 || rc.top <= 0)
					return;

				int cx = GetWidth(rc);
				int cy = GetHeight(rc);

				RECT rcIntersect;
				
				RECT rcTemp = m_rcBound;
				rcTemp.right -= 1;
				rcTemp.bottom -= 1;
				/// 防止鼠标移动太快, 移出矩形区
				while ( !IntersectRect(&rcIntersect, &rc, &rcTemp) )
				{
					/// 是横线?
					if ( (rc.left > rcTemp.right) || (rc.right > rcTemp.left) )
					{
						y = cy - 1;
					}
					if ( (rc.top > rcTemp.bottom) || (rc.bottom > rcTemp.top) )
					{
						x = cx - 1;
					}

					m_ptStart.x = ptStartRaw.x + x;
					m_ptStart.y = ptStartRaw.y + y;
					m_ptEnd.x = ptEndRaw.x + x;
					m_ptEnd.y = ptEndRaw.y + y;

					rc.left = min(m_ptStart.x, m_ptEnd.x);
					rc.right = max(m_ptStart.x, m_ptEnd.x);
					rc.top = min(m_ptStart.y, m_ptEnd.y);
					rc.bottom  = max(m_ptStart.y, m_ptEnd.y);

					continue;
				}

				if ( IntersectRect(&rcIntersect, &rc, &rcTemp) )
				{
					bool bInCorner = false;
					POINT pt;
					pt.x = rcTemp.left + 1;
					pt.y = rcTemp.top + 1;
					/// 在左上角
					if ( ::PtInRect(&rc, pt) )
					{
						m_ptStart.x = rcTemp.left;
						m_ptStart.y = rcTemp.top;

						m_ptEnd.x = m_ptStart.x + cx;
						m_ptEnd.y = m_ptStart.y + cy;
						break;
					}

					/// 在右上角
					pt.x = rcTemp.right - 1;
					pt.y = rcTemp.top + 1;
					if ( ::PtInRect(&rc, pt) )
					{
						m_ptStart.x = rcTemp.right - cx;
						m_ptStart.y = rcTemp.top;
						m_ptEnd.x = m_ptStart.x + cx;
						m_ptEnd.y = m_ptStart.y + cy;
						break;
					}

					/// 左下角
					pt.x = rcTemp.left + 1;
					pt.y = rcTemp.bottom - 1;
					if ( ::PtInRect(&rc, pt) )
					{
						m_ptStart.x = rcTemp.left;
						m_ptStart.y = rcTemp.bottom - cy;
						m_ptEnd.x = m_ptStart.x + cx;
						m_ptEnd.y = m_ptStart.y + cy;
						break;
					}

					/// 右下角
					pt.x = rcTemp.right - 1;
					pt.y = rcTemp.bottom - 1;
					if ( ::PtInRect(&rc, pt) )
					{
						m_ptStart.x = rcTemp.right - cx;
						m_ptStart.y = rcTemp.bottom - cy;
						m_ptEnd.x = m_ptStart.x + cx;
						m_ptEnd.y = m_ptStart.y + cy;
						break;
					}

					/// 上边交 和 左边交
					if ( (rcTemp.top == rcIntersect.top) || (rcTemp.left == rcIntersect.left) )
					{
						m_ptStart.x = rcIntersect.left;
						m_ptStart.y = rcIntersect.top;
						m_ptEnd.x = m_ptStart.x + cx;
						m_ptEnd.y = m_ptStart.y + cy;
						break;
					}
					/// 下边交 和 右边交
					if ( (rcTemp.right == rcIntersect.right) || (rcTemp.bottom == rcIntersect.bottom) )
					{
						m_ptStart.x = rcIntersect.right - cx;
						m_ptStart.y = rcIntersect.bottom - cy;
						m_ptEnd.x = m_ptStart.x + cx;
						m_ptEnd.y = m_ptStart.y + cy;
						break;
					}
				}
			}
			else
			{
				m_ptStart.x += x;
				m_ptStart.y += y;
				m_ptEnd.x += x;
				m_ptEnd.y += y;
			}
		}
		break;
	case LEFTBOX:
		{
			m_ptStart.x += x;
			if ( !m_bLockTracRatio ) break;

			POINT ptFix;
			ptFix.x = m_ptEnd.x;
			ptFix.y = (m_ptEnd.y + m_ptStart.y) / 2;

			//if ( point.x > ptFix.x )	m_hitPlace = RIGHTBOX;

			RECT rc;
			rc.top = ptFix.y;	
			rc.left = min(ptFix.x, point.x);			
			rc.right = ptFix.x;	
			rc.bottom  = max(ptFix.y, point.y);	

			GetRectByRatioByWE(rc);
			GetRectByMinSizeByWE(rc, &ptFix);
			GetRectByBoundByWE(rc, point, ptFix);

			m_ptStart.x = rc.left;
			m_ptStart.y = rc.top;
			m_ptEnd.x = rc.right;
			m_ptEnd.y = rc.bottom;
		}
		break;
	case RIGHTBOX:
		{
			m_ptEnd.x += x;
			if ( !m_bLockTracRatio ) break;

			POINT ptFix;
			ptFix.x = m_ptStart.x;
			ptFix.y = (m_ptEnd.y + m_ptStart.y) / 2;

			//if ( point.x < ptFix.x )	m_hitPlace = LEFTBOX;

			RECT rc;
			rc.top = ptFix.y;	
			rc.left = ptFix.x;	
			rc.right = max(ptFix.x, point.x);	
			rc.bottom  = max(ptFix.y, point.y);	

			GetRectByRatioByWE(rc);
			GetRectByMinSizeByWE(rc, &ptFix);
			GetRectByBoundByWE(rc, point, ptFix);

			m_ptStart.x = rc.left;
			m_ptStart.y = rc.top;
			m_ptEnd.x = rc.right;
			m_ptEnd.y = rc.bottom;
		}
		break;
	case TOPBOX:
		{
			m_ptStart.y +=y;
			if ( !m_bLockTracRatio ) break;

			POINT ptFix;
			ptFix.x = (m_ptEnd.x + m_ptStart.x) / 2;
			ptFix.y = m_ptEnd.y;

 			//if ( point.y > ptFix.y )	m_hitPlace = BOTTOMBOX;

			RECT rc;
			rc.top = min(ptFix.y, point.y);	
			rc.left = ptFix.x;					
			rc.right = min(ptFix.x, point.x);	
			rc.bottom  = ptFix.y;	

			GetRectByRatioByWE(rc);
			GetRectByMinSizeByWE(rc, &ptFix);
			GetRectByBoundByWE(rc, point, ptFix);

			m_ptStart.x = rc.left;
			m_ptStart.y = rc.top;
			m_ptEnd.x = rc.right;
			m_ptEnd.y = rc.bottom;

		}
		break;
	case BOTTOMBOX:
		{
			m_ptEnd.y += y;
			if ( !m_bLockTracRatio ) break;

			POINT ptFix;
			ptFix.x = (m_ptEnd.x + m_ptStart.x) / 2;
			ptFix.y = m_ptStart.y;

 			//if ( point.y < ptFix.y ) m_hitPlace = TOPBOX;

			RECT rc;
			rc.top = ptFix.y;	
			rc.left = ptFix.x;	
			rc.right = min(ptFix.x, point.x);	
			rc.bottom  = max(ptFix.y, point.y);	

			GetRectByRatioByWE(rc);
			GetRectByMinSizeByWE(rc, &ptFix);
 			GetRectByBoundByWE(rc, point, ptFix);

			m_ptStart.x = rc.left;
			m_ptStart.y = rc.top;
			m_ptEnd.x = rc.right;
			m_ptEnd.y = rc.bottom;
		}
		break;
	case BUTTOMLEFTBOX:
		{
			if ( !m_bLockTracRatio ) 
			{
				m_ptStart.x += x;
				m_ptEnd.y += y;
				break;
			}
			
			POINT ptFix;
			if ( m_ptFixLast.x != POINT_DEF_X && m_ptFixLast.y != POINT_DEF_Y )
			{
				ptFix = m_ptFixLast;
			}
			else
			{
				ptFix.x = m_ptEnd.x;
				ptFix.y = m_ptStart.y;
			}

			RECT rc;
			rc.top = ptFix.y;
 			rc.right = ptFix.x;	

			rc.left = min(ptFix.x, point.x);	//	rc.right - abs(ptFix.x - point.x);
 			rc.bottom = max(ptFix.y, point.y);	//  rc.top + abs(ptFix.y - point.y);
 
 			GetRectByRatioByNESW(rc);
			GetRectByMinSizeByNESW(rc);
			
			POINT ptMove;
  			ptMove.x = rc.left;
  			ptMove.y = rc.bottom;
  			GetRectByBoundByNESW(rc, ptMove, ptFix);
 
			m_ptStart.x = rc.left;
			m_ptStart.y = rc.top;
 			m_ptEnd.x = rc.right;
 			m_ptEnd.y = rc.bottom;

		}
		break;
	case TOPRIGHTBOX:
		{
			if ( !m_bLockTracRatio ) 
			{
				m_ptEnd.x += x;
				m_ptStart.y += y;
				break;
			}

			POINT ptFix;
			if ( m_ptFixLast.x != POINT_DEF_X && m_ptFixLast.y != POINT_DEF_Y )
			{
				ptFix = m_ptFixLast;
			}
			else
			{
				ptFix.x = m_ptStart.x;
				ptFix.y = m_ptEnd.y;
			}

			RECT rc;
			rc.left = ptFix.x;
			rc.bottom  = ptFix.y;

			rc.top = min(ptFix.y, point.y);		// rc.bottom - abs(ptFix.y - point.y);
			rc.right = max(ptFix.x, point.x);	// rc.left + abs(ptFix.x - point.x);	

			GetRectByRatioByNESW(rc);
			GetRectByMinSizeByNESW(rc);

			POINT ptMove;
			ptMove.x = rc.right;
			ptMove.y = rc.top;
			GetRectByBoundByNESW(rc, ptMove, ptFix);

			m_ptStart.x = rc.left;
			m_ptStart.y = rc.top;
			m_ptEnd.x = rc.right;
			m_ptEnd.y = rc.bottom;
		}
		break;
	case BUTTOMRIGHTBOX:
		{
			if ( !m_bLockTracRatio ) 
			{
				m_ptEnd.x += x;
				m_ptEnd.y += y;
				break;
			}				

			POINT ptFix;
 			if ( m_ptFixLast.x != POINT_DEF_X && m_ptFixLast.y != POINT_DEF_Y )
 			{
 				ptFix = m_ptFixLast;
 			}
 			else
 			{
				ptFix = m_ptStart;
 			}

			RECT rc;
			rc.left = ptFix.x;	
			rc.top = ptFix.y;

			rc.right = max(ptFix.x, point.x);	//rc.left + abs(ptFix.x - point.x);	
			rc.bottom = max(ptFix.y, point.y);	//rc.top + abs(ptFix.y - point.y);	

			GetRectByRatioByNESW(rc);
			GetRectByMinSizeByNESW(rc);
 
 			POINT ptMove;
 			ptMove.x = rc.right;
 			ptMove.y = rc.bottom;
 			GetRectByBoundByNESW(rc, ptMove, ptFix);

			m_ptEnd.x = rc.right;
			m_ptEnd.y = rc.bottom;
			m_ptStart = ptFix;
		}
		break;
	case TOPLEFTBOX:
		{
			if ( !m_bLockTracRatio ) 
			{
				m_ptStart.x += x;
				m_ptStart.y += y;
				break;
			}

			POINT ptFix;
 			if ( m_ptFixLast.x != POINT_DEF_X && m_ptFixLast.y != POINT_DEF_Y )
 			{
 				ptFix = m_ptFixLast;
 			}
 			else
 			{
				ptFix =  m_ptEnd;
 			}

			RECT rc;
			rc.right = ptFix.x;	
			rc.bottom  = ptFix.y;

			rc.left = min(point.x, ptFix.x);	// ptFix.x - abs(point.x - ptFix.x);
			rc.top = min(point.y, ptFix.y);		// ptFix.y - abs(point.y - ptFix.y);
			
 			GetRectByRatioByNESW(rc);
			GetRectByMinSizeByNESW(rc);

 			POINT ptMove;
 			ptMove.x = rc.left;
 			ptMove.y = rc.top;
 			GetRectByBoundByNESW(rc, ptMove, ptFix);

			m_ptStart.x = rc.left;
			m_ptStart.y = rc.top;
			m_ptEnd = ptFix;
		}
		break;
	default:
		break;
	}
}


void CTrackerBox::SetCursor(POINT& pt)
{
	HITPLACE nHit = HitTest(pt);
	switch(nHit)
	{
	case CTrackerBox::NOHIT:
		::SetCursor(m_cursorArray[0]);
		break;
	case CTrackerBox::INBOX:
		::SetCursor(m_cursorArray[1]);
		break;
	case CTrackerBox::LEFTBOX:
	case CTrackerBox::RIGHTBOX:
		::SetCursor(m_cursorArray[3]);
		break;
	case CTrackerBox::TOPBOX:
	case CTrackerBox::BOTTOMBOX:
		::SetCursor(m_cursorArray[2]);
		break;
	case CTrackerBox::TOPLEFTBOX:
	case CTrackerBox::BUTTOMRIGHTBOX:
		::SetCursor(m_cursorArray[5]);
		break;
	case CTrackerBox::TOPRIGHTBOX:
	case CTrackerBox::BUTTOMLEFTBOX:
		::SetCursor(m_cursorArray[4]);
		break;
	default:
		::SetCursor(m_cursorArray[0]);
		break;
	}

}

void CTrackerBox::SetLineColor(COLORREF color)
{
	if (m_hPen)
		DeleteObject(m_hPen);
	if (m_hBrush)
		DeleteObject(m_hBrush);
	m_hPen = CreatePen(PS_SOLID, 1, color);
	m_hBrush = CreateSolidBrush(color);
}

void CTrackerBox::SetBoundRect(RECT& rect)
{
	m_rcBound = rect;
}

RECT CTrackerBox::GetBoundRect()
{
	return m_rcBound;
}

void CTrackerBox::SetRect(RECT& rect)
{
	m_ptStart.x = rect.left;
	m_ptStart.y = rect.top;
	m_ptEnd.x = rect.right;
	m_ptEnd.y = rect.bottom;
	m_isExistBox = true;
	CalcRect();
}

void CTrackerBox::GetRectByRatioByNESW(RECT& rc)
{
	float fRatioCur = 0;
	if ( GetHeight(rc) != 0 )
	{
		fRatioCur = GetWidth(rc) / (float)GetHeight(rc);
	}

	switch (m_hitPlace)
	{
	case BUTTOMRIGHTBOX: 
		{
			if ( m_fRatio > fRatioCur )/// 说明高度太高
			{
				int width = (int)(GetHeight(rc) * m_fRatio);
				rc.right = rc.left + width;
			}
			else if ( m_fRatio < fRatioCur )
			{
				int height = (int)(GetWidth(rc) / m_fRatio);
				rc.bottom = rc.top + height;
			}
		}
		break;
	case TOPLEFTBOX: 
		{
			if ( m_fRatio > fRatioCur )/// 说明高度太高
			{
				int width = (int)(GetHeight(rc) * m_fRatio);
				rc.left = rc.right - width;
			}
			else if ( m_fRatio < fRatioCur )
			{
				int height = (int)(GetWidth(rc) / m_fRatio);
				rc.top = rc.bottom - height;
			}
		}
		break;
	case BUTTOMLEFTBOX:
		{
			if ( m_fRatio > fRatioCur )/// 说明高度太高
			{
				int width = (int)(GetHeight(rc) * m_fRatio);
				rc.left = rc.right - width;
			}
			else if ( m_fRatio < fRatioCur )
			{
				int height = (int)(GetWidth(rc) / m_fRatio);
				rc.bottom = rc.top + height;
			}
		}
		break;
	case TOPRIGHTBOX:
		{
			if ( m_fRatio > fRatioCur )/// 说明高度太高
			{
				int width = (int)(GetHeight(rc) * m_fRatio);
				rc.right = rc.left + width;
			}
			else if ( m_fRatio < fRatioCur )
			{
				int height = (int)(GetWidth(rc) / m_fRatio);
				rc.top = rc.bottom - height;
			}
		}
		break;
	case BOTTOMBOX:
		{
			int height = GetHeight(rc);
			int width = height * m_fRatio;
			rc.left = rc.left - (width / 2);
			rc.right = rc.left + width;
		}
	}
}


bool CTrackerBox::GetRectByMinSizeByNESW(RECT& rc)
{
	if ( GetWidth(rc) < m_szMinSize.cx || GetHeight(rc) < m_szMinSize.cy )
	{
		switch (m_hitPlace)
		{
		case BUTTOMRIGHTBOX: 
			{
				POINT ptTest;
				ptTest.x = rc.left + m_szMinSize.cx;
				ptTest.y = rc.top + m_szMinSize.cy;

				RECT rcTemp = m_rcBound;
				rcTemp.right += 1;
				rcTemp.bottom += 1;
				if (!PtInRect(&rcTemp, ptTest))
 				{
 					return false;
 				}

				rc.right = ptTest.x;
				rc.bottom = ptTest.y;
			}
			break;
		case TOPLEFTBOX:
			{
				POINT ptTest;
				ptTest.x = rc.right - m_szMinSize.cx;
				ptTest.y = rc.bottom - m_szMinSize.cy;

				RECT rcTemp = m_rcBound;
				rcTemp.right += 1;
				rcTemp.bottom += 1;
				if (!PtInRect(&rcTemp, ptTest))
				{
					return false;
				}

				rc.left = ptTest.x;
				rc.top = ptTest.y;
			}
			break;
		case BUTTOMLEFTBOX:
			{
				POINT ptTest;
				ptTest.x = rc.right - m_szMinSize.cx;
				ptTest.y = rc.top + m_szMinSize.cy;

				RECT rcTemp = m_rcBound;
				rcTemp.right += 1;
				rcTemp.bottom += 1;
				if (!PtInRect(&rcTemp, ptTest))
 				{
 					return false;
 				}

				rc.left = ptTest.x;
				rc.bottom = ptTest.y;
			}
			break;
		case TOPRIGHTBOX:
			{
				POINT ptTest;
				ptTest.x = rc.left + m_szMinSize.cx;
				ptTest.y = rc.bottom - m_szMinSize.cy;

				RECT rcTemp = m_rcBound;
				rcTemp.right += 1;
				rcTemp.bottom += 1;
				if (!PtInRect(&rcTemp, ptTest))
 				{
 					return false;
 				}

				rc.right = ptTest.x;
				rc.top = ptTest.y;
			}
			break;
		}
	}

	return true;
}

bool CTrackerBox::GetRectByBoundByNESW(RECT& rc, POINT ptMove, POINT ptFix)
{
	/// 超出了...
	RECT rcTemp = m_rcBound;
	rcTemp.right += 1;
	rcTemp.bottom += 1;
	if (!PtInRect(&rcTemp, ptMove))
	{
		switch (m_hitPlace)
		{
		case BUTTOMRIGHTBOX: 
			{
				POINT ptBound;
				ptBound.x = m_rcBound.right - 1;
				ptBound.y = m_rcBound.bottom - 1;

				int nToBCx = abs(ptBound.x - ptFix.x);
				int nToBCy = abs(ptBound.y - ptFix.y);
				int nToBcyTest = nToBCx / m_fRatio;

				if ( nToBcyTest > nToBCy )
				{
					nToBCx = nToBCy * m_fRatio;
				}
				else
				{
					nToBCy = nToBcyTest;
				}

				rc.right = rc.left + nToBCx;
				rc.bottom = rc.top + nToBCy;
			}
			break;
		case TOPLEFTBOX:
			{
				POINT ptBound;
				ptBound.x = m_rcBound.left;
				ptBound.y = m_rcBound.top;

				int nToBCx = abs(ptBound.x - ptFix.x);
				int nToBCy = abs(ptBound.y - ptFix.y);
				int nToBcyTest = nToBCx / m_fRatio;

				if ( nToBcyTest > nToBCy )
				{
					nToBCx = nToBCy * m_fRatio;
				}
				else
				{
					nToBCy = nToBcyTest;
				}

				rc.left = rc.right - nToBCx;
				rc.top = rc.bottom - nToBCy;
			}
			break;
		case BUTTOMLEFTBOX:
			{
				POINT ptBound;
				ptBound.x = m_rcBound.left;
				ptBound.y = m_rcBound.bottom;

				int nToBCx = abs(ptBound.x - ptFix.x);
				int nToBCy = abs(ptBound.y - ptFix.y);
				int nToBcyTest = nToBCx / m_fRatio;

				if ( nToBcyTest > nToBCy )
				{
					nToBCx = nToBCy * m_fRatio;
				}
				else
				{
					nToBCy = nToBcyTest;
				}

				rc.left = rc.right - nToBCx;
				rc.bottom = rc.top + nToBCy;
			}
			break;
		case TOPRIGHTBOX:
			{
				POINT ptBound;
				ptBound.x = m_rcBound.right;
				ptBound.y = m_rcBound.top;

				int nToBCx = abs(ptBound.x - ptFix.x);
				int nToBCy = abs(ptBound.y - ptFix.y);

				int nToBcyTest = nToBCx / m_fRatio;

				if ( nToBcyTest > nToBCy )
				{
					nToBCx = nToBCy * m_fRatio;
				}
				else
				{
					nToBCy = nToBcyTest;
				}

				rc.right = rc.left + nToBCx;
				rc.top = rc.bottom - nToBCy;
			}
			break;
		}
	}

	return true;
}




void CTrackerBox::GetRectByRatioByWE(RECT& rc)
{
	switch (m_hitPlace)
	{
	case BOTTOMBOX:
	case TOPBOX:
		{
			int height = GetHeight(rc);
			int width = height * m_fRatio;
			rc.left = rc.left - (width / 2);
			rc.right = rc.left + width;
		}
		break;
	case RIGHTBOX:
	case LEFTBOX:
		{
			int width = GetWidth(rc);
			int height = width / m_fRatio;
			rc.top = rc.top - (height / 2);
			rc.bottom = rc.top + height;
		}
		break;
	}
}

void CTrackerBox::GetRectByMinSizeByWE(RECT& rc, POINT* ptFix)
{
	if ( GetWidth(rc) < m_szMinSize.cx || GetHeight(rc) < m_szMinSize.cy )
	{
		switch (m_hitPlace)
		{
		case BOTTOMBOX:
			{
				if ( NULL == ptFix ) return;

				rc.left = ptFix->x - (m_szMinSize.cx / 2);
				rc.right = rc.left + m_szMinSize.cx;
				rc.bottom = rc.top + m_szMinSize.cy;
			}
			break;
		case TOPBOX:
			{
				if ( NULL == ptFix ) return;

				rc.left = ptFix->x - (m_szMinSize.cx / 2);
				rc.right = rc.left + m_szMinSize.cx;
				rc.top = rc.bottom - m_szMinSize.cy;
			}
			break;
		case RIGHTBOX:
			{
				if ( NULL == ptFix ) return;

				rc.top = ptFix->y - (m_szMinSize.cy / 2);
				rc.bottom = rc.top + m_szMinSize.cy;
				rc.right = rc.left + m_szMinSize.cx;
			}
			break;
		case LEFTBOX:
			{
				if ( NULL == ptFix ) return;

				rc.top = ptFix->y - (m_szMinSize.cy / 2);
				rc.bottom = rc.top + m_szMinSize.cy;
				rc.left = rc.right - m_szMinSize.cx;
			}
			break;
		}
	}
}

void CTrackerBox::GetRectByBoundByWE(RECT& rc, POINT ptMove, POINT ptFix)
{
	switch (m_hitPlace)
	{
	case BOTTOMBOX:
		{
			if ( ptMove.y < m_rcBound.top ) return;

			POINT ptTest;
			ptTest.y = ptMove.y;
			/// 看它在中点的左边还是右边.
			bool bAtRight = false;
			if ( (m_rcBound.left + m_rcBound.right) / 2 < ptFix.x )/// 看它在中点的右边.
			{
				ptTest.x = ptFix.x + GetHeight(rc)*m_fRatio/2;
				bAtRight = true;
			}		
			else
			{
				ptTest.x = ptFix.x - GetHeight(rc)*m_fRatio/2;
			} 

			RECT rcTemp = m_rcBound;
			rcTemp.right += 1;
			rcTemp.bottom += 1;
			if (!PtInRect(&rcTemp, ptTest))
			{
				if ( bAtRight )
				{
					rc.right = m_rcBound.right - 1;
					int width = 2*(rc.right - ptFix.x);
					rc.left = rc.right - width;
					int height = GetWidth(rc)/m_fRatio;
					rc.bottom = rc.top + height;
				}
				else
				{
					rc.left = m_rcBound.left;
					int width = 2*(ptFix.x - rc.left);
					rc.right = rc.left + width;
					int height = GetWidth(rc)/m_fRatio;
					rc.bottom = rc.top + height;
				}

				if ( rc.bottom > m_rcBound.bottom )
				{
					rc.bottom = m_rcBound.bottom - 1;
 					int height = GetHeight(rc);
 					int width = height * m_fRatio;
 					rc.left = ptFix.x - (width / 2);
 					rc.right = rc.left + width;
				}
			}
		}
		break;
	case TOPBOX:
		{
			if ( ptMove.y > m_rcBound.bottom ) return;

			POINT ptTest;
			ptTest.y = ptMove.y;
			/// 看它在中点的左边还是右边.
			bool bAtRight = false;
			if ( (m_rcBound.left + m_rcBound.right) / 2 < ptFix.x )/// 看它在中点的右边.
			{
				ptTest.x = ptFix.x + GetHeight(rc)*m_fRatio/2;
				bAtRight = true;
			}		
			else
			{
				ptTest.x = ptFix.x - GetHeight(rc)*m_fRatio/2;
			} 

			RECT rcTemp = m_rcBound;
			rcTemp.right += 1;
			rcTemp.bottom += 1;
			if (!PtInRect(&rcTemp, ptTest))
			{
				if ( bAtRight )
				{
					rc.right = m_rcBound.right;
					int width = 2*(rc.right - ptFix.x);
					rc.left = rc.right - width;
					int height = GetWidth(rc)/m_fRatio;
					rc.top = rc.bottom - height;/// 这句与上面不同
				}
				else
				{
					rc.left = m_rcBound.left;
					int width = 2 * (ptFix.x - rc.left);
					rc.right = rc.left + width;
					int height = GetWidth(rc)/m_fRatio;
					rc.top = rc.bottom - height;/// 这句与上面不同
				}

				if ( rc.top < m_rcBound.top )
				{
					rc.top = m_rcBound.top;
					
					int height = GetHeight(rc);
					int width = height * m_fRatio;
					rc.left = ptFix.x - (width / 2);
					rc.right = rc.left + width;
				}
			}
		}
		break;
	case RIGHTBOX:
		{
			if ( ptMove.x < m_rcBound.left ) return;

			POINT ptTest;
			ptTest.x = ptMove.x;
			/// 看它在中点的上边还是下边.
			bool bAtBottom = false;
			if ( (m_rcBound.top + m_rcBound.bottom) / 2 < ptFix.y )/// 在中点的上边.
			{
				ptTest.y = ptFix.y + GetWidth(rc)/m_fRatio/2;
				bAtBottom = true;
			}
			else
			{
				ptTest.y = ptFix.y - GetWidth(rc)/m_fRatio/2;
			} 

			RECT rcTemp = m_rcBound;
			rcTemp.right += 1;
			rcTemp.bottom += 1;
			if (!PtInRect(&rcTemp, ptTest))
			{
				if ( bAtBottom )
				{
					rc.bottom = m_rcBound.bottom - 1;
					int height = 2*(rc.bottom - ptFix.y);
					int width = height * m_fRatio;

					rc.top = rc.bottom - height;
					rc.right = rc.left + width;
				}
				else
				{ 
 					rc.top = m_rcBound.top;
 					int height = 2*(ptFix.y - rc.top);
 					int width = height * m_fRatio;
 
 					rc.bottom = rc.top + height;
 					rc.right =rc.left + width;
				}

				if ( rc.right > m_rcBound.right )
				{
					rc.right = m_rcBound.right - 1;
					int width = GetWidth(rc);
					int height = width / m_fRatio;
					
					rc.top = ptFix.y - (height / 2);
					rc.bottom = rc.top + height;
				}
			}
		}
		break;
	case LEFTBOX:
		{
			if ( ptMove.x > m_rcBound.right ) return;

			POINT ptTest;
			ptTest.x = ptMove.x;
			/// 看它在中点的上边还是下边.
			bool bAtBottom = false;
			if ( (m_rcBound.top + m_rcBound.bottom) / 2 < ptFix.y )/// 在中点的上边.
			{
				ptTest.y = ptFix.y + GetWidth(rc)/m_fRatio/2;
				bAtBottom = true;
			}
			else
			{
				ptTest.y = ptFix.y - GetWidth(rc)/m_fRatio/2;
			} 
			
			RECT rcTemp = m_rcBound;
			rcTemp.right += 1;
			rcTemp.bottom += 1;
			if (!PtInRect(&rcTemp, ptTest))
			{
				if ( bAtBottom )
				{
					rc.bottom = m_rcBound.bottom;
					int height = 2*(rc.bottom - ptFix.y);
					int width = height * m_fRatio;

					rc.top = rc.bottom - height;

					rc.left = rc.right - width;
				}
				else
				{ 
					rc.top = m_rcBound.top;
					int height = 2*(ptFix.y - rc.top);
					int width = height * m_fRatio;

					rc.bottom = rc.top + height;

					rc.left = rc.right - width;
				}

				if ( rc.left < m_rcBound.left )
				{
					rc.left = m_rcBound.left;
					int width = GetWidth(rc);
					int height = width / m_fRatio;

					rc.top = ptFix.y - (height / 2);
					rc.bottom = rc.top + height;
				}
			}
		}
		break;
	}
}