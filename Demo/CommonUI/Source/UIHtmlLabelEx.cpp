#include "stdafx.h"
#include "UIHtmlLabelEx.h"

CHtmlLabelExUI::CHtmlLabelExUI()
{

}

CHtmlLabelExUI::~CHtmlLabelExUI()
{

}

int CHtmlLabelExUI::CalcWidth(int nHeight)
{
	if (m_strText.empty() || !GetHtmlLabelStyle())
	{
		return 0;
	}

	if (!this->GetWindow())
	{
		return 0;
	}

	HDC hDC = this->GetWindow()->GetPaintDC();
	m_dib.Create(hDC, 1, nHeight);
	//m_dib.Fill(RGB(255, 255, 255), 255);

	HtmlPaintParams paintParams;
	m_iLinks = sizeof(m_rcLinks) / sizeof(*m_rcLinks);

	IRenderDC render;
	render.SetDevice(&m_dib);

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = 9999;
	rc.bottom = nHeight;

	bool bEndEllipsis = false;
	render.DrawHtmlText(GetHtmlLabelStyle()->GetFontObj(), rc, m_strText.c_str(), 0, m_rcLinks, m_strLinks, m_iLinks,
		m_sHoverLink, GetLinkTextColor(), GetLinkOverTextColor(), m_uAlignStyle | DT_SINGLELINE | DT_CALCRECT, paintParams, bEndEllipsis, GetHtmlLabelStyle()->GetFontType());

	return rc.right;
}
