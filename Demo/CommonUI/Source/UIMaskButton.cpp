#include "stdafx.h"
#include <math.h>

#include "UIMaskButton.h"

#define  IMAGE_FLAG_MASK 0x00000001
CMaskButtonUI::CMaskButtonUI(void)
{
	m_pImageStyle = NULL;
	m_pMaskStyle = NULL;
	m_strImageStyleName = _T("icon");
	m_strMaskStyleName = _T("mask");
	SetStyle(_T("MaskButton"));
}

CMaskButtonUI::~CMaskButtonUI(void)
{

}

ImageStyle* CMaskButtonUI::GetImageStyle()
{
	if (!m_pImageStyle)
	{
		m_pImageStyle = __super::GetImageStyle(m_strImageStyleName.c_str());
	}
	return m_pImageStyle;

}

ImageStyle* CMaskButtonUI::GetMaskStyle()
{
	if (!m_pMaskStyle)
	{
		m_pMaskStyle = __super::GetImageStyle(m_strMaskStyleName.c_str());
	}
	return m_pMaskStyle;

}

void CMaskButtonUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	do 
	{
		ImageStyle* pImageStyle = GetImageStyle();
		ImageStyle* pMaskStyle = GetMaskStyle();
		if (!pImageStyle || !pMaskStyle)
		{
			break;
		}

		if (!pImageStyle->GetImageObj()
			|| !pMaskStyle->GetImageObj())
		{
			break;
		}
		
		if (pImageStyle->GetImageObj()->GetWidth() != pMaskStyle->GetImageObj()->GetWidth())
		{
			CImageLib::DoResize(pImageStyle->GetImageObj(),
				pMaskStyle->GetImageObj()->GetWidth(),
				pMaskStyle->GetImageObj()->GetHeight(),
				pMaskStyle->GetPaintParams());
		}

		if (pImageStyle->GetImageObj()->GetFlags() != IMAGE_FLAG_MASK)
		{
			CImageLib::DoMask(pImageStyle->GetImageObj(), pMaskStyle->GetImageObj());
			pImageStyle->GetImageObj()->SetFlags(IMAGE_FLAG_MASK);
		}

	} while (false);

	__super::Render(pRenderDC, rcPaint);

}

void CMaskButtonUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
// 	if (_tcsicmp(lpszName, _T("imagestyle")) == 0)
// 	{
// 		m_strImageStyleName = lpszValue;
// 	}
// 	else if (_tcsicmp(lpszName, _T("maskstyle")) == 0)
// 	{
// 		m_strMaskStyleName = lpszValue;
// 	}
// 	else
// 	{
// 		__super::SetAttribute(lpszName, lpszValue);
// 	}
	__super::SetAttribute(lpszName, lpszValue);
}