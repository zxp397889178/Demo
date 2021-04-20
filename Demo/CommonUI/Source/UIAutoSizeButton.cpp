#include "stdafx.h"
#include "UIAutoSizeButton.h"

CAutoSizeButtonUI::CAutoSizeButtonUI()
{
	SetStyle(_T("Button"));
}

void CAutoSizeButtonUI::SetText(LPCTSTR lpszText)
{
	__super::SetText(lpszText);
	if (GetParent())
	{
		GetParent()->OnlyResizeChild();
	}
}

UITYPE_ANCHOR GetAnchorHori(Coordinate& coordinate)
{
	UITYPE_ANCHOR anchorHori;
	switch(coordinate.anchor)
	{
	case UIANCHOR_LTRT:
	case UIANCHOR_LBRB:
	case UIANCHOR_LTRB:
		{
			anchorHori = UIANCHOR_LT;
		}
		break;
	case UIANCHOR_LTLB:
	case UIANCHOR_LEFTTOP:
	case UIANCHOR_LEFTBOTTOM:
		{
			anchorHori = UIANCHOR_LTLB;
		}
		break;
	case UIANCHOR_RTRB:
	case UIANCHOR_RIGHTTOP:
	case UIANCHOR_RIGHTBOTTOM:
		{
			anchorHori = UIANCHOR_RTRB;
		}
		break;
	case UIANCHOR_LT:
		{
			if (coordinate.rect.right >= 0)
				anchorHori = UIANCHOR_LTLB;
			else
				anchorHori = UIANCHOR_RTRB;
		}
		break;
	case UIANCHOR_RT:
		{
			anchorHori = UIANCHOR_RT;
		}
		break;
	default:
		{
			anchorHori = UIANCHOR_LTLB;
		}
		break;
	}
	return anchorHori;

}

void CAutoSizeButtonUI::SetRect(RECT& rectRegion)
{
	CViewUI::SetRect(rectRegion);

	TextStyle* pStyle = GetTextStyle(_T("txt"));
	

	if (pStyle)
	{
		tstring strText = m_strText;
		do
		{
			if (IsEnabled())//禁用下走一下流程，使用禁用态的文本
			{
				break;
			}
			auto pDis = GetTextStyle(_T("txt_disable"));
			if (!pDis)
			{
				break;
			}
			tstring strDisable = pDis->GetText();
			if (!strDisable.empty())
			{
				strText = strDisable;
			}
		} while (false);
		pStyle->OnSize(rectRegion);
		RECT rect = pStyle->GetRect();

		RECT rect1 = rect;
		OffsetRect(&rect, -rect.left, -rect.top);
		pStyle->GetFontObj()->CalcText(GetWindow()->GetPaintDC(), rect, strText.c_str(), pStyle->GetAlign());
		if (!pStyle->IsVisible() && !m_bAutoSizeEvenHideTxt)
		{
			rect = { 0, 0, 0, 0 };
			rect1 = { 0, 0, 0, 0 };
		}
		int nOffset = (rect.right - rect.left) - (rect1.right - rect1.left);
		if (nOffset > 0)
		{
			//GetAnchorHori 先拷贝一份，可以考虑由DirectUI开放该函数
			UITYPE_ANCHOR anchor = GetAnchorHori(*GetCoordinate());
			RECT rcItem = GetRect();
			switch(anchor)
			{
			case UIANCHOR_LT:
			case UIANCHOR_LTLB:
				rcItem.right += nOffset;
				break;
			case UIANCHOR_RT:
			case UIANCHOR_RTRB:
				rcItem.left -= nOffset;
				break;
			}
			//RECT rcItem = GetRect();
			//rcItem.right += nOffset;
			int nMaxCx = DPI_SCALE(GetMaxSize().cx);
			if (nMaxCx != 0 && rcItem.right - rcItem.left > nMaxCx)
			{
				rcItem.right = rcItem.left + nMaxCx;
			}

			CControlUI* pParent = GetParent();
			RECT rectParent = pParent->GetInterRect();
			int nParentMaxCx = pParent->GetMaxSize().cx;
			if (nParentMaxCx != 0)
			{
				//高分屏的需要加偏移
				int nScaleOffset = GetUIEngine()->GetDPI() > 96 ? 1 : 0;
				nParentMaxCx = DPI_SCALE(nParentMaxCx + nScaleOffset);
				if (rcItem.right > rectParent.left + nParentMaxCx)
				{
					rcItem.right = rectParent.left + nParentMaxCx;
				}

				rectParent.right = rcItem.right;

				//去掉SetRect重载
				*(pParent->GetRectPtr()) = rectParent;	
			}
			else
			{
				if (rcItem.right > rectParent.right)
					rcItem.right = rectParent.right;
			}
			
			/*if (m_nMaxHeight != 0 && rcItem.bottom - rcItem.top > m_nMaxHeight)
			{
				rcItem.bottom = rcItem.top + m_nMaxHeight;
			}*/
			CViewUI::SetRect(rcItem);
		}
	}
	FlipRect();
	int nCnt = GetStyleCount();
	RECT rcItem = GetRect();
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
		if (pStyleObj)
			pStyleObj->OnSize(rcItem);
	}
}

void CAutoSizeButtonUI::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{	
	if (_tcsicmp(lpszName, _T("AutoSizeEvenHideTxt")) == 0)
	{
		m_bAutoSizeEvenHideTxt = !!_ttoi(lpszValue);
	}

	__super::SetAttribute(lpszName, lpszValue);
}

