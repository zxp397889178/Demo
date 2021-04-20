#include "stdafx.h"
#include "UIInteractOptionLayout.h"
#include <WinDef.h>
#include "Common/Common.h"


CInteractOptionLayoutUI::CInteractOptionLayoutUI()
{
	m_nOverlappadX	= 0;
	m_nItemMaxWidth	= 150;
	m_bCrossOverlappad = true;
	m_bItemAutoSize = false;
}

CInteractOptionLayoutUI::~CInteractOptionLayoutUI()
{
	ClearMap();
}

void CInteractOptionLayoutUI::Init()
{
	__super::Init();

}


bool CInteractOptionLayoutUI::Insert(CControlUI* pControlChild, CControlUI* pControlAfter)
{
	if (!pControlChild)
	{
		return false;
	}

// 	do
// 	{
// 		ImageStyle* pBKImageStyle = pControlChild->GetImageStyle(_T("bk"));
// 		if (!pBKImageStyle
// 			|| !pBKImageStyle->LoadImage())
// 		{
// 			break;
// 		}
// 
// 		ImageObj* pImageObj = pBKImageStyle->GetImageObj();
// 		if (!pImageObj)
// 		{
// 			break;
// 		}
// 
// 		HRGN hRgn = util::CreateRgnByPng(pImageObj->GetBitmap());
// 		if (!hRgn)
// 		{
// 			break;
// 		}
// 
// 		m_mapCtrlsRgn[pControlChild] = hRgn;
// 
// 	} while (false);

	return __super::Insert(pControlChild, pControlAfter);
}

bool CInteractOptionLayoutUI::Add(CControlUI* pControlChild)
{
	if (!pControlChild)
	{
		return false;
	}

	
// 	do
// 	{
// 		ImageStyle* pBKImageStyle = pControlChild->GetImageStyle(_T("bk"));
// 		if (!pBKImageStyle
// 			|| !pBKImageStyle->LoadImage())
// 		{
// 			break;
// 		}
// 
// 		ImageObj* pImageObj = pBKImageStyle->GetImageObj();
// 		if (!pImageObj)
// 		{
// 			break;
// 		}
// 
// 		HRGN hRgn = util::CreateRgnByPng(pImageObj->GetBitmap());
// 		if (!hRgn)
// 		{
// 			break;
// 		}
// 
// 		m_mapCtrlsRgn[pControlChild] = hRgn;
// 
// 	} while (false);

	return __super::Add(pControlChild);
}

bool CInteractOptionLayoutUI::SelectControl(CControlUI* pItem)
{
	if (!pItem)
	{
		return false;
	}

	return __super::SelectControl(pItem);
}

void CInteractOptionLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("OverlappadX")) == 0)
	{
		m_nOverlappadX = _ttoi(lpszValue);
	}
	else if (_tcsicmp(lpszName, _T("ItemMaxWidth")) == 0)
	{
		m_nItemMaxWidth = _ttoi(lpszValue);
	}
	else if (_tcsicmp(lpszName, _T("CrossOverlappad")) == 0)
	{
		m_bCrossOverlappad = !!_ttoi(lpszValue);
	}
	else if (_tcsicmp(lpszName, _T("ItemAutoSize")) == 0)
	{
		m_bItemAutoSize = !!_ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
	
}

bool CInteractOptionLayoutUI::Event(TEventUI& event)
{
	return __super::Event(event);
}

void CInteractOptionLayoutUI::MoveItem(int nIndx, int nAfterIndex)
{
	CControlUI* pItem = GetItem(nIndx);
	CControlUI* pAfterItem = GetItem(nAfterIndex);

	if (!pItem
		|| !pAfterItem)
	{
		return;
	}

	__super::MoveItem(pItem, pAfterItem);
}

void CInteractOptionLayoutUI::SetRect(RECT& rectRegion)
{
	bool bCanHoldChild = LayoutChild();
	if (!bCanHoldChild)
	{
		bool bAutoSize = m_bItemAutoSize;
		m_bItemAutoSize = false;
		ResetChildWidth();
		m_bItemAutoSize = bAutoSize;

	}

	__super::SetRect(rectRegion);
}

bool CInteractOptionLayoutUI::LayoutChildInternal(int nParentWidth, int nParentHeight, int nItemDefaultWidth, int nModWidth)
{
	int nXBase = 0;
	int nOverlappdOffset = nXBase;

	int nShiftXBase = 0;
	int nShiftOverlappdOffset = nShiftXBase;
	for (int i = 0; i < this->GetCount(); ++i)
	{
		CControlUI* pItem = GetItem(i);
		if (!pItem
			|| !pItem->IsVisible())
		{
			continue;
		}

		Coordinate coorItem = *pItem->GetCoordinate();
		int* pOverlappdOffset = NULL;
		int* pXBase = NULL;
		//目前支持 LT， RT
		switch (coorItem.anchor)
		{
			case UIANCHOR_LT:
			default:
				{
					coorItem.anchor = UIANCHOR_LT; 
					pOverlappdOffset = &nOverlappdOffset;
					pXBase = &nXBase;
				}
				break;
			case UIANCHOR_RT:
				{
					pOverlappdOffset = &nShiftOverlappdOffset;
					pXBase = &nShiftXBase;
				}
				break;
		}

		int nDiff = *pOverlappdOffset - DPI_SCALE(m_nOverlappadX);
		if (nDiff < *pXBase)
		{
			*pOverlappdOffset = *pXBase;
		}
		else
		{
			*pOverlappdOffset = nDiff;
		}

		coorItem.rect.left = *pOverlappdOffset;

		int nTxtWidth = 0;
		if (m_bItemAutoSize)
		{
			nTxtWidth = GetItemWidth(pItem);
		}
		
		if (nTxtWidth <= 0)
		{
			if (pItem->IsExceptionGroup())
			{
				coorItem.rect.right = DPI_SCALE(pItem->GetCoordinate()->rect.right);
			}
			else
			{
				coorItem.rect.right = nItemDefaultWidth;
				int nAppendWdith = DPI_SCALE(1);
				nModWidth -= nAppendWdith;
				if (nModWidth > 0)
				{
					coorItem.rect.right += nAppendWdith;
				}
			}
		}
		else
		{
			coorItem.rect.right = nTxtWidth;
			int nAppendWdith = DPI_SCALE(1);
			nModWidth -= nAppendWdith;
			if (nModWidth > 0)
			{
				coorItem.rect.right += nAppendWdith;
			}
		}
		//只处理横向
		*pOverlappdOffset += coorItem.rect.right;

		coorItem.rect.left = DPI_SCALE_BACK(coorItem.rect.left);
		coorItem.rect.right = DPI_SCALE_BACK(coorItem.rect.right);

		coorItem.rect.top = 0;
		coorItem.rect.bottom = DPI_SCALE_BACK(nParentHeight);

		//need reset width
		if (nOverlappdOffset + nShiftOverlappdOffset > nParentWidth)
		{
			return false;
		}

		//need reset width
		if (nOverlappdOffset + nShiftOverlappdOffset > nParentWidth)
		{
			return false;
		}

		pItem->SetCoordinate(coorItem);

	}

	return true;
}

bool CInteractOptionLayoutUI::LayoutChild()
{
	RECT rectParent = GetInterRect();
	int nParentWidth = rectParent.right - rectParent.left;
	int nParentHeight = rectParent.bottom - rectParent.top;
	if (nParentWidth <= 0)
	{
		return true;
	}

	return LayoutChildInternal(nParentWidth, nParentHeight, DPI_SCALE(m_nItemMaxWidth), 0);

}

void CInteractOptionLayoutUI::ResetChildWidth()
{
	RECT rectParent = GetInterRect();
	int nParentWidth = rectParent.right - rectParent.left;
	int nParentHeight = rectParent.bottom - rectParent.top;
	int nCount = this->GetCount();

	int nExceptCtrlsWidth = 0;
	bool bHasCtrlOnLeft = false;
	bool bHasCtrlOnRight = false;
	bool bHasExceptCtrlOnLeft = false;
	bool bHasExceptCtrlOnRight = false;
	int nCtrlsOverlappadX = 0;
	int nExceptCtrlsCount = 0;
	for (int i = 0; i < nCount; ++i)
	{
		CControlUI* pItem = GetItem(i);
		if (!pItem
			|| !pItem->IsVisible())
		{
			continue;
		}

		if (!pItem->IsExceptionGroup())
		{
			if (bHasCtrlOnLeft
				|| bHasCtrlOnRight)
			{
				nCtrlsOverlappadX += m_nOverlappadX;
			}

			//目前支持 LT， RT
			switch (pItem->GetCoordinate()->anchor)
			{
				case UIANCHOR_LT:
				default:
					{
						bHasCtrlOnLeft = true;
						bHasCtrlOnRight = false;
					}
					break;
				case UIANCHOR_RT:
					{
						bHasCtrlOnLeft = false;
						bHasCtrlOnRight = true;
					}
					break;
			}
		}
		else
		{
			if (bHasExceptCtrlOnLeft
				|| bHasExceptCtrlOnRight)
			{
				nExceptCtrlsWidth -= m_nOverlappadX;
			}

			//默认LT
			nExceptCtrlsWidth += pItem->GetCoordinate()->rect.right;
			nExceptCtrlsCount++;

			//目前支持 LT， RT
			switch (pItem->GetCoordinate()->anchor)
			{
				case UIANCHOR_LT:
				default:
					{
						bHasExceptCtrlOnLeft = true;
						bHasExceptCtrlOnRight = false;
					}
					break;
				case UIANCHOR_RT:
					{
						bHasExceptCtrlOnLeft = false;
						bHasExceptCtrlOnRight = true;
					}
					break;
			}
		}
	}

	//ctrls count except non-group ctrl
	int nGroupCtrlsCount = nCount - nExceptCtrlsCount;
    if (nGroupCtrlsCount <= 0)
    {
        return;
    }

	int nDiffWith = nParentWidth - DPI_SCALE(nExceptCtrlsWidth) + DPI_SCALE(nCtrlsOverlappadX);
	if (m_bCrossOverlappad)
	{
		nDiffWith += !nExceptCtrlsCount ? 0 : DPI_SCALE(m_nOverlappadX);
	}
	int nNewWidth = nDiffWith / nGroupCtrlsCount;
	int nModWidth = nDiffWith % nGroupCtrlsCount;
	LayoutChildInternal(nParentWidth, nParentHeight, nNewWidth, nModWidth);
}

CControlUI* CInteractOptionLayoutUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	do 
	{
		CStdPtrArray* items = GetItemArray();
		if (!items)
		{
			break;
		}

		CControlUI* pControl = GetCurSelControl();
		if (!pControl)
		{
			break;
		}

		int nCur = pControl->GetIndex();
		int nCount = items->GetSize();
		for (int it = nCur; it >= 0; it--)
		{
			pControl = static_cast<CControlUI*>((*items)[it]);
			if (NULL == pControl) continue;

			pControl = pControl->FindControl(Proc, pData, uFlags | UIFIND_ME_FIRST);
			if (pControl != NULL)
				return pControl;
		}


		for (int it = nCur; it < nCount; it++)
		{
			pControl = static_cast<CControlUI*>((*items)[it]);
			pControl = pControl->FindControl(Proc, pData, uFlags | UIFIND_ME_FIRST);
			if (pControl != NULL)
				return pControl;
		}

	} while (false);
	
	return __super::FindControl(Proc, pData, uFlags);
}

bool CInteractOptionLayoutUI::Remove(CControlUI* pControlChild)
{
	auto itor = m_mapCtrlsRgn.find(pControlChild);
	if (itor != m_mapCtrlsRgn.end())
	{
		::DeleteObject(itor->second);
		m_mapCtrlsRgn.erase(itor);
	}

	return __super::Remove(pControlChild);
}

void CInteractOptionLayoutUI::RemoveAll()
{
	ClearMap();

	return __super::RemoveAll();
}

void CInteractOptionLayoutUI::ClearMap()
{
	for (auto& itor : m_mapCtrlsRgn)
	{
		::DeleteObject(itor.second);
	}

	m_mapCtrlsRgn.clear();
}

void CInteractOptionLayoutUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	int nCount = this->GetCount();
	CControlUI* pCur = this->GetCurSelControl();
	if (pCur)
	{
		for (int i = 0; i < nCount; ++i)
		{
			CControlUI* pItem = GetItem(i);
			if (!pItem)
			{
				continue;
			}

			if (pCur == pItem)
			{
				pItem->SetZOrder(-1);
				continue;
			}

			pItem->SetZOrder(nCount - i);
		}
	}
	
	__super::Render(pRenderDC, rcPaint);
}

int CInteractOptionLayoutUI::GetItemMaxWidth()
{
	return m_nItemMaxWidth;
}

int CInteractOptionLayoutUI::GetItemWidth(CControlUI* pItem)
{
	if (!pItem
		|| !pItem->IsVisible())
	{
		return 0;
	}

	TextStyle* pTxtStyle = pItem->GetTextStyle(_T("txt"));
	if (!pTxtStyle)
	{
		return 0;
	}

	RECT rect = pTxtStyle->GetRect();
	RECT rect1 = rect;
	OffsetRect(&rect, -rect.left, -rect.top);

	LPCTSTR lptcsText = pTxtStyle->GetText();
	if (_tcsicmp(lptcsText, _T("")) == 0)
	{
		lptcsText = pItem->GetText();
	}

	pTxtStyle->GetFontObj()->CalcText(GetWindow()->GetPaintDC(), rect, lptcsText, pTxtStyle->GetAlign());
	int nTxtWidth = (rect.right - rect.left);
	nTxtWidth += 10;

	if (nTxtWidth > GetItemMaxWidth())
	{
		nTxtWidth = GetItemMaxWidth();
	}

	return nTxtWidth;
}

