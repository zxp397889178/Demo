#include "stdafx.h"
#include "UIScrollBar.h"

#define _Style_ScrollBar_Bk 0
#define _Style_ScrollBar_Begin 1
#define _Style_ScrollBar_End 2
#define _Style_ScrollBar_Thumb 3
#define _Style_ScrollBar_Thumb_Over 4
#define _Style_ScrollBar_Thumb_Down 5

enum {
	MIN_THUMB_SIZE = 30,
	DEFAULT_SCROLL_TIMERID = 10,
	WHEEL_SCROLL_TIMERID  = 11,
	TWEEN_SCROLL_TIMERID = 19,
	TWEEN_SCROLL_TIMER_ELAPSE = 20,
	TWEEN_SCROLL_TIMER_COUNT = 21,
};

CScrollbarUI::CScrollbarUI()
{	
	m_bHorizontal = false;
	// 
	RECT rcInitPos = GetResObj()->GetSystemScrollBarPosition(true);
	SetCoordinate(rcInitPos.left, rcInitPos.top, rcInitPos.right, rcInitPos.bottom);
	SetMouseEnabled(true);
	SetGestureEnabled(true);
	SetEnabled(true);
	m_nRange = 100;
	m_nScrollPos = 0;
	m_cxyFixed.cx = m_cxyFixed.cy = 0;
	m_nLastScrollPos = 0;
	m_nLineSize = 8;
	m_iStatusBegin = UISTATE_NORMAL;
	m_iStatusEnd = UISTATE_NORMAL;
	m_iStatusThumb = UISTATE_NORMAL;

	m_nScrollRepeatDelay = 0;
	m_nLastScrollOffset = 0;
	SetStyle(_T("ScrollBar"));
	SetCoordinate(14, 0, 0, 0, UIANCHOR_RTRB);
	SetNeedScroll(false);

	// 鼠标中轮滚动条使用
	m_dwDelayDeltaY = 0;
	m_dwDelayNum = 0;
	m_dwDelayLeft = 0;

	m_bTweenScrolling = false;
	m_nTweenLastScrollPos = 0;
	m_nTweenOffset = 0;
	m_nTweenCount = 0;

	ModifyFlags(UICONTROLFLAG_SETFOCUS, 0);
}

CScrollbarUI::~CScrollbarUI()
{
	KillTimer(DEFAULT_SCROLL_TIMERID);
}


bool CScrollbarUI::IsHorizontal()
{
	return m_bHorizontal;
}

void CScrollbarUI::SetHorizontal(bool bHorizontal)
{
	m_bHorizontal = bHorizontal;
	RECT rcInitPos = GetResObj()->GetSystemScrollBarPosition(!bHorizontal);
	SetCoordinate(rcInitPos.left, rcInitPos.top, rcInitPos.right, rcInitPos.bottom);
	SetAnchor(bHorizontal ? UIANCHOR_LBRB : UIANCHOR_RTRB);
	this->SetStyle(m_bHorizontal?_T("HScrollBar"):_T("ScrollBar"));
}

int CScrollbarUI::GetScrollRange()
{
	return m_nRange;
}

void CScrollbarUI::SetScrollRange(int nRange)
{
	if (nRange == 0 || m_nRange == nRange)
		return;
	m_nRange = nRange;
	if( m_nRange < 0 )
		m_nRange = 0;
	if( m_nScrollPos > m_nRange )
		m_nScrollPos = m_nRange;
	this->CalcRect();

}

int CScrollbarUI::GetScrollPos()
{
	return m_nScrollPos;
}

void CScrollbarUI::PageUp()
{
	RECT rcItem = GetRect();
	bool bFilp = IsRTLLayoutEnable() && GetUIEngine()->IsRTLLayout();
	if (m_bHorizontal)
	{
		if (bFilp)
			OffsetScrollPos(-(rcItem.left - rcItem.right));
		else
			OffsetScrollPos(rcItem.left - rcItem.right);
	}
	else
	{
		if (bFilp)
			OffsetScrollPos(-(rcItem.top - rcItem.bottom));
		else
			OffsetScrollPos(rcItem.top - rcItem.bottom);
	}
}

void CScrollbarUI::PageDown()
{
	RECT rcItem = GetRect();
	bool bFilp = IsRTLLayoutEnable() && GetUIEngine()->IsRTLLayout();
	if (m_bHorizontal)
	{
		if (bFilp)
			OffsetScrollPos(rcItem.left - rcItem.right);
		else
			OffsetScrollPos(-(rcItem.left - rcItem.right));
	}
	else
	{
		if (bFilp)
			OffsetScrollPos(rcItem.top - rcItem.bottom);
		else
			OffsetScrollPos(-(rcItem.top - rcItem.bottom));
	}
}

void CScrollbarUI::LineUp()
{
	OffsetScrollPos(-DPI_SCALE(m_nLineSize));
}

void CScrollbarUI::LineDown()
{
	OffsetScrollPos(DPI_SCALE(m_nLineSize));
}

void CScrollbarUI::SetScrollPos(int nPos, bool bRedraw)
{
	if ( nPos < 0 )
		nPos = 0;
	if ( nPos > m_nRange )
		nPos = m_nRange;
	int nOffset = nPos - m_nScrollPos;
	if(m_nScrollPos == nPos) 
		return;

	m_nScrollPos = nPos;

	if ( m_nScrollPos < 0 )
		m_nScrollPos = 0;
	if ( m_nScrollPos > m_nRange )
		m_nScrollPos = m_nRange;

	CalcRect();

	if (!bRedraw)
	{
		return;
	}

	TEventUI event = { 0 };
	if (m_bHorizontal)
		event.nType = UIEVENT_HSCROLL;
	else
		event.nType = UIEVENT_VSCROLL;

	event.wParam = (WPARAM)0;
	event.lParam = (LPARAM)nOffset;

	if (GetOwner())
		 GetOwner()->SendEvent(event);
}
void CScrollbarUI::SetLineSize(int nLineSize)
{
	m_nLineSize = nLineSize;
}
int CScrollbarUI::GetLineSize()
{
	return m_nLineSize;
}
SIZE CScrollbarUI::GetFixedSize()
{
	int l,t,r,b;
	GetCoordinate(l, t, r, b);
	SIZE sz = {max(r, l), max(b, t)};
	return sz;
}

void CScrollbarUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)//重载基类
{
	if (equal_icmp(lpszName, _T("vert")))
	{
		m_bHorizontal = !_ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

inline double CalculateDelay(double state) {
	if(state < 0.5) {
		return pow(state * 2, 2) / 2.0;
	}
	return 1.0 - (pow((state - 1.0) * 2, 2) / 2.0);
}

bool CScrollbarUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_BUTTONDOWN:
		{
			if( !IsEnabled() ) return true;
			m_nLastScrollOffset = 0;
			m_nScrollRepeatDelay = 0;
			RECT rcThumb = GetThumbRect();
			StyleObj* pStyleLeft = GetStyle(_Style_ScrollBar_Begin);
			StyleObj* pStyleRight = GetStyle(_Style_ScrollBar_End);
			if (pStyleLeft && pStyleLeft->HitTest(event.ptMouse))
			{
				m_iStatusBegin = UISTATE_DOWN;
				LineUp();
				this->Invalidate();
			}
			else if (pStyleRight && pStyleRight->HitTest(event.ptMouse))
			{
				m_iStatusEnd = UISTATE_DOWN;
				LineDown();
				this->Invalidate();

			}
			else if (PtInRect(&rcThumb, event.ptMouse))
			{
				m_iStatusThumb = UISTATE_DOWN;
				m_ptLastMouse = event.ptMouse;
				m_nLastScrollPos = m_nScrollPos;
				this->Invalidate();
			}
			else
			{
				if( m_bHorizontal ) 
				{
					if( event.ptMouse.x < rcThumb.left ) {
						PageUp();
					}
					else if ( event.ptMouse.x > rcThumb.right ){
						PageDown();
					}

				}
				else
				{
					if( event.ptMouse.y < rcThumb.top ) 
					{
						PageUp();
					}
					else if ( event.ptMouse.y > rcThumb.bottom ){
						PageDown();
					}
				}
			}
			SetTimer(DEFAULT_SCROLL_TIMERID, 30U);
		}
		return true;

	case UIEVENT_BUTTONUP:
		{
			KillTimer(DEFAULT_SCROLL_TIMERID);
			m_nScrollRepeatDelay = 0;
			m_nLastScrollOffset = 0;
			
			if (!PtInRect(GetRectPtr(), event.ptMouse))
			{
				SetState(UISTATE_NORMAL);
				m_iStatusThumb = UISTATE_NORMAL;
				m_iStatusBegin = UISTATE_NORMAL;
				m_iStatusEnd = UISTATE_NORMAL;
			}
			else
			{
				SetState(UISTATE_OVER);
				m_iStatusThumb = UISTATE_OVER;
				m_iStatusBegin = UISTATE_OVER;
				m_iStatusEnd = UISTATE_OVER;
			}
			Invalidate();
		}
		return true;

	case UIEVENT_MOUSEENTER:
		{
			if (event.wParam == MK_LBUTTON) break;

			SetState(UISTATE_OVER);
			m_iStatusThumb = UISTATE_OVER;
			m_iStatusBegin = UISTATE_OVER;
			m_iStatusEnd = UISTATE_OVER;
			Invalidate();
		}
		break;
	case UIEVENT_MOUSELEAVE:
		{
			if (event.wParam == MK_LBUTTON)  break;

			SetState(UISTATE_NORMAL);
			m_iStatusThumb = UISTATE_NORMAL;
			m_iStatusBegin = UISTATE_NORMAL;
			m_iStatusEnd = UISTATE_NORMAL;

			this->Invalidate();
		}
		break;
	case UIEVENT_MOUSEMOVE:
		{
			if (m_iStatusThumb == UISTATE_DOWN && m_nRange > 0)
			{
				RECT rcThumb = GetThumbRect();
				int nTemp;
				RECT rcItem = GetRect();
				if( m_bHorizontal ) 
				{
					nTemp = (rcItem.right - rcItem.left - rcThumb.right + rcThumb.left - 2 * m_cxyFixed.cx);
					if (nTemp == 0)
						nTemp = 1;

					if (GetUIEngine()->IsRTLLayout())
					{
						m_nLastScrollOffset = -((event.ptMouse.x - m_ptLastMouse.x) * m_nRange / nTemp);

					}
					else
					{
						m_nLastScrollOffset = (event.ptMouse.x - m_ptLastMouse.x) * m_nRange / nTemp;

					}

				}
				else 
				{
					nTemp = (rcItem.bottom - rcItem.top - rcThumb.bottom + rcThumb.top - 2 * m_cxyFixed.cy);
					if (nTemp == 0)
						nTemp = 1;
					m_nLastScrollOffset = (event.ptMouse.y - m_ptLastMouse.y) * m_nRange / nTemp;
				}
			}
		}
		return true;
	case UIEVENT_TIMER:
		if(event.wParam == DEFAULT_SCROLL_TIMERID)
		{
			++m_nScrollRepeatDelay;
			if (m_iStatusThumb == UISTATE_DOWN)
			{
				SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
			}
			else if (m_iStatusBegin == UISTATE_DOWN)
			{
				if (m_nScrollRepeatDelay < 5)
					return true;

				LineUp();
			}
			else if (m_iStatusEnd == UISTATE_DOWN)
			{
				if (m_nScrollRepeatDelay < 5)
					return true;

				if (m_nScrollPos + DPI_SCALE(m_nLineSize) >= m_nRange)
					KillTimer(DEFAULT_SCROLL_TIMERID);

				LineDown();
			}
			else 
			{
				if (m_nScrollRepeatDelay < 5)
					return true;

				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(GetWindow()->GetHWND(), &pt);

				RECT rcThumb = GetThumbRect();
				if( m_bHorizontal ) 
				{
					if( pt.x < rcThumb.left ) {
						PageUp();
					}
					else if ( pt.x > rcThumb.right ){
						PageDown();
					}

				}
				else
				{
					if( pt.y < rcThumb.top ) 
					{
						PageUp();
					}
					else if ( pt.y > rcThumb.bottom ){
						PageDown();
					}
				}

			}	
		}
		else if (event.wParam == WHEEL_SCROLL_TIMERID)
		{
			if( m_dwDelayLeft > 0 )
			{
				--m_dwDelayLeft;
				int cy  = GetScrollPos();
				int cyold = cy;
				if (m_dwDelayNum > 0)
				{
					LONG lDeltaY =  (LONG)(CalculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
					if( (lDeltaY > 0 && cy != 0)  || (lDeltaY < 0 && cy != GetScrollRange() ) ) {
						OffsetScrollPos(-lDeltaY);
						// 解决有时无法滚动的现象
						if (cyold == GetScrollPos())
						{
							m_dwDelayDeltaY = 0;
							m_dwDelayNum = 0;
							m_dwDelayLeft = 0;
							KillTimer(WHEEL_SCROLL_TIMERID);
						}
						return true;
					}
				}
			}
			m_dwDelayDeltaY = 0;
			m_dwDelayNum = 0;
			m_dwDelayLeft = 0;
			KillTimer(WHEEL_SCROLL_TIMERID);
		}
		else if (event.wParam == TWEEN_SCROLL_TIMERID)
		{
			if (m_bTweenScrolling)
			{
				m_nTweenCount++;
				if (m_nTweenCount >= TWEEN_SCROLL_TIMER_COUNT)
				{
					m_nTweenCount = TWEEN_SCROLL_TIMER_COUNT;
					KillTimer(TWEEN_SCROLL_TIMERID);
					m_bTweenScrolling = false;
				}
				int nPos = (int)floor(TweenAlgorithm::Quad( m_nTweenCount, 0, m_nTweenOffset, TWEEN_SCROLL_TIMER_COUNT , 2));

				OffsetScrollPos(nPos - m_nTweenLastScrollPos);

				m_nTweenLastScrollPos = nPos;
			}
		}
		return true;

	case UIEVENT_SCROLLWHEEL:
		{
			// 解决有时无法滚动的现象
			if (m_dwDelayLeft <= 0)
			{
				m_dwDelayDeltaY = 0;
				m_dwDelayNum = 0;
				m_dwDelayLeft = 0;
				KillTimer(WHEEL_SCROLL_TIMERID);
			}
			LONG lDeltaY = 0;
			if( m_dwDelayNum > 0 ) lDeltaY =  (LONG)(CalculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
			if (GET_WHEEL_DELTA_WPARAM(event.wParam) < 0)
			{
				if( m_dwDelayDeltaY <= 0 ) m_dwDelayDeltaY = lDeltaY - 7;
				else m_dwDelayDeltaY = lDeltaY - 10;
			} 
			else
			{
				if( m_dwDelayDeltaY >= 0 ) m_dwDelayDeltaY = lDeltaY + 7;
				else m_dwDelayDeltaY = lDeltaY + 10;
			}
			if( m_dwDelayDeltaY > 100 ) m_dwDelayDeltaY = 100;
			else if( m_dwDelayDeltaY < -100 ) m_dwDelayDeltaY = -100;
			m_dwDelayNum = (DWORD)sqrt((double)abs(m_dwDelayDeltaY)) * 4;
			m_dwDelayLeft = m_dwDelayNum;
			SetTimer(WHEEL_SCROLL_TIMERID, 30U);
			return true;
		}
		break;
	case UIEVENT_GESTURENOTIFY:
	{
		RECT rcThumb = GetThumbRect();
		if (PtInRect(&rcThumb, event.ptMouse))
		{
			GESTURECONFIG gc = { 0, 0, GC_ALLGESTURES};
			UINT uiGcs = 1;
			if (set_gesture_config)
				set_gesture_config(GetWindow()->GetHWND(), 0, uiGcs, &gc, sizeof(GESTURECONFIG));
			return true;
		}
	
	}
	break;

	case UIEVENT_GESTURE:
	{
		HandleGestureEvent(event);
	}
	break;
	}
	return CControlUI::Event(event);
}

void CScrollbarUI::HandleGestureEvent(TEventUI& event)
{
	static POINT lastPt = { 0, 0 }; // 记录上次的位置
	static bool bInThumb = false;
	GESTUREINFO gi = {};
	gi.cbSize = sizeof(GESTUREINFO);
	if (get_gesture_info((HGESTUREINFO)event.lParam, &gi))
	{
		// 默认只处理滑动消息
		if (gi.dwID == GID_PAN)
		{
			if (gi.dwFlags & GF_BEGIN)
			{
				RECT rcThumb = GetThumbRect();
				if (PtInRect(&rcThumb, event.ptMouse))
					bInThumb = true;
				lastPt = event.ptMouse;

			}
			else if (gi.dwFlags & GF_END)
			{
				lastPt.x = lastPt.y = 0;
				bInThumb = false;
			}
			else
			{
				int offsetX = event.ptMouse.x - lastPt.x;
				int offsetY = event.ptMouse.y - lastPt.y;
				lastPt = event.ptMouse;

				int nCurPos = GetScrollPos();
				if (bInThumb)
				{
					if (IsHorizontal())
						SetScrollPos(nCurPos + offsetX);
					else
						SetScrollPos(nCurPos + offsetY);
				}
				else
				{
					if (IsHorizontal())
						SetScrollPos(nCurPos - offsetX);
					else
						SetScrollPos(nCurPos - offsetY);
				}	
			}
		}
	}
}


void CScrollbarUI::SetRect(RECT& rectRegion)
{
	CViewUI::SetRect(rectRegion);
	FlipRect();
	int nCnt = GetStyleCount();
	RECT rcStyle = {0};
	int l, t, r, b;
	UITYPE_ANCHOR a;
	StyleObj* pStyleObj = NULL;
	RECT rcItem = GetRect();
	for (int i = 0; i < nCnt; i++)
	{
		if (i != _Style_ScrollBar_Begin && i != _Style_ScrollBar_End)
		{
			pStyleObj = (StyleObj*)GetStyle(i);
			if (pStyleObj)
				pStyleObj->OnSize(rcItem);
		}
		else
		{
			//处理rtl问题
			pStyleObj = (StyleObj*)GetStyle(i);
			if (pStyleObj)
			{
				pStyleObj->GetCoordinate(l, t, r, b);
				a = pStyleObj->GetAnchor();

				rcStyle.left = l;
				rcStyle.top = t;
				rcStyle.right = r;
				rcStyle.bottom = b;
				DPI_SCALE(&rcStyle);
				GetAnchorPos(a, &rcItem, &rcStyle);
				LPRECT lpRect= pStyleObj->GetRectPtr();
				*lpRect = rcStyle;
			}
		}
	}

	StyleObj* pStyle = GetStyle(_Style_ScrollBar_Begin);
	if (pStyle)
	{
		RECT rect = pStyle->GetRect();
		m_cxyFixed.cx = rect.right - rect.left;
		m_cxyFixed.cy = rect.bottom - rect.top;
	}
		
}

void CScrollbarUI::CalcRect()
{
	RECT rcItem = GetRect();
	if (m_bHorizontal)
	{
		int cx = rcItem.right - rcItem.left - 2 * m_cxyFixed.cx;
		if (cx > MIN_THUMB_SIZE)
		{
			m_rcThumb.top = rcItem.top;
			m_rcThumb.bottom = rcItem.bottom;

			if (m_nRange > 0)
			{
				int cxThumb = cx * (rcItem.right - rcItem.left) / ((m_nRange + rcItem.right - rcItem.left)!=0?(m_nRange + rcItem.right - rcItem.left):1);
				if( cxThumb < MIN_THUMB_SIZE ) cxThumb = MIN_THUMB_SIZE;

				m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + rcItem.left + m_cxyFixed.cx;
				m_rcThumb.right = m_rcThumb.left + cxThumb;
				if( m_rcThumb.right > rcItem.right - m_cxyFixed.cx ) 
				{
					m_rcThumb.left = rcItem.right - m_cxyFixed.cx - cxThumb;
					m_rcThumb.right = rcItem.right - m_cxyFixed.cx;
				}
			}
			else 
			{
				m_rcThumb.left = rcItem.left + m_cxyFixed.cx;
				m_rcThumb.right = rcItem.right - m_cxyFixed.cx;
			}

		}
		else
		{
			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}

	}
	else
	{
		int cy = rcItem.bottom - rcItem.top - 2 * m_cxyFixed.cy;
		if (cy > MIN_THUMB_SIZE)
		{
			m_rcThumb.left = rcItem.left;
			m_rcThumb.right = rcItem.right;
			if( m_nRange > 0 ) 
			{
				int cyThumb = cy * (rcItem.bottom - rcItem.top) / ((m_nRange + rcItem.bottom - rcItem.top)!=0?(m_nRange + rcItem.bottom - rcItem.top):1);
				if( cyThumb < MIN_THUMB_SIZE ) cyThumb = MIN_THUMB_SIZE;

				m_rcThumb.top = m_nScrollPos * (cy - cyThumb) / m_nRange + rcItem.top + m_cxyFixed.cy;
				m_rcThumb.bottom = m_rcThumb.top + cyThumb;
				if( m_rcThumb.bottom > rcItem.bottom - m_cxyFixed.cy ) 
				{
					m_rcThumb.top = rcItem.bottom - m_cxyFixed.cy - cyThumb;
					m_rcThumb.bottom = rcItem.bottom - m_cxyFixed.cy;
				}
			}
			else 
			{
				m_rcThumb.top = rcItem.top + m_cxyFixed.cy;
				m_rcThumb.bottom = rcItem.bottom - m_cxyFixed.cy;
			}
		}
		else
		{
			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}

	}
}
void CScrollbarUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	CalcRect();
	StyleObj* pStyleObj = NULL;
	ImageStyle* pImageStyle = NULL;

	pStyleObj = GetStyle(_Style_ScrollBar_Bk);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T("Style ScrollBar Bk"));
	pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle) 
	{
		pImageStyle->SetState(GetState());
		pImageStyle->Render(pRenderDC, rcPaint);
	}

	pStyleObj = GetStyle(_Style_ScrollBar_Begin);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T("Style ScrollBar Left"));
	pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle) 
	{
		pImageStyle->SetState(m_iStatusBegin);
		pImageStyle->Render(pRenderDC, rcPaint);
	}

	pStyleObj = GetStyle(_Style_ScrollBar_End);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T("Style ScrollBar Right"));
	pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle) 
	{
		pImageStyle->SetState(m_iStatusEnd);
		pImageStyle->Render(pRenderDC, rcPaint);
	}

	pStyleObj = GetStyle(_Style_ScrollBar_Thumb);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T("Style ScrollBar Thumb"));
	pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle)
	{
		pImageStyle->SetRect(m_rcThumb);
		pImageStyle->SetState(m_iStatusThumb);
		pImageStyle->Render(pRenderDC, rcPaint);
	}

    pStyleObj = GetStyle(_Style_ScrollBar_Thumb_Over);
    RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T("Style ScrollBar Thumb_Over"));
    pImageStyle = static_cast<ImageStyle*>(pStyleObj);
    if (pImageStyle && m_iStatusThumb == UISTATE_OVER)
    {
        pImageStyle->SetRect(m_rcThumb);
        pImageStyle->SetState(m_iStatusThumb);
        pImageStyle->Render(pRenderDC, rcPaint);
    }

    pStyleObj = GetStyle(_Style_ScrollBar_Thumb_Down);
    RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T("Style ScrollBar Thumb_Down"));
    pImageStyle = static_cast<ImageStyle*>(pStyleObj);
    if (pImageStyle && m_iStatusThumb == UISTATE_DOWN)
    {
        pImageStyle->SetRect(m_rcThumb);
        pImageStyle->SetState(m_iStatusThumb);
        pImageStyle->Render(pRenderDC, rcPaint);
    }

    RenderStyle(pRenderDC, rcPaint, _Style_ScrollBar_Thumb_Down + 1);
}

void CScrollbarUI::OffsetScrollPos( int nOffset, bool bTween/* = false*/)
{
	if (bTween)
	{
		if (m_bTweenScrolling)
			return;

		m_bTweenScrolling = true;
		m_nTweenLastScrollPos = 0;
		m_nTweenOffset = nOffset;
		m_nTweenCount = 0;
		SetTimer(TWEEN_SCROLL_TIMERID, TWEEN_SCROLL_TIMER_ELAPSE);
		return;
	}
	if (GetUIEngine()->IsRTLLayout())
	{
		if (m_bHorizontal)
		{
			SetScrollPos(GetScrollPos() - nOffset);
			return;
		}

	}
	
	SetScrollPos(GetScrollPos() + nOffset);
}

RECT CScrollbarUI::GetThumbRect()
{
	RECT rcStyle = GetRect();
	StyleObj* pStyle = GetStyle(_Style_ScrollBar_Thumb);
	if (pStyle)
		rcStyle = pStyle->GetRect();
	if (IsRectEmpty(&rcStyle))
	{
		rcStyle = m_rcThumb;
		if (GetUIEngine()->IsRTLLayout())
		{
			RECT rcOwner = GetClientRect();
			RtlRect(rcStyle, &rcOwner);
		}
	}
	return rcStyle;
}