/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************/

#include "stdafx.h"
#include "UITree.h"

#define _Style_Tree_Bk 0
#define _Style_Tree_Item 1
#define _Style_Tree_ItemBkg 2
#define _Style_TreeNode_Icon 0
#define _Style_TreeNode_Txt 1
#define _Style_CheckTreeNode_Txt 0
#define _Style_CheckTreeNode_Chk 1
#define _Style_CheckTreeNode_Group 2


//////////////////////////////////////////////////////////////////////////
class ICheckNodeUI
{
public:
	ICheckNodeUI(CTreeNodeUI* pOwner);
	~ICheckNodeUI();
	int  GetCheckOffset(){ return 17; }
	bool Active();
	void SetRect(RECT& rectRegion);
	bool HitTest(POINT point);
	void RenderCheck(IRenderDC* pRenderDC, RECT& rcPaint, int nIndex);
	void SetPartialChecked(bool bPartialChecked){ m_bPartialChecked = bPartialChecked; }
	bool IsPartialChecked(){return m_bPartialChecked;}
protected:
	bool m_bPartialChecked;
	RECT m_rcItem;
	int  m_nInsetIndex;
	ImageObj* m_pImageObj;
	CTreeNodeUI* m_pOwner;
};

//////////////////////////////////////////////////////////////////////////
ICheckNodeUI::ICheckNodeUI(CTreeNodeUI* pOwner)
{
	m_nInsetIndex = 0;
	m_pOwner = pOwner;
	m_bPartialChecked = false;
	m_pImageObj = GetUIRes()->LoadImage(_T("#checkbox"));
}

ICheckNodeUI::~ICheckNodeUI()
{
	if (m_pImageObj)
		m_pImageObj->Release();
	m_pImageObj = NULL;
}

void ICheckNodeUI::RenderCheck(IRenderDC* pRenderDC, RECT& rcPaint, int nIndex)
{
	if (m_pImageObj)
		pRenderDC->DrawImage(m_pImageObj, m_rcItem, nIndex);
}
#define CHECK_SIZE DPI_SCALE(15)
void ICheckNodeUI::SetRect(RECT& rectRegion)
{
	RECT rc = rectRegion;
	rc.right = rc.left + CHECK_SIZE;
	int nHeight = rc.bottom - rc.top;
	rc.top += (nHeight - CHECK_SIZE) / 2;
	rc.bottom = rc.top + CHECK_SIZE;
	m_rcItem = rc;
}

bool ICheckNodeUI::HitTest(POINT point)
{
	if (PtInRect(&m_rcItem, point))
	{
		if (IsPartialChecked())
		{
			m_pOwner->Check(((CTreeUI*)m_pOwner->GetParent())->IsCheckedAfterPartialChecked());
		}
		else
		{
			m_pOwner->Check(!m_pOwner->IsCheck());
		}
		//modify by hanzp. 当前点击不展开
		if(m_pOwner->GetParent())
		{
			m_pOwner->GetParent()->Resize();
			m_pOwner->GetParent()->Invalidate();
		}
		Active();
		return true;
	}
	return false;
}


bool ICheckNodeUI::Active()
{
	if (m_pOwner && m_pOwner->GetParent())
		m_pOwner->GetParent()->SendNotify(UINOTIFY_TVN_CHECK, NULL, (LPARAM)m_pOwner);
	return true;
}

//////////////////////////////////////////////////////////////////////////
class CDragTool;
CTreeNodeUI* g_pDragNode = NULL;
CDragTool* g_pDragTool = NULL;
POINT g_ptDrag;

class CDragTool: public CWindowUI
{
public:
	CDragTool(){  m_pDibObj = NULL;}
	~CDragTool(){ if ( m_pDibObj) delete  m_pDibObj; m_pDibObj=NULL;}
	void SetDibObj(DibObj* pDibObj){ m_pDibObj = pDibObj;}
	void HideCloseWindow()
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		::DestroyWindow(m_hWnd);
	}
protected:
	void OnCreate(){ModifyWndStyle(GWL_EXSTYLE, 0, WS_EX_LAYERED);};
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_PAINT:
			{
				PAINTSTRUCT ps ={0};
				::BeginPaint(m_hWnd, &ps);
				if (m_pDibObj && m_pDibObj->IsValid())
					BitBlt(ps.hdc, 0, 0, m_pDibObj->GetWidth(), m_pDibObj->GetHeight(), m_pDibObj->GetSafeHdc(), 0, 0, SRCCOPY);
				EndPaint(m_hWnd, &ps);
				return 1;
			}
			break;
		case WM_ACTIVATEAPP:
			{
				if (!wParam && g_pDragTool)
				{
					g_pDragTool->HideCloseWindow();
					delete g_pDragTool;
					g_pDragTool = NULL;
					g_pDragNode = NULL;
				}
				return 1;
			}
			break;
		}
		return CWindowUI::WindowProc(message, wParam, lParam);
	};
protected:
	DibObj* m_pDibObj;
};

/*!
    \brief    过滤窗口的wm_mousemove，判断列表的高亮节点
*****************************************/
class TreeMessageFilter : public ItemHighlightMessageFilter
{
public:
	TreeMessageFilter(CTreeUI* pTree):ItemHighlightMessageFilter(pTree)
	{
		m_pTree = pTree;
	}
protected:
	virtual CControlUI* ItemFromPos(POINT& pt)
	{
		return m_pTree->ItemFromPos(pt);
	}

	virtual LRESULT MessageFilter( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled ) 
	{
		if (m_pTree)
		{
			switch (uMsg)
			{
			case WM_MOUSEMOVE:
				{
					if (m_pTree->IsVisible())
					{
						POINT pt = {(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)};

						CWindowUI* pWindow = m_pTree->GetWindow();
						POINT ptScreen = pt;
						::ClientToScreen(pWindow->GetHWND(), &ptScreen);

						if (m_pTree->IsEnableDragNode() && g_pDragNode && wParam == MK_LBUTTON)
						{
							{
								if (!g_pDragTool)
								{
									RECT rect = g_pDragNode->GetRect();
									if (abs(pt.x - rect.left -  g_ptDrag.x) > 5 || abs(pt.y - rect.top - g_ptDrag.y) > 5)
									{
										g_pDragTool = new CDragTool();
										g_pDragTool->SetAutoDel(false);
										DibObj* pDibObj = new DibObj;
										pDibObj->Create(pWindow->GetPaintDC(), rect.right - rect.left, rect.bottom - rect.top);

										::BitBlt(pDibObj->GetSafeHdc(), 
											0, 
											0, 
											rect.right - rect.left,
											rect.bottom - rect.top,
											pWindow->GetMemDC(),
											rect.left,
											rect.top,
											SRCCOPY);
										g_pDragTool->SetDibObj(pDibObj);

										g_pDragTool->SetWndStyle(GWL_STYLE, WS_POPUP);
										g_pDragTool->SetWndStyle(GWL_EXSTYLE,   WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
										g_pDragTool->Create(pWindow->GetHWND(), _T(""), ptScreen.x, ptScreen.y, rect.right - rect.left, rect.bottom - rect.top);
										g_pDragTool->SetAlpha(130);
										g_pDragTool->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW /*| SWP_NOACTIVATE*/);
										g_pDragTool->Invalidate(NULL, TRUE);

										CTreeNodeUI* pNode = dynamic_cast<CTreeNodeUI*>(m_pTree->ItemFromPos(pt));
										if (pNode)
											m_pTree->SendNotify(UINOTIFY_TVN_BEGINDRAG, NULL, (LPARAM)pNode);
										m_pTree->Invalidate(true);
									}	
								}
								else
								{

									g_pDragTool->SetWindowPos(HWND_TOPMOST, ptScreen.x - g_ptDrag.x, ptScreen.y - g_ptDrag.y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW/*| SWP_NOACTIVATE*/);
									CScrollbarUI* pVScrollBar = m_pTree->GetVScrollbar();
									if (pVScrollBar && pVScrollBar->IsVisible())
									{
										static DWORD dwLastTicketcount = ::GetTickCount();
										DWORD dwCurrentTicketcount = ::GetTickCount();
										if (dwCurrentTicketcount - dwLastTicketcount > 30)
										{
											RECT rcItem = m_pTree->GetRect();
											if (pt.y < rcItem.top)
												pVScrollBar->LineUp();
											else if (pt.y > rcItem.bottom)
												pVScrollBar->LineDown();
											dwLastTicketcount = dwCurrentTicketcount;
										}	
									}
									CControlUI* pControl = m_pTree->ItemFromPos(pt);
									if (pControl == NULL)
									{
										if (WindowFromPoint(pt) != m_pTree->GetWindow()->GetHWND())
											::SetCursor(::LoadCursor(NULL, IDC_ARROW));
										else
											::SetCursor(::LoadCursor(NULL, IDC_NO));
									}
									else
									{
										::SetCursor(GetCursor());
									}
								}
							}
						}
					}
				}
				break;
			}
		}
		return __super::MessageFilter(uMsg, wParam, lParam, bHandled);
	}

private:
	CTreeUI* m_pTree;
};
//////////////////////////////////////////////////////////////////////////
CTreeUI::CTreeUI()
:m_bLockChildsRect(true)
,m_nFirstVisible(0)
,m_nLastVisible(0)
,m_bDragNode(false)
,m_bEnableItemOver(true)
,m_bEnableItemBkg(false)
,m_bEnableItemSelect(true)
,m_bLimitGroupClick(true)
,m_nMaginToParent(10)
,m_pHighlight(NULL)
,m_bCheckedAfterPartialChecked(true)
{
	SetStyle(_T("Tree"));

	EnableScrollBar(true, false);
	RECT rcInset = {2, 0, 2, 0};
	SetInset(rcInset);
	SetMouseEnabled(true);
	SetGestureEnabled(true);

	CTreeNodeUI* pRoot = new CTreeNodeUI;
	pRoot->Expand(true, 0);
	pRoot->node_group(true);
	pRoot->SetId(_T("root"));
	pRoot->SetIndex(-1);
	pRoot->set_node_owner(this);
	pRoot->SetManager(NULL, this);
	set_root(pRoot);

	ModifyFlags(UICONTROLFLAG_SETFOCUS, 0);

}

CTreeUI::~CTreeUI()
{
	
	ItemHighlightMessageFilter::RemoveFromMessageFilterList(this);

	CTreeNodeUI* pRoot = GetRoot();
	if (pRoot)
		delete pRoot;
	set_root(NULL);
}

void CTreeUI::RemoveItem(CTreeNodeUI* pControl)
{
	Remove(pControl);
}

bool CTreeUI::Remove(CControlUI* pControl)
{
	if (!pControl)
		return true;
	CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(pControl);
	int nStart = pNode->GetIndex();
	int nEnd = pNode->get_last_child()->GetIndex();

	//已经在INode析构函数中执行删除
// 	CTreeNodeUI* pParent = pNode->get_parent();
// 	if (pParent)
// 		pParent->remove_child(pNode);

	int nDelItems = nEnd - nStart + 1;
	if (GetCurSel() == pControl)
		SelectControl(NULL);
	//CControlUI* pSubControl = NULL;
	while (nDelItems > 0)
	{ 
		CControlUI* pSubControl = GetItem(nStart);
		if (GetCurSel() == pSubControl)
			SelectControl(NULL);
		if (pSubControl)
			__super::Remove(pSubControl);
		nDelItems--;
	}
	return true;
}

bool CTreeUI::Add(CControlUI* pControl)
{
	RT_CLASS_ASSERT(pControl, CTreeNodeUI, _T("CTreeUI::Add"));
	CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(pControl);
	if (pNode)
		InsertItem(pNode);
	return true;
}

CTreeNodeUI* CTreeUI::InsertItem(CTreeNodeUI* pControl, CTreeNodeUI* pParent/* = UITVI_ROOT*/, CTreeNodeUI* pInsertAfter/* = UITVI_LAST*/)
{
	CTreeNodeUI* pRoot = GetRoot();
	if (pParent == UITVI_ROOT || !pParent)
		pParent = pRoot;

	// 不是组节点不能插入子节点
	if (!pParent->IsGroup())
	{
		if (!pControl->get_parent())
			delete pControl;
		return NULL;
	}
	int nChildIndex = 0;//pParent下的节点逻辑位置
	if (pInsertAfter == UITVI_FIRST)
	{
		if (pParent == pRoot)
			pInsertAfter = NULL;
		else
			pInsertAfter = pParent;
		nChildIndex = 0;
	}
	else if (pInsertAfter == UITVI_LAST ||  !pInsertAfter)
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
	
	if (pParent != GetRoot())
		SetItemInset(pControl, pParent);
	
	if (!pParent->IsExpand())
		SetItemVisible(pControl, false);

	// 移动节点
	if (pControl->get_parent())
	{
		//判断是否已经在指定位置
		CControlUI* pPrev = GetItem(pControl->GetIndex() - 1);
		if (pPrev)
		{
			CTreeNodeUI* pPrevNode = static_cast<CTreeNodeUI*>(pPrev);
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
		CTreeNodeUI* pLastChild = pControl->get_last_child();
		int nEnd = pLastChild->GetIndex();
		pControl->get_parent()->remove_child(pControl);

		int nAfterIndex = 0;
		if (pInsertAfter) 
			nAfterIndex = pInsertAfter->GetIndex();
		if (nStart > nAfterIndex && pInsertAfter)
			nAfterIndex = nAfterIndex + 1;

		CStdPtrArray* pArrItems = GetItemArray();
		if (pArrItems)
			pArrItems->Move(nStart, nEnd, nAfterIndex);

		int nBeginIndex = min(nStart, nAfterIndex);
		int nEndIndex = max(nEnd, nAfterIndex);
		if (nBeginIndex < 0)
			nBeginIndex = 0;
		if (nEndIndex >= GetCount())
			nEndIndex = GetCount() - 1;
		for (int i = nBeginIndex; i <= nEndIndex; i++)
			GetItem(i)->SetIndex(i);

		pParent->add_child(pControl, nChildIndex);
		NeedUpdate();
	}
	else //插入节点
	{
		if( !__super::Insert(pControl, pInsertAfter) )
		{
			delete pControl;
			return NULL;
		}

		pParent->add_child(pControl, nChildIndex);
		//modify by hanzp. 效率考虑
		//Resize();  // 徐情波临时添加函数
	}
	return pControl;
}

CTreeNodeUI* CTreeUI::InsertItemByCompare(CTreeNodeUI* pControl, CTreeNodeUI* pParent, bool(* pCompare)(CTreeNodeUI* ,CTreeNodeUI* ))
{
	if (pParent == UITVI_ROOT || !pParent)
		pParent = GetRoot();
	CTreeNodeUI* pAfter = UITVI_FIRST;
	int nCnt = pParent->num_child();
	for (int i = nCnt - 1; i >= 0; i--)
	{
		if (pCompare(pControl, pParent->get_child(i)))
		{
			pAfter = pParent->get_child(i);
			break;
		}
	}
	return InsertItem(pControl, pParent, pAfter);
}

bool CTreeUI::ChildEvent(TEventUI& event)
{
	switch(event.nType)
	{
	case UIEVENT_LDBCLICK:
		{
			SendNotify(UINOTIFY_DBCLICK, NULL, (LPARAM)dynamic_cast<CTreeNodeUI*>(event.pSender));
		}
		break;
	case UIEVENT_RBUTTONDOWN:
		{
			if (m_bDragNode)
			{
				if (g_pDragNode || g_pDragTool)
				{
					if (g_pDragTool)
					{
						g_pDragTool->HideCloseWindow();
						delete g_pDragTool;
					}
					g_pDragTool = NULL;
					g_pDragNode = NULL;
					Resize();
					Invalidate();
					return true;
				}
			}
		}
		break;
	case UIEVENT_RBUTTONUP:
		{
			if (m_bDragNode)
			{
				if (g_pDragNode || g_pDragTool)
				{
					if (g_pDragTool)
					{
						g_pDragTool->HideCloseWindow();
						delete g_pDragTool;
					}
					g_pDragTool = NULL;
					g_pDragNode = NULL;
					Resize();
					Invalidate();
					break;
				}
			}
			if (event.pSender)
			{
				CTreeNodeUI* pNode = dynamic_cast<CTreeNodeUI*>(event.pSender);
				if (pNode)
				{
					SelectItem(pNode);
					if (pNode->IsGroup())
					{
						Resize();
						Invalidate();
					}
					else
					{
						pNode->Activate();
					}
					EnsureVisible(pNode);
					UpdateWindow(GetWindow()->GetHWND());
					SendNotify(UINOTIFY_RCLICK, NULL, (LPARAM)pNode);
				}
			}

		}
		break;
	case UIEVENT_BUTTONDOWN:
		{
			if (m_bDragNode)
			{
				if (g_pDragTool)
				{
					g_pDragTool->HideCloseWindow();
					delete g_pDragTool;
					g_pDragTool = NULL;
				}
				g_pDragNode = NULL;

				if (event.pSender)
				{
					g_pDragNode = dynamic_cast<CTreeNodeUI*>(event.pSender);
					if (g_pDragNode)
					{
						if (!g_pDragNode->m_bCanDrag)
							g_pDragNode = NULL;
						else
						{
							RECT rect = g_pDragNode->GetRect();
							g_ptDrag.x = event.ptMouse.x - rect.left;
							g_ptDrag.y = event.ptMouse.y - rect.top;

						}
					}
				}
			}
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			if (m_bDragNode && g_pDragNode && g_pDragTool)
			{
				g_pDragTool->HideCloseWindow();
				delete g_pDragTool;
				g_pDragTool = NULL;
				g_pDragNode->SetState(UISTATE_NORMAL);
				SendNotify(UINOTIFY_TVN_ENDDRAG, WPARAM(ItemFromPos(event.ptMouse)), (LPARAM)g_pDragNode);
			}
			else if (event.pSender)
			{
				CTreeNodeUI* pNode = dynamic_cast<CTreeNodeUI*>(event.pSender);
				if (pNode)
				{
					SelectItem(pNode);
					if (m_bLimitGroupClick && pNode->IsGroup())
					{
						StyleObj* pStyleObj = pNode->GetStyle(_Style_TreeNode_Icon);
						if (pStyleObj && pNode->HitTest(event.ptMouse)/*pStyleObj->HitTest(event.ptMouse)*/)
						{
							pNode->Activate();
						}
					}
					else 
					{
						pNode->Activate();
					}
					EnsureVisible(pNode);
					SendNotify(UINOTIFY_CLICK, NULL, (LPARAM)pNode);	
				}
			}
			g_pDragNode = NULL;
		}
		break;
	case UIEVENT_KEYDOWN:
	case UIEVENT_SCROLLWHEEL:
		return Event(event);
	}
	return true;
}

bool CTreeUI::Event(TEventUI& event)
{
	switch(event.nType)
	{
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

	case UIEVENT_RBUTTONUP:
		{
			SendNotify(UINOTIFY_RCLICK, NULL, LPARAM(event.pSender != this?event.pSender:NULL));
		}
		break;
	case UIEVENT_KEYDOWN:
		{
			switch(event.wParam)
			{
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
			case VK_RIGHT:
				{
					CControlUI* pSel = GetCurSel();
					CTreeNodeUI* pItem = dynamic_cast<CTreeNodeUI*>(pSel);
					if (pItem)
					{
						if (pItem->IsGroup())
						{
							if (!pItem->IsExpand())
							{
								pItem->Expand(true);
								break;
							}
						}
					}
				}
				//break;
			case VK_DOWN:
				{
					CControlUI* pNext = GetNextItem(GetCurSel());
					if (pNext)
					{
						CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(pNext);
						SelectItem(pNode);
						if (!pNode->IsGroup())
						{
							pNode->Activate();
						}
						EnsureVisible(pNode);
					}
				}
				break;
			case VK_LEFT:
				{
					CControlUI* pSel = GetCurSel();
					CTreeNodeUI* pItem = dynamic_cast<CTreeNodeUI*>(pSel);
					if (pItem)
					{
						if (pItem->IsGroup())
						{
							if (pItem->IsExpand())
							{
								pItem->Expand(false);
								break;
							}
						}
					}
				}
				//break;
			case VK_UP:
				{
					CControlUI* pPrev = GetPrevItem(GetCurSel());
					if (pPrev)
					{
						CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(pPrev);
						SelectItem(pNode);
						if (!pNode->IsGroup())
							pNode->Activate();
						EnsureVisible(pNode);
					}
				}
				break;
			case VK_RETURN:
				{
					CTreeNodeUI* pNode = GetCurSel();
					if (pNode)
					{
						pNode->Activate();
						SendNotify(UINOTIFY_DBCLICK, NULL, (LPARAM)pNode);	
					}
				}
				break;
			}
			SendNotify(UINOTIFY_TVN_KEYDOWN, event.wParam, event.lParam);
		}
		break;
	default:
		return __super::Event(event);
	}
	return true;
}

CTreeNodeUI* CTreeUI::GetRoot()
{
	return static_cast<CTreeNodeUI*>(get_root());
}

CControlUI*  CTreeUI::ItemFromPos(POINT pt)
{
	//CControlUI* pControl = NULL;
	for (int i = m_nFirstVisible; i < m_nLastVisible; i++)
	{
		CControlUI* pControl = GetItem(i);
		if (pControl && pControl->IsVisible() && pControl->HitTest(pt))
			return pControl;
	}
	return NULL;
}

ImageObj* CTreeUI::GetStateImage()
{
	ImageStyle* pStyle = dynamic_cast<ImageStyle*>(GetStyle(_Style_Tree_Item));
	if (pStyle)
	{
		pStyle->LoadImage();
		return pStyle->GetImageObj();
	}
	return NULL;
}

ImageObj* CTreeUI::GetItemBkgImage()
{
	if (!IsEnableItemBkg())
		return NULL;
	ImageStyle* pStyle = dynamic_cast<ImageStyle*>(GetStyle(_Style_Tree_ItemBkg));
	if (pStyle)
	{
		pStyle->LoadImage();
		return pStyle->GetImageObj();
	}
	return NULL;
}

CControlUI* CTreeUI::GetNextItem(CControlUI* pControl, UINT uFlags /*= UIITEMFLAG_VISIBLE | UIITEMFLAG_ENABLED*/)
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

CControlUI* CTreeUI::GetPrevItem(CControlUI* pControl, UINT uFlags /*= UIITEMFLAG_VISIBLE | UIITEMFLAG_ENABLED*/)
{
	if (!pControl)
	{
		if (GetCount())
			return GetItem(GetCount() - 1);
		else
			return NULL;
	}

	//CControlUI* pNextItem = NULL;
//	int nSum = GetCount();
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

void CTreeUI::EnsureVisible(CControlUI* pControl)
{
	if (pControl)
		((static_cast<CTreeNodeUI*>(pControl))->get_parent())->Expand(true);
	CScrollbarUI* pVerticalScrollbar = GetVScrollbar();
	if (pVerticalScrollbar && pVerticalScrollbar->IsVisible() && pControl)
	{
		m_bLockChildsRect = false;
		Resize();
		RECT rcItem = pControl->GetRect();
		RECT rcClient = GetClientRect();
		if( rcItem.bottom > rcClient.bottom)
			pVerticalScrollbar->SetScrollPos(pVerticalScrollbar->GetScrollPos() + rcItem.bottom - rcClient.bottom);
		else if( rcItem.top < rcClient.top)
			pVerticalScrollbar->SetScrollPos(pVerticalScrollbar->GetScrollPos() + rcItem.top - rcClient.top);
		m_bLockChildsRect = true;
	}
}

void CTreeUI::EnsureVisibleFirst( CControlUI* pControl )
{
	if (pControl)
	{
		RT_CLASS_ASSERT(pControl, CTreeNodeUI, _T("CTreeUI::EnsureVisibleFirst"));
		CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(pControl);
		if (pNode->get_parent())
			(pNode->get_parent())->Expand(true);
	}
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

int CTreeUI::EnsureVisibleCenter( CControlUI* pControl,int& nBegin, int &nEnd, bool bRedraw /*= true*/ )
{
	if (pControl)
		((static_cast<CTreeNodeUI*>(pControl))->get_parent())->Expand(true);

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
		pVerticalScrollbar->SetScrollPos(nLastScrollPos, false);
		if (bRedraw)
		{
			pVerticalScrollbar->SetScrollPos(nLastScrollPos + nOffset);
		}

		nBegin = nLastScrollPos;
		nEnd = nNewScrollPos;
		m_bLockChildsRect = true;
	}
	return nNewScrollPos - nLastScrollPos;
}

void CTreeUI::SetItemInset(CTreeNodeUI* pControl, CTreeNodeUI* pParent)
{
	RECT rect = pControl->GetInset();
	/*if (GetUIEngine()->IsRTLLayout())
	{
		rect.right = pParent->GetInset().right + m_nMaginToParent;
	}
	else*/
	{
		rect.left = pParent->GetInset().left + m_nMaginToParent;
	}

// 	if (!pControl->IsGroup())
// 		rect.left = pParent->GetInset().left;
// 	else
	/*	rect.left = pParent->GetInset().left + m_nMaginToParent;*/
	pControl->SetInset(rect);
	int nCnt = pControl->num_child();
	for (int i = 0; i < nCnt; i++)
		SetItemInset(pControl->get_child(i), pControl);
}

void CTreeUI::SetItemVisible(CTreeNodeUI* pControl, bool bVisible)
{
	GetWindow()->LockUpdate();
	pControl->SetVisible(bVisible);
	GetWindow()->UnLockUpdate();
	int nCnt = pControl->num_child();
	for (int i = 0; i < nCnt; i++)
		SetItemVisible(pControl->get_child(i), bVisible);
}

void CTreeUI::SetMaginToParent(int nMagin)
{
	m_nMaginToParent = nMagin;
}

void CTreeUI::RemoveAll()
{
	m_nFirstVisible = 0;
	m_nLastVisible = 0;
	__super::RemoveAll();
	if (GetRoot())
		GetRoot()->remove_all();
}

CTreeNodeUI* CTreeUI::GetCurSel()
{
	return static_cast<CTreeNodeUI*>(GetCurSelControl());
}

bool CTreeUI::SelectItem(CControlUI* pControl)
{
	if (SelectControl(pControl))
		SendNotify(UINOTIFY_SELCHANGE, NULL, (LPARAM)GetCurSel());
	Invalidate();
	return true;
}

void CTreeUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(_T("Drag"), lpszName))
	{
		m_bDragNode = !!_ttoi(lpszValue);
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
	else if (equal_icmp(lpszName, _T("LimitClick")))
	{
		m_bLimitGroupClick = !!_ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}
//////////////////////////////////////////////////////////////////////////
bool(* gs_pTreeSortCompareProc)(CTreeNodeUI* ,CTreeNodeUI* ) = NULL;

bool Tree_CompareFunc(LPVOID ptrA, LPVOID ptrB)
{
	if (gs_pTreeSortCompareProc)
	{
		CTreeNodeUI* pItemA = static_cast<CTreeNodeUI*>(ptrA);
		CTreeNodeUI* pItemB = static_cast<CTreeNodeUI*>(ptrB);
		if (pItemA->get_parent() == pItemB->get_parent())
		{
			CTreeNodeUI* pParent = pItemA->get_parent();
			bool bRet = !gs_pTreeSortCompareProc(pItemA, pItemB);
			if (bRet)
			{
				if (!gs_pTreeSortCompareProc(pItemB, pItemA))
					return false;
			}
			return bRet;
		}
	}
	return false;
}

void SortChildNode(bool(* pCompareProc)(CTreeNodeUI* ,CTreeNodeUI* ), CTreeNodeUI* pParent)
{
	if (!pParent || !pCompareProc)
		return;
	int nChildCnt = pParent->num_child();
	if (nChildCnt > 0)
	{
		CTreeNodeUI* pNode = NULL;
		if (nChildCnt > 1)
		{
			//对组节点pParent的直属子节点进行排序
			CStdPtrArray items;
			for (int i = 0; i < nChildCnt; i++)
			{
				pNode = pParent->get_child(i);
				if (pNode)
					items.Add(pNode);
			}
			gs_pTreeSortCompareProc = pCompareProc;
			items.Sort(Tree_CompareFunc);
			gs_pTreeSortCompareProc = NULL;

			//排序后对直属子节点的索引重新设置
			for (int i = 0; i < nChildCnt; i++)
			{
				pNode = static_cast<CTreeNodeUI*>(items.GetAt(i));
				if (pNode)
				{
					pParent->set_child(pNode, i);
				}
			}
		}

		for (int i = 0; i < nChildCnt; i++)
		{
			pNode = pParent->get_child(i);
			if (pNode && pNode->IsGroup())
			{
				SortChildNode(pCompareProc, pNode);
			}
		}
	}
}

//************************************
// @brief:    设置pParent下所有节点的容器索引
// @note:     
// @param: 	  CTreeNodeUI * pParent 
// @param: 	  int & nIndex 
// @param: 	  CStdPtrArray & items 
// @param: 	  int nBegin 
// @param: 	  int nSize 
// @return:   void 
//************************************
void SetTreeNodeIndex(CTreeNodeUI* pParent, int& nIndex, CStdPtrArray& items, int nBegin = 0, int nSize = -1)
{
	int nCnt = pParent->num_child();
	if (nSize != -1)
	{
		nCnt = nBegin + nSize;
	}
	//CTreeNodeUI* pItem = NULL;
	for (int i = nBegin; i < nCnt; i++)
	{
		CTreeNodeUI* pItem = pParent->get_child(i);
		if (pItem)
		{
			pItem->set_node_index(nIndex);
			items.SetAt(nIndex, static_cast<CControlUI*>(pItem));
			++nIndex;
			if (pItem->IsGroup())
			{
				SetTreeNodeIndex(pItem, nIndex, items);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CTreeUI::Sort(bool(* pCompare)(CTreeNodeUI* ,CTreeNodeUI* ), CTreeNodeUI* node)
{
	if (UITVI_ROOT == node)
		node = GetRoot();

	CTreeNodeUI* pBegin = node->get_child(0);
	CTreeNodeUI* pEnd	= node->get_last_child();
	if (pBegin && pEnd && pBegin != pEnd)
	{
		int nBegin = pBegin->GetIndex();
//		int nEnd   = pEnd->GetIndex();
		SortChildNode(pCompare, node);
		int nAllIndex = nBegin;
		CStdPtrArray* pArr = GetItemArray();
		SetTreeNodeIndex(node, nAllIndex, *pArr);
	}
	this->Resize();
	this->Invalidate();
}

SIZE CTreeUI::OnChildSize(RECT& rcWinow, RECT& rcClient)
{
	int nPos = 0;
	CScrollbarUI* pScrollbar = GetVScrollbar();
	if (pScrollbar && pScrollbar->IsVisible())
		nPos = pScrollbar->GetScrollPos();
	RECT rect = rcClient;
	rect.bottom = rcClient.top;
	nPos += rcClient.top;
	int l, t, r, b;
	int nMaxWidth = rcClient.right - rcClient.left;
	int nCnt = GetCount();
	CControlUI* pControl = NULL;
	if (IsEnableHScrollbar())//存在横向滚动条
	{
		RECT rectCalc = {0};
		CTreeNodeUI* pNode = NULL;
		for (int i = 0; i < nCnt; i++)
		{
			pControl = GetItem(i);
			if (pControl->IsVisible())
			{
				pNode = static_cast<CTreeNodeUI*>(pControl);
				pNode->CalcControl(rectCalc);
				nMaxWidth = max(rectCalc.right - rectCalc.left, nMaxWidth);
			}
		}
		rect.right = rect.left + nMaxWidth;
	}
	RECT rcChild = rect;
	bool bFirstFlag = true;
	//获取大小
	m_nFirstVisible = 0;
	RECT rcEmpty = {0};
	for (int i = 0; i < nCnt; i++)
	{
		pControl = GetItem(i);
		if (pControl)
		{
			if (pControl->IsVisible())
			{
				pControl->GetCoordinate(l, t, r, b);
				rect.top = rect.bottom;
				rect.bottom = rect.top + DPI_SCALE(b - t);
				if (!m_bLockChildsRect)
					*(pControl->GetRectPtr()) = rect;//pControl->SetRect(rect, false);// xqb去掉SetRect重载
				if (bFirstFlag && rect.bottom > nPos)
				{
					rcChild.bottom = rect.top;
					rcChild.top = rcChild.bottom;
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
	m_nLastVisible = nCnt;
	//计算可视界面的控件
	for (int i = m_nFirstVisible; i < nCnt; i++)
	{
		pControl = GetItem(i);
		if (pControl->IsVisible())
		{
			pControl->GetCoordinate(l, t, r, b);
			rcChild.top = rcChild.bottom;
			rcChild.bottom = rcChild.top + DPI_SCALE(b - t);
			pControl->SetRect(rcChild);

			if (rcChild.top > rcWinow.bottom)
			{
				m_nLastVisible = i;
				break;
			}
		}
	}
	SIZE sz = {rect.right - rect.left, rect.bottom - rcClient.top};
	return sz;
}

void CTreeUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	int nCnt = GetCount();
	if (m_nFirstVisible < 0 || m_nFirstVisible >= nCnt)
		m_nFirstVisible = 0;
	if (m_nLastVisible < 0 || m_nLastVisible >= nCnt)
		m_nLastVisible = nCnt;
	if (m_nFirstVisible > m_nLastVisible)
		m_nFirstVisible = m_nLastVisible;
	HDC hDestDC = pRenderDC->GetDC();
	if (!this->IsVisible())
		return;

	RECT rcTemp = { 0 };
	RECT rcItem = GetRect();
	RECT rcClient = GetClientRect();
	CControlUI* pParent = GetParent();
	if (pParent)
	{
		::IntersectRect(&rcItem, &pParent->GetRect(), &rcItem);
		::IntersectRect(&rcClient, &rcItem, &rcClient);
	}

	if( !::IntersectRect(&rcTemp, &rcPaint, &rcItem)) 
		return;
	//画父控件
	ClipObj clip;
	ClipObj::GenerateClip(hDestDC, rcTemp, clip);
	CControlUI::Render(pRenderDC, rcPaint);


	ImageObj* pImgState = GetItemBkgImage();
	ImageObj* pItemStateImage = GetStateImage();

	RECT rcChild;
	if( ::IntersectRect(&rcTemp, &rcPaint, &rcClient)) 
	{
		CControlUI* pControl = NULL;
		ClipObj childClip;
		/*if (DrawLimit())*/
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

			if (pImgState)
			{
				/*if (DrawLimit())*/
					RenderItemBkg(pImgState, pControl, pRenderDC, rcTemp);
				/*else
					RenderItemBkg(pImgState, pControl, pRenderDC, rcPaint);*/
			}
			if (pItemStateImage)
				RenderItemState(pItemStateImage, pControl, pRenderDC, rcPaint);
		}

		UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, *GetItemArray(), m_nFirstVisible, m_nLastVisible);
	}
	UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, GetVScrollbar());
	UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcClient, GetHScrollbar());
}

void CTreeUI::RenderItemBkg(ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint)
{
	if (pImgState && pControl)
	{
		RECT rcChild = pControl->GetRect();
		PaintParams params;
		params.nAlpha = GetAlpha();
		pRenderDC->DrawImage(pImgState,  rcChild, pControl->GetIndex()%2, &params);
	}
}

bool CTreeUI::IsEnableItemBkg()
{
	return m_bEnableItemBkg;
}

void CTreeUI::expanding( INode* pNode, bool bExpand )
{
	SendNotify(UINOTIFY_TVN_ITEMEXPENDING, WPARAM(bExpand), LPARAM(static_cast<CTreeNodeUI*>(pNode)));
}

void CTreeUI::expandend( INode* pNode, bool bExpand )
{
	SendNotify(UINOTIFY_TVN_ITEMEXPENDED, WPARAM(bExpand), LPARAM(static_cast<CTreeNodeUI*>(pNode)));
}

void CTreeUI::NeedUpdate()
{
	Resize();
	Invalidate();
}

INode* CTreeUI::get_node( int nIndex )
{
	return static_cast<CTreeNodeUI*>(GetItem(nIndex));
}

void CTreeUI::set_node( int nIndex, INode* pNode )
{
	GetItemArray()->SetAt(nIndex, static_cast<CTreeNodeUI*>(pNode));
}

void CTreeUI::RenderItemState( ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint )
{
	if (pControl)
	{
		CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(pControl);
		if (pNode->EnableItemState() && (GetHotItem() == pControl || pNode->IsSelected()))
		{
			RECT rc = pNode->GetRect();
			if (pNode->IsSelected())
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

CControlUI* CTreeUI::GetHotItem()
{
	return m_pHighlight;
}

bool CTreeUI::IsEnableDragNode()
{
	return m_bDragNode;
}

bool CTreeUI::IsEnableSelect()
{
	return m_bEnableItemSelect;
}

bool CTreeUI::IsEnableItemOver()
{
	return m_bEnableItemOver;
}

CControlUI* CTreeUI::FindControl( FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags )
{
	if (!FindControlTest(this, pData, uFlags))
		return NULL;

	CControlUI* pControl = NULL;
	if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
		pControl = __super::FindControl(Proc, pData, uFlags);
		if(pControl)
			return pControl;
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
}

void CTreeUI::Init()
{
	ItemHighlightMessageFilter::AddToMessageFilterList(this, new TreeMessageFilter(this));
	__super::Init();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeUI::ms_bTreeNodeExpand = false;
//////////////////////////////////////////////////////////////////////////
CTreeNodeUI::CTreeNodeUI(bool bGroup)
:INode(bGroup)
,m_bCanDrag(!bGroup)
,m_bCheck(false)
,m_bCalc(false)
,m_pMyData(NULL)
,m_bShake(false)
,m_bEnableExpand(true)
,m_pCheckNode(NULL)
,m_bCheckVisible(false)
,m_bEnableItemState(true)
{
	SetMouseEnabled(true);
	SetCoordinate(0, 0, 0, 24, UIANCHOR_LEFTTOP);
	SetState(UISTATE_NORMAL);
	Select(false);
	SetStyle(_T("TreeNode"));
	SetMouseEnabled(true);
	ModifyFlags(UICONTROLFLAG_SETFOCUS, 0);
}

CTreeNodeUI::~CTreeNodeUI()
{
	if (g_pDragNode == this)
		g_pDragNode = NULL;
	if (m_pMyData)
		delete m_pMyData;
	m_pMyData = NULL;
	if (m_pCheckNode)
		delete m_pCheckNode;
	m_pCheckNode = NULL;
}

void CTreeNodeUI::SetMyData(void *pData)
{
	if (m_pMyData != NULL)
		delete m_pMyData;
	m_pMyData = pData;
}

void* CTreeNodeUI::GetMyData()
{
	return m_pMyData;
}

int CTreeNodeUI::num_child()
{
	return num_child_node();
}

CTreeNodeUI* CTreeNodeUI::get_child(int i)
{
	return static_cast<CTreeNodeUI*>(get_child_node(i));
}

int CTreeNodeUI::find_child(CTreeNodeUI* node)
{
	int nCnt = num_child();
	for (int i = 0; i < nCnt; i++)
	{
		if (get_child(i) == node)
			return i;
	}
	return -1;
}

CTreeNodeUI* CTreeNodeUI::find_child(UINT uData)
{
	//CTreeNodeUI* pNode = NULL;
	int nCnt = num_child();
	for (int i = 0; i < nCnt; i++)
	{
		CTreeNodeUI* pNode = get_child(i);
		if (pNode && pNode->GetUserData() == uData)
			return pNode;
	}
	return NULL;
}

CTreeNodeUI* CTreeNodeUI::get_parent()
{
	return static_cast<CTreeNodeUI*>(get_parent_node());
}

void CTreeNodeUI::add_child(CTreeNodeUI* child, int nIndex) 
{
	add_child_node(child, nIndex);
}

void CTreeNodeUI::remove_child(CTreeNodeUI* child)
{
	remove_child_node(child);
}

void  CTreeNodeUI::remove_all()
{
	remove_all_node();
}

CTreeNodeUI* CTreeNodeUI::get_last_child()
{
	return static_cast<CTreeNodeUI*>(get_last_child_node());
}

bool CTreeNodeUI::Expand( bool bExpand, UINT uFlags /*= UINODE_EXPAND_TOROOT | UINODE_UPDATE*/ )
{
	return node_expand(bExpand, uFlags);
}

bool CTreeNodeUI::IsExpand(UINT uFlags /*= UINODE_EXPAND_TOROOT*/)
{
	return is_node_expand(uFlags);
}

bool CTreeNodeUI::IsGroup()
{
	return is_node_group();
}

void CTreeNodeUI::Check( bool bCheck, UINT uFlags /*= UIITEMFLAG_CHECKED | UIITEMFLAG_ALL*/)
{
	if (uFlags & UIITEMFLAG_CHECKED)
	{
		m_bCheck = bCheck;
		if (m_pCheckNode)
			m_pCheckNode->SetPartialChecked(false);
		if (IsGroup() && (uFlags & UIITEMFLAG_ALL))
		{
			//CTreeNodeUI* pNode = NULL;
			int nCnt = num_child();
			for (int i = 0; i < nCnt; i++)
			{
				CTreeNodeUI* pNode = get_child(i);
				if (pNode)
					pNode->Check(bCheck, uFlags & ~UIITEMFLAG_NOTIFY);
			}
		}
		CTreeNodeUI* pParent = get_parent();
		if (pParent)
		{
			pParent->CheckLegal();
		}
		if (m_pCheckNode && (uFlags & UIITEMFLAG_NOTIFY))
		{
			m_pCheckNode->Active();
		}
	}
	else if (uFlags & UIITEMFLAG_VISIBLE)
	{
		m_bCheckVisible = bCheck;

		if (m_pCheckNode)
			delete m_pCheckNode;
		m_pCheckNode = NULL;

		if (bCheck)
			m_pCheckNode = new ICheckNodeUI(this);

		if (IsGroup() && (uFlags & UIITEMFLAG_ALL) == UIITEMFLAG_ALL)
		{
			//CTreeNodeUI* pNode = NULL;
			int nCnt = num_child();
			for (int i = 0; i < nCnt; i++)
			{
				CTreeNodeUI* pNode = get_child(i);
				if (pNode)
					pNode->Check(bCheck, uFlags & ~UIITEMFLAG_NOTIFY);
			}
		}
	}
}

bool CTreeNodeUI::IsCheck(UINT uFlags /*= UIITEMFLAG_CHECKED*/)
{
	bool bRet = true;
	if (uFlags & UIITEMFLAG_CHECKED)
	{
		bRet &= m_bCheck;
	}
	if (uFlags & UIITEMFLAG_VISIBLE)
	{
		bRet &= m_bCheckVisible;
	}
	return bRet;
}

void CTreeNodeUI::SetCheckRect(RECT &rc)
{
	if (m_pCheckNode && IsCheck(UIITEMFLAG_VISIBLE))
		m_pCheckNode->SetRect(rc);
}

int  CTreeNodeUI::GetCheckOffset()
{
	if (m_pCheckNode && IsCheck(UIITEMFLAG_VISIBLE))
		return m_pCheckNode->GetCheckOffset();
	return 0;
}

void CTreeNodeUI::RenderCheck(IRenderDC* pRenderDC, RECT& rcPaint)
{
	if (m_pCheckNode && IsCheck(UIITEMFLAG_VISIBLE))
	{
		int nIndex = 0;
		UITYPE_STATE nState = GetState();
		if (m_pCheckNode->IsPartialChecked())
		{
			if (nState != UISTATE_NORMAL)
			{
				nIndex = 5;
			}
			else
			{
				nIndex = 4;
			}
		}
		else
		{
			nIndex = (GetState() != UISTATE_NORMAL)?(IsCheck()?3:1):(IsCheck()?2:0);
		}
		m_pCheckNode->RenderCheck(pRenderDC, rcPaint, nIndex);
	}
}

void CTreeNodeUI::EnableDrag(bool bEnable)
{
	m_bCanDrag = bEnable;
}

ImageObj* CTreeNodeUI::GetImage()
{
	StyleObj* pStyleObj = GetStyle(_Style_TreeNode_Icon);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T(" Style TreeNode Icon"));
	ImageStyle* pStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pStyle)
	{
		pStyle->LoadImage();
		return pStyle->GetImageObj();
	}
	return NULL;
}

void CTreeNodeUI::SetImage(ImageObj* pImageObj)
{
	if (pImageObj == NULL) return;
	StyleObj* pStyleObj = GetStyle(_Style_TreeNode_Icon);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T(" Style TreeNode Icon"));
	ImageStyle* pStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pStyle)
		pStyle->SetImage(pImageObj);
}

void CTreeNodeUI::SetImage(LPCTSTR lpszName)
{
	if (lpszName == NULL || equal_icmp(lpszName, _T(""))) return;
	StyleObj* pStyleObj = GetStyle(_Style_TreeNode_Icon);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T(" Style TreeNode Icon"));
	ImageStyle* pStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pStyle)
		pStyle->SetImage(lpszName);
}
void CTreeNodeUI::SetImage(LPVOID lpIcon, long nSize)
{
	if (lpIcon == NULL) return;
	StyleObj* pStyleObj = GetStyle(_Style_TreeNode_Icon);
	RT_CLASS_ASSERT(pStyleObj, ImageStyle, _T(" Style TreeNode Icon"));
	ImageStyle* pStyle = static_cast<ImageStyle*>(pStyleObj);
	if (pStyle)
		pStyle->SetImage(lpIcon, nSize);
}
void CTreeNodeUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	if (!m_bCalc)
	{
		CalcRect();
		m_bCalc = true;
	}

	RenderCheck(pRenderDC, rcPaint);

	//StyleObj* pStyle = NULL;
	if (IsGroup())
	{
		StyleObj* pStyle = (StyleObj*)GetStyle(_Style_TreeNode_Icon);
		if (pStyle)
			pStyle->SetState((UITYPE_STATE)((IsSelected())?(IsExpand(0)?3:1):(IsExpand(0)?2:0)));
	}
	__super::Render(pRenderDC, rcPaint);
}

bool CTreeNodeUI::Add(CControlUI* pControl)
{
	if (IsGroup())
	{
		CTreeUI* pTree = static_cast<CTreeUI*>(GetParent());
		CTreeNodeUI* pNode = dynamic_cast<CTreeNodeUI*>(pControl);
		if (pTree && pNode)
		{
			return pTree->InsertItem(pNode, this) != NULL;
		}
	}
	return false;
}

bool CTreeNodeUI::Activate()
{
	if (!CControlUI::Activate())
		return false;
	if (m_bEnableExpand)
		Expand(!IsExpand());
	return true;
}

bool CTreeNodeUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_BUTTONDOWN:
		{
			if(!IsSelected())
				CTreeNodeUI::ms_bTreeNodeExpand = false;
			else
				CTreeNodeUI::ms_bTreeNodeExpand = true;
			event.pSender = this;
			static_cast<CTreeUI*>(GetParent())->CTreeUI::ChildEvent(event);
		}
		return true;
	case UIEVENT_BUTTONUP:
		{
			if (m_pCheckNode && IsCheck(UIITEMFLAG_VISIBLE))
			{
				if (m_pCheckNode->HitTest(event.ptMouse))
				{
					if (IsGroup())
						return true;
				}
			}

			event.pSender = this;
			static_cast<CTreeUI*>(GetParent())->CTreeUI::ChildEvent(event);
		}
		return true;
	case UIEVENT_SCROLLWHEEL:
	case UIEVENT_KEYDOWN:
	case UIEVENT_RBUTTONUP:
		{
			event.pSender = this;
			static_cast<CTreeUI*>(GetParent())->CTreeUI::ChildEvent(event);
		}
		return true;
	case UIEVENT_LDBCLICK:
		{
			event.pSender = this;
			static_cast<CTreeUI*>(GetParent())->CTreeUI::ChildEvent(event);
		}
		return true;
	case UIEVENT_SELECT:
		{
			GetWindow()->LockUpdate();
			Select(!event.lParam);
			GetWindow()->UnLockUpdate();
			Invalidate();
		}
		break;
	}
	return CControlUI::Event(event);
}


void CTreeNodeUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(_T("group"), lpszName))
	{
		node_group((bool)!!_ttoi(lpszValue));
		if (IsGroup())
			m_bCanDrag = false;
		else
			m_bCanDrag = true;
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

void CTreeNodeUI::SetRect(RECT& rectRegion)
{
	m_bCalc = false;
	__super::SetRect(rectRegion);
}

void CTreeNodeUI::CalcRect()
{
	if (GetStyleCount())
	{
		RECT rcInset = GetInterRect();
		RECT rcOffset = rcInset;
		if (m_pCheckNode && IsCheck(UIITEMFLAG_VISIBLE))
			rcOffset.left += DPI_SCALE(GetCheckOffset());
		StyleObj* pStyleObj = NULL;
		int nCheckIndex = IsGroup()?_Style_TreeNode_Txt:_Style_TreeNode_Icon;

		if (m_pCheckNode && IsCheck(UIITEMFLAG_VISIBLE))
		{
			pStyleObj = (StyleObj*)GetStyle(nCheckIndex);
			if (pStyleObj)
			{
				pStyleObj->OnSize(rcInset);
				RECT rc = rcInset;
				rc.left = pStyleObj->GetRect().left;
				SetCheckRect(rc);
			}
		}
		int nCnt = GetStyleCount();
		for (int i = 0; i < nCnt; i++)
		{
			pStyleObj = (StyleObj*)GetStyle(i);
			if (pStyleObj)
			{
				if (i >= nCheckIndex)
					pStyleObj->OnSize(rcOffset);
				else
					pStyleObj->OnSize(rcInset);
			}
		}
	}
}

void CTreeNodeUI::node_show( bool bShow )
{
	GetWindow()->LockUpdate();
	SetVisible(bShow);
	GetWindow()->UnLockUpdate();
}

bool CTreeNodeUI::is_node_show()
{
	return IsVisible();
}

int CTreeNodeUI::get_node_index()
{
	return GetIndex();
}

void CTreeNodeUI::set_node_index( int nIndex )
{
	SetIndex(nIndex);
}

void CTreeNodeUI::CheckLegal()
{
	CTreeNodeUI* pNode = NULL;
	//CTreeNodeUI* pSubNode = NULL;
	pNode = this;
	//int nCheck = 0;
	//int nVisible = 0;
	bool bCheckLegalParent = true;
	if(pNode && pNode->GetIndex() != -1)
	{
		int nCheck = 0;
		int nVisible = 0;
		int nChildCnt = pNode->num_child();
		for (int i = 0; i < nChildCnt; i++)
		{
			CTreeNodeUI* pSubNode = pNode->get_child(i);
			if (pSubNode)
			{
				if (pSubNode->IsCheck(UIITEMFLAG_VISIBLE))
				{
					nVisible += 2;
					if (pSubNode->m_pCheckNode && pSubNode->m_pCheckNode->IsPartialChecked())
						nCheck += 1;
					else
						nCheck += (((int)pSubNode->IsCheck(UIITEMFLAG_CHECKED))*2);
				}
			}
		}
		if (m_pCheckNode)
			m_pCheckNode->SetPartialChecked(false);
		if (nVisible > 0 && nCheck > 0 && nVisible != nCheck)
		{
			if (m_pCheckNode)
				m_pCheckNode->SetPartialChecked(true);
		}
		if (nVisible > 0)
		{
			if (nVisible == nCheck)
			{
				pNode->Check(true, UIITEMFLAG_CHECKED);
				bCheckLegalParent = false;
			}
			if (nCheck == 0)
			{
				pNode->Check(false, UIITEMFLAG_CHECKED);
				bCheckLegalParent = false;
			}
		}
	}
	if (bCheckLegalParent)
	{
		CTreeNodeUI* pParent = get_parent();
		if (pParent)
			pParent->CheckLegal();
	}
}

void CTreeNodeUI::set_child( CTreeNodeUI* node, int nIndex )
{
	set_child_node(node, nIndex);
}

void CTreeNodeUI::CalcControl( RECT & rectCalc )
{
	rectCalc = GetInset();
	DPI_SCALE(&rectCalc);
	if (m_pCheckNode && IsCheck(UIITEMFLAG_VISIBLE))
		rectCalc.left += DPI_SCALE(GetCheckOffset());
	rectCalc.right = rectCalc.left;
	rectCalc.left = 0;
	RECT rcCalc = {0, 0, 0, 0};
	Coordinate* pCoordinate = NULL;
	TextStyle* pText = GetDefaultTextStyle();
	if (pText)
		pCoordinate = pText->GetCoordinate();
	FontObj* pFont = NULL;
	if (pText)
		pFont = pText->GetFontObj();
	else
		pFont = GetUIRes()->GetDefaultFont();
	if (pFont && GetWindow())
		pFont->CalcText(GetWindow()->GetPaintDC(),  rcCalc, GetText());
	if (pCoordinate)
	{
		RECT rcParent = {0};
		RECT rcCoordinate = pCoordinate->rect;
		DPI_SCALE(&rcCoordinate);
		CalcParentPosWithAnchor(pCoordinate->anchor, &rcCoordinate, &rcCalc, &rcParent);
		rectCalc.right += (rcParent.right - rcParent.left);
	}
	else
	{
		rectCalc.right += rcCalc.right + DPI_SCALE(18);
	}
}

CTreeNodeUI* CTreeNodeUI::get_nextsibling( CTreeNodeUI* pNode )
{
	return static_cast<CTreeNodeUI*>(get_nextsibling_node(pNode));
}

CTreeNodeUI* CTreeNodeUI::get_prevsibling( CTreeNodeUI* pNode )
{
	return static_cast<CTreeNodeUI*>(get_prevsibling_node(pNode));
}
//////////////////////////////////////////////////////////////////////////
CTreeContainerNodeUI::CTreeContainerNodeUI(bool bGroup /*= false*/):CTreeNodeUI(bGroup)
{
	SetCoordinate(0, 0, 0, 28);
	m_bDrawSuper = false;
	m_nMaskClick = 0;
}

CTreeContainerNodeUI::~CTreeContainerNodeUI()
{
	UICustomControlHelper::RemoveAllChild(m_items);
}


bool CTreeContainerNodeUI::Add(CControlUI* pControl)
{
	if (pControl)
	{
		CTreeNodeUI* pNode = dynamic_cast<CTreeNodeUI*>(pControl);
		if (pNode == NULL)
		{
			m_bCalc = false;
			pControl->OnEvent += MakeDelegate(this, &CTreeContainerNodeUI::ChildEvent);
	
			UICustomControlHelper::AddChildControl(pControl, this, m_items);
		}
		else
		{
			__super::Add(pControl);
		}
		return true;
	}
	return false;
}

void CTreeContainerNodeUI::SetRect(RECT& rectRegion)
{
	m_bCalc = false;
	__super::SetRect(rectRegion);
}

bool CTreeContainerNodeUI::ChildEvent(TEventUI& event)
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
			CTreeUI* pTree = static_cast<CTreeUI*>(GetParent());
			if (pTree)
			{
				pTree->SelectItem(this);
				pTree->SendNotify(UINOTIFY_DBCLICK, (WPARAM)event.pSender, (LPARAM)this);
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
			CTreeUI* pTree = static_cast<CTreeUI*>(GetParent());
			if (pTree)
			{
				pTree->SelectItem(this);
				pTree->EnsureVisible(this);
				pTree->SendNotify(UINOTIFY_CLICK, (WPARAM)event.pSender, (LPARAM)this);
			}
			if (nMaskClick == 1)
				event.nType = UIEVENT_UNUSED;
		}
		break;
	}
	return true;
}

void CTreeContainerNodeUI::CalcRect()
{
	RECT rcInset = GetInterRect();
	RECT rcOffset = rcInset;
	if (m_pCheckNode && IsCheck(UIITEMFLAG_VISIBLE))
		rcOffset.left += DPI_SCALE(GetCheckOffset());

	RECT rcClient = GetRect();
	UICustomControlHelper::LayoutChildNormal(rcClient, rcOffset, m_items, false);
	if (m_bDrawSuper)
	{
		__super::CalcRect();
	}
}

void CTreeContainerNodeUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	if (!m_bCalc)
	{
		CalcRect();
		m_bCalc = true;
	}

	if (m_bDrawSuper)
		__super::Render(pRenderDC, rcPaint);

	RenderCheck(pRenderDC, rcPaint);

	RECT rcItem = GetRect();
	UICustomControlHelper::RenderChildControl(pRenderDC, rcPaint, rcItem, rcItem, m_items, 0, -1);

}

CControlUI* CTreeContainerNodeUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if (!FindControlTest(this, pData, uFlags))
		return NULL;
	if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
		CControlUI* pControl = CControlUI::FindControl(Proc, pData, uFlags);
		if( pControl != NULL ) return pControl;
	}

	CControlUI* pControl = UICustomControlHelper::FindChildControl(m_items, Proc, pData, uFlags);
	if (pControl)
	{
		return pControl;
	}

	return CControlUI::FindControl(Proc, pData, uFlags);

}
CControlUI* CTreeContainerNodeUI::GetItem(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_items.GetSize() ) return NULL;
	return static_cast<CControlUI*>(m_items[iIndex]);
}

CControlUI* CTreeContainerNodeUI::GetItem(LPCTSTR lpszId)
{
	tstring strID = lpszId;
	int nOffset  = strID.find_first_of(_T('.'));
	if (nOffset == tstring::npos)
	{
		//CControlUI* pControl = NULL;
		int nCnt = GetCount();
		for( int i = 0;  i < nCnt; i++ )
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items.GetAt(i));
			if (pControl && equal_icmp(pControl->GetId(), lpszId))
				return pControl;
		}
	}
	else
	{
		CControlUI* pControl = GetItem(strID.substr(0, nOffset).c_str());
		if (pControl)
		{
			strID = strID.substr(nOffset + 1);
			return pControl->GetItem(strID.c_str());
		}
	}
	return NULL;
}

bool CTreeContainerNodeUI::Remove(CControlUI* pControl)
{
	return UICustomControlHelper::RemoveChildControl(pControl, m_items);
}

void CTreeContainerNodeUI::SetINotifyUI(INotifyUI* pINotify)
{
	__super::SetINotifyUI(pINotify);
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
		GetItem(i)->SetINotifyUI(pINotify);
}

void CTreeContainerNodeUI::SetManager(CWindowUI* pManager, CControlUI* pParent)
{
	__super::SetManager(pManager, pParent);
	int nCnt = GetCount();
	for (int i = 0; i < nCnt; i++)
		GetItem(i)->SetManager(pManager, this);
}

int CTreeContainerNodeUI::GetCount()
{
	return m_items.GetSize();
}

void CTreeContainerNodeUI::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{
	if (equal_icmp(_T("DrawNode"), lpszName))
	{
		m_bDrawSuper = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("MaskClick")))
	{
		m_nMaskClick = _ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

void CTreeContainerNodeUI::SetVisible( bool bVisible)
{
	int nCnt = GetCount();
	for( int i = 0; i < nCnt; i++ ) 
		static_cast<CControlUI*>(m_items.GetAt(i))->SetInternVisible(bVisible);
	__super::SetVisible(bVisible);
}

void CTreeContainerNodeUI::SetInternVisible( bool bVisible )
{
	int nCnt = GetCount();
	for( int i = 0; i < nCnt; i++ ) 
		static_cast<CControlUI*>(m_items.GetAt(i))->SetInternVisible(bVisible);
	__super::SetInternVisible(bVisible);
}