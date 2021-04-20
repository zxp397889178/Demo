/*********************************************************************
 *       Copyright (C) 2013,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "UICaret.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
CCaretUI* CCaretUI::m_pCaretUI = NULL;

CCaretUI* CCaretUI::GetInstance()
{
	if (m_pCaretUI == NULL)
		m_pCaretUI = new CCaretUI();
	return m_pCaretUI;
}

void CCaretUI::ReleaseInstance()
{
	if (m_pCaretUI)
		delete m_pCaretUI;
}

CCaretUI::CCaretUI()
{
	m_hBitmap = NULL;
	m_hWnd = NULL;
	ZeroMemory(&m_rcItem, sizeof(RECT));
	m_objTimerSource += MakeDelegate(this, &CCaretUI::OnTimer);
	SetCaretBlinkTime(::GetCaretBlinkTime());
	m_bBlinkShow = FALSE;
	m_pWindow = NULL;
}

CCaretUI::~ CCaretUI()
{
	m_objTimerSource.Stop();
	m_pCaretUI = NULL;
	
}

void CCaretUI::OnTimer(CUITimerBase* pTimer)
{
	m_bBlinkShow = !m_bBlinkShow;
	m_pWindow->Invalidate(&m_rcItem);
}

BOOL  CCaretUI::SetCaretBlinkTime(__in UINT uMSeconds)
{
	m_objTimerSource.Stop();
	m_objTimerSource.SetTimerParam(uMSeconds);
	return ::SetCaretBlinkTime(uMSeconds);
}

UINT CCaretUI::GetCaretBlinkTime()
{
	return ::GetCaretBlinkTime();
}

BOOL  CCaretUI::CreateCaret(CWindowUI* pWindow, __in_opt HBITMAP hBitmap, __in int nWidth, __in int nHeight)
{
	::DestroyCaret();
	HideCaret();
	m_pWindow = pWindow;
	m_hWnd = pWindow->GetHWND();
	m_hBitmap = hBitmap;
	m_rcItem.right = m_rcItem.left + nWidth;
	m_rcItem.bottom = m_rcItem.top + nHeight;

	if (!m_pWindow)
		return FALSE;
	if (!m_pWindow->IsLayerWindow())
	{
		return ::CreateCaret(m_pWindow->GetHWND(), hBitmap, nWidth, nHeight);
	}
	else
	{
		HDC hDC = GetDC(pWindow->GetHWND());
		m_dibObj.Create(hDC, nWidth, nHeight);
		::ReleaseDC(pWindow->GetHWND(), hDC);
		return ::CreateCaret(m_pWindow->GetHWND(), hBitmap, nWidth, nHeight);
	}

	return TRUE;
}

BOOL CCaretUI::DestroyCaret(CWindowUI* pWindow)
{
	if (pWindow != m_pWindow)
		return FALSE;

	if (m_pWindow && m_pWindow->IsLayerWindow())
	{
		HideCaret();
	}
	m_pWindow = NULL;
	return ::DestroyCaret();
}

BOOL CCaretUI::SetCaretPos(__in int X, __in int Y)
{
	if (!m_pWindow || !IsWindow(m_hWnd))
		return FALSE;


	RECT rectOld = m_rcItem;
	int nWidth = m_rcItem.right - m_rcItem.left;
	int nHeight = m_rcItem.bottom - m_rcItem.top;
	m_rcItem.left = X;
	m_rcItem.top = Y;
	m_rcItem.right = m_rcItem.left + nWidth;
	m_rcItem.bottom = m_rcItem.top + nHeight;

	if (!m_pWindow->IsLayerWindow())
	{
		return ::SetCaretPos(X, Y);
	}
	else
	{
		BOOL bRet = ::SetCaretPos(X, Y);
	    if (X != rectOld.left || Y != rectOld.top)
		{
			m_bBlinkShow = TRUE;
			m_pWindow->Invalidate(&rectOld);
		}
		m_pWindow->Invalidate(&m_rcItem);
		return bRet;

	}
	return TRUE;
}

BOOL CCaretUI::GetCaretPos(LPPOINT lpPoint)
{
	if (!m_pWindow || !IsWindow(m_hWnd))
		return FALSE;
	lpPoint->x = m_rcItem.left;
	lpPoint->y = m_rcItem.top;
	return TRUE;
}

BOOL CCaretUI::HideCaret()
{
	if (!m_pWindow || !IsWindow(m_hWnd))
		return FALSE;

	 m_bVisible = false;

	if (!m_pWindow->IsLayerWindow())
	{
		::HideCaret(m_pWindow->GetHWND());
	}
	else
	{
		m_objTimerSource.Stop();
		m_pWindow->Invalidate(&m_rcItem);

	}
	return TRUE;
}

BOOL CCaretUI::ShowCaret()
{
	if (!m_pWindow || !IsWindow(m_hWnd))
		return FALSE;

	 m_bVisible = true;

	if (!m_pWindow->IsLayerWindow())
	{
		::ShowCaret(m_pWindow->GetHWND());
	}
	else
	{
		m_bBlinkShow = TRUE;
		m_pWindow->Invalidate(&m_rcItem);
		m_objTimerSource.Start();
	
	}

	return TRUE;
}

void CCaretUI::Render(IRenderDC* pRenderDC, RECT& rcUpdate)
{
	if (!m_pWindow || !IsWindow(m_hWnd))
		return;

	if (!m_pWindow->IsLayerWindow())
		return;

	if (!m_bVisible) return;
	if (!m_bBlinkShow) return;
	DibObj* pDibObj = pRenderDC->GetDibObj();
	if (!pDibObj ||  !pDibObj->IsValid()) return;
	if (m_rcItem.left <= 0 || m_rcItem.top <= 0 || m_rcItem.right > pDibObj->GetWidth() || m_rcItem.bottom > pDibObj->GetHeight()) return;
	int nWidth = m_rcItem.right - m_rcItem.left;
	int nHeight = m_rcItem.bottom - m_rcItem.top;
	RT_ASSERT((nWidth >= 0 && nHeight >= 0), _T("CCaretUI Render failed"));

	::BitBlt(m_dibObj.GetSafeHdc(), 0, 0, nWidth, nHeight, pDibObj->GetSafeHdc(), m_rcItem.left, m_rcItem.top, SRCCOPY);

	BYTE* pBmpBufferByte = m_dibObj.GetBits();
	UINT nBmpBufferSize = nWidth * nHeight * 4;
	for (UINT i = 0; i < nBmpBufferSize;)
	{
		UINT32 *p = (UINT32 *)(pBmpBufferByte + i);
		*p = ~*p;
		pBmpBufferByte[i + 3] = ~pBmpBufferByte[i + 3];

		if (m_hBitmap == 0)
			i += 4; // solid caret
		else
			i += 8; // gray caret
	}

	::BitBlt(pDibObj->GetSafeHdc(), m_rcItem.left, m_rcItem.top, nWidth, nHeight, m_dibObj.GetSafeHdc(), 0, 0, SRCCOPY);

}