/*********************************************************************
*            Copyright (C) 2010, 网龙天晴程序应用软件部
*********************************************************************/
#include "stdafx.h"
#include "UIList.h"




//////////////////////////////////////////////////////////////////////////

INode::INode(bool bGroup) 
:m_pArrChildNode(NULL),m_pParentNode(NULL),m_bNodeExpand(false),m_pNodeOwner(NULL),m_bNodeGroup(bGroup)
{
}

INode::~INode()
{
	INode* pParent = get_parent_node();
	if (pParent)
	{
		pParent->remove_child_node(this);
	}
	remove_all_node();
	m_pArrChildNode = NULL;
	m_pParentNode = NULL;
	m_pNodeOwner = NULL;
	m_bNodeExpand = false;
}

bool INode::node_expand( bool bExpand, UINT uFlags /*= UINODE_EXPAND_TOROOT | UINODE_EXPAND_UPDATE*/ )
{
	bool bLeaf = uFlags & UINODE_EXPAND_TOLEAF;
	if (bLeaf)
	{
		int nCnt = num_child_node();
		if (nCnt > 0)
		{
			INode* begin = get_child_node(0);
			INode* end = get_child_node(nCnt-1);
			//INode* pNode = NULL;
			INodeOwner* pOwner = get_node_owner();
			if (begin != NULL && end != NULL && pOwner != NULL)
			{
				for( int i = 0; i < nCnt; i++ )
				{
					INode* pNode = get_child_node(i);
					if (pNode->is_node_group())
						pNode->node_expand(bExpand, uFlags & ~UINODE_EXPAND_TOROOT);
				}
			}
		}
	}

	bool bRoot = !!(uFlags & UINODE_EXPAND_TOROOT);
	bool bResize = !!(uFlags & UINODE_EXPAND_UPDATE);
	bool bExpandToRoot = is_node_expand();
	bool bSelf = is_node_expand(~UINODE_EXPAND_TOROOT);
	if( (bRoot && bExpandToRoot == bExpand) ||  (!bRoot && bSelf == bExpand) ) 
		return false;
	m_bNodeExpand = bExpand;

	INodeOwner* pOwner = get_node_owner();
	if (is_node_group() && pOwner && this != pOwner->get_root())
		pOwner->expanding(this, bExpand);

	if (bExpand && get_parent_node() && !bExpandToRoot )
		get_parent_node()->node_expand(bExpand, uFlags & ~UINODE_EXPAND_UPDATE);

	if(pOwner && num_child_node() > 0 )
	{
		INode* begin = get_child_node(0);
		INode* end = get_last_child_node();
		if (!bExpand)
		{
			int nBegin = begin->get_node_index();
			int nEnd = end->get_node_index();
			for( int i = nBegin; i <= nEnd; i++ )
				pOwner->get_node(i)->node_show(false);
		}
		else
		{
			int m = begin->get_node_index();
			int n = end->get_node_index();
			//INode* pNode = NULL;
			for( int i = m; i <= n; ++i )
			{
				INode* pNode = pOwner->get_node(i);
				if (pNode)
				{
					//父节点展开且可见
					if(pOwner->node_show_ploy(pNode))
						pNode->node_show(true);
					else
						pNode->node_show(false);
				}
			}
		}
	}
	if (is_node_group() && pOwner && this != pOwner->get_root())
		pOwner->expandend(this, bExpand);
	if (bResize && get_node_owner())
	{
		get_node_owner()->NeedUpdate();
		return true;
	}
	return false;
}

bool INode::is_node_expand(UINT uFlags /*= UINODE_EXPAND_TOROOT*/)
{
	if (uFlags & UINODE_EXPAND_TOROOT)
	{
		INode* _rootNode = NULL;
		INode* _ParentNode = get_parent_node();
		bool bRet = m_bNodeExpand;
		if (m_pNodeOwner)
		{
			_rootNode = m_pNodeOwner->get_root();
		}
		while (_rootNode && _ParentNode && _rootNode != _ParentNode)
		{
			bRet = bRet && _ParentNode->is_node_expand(~UINODE_EXPAND_TOROOT);
			_ParentNode = _ParentNode->get_parent_node();
		}
		return bRet;
	}
	else
	{
		return m_bNodeExpand;
	}
}

bool INode::is_node_group()
{
	return m_bNodeGroup;
}

void INode::node_group( bool bGroup )
{
	m_bNodeGroup = bGroup;
}

void INode::add_child_node( INode* child, int nIndex )
{
	if (!m_pArrChildNode)
		m_pArrChildNode = new CStdPtrArray;
	child->set_node_owner(get_node_owner());
	child->m_pParentNode = this;
//	int nChilds = num_child_node();
	if (num_child_node() == 0)
	{
		m_pArrChildNode->Add(child);
	}
	else
	{
		if (nIndex < num_child_node())
			m_pArrChildNode->InsertAt(nIndex, child);
		else
			m_pArrChildNode->Add(child);
	}
}

void INode::set_child_node( INode* child, int nIndex )
{
	if (!m_pArrChildNode)
		m_pArrChildNode = new CStdPtrArray;
	child->m_pParentNode = this;
	m_pArrChildNode->SetAt(nIndex, child);
}

INode* INode::get_child_node( int nIndex )
{
	if (!m_pArrChildNode)
		return NULL;
	return (INode*)(m_pArrChildNode->GetAt(nIndex));
}

INode* INode::get_last_child_node()
{
	if( num_child_node() > 0 )
	{
		return get_child_node(num_child_node() - 1)->get_last_child_node();
	}
	else 
	{
		return this;
	}
}

void INode::remove_child_node( INode* child )
{
	if (!m_pArrChildNode)
		return;
	m_pArrChildNode->Remove(child);
}

void INode::remove_all_node()
{
	int nCnt = num_child_node();
	//INode* pNode = NULL;
	for (int i = 0; i < nCnt; i++)
	{
		INode* pNode = get_child_node(i);
		if (pNode)
		{
			pNode->m_pParentNode = NULL;
			pNode->remove_all_node();
		}
	}
	if (m_pArrChildNode)
		m_pArrChildNode->Empty();
}

INode* INode::get_parent_node()
{
	return m_pParentNode;
}

int INode::num_child_node()
{
	if (!m_pArrChildNode)
		return 0;
	return m_pArrChildNode->GetSize();
}

INodeOwner* INode::get_node_owner()
{
	return m_pNodeOwner;
}

void INode::set_node_owner( INodeOwner* pOwner )
{
	m_pNodeOwner = pOwner;
}

CStdPtrArray* INode::GetNodeArray()
{
	return m_pArrChildNode;
}

INode* INode::get_nextsibling_node( INode* pNode )
{
	int nCnt = num_child_node();
	//INode* pFindNode = NULL;
	for (int i = 0; i < nCnt; i++)
	{
		INode* pFindNode = get_child_node(i);
		if (pFindNode && pFindNode == pNode)
		{
			return get_child_node(i + 1);
		}
	}
	return NULL;
}

INode* INode::get_prevsibling_node( INode* pNode )
{
	int nCnt = num_child_node();
	//INode* pFindNode = NULL;
	for (int i = 0; i < nCnt; i++)
	{
		INode* pFindNode = get_child_node(i);
		if (pFindNode && pFindNode == pNode)
		{
			return get_child_node(i - 1);
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
INodeOwner::INodeOwner()
:m_pNodeRoot(NULL)
{
}

INode* INodeOwner::get_root()
{
	return m_pNodeRoot;
}

void INodeOwner::set_root( INode* pRoot )
{
	m_pNodeRoot = pRoot;
}

bool INodeOwner::node_show_ploy( INode* pNode )
{
	return pNode->get_parent_node()->is_node_expand() && pNode->get_parent_node()->is_node_show();
}

//////////////////////////////////////////////////////////////////////////
class CListItemCheckUI : public CCheckBoxUI
{
public:
	CListItemCheckUI()
	{
		SetNeedScroll(false);
		SetStyle(_T("CheckListItem"));
	}
protected:
	virtual bool Event(TEventUI& event)
	{
		switch (event.nType)
		{
		case UIEVENT_BUTTONUP:
			{
				CControlUI* pParent = GetParent();
				if (pParent)
				{
					CListUI* pList = dynamic_cast<CListUI*>(GetOwner());
					if (!pList)
					{
						CListUI* pOwner = dynamic_cast<CListUI*>(pParent);
						if (pOwner)
						{
							int nIndex = GetOwner()->GetIndex();
							pOwner->CheckItem(!pOwner->IsCheckItem(nIndex), nIndex, UIITEMFLAG_CHECKED | UIITEMFLAG_ALL | UIITEMFLAG_NOTIFY);
							pOwner->Invalidate();
						}
					}
					else
					{
						CListUI* pOwner = dynamic_cast<CListUI*>(pParent);
						if (pOwner)
						{
							bool bCheck = !IsChecked();
							pOwner->CheckItem(bCheck, -1, UIITEMFLAG_CHECKED | UIITEMFLAG_ALL | UIITEMFLAG_NOTIFY);
							pOwner->Invalidate();
						}
					}
				}
				return true;
			}
			break;
		}
		return __super::Event(event);
	}
private:
};


//////////////////////////////////////////////////////////////////////////
#define _Style_List_Bk 0
#define _Style_List_Item 1
#define _Style_List_ItemBkg 2
#define _Style_List_BoundImage 3
#define _Style_ListCtrl_Item 1
#define _Style_ListItem_Bk 0
#define _Style_ListItem_Icon 1
#define _Style_ListItem_Txt 2
#define _Style_ListHeaderItem_Bk 0
#define _Style_ListHeaderItem_Sort 1
#define _Style_ListHeaderItem_Txt 2
#define ListHeaderItemMinWidth DPI_SCALE(12)
//////////////////////////////////////////////////////////////////////////
CListItemUI*  CListUI::GetCurSel()
{
	return static_cast<CListItemUI*>(GetCurSelControl());
}

bool CListUI::SelectItem(CControlUI* pControl)
{
	if (m_bEnableItemOverOnly)
		return true;

	CControlUI* pSub = NULL;
	int nCnt = GetCount();
	int nIndex = 0;
	int nSel = -1;
	int nControl = -1;
	bool bChange = false;
	if (GetCurSel())
		nSel = GetCurSel()->GetIndex();
	if (pControl)
		nControl = pControl->GetIndex();

	GetWindow()->LockUpdate();
	for (int i = 0; i < nCnt; i++)
	{
		pSub = GetItem(i);
		if (pSub)
		{
			if (pSub->IsSelected())
			{
				nIndex = pSub->GetIndex();
				if (nIndex != nSel || nIndex != nControl)
				{
					bChange = true;
					pSub->Select(false);
					
				}
			}
		}
	}
	GetWindow()->UnLockUpdate();

	SelectControl(pControl);
	if (bChange)
	{
		SendNotify(UINOTIFY_SELCHANGE, NULL, (LPARAM)GetCurSel());
		Invalidate();
	}
	return true;
}

bool CListUI::SelectItemAdd( CControlUI* pControl )
{
	if (m_bEnableItemOverOnly)
		return true;

	if (pControl)
	{
		if (GetCurSel() == pControl)
		{
			if (pControl->IsSelected())
				SelectControl(NULL);
		}
		else
		{
			if (!pControl->IsSelected())
				SelectControl(pControl);
		}

		pControl->Select(!pControl->IsSelected());

		SendNotify(UINOTIFY_SELCHANGE, NULL, (LPARAM)pControl);
		return true;
	}
	return false;
}

bool CListUI::SelectItemEnd(CControlUI* pControl)
{
	if (pControl)
	{
		int nA = pControl->GetIndex();
		int nB = nA;
		if (GetCurSel())
			nA = GetCurSel()->GetIndex();
		else
			nA = 0;
		int nBegin = min(nA, nB);
		int nEnd = max(nA, nB);
		int nCnt = GetCount();
		int nIndex = 0;
		CListItemUI* pSub = NULL;
		for (int i = 0; i < nCnt; i++)
		{
			pSub = static_cast<CListItemUI*>(GetItem(i));
			if (pSub)
			{
				nIndex = pSub->GetIndex();
				if (nIndex < nBegin || nIndex > nEnd)
					pSub->Select(false);
				else
					pSub->Select(true);
			}
		}
		Invalidate();
		SendNotify(UINOTIFY_SELCHANGE, NULL, (LPARAM)pControl);
		return true;
	}
	return false;
}

bool CListUI::SelectItemAll( bool bSelect /*= true*/ )
{
	int nCnt = GetCount();
	//int nIndex = 0;
	//CListItemUI* pControl = NULL;
	for (int i = 0; i < nCnt; i++)
	{
		CListItemUI* pControl = static_cast<CListItemUI*>(GetItem(i));
		if (pControl)
			pControl->Select(bSelect);
	}
	Invalidate();
	SendNotify(UINOTIFY_SELCHANGE, NULL, NULL);
	return true;
}

CControlUI*  CListUI::ItemFromPos(POINT& pt)
{
	//CControlUI* pControl = NULL;
	for (int i = m_nFirstVisible; i < m_nLastVisible; i++)
	{
		CControlUI* pControl = GetItem(i);
		if (pControl && pControl->HitTest(pt))
			return pControl;
	}
	return NULL;
}

void CListUI::EnsureVisible(CControlUI* pControl)
{
	CScrollbarUI* pScrollbar = GetVScrollbar();
	if (pScrollbar && pScrollbar->IsVisible() && pControl)
	{
		m_bLockChildsRect = false;
		Resize();
		RECT rcItem = pControl->GetRect();
		RECT rcClient = GetClientRect();
		if( rcItem.bottom > rcClient.bottom)
			pScrollbar->SetScrollPos(pScrollbar->GetScrollPos() + rcItem.bottom - rcClient.bottom);
		else if( rcItem.top < rcClient.top)
			pScrollbar->SetScrollPos(pScrollbar->GetScrollPos() + rcItem.top - rcClient.top);
		m_bLockChildsRect = true;
	}

}

void CListUI::EnsureVisibleFirst( CControlUI* pControl )
{
	CScrollbarUI* pVerticalScrollbar = GetVScrollbar();
	if (pVerticalScrollbar && pVerticalScrollbar->IsVisible() && pControl)
	{
		m_bLockChildsRect = false;
		Resize();
		RECT rcItem = pControl->GetRect();
		RECT rcClient = GetClientRect();
		pVerticalScrollbar->SetScrollPos(pVerticalScrollbar->GetScrollPos() + rcItem.top - rcClient.top);
		m_bLockChildsRect = true;
	}
}

int CListUI::EnsureVisibleCenter( CControlUI* pControl,int& nBegin, int &nEnd, bool bRedraw /*= true*/ )
{
	int nLastScrollPos = 0;
	int nNewScrollPos = 0;
	CScrollbarUI* pVerticalScrollbar = GetVScrollbar();
	if (pVerticalScrollbar && pVerticalScrollbar->IsVisible() && pControl)
	{
		m_bLockChildsRect = false;
		Resize();
		RECT rcItem = pControl->GetRect();
		RECT rcClient = GetClientRect();
		int nOffset = -(rcClient.top - rcItem.bottom + (rcClient.bottom - rcClient.top)/3);
		nLastScrollPos = pVerticalScrollbar->GetScrollPos();
		pVerticalScrollbar->SetScrollPos(pVerticalScrollbar->GetScrollPos() + nOffset, false);
		nNewScrollPos = pVerticalScrollbar->GetScrollPos();
		pVerticalScrollbar->SetScrollPos(nLastScrollPos,  false);
		if (bRedraw)
			pVerticalScrollbar->SetScrollPos(nLastScrollPos + nOffset);

		nBegin = nLastScrollPos;
		nEnd = nNewScrollPos;
		m_bLockChildsRect = true;
	}
	return nNewScrollPos - nLastScrollPos;
}

CControlUI* CListUI::GetNextItem( CControlUI* pControl, UINT uFlags /*= UIITEMFLAG_VISIBLE | UIITEMFLAG_ENABLED*/ )
{
	if (!pControl)
	{
		if (GetCount())
			return GetItem(0);
		else
			return NULL;
	}

	//CControlUI* pNextItem = NULL;
	int nSum = GetCount();
	bool bVisible = !!(uFlags & UIITEMFLAG_VISIBLE);
	bool bEnable = !!(uFlags & UIITEMFLAG_ENABLED);
	for (int i = pControl->GetIndex() + 1; i < nSum; i++)
	{
		CControlUI* pNextItem = GetItem(i);
		if (pNextItem)
		{
			if (bVisible && bEnable && pNextItem->IsVisible() && pNextItem->IsEnabled())
			{
				return pNextItem;
			}
			if (bVisible && !bEnable && pNextItem->IsVisible())
			{
				return pNextItem;
			}
			if (!bVisible && bEnable && pNextItem->IsEnabled())
			{
				return pNextItem;
			}
		}
	}
	return NULL;
}

CControlUI* CListUI::GetPrevItem( CControlUI* pControl, UINT uFlags /*= UIITEMFLAG_VISIBLE | UIITEMFLAG_ENABLED*/ )
{
	if (!pControl)
	{
		if (GetCount())
			return GetItem(GetCount() - 1);
		else
			return NULL;
	}

	//CControlUI* pNextItem = NULL;
	//int nSum = GetCount();
	bool bVisible = !!(uFlags & UIITEMFLAG_VISIBLE);
	bool bEnable = !!(uFlags & UIITEMFLAG_ENABLED);
	for (int i = pControl->GetIndex() - 1; i >= 0; i--)
	{
		CControlUI* pNextItem = GetItem(i);
		if (pNextItem)
		{
			if (bVisible && bEnable && pNextItem->IsVisible() && pNextItem->IsEnabled())
			{
				return pNextItem;
			}
			if (bVisible && !bEnable && pNextItem->IsVisible())
			{
				return pNextItem;
			}
			if (!bVisible && bEnable && pNextItem->IsEnabled())
			{
				return pNextItem;
			}
		}
	}
	return NULL;
}


ImageObj* CListUI::GetStateImage()
{
	StyleObj* pStyleObj = GetStyle(_Style_List_Item);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T(" Style List Item"));
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle)
	{
		pImageStyle->LoadImage();
		return pImageStyle->GetImageObj();
	}
	return NULL;
}

ImageObj* CListUI::GetItemBkgImage()
{
	if (!IsEnableItemBkg())
		return NULL;

	StyleObj* pStyleObj = GetStyle(_Style_List_ItemBkg);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T(" Style List ItemBkg"));
	ImageStyle* pImageStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pImageStyle)
	{
		pImageStyle->LoadImage();
		return pImageStyle->GetImageObj();
	}
	return NULL;
}

bool CListUI::ChildEvent(TEventUI& event)
{
	switch(event.nType)
	{
	case UIEVENT_KEYDOWN:
	case UIEVENT_SCROLLWHEEL:
		return Event(event);
	}
	return true;
}

bool CListUI::Event(TEventUI& event)
{
	switch(event.nType)
	{
	case UIEVENT_RBUTTONUP:
		{
			SendNotify(UINOTIFY_RCLICK, NULL, LPARAM(event.pSender != this?event.pSender:NULL));
		}
		break;
	case UIEVENT_KEYDOWN:
		{
			switch(event.wParam)
			{
			case 'A':
				{
					SHORT sCtrl = ::GetKeyState(VK_CONTROL);
					SHORT sShift = ::GetKeyState(VK_SHIFT);
					bool bMulti = IsEnableMultiSelect();
					if (bMulti && sCtrl < 0 && sShift >= 0)
					{
						SelectItemAll();
					}
				}
				break;
			case VK_RETURN:
				{
					if (GetCurSel())
					{
						GetCurSel()->Activate();
						this->SendNotify(UINOTIFY_DBCLICK, NULL, (LPARAM)GetCurSel());	
					}
				}
				break;
			case VK_HOME:
				{
					CScrollbarUI* pScrollbar = GetVScrollbar();
					if (pScrollbar && pScrollbar->IsVisible())
					{
						pScrollbar->SetScrollPos(0);
					}
				}
				break;
			case VK_END:
				{
					CScrollbarUI* pScrollbar = GetVScrollbar();
					if (pScrollbar && pScrollbar->IsVisible())
						pScrollbar->SetScrollPos(pScrollbar->GetScrollRange());
				}
				break;
			case VK_PRIOR:
				{
					CScrollbarUI* pScrollbar = GetVScrollbar();
					if (pScrollbar && pScrollbar->IsVisible())
					{
						pScrollbar->PageUp();
					}
				}
				break;
			case VK_NEXT:
				{
					CScrollbarUI* pScrollbar = GetVScrollbar();
					if (pScrollbar && pScrollbar->IsVisible())
					{
						pScrollbar->PageDown();
					}
				}
				break;
			case VK_LEFT:
			case VK_UP:
				{
					SHORT sCtrl = ::GetKeyState(VK_CONTROL);
					SHORT sShift = ::GetKeyState(VK_SHIFT);
					if (sCtrl < 0 || sShift <0)
						return false;
					CControlUI* pPrev = GetPrevItem(GetCurSel());
					if (pPrev)
					{
						SelectItem(pPrev);
						EnsureVisible(pPrev);
					}
				}
				break;
			case VK_RIGHT:
			case VK_DOWN:
				{
					SHORT sCtrl = ::GetKeyState(VK_CONTROL);
					SHORT sShift = ::GetKeyState(VK_SHIFT);
					if (sCtrl < 0 || sShift <0)
						return false;
					CControlUI* pNext = GetNextItem(GetCurSel());
					if (pNext)
					{
						SelectItem(pNext);
						EnsureVisible(pNext);
					}
				}
				break;
			}
			SendNotify(UINOTIFY_LB_KEYDOWN, event.wParam, event.lParam);
		}
		break;

	case UIEVENT_BUTTONUP:
		{
			SelectItem(NULL);
		}
		break;
	case UIEVENT_DRAG_DROP_GROUPS:
		{	
			if (IsEnableMultiSelect() && (GetFlags() & UICONTROLFLAG_ENABLE_AREAS))
			{
				if (event.wParam == UIEVENT_BEGIN_DRAG)
				{
					POINT beginPt = {(int)(short)LOWORD(event.lParam),(int)(short)HIWORD(event.lParam) };
					CControlUI* pItemFromPos = ItemFromPos(beginPt);
					if (!pItemFromPos || pItemFromPos != GetCurSel())
						return true;
				}
			}
		}
		break;
	case UIEVENT_AREA_SELECT:
		{
			if (!IsEnableMultiSelect())
				break;
			POINT beginPt = {(int)(short)LOWORD(event.wParam),(int)(short)HIWORD(event.wParam) };
			POINT endPt = {(int)(short)LOWORD(event.lParam),(int)(short)HIWORD(event.lParam) };
			CControlUI* pItemFromPos = ItemFromPos(beginPt);
			if (pItemFromPos && pItemFromPos == GetCurSel())
				break;
			SelectItem(NULL);
			m_bAreaSelecting = true;
			int nBeginY = min(beginPt.y, endPt.y);
			int nEndY   = max(beginPt.y, endPt.y);
			m_rcAreaSelection.left = min(beginPt.x, endPt.x);
			m_rcAreaSelection.top  = nBeginY;
			m_rcAreaSelection.right= max(beginPt.x, endPt.x);
			m_rcAreaSelection.bottom=nEndY;
			int nCnt = GetCount();
			RECT rcChild;
			RECT rcTemp;

			CListItemUI* pSub = NULL;
			for (int i = 0; i < nCnt; i++)
			{
				pSub = static_cast<CListItemUI*>(GetItem(i));
				if (pSub)
				{
					rcChild = pSub->GetClientRect();
					if (i >= m_nFirstVisible && i < m_nLastVisible && pSub->IsVisible() && IntersectRect(&rcTemp, &rcChild, &m_rcAreaSelection))
					{
						pSub->Select(true);
					}
					else
					{
						pSub->Select(false);
					}
				}
			}
			Invalidate();
			SendNotify(UINOTIFY_LB_AREASELECT);
		}
		return true;
	case UIEVENT_AREA_SELECT_END:
		{
			if (!IsEnableMultiSelect())
				break;
			m_bAreaSelecting = false;
			SetRectEmpty(&m_rcAreaSelection);
			Invalidate();
			SendNotify(UINOTIFY_LB_AREASELECT_ENDED);
		}
		return true;

	case UIEVENT_ITEM_MOUSEHOVER_CHANGE:
		{
			CControlUI* pControl = NULL;
			if (event.wParam)
			{
				pControl = (CControlUI*)event.wParam;
				pControl->SetState(UISTATE_NORMAL);
				pControl->Invalidate();
			}
			if (event.lParam)
			{
				pControl = (CControlUI*)event.lParam;
				pControl->SetState(UISTATE_OVER);
				pControl->Invalidate();
			}
			m_pHighlight = (CControlUI*)event.lParam;
		}
		return true;
	}

	
	return __super::Event(event);
}

bool CListUI::IsEnableItemBkg()
{
	return m_bEnableItemBkg;
}

void CListUI::RenderItemBkg(ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint)
{
	//HDC hDestDC = pRenderDC->GetDC();
	if (pImgState && pControl)
	{
		RECT rcChild = pControl->GetRect();
		pRenderDC->DrawImage(pImgState, rcChild, pControl->GetIndex()%2);
	}
}

CControlUI* CListUI::GetHotItem()
{
	return m_pHighlight;
}

bool CListUI::IsEnableMultiSelect()
{
	return m_bMultiSelect;
}

bool CListUI::IsEnableItemSelect()
{
	return m_bEnableItemSelect;
}

bool CListUI::IsEnableItemOver()
{
	return m_bEnableItemOver;
}


void CListUI::RenderItemState( ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint )
{
	if (IsEnableItemSelect() && pControl)
	{
		CListItemUI* pItem = static_cast<CListItemUI*>(pControl);

		//三个属性控制item状态图片
		//EnableSelect
		//EnableItemOver
		//item 的EnableItemState
		if (pItem->EnableItemState() && (GetHotItem() == pControl || pItem->IsSelected()))
		{
			RECT rc = pItem->GetRect();
			if (pItem->IsSelected())
			{
				pRenderDC->DrawImage(pImgState, rc, UISTATE_OVER);
			}
			else if (IsEnableItemOver())
			{
				pRenderDC->DrawImage(pImgState, rc, UISTATE_NORMAL);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////

CListUI::CListUI()
:m_bLockChildsRect(true)
{
	EnableScrollBar(true, false);
	m_nFirstVisible = 0;
	m_nLastVisible = 0;
	m_bEnableItemBkg = false;
	m_bEnableItemSelect = true;
	m_bEnableItemOver = true;
	m_bEnableItemOverOnly = false;
	m_bMultiSelect = false;
	m_bAreaSelecting = false;
	m_pBoundImage = NULL;
	SetRectEmpty(&m_rcAreaSelection);

	SetMouseEnabled(true);
	SetGestureEnabled(true);

	ModifyFlags(UICONTROLFLAG_SETFOCUS, 0);

	m_nHeaderHeight = 20;
	m_bHeaderVisible = true;
	m_bEnableHeaderItemSort = false;
	m_bEnableCheck = false;
	m_bVisibleHeaderCheck = false;

	m_nCheckWidth = 0;
	m_pCheck = new CListItemCheckUI;
	m_pHeader = new CListHeaderUI;
	m_pHeader->SetAttribute(_T("id"), _T("ListHeader"));
	m_pHeader->SetAttribute(_T("scroll"), _T("0"));
	m_pHeader->SetCoordinate(0,0,0,m_nHeaderHeight,UIANCHOR_LTRT);

	m_nCurSortCol = -1;
	m_pListSortCompareProc = NULL;
	m_rcOldRectInset.left = -1;
	m_rcOldRectInset.right = -1;
	m_rcOldRectInset.top = -1;
	m_rcOldRectInset.bottom = -1;
	m_pHighlight = NULL;

	RECT rcInset = {1, 1, 1, 1};
	SetInset(rcInset);
	SetStyle(_T("List"));

	EnableScrollBar(true, true);

}

CListUI::~CListUI()
{
	ItemHighlightMessageFilter::RemoveFromMessageFilterList(this);

	if (m_pHeader)
		delete m_pHeader;
	m_pHeader = NULL;
	if (m_pCheck)
		delete m_pCheck;
	m_pCheck = NULL;
	m_pListSortCompareProc = NULL;
}


bool CListUI::Remove(CControlUI* pControl)
{
	bool bRet = __super::Remove(pControl);
	CheckLegal();
	return bRet;
}

void CListUI::RemoveAll()
{
	__super::RemoveAll();
	if (EnableHeaderItemSort())
		m_nCurSortCol = -1;
	CheckLegal();
}

bool CListUI::Add(CControlUI* pControl)
{
	if (pControl)
	{
		if (pControl->IsClass(_T("ListHeader")))
		{
			if (m_pHeader)
				delete m_pHeader;
			m_pHeader = NULL;
			m_pHeader = static_cast<CListHeaderUI*>(pControl);
			m_pHeader->SetAttribute(_T("id"), _T("ListHeader"));
			m_pHeader->SetAttribute(_T("scroll"), _T("0"));
			return true;
		}
		else if (pControl->IsClass(_T("ListHeaderItem")))
		{
			if (m_pHeader)
			{
				pControl->SetOwner(this);
				return m_pHeader->Add(pControl);
			}
		}
		else
		{
			RT_CLASS_ASSERT(pControl, CListItemUI, _T("CListUI::Add"));
			CListItemUI* pItem = static_cast<CListItemUI*>(pControl);
			if (pItem)
			{
				if (m_bEnableCheck)
				{
					CListItemCheckUI* pCheck = new CListItemCheckUI;
					pCheck->SetOwner(pItem);
					pCheck->SetManager(GetWindow(), this);
					pItem->SetCheck(pCheck);
				}
			}
			return __super::Add(pControl);
		}
	}
	return false;
}

bool CListUI::Insert(CControlUI* pControl, CControlUI* pAfter)
{
	if (pControl->IsClass(_T("ListHeaderItem")))
	{
		if (m_pHeader)
		{
			pControl->SetOwner(this);
			return m_pHeader->Insert(pControl, pAfter);
		}
	}
	else
	{
		RT_CLASS_ASSERT(pControl, CListItemUI, _T("CListUI::Insert failed"));
		CListItemUI* pItem = static_cast<CListItemUI*>(pControl);
		if (pItem)
		{
			if (m_bEnableCheck)
			{
				CListItemCheckUI* pCheck = new CListItemCheckUI;
				pCheck->SetOwner(pItem);
				pCheck->SetManager(GetWindow(), this);
				pItem->SetCheck(pCheck);
			}
		}
		return __super::Insert(pControl, pAfter);
	}
	return false;
}

void CListUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("headerVisible")))
	{
		m_bHeaderVisible = !!_ttoi(lpszValue);
	}
	if (equal_icmp(lpszName, _T("headerHeight")))
	{
		m_nHeaderHeight = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("HeaderBkg")))
	{
		if (m_pHeader != NULL)
		{
			m_pHeader->SetAttribute(_T("bk.image"), lpszValue);
		}
	}
	else if (equal_icmp(lpszName, _T("enableHeaderSort")))
	{
		m_bEnableHeaderItemSort = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("enableCheck")))
	{
		m_bEnableCheck = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("visibleHeaderCheck")))
	{
		m_bVisibleHeaderCheck = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("HeaderCheckBkg")))
	{
		if (m_pCheck != NULL)
		{
			m_pCheck->SetAttribute(_T("itemmask.image"), lpszValue);
		}
	}
	else if (equal_icmp(lpszName, _T("EnableSelect")))
	{
		m_bEnableItemSelect = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("EnableItemBkg")))
	{
		m_bEnableItemBkg = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("EnableItemOver")))
	{
		m_bEnableItemOver = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("EnableItemOverOnly")))
	{
		m_bEnableItemOverOnly = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("MultiSelect")))
	{
		m_bMultiSelect = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("AreaSelect")))
	{
		ModifyFlags(UICONTROLFLAG_ENABLE_AREAS, 0);
		m_bMultiSelect = true;
	}
	else
		__super::SetAttribute(lpszName, lpszValue);
}

void CListUI::SetRect(RECT& rectRegion)
{
	if (m_rcOldRectInset.left == -1)
		m_rcOldRectInset = GetInset();
	//有多列且指定表头可见
	//表头的check可见
	GetWindow()->LockUpdate();
	if ((GetCols() > 0 && m_bHeaderVisible) || (m_bVisibleHeaderCheck))
	{
		if (m_pHeader)
		{
			m_pHeader->SetCoordinate(0,0,0,m_nHeaderHeight,UIANCHOR_LTRT);
			m_pHeader->SetVisible(true);
		}
		if (GetVScrollbar())
		{
			int l,t,r,b;
			GetVScrollbar()->GetCoordinate(l, t, r, b);
			GetVScrollbar()->SetCoordinate(l, m_nHeaderHeight+1, r, b);
		}
		GetInset().top = 1 + m_rcOldRectInset.top + m_nHeaderHeight;
	}
	else
	{
		//没有列的时候需要置空
		//有列但表头不可见时需要列的位置信息
		if (m_pHeader)
		{
			m_pHeader->SetCoordinate(0,0,0,m_nHeaderHeight,UIANCHOR_LTRT);
			m_pHeader->SetVisible(false);
		}
		if (GetVScrollbar())
		{
			int l,t,r,b;
			GetVScrollbar()->GetCoordinate(l, t, r, b);
			GetVScrollbar()->SetCoordinate(l, 1, r, b);
		}
		GetInset().top = 1 + m_rcOldRectInset.top;
	}
	GetWindow()->UnLockUpdate();
	__super::SetRect(rectRegion);
}

int CListUI::GetCols()
{
	if (m_pHeader)
		return m_pHeader->GetCount();
	return -1;
}

CListHeaderUI* CListUI::GetHeader()
{
	return m_pHeader;
}

CListHeaderItemUI* CListUI::GetCol(int nIndex)
{
	if (m_pHeader)
	{
		CControlUI* pCol = m_pHeader->GetItem(nIndex);
		return static_cast<CListHeaderItemUI*>(pCol);
	}
	return NULL;
}

bool CListUI::InsertCol(CListHeaderItemUI* pItem, int nIndex/* = -1*/)
{
	if (m_pHeader)
	{
		if (nIndex==-1 || this->GetCols() < nIndex)
			m_pHeader->Add(pItem);
		else
		{
			CControlUI* pControl = m_pHeader->GetItem(nIndex);
			if (pControl)
				m_pHeader->Insert(pItem, pControl);
			else
				m_pHeader->Add(pItem);
		}
		return true;
	}
	return false;
}

void CListUI::Init()
{
	if (m_pHeader)
	{
		m_pHeader->SetOwner(this);
		m_pHeader->SetManager(GetWindow(), this);
		m_pHeader->Init();
	}
	if (m_bEnableCheck)
	{
		m_nCheckWidth = 25;

		if (m_pCheck == NULL)
			m_pCheck = new CListItemCheckUI;
		m_pCheck->SetId(_T("_listheader_checkitem"));
		m_pCheck->SetOwner(this);
		m_pCheck->SetManager(GetWindow(), this);
		m_pCheck->Init();
		m_pCheck->SetINotifyUI(this);
		m_pCheck->SetAttribute(_T("itemmask.visible"), _T("1"));
		m_pCheck->SetAttribute(_T("scroll"), _T("0"));
	}
	else
	{
		if (m_pCheck)
			delete m_pCheck;
		m_pCheck = NULL;
	}
	
	ItemHighlightMessageFilter::AddToMessageFilterList(this);
	__super::Init();
}

bool CListUI::Resize()
{
	if (__super::Resize())
	{
		if (m_pHeader)
		{
			CScrollbarUI* pHScroll = GetHScrollbar();
			if (pHScroll && pHScroll->IsVisible())
			{
				int nPos = pHScroll->GetScrollPos();
				int nCnt = this->GetCols();
				//CControlUI* pControl = NULL;
				RECT rcCol = m_pHeader->GetClientRect();
				rcCol.left -= nPos;
				rcCol.right = rcCol.left;
				int l,t,r,b;
				for (int i = 0; i < nCnt; i++)
				{
					CControlUI* pControl = m_pHeader->GetItem(i);
					if (pControl && pControl->IsVisible())
					{
						pControl->GetCoordinate(l, t, r, b);
						rcCol.left = rcCol.right;
						rcCol.right = rcCol.left + DPI_SCALE(r);
						pControl->SetRect(rcCol);
					}
				}
				//m_pHeader->Invalidate();
			}
		}
		return true;
	}
	return false;
}

void CListUI::SetItemText(int nIndex, int nCol, LPCTSTR lpszText)
{
	CControlUI* pControl = GetItem(nIndex);
	if (pControl)
		static_cast<CListItemUI*>(pControl)->SetSubItemText(nCol, lpszText);
}

LPCTSTR CListUI::GetItemText(int nIndex, int nCol)
{
	CControlUI* pControl = GetItem(nIndex);
	if (pControl->IsClass(_T("ListItem")))
		return static_cast<CListItemUI*>(pControl)->GetSubItemText(nCol);
	return NULL;
}

bool CListUI::EnableHeaderItemSort()
{
	return m_bEnableHeaderItemSort;
}

bool CListUI::SetHeaderText( int nCol, LPCTSTR lpszText )
{
	if (m_pHeader)
	{
		if (nCol < m_pHeader->GetCount() && nCol >=0 )
		{
			m_pHeader->GetItem(nCol)->SetText(lpszText);
			return true;
		}
	}
	return false;
}

void CListUI::SetSortCallBack(LISTSORTPROC pCompare)
{
	m_pListSortCompareProc = pCompare;
}

LISTSORTPROC gs_pListSortCompareProc = NULL;
bool gs_bListSortAsc = false;
int gs_nListSortCurSortCol = 0;
bool List_Col_CompareFunc(LPVOID ptrA, LPVOID ptrB)
{
	if (gs_pListSortCompareProc)
	{
		int nRes = gs_pListSortCompareProc(static_cast<CListItemUI*>(ptrA),static_cast<CListItemUI*>(ptrB),gs_nListSortCurSortCol,gs_bListSortAsc);
		nRes = gs_bListSortAsc ? nRes : -nRes;
		if (nRes < 0)
		{
			return true;
		}
	}
	return false;
}

bool CListUI::Sort(int nColumn /*= -1*/, int nAsc /*= -1*/, int nBegin /*= 0*/, int nSize /*= -1*/)
{
	if (m_pListSortCompareProc)
	{
		if (nColumn >= 0)
		{
			m_nCurSortCol = nColumn;
		}
		bool bAsc = true;
		if (nAsc != -1)
		{
			if (nAsc == 0)
			{
				bAsc = false;
			}
			else if (nAsc == 1)
			{
				bAsc = true;
			}
		}
		if (EnableHeaderItemSort())
		{
			if (m_pHeader)
			{
				CListHeaderItemUI* pHeadItem = GetCol(m_nCurSortCol);
				if (pHeadItem)
				{
					if (nAsc == -1)
						bAsc = pHeadItem->IsAscending();
					else
						pHeadItem->m_bAscending = bAsc;
				}
			}
		}
		if (nSize < 0)
			nSize = GetCount();
		if (nSize > GetCount())
			nSize = GetCount();
		if (nSize > GetCount() - nBegin)
			nSize = GetCount() - nBegin;

		gs_pListSortCompareProc = m_pListSortCompareProc;
		gs_bListSortAsc = bAsc;
		gs_nListSortCurSortCol = m_nCurSortCol;
		CStdPtrArray* pArrItems = GetItemArray();
		if (pArrItems)
			pArrItems->Sort(List_Col_CompareFunc, nBegin, nSize);
		gs_pListSortCompareProc = NULL;
		gs_bListSortAsc = false;
		gs_nListSortCurSortCol = -1;

		//CControlUI* pControl = NULL;
		for (int i = nBegin; i < nBegin + nSize; i++)
		{
			CControlUI* pControl = GetItem(i);
			if (pControl)
				pControl->SetIndex(i);
		}
		this->Resize();
		this->Invalidate();
	}
	return true;
}

int CListUI::GetSortCol()
{
	return m_nCurSortCol;
}

void CListUI::SetSortCol(int nCol) 
{
	m_nCurSortCol = nCol;
}

void CListUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	//画子控件
	{
		int nCnt = GetCount();
		if (m_nFirstVisible < 0 || m_nFirstVisible >= nCnt)
			m_nFirstVisible = 0;
		if (m_nLastVisible < 0 || m_nLastVisible >= nCnt)
			m_nLastVisible = nCnt;
		if (m_nFirstVisible > m_nLastVisible)
			m_nFirstVisible = m_nLastVisible;

		RECT rcTemp = { 0 };
		RECT rcItem = GetRect();
		RECT rcClient = GetClientRect();
		if (GetParent())
		{
			RECT rcParent = GetParent()->GetRect();
			::IntersectRect(&rcItem, &rcParent, &rcItem);
			::IntersectRect(&rcClient, &rcItem, &rcClient);
		}

		if( !::IntersectRect(&rcTemp, &rcPaint, &rcItem)) 
			return;
		HDC hDestDC = pRenderDC->GetDC();

		//画父控件
		ClipObj clip;
		ClipObj::GenerateClip(hDestDC, rcTemp, clip);
		CControlUI::Render(pRenderDC, rcPaint);

		ImageObj * pItemBkgImage = GetItemBkgImage();
		ImageObj * pItemStateImage = GetStateImage();

		RECT rcChild;
		if( ::IntersectRect(&rcTemp, &rcPaint, &rcClient)) 
		{
			CControlUI* pControl = NULL;
			ClipObj childClip;
		/*	if (DrawLimit())*/
				ClipObj::GenerateClip(hDestDC, rcTemp, childClip);
			/*else
				ClipObj::GenerateClip(hDestDC, rcClient, childClip);*/
			for( int i = m_nFirstVisible;  i < m_nLastVisible; i++ )
			{
				pControl = GetItem(i);
				if (!pControl)
					continue;
				if (!pControl->IsVisible())
					continue;

				rcChild = pControl->GetRect();

				if( !::IntersectRect(&rcTemp, &rcClient, &rcChild)) 
					continue;

				if( !::IntersectRect(&rcTemp, &rcPaint, &rcTemp)) 
					continue;

				if (pItemBkgImage)
				{
					/*if (DrawLimit())*/
						RenderItemBkg(pItemBkgImage, pControl, pRenderDC, rcTemp);
					/*else
						RenderItemBkg(pItemBkgImage, pControl, pRenderDC, rcPaint);*/
				}
				if (pItemStateImage)
					RenderItemState(pItemStateImage, pControl, pRenderDC, rcPaint);
			}
			UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, *GetItemArray(), m_nFirstVisible, m_nLastVisible);
		}

		UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, GetHScrollbar());
		UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, GetVScrollbar());
	}

	if (m_pHeader && GetCols())
	{
		m_pHeader->Render(pRenderDC, rcPaint);
	}

	if (m_bEnableCheck)
	{
		if (m_pCheck)
			m_pCheck->Render(pRenderDC, rcPaint);

		RECT rcRender = GetClientRect();
		RECT rcItem = GetRect();
		rcRender.left = rcItem.left;
		rcRender.right = rcItem.right;
		ClipObj childClip;
		ClipObj::GenerateClip(pRenderDC->GetDC(), rcRender, childClip);

		//CControlUI* pControl = NULL;
		CControlUI* pCheck = NULL;
		//int nCnt = GetCount();
		RECT rcClient = GetClientRect();
		RECT rcCheckItem;
		RECT rcTemp = {0};
		for (int i = m_nFirstVisible; i < m_nLastVisible; i++)
		{
			CControlUI* pControl = GetItem(i);
			if (pControl == NULL)
				continue;
			if (!pControl->IsVisible())
				continue;
			pCheck = static_cast<CListItemUI*>(pControl)->GetCheck();
			if (pCheck)
			{
				if (!pCheck->IsVisible())
					continue;
				rcCheckItem = pCheck->GetRect();

				if( !::IntersectRect(&rcTemp, &rcPaint, &rcCheckItem)) 
					continue;

				pCheck->Render(pRenderDC, rcPaint);
			}
		}
	}
}
CControlUI* CListUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if (!FindControlTest(this, pData, uFlags))
		return NULL;

	CControlUI* pControl = NULL;

	if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
		pControl = __super::FindControl(Proc, pData, uFlags);
		if(pControl)
			return pControl;
	}

	if (m_pHeader && m_pHeader->IsVisible())
		pControl = m_pHeader->FindControl(Proc, pData, uFlags);
	if (pControl != NULL)
		return pControl;

	if (m_bEnableCheck && m_pCheck && m_pCheck->IsVisible())
		pControl = m_pCheck->FindControl(Proc, pData, uFlags);
	if (pControl != NULL)
		return pControl;

	if (m_bEnableCheck)
	{
		//CControlUI* pItem = NULL;
		CControlUI* pCheck = NULL;
		//int nCnt = GetCount();
		RECT rcClient = GetClientRect();
		for (int i = m_nFirstVisible; i < m_nLastVisible; i++)
		{
			CControlUI* pItem = GetItem(i);
			if (!pItem || !pItem->IsVisible())
				continue;
			pCheck = static_cast<CListItemUI*>(pItem)->GetCheck();
			if (pCheck)
			{
				pControl = pCheck->FindControl(Proc, pData, uFlags);
				if (pControl)
					return pControl;
			}
		}
	}

	if( pControl == NULL && GetVScrollbar() != NULL) 
		pControl = GetVScrollbar()->FindControl(Proc, pData, uFlags);

	if( pControl == NULL && GetHScrollbar() != NULL ) 
		pControl = GetHScrollbar()->FindControl(Proc, pData, uFlags);

	if (pControl != NULL)
		return pControl;

	pControl = UICustomControlHelper::FindChildControl(*GetItemArray(), Proc, pData, uFlags, m_nFirstVisible, m_nLastVisible);
	if (pControl != NULL)
		return pControl;

	return __super::FindControl(Proc, pData, uFlags);

	return pControl;
}

void CListUI::OnLanguageChange()
{
	__super::OnLanguageChange();
	if (m_pHeader) ((CControlUI*)m_pHeader)->OnLanguageChange();
}

SIZE CListUI::OnChildSize(RECT& rcWinow, RECT& rcClient)
{
	if (m_pHeader)
	{
		//调整表头位置
		CScrollbarUI* pScrollbar = GetVScrollbar();
		RECT rcInset = {m_nCheckWidth, 0, 
			(pScrollbar && pScrollbar->IsVisible()) ? pScrollbar->GetFixedSize().cx + GetInset().right : GetInset().right, 0 };
		if (GetUIEngine()->IsRTLLayout())
		{
			int temp = rcInset.left;
			rcInset.left = rcInset.right;
			rcInset.right = temp;
		}
		m_pHeader->SetInset(rcInset);
		m_pHeader->OnSize(GetRect());

		//子控件位置
		//SIZE sz = __super::OnChildSize(rcWinow, rcChildClient);
		SIZE sz;
		{
			CScrollbarUI* pScrollbar = GetVScrollbar();
			int nPos = 0;
			if (pScrollbar && pScrollbar->IsVisible())
				nPos = pScrollbar->GetScrollPos();
			nPos = nPos + rcClient.top;

			//初始化客户区区域（存在横向滚动条情况下）
			RECT rectBegin = rcClient;
			rectBegin.bottom = rectBegin.top;
			{
				SIZE szHeader = m_pHeader->GetChildSize();
				rectBegin.right = rectBegin.left + max(rcClient.right - rcClient.left, szHeader.cx);
			}

			int l, t, r, b;
			int nMaxWidth = rcClient.right - rcClient.left;
			int nCnt = GetCount();
			RECT rcEmpty = {0};
			bool bFirstFlag = true;
			CControlUI* pControl = NULL;
			//获取大小
			m_nFirstVisible = 0;
			RECT rcChild = rectBegin;
			RECT rcCalc;
			for (int i = 0; i < nCnt; i++)
			{
				pControl = GetItem(i);
				if (pControl)
				{
					if (pControl->IsVisible())
					{
						pControl->GetCoordinate(l, t, r, b);
						rcCalc = rectBegin;
						rcCalc.top = rectBegin.bottom;
						rcCalc.bottom = rectBegin.bottom + DPI_SCALE(b - t);
						if (!m_bLockChildsRect)
							*(pControl->GetRectPtr()) = rcCalc;//pControl->SetRect(rcCalc, false);// xqb去掉SetRect重载
						rectBegin.bottom = rectBegin.bottom + DPI_SCALE(b - t);
						if (bFirstFlag && rectBegin.bottom > nPos)
						{
							rcChild.top = rectBegin.bottom - DPI_SCALE(b - t);//记录第一个控件的位置
							m_nFirstVisible = i;
							bFirstFlag = false;
						}
					}
					else
					{
						if (!m_bLockChildsRect)
							ZeroMemory(pControl->GetRectPtr(), sizeof(RECT));//pControl->SetRect(rcEmpty, false);// xqb去掉SetRect重载
					}
				}
			}
			//计算可视界面的控件
			m_nLastVisible = nCnt;
			rcChild.bottom = rcChild.top;
			for (int i = m_nFirstVisible; i < nCnt; i++)
			{
				pControl = GetItem(i);
				if (pControl && pControl->IsVisible())
				{
					pControl->GetCoordinate(l, t, r, b);

					rcChild.top = rcChild.bottom;
					rcChild.bottom = rcChild.top + DPI_SCALE(b - t);
					if (r - l != 0)
					{
						rcChild.left = rectBegin.left + (nMaxWidth - DPI_SCALE(r - l)) / 2;
						rcChild.right = rcChild.left + DPI_SCALE(r - l);
					}
					else
					{
						rcChild.left = rectBegin.left;
						rcChild.right = rectBegin.right;
					}
					pControl->SetRect(rcChild);

					if (rcChild.top > rcWinow.bottom)
					{
						m_nLastVisible = i;
						break;
					}
				}
			}
			sz.cx = rectBegin.right - rectBegin.left;
			sz.cy = rectBegin.bottom - rectBegin.top;
		}

		//表头check位置
		if (m_pCheck)
		{
			GetWindow()->LockUpdate();
			if (m_bEnableCheck && m_pHeader && m_pHeader->IsVisible())
			{
				//////////////////////////////////////////////////////////////////////////
				// 全选check的位置
				RECT rcHeader = m_pHeader->GetRect();
				RECT rcItem = GetRect();
				m_pCheck->SetCoordinate(1, DPI_SCALE_BACK(rcHeader.top - rcItem.top), m_nCheckWidth, DPI_SCALE_BACK(rcHeader.bottom - rcHeader.top), UIANCHOR_LEFTTOP);
				m_pCheck->OnSize(GetRect());
				m_pCheck->SetVisible(true);
			}
			else
			{
				m_pCheck->SetVisible(false);
			}
			GetWindow()->UnLockUpdate();
		}
		//控件中check位置
		if (m_bEnableCheck)
		{
			RECT rcList = GetRect();

			RECT rcCheck = {0};
			if (GetUIEngine()->IsRTLLayout())
			{
				RECT rc = { rcList.right - DPI_SCALE(m_nCheckWidth), rcClient.top, rcList.right - 1, rcClient.bottom };
				rcCheck = rc;

			}
			else
			{
				RECT rc = { rcList.left + 1, rcClient.top, rcList.left + DPI_SCALE(m_nCheckWidth), rcClient.bottom };
				rcCheck = rc;
			}

			RECT rcEmpty = {0};
			RECT rcChild;
			RECT rcCheckItem;
			CControlUI* pCheck = NULL;
			CListItemUI* pControl = NULL;
			int nCnt = GetCount();
			//置空
			for (int i = 0; i < nCnt; i++)
			{
				pControl = static_cast<CListItemUI*>(GetItem(i));
				if (pControl)
				{
					pCheck = pControl->GetCheck();
					if (pCheck)
					{
						ZeroMemory(pCheck->GetRectPtr(), sizeof(RECT));//pCheck->SetRect(rcEmpty, false);// xqb去掉SetRect重载
					}
				}
			}
			//可见区域
			for (int i = m_nFirstVisible; i < m_nLastVisible; i++)
			{
				pControl = static_cast<CListItemUI*>(GetItem(i));
				if (pControl)
				{
					rcChild = pControl->GetRect();
					pCheck = pControl->GetCheck();
					if (pCheck)
					{
						rcCheckItem = rcChild;
						rcCheckItem.left = rcCheck.left;
						rcCheckItem.right = rcCheck.right;
						pCheck->EnableRTLLayout(false);
						pCheck->SetRect(rcCheckItem/*, true*/);// xqb去掉SetRect重载，这边阿拉伯版本会不会有问题？
					}
				}
			}
		}
		return sz;
	}
	else
	{
		return __super::OnChildSize(rcWinow, rcClient);
	}
}

void CListUI::CheckItem( bool bCheck, int nIndex /*= -1*/, UINT uFlag /*= UIITEMFLAG_CHECKED | UIITEMFLAG_NOTIFY*/ )
{
	if (m_bEnableCheck)
	{
		bool bFlagVisible = false;
		bool bFlagEnabled = false;
		bool bFlagCheck = false;
		if (uFlag & UIITEMFLAG_VISIBLE)
		{
			bFlagVisible = true;
		}
		else if (uFlag & UIITEMFLAG_ENABLED)
		{
			bFlagEnabled = true;
		}
		else if (uFlag & UIITEMFLAG_CHECKED)
		{
			bFlagCheck = true;
		}
		//没一个为真
		if (!(bFlagVisible || bFlagEnabled || bFlagCheck))
			return;

		CCheckBoxUI* pCheck = NULL;
		CControlUI* pControl = NULL;
		if (nIndex == -1)
		{
			//全部check
			int nCnt = GetCount();
			for (int i = 0; i < nCnt; i++)
			{
				pControl = GetItem(i);
				if (pControl)
				{
					pCheck = static_cast<CListItemUI*>(pControl)->GetCheck();
					if (pCheck)
					{
						if (bFlagCheck)
						{
							if (pCheck->IsVisible() && pCheck->IsEnabled())
								pCheck->SetChecked(bCheck, UIITEMFLAG_CHECKED);
						}
						else if (bFlagVisible)
						{
							pCheck->SetVisible(bCheck);
						}
						else if (bFlagEnabled)
						{
							pCheck->SetEnabled(bCheck);
						}
						if ((uFlag&UIITEMFLAG_NOTIFY) && bFlagCheck)
							SendNotify( UINOTIFY_TVN_CHECK, (WPARAM)bCheck, (LPARAM)pControl);	
					}
				}
			}
			//表头check的通知
			if (bFlagCheck)
				SendNotify(UINOTIFY_TVN_CHECK, (WPARAM)bCheck, (LPARAM)NULL);
		}
		else
		{
			pControl = GetItem(nIndex);
			if (pControl)
			{
				pCheck = static_cast<CListItemUI*>(pControl)->GetCheck();
				if (pCheck)
				{
					if (bFlagCheck)
					{
						if (pCheck->IsVisible() && pCheck->IsEnabled())
							pCheck->SetChecked(bCheck, UIITEMFLAG_CHECKED);
					}
					else if (bFlagVisible)
					{
						pCheck->SetVisible(bCheck);
					}
					else if (bFlagEnabled)
					{
						pCheck->SetEnabled(bCheck);
					}
					if ((uFlag&UIITEMFLAG_NOTIFY) && bFlagCheck)
						SendNotify( UINOTIFY_TVN_CHECK, (WPARAM)bCheck, (LPARAM)pControl);	
				}
			}
		}
		CheckLegal();
	}
}

bool CListUI::IsCheckItem(int nIndex, UINT uFlag /*= UILIST_CHECK*/)
{
	bool bFlagVisible = false;
	bool bFlagEnabled = false;
	bool bFlagCheck = false;
	switch(uFlag)
	{
	case UIITEMFLAG_VISIBLE:
		bFlagVisible = true;
		break;
	case UIITEMFLAG_ENABLED:
		bFlagEnabled = true;
		break;
	case UIITEMFLAG_CHECKED:
		bFlagCheck = true;
		break;
	}
	if (m_bEnableCheck)
	{
		CControlUI* pControl = GetItem(nIndex);
		if (pControl)
		{
			CCheckBoxUI* pCheck = static_cast<CListItemUI*>(pControl)->GetCheck();
			if (pCheck)
			{
				if (bFlagCheck)
				{
					if (pCheck->IsVisible())
						return pCheck->IsChecked();
				}
				else if (bFlagVisible)
				{
					return pCheck->IsVisible();
				}
				else if (bFlagEnabled)
				{
					return pCheck->IsEnabled();
				}
			}
		}
	}
	return false;
}

bool CListUI::OnBtnClick(TNotifyUI* pNotify)
{
	if (m_pCheck)
		CheckItem(m_pCheck->IsChecked());
	return true;
}

void CListUI::CheckLegal()
{
	if (m_pCheck)
	{
		//bool bCheck = m_pCheck->IsChecked();
		int nCnt = GetCount();
		int nVisibleCnt = 0;
		int nCheckCnt = 0;

		//CControlUI* pControl = NULL;
		CCheckBoxUI* pCheck = NULL;
		for (int i = 0; i < nCnt; i++)
		{
			CControlUI* pControl = GetItem(i);
			if (pControl)
			{
				pCheck = static_cast<CListItemUI*>(pControl)->GetCheck();
				if (pCheck && pCheck->IsVisible())
				{
					nCheckCnt += pCheck->IsChecked();
					nVisibleCnt++;
				}
			}
		}
		if (nCheckCnt == 0)
		{
			m_pCheck->SetChecked(false, UIITEMFLAG_CHECKED);
			m_pCheck->Indef(false);
		}
		else
		{
			if (nCheckCnt == nVisibleCnt)
			{
				m_pCheck->SetChecked(true, UIITEMFLAG_CHECKED);
				m_pCheck->Indef(false);
			}
			else
			{
				m_pCheck->SetChecked(false, UIITEMFLAG_CHECKED);
				m_pCheck->Indef(true);
			}
		}
	}
}

CCheckBoxUI* CListUI::GetCheck()
{
	return m_pCheck;
}

bool CListUI::GetItemRect( CControlUI* pControl, RECT& rcChild )
{
	CControlUI* pParent = pControl->GetParent();
	if (pParent && pParent == this)
	{
		rcChild = pControl->GetRect();
		if (m_bEnableCheck)
			rcChild.left = rcChild.left + m_nCheckWidth;
		return true;
	}
	return false;
}

int CListUI::CheckWidth() const
{
	return m_nCheckWidth;
}
//////////////////////////////////////////////////////////////////////////
CTableUI::CTableUI()
:m_pShowPloyFunc(NULL),
m_pTableSortCompareProc(NULL)
{
	CTableItemUI* pRoot = new CTableItemUI;
	pRoot->Expand(true, 0);
	pRoot->node_group(true);
	pRoot->SetId(_T("root"));
	pRoot->SetIndex(-1);
	pRoot->set_node_owner(this);
	pRoot->SetManager(NULL, this);
	set_root(pRoot);
}

CTableUI::~CTableUI()
{
	m_pShowPloyFunc = NULL;
	m_pTableSortCompareProc = NULL;

	CTableItemUI* pRoot = GetRoot();
	if (pRoot)
		delete pRoot;
	set_root(NULL);
}

void CTableUI::expanding( INode* pNode, bool bExpand )
{
	SendNotify(UINOTIFY_TVN_ITEMEXPENDING, WPARAM(bExpand), LPARAM(dynamic_cast<CTableItemUI*>(pNode)));
}

void CTableUI::expandend( INode* pNode, bool bExpand )
{
	SendNotify(UINOTIFY_TVN_ITEMEXPENDED, WPARAM(bExpand), LPARAM(dynamic_cast<CTableItemUI*>(pNode)));
}

void CTableUI::NeedUpdate()
{
	Resize();
	Invalidate();
}

INode* CTableUI::get_node( int nIndex )
{
	return static_cast<CTableItemUI*>(GetItem(nIndex));
}

void CTableUI::set_node( int nIndex, INode* pNode )
{
	GetItemArray()->SetAt(nIndex, static_cast<CTableItemUI*>(pNode));
}

CTableItemUI* CTableUI::InsertItem( CTableItemUI* pControl, CTableItemUI* pParent /*= UITABLE_ROOT*/, CTableItemUI* pInsertAfter /*= UITABLE_LAST*/ )
{
	CTableItemUI* pRoot = GetRoot();
	if (pParent == UITABLE_ROOT || !pParent)
		pParent = pRoot;

	// 不是组节点不能插入子节点
	if (!pParent->IsGroup())
	{
		if (!pControl->get_parent())
			delete pControl;
		return NULL;
	}
	int nChildIndex = 0;//pParent下的节点逻辑位置
	if (pInsertAfter == UITABLE_FIRST)
	{
		if (pParent == pRoot)
			pInsertAfter = NULL;
		else
			pInsertAfter = pParent;
		nChildIndex = 0;
	}
	else if (pInsertAfter == UITABLE_LAST ||  !pInsertAfter)
	{
		nChildIndex = pParent->num_child();
		if (pParent == pRoot && nChildIndex == 0)
		{
			pInsertAfter = NULL;
		}
		else
		{
			pInsertAfter = pParent->get_last_child();
		}
	}
	else
	{
		bool bFind = false;
		int nCnt = pParent->num_child();
		for (int i = 0; i < nCnt; i++)
		{
			if (pParent->get_child(i) == pInsertAfter)
			{
				pInsertAfter = pInsertAfter->get_last_child();
				nChildIndex = i + 1;
				bFind = true;
				break;
			}
		}
		if (!bFind)
		{
			nChildIndex = pParent->num_child();
			if (pParent == pRoot && nChildIndex == 0)
			{
				pInsertAfter = NULL;
			}
			else
			{
				pInsertAfter = pParent->get_last_child();
			}
		}
	}  

	if (!pParent->IsExpand())
		SetItemVisible(pControl, false);

	// 移动节点
	if (pControl->get_parent())
	{
		//判断是否已经在指定位置
		CControlUI* pPrev = GetItem(pControl->GetIndex() - 1);
		if (pPrev)
		{
			CTableItemUI* pPrevNode = static_cast<CTableItemUI*>(pPrev);
			if (pPrevNode == pInsertAfter)
			{
				return pControl;
			}
		}
		else
		{
			//前一个为空，且pInsertAfter为空
			if (pPrev == pInsertAfter)
			{
				return pControl;
			}
		}
		//需要移动
		int nStart = pControl->GetIndex();
		CTableItemUI* pLastChild = pControl->get_last_child();
		int nEnd = pLastChild->GetIndex();
		int nTo = 0;
		if (pInsertAfter)
			nTo = pInsertAfter->GetIndex() + 1;
		else
			nTo = 0;
		pControl->get_parent()->remove_child_node(pControl);
		CStdPtrArray* pArrItems = GetItemArray();
		if (pArrItems)
			pArrItems->Move(nStart, nEnd, nTo);
		nStart = min(nStart, nTo);
		nEnd   = max(nEnd, nTo);
		if (nStart < 0)
			nStart = 0;
		if (nEnd >= GetCount())
			nEnd = GetCount() - 1;
		//CControlUI* pSubMove = NULL;
		for (int i = nStart; i <= nEnd; i++)
		{
			CControlUI* pSubMove = GetItem(i);
			if (pSubMove)
			{
				pSubMove->SetIndex(i);
			}
		}

		pParent->add_child_node(pControl, nChildIndex);
		NeedUpdate();
	}
	else //插入节点
	{
		if( !__super::Insert(pControl, pInsertAfter) )
		{
			delete pControl;
			return NULL;
		}

		pParent->add_child_node(pControl, nChildIndex);
		//modify by hanzp. 效率考虑
		//Resize();  // 徐情波临时添加函数
	}
	return pControl;
}

bool CTableUI::Add( CControlUI* pControl )
{
	CTableItemUI* pItem = dynamic_cast<CTableItemUI*>(pControl);
	if (pItem)
	{
		return (InsertItem(pItem) != NULL);
	}
	else
	{
		CListItemUI* pListItem = static_cast<CListItemUI*>(pControl);
		if (pListItem)
			return __super::Add(pControl);
	}
	return __super::Add(pControl);;
}

void CTableUI::EnsureVisible( CControlUI* pControl )
{
	RT_CLASS_ASSERT(pControl, CListItemUI, _T("CTableUI::EnsureVisible"));
	CTableItemUI* pItem = dynamic_cast<CTableItemUI*>(pControl);
	if (pItem)
	{
		CTableItemUI* pParent = pItem->get_parent();
		if (pParent)
			pParent->Expand(true);
	}
	__super::EnsureVisible(pControl);
}

void CTableUI::EnsureVisibleFirst( CControlUI* pControl )
{
	RT_CLASS_ASSERT(pControl, CListItemUI, _T("CTableUI::EnsureVisible"));
	CTableItemUI* pItem = dynamic_cast<CTableItemUI*>(pControl);
	if (pItem)
	{
		CTableItemUI* pParent = pItem->get_parent();
		if (pParent)
			pParent->Expand(true);
	}
	__super::EnsureVisibleFirst(pControl);
}

int CTableUI::EnsureVisibleCenter( CControlUI* pControl,int& nBegin, int &nEnd, bool bRedraw /*= true */ )
{
	RT_CLASS_ASSERT(pControl, CListItemUI, _T("CTableUI::EnsureVisible"));
	CTableItemUI* pItem = dynamic_cast<CTableItemUI*>(pControl);
	if (pItem)
	{
		CTableItemUI* pParent = pItem->get_parent();
		if (pParent)
			pParent->Expand(true);
	}
	return __super::EnsureVisibleCenter(pControl, nBegin, nEnd, bRedraw);
}

//************************************
// @brief:    设置pParent下所有节点的容器索引
// @note:     
// @param: 	  CTableItemUI * pParent 
// @param: 	  int & nIndex 
// @param: 	  int nBegin 
// @param: 	  int nSize 
// @return:   void 
//************************************
void SetTableItemIndex(CTableItemUI* pParent, int& nIndex, CStdPtrArray& items, int nBegin = 0, int nSize = -1)
{
	int nCnt = pParent->num_child();
	if (nSize != -1)
	{
		nCnt = nBegin + nSize;
	}
	//CTableItemUI* pItem = NULL;
	for (int i = nBegin; i < nCnt; i++)
	{
		CTableItemUI* pItem = pParent->get_child(i);
		if (pItem)
		{
			pItem->set_node_index(nIndex);
			items.SetAt(nIndex, static_cast<CControlUI*>(pItem));
			++nIndex;
			if (pItem->IsGroup())
			{
				SetTableItemIndex(pItem, nIndex, items);
			}
		}
	}
}

TABLESORTPROC gs_pTableSortCompareProc = NULL;
bool gs_bTableSortAsc = false;
int gs_nTableSortCurSortCol = 0;
bool Table_Col_CompareFunc(LPVOID ptrA, LPVOID ptrB)
{
	if (gs_pTableSortCompareProc)
	{
		CTableItemUI* pItemA = static_cast<CTableItemUI*>(ptrA);
		CTableItemUI* pItemB = static_cast<CTableItemUI*>(ptrB);
		if (pItemA->get_parent() == pItemB->get_parent())
		{
			CTableItemUI* pParent = pItemA->get_parent();
			int nRes = gs_pTableSortCompareProc(pItemA, pItemB, pParent, gs_nTableSortCurSortCol, gs_bTableSortAsc);
			nRes = gs_bTableSortAsc ? nRes : -nRes;
			if (nRes < 0)
			{
				return true;
			}
		}
	}
	return false;
}

//************************************
// @brief:    对pParent的子节点进行排序
// @note:     
// @param: 	  TABLESORTPROC pCompareProc 
// @param: 	  CTableItemUI * pParent 
// @param: 	  int nColumn 
// @param: 	  bool bAsc 
// @return:   void 
//************************************
void SortChildNode(TABLESORTPROC pCompareProc, CTableItemUI* pParent, int nColumn, bool bAsc)
{
	if (!pParent || !pCompareProc)
		return;
	int nChildCnt = pParent->num_child();
	if (nChildCnt > 0)
	{
		CTableItemUI* pNode = NULL;
		if (nChildCnt > 1)
		{
			gs_pTableSortCompareProc = pCompareProc;
			gs_bTableSortAsc = bAsc;
			gs_nTableSortCurSortCol = nColumn;

			//对组节点pParent的直属子节点进行排序
			CStdPtrArray items;
			for (int i = 0; i < nChildCnt; i++)
			{
				pNode = pParent->get_child(i);
				if (pNode)
					items.Add(pNode);
			}
			items.Sort(Table_Col_CompareFunc);

			//排序后对直属子节点的索引重新设置
			for (int i = 0; i < nChildCnt; i++)
			{
				pNode = static_cast<CTableItemUI*>(items.GetAt(i));
				if (pNode)
				{
					pParent->set_child(pNode, i);
				}
			}

			gs_pTableSortCompareProc = NULL;
			gs_bTableSortAsc = false;
			gs_nTableSortCurSortCol = -1;
		}

		for (int i = 0; i < nChildCnt; i++)
		{
			pNode = pParent->get_child(i);
			if (pNode && pNode->IsGroup())
			{
				SortChildNode(pCompareProc, pNode, nColumn, bAsc);
			}
		}
	}
}

//************************************
// @brief:    根据目标数组排序子节点
// @note:     
// @param: 	  TABLESORTPROC pCompareProc 
// @param: 	  CTableItemUI * pParent 
// @param: 	  int nColumn 
// @param: 	  bool bAsc 
// @param: 	  CStdPtrArray & items 
// @param: 	  int nChildBegin 
// @return:   void 
//************************************
void SortChildNode(TABLESORTPROC pCompareProc, CTableItemUI* pParent, int nColumn, bool bAsc, CStdPtrArray& items, int nChildBegin)
{
	if (!pParent || !pCompareProc)
		return;
	int nChildCnt = items.GetSize();
	if (nChildCnt > 0)
	{
		CTableItemUI* pNode = NULL;
		if (nChildCnt > 1)
		{
			gs_pTableSortCompareProc = pCompareProc;
			gs_bTableSortAsc = bAsc;
			gs_nTableSortCurSortCol = nColumn;

			//对组节点pParent的直属子节点进行排序
			items.Sort(Table_Col_CompareFunc);

			//排序后对直属子节点的索引重新设置
			for (int i = nChildBegin; i < nChildCnt; i++)
			{
				pNode = static_cast<CTableItemUI*>(items.GetAt(i));
				if (pNode)
				{
					pParent->set_child(pNode, i);
				}
			}

			gs_pTableSortCompareProc = NULL;
			gs_bTableSortAsc = false;
			gs_nTableSortCurSortCol = -1;
		}

		for (int i = nChildBegin; i < nChildCnt; i++)
		{
			pNode = pParent->get_child(i);
			if (pNode && pNode->IsGroup())
			{
				SortChildNode(pCompareProc, pNode, nColumn, bAsc);
			}
		}
	}
}

bool CTableUI::Sort( int nColumn /*= -1*/, int nAsc /*= -1*/, int nBegin /*= 0*/, int nSize /*= -1*/ )
{
	if (m_pTableSortCompareProc)
	{
		if (nColumn >= 0)
		{
			SetSortCol(nColumn);
		}
		bool bAsc = true;
		if (nAsc != -1)
		{
			if (nAsc == 0)
			{
				bAsc = false;
			}
			else if (nAsc == 1)
			{
				bAsc = true;
			}
		}
		if (EnableHeaderItemSort())
		{
			if (GetHeader())
			{
				CListHeaderItemUI* pHeadItem = GetCol(GetSortCol());
				if (pHeadItem)
				{
					if (nAsc == -1)
						bAsc = pHeadItem->IsAscending();
					else
						pHeadItem->m_bAscending = bAsc;
				}
			}
		}
		int nCount = GetCount();
		if (nSize < 0)
			nSize = nCount;
		if (nSize > nCount)
			nSize = nCount;
		if (nSize > nCount - nBegin)
			nSize = nCount - nBegin;

		//对所有节点进行排序，即对根节点进行子节点排序
		if (nBegin == 0 && nSize == nCount)
		{
			SortChildNode(m_pTableSortCompareProc, GetRoot(), GetSortCol(), bAsc);
		}
		else
		{
			//对一定范围的节点进行排序
			CTableItemUI* pItemBegin = static_cast<CTableItemUI*>(GetItem(nBegin));
			if (pItemBegin)
			{
				CTableItemUI* pItemParent = pItemBegin->get_parent();
				if (pItemParent)
				{
					//找到一级排序子节点
					int nBoundry = nBegin + nSize;
					CStdPtrArray items;
					int nItemIndex = pItemBegin->GetIndex();
					CTableItemUI* pItemEnd = pItemBegin;
					while (nItemIndex < nBoundry)
					{
						if (!pItemEnd)
							break;
						items.Add(pItemEnd);
						pItemEnd = pItemParent->get_nextsibling(pItemEnd);
						if (pItemEnd)
						{
							nItemIndex = pItemEnd->GetIndex();
						}
					}

					//检查排序范围中的节点是否是完整树
					pItemEnd = static_cast<CTableItemUI*>(items.GetAt(items.GetSize() - 1));
					if (pItemEnd)
					{
						CTableItemUI* pItemLast = static_cast<CTableItemUI*>(GetItem(nBoundry - 1));
						if (pItemLast)
						{
							bool bFind = false;
							while(pItemLast)
							{
								pItemLast = pItemLast->get_parent();
								if (pItemLast && pItemLast == pItemEnd)
								{
									bFind = true;
									break;
								}
							}
							//未按照指定要求设置参数
							if (!bFind)
							{
								return false;
							}
						}
					}


					//查找第一个排序节点对于父节点的索引
					int nChildBegin = 0;
					int nChildCnt = pItemParent->num_child();
					//CTableItemUI* pItem = NULL;
					for (int i = 0; i < nChildCnt; i++)
					{
						CTableItemUI* pItem = pItemParent->get_child(i);
						if (pItem == pItemBegin)
						{
							nChildBegin = i;
							break;
						}
					}

					//排序
					SortChildNode(m_pTableSortCompareProc, pItemParent, GetSortCol(), bAsc, items, nChildBegin);
				}
			}
		}
		int nAllIndex = 0;
		CStdPtrArray* pArr = GetItemArray();
		SetTableItemIndex(GetRoot(), nAllIndex, *pArr);
		Resize();
		Invalidate();
	}
	return true;
}

void CTableUI::SetItemVisible( CTableItemUI* pControl, bool bVisible )
{
	pControl->node_show(bVisible);
	int nCnt = pControl->num_child();
	for (int i = 0; i < nCnt; i++)
		SetItemVisible(pControl->get_child(i), bVisible);
}

CTableItemUI* CTableUI::GetRoot()
{
	return static_cast<CTableItemUI*>(get_root());
}

bool CTableUI::node_show_ploy( INode* pNode )
{
	NODESHOWPLOYFUNC pFun = GetShowPloyFunc();
	if (pFun)
		return pFun(dynamic_cast<CTableItemUI*>(pNode), this);
	return INodeOwner::node_show_ploy(pNode);
}

void CTableUI::SetShowPloyFunc( NODESHOWPLOYFUNC pFunc )
{
	m_pShowPloyFunc = pFunc;
}

NODESHOWPLOYFUNC CTableUI::GetShowPloyFunc()
{
	return m_pShowPloyFunc;
}

void CTableUI::RemoveAll()
{
	__super::RemoveAll();
	CTableItemUI* pRoot = GetRoot();
	if (pRoot)
	{
		pRoot->remove_all_node();
	}
}

void CTableUI::SetSortCallBack( TABLESORTPROC pCompare )
{
	m_pTableSortCompareProc = pCompare;
}

void CTableUI::CheckItem( bool bCheck, int nIndex /*= -1*/, UINT uFlag /*= UIITEMFLAG_CHECKED | UIITEMFLAG_ALL | UIITEMFLAG_NOTIFY*/ )
{
	if (!m_bEnableCheck)
		return;
	bool bFlagVisible = false;
	bool bFlagEnabled = false;
	bool bFlagCheck = false;
	if (uFlag & UIITEMFLAG_VISIBLE)
	{
		bFlagVisible = true;
	}
	else if (uFlag & UIITEMFLAG_ENABLED)
	{
		bFlagEnabled = true;
	}
	else if (uFlag & UIITEMFLAG_CHECKED)
	{
		bFlagCheck = true;
	}
	//没一个为真
	if (!(bFlagVisible || bFlagEnabled || bFlagCheck))
		return;

	CCheckBoxUI* pCheck = NULL;
	CControlUI* pControl = NULL;
	if (nIndex == -1)
	{
		//全部check
		int nCnt = GetCount();
		for (int i = 0; i < nCnt; i++)
		{
			pControl = GetItem(i);
			if (pControl)
			{
				pCheck = static_cast<CListItemUI*>(pControl)->GetCheck();
				if (pCheck)
				{
					if (bFlagCheck)
					{
						if (pCheck->IsVisible() && pCheck->IsEnabled())
							pCheck->SetChecked(bCheck, UIITEMFLAG_CHECKED);
					}
					else if (bFlagVisible)
					{
						pCheck->SetVisible(bCheck);
					}
					else if (bFlagEnabled)
					{
						pCheck->SetEnabled(bCheck);
					}
					if ((uFlag&UIITEMFLAG_NOTIFY) && bFlagCheck)
						SendNotify( UINOTIFY_TVN_CHECK, (WPARAM)bCheck, (LPARAM)pControl);	
				}
			}
		}
		//表头check的通知
		if (bFlagCheck)
			SendNotify(UINOTIFY_TVN_CHECK, (WPARAM)bCheck, (LPARAM)NULL);
	}
	else
	{
		pControl = GetItem(nIndex);
		if (pControl)
		{
			pCheck = static_cast<CListItemUI*>(pControl)->GetCheck();
			if (pCheck)
			{
				if (bFlagCheck)
				{
					if (pCheck->IsVisible() && pCheck->IsEnabled())
					{
						pCheck->SetChecked(bCheck, UIITEMFLAG_CHECKED);
						if (pCheck)
							pCheck->Indef(false);
						CTableItemUI* pCheckItem = static_cast<CTableItemUI*>(pControl);
						CTableItemUI* pSubItem = NULL;
						if (pCheckItem->IsGroup() && (uFlag & UIITEMFLAG_ALL))
						{
							int nChildCnt = pCheckItem->num_child();
							for (int i = 0; i < nChildCnt; i++)
							{
								pSubItem = pCheckItem->get_child(i);
								if (pSubItem)
								{
									CheckItem(bCheck, pSubItem->GetIndex(), uFlag & ~UIITEMFLAG_NOTIFY);
								}
							}
						}
						pSubItem = pCheckItem->get_parent();
						if (pSubItem)
							pSubItem->CheckLegal();
					}
				}
				else if (bFlagVisible)
				{
					pCheck->SetVisible(bCheck);
				}
				else if (bFlagEnabled)
				{
					pCheck->SetEnabled(bCheck);
				}
				if ((uFlag&UIITEMFLAG_NOTIFY) && bFlagCheck)
					SendNotify( UINOTIFY_TVN_CHECK, (WPARAM)bCheck, (LPARAM)pControl);	
			}
		}
	}
	CheckLegal();
}

bool CTableUI::Event( TEventUI& event )
{
	bool bRet = false;
	switch(event.nType)
	{
	case UIEVENT_KEYDOWN:
		{
			switch(event.wParam)
			{
			case VK_LEFT:
				{
					CControlUI* pSel = GetCurSel();
					CTableItemUI* pItem = dynamic_cast<CTableItemUI*>(pSel);
					if (pItem)
					{
						if (pItem->IsGroup() && pItem->IsExpand())
						{
							pItem->Expand(false);
							bRet = true;
						}
					}
				}
				break;
			case VK_RIGHT:
				{
					CControlUI* pSel = GetCurSel();
					CTableItemUI* pItem = dynamic_cast<CTableItemUI*>(pSel);
					if (pItem)
					{
						if (pItem->IsGroup() && !pItem->IsExpand())
						{
							pItem->Expand(true);
							bRet = true;
						}
					}
				}
				break;
			}
			if (bRet)
				SendNotify(UINOTIFY_LB_KEYDOWN, event.wParam, event.lParam);
		}
		break;
	}
	if (!bRet)
		return __super::Event(event);
	return true;
}
//////////////////////////////////////////////////////////////////////////
CListHeaderUI::CListHeaderUI()
{
	SetLayoutType(UILAYOUT_LEFTTORIGHT);;
	SetStyle(_T("ListHeader"));
	EnableScrollBar(false, false);
	m_szChildSize.cx = 0;
	m_szChildSize.cy = 0;
}

CListHeaderUI::~CListHeaderUI()
{
}


SIZE CListHeaderUI::OnChildSize(RECT& rcWinow, RECT& rcClient)
{
	if (GetLayoutType() == UILAYOUT_LEFTTORIGHT)
	{
		//CControlUI* pOwner = GetParent();
		int l, t, r, b;
		CControlUI* pControl = NULL;
		RECT rect = rcClient;
		rect.right = rcClient.left;
		int nWidth = rcClient.right - rcClient.left;
		int nFix = 0;
		int nPercentCount = 0;
		int nCnt = GetCount();
		
//		int nDestClientWidth = 0;
		for (int i = 0; i < nCnt; i++)
		{
			pControl = GetItem(i);
			if (pControl && static_cast<CListHeaderItemUI*>(pControl)->IsVisible())
			{
				if (pControl->IsAutoSize())
				{
					nPercentCount += DPI_SCALE(static_cast<CListHeaderItemUI*>(pControl)->GetPercentWidth());
				}
				else
				{
					pControl->GetCoordinate(l, t, r, b);
					nFix += DPI_SCALE(r - l);
				}
			}
		}
		if (nPercentCount <= 0)
			nPercentCount = 1;
		for (int i = 0; i < nCnt; i++)
		{
			pControl = GetItem(i);
			if (pControl)
			{
				if (static_cast<CListHeaderItemUI*>(pControl)->IsVisible())
				{
					if (pControl->IsAutoSize())
					{
						rect.left = rect.right;
						int nCellWidth = (nWidth - nFix)*  DPI_SCALE((static_cast<CListHeaderItemUI*>(pControl)->GetPercentWidth())) / nPercentCount;
						rect.right = rect.left + (nCellWidth>ListHeaderItemMinWidth?nCellWidth:ListHeaderItemMinWidth);
						pControl->SetCoordinate(0, 0, DPI_SCALE_BACK(rect.right - rect.left), 0, UIANCHOR_LEFTTOP);
						pControl->SetRect(rect);
						pControl->SetAttribute(_T("Dragable"), _T("0"));
					}
					else
					{
						pControl->GetCoordinate(l, t, r, b);
						rect.left = rect.right;
						rect.right = rect.left + DPI_SCALE(r - l);
						pControl->SetRect(rect);
					}
				}
			}
		}
		SIZE sz = {rect.right - rcClient.left, rect.bottom - rect.top};
		m_szChildSize = sz;
		return sz;
	}
	else 
	{
		m_szChildSize = CLayoutUI::OnChildSize(rcWinow, rcClient);
		return m_szChildSize;
	}
}

SIZE CListHeaderUI::GetChildSize()
{
	return m_szChildSize;
}
//////////////////////////////////////////////////////////////////////////
CListHeaderItemUI::CListHeaderItemUI()
{
	SetStyle(_T("ListHeaderItem"));
	SetState(UISTATE_NORMAL);
	SetEnabled(false);
	SetMouseEnabled(true);
	ModifyFlags(UICONTROLFLAG_SETCURSOR|UICONTROLFLAG_SETFOCUS, 0);
	m_bDragStatus = false;
	m_bDragable = false;
	m_bEnableSort = false;
	m_bAscending = false;
	SetAnchor(UIANCHOR_LTLB);
	m_ptLastMouse.x = 0;
	m_ptLastMouse.y = 0;
	m_uColAlignStyle = DT_SINGLELINE|DT_VCENTER|DT_LEFT_EX|DT_NOPREFIX;
	m_nPercentWidth = 0;
}

CListHeaderItemUI::~CListHeaderItemUI()
{
}


void CListHeaderItemUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("enable_sort")))
	{
		m_bEnableSort = (bool)!!_ttoi(lpszValue);
		if (m_bEnableSort)
			SetEnabled(true);
	}
	else if (equal_icmp(lpszName, _T("")))
	{
		m_bDragable = (bool)!!_ttoi(lpszValue);
		if (m_bDragable)
			SetEnabled(true);
	}
	else if (equal_icmp(lpszName, _T("width")))
	{
		GetCoordinate()->rect.right = _ttoi(lpszValue);
		GetCoordinate()->rect.left = 0;
		LPCTSTR pFind = _tcschr(lpszValue, '%');
		if (pFind)
		{
			SetAutoSize(true);
			m_nPercentWidth = GetCoordinate()->rect.right;
		}
	}
	else if (equal_icmp(lpszName, _T("colhalign")))
	{
		if(equal_icmp(lpszValue, _T("left"))) {
			m_uColAlignStyle &= ~(DT_CENTER | DT_RIGHT_EX);
			m_uColAlignStyle |= DT_LEFT_EX;
		}
		if(equal_icmp(lpszValue, _T("center"))) {
			m_uColAlignStyle &= ~(DT_LEFT_EX | DT_RIGHT_EX);
			m_uColAlignStyle |= DT_CENTER;
		}
		if(equal_icmp(lpszValue, _T("right"))) {
			m_uColAlignStyle &= ~(DT_LEFT_EX | DT_CENTER);
			m_uColAlignStyle |= DT_RIGHT_EX;
		}
	}
	else
		__super::SetAttribute(lpszName, lpszValue);
}
bool CListHeaderItemUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_SETCURSOR:
		{
			RECT rcSeparator = GetArrowRect();
			if (m_bDragable && PtInRect(&rcSeparator,event.ptMouse))
				::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
			else
				CControlUI::Event(event);
			return true;
		}
		break;
	case UIEVENT_BUTTONDOWN:
		{
			RECT rcSeparator = GetArrowRect();;
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				if( m_bDragable ) {
					m_bDragStatus = true;
					m_ptLastMouse = event.ptMouse;
				}
			}
			else{
				SetState(UISTATE_DOWN);
				this->Invalidate();
			}
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			if(m_bDragStatus) 
			{
				m_bDragStatus = false;
				RECT rc = GetRect();
				rc.right = GetArrowRight(event.ptMouse);
				GetCoordinate()->rect.right = DPI_SCALE_BACK(rc.right - rc.left);
				if( GetOwner() ) 
				{
					GetOwner()->Resize();
					GetOwner()->Invalidate();
				}
			}
			else
			{
				SetState(UISTATE_NORMAL);
				if (m_bEnableSort && GetOwner() != NULL)
				{
					m_bAscending = !m_bAscending;
					CListUI* pList = static_cast<CListUI*>(GetOwner());
					if (pList)
					{
						pList->Sort(GetIndex());
					}
				}
				this->Invalidate();
			}
		}
		break;
	case UIEVENT_MOUSEMOVE:
		{
			if (GetState() != UISTATE_OVER)
			{
				SetState(UISTATE_OVER);
				this->Invalidate();
			}
		}
		break;
	case UIEVENT_MOUSELEAVE:
		{
			if (GetState() != UISTATE_NORMAL)
			{
				SetState(UISTATE_NORMAL);
				this->Invalidate();
			}
		}
		break;
	}
	return CControlUI::Event(event);
}

bool CListHeaderItemUI::IsAscending()
{
	return m_bAscending;
}

UINT CListHeaderItemUI::GetColAlign()
{
	return m_uColAlignStyle;
}

int CListHeaderItemUI::GetPercentWidth()
{
	return m_nPercentWidth;
}

void CListHeaderItemUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	StyleObj* pStyle = NULL;

	pStyle = (StyleObj*)GetStyle(_Style_ListHeaderItem_Bk);
	if (pStyle)
		pStyle->SetState(GetState());

	GetWindow()->LockUpdate();
	pStyle = (StyleObj*)GetStyle(_Style_ListHeaderItem_Sort);
	if (pStyle)
	{
		if (m_bEnableSort)
		{
			UINT uColSel = static_cast<CListUI*>(GetOwner())->GetSortCol();
			if (uColSel == this->GetIndex())
			{
				pStyle->SetState(m_bAscending?UISTATE_OVER:UISTATE_NORMAL);
				pStyle->SetVisible(true);
			}
			else
			{
				pStyle->SetVisible(false);
			}
		}
		else
		{
			pStyle->SetVisible(false);
		}
	}
	GetWindow()->UnLockUpdate();

	pStyle = (StyleObj*)GetStyle(_Style_ListHeaderItem_Txt);
	if (pStyle)
		pStyle->SetState(GetState());

	__super::Render(pRenderDC, rcPaint);
}

RECT CListHeaderItemUI::GetArrowRect()
{
	RECT rcSeparator = GetRect();
	if (GetUIEngine()->IsRTLLayout())
	{
		rcSeparator.right = min(rcSeparator.left + ListHeaderItemMinWidth/2, rcSeparator.right - ListHeaderItemMinWidth/2);

	}
	else
	{
		rcSeparator.left = max(rcSeparator.right - ListHeaderItemMinWidth/2, rcSeparator.left + ListHeaderItemMinWidth/2);
	}

	return rcSeparator;
}

int CListHeaderItemUI::GetArrowRight( POINT& pt )
{
	RECT rc = GetRect();

	if (GetUIEngine()->IsRTLLayout())
	{
		rc.right += m_ptLastMouse.x - pt.x;
		if (rc.right - rc.left < ListHeaderItemMinWidth)
			rc.right = rc.left + ListHeaderItemMinWidth;
	}
	else
	{
		rc.right -= m_ptLastMouse.x - pt.x;
		if (rc.right - rc.left < ListHeaderItemMinWidth)
			rc.right = rc.left + ListHeaderItemMinWidth;

	}
	return rc.right;
}

//////////////////////////////////////////////////////////////////////////
CListContainerItemUI::CListContainerItemUI()
{

}

CListContainerItemUI::~CListContainerItemUI()
{
}

//////////////////////////////////////////////////////////////////////////
class CListItemDataImpl
{
public:
	~CListItemDataImpl()
	{
		m_mapColsText.clear();
		m_mapColsUserdata.clear();
	}
	std::map<UINT, tstring> m_mapColsText;
	std::map<UINT, unsigned long long> m_mapColsUserdata;
};
CListItemUI::CListItemUI()
:m_pCheck(NULL),
m_pItems(NULL)
{
	m_nMaskClick = 0;
	m_bMaskCols = false;
	m_bCalc = true;
	SetCoordinate(0, 0, 0, 28);
	m_bDrawSuper = false;
	m_bEnableItemState = true;
	m_bEnableItems = true;
	m_bEnableColsText = true;
	m_bEnableSuper = true;
	SetMouseEnabled(true);
	SetStyle(_T("ListItem"));
	ModifyFlags(UICONTROLFLAG_SETFOCUS, 0);
	m_pDataImpl = new CListItemDataImpl;
}

CListItemUI::~CListItemUI()
{
	if (m_pItems)
		UICustomControlHelper::RemoveAllChild(*m_pItems);
	m_pItems = NULL;

	if (m_pCheck)
		delete m_pCheck;
	m_pCheck = NULL;
	if (m_pDataImpl)
	{
		delete m_pDataImpl;
	}
	m_pDataImpl = NULL;
}


bool CListItemUI::Activate()
{
	if (!CControlUI::Activate())
		return false;

	CListUI* pList = static_cast<CListUI*>(GetParent());
	if (pList)
	{
		pList->SelectItem(this);
		pList->EnsureVisible(this);
		pList->SendNotify(UINOTIFY_CLICK, NULL, LPARAM(this));
	}
	return true;
}

bool CListItemUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_LDBCLICK:
		{
			SHORT sCtrl = ::GetKeyState(VK_CONTROL);
			SHORT sShift = ::GetKeyState(VK_SHIFT);
			if (sCtrl < 0 || sShift <0)
				return false;
			CListUI* pList = static_cast<CListUI*>(GetParent());
			if (pList)
			{
				pList->SelectItem(this);
				pList->SendNotify(UINOTIFY_DBCLICK, 0, LPARAM(this));
			}
		}
		return true;
	case UIEVENT_BUTTONUP:
		{
			SHORT sCtrl = ::GetKeyState(VK_CONTROL);
			SHORT sShift = ::GetKeyState(VK_SHIFT);
			CListUI* pList = static_cast<CListUI*>(GetParent());
			if (pList)
			{
				bool bMulti = pList->IsEnableMultiSelect();
				if (bMulti && sCtrl < 0 && sShift >= 0)
				{
					pList->SelectItemAdd(this);
				}
				else if (bMulti && sShift < 0 && sCtrl >=0)
				{
					pList->SelectItemEnd(this);
				}
				else
					Activate();
			}
		}
		return true;
	case UIEVENT_RBUTTONUP:
		{
			CListUI* pList = static_cast<CListUI*>(GetParent());
			if (pList)
			{
				if (!IsSelected())
					pList->SelectItem(this);
				pList->SendNotify(UINOTIFY_RCLICK, NULL, LPARAM(this));
			}
		}
		return true;
	case UIEVENT_KEYDOWN:
	case UIEVENT_SCROLLWHEEL:
		{
			CListUI* pList = static_cast<CListUI*>(GetParent());
			if (pList)
				pList->CListUI::ChildEvent(event);
		}
		return true;
	}
	return __super::Event(event);
}

void CListItemUI::SetSubItemText(int nCol, LPCTSTR lpszText)
{
	m_pDataImpl->m_mapColsText[nCol] = lpszText;
}

LPCTSTR CListItemUI::GetSubItemText( int nCol )
{
	std::map<UINT, tstring>::iterator it = m_pDataImpl->m_mapColsText.find(nCol);
	if (it != m_pDataImpl->m_mapColsText.end())
	{
		return it->second.c_str();
	}
	return NULL;
}

void CListItemUI::SetSubItemUserdata(int nCol, unsigned long long uUserData)
{
	m_pDataImpl->m_mapColsUserdata[nCol] = uUserData;
}

unsigned long long CListItemUI::GetSubItemUserdata(int nCol)
{
	std::map<UINT, unsigned long long>::iterator it = m_pDataImpl->m_mapColsUserdata.find(nCol);
	if (it != m_pDataImpl->m_mapColsUserdata.end())
	{
		return it->second;
	}
	return 0;
}

CCheckBoxUI* CListItemUI::GetCheck()
{
	return m_pCheck;
}

void CListItemUI::SetCheck( CCheckBoxUI* pCheck )
{
	m_pCheck = pCheck;
	if (m_pCheck)
		m_pCheck->SetId(_T("_listitem_checkitem"));
}

bool CListItemUI::Add(CControlUI* pControl)
{
	if (m_pItems == NULL)
		m_pItems = new CStdPtrArray;
	m_bCalc = false;
	pControl->OnEvent += MakeDelegate(this, &CListItemUI::ChildEvent);
	pControl->EventToParent(true);

	if (!m_bMaskCols)
		pControl->EnableRTLLayout(false);

	UICustomControlHelper::AddChildControl(pControl, this, *m_pItems);
	return true;
}

void CListItemUI::SetRect(RECT& rectRegion)
{
	m_bCalc = false;
	__super::SetRect(rectRegion);
	if (!IsRectEmpty(GetRectPtr()))
	{
		RECT rcClient = GetClientRect();

		int nCnt = GetStyleCount();
		//StyleObj* pStyleObj = NULL;
		for (int i = 0; i < nCnt; i++)
		{
			StyleObj* pStyleObj = GetStyle(i);
			if (pStyleObj)
				pStyleObj->OnSize(rcClient);
		}
	}
}

bool CListItemUI::ChildEvent(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_LDBCLICK:
		{
			int nMaskClick = m_nMaskClick;
			if (nMaskClick == 2)
				break;
			SHORT sCtrl = ::GetKeyState(VK_CONTROL);
			SHORT sShift = ::GetKeyState(VK_SHIFT);
			if (sCtrl < 0 || sShift <0)
				return false;
			CListUI* pList = static_cast<CListUI*>(GetParent());
			if (pList)
			{
				pList->SelectItem(this);
				pList->SendNotify(UINOTIFY_DBCLICK, (WPARAM)event.pSender, (LPARAM)this);
			}
			if (nMaskClick == 1)
				event.nType = UIEVENT_UNUSED;
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			int nMaskClick = m_nMaskClick;
			if (nMaskClick == 2)
				break;
			SHORT sCtrl = ::GetKeyState(VK_CONTROL);
			SHORT sShift = ::GetKeyState(VK_SHIFT);
			CListUI* pList = static_cast<CListUI*>(GetParent());
			if (pList)
			{
				bool bMulti = pList->IsEnableMultiSelect();
				if (bMulti && sCtrl < 0 && sShift >= 0)
				{
					CListItemUI::Event(event);
				}
				else if (bMulti && sShift < 0 && sCtrl >=0)
				{
					CListItemUI::Event(event);
				}
				else
				{
					pList->SelectItem(this);
					pList->EnsureVisible(this);
					pList->SendNotify(UINOTIFY_CLICK, (WPARAM)event.pSender, (LPARAM)this);
				}
			}
			if (nMaskClick == 1)
				event.nType = UIEVENT_UNUSED;
		}
		break;
	case UIEVENT_KEYDOWN:
	case UIEVENT_SCROLLWHEEL:
		{
			this->SendEvent(event);
		}
		break;
	}
	return true;
}

void CListItemUI::CalcRect()
{
	if (m_pItems == NULL)
		return;
	RECT rcItem = GetRect();
	RECT rcClient = GetClientRect();
	if (!m_bMaskCols)
	{
		CListUI* pList = static_cast<CListUI*>(GetParent());
		if (pList)
		{
			int nColCount = pList->GetCols();
			if (nColCount > 0)
			{
				int nWidth,nHeight;
				RECT rcSubItem,rcCol;
				//LPCTSTR lpszText = NULL;
				CListHeaderItemUI* pHeaderItem = NULL;
				//CControlUI* pItem = NULL;
				RECT rcEmpty = {0};
				int l, t, r, b;
				int nControlCount = GetCount();
				int nColIndex = 0;
				for (int i = 0; i < nControlCount; i++)
				{
					CControlUI* pItem = GetItem(i);
					pHeaderItem = pList->GetCol(nColIndex);
					if (pHeaderItem)
					{
						//在列里的控件
						if (pItem)
							ZeroMemory(pItem->GetRectPtr(), sizeof(RECT));//pItem->SetRect(rcEmpty, false); // xqb去掉SetRect重载
						nColIndex++;
						if (pHeaderItem->IsVisible())
						{
							rcCol = pHeaderItem->GetRect();
							if (pItem)
							{
								rcCol.top = rcClient.top;
								rcCol.bottom = rcClient.bottom;
								pItem->GetCoordinate(l, t, r, b);
								rcSubItem.left = l;
								rcSubItem.top = t;
								rcSubItem.right = r;
								rcSubItem.bottom = b;
								DPI_SCALE(&rcSubItem);
								GetAnchorPos(pItem->GetAnchor(), &rcCol, &rcSubItem);
								nWidth = rcSubItem.right - rcSubItem.left;
								nHeight = rcSubItem.bottom - rcSubItem.top;
								if (nWidth == 0)
								{
									rcSubItem.left = rcCol.left + 2;
									rcSubItem.right = rcCol.right - 2;
								}
								if (nHeight == 0)
								{
									rcSubItem.top = rcClient.top;
									rcSubItem.bottom = rcClient.bottom;
								}

								if (rcSubItem.left < rcCol.left)
									rcSubItem.left = rcCol.left;
								if (rcSubItem.top < rcCol.top)
									rcSubItem.top = rcCol.top;
								if (rcSubItem.right > rcCol.right)
									rcSubItem.right = rcCol.right;
								if (rcSubItem.bottom > rcCol.bottom)
									rcSubItem.bottom = rcCol.bottom;

								pItem->SetRect(rcSubItem/*, true*/);// xqb去掉SetRect重载,这边阿拉伯版本会不会有问题？
							}
							else
								break;
						}
					}
					else
					{
						//不在列里的控件需要摆放位置
						if (pItem->IsNeedScroll())
						{
							pItem->OnSize(rcClient);
						}
						else
						{
							pItem->OnSize(rcItem);
						}
					}
				}
				return;
			}
		}
	}

	UICustomControlHelper::LayoutChildNormal(rcItem, rcClient, *m_pItems, false);
}

void CListItemUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("MaskClick")))
	{
		m_nMaskClick = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("MaskCols")))
	{
		m_bMaskCols = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("DrawNode")))
	{
		m_bDrawSuper = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("EnableItemState")))
	{
		m_bEnableItemState = (bool)!!_ttoi(lpszValue);
	}
	else
		__super::SetAttribute(lpszName, lpszValue);
}

void CListItemUI::ReCalcRect()
{
	m_bCalc = false;
}

CControlUI* CListItemUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if (m_pItems)
	{
		if (!FindControlTest(this, pData, uFlags))
			return NULL;

		if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
			CControlUI* pControl = CControlUI::FindControl(Proc, pData, uFlags);
			if( pControl)
				return pControl;
		}

		CControlUI* pControl = UICustomControlHelper::FindChildControl(*m_pItems, Proc, pData, uFlags);
		if (pControl)
			return pControl;
	}

	return CControlUI::FindControl(Proc, pData, uFlags);

}
CControlUI* CListItemUI::GetItem(int nIndex)
{
	if (m_pItems)
	{
		if( nIndex < 0 || nIndex >= m_pItems->GetSize() ) return NULL;
		return static_cast<CControlUI*>(m_pItems->GetAt(nIndex));
	}
	else
	{
		return __super::GetItem(nIndex);
	}
}

CControlUI* CListItemUI::GetItem(LPCTSTR lpszId)
{
	if (m_pItems)
	{
		tstring strID = lpszId;
		int i = strID.find_first_of(L'.');
		if (i < 0)
		{
			//CControlUI* pControl = NULL;
			int nCnt = m_pItems->GetSize();
			for( int j = 0;  j < nCnt; j++ )
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_pItems->GetAt(j));
				if (equal_icmp(pControl->GetId(), lpszId))
					return pControl;
			}
			return NULL;
		}
		else
		{
			CControlUI* pControl = GetItem(strID.substr(0, i).c_str());
			strID = strID.substr(i + 1);
			if (pControl == NULL)
			{return NULL ;		}
			return pControl->GetItem(strID.c_str());
		}	
	}
	else
	{
		return __super::GetItem(lpszId);
	}
}

int CListItemUI::GetCount()
{
	if (m_pItems)
		return m_pItems->GetSize();
	return __super::GetCount();
}

bool CListItemUI::Remove(CControlUI* pControl)
{
	if (m_pItems)
		return UICustomControlHelper::RemoveChildControl(pControl, *m_pItems);
	return __super::Remove(pControl);
}

void CListItemUI::SetINotifyUI(INotifyUI* pINotify)
{
	__super::SetINotifyUI(pINotify);
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
		GetItem(i)->SetINotifyUI(pINotify);
}

void CListItemUI::SetManager(CWindowUI* pManager, CControlUI* pParent)
{
	if (dynamic_cast<CComboBoxUI*>(pParent))
	{
		CComboBoxUI* pCombo = dynamic_cast<CComboBoxUI*>(pParent);
		__super::SetManager(pManager, pCombo->GetList());
		int nCnt = GetCount();
		//CControlUI* pSub = NULL;
		for (int i = 0; i < nCnt; i++)
		{
			CControlUI* pSub = GetItem(i);
			if (pSub)
			{
				pSub->SetManager(pManager, this);
			}
		}
	}
	else
	{
		__super::SetManager(pManager, pParent);
		int nCnt = GetCount();
		//CControlUI* pSub = NULL;
		for (int i = 0; i < nCnt; i++)
		{
			CControlUI* pSub = GetItem(i);
			if (pSub)
			{
				pSub->SetManager(pManager, this);
			}
		}
	}
}

void CListItemUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	if (!m_bCalc)
	{
		CalcRect();
		m_bCalc = true;
	}

	CListUI* pList = static_cast<CListUI*>(GetParent());
	if (pList)
	{
		RECT rcItem = GetRect();
		RECT rcClient = GetClientRect();
		if (EnableSuper() && m_bDrawSuper)
			__super::Render(pRenderDC, rcPaint);

		if (m_pItems && EnableItems())
		{
			RECT rcListClient = pList->GetClientRect();
			RECT rcList = pList->GetRect();
			::IntersectRect(&rcListClient, &rcItem, &rcListClient);
			if (!::EqualRect(&rcItem, &rcClient) && pList->EnableCheck())
				rcListClient.left = rcListClient.left + pList->CheckWidth();
			::IntersectRect(&rcClient, &rcClient, &rcListClient);

			UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, *m_pItems, 0, -1);
		}
		else if (EnableColsText() && pList && pList->GetCols() > 0)
		{
			RECT rcListClient = pList->GetClientRect();
			RECT rcList = pList->GetRect();
			::IntersectRect(&rcListClient, &rcItem, &rcListClient);
			if (!::EqualRect(&rcItem, &rcClient) && pList->EnableCheck())
				rcListClient.left = rcListClient.left + pList->CheckWidth();
			::IntersectRect(&rcClient, &rcClient, &rcListClient);

			ClipObj clip;
			ClipObj::GenerateClip(pRenderDC->GetDC(), rcClient, clip);

			TextStyle* pText = static_cast<TextStyle*>(GetStyle(_Style_ListItem_Txt));
			if (pText)
			{
				int nCnt = pList->GetCols();
				RECT rcSubItem;
				LPCTSTR lpszText = NULL;
				CListHeaderItemUI* pColHeader = NULL;
				for (int i = 0; i < nCnt; i++)
				{
					pColHeader = pList->GetCol(i);
					if (pColHeader && pColHeader->IsVisible())
					{
						rcSubItem = pColHeader->GetRect();
						rcSubItem.left += 2;
						rcSubItem.right -= 2;
						rcSubItem.top = rcItem.top;
						rcSubItem.bottom = rcItem.bottom;
						lpszText = GetSubItemText(i);
						if (lpszText)
							pRenderDC->DrawText(pText->GetFontObj(),  rcSubItem, lpszText, 0, pColHeader->GetColAlign());
					}
				}
			}
		}
		else if (!m_bDrawSuper && EnableSuper())
		{
			__super::Render(pRenderDC, rcPaint);
		}
	}
}

RECT CListItemUI::GetClientRect()
{
	CListUI* pList = static_cast<CListUI*>(GetParent());
	if (pList)
	{
		RECT rcClient = GetRect();

		if (GetUIEngine()->IsRTLLayout())
		{
			rcClient.right -= pList->CheckWidth();
		}
		else
		{
			rcClient.left += pList->CheckWidth();
		}
		return rcClient;
	}
	return __super::GetClientRect();
}

void CListItemUI::SetVisible( bool bVisible)
{
	if (m_pItems)
	{
		int nCnt = GetCount();
		for( int i = 0; i < nCnt; i++ ) 
			static_cast<CControlUI*>(m_pItems->GetAt(i))->SetInternVisible(bVisible);
	}
	__super::SetVisible(bVisible);
}

void CListItemUI::SetInternVisible( bool bVisible )
{
	if (m_pItems)
	{
		int nCnt = GetCount();
		for( int i = 0; i < nCnt; i++ ) 
			static_cast<CControlUI*>(m_pItems->GetAt(i))->SetInternVisible(bVisible);
	}
	__super::SetInternVisible(bVisible);
}
//////////////////////////////////////////////////////////////////////////
CTableItemUI::CTableItemUI( bool bGroup /*= false*/ )
:INode(bGroup),
m_bEnableExpand(true)
{
	SetStyle(_T("TableItem"));
}


bool CTableItemUI::Activate()
{
	if (!CControlUI::Activate())
		return false;
	if (m_bEnableExpand)
	{
		Expand(!IsExpand());
	}

	return __super::Activate();
}

void CTableItemUI::node_show( bool bShow )
{
	SetVisible(bShow);
}

bool CTableItemUI::is_node_show()
{
	return IsVisible();
}

int CTableItemUI::get_node_index()
{
	return GetIndex();
}

void CTableItemUI::set_node_index( int nIndex )
{
	SetIndex(nIndex);
}

void CTableItemUI::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{
	if (equal_icmp(_T("group"), lpszName))
	{
		node_group((bool)!!_ttoi(lpszValue));
	}
	else if (equal_icmp(_T("expand"), lpszName))
	{
		m_bNodeExpand = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(_T("EnableExpand"), lpszName))
	{
		m_bEnableExpand = (bool)!!_ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

bool CTableItemUI::Add( CControlUI* pControl )
{
	if (IsGroup())
	{
		CTableUI* pTree = static_cast<CTableUI*>(GetParent());
		CTableItemUI* pNode = dynamic_cast<CTableItemUI*>(pControl);
		if (pTree && pNode)
		{
			return (pTree->InsertItem(pNode, this) != NULL);
		}
	}
	return __super::Add(pControl);
}

int CTableItemUI::num_child()
{
	return num_child_node();
}

CTableItemUI* CTableItemUI::get_nextsibling( CTableItemUI* pNode )
{
	return static_cast<CTableItemUI*>(get_nextsibling_node(pNode));
}

CTableItemUI* CTableItemUI::get_prevsibling( CTableItemUI* pNode )
{
	return static_cast<CTableItemUI*>(get_prevsibling_node(pNode));
}

bool CTableItemUI::Expand( bool bExpand, UINT uFlags /*= UINODE_EXPAND_TOROOT | UINODE_EXPAND_UPDATE*/ )
{
	return node_expand(bExpand, uFlags);
}

bool CTableItemUI::IsExpand( UINT uFlags /*= UINODE_EXPAND_TOROOT*/ )
{
	return is_node_expand(uFlags);
}

CTableItemUI* CTableItemUI::get_child( int i )
{
	return static_cast<CTableItemUI*>(get_child_node(i));
}

CTableItemUI* CTableItemUI::get_last_child()
{
	return static_cast<CTableItemUI*>(get_last_child_node());
}

CTableItemUI* CTableItemUI::get_parent()
{
	return static_cast<CTableItemUI*>(get_parent_node());
}

CTableItemUI* CTableItemUI::find_child( UINT uData )
{
	//CTableItemUI* pNode = NULL;
	int nCnt = num_child();
	for (int i = 0; i < nCnt; i++)
	{
		CTableItemUI* pNode = get_child(i);
		if (pNode && pNode->GetUserData() == uData)
			return pNode;
	}
	return NULL;
}

int CTableItemUI::find_child( CTableItemUI* node )
{
	int nCnt = num_child();
	for (int i = 0; i < nCnt; i++)
	{
		if (get_child(i) == node)
			return i;
	}
	return -1;
}

bool CTableItemUI::IsGroup()
{
	return is_node_group();
}

void CTableItemUI::set_child( CTableItemUI* node, int nIndex )
{
	set_child_node(node, nIndex);
}

void CTableItemUI::CheckLegal()
{
	CTableUI* pTable = static_cast<CTableUI*>(GetParent());
	CTableItemUI* pNode = NULL;
	//CTableItemUI* pSubNode = NULL;
	pNode = this;
	//int nCheck = 0;
	//int nVisible = 0;
	bool bCheckLegalParent = true;
	//CListItemCheckUI* pCheck = NULL;
	if(pNode && pNode->GetIndex() != -1)
	{
		int nCheck = 0;
		int nVisible = 0;
		int nChildCnt = pNode->num_child();
		for (int i = 0; i < nChildCnt; i++)
		{
			CTableItemUI* pSubNode = pNode->get_child(i);
			if (pSubNode)
			{
				if (pTable->IsCheckItem(pSubNode->GetIndex(), UIITEMFLAG_VISIBLE))
				{
					nVisible += 2;
					if (pSubNode->GetCheck() && pSubNode->GetCheck()->Indef())
						nCheck += 1;
					else
						nCheck += (((int)pTable->IsCheckItem(pSubNode->GetIndex()))*2);
				}
			}
		}
		CListItemCheckUI* pCheck = static_cast<CListItemCheckUI*>(pNode->GetCheck());
		if (pCheck)
			pCheck->Indef(false);
		if (nVisible > 0 && nCheck > 0 && nVisible != nCheck)
		{
			if (pCheck)
			{
				pCheck->Indef(true);

				if (pCheck->IsVisible() && pCheck->IsEnabled())
				{
					pCheck->SetChecked(false, UIITEMFLAG_CHECKED);
				}
			}
		}
		if (nVisible > 0)
		{
			if (nVisible == nCheck)
			{
				pTable->CheckItem(true, pNode->GetIndex(),UIITEMFLAG_CHECKED);
				bCheckLegalParent = false;
			}
			if (nCheck == 0)
			{
				pTable->CheckItem(false, pNode->GetIndex(),UIITEMFLAG_CHECKED);
				bCheckLegalParent = false;
			}
		}
	}
	if (bCheckLegalParent)
	{
		CTableItemUI* pParent = get_parent();
		if (pParent)
			pParent->CheckLegal();
	}
}

