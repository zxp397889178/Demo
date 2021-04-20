#include "stdafx.h"
#include "UIView.h"

LPCTSTR BaseObject::GetId() const
{
	return m_strId.c_str();
}

void BaseObject::SetId( LPCTSTR val )
{
	m_strId = val;
}

void BaseObject::SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue)
{
	if (equal_icmp(lpszName, _T("id")))
	{
		SetId(lpszValue);
	}
}


CViewUI::CViewUI()
{
	SetRectEmpty(&m_coordinate.rect);
	m_coordinate.anchor = UIANCHOR_LTRB;
	memset(&m_rcItem, 0, sizeof(RECT));
	m_cxyMax.cx = m_cxyMax.cy = 0;
	m_cxyMin.cx = m_cxyMin.cy = 0;
	m_bVisible = true;
	SetState((UITYPE_STATE)UISTATE_UNKNOWN);
	SetAlpha(255);

	m_bFlipRect = true;
}

void CViewUI::SetCoordinate( int l,int t,int r,int b, UITYPE_ANCHOR anchor)
{
	m_coordinate.rect.left=l;
	m_coordinate.rect.top=t;
	m_coordinate.rect.right=r;
	m_coordinate.rect.bottom=b;
	if (anchor != UIANCHOR_UNCHANGED)
		m_coordinate.anchor = anchor;
}

void CViewUI::GetCoordinate( int& l, int& t, int& r, int& b )
{
	l = m_coordinate.rect.left;
	t = m_coordinate.rect.top;
	r = m_coordinate.rect.right;
	b = m_coordinate.rect.bottom;
}

void CViewUI::SetAnchor( UITYPE_ANCHOR anchor )
{
	m_coordinate.anchor = anchor;
}

UITYPE_ANCHOR CViewUI::GetAnchor()
{
	return m_coordinate.anchor;
}

void CViewUI::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{
	if (equal_icmp(lpszName, _T("pos")))
	{
		int l, t, r, b;
		UITYPE_ANCHOR anchor;
		PrasePosString(lpszValue, l, t, r, b, anchor);
		SetCoordinate(l, t, r, b, anchor);
	}
	else if (equal_icmp(lpszName, _T("visible")))
	{
		SetVisible(!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("alpha")))
	{
		SetAlpha(_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("maxwidth")))
	{
		m_cxyMax.cx = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("maxheight")))
	{
		m_cxyMax.cy = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("flip")))
	{
		m_bFlipRect = !!_ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

CViewUI::~CViewUI()
{
	ActionManager::GetInstance()->removeViewAnimationByTarget(this);
}

void CViewUI::SetState( UITYPE_STATE nState )
{
	m_nState = nState;
}

UITYPE_STATE CViewUI::GetState()
{
	return m_nState;
}


void CViewUI::OnSize( const RECT& rcParent )
{
	m_rcItem = m_coordinate.rect;
	DPI_SCALE(&m_rcItem);
	GetAnchorPos(m_coordinate.anchor, &rcParent, &m_rcItem);
	RECT rect = m_rcItem;
	SIZE cxyMax = m_cxyMax;
	SIZE cxyMin = m_cxyMin;
	DPI_SCALE(&cxyMax);
	DPI_SCALE(&cxyMin);

	if (cxyMax.cx != 0)
	{
		if (m_rcItem.right -  m_rcItem.left > cxyMax.cx)
			m_rcItem.right = m_rcItem.left +  cxyMax.cx;
	}

	if (cxyMax.cy != 0)
	{
		if (m_rcItem.bottom -  m_rcItem.top > cxyMax.cy)
			m_rcItem.bottom = m_rcItem.top +  cxyMax.cy;
	}

	if (cxyMin.cx != 0)
	{
		if (m_rcItem.right - m_rcItem.left < cxyMin.cx)
			m_rcItem.right = m_rcItem.left + cxyMin.cx;
	}

	if (cxyMin.cy != 0)
	{
		if (m_rcItem.bottom - m_rcItem.top > cxyMin.cy)
			m_rcItem.bottom = m_rcItem.top + cxyMin.cy;
	}

	rect = m_rcItem; // ’‚æ‰ª∞
	SetRect(rect);
}

int CViewUI::GetAlpha()
{
	return m_nAlpha;
}

void CViewUI::SetAlpha( int val )
{
	m_nAlpha = val;
}

Coordinate* CViewUI::GetCoordinate()
{
	return &m_coordinate;
}

void CViewUI::SetCoordinate(Coordinate& coordinate )
{
	m_coordinate = coordinate;
}

bool CViewUI::Resize()
{
	return true;
}

void CViewUI::SetVisible( bool bVisible)
{
	m_bVisible = bVisible;
}

bool CViewUI::IsVisible()
{
	return m_bVisible;
}

void CViewUI::SetRect( RECT& rectRegion )
{
	m_rcItem = rectRegion;
}

RECT CViewUI::GetRect()
{
	return m_rcItem;
}

LPRECT CViewUI::GetRectPtr()
{
	return &m_rcItem;
}

SIZE CViewUI::GetMaxSize()
{
	return m_cxyMax;
}

void CViewUI::SetMaxSize(SIZE& size)
{
	m_cxyMax = size;
}

SIZE CViewUI::GetMinSize()
{
	return m_cxyMin;
}

void CViewUI::SetMinSize(SIZE& size)
{
	m_cxyMin = size;
}

void CViewUI::EnableRTLLayout(bool bEnableLayout)
{
	m_bFlipRect = bEnableLayout;
}

bool CViewUI::IsRTLLayoutEnable()
{
	return m_bFlipRect;
}
