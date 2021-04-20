#include "stdafx.h"
#include "UIEdit_AR.h"
//#ifdef DIRECTUI_LAYOUT_RTL

//////////////////////////////////////////////////////////////////////////
#define _UICOLOR_EDIT_BACKGROUND_READONLY RGB(240, 240, 240)

//////////////////////////////////////////////////////////////////////////////////////
#include "TextHostImpl.h"
class CEditTextHost : public CTextHostBase
{
public:
	CEditTextHost(CControlUI* pOwner):CTextHostBase(pOwner){m_bSetText = false;}
	void SetText(LPCTSTR lpcszText)
	{
		m_bSetText = true;
		__super::SetText(lpcszText);
		m_bSetText = false;
	}
	virtual HRESULT TxNotify(DWORD iNotify, void *pv)
	{
		switch (iNotify)
		{
		case EN_CHANGE:
			{
				if (!pv) break;
				CHANGENOTIFY * penChangeNotify = (CHANGENOTIFY*)pv;
				if (penChangeNotify->dwChangeType != CN_TEXTCHANGED)
					break;
				if (m_bSetText)
					break;

				GETTEXTLENGTHEX textLenEx;
				textLenEx.flags = GTL_DEFAULT;
				textLenEx.codepage = 1200;
				LRESULT lResult;
				GetTextServices()->TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
				m_pOwner->SendNotify(UINOTIFY_EN_CHANGE, (WPARAM)lResult);
				return S_OK;
			}
			break;
		}
		return __super::TxNotify(iNotify, pv);
	}
protected:
	bool m_bSetText;
private:
};

static CEditTextHost* ms_pTextHostEdit = NULL;

bool CEditArUI::m_bEnableSystemMenu = true;
CEditArUI::CEditArUI()
{
	m_bInit = false;
	m_bReadOnly = false;
	m_bTransparent = false;
	SetEnabled(true);
	SetMouseEnabled(true);
	m_bMenuEnable = CEditArUI::m_bEnableSystemMenu;
	m_hParentWnd = NULL;
	RECT rcInset = {4, 4, 4, 4};
	SetInset(rcInset);
	SetStyle(_T("Edit"));
	ModifyFlags(UICONTROLFLAG_SETCURSOR | UICONTROLFLAG_TABSTOP | UICONTROLFLAG_SETFOCUS, 0);

	m_bPassword = false;
	m_bPasswordVisiblity = false;
	m_bMaxSize = false;
	m_bAutoSize = false;
	ZeroMemory(&m_cxyMaxSize, sizeof(SIZE));
	m_bRTLReading = true;
	m_pTextStyle = NULL;
	m_pEmptyTextStyle = NULL;
	m_nMaxchar = -1;

	SetAttribute(_T("cursor"), _T("IBEAM"));
}

CEditArUI::~CEditArUI()
{
	HideToolTip();
	if (IsBindTextHost())
		delete ms_pTextHostEdit;
	ms_pTextHostEdit = NULL;
}


void CEditArUI::Render(IRenderDC* pRenderDC, RECT &rcPaint)
{
	if (!GetEditStyle()) return;
	if(!m_bTransparent)
		__super::Render(pRenderDC, rcPaint);

	if(m_bReadOnly && !m_bTransparent)
	{
		HDC hDestDC = pRenderDC->GetDC();
		HBRUSH hBr = CreateSolidBrush(_UICOLOR_EDIT_BACKGROUND_READONLY);
		FillRect(hDestDC, &/*GetInterRect()*/GetFlipInterRect(), hBr);
		::DeleteObject(hBr);
	}

	if (IsBindTextHost())
	{
		RECTL rect = { ms_pTextHostEdit->rcClient.left, ms_pTextHostEdit->rcClient.top, ms_pTextHostEdit->rcClient.right, ms_pTextHostEdit->rcClient.bottom};
		RECT rect1 = { ms_pTextHostEdit->rcClient.left, ms_pTextHostEdit->rcClient.top, ms_pTextHostEdit->rcClient.right, ms_pTextHostEdit->rcClient.bottom};
		ITextServices* pServ = ms_pTextHostEdit->GetTextServices();
		pServ->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, (HDC)pRenderDC->GetDC(), NULL, &rect, NULL, &rect1, NULL, NULL, TXTVIEW_ACTIVE);
	}
	else
	{
		RECT rect = /*GetInterRect()*/GetFlipInterRect();

		UINT uFlags = GetEditStyle()->GetAlign();
		if(m_strText.empty() && !m_strEmptyTipsText.empty())
		{
			if (GetEmptyEditStyle() != NULL)
			{
				uFlags = GetEmptyEditStyle()->GetAlign();
				if (m_bRTLReading)
					uFlags |= DT_RTLREADING;
				pRenderDC->DrawText(GetEmptyEditStyle()->GetFontObj(), rect, m_strEmptyTipsText.c_str(), GetEmptyEditStyle()->GetTextColor(), GetEmptyEditStyle()->GetAlign());
			}
			return;
		}
		if (m_bRTLReading)
			uFlags |= DT_RTLREADING;

		if (!m_bPassword)
			pRenderDC->DrawText(GetEditStyle()->GetFontObj(), /*GetInterRect()*/GetFlipInterRect(), m_strText.c_str(), RGB(0, 0, 0), uFlags, GetEditStyle()->GetFontEffect());
		else
		{
			tstring sText = m_strText;
			sText.replace(sText.begin(), sText.end(), sText.length(), _T('●'));
			pRenderDC->DrawText(GetEditStyle()->GetFontObj(), /*GetInterRect()*/GetFlipInterRect(), sText.c_str(), RGB(0, 0, 0), uFlags, GetEditStyle()->GetFontEffect());
		}
	}
}

bool CEditArUI::Event(TEventUI& event)
{
	//UINT uMsg = 0;
	//LRESULT lRes = 0;
	switch (event.nType)
	{
	case UIEVENT_SETFOCUS:
		{
			if (!GetEditStyle()) break;
			m_bFocused = true;
			if (IsBindTextHost())
			{
				delete ms_pTextHostEdit;
				ms_pTextHostEdit = NULL;
			}
			if (!ms_pTextHostEdit) 
				ms_pTextHostEdit = new CEditTextHost(this);

			CHARFORMAT2 cf;
			InitDefaultCharFormat(this, &cf, GetEditStyle()->GetTextColor(), m_bRTLReading);

			PARAFORMAT2 pf;
			InitDefaultParaFormat(this, &pf, m_bRTLReading);

			if (GetEditStyle()->GetAlign() & DT_LEFT)
				pf.wAlignment = PFA_LEFT;
			else if (GetEditStyle()->GetAlign() & DT_CENTER)
				pf.wAlignment = PFA_CENTER;
			else
				pf.wAlignment = PFA_RIGHT;

			ms_pTextHostEdit->fMultiLine =FALSE;
			ms_pTextHostEdit->fRich = FALSE;
			ms_pTextHostEdit->fWordWrap = FALSE;

			TEXTMETRIC tm;
			HFONT hOldFont = (HFONT) ::SelectObject(GetWindow()->GetPaintDC(), GetEditStyle()->GetFontObj()->GetFont());
			::GetTextMetrics(GetWindow()->GetPaintDC(), &tm);
			::SelectObject(GetWindow()->GetPaintDC(), hOldFont);

			m_tmHeight = tm.tmHeight;

			// 单行居中
			RECT rcInset = /*GetInterRect()*/GetFlipInterRect();
			int dy = (rcInset.bottom - rcInset.top - m_tmHeight) / 2;
			rcInset.top += dy;
			rcInset.bottom -= dy;
			rcInset.right += 1;
			ms_pTextHostEdit->Init(&cf, &pf);

			if (m_bPassword)
				ms_pTextHostEdit->SetPasswordChar(L'●');
			ms_pTextHostEdit->SetRect(rcInset);
			// 激活一下
		
			ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, NULL);
			ms_pTextHostEdit->SetReadOnly(m_bReadOnly);
			if (m_nMaxchar > 0)
				ms_pTextHostEdit->SetMaxChar(m_nMaxchar);

			// 设置文字
			ms_pTextHostEdit->SetText(m_strText.c_str());
			OnSetFocus();
			this->SendNotify(UINOTIFY_SETFOCUS, event.wParam, event.lParam);

		};
		break;
	case UIEVENT_KILLFOCUS:
		{
			m_bFocused = false;
			if (IsBindTextHost())
			{
				ms_pTextHostEdit->GetText(m_strText);
				ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, NULL);
				ms_pTextHostEdit->OnTxInPlaceActivate(NULL);
				delete ms_pTextHostEdit;
				ms_pTextHostEdit = NULL;
			}
			OnKillFocus();
			this->SendNotify(UINOTIFY_KILLFOCUS, event.wParam,event.lParam);//modify by hanzp.用于传出当前焦点
		};
		break;

	case UIEVENT_SETCURSOR:
		{
			if (IsBindTextHost())
			{
				ms_pTextHostEdit->DoSetCursor(NULL, &event.ptMouse);
			}
			else
			{
				__super::Event(event);
			}
			return true;
		}

	case UIEVENT_IME_ING:
		{
			if (IsBindTextHost())
			{
				HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
				if(hImc)
				{
					if (event.lParam & GCS_CURSORPOS)
					{
						POINT pt;
						GetCaretPos(&pt);
						COMPOSITIONFORM form;
						ImmGetCompositionWindow(hImc, &form);
						form.dwStyle = CFS_POINT;
						form.ptCurrentPos.x = pt.x;
						form.ptCurrentPos.y = pt.y;
						ImmSetCompositionWindow(hImc, &form);

					}
					ImmReleaseContext(GetWindow()->GetHWND(), hImc);
				}
			}
		}
		break;

	case UIEVENT_KEYDOWN:
		{
			OnKeyDown(event.wParam, event.lParam, &event.ptMouse);
		}
		break;
	case UIEVENT_CHAR:
		{
			OnChar(event.wParam, event.lParam, &event.ptMouse);
		}
		break;
	case UIEVENT_MOUSEMOVE:
		{
			OnMouseMove(event.wParam, event.lParam, &event.ptMouse);
		}
		break;
	case UIEVENT_BUTTONDOWN:
		{
			OnLButtonDown(event.wParam, event.lParam, &event.ptMouse);
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			OnLButtonUp(event.wParam, event.lParam, &event.ptMouse);
		}
		break;
	case UIEVENT_LDBCLICK:
		{
			OnLButtonDblick(event.wParam, event.lParam, &event.ptMouse);
		}
		break;
	case UIEVENT_RBUTTONUP:
		{
			OnRButtonUp(event.wParam, event.lParam, &event.ptMouse);
		}
		break;
	}
	return __super::Event(event);
}

void  CEditArUI::SetRect(RECT& rect)
{
	//__super::SetRect(rect);

	m_rcItem = rect;
	if (m_bAutoSize)
	{
		if (!GetEditStyle()) return;

		if(!m_bMaxSize)
		{
			RECT rcParent = GetParent()->GetInterRect();
			m_cxyMaxSize.cx = DPI_SCALE_BACK(rcParent.right - m_rcItem.left);
		}

		RECT rcInset = GetInset();
		DPI_SCALE(&rcInset);
		RECT rect1 = rect;
		OffsetRect(&rect1, -rect1.left, -rect1.top);
		GetEditStyle()->GetFontObj()->CalcText(GetWindow()->GetPaintDC(), rect1, m_strText.c_str(),GetEditStyle()->GetAlign());
		int nTextWidth = rect1.right - rect1.left;
		if (DPI_SCALE(m_cxyMaxSize.cx) > nTextWidth)
			m_rcItem.right = m_rcItem.left + nTextWidth + rcInset.right + rcInset.left;
		else
			m_rcItem.right = m_rcItem.left + DPI_SCALE(m_cxyMaxSize.cx) + rcInset.right + rcInset.left;

	}
	CControlUI::SetRect(m_rcItem);

	if (IsBindTextHost())
	{
		// 单行居中
		RECT rcInset = /*GetInterRect()*/GetFlipInterRect();
		int dy = (rcInset.bottom - rcInset.top - m_tmHeight) / 2;
		rcInset.top += dy;
		rcInset.bottom -= dy;
		rcInset.right += 1;

		ms_pTextHostEdit->SetRect(rcInset);
	}
}

void CEditArUI::SetText(LPCTSTR lpszText)
{
	bool bChange = !equal_icmp(m_strText.c_str(), lpszText);
	m_strText = lpszText;
	if (IsBindTextHost())
		ms_pTextHostEdit->SetText(lpszText);
	if (m_bInit)
	{
		if (bChange)
			this->SendNotify(UINOTIFY_EN_CHANGE);
		this->Invalidate();
	}
}

void CEditArUI::SetTextWithoutNotify( LPCTSTR lpszText )
{
	m_strText = lpszText;
	if (IsBindTextHost())
		ms_pTextHostEdit->SetText(lpszText);
	this->Invalidate();
}

void CEditArUI::SelAllText()
{
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->TxSetFocus();//add by hanzp. 全选时候需要设置焦点
		ms_pTextHostEdit->SetSel(0, -1);
	}
}

void CEditArUI::HideToolTip()
{
	/*CWindowUI* pWindow = NULL;
	if (m_hToolTipWnd && IsWindow(m_hToolTipWnd))
	{
		pWindow = GetEngineObj()->GetWindow(m_hToolTipWnd);
	}
	if (pWindow)
	{
		pWindow->CloseWindow();
	}*/
}

void CEditArUI::Init()
{
	m_hParentWnd = GetWindow()->GetHWND();
	m_bInit = true;
}

bool CEditArUI::SetSelPos(int pos/*=-1*/)
{
	GetWindow()->SetFocus(this);
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->SetSel(pos,pos);
	}
	return true;
}

void CEditArUI::OnSetFocus()
{
}

void CEditArUI::OnKillFocus()
{

}

bool CEditArUI::IsMouseWhellEnabled()
{
	if (IsFocused())
	{
		return true;
	}
	return __super::IsMouseWhellEnabled();
}

void CEditArUI::OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	UINT nChar = (UINT)wParam;
	if (nChar == 'V')
	{
		if(!m_bReadOnly)
		{
			SHORT value1 = GetKeyState(VK_LCONTROL);
			SHORT value2 = GetKeyState(VK_RCONTROL);
			if ((value1 & 0x8000) || (value2 & 0x8000))
			{
				Paste();
				return;
			}
		}
	}
	else if (nChar == 'C')
	{
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
		{
			Copy();
			return;
		}
	}
	else if (nChar == 'X')
	{
		if(!m_bReadOnly)
		{
			SHORT value1 = GetKeyState(VK_LCONTROL);
			SHORT value2 = GetKeyState(VK_RCONTROL);
			if ((value1 & 0x8000) || (value2 & 0x8000))
			{
				Cut();
				return;
			}
		}
	}
	else if (nChar == VK_RETURN)
	{

		GetWindow()->KillFocus(this);
		this->SendNotify(UINOTIFY_EN_KEYDOWN, (WPARAM)nChar, NULL);
		return;
	}
	LRESULT lRes;
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_KEYDOWN, wParam, lParam, &lRes);
	}
}

void CEditArUI::OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if (m_bReadOnly)
	{
		return;
	}
	UINT nChar = (UINT)wParam;
	if (nChar < VK_SPACE && nChar != VK_RETURN && nChar != VK_ESCAPE && nChar != '\t')
	{
		return;
	}
	if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		return;
	}
	SHORT value1 = GetKeyState(VK_LCONTROL);
	SHORT value2 = GetKeyState(VK_RCONTROL);
	if ((value1 & 0x8000) || (value2 & 0x8000))
	{
		return;
	}

	if(OnLimitChar(wParam))
	{
		return;
	}
	LRESULT lRes;
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_CHAR, wParam, lParam, &lRes);
	}
}

void CEditArUI::OnLButtonDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	LRESULT lRes;
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_LBUTTONDOWN, wParam, lParam, &lRes);
	}
}

void CEditArUI::OnLButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	LRESULT lRes;
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_LBUTTONUP, wParam, lParam, &lRes);
	}
}

void CEditArUI::OnLButtonDblick(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	LRESULT lRes;
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_LBUTTONDBLCLK, wParam, lParam, &lRes);
	}
}

void CEditArUI::OnRButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if(!m_bMenuEnable) return;
	CMenuUI *pMenu = CMenuUI::Load(IDR_MENU_UIEDIT);
	if (pMenu == NULL) return;
	CMenuUI *pSubMenu = NULL;
	int nSelEnd;
	int nSelBegin;
	GetSel(nSelBegin, nSelEnd);
	if(m_bReadOnly)
	{
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_CUT, BY_COMMAND);
		if(pSubMenu) pMenu->Remove(pSubMenu);
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_PASTE, BY_COMMAND);
		if(pSubMenu) pMenu->Remove(pSubMenu);
	}
	if(nSelBegin == nSelEnd)
	{
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_CUT, BY_COMMAND);
		if(pSubMenu) pSubMenu->EnableItem(false);
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_COPY, BY_COMMAND);
		if(pSubMenu) pSubMenu->EnableItem(false);
	}
#ifdef UNICODE
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
#else
	if (!IsClipboardFormatAvailable(CF_TEXT))
#endif
	{
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_PASTE, BY_COMMAND);
		if(pSubMenu) pSubMenu->EnableItem(false);
	}
	pMenu->Show(GetWindow()->GetHWND(), NULL, this);

	LRESULT lRes;
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_RBUTTONUP, wParam, lParam, &lRes);
	}
}

void CEditArUI::OnMouseMove(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	LRESULT lRes;
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->GetTextServices()->TxSendMessage(WM_MOUSEMOVE, wParam, lParam, &lRes);
	}
}

void CEditArUI::SetReadOnly( bool bReadOnly /*= true*/ )
{
	m_bReadOnly = bReadOnly;
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->SetReadOnly(bReadOnly);
	}
}

bool CEditArUI::IsBindTextHost() const
{
	return ms_pTextHostEdit && (ms_pTextHostEdit->GetOwner() == this);
}

bool CEditArUI::IsReadOnly()
{
	return m_bReadOnly;
}

void CEditArUI::EnableSystemMenu(bool bEnableSystemMenu)
{
	m_bEnableSystemMenu = bEnableSystemMenu;
}

void CEditArUI::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{
	if (equal_icmp(lpszName, _T("readonly")))
	{
		m_bReadOnly = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("password")))
	{
		m_bPassword = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("transparent")))
	{
		m_bTransparent = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("menuenable")))
	{
		m_bMenuEnable = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("RTLReading")))
	{
		m_bRTLReading = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("emptytips")))
	{
		m_strEmptyTipsText = I18NSTR(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("maxwidth")))
	{
		m_bMaxSize = true;
		m_cxyMaxSize.cx = (int)_ttoi(lpszValue);
		RECT rcInset = GetInset();
		m_cxyMaxSize.cx -= rcInset.left + rcInset.right;
	}
	else if (equal_icmp(lpszName, _T("autosize")))
	{
		m_bAutoSize = (bool)!!_ttoi(lpszValue);
		//if(m_bAutoSize) m_bTransparent = true;
	}
	else if (equal_icmp(lpszName, _T("maxchar")))
	{
		m_nMaxchar = _ttoi(lpszValue);
		if(m_nMaxchar < 0) m_nMaxchar = -1;
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

bool CEditArUI::Notify( TNotifyUI* msg )
{
	switch(msg->nType)
	{
	case UINOTIFY_EN_CHANGE:
		{
			if (IsBindTextHost())
			{
				ms_pTextHostEdit->GetText(m_strText);
			}
		}
		break;
	}
   return __super::Notify(msg);
}

void CEditArUI::GetSel( int& nStartChar, int& nEndChar) const
{
	if (IsBindTextHost())
	{
		CHARRANGE cr;
		ms_pTextHostEdit->GetSel(cr);
		nStartChar = cr.cpMin;
		nEndChar = cr.cpMax;
	}
	else
	{
		nStartChar = 0;
		nEndChar = 0;
	}
}

bool CEditArUI::OnLimitChar( UINT nChar )
{
	if (nChar == 0)//0为空字符
	{
		return true;
	}

	/*bool bCanInput = false;
	if (m_bLimit)
	{
		if (m_uLimitType < enEditExceptNumber)
		{
			bCanInput = false;
			if ((m_uLimitType & enEditLimitNumber) && _istdigit(nChar))
				bCanInput = true;
			if (!bCanInput && (m_uLimitType & enEditLimitLetter) && _istalpha(nChar))
				bCanInput = true;
			if (!bCanInput && (m_uLimitType & enEditLimitChinese) && IsChinese(nChar))
				bCanInput = true;
			if (!bCanInput && (m_uLimitType & enEditLimitString) && m_strLimitString.find_first_of(nChar) != tstring::npos)
				bCanInput = true;
		}

		if (m_uLimitType >= enEditExceptNumber)
		{
			bCanInput = true;
			if ((m_uLimitType & enEditExceptNumber) && _istdigit(nChar))
				bCanInput = false;
			if (bCanInput && (m_uLimitType & enEditExceptLetter) && _istalpha(nChar))
				bCanInput = false;
			if (bCanInput && (m_uLimitType & enEditExceptChinese) && IsChinese(nChar))
				bCanInput = false;
			if (bCanInput && (m_uLimitType & enEditExceptString) && m_strLimitString.find_first_of(nChar) != tstring::npos)
				bCanInput = false;
		}
	}
	if (!bCanInput)
	{
		if (m_uLimitType < enEditExceptNumber)
		{
			//modify by zg1129 阿语字符拼接有误
#if 0
			tstring strLimit = I18NSTR(_T("#LIMIT_INPUT"));
			bool bPrev = false;
			if (m_uLimitType & enEditLimitNumber)
			{
				if (bPrev)
					strLimit += _T(" ");
				strLimit += I18NSTR(_T("#LIMIT_NUMBER"));
				bPrev = true;
			}
			if (m_uLimitType & enEditLimitLetter)
			{
				if (bPrev)
					strLimit += _T(" ");
				strLimit += I18NSTR(_T("#LIMIT_LETTER"));
				bPrev = true;
			}
			if (m_uLimitType & enEditLimitChinese)
			{
				if (bPrev)
					strLimit += _T(" ");
				strLimit += I18NSTR(_T("#LIMIT_CHAR"));
				bPrev = true;
			}
			if (m_uLimitType & enEditLimitString)
			{
				if (bPrev)
					strLimit += _T(" ");
				if (m_strLimitString == _T(" "))
					strLimit += I18NSTR(_T("#LIMIT_BLANK"));
				else
					strLimit += m_strLimitString.c_str();
				bPrev = true;
			}
#endif
			tstring strLimit = I18NSTR(_T("#INPUT_ERROR"));
			bool bDefault = true;
			SendNotify(UINOTIFY_EN_LIMIT, NULL, (LPARAM)&bDefault);
			if (bDefault)
			{
				ShowToolTip(strLimit.c_str(), UITOOLTIP_Warn);
			}
		}
		else
		{
			//modify by zg1129 阿语字符拼接有误
#if 0
			tstring strLimit = I18NSTR(_T("#LIMIT_NOINPUT"));
			bool bPrev = false;
			if (m_uLimitType & enEditExceptNumber)
			{
				if (bPrev)
					strLimit += _T(" ");
				strLimit += I18NSTR(_T("#LIMIT_NUMBER"));
				bPrev = true;
			}
			if (m_uLimitType & enEditExceptLetter)
			{
				if (bPrev)
					strLimit += _T(" ");
				strLimit += I18NSTR(_T("#LIMIT_LETTER"));
				bPrev = true;
			}
			if (m_uLimitType & enEditExceptChinese)
			{
				if (bPrev)
					strLimit += _T(" ");
				strLimit += I18NSTR(_T("#LIMIT_CHAR"));
				bPrev = true;
			}
			if (m_uLimitType & enEditExceptString)
			{
				if (bPrev)
					strLimit += _T(" ");
				if (m_strLimitString == _T(" "))
					strLimit += I18NSTR(_T("#LIMIT_BLANK"));
				else
					strLimit += m_strLimitString.c_str();
				bPrev = true;
			}
#endif
			tstring strLimit = I18NSTR(_T("#INPUT_ERROR"));
			bool bDefault = true;
			SendNotify(UINOTIFY_EN_LIMIT, NULL, (LPARAM)&bDefault);
			if (bDefault)
			{
				ShowToolTip(strLimit.c_str(), UITOOLTIP_Warn);
			}
		}

	}
	return !bCanInput;*/
	return false;
}

void CEditArUI::Paste()
{
	OnPaste();
	{
		if(m_bReadOnly) return;
#ifdef UNICODE
		if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
#else
		if (!IsClipboardFormatAvailable(CF_TEXT))
#endif
			return;
		if (!::OpenClipboard(m_hParentWnd))
			return;
#ifdef UNICODE
		HGLOBAL hGlobalMem = GetClipboardData(CF_UNICODETEXT);
#else
		HGLOBAL hGlobalMem = GetClipboardData(CF_TEXT);
#endif
		tstring strText = _T("");
		if (hGlobalMem != NULL)
		{
			LPCTSTR lpszStr = (LPCTSTR)GlobalLock(hGlobalMem);
			if (lpszStr)
			{
				TCHAR *pStart = const_cast<LPTSTR>(lpszStr);
				TCHAR *pNext = pStart;
				//TCHAR ch = 0;
				bool bLimit = false;

				while (TRUE)
				{
					if(OnLimitChar(*pNext))
					{
						bLimit = true;
						break;
					}
					pNext = CharNext(pStart);
					if (pNext == pStart || *pNext == 0)
						break;
					pStart = pNext;
				}
				//ch = *pNext;
				*pNext = (TCHAR)0;

				if (!bLimit)
				{
					strText = lpszStr;
				}
			}
		}
		CloseClipboard();

		ms_pTextHostEdit->ReplaceSel(strText.c_str(), true);
	}
}

void CEditArUI::Copy()
{
	if (IsBindTextHost())
	{
		if (!OpenClipboard(m_hParentWnd))
			return;
		tstring selStr;
		ms_pTextHostEdit->GetSelText(selStr);
		int nCount = selStr.size();
		if (nCount == 0)
			return;
		HGLOBAL hGlobalMem = GlobalAlloc(GMEM_MOVEABLE, (nCount+1)*sizeof(TCHAR));
		if (!hGlobalMem)
		{
			CloseClipboard();
			return;
		}
		EmptyClipboard();
		LPTSTR lpszStr= (LPTSTR)GlobalLock(hGlobalMem);
		memcpy(lpszStr, selStr.c_str(), nCount* sizeof(TCHAR));
		lpszStr[nCount] = (TCHAR) 0;
		GlobalUnlock(hGlobalMem);
#ifdef UNICODE
		SetClipboardData(CF_UNICODETEXT, hGlobalMem);
#else
		SetClipboardData(CF_TEXT, hGlobalMem);
#endif
		CloseClipboard();
	}
}

void CEditArUI::Cut()
{
	Copy();
	if (IsBindTextHost())
	{
		ms_pTextHostEdit->ReplaceSel(_T(""), true);
	}
}

bool CEditArUI::OnMenuItemClick(TNotifyUI* pNotify)
{
	UIMENUINFO* pInfo = (UIMENUINFO*)pNotify->wParam;
	switch (pInfo->uID)
	{
	case IDR_MENU_UIEDIT_CUT:
		{
			Cut();
		}
		break;
	case IDR_MENU_UIEDIT_COPY:
		{
			Copy();
		}
		break;
	case IDR_MENU_UIEDIT_PASTE:
		{
			Paste();
			Invalidate();
		}
		break;
	case IDR_MENU_UIEDIT_SELALL:
		{
			SelAllText();
		}
		break;	
	default:
		break;
	}
	return true;
}

TextStyle* CEditArUI::GetEditStyle()
{
	if (!m_pTextStyle)
		m_pTextStyle = CControlUI::GetTextStyle(_T("txt"));
	return m_pTextStyle;
}

TextStyle* CEditArUI::GetEmptyEditStyle()
{
	if (!m_pEmptyTextStyle)
		m_pEmptyTextStyle = CControlUI::GetTextStyle(_T("txt_empty"));
	return m_pEmptyTextStyle;
}

tstring CEditArUI::GetPasswordText()
{
	return __super::GetText();

}

RECT CEditArUI::GetFlipInterRect()
{
	RECT rc = GetRect();
	RECT rcInset = GetInset();
	DPI_SCALE(&rcInset);
 	if (GetUIEngine()->IsRTLLayout() && IsRTLLayoutEnable())
 	{
 		rc.left += rcInset.right;
 		rc.right -= rcInset.left;
 		rc.top += rcInset.top;
 		rc.bottom -= rcInset.bottom;
 	}
 	else
	{
		rc.left += rcInset.left;
		rc.right -= rcInset.right;
		rc.top += rcInset.top;
		rc.bottom -= rcInset.bottom;
	}
	return rc;
}

//#endif 