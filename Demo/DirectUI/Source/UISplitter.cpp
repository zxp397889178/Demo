/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************/

#include "stdafx.h"
#include "UISplitter.h"

CSplitterUI::CSplitterUI()
{
	ModifyFlags(UICONTROLFLAG_SETFOCUS | UICONTROLFLAG_SETCURSOR, 0);
	SetMouseEnabled(true);
	m_pFirst = NULL;
	m_pSecond = NULL;
	m_bHorizontal = true;
	m_nMinFirst = m_nMinSecond = 0;
	memset(&m_rcFirstCfg, 0, sizeof(RECT));
	memset(&m_rcSecondCfg, 0, sizeof(RECT));
}
CSplitterUI::~CSplitterUI()
{

}


void CSplitterUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("first")))
	{
		m_szFirstName = lpszValue;
	}
	else if (equal_icmp(lpszName, _T("second")))
	{
		m_szSecondName = lpszValue;
	}
	else if (equal_icmp(lpszName, _T("minfirst")))
	{
		m_nMinFirst = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("minsecond")))
	{
		m_nMinSecond = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("vert")))
	{
		m_bHorizontal = (bool)!!_ttoi(lpszValue);
		m_bHorizontal = !m_bHorizontal;
	}
	else 
		CControlUI::SetAttribute(lpszName, lpszValue);
}
bool CSplitterUI::Event(TEventUI& event)
{
	switch(event.nType)
	{
	case UIEVENT_SETCURSOR:
		{
			if (m_bHorizontal)
				::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
			else
				::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		}
		break;
	case UIEVENT_BUTTONDOWN:
		{
			m_ptLastMouse = event.ptMouse;
			GetWindow()->SetTimer(this, 1, 30U);
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			GetWindow()->KillTimer(this, 1);
		}
		break;
	
	case UIEVENT_TIMER:
		{
			POINT ptCurMouse;
			GetCursorPos(&ptCurMouse);
			ScreenToClient(GetWindow()->GetHWND(), &ptCurMouse);
			int nOffset = 0;
			if (m_bHorizontal)
				nOffset = ptCurMouse.y - m_ptLastMouse.y;
			else
				nOffset = ptCurMouse.x - m_ptLastMouse.x;
			m_ptLastMouse = ptCurMouse;

			if (nOffset == 0)
				return true;

			//第一次改变大小时保存两个控件及其初始配置
			int l, t, r, b;
			if (!m_pFirst)
			{
				GetCoordinate(l, t, r, b);
				m_rcSefCfg.left = l;
				m_rcSefCfg.right = r;
				m_rcSefCfg.top = t;
				m_rcSefCfg.bottom = b;
				m_pFirst = GetParent()->GetItem(m_szFirstName.c_str());
				if (m_pFirst)
				{
					m_pFirst->GetCoordinate(l, t, r, b);
					m_rcFirstCfg.left = l;
					m_rcFirstCfg.top = t;
					m_rcFirstCfg.right = r;
					m_rcFirstCfg.bottom = b;
					
				}
			}
			if (!m_pSecond)
			{
				m_pSecond = GetParent()->GetItem(m_szSecondName.c_str());
				if (m_pSecond)
				{
					m_pSecond->GetCoordinate(l, t, r, b);
					m_rcSecondCfg.left = l;
					m_rcSecondCfg.top = t;
					m_rcSecondCfg.right = r;
					m_rcSecondCfg.bottom = b;
				}
			}
			if (!m_pFirst || !m_pSecond)
				return true;
			RECT rect;

			//实际应移动的大小，放大移动过度
			if (m_bHorizontal)
			{
				if (nOffset > 0)
				{
					rect = m_pSecond->GetRect();
					if (rect.bottom - rect.top <= DPI_SCALE(m_nMinSecond))
						return true;
					nOffset = min(rect.bottom - rect.top - DPI_SCALE(m_nMinSecond), nOffset);

				}
				else
				{
					rect = m_pFirst->GetRect();
					if (rect.bottom - rect.top <= DPI_SCALE(m_nMinFirst))
						return true;

					nOffset = -min(rect.bottom - rect.top - DPI_SCALE(m_nMinSecond), abs(nOffset));
				}
			}
			else
			{
				if (nOffset > 0)
				{
					rect = m_pSecond->GetRect();
					if (rect.right - rect.left <= DPI_SCALE(m_nMinSecond))
						return true;
					nOffset = min(rect.right - rect.left - DPI_SCALE(m_nMinSecond), nOffset);

				}
				else
				{
					rect = m_pFirst->GetRect();
					if (rect.right - rect.left <= DPI_SCALE(m_nMinFirst))
						return true;

					nOffset = -min(rect.right - rect.left - DPI_SCALE(m_nMinSecond), abs(nOffset));
				}

			}
			
			nOffset = DPI_SCALE_BACK(nOffset);
			//计算前后两个控件及自身的新初始位置
			if (m_bHorizontal)
			{
				int nAnchor = m_pFirst->GetAnchor();
				m_pFirst->GetCoordinate(l, t, r, b);
				switch (nAnchor)
				{
				case 0:
				case 1:
				case 4:
					b += nOffset;
					break;
				default:
					b -= nOffset;
					break;
				}
				m_pFirst->SetCoordinate(l, t, r, b);

				nAnchor = m_pSecond->GetAnchor();
				m_pSecond->GetCoordinate(l, t, r, b);
				switch (nAnchor)
				{
				case 2:
				case 3:
				case 6:
					t -= nOffset;
					break;
				default:
					t += nOffset;
					break;
				}
				m_pSecond->SetCoordinate(l, t, r, b);


				nAnchor = this->GetAnchor();
				this->GetCoordinate(l, t, r, b);
				switch (nAnchor)
				{
				case 2:
				case 3:
				case 6:
					t -= nOffset;
					b -= nOffset;
					break;
				case 0:
				case 1:
				case 4:
					t += nOffset;
					b += nOffset;
					break;
				case 5:
				case 7:
				case 8:
					t += nOffset;
					b -= nOffset;
					break;
				}
				this->SetCoordinate(l, t, r, b);
			
			}
			else
			{
				int nAnchor = m_pFirst->GetAnchor();
				m_pFirst->GetCoordinate(l, t, r, b);
				switch (nAnchor)
				{
				case 0:
				case 3:
				case 7:
					r += nOffset;
					break;
				default:
					r -= nOffset;
					break;
				}
				m_pFirst->SetCoordinate(l, t, r, b);

				nAnchor = m_pSecond->GetAnchor();
				m_pSecond->GetCoordinate(l, t, r, b);
				switch (nAnchor)
				{
				case 1:
				case 2:
				case 5:
					l -= nOffset;
					break;
				default:
					l += nOffset;
					break;
				}
				m_pSecond->SetCoordinate(l, t, r, b);


				nAnchor = this->GetAnchor();
				this->GetCoordinate(l, t, r, b);
				switch (nAnchor)
				{
				case 1:
				case 2:
				case 5:
					l -= nOffset;
					r -= nOffset;
					break;
				case 0:
				case 3:
				case 7:
					l += nOffset;
					r += nOffset;
					break;
				case 4:
				case 6:
				case 8:
					l += nOffset;
					r -= nOffset;
					break;
				}
				this->SetCoordinate(l, t, r, b);
			}
			dynamic_cast<CContainerUI*>(GetParent())->Resize();
			GetParent()->Invalidate();
			this->SendNotify(UINOTIFY_SELCHANGE);
		}
		break;
	default:
		return CControlUI::Event(event);
		break;
	}
	return true;
}

void CSplitterUI::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);
	if (m_pFirst && m_pSecond)
	{
		RECT rcFirst = m_pFirst->GetRect();
		RECT rcSecond = m_pSecond->GetRect();
		//窗口缩小时会出现矩形小于0的错误
		bool bNeedResize = false;
		if (m_bHorizontal)
		{
			if (rcFirst.bottom <= rcFirst.top || rcSecond.bottom <= rcSecond.top)  
				bNeedResize = true;
		}
		else
		{
			if (rcFirst.right <= rcFirst.left || rcSecond.right <= rcSecond.left)  
				bNeedResize = true;
			
		}
		
		if (bNeedResize)
		{
			this->SetCoordinate(m_rcSefCfg.left, m_rcSefCfg.top, m_rcSefCfg.right, m_rcSefCfg.bottom);
			m_pFirst->SetCoordinate(m_rcFirstCfg.left, m_rcFirstCfg.top, m_rcFirstCfg.right, m_rcFirstCfg.bottom);
			m_pSecond->SetCoordinate(m_rcSecondCfg.left, m_rcSecondCfg.top, m_rcSecondCfg.right, m_rcSecondCfg.bottom);
			RECT rcParent = GetParent()->GetRect();
			m_pFirst->OnSize(rcParent);
			m_pSecond->OnSize(rcParent);
		}
	}
}