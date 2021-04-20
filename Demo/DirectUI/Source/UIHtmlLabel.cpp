#include "stdafx.h"
#include "UIHtmlLabel.h"

#define DEFAULT_COLOR_LINK		0xFFEB7016
#define DEFAULT_COLOR_LINKHOVER 0xFF1670EB

CHtmlLabelUI::CHtmlLabelUI()
{	
	SetAnchor(UIANCHOR_LEFTTOP);
	SetEnabled(true);
	SetMouseEnabled(true);
	SetStyle(_T("Label"));
	m_iLinks = 0;
	m_iHoverLink = -1;
	m_sHoverLink = _T("");
	::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
	m_pTextStyle = NULL;
	ModifyFlags(UICONTROLFLAG_SETFOCUS | UICONTROLFLAG_SETCURSOR, 0);
	m_uAlignStyle = DT_LEFT;
	m_bEndEllipsis = false;
	m_pTextColorObj = NULL;
	m_pLinkTextColorObj = NULL;
	m_pLinkOverTextColorObj = NULL;
}

CHtmlLabelUI::~CHtmlLabelUI()
{
	ColorObjExHelper::ClearColor(&m_pTextColorObj);
	ColorObjExHelper::ClearColor(&m_pLinkTextColorObj);
	ColorObjExHelper::ClearColor(&m_pLinkOverTextColorObj);
}


void CHtmlLabelUI::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
	Invalidate();
}

TextStyle* CHtmlLabelUI::GetHtmlLabelStyle()
{
	if (!m_pTextStyle)
		m_pTextStyle = GetTextStyle(_T("txt"));
	return m_pTextStyle;
}

void CHtmlLabelUI::SetTextColor(DWORD dwColor)
{
	ColorObjExHelper::SetColor(&m_pTextColorObj, dwColor);
}

DWORD CHtmlLabelUI::GetTextColor()
{
	return ColorObjExHelper::GetColor(m_pTextColorObj);
}

DWORD CHtmlLabelUI::GetLinkTextColor()
{
	if (m_pLinkTextColorObj == NULL)
		return DEFAULT_COLOR_LINK;
	return ColorObjExHelper::GetColor(m_pLinkTextColorObj);
}

DWORD CHtmlLabelUI::GetLinkOverTextColor()
{
	if (m_pLinkOverTextColorObj == NULL)
		return DEFAULT_COLOR_LINKHOVER;
	return ColorObjExHelper::GetColor(m_pLinkOverTextColorObj);
}


void CHtmlLabelUI::SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue)
{
	if (equal_icmp(lpszName, _T("linkcolor")))
	{
		ColorObjExHelper::SetColor(&m_pLinkTextColorObj, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("linkhovercolor")))
	{
		ColorObjExHelper::SetColor(&m_pLinkOverTextColorObj, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("halign")))
	{
		if (equal_icmp(lpszValue, _T("left")))
		{
			if (GetUIEngine()->IsRTLLayout())
			{
				m_uAlignStyle &= ~(DT_CENTER | DT_LEFT);
				m_uAlignStyle |= DT_RIGHT;

			}
			else
			{
				m_uAlignStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uAlignStyle |= DT_LEFT;
			}
		}
		else if (equal_icmp(lpszValue, _T("center")))
		{
			m_uAlignStyle &= ~(DT_LEFT | DT_RIGHT);
			m_uAlignStyle |= DT_CENTER;
		}
		else if (equal_icmp(lpszValue, _T("right")))
		{
			if (GetUIEngine()->IsRTLLayout())
			{
				m_uAlignStyle &= ~(DT_RIGHT | DT_CENTER);
				m_uAlignStyle |= DT_LEFT;

			}
			else
			{
				m_uAlignStyle &= ~(DT_LEFT | DT_CENTER);
				m_uAlignStyle |= DT_RIGHT;
			}
		}
	}
	else if (equal_icmp(lpszName, _T("valign")))
	{
		if (equal_icmp(lpszValue, _T("top")))
		{
			m_uAlignStyle &= ~(DT_BOTTOM | DT_VCENTER);
			m_uAlignStyle |= DT_TOP;
		}
		else if (equal_icmp(lpszValue, _T("center")))
		{
			m_uAlignStyle &= ~(DT_BOTTOM | DT_TOP);
			m_uAlignStyle |= DT_VCENTER;
		}
		else if (equal_icmp(lpszValue, _T("bottom")))
		{
			m_uAlignStyle &= ~(DT_TOP | DT_VCENTER);
			m_uAlignStyle |= DT_BOTTOM;
		}
	}
	else if (equal_icmp(lpszName, _T("singleline")))
	{
		if (!!_ttoi(lpszValue))
		{
			m_uAlignStyle |= DT_SINGLELINE;
		}
		else
		{
			m_uAlignStyle &= ~DT_SINGLELINE;
			m_uAlignStyle |= DT_WORDBREAK | DT_EDITCONTROL;
		}
	}
	else if (equal_icmp(lpszName, _T("endellipsis")))
	{
		if (!!_ttoi(lpszValue))
		{
			m_uAlignStyle &= ~DT_PATH_ELLIPSIS;
			m_uAlignStyle |= DT_END_ELLIPSIS;
		}
		else
		{
			m_uAlignStyle &= ~DT_END_ELLIPSIS;
		}
	}
	else if (equal_icmp(lpszName, _T("textcolor")) || equal_icmp(lpszName, _T("color")))
	{
		ColorObjExHelper::SetColor(&m_pTextColorObj, lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

void CHtmlLabelUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	if ( m_strText.empty() || !GetHtmlLabelStyle())
	{
		return;
	}
	m_bEndEllipsis = false;
	HtmlPaintParams paintParams;
	m_iLinks = sizeof(m_rcLinks)/sizeof(*m_rcLinks);
	TextStyle* pStyle = GetHtmlLabelStyle();
	if (!pStyle)
	{
		return;
	}

	pRenderDC->DrawHtmlText(GetHtmlLabelStyle()->GetFontObj(), pStyle->GetRect(), m_strText.c_str(), GetTextColor(), m_rcLinks, m_strLinks, m_iLinks,
		m_sHoverLink, GetLinkTextColor(), GetLinkOverTextColor(), m_uAlignStyle, paintParams, m_bEndEllipsis, GetHtmlLabelStyle()->GetFontType());
}

int CHtmlLabelUI::CalcHeight(int nWidth)
{
	if ( m_strText.empty() || !GetHtmlLabelStyle())
	{
		return 0;
	}

	if (!this->GetWindow())
	{
		return 0;
	}

	if (m_dib.GetWidth() != nWidth)
	{
		HDC hDC = this->GetWindow()->GetPaintDC();
		m_dib.Create(hDC, nWidth, 1);
		//m_dib.Fill(RGB(255, 255, 255), 255);
	}

	HtmlPaintParams paintParams;
	m_iLinks = sizeof(m_rcLinks)/sizeof(*m_rcLinks);

	IRenderDC render;
	render.SetDevice(&m_dib);

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = nWidth;
	rc.bottom = 9999;

	bool bEndEllipsis = false;
	render.DrawHtmlText(GetHtmlLabelStyle()->GetFontObj(), rc, m_strText.c_str(), 0, m_rcLinks, m_strLinks, m_iLinks, 
		m_sHoverLink, GetLinkTextColor(), GetLinkOverTextColor(), m_uAlignStyle, paintParams, bEndEllipsis, GetHtmlLabelStyle()->GetFontType());
	
	return paintParams.nHeight;
}

bool CHtmlLabelUI::IsEndEllipsis()
{
	return m_bEndEllipsis;
}

bool CHtmlLabelUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_BUTTONDOWN:
		{
			for(int i = 0; i < m_iLinks; i++) 
			{
				if(::PtInRect(&m_rcLinks[i], event.ptMouse))
				{
					Invalidate();
					return true;
				}
			}
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			for(int i = 0; i < m_iLinks; i++) 
			{
				if(::PtInRect(&m_rcLinks[i], event.ptMouse))
				{
					SendNotify(UINOTIFY_CLICK_LINK, (WPARAM)i, (LPARAM)&m_strLinks[i]);
					return true;
				}
			}
		}
		break;
	case UIEVENT_SETCURSOR:
		{
			for(int i = 0; i < m_iLinks; i++) 
			{
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) )
				{
					::SetCursor(::LoadCursor(NULL, IDC_HAND));
					return true;
				}
			}
		}
		break;
	case UIEVENT_MOUSEMOVE:
		{
			if (m_iLinks > 0)
			{
				int iHoverLink = -1;
				m_sHoverLink = _T("");
				for(int i = 0; i < m_iLinks; i++) 
				{
					if(::PtInRect(&m_rcLinks[i], event.ptMouse))
					{
						iHoverLink = i;
						m_sHoverLink = m_strLinks[iHoverLink];
						break;
					}
				}
				if (m_iHoverLink != iHoverLink)
				{
					m_iHoverLink = iHoverLink;
					Invalidate();
					return true;			
				}
			}
		}
		break;
	case UIEVENT_MOUSELEAVE:
		{
			if(m_iLinks > 0)
			{
				if (m_iHoverLink != -1)
				{
					m_iHoverLink = -1;
					m_sHoverLink = _T("");
					Invalidate();
					return true;
				}
			}
		}
		break;
	case UIEVENT_SETFOCUS:
		{
			SendNotify(UINOTIFY_SETFOCUS, event.wParam, event.lParam);
		}
		break;
	}
	
	return CControlUI::Event(event);
}
