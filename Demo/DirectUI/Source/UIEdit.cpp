/********************************************************************
*            Copyright (C) 2010, 网龙天晴程序应用软件部
*********************************************************************/

#include "stdafx.h"


#include "UIEdit.h"
#include <stdio.h>
#include <ctype.h>
#include<time.h>
#include "UICaret.h"

CMultiLineEditUI::CMultiLineEditUI()
{
	SetStyle(_T("Edit"));
	RECT rcInset = {4, 4, 4, 4};
	SetInset(rcInset);
	SetAttribute(_T("enableimage"), _T("0"));
	m_pTextStyle = NULL;
	m_bTransparent = false;
	m_pEmptyTextStyle = NULL;
}

CMultiLineEditUI::~CMultiLineEditUI()
{
}


LPCTSTR CMultiLineEditUI::GetText()
{
	CRichEditUI::GetText(m_strText);
	return m_strText.c_str();
}

void CMultiLineEditUI::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
	CRichEditUI::SetText(lpszText);
}


void CMultiLineEditUI::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{
	if (equal_icmp(lpszName, _T("transparent")))
	{
		m_bTransparent = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("emptytips")))
	{
		m_strEmptyTipsText = I18NSTR(lpszValue);
	}
	else
		__super::SetAttribute(lpszName, lpszValue);
}

void CMultiLineEditUI::Init()
{
	__super::Init();
	CScrollbarUI* pScrollBar = GetVScrollbar();
	if (pScrollBar)
	{
		int l ,t ,r,  b;
		pScrollBar->GetCoordinate(l, t, r, b);
		pScrollBar->SetCoordinate(0,t,0,b);
	}

	LRESULT lres=0;
	TxSendMessage( EM_SETLANGOPTIONS , 0, IMF_UIFONTS,  &lres);
	EnableVScrollBar(false);

	if (m_bTransparent)
	{
		for (int i = 0; i < GetStyleCount(); i++)
			GetStyle(i)->SetVisible(false);
	}
	TextStyle* pTextStyle = GetEditStyle();
	if (pTextStyle)
	{
		FontObj* fontObj = pTextStyle->GetFontObj();
		if (fontObj)
		{
			SetFontName(fontObj->GetFaceName());
			int fontSize = DPI_SCALE(fontObj->GetHeight()) * LY_PER_INCH / 96 / 20;
			SetFontSize(fontSize);
			SetTextColor(DUI_ARGB2RGB(pTextStyle->GetTextColor()));
			SetBold(fontObj->IsBold());
			SetItalic(fontObj->IsItalic());
			SetUnderline(fontObj->IsUnderline());
		}
		
	}
	SetText(m_strText.c_str());
}

void CMultiLineEditUI::Render(IRenderDC* pRenderDC, RECT &rcPaint)
{
	__super::Render(pRenderDC, rcPaint);

	RECT rect = GetInterRect();
    if(!GetEditStyle())
    {
        return;
    }

	UINT uFlags = GetEditStyle()->GetAlign();
	if (GetTextLength() <= 0 && !m_strEmptyTipsText.empty() && !IsFocused())
	{
		if (GetEmptyEditStyle() != NULL)
		{
			uFlags = GetEmptyEditStyle()->GetAlign();
			pRenderDC->DrawText(GetEmptyEditStyle()->GetFontObj(), rect, m_strEmptyTipsText.c_str(), GetEmptyEditStyle()->GetTextColor(), GetEmptyEditStyle()->GetAlign());
		}
		return;
	}
}


bool CMultiLineEditUI::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_DPI_CHANGED:
	{
		SetFontSize(GetFontSize());
	}
	break;
	}
	return __super::Event(event);
}

TextStyle* CMultiLineEditUI::GetEditStyle()
{
	if (!m_pTextStyle)
	{
		m_pTextStyle = CControlUI::GetTextStyle(_T("txt"));
	}
	return m_pTextStyle;
}

TextStyle* CMultiLineEditUI::GetEmptyEditStyle()
{
	if (!m_pEmptyTextStyle)
	{
		m_pEmptyTextStyle = CControlUI::GetTextStyle(_T("txt_empty"));
		if (m_pEmptyTextStyle)
		{
			m_pEmptyTextStyle->SetAttribute(_T("singleline"), _T("0"));
			m_pEmptyTextStyle->SetAttribute(_T("valign"), _T("top"));
		}
	}
	return m_pEmptyTextStyle;
}



#ifndef DIRECTUI_LAYOUT_RTL
///////////////////////////////////////////////////////////////////////
#define COLOR_INPUT_BK		RGB(255, 255, 255)
#define COLOR_SELECT_TEXT	RGB(255, 255, 255)
#define COLOR_SELECT_BK		RGB(55, 104, 199)
#define COLOR_READONLY_BK	RGB(245, 245, 245)
#define COLOR_EMPTY_TEXT	RGB(128, 128 ,128)


inline int GetStringLine(HDC hDC, const TCHAR *lpszText, int nCount)  
{
	assert(hDC);
	SIZE size = {0, 0};
	if (hDC && lpszText!=NULL)
	{
		::GetTextExtentPoint32(hDC, lpszText, nCount, &size);
	}
	return size.cx;
}

inline int GetStringLine(HDC hDC, const TCHAR *lpszText) //统计字符串的长度(像素个数)
{
	assert(hDC);
	SIZE size = {0, 0};
	if (hDC && lpszText!=NULL)
	{
		::GetTextExtentPoint32(hDC, lpszText, _tcslen(lpszText), &size);
	}
	return size.cx;
}

namespace 
{
#ifdef UNICODE
	const TCHAR crlf[] = _T("\r\n");
	const int crlf_Count = 2;
#else
	const TCHAR crlf = _T("\n");
	const int crlf_Count = 1;
#endif

	inline tstring Mid(const tstring &str, unsigned index)
	{
		unsigned nSize=str.length();
		if (nSize<index) index=nSize;
		return tstring(str.begin()+index, str.end());
	}

	inline tstring Mid(const tstring &str, unsigned startPos, unsigned nCount)
	{
		unsigned nSize=str.length();
		if (nSize < startPos) return tstring(TEXT(""));
		tstring::const_iterator it = str.begin();
		it += startPos;
		unsigned subLast = str.end() - it;
		return subLast < nCount ? tstring(it, str.end()) : tstring(it, it+nCount);
	}

	inline tstring Left(const tstring &str, unsigned index)
	{
		unsigned nSize=str.length();
		if (nSize<index) index=nSize;
		return tstring(str.begin(), str.begin()+index);
	}

	template <class T>
	inline void InsertAt(T &var, unsigned index, typename T::value_type value)
	{
		unsigned nSize = var.size();
		if (nSize<index) index = nSize;
		typename T::iterator it = var.begin();
		it += index;
		var.insert(it, value);
	}

	template <class T>
	inline void RemoveAt(T &var, unsigned index, unsigned nCount)
	{
		unsigned nSize = var.size();
		if (index > nSize) return;
		if (index + nCount > nSize) nCount = nSize - index;
		typename T::iterator itBegin = var.begin() + index;
		typename T::iterator itEnd = itBegin + nCount;
		var.erase(itBegin, itEnd);
	}

	template <class T>
	inline void RemoveAt(T &var, unsigned index)
	{
		unsigned nSize = var.size();
		if (index > nSize) return;
		var.erase(var.begin() + index, var.end());
	}

	template <class T>
	inline unsigned GetUpperBound(const T &val)   
	{
		return val.size() - 1;
	}

	inline void FillSolidRect(HDC hDC, LPRECT lpRect, COLORREF clr)
	{
		::SetBkColor(hDC, clr);
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	}

	inline bool IsChinese(TCHAR ch)//判断是不是中文字符
	{
		if((unsigned short)ch >= 0x0391 && (unsigned short)ch <= 0xFFE5)
			return true;
		return false;
	}

	int CountUTF8Chars(LPCTSTR lpszText, int nLen)    //返回UTF8字节数
	{
		int nCount = 0;
		LPTSTR pStart = const_cast<LPTSTR>(lpszText);
		LPTSTR pNext = pStart;
		LPTSTR pEnd = pStart + nLen;
		while (TRUE)
		{			
			if(IsChinese(*pNext)) nCount += 3;
			else nCount += 1;
			pNext = CharNext(pStart);
			if((pNext == pEnd) || (pNext == pStart)) break;
			pStart = pNext;
		}
		return nCount;
	}

	int CountUTF8Chars(LPCTSTR lpszText, int nCurCounts, int nMaxCounts, int *pAddUTF8Chars)  //返回字符个数
	{
		if (equal_icmp(lpszText, _T("")))
		{
			return 0;
		}
		LPTSTR pStart = const_cast<LPTSTR>(lpszText);
		LPTSTR pNext = pStart;
		int tmp = nCurCounts;
		while (TRUE)
		{
			if(IsChinese(*pNext)) 
			{
				tmp += 3;
				if(tmp > nMaxCounts) 
				{
					tmp -= 3;
					break;
				}
			}
			else
			{
				tmp += 1;
				if(tmp > nMaxCounts) 
				{
					tmp -= 1;
					break;
				}
			}			
			pNext = CharNext(pStart);
			if(*pNext == 0 || pNext == pStart) break;
			pStart = pNext;
		}
		if(pAddUTF8Chars) *pAddUTF8Chars = tmp - nCurCounts;
		return (int)(pNext - lpszText);
	}
}


static int g_key = 0;
static unsigned int encrypt(unsigned int plainText)
{
	if (g_key == 0)
	{
		srand((int)time(0));
		g_key = rand() % 10000;
	}
	unsigned int key = 1313143 + g_key;
	if (plainText == _T('*'))
		return _T('*');
	return plainText ^ key;
}

// 解密算法也可以公开
static unsigned int decrypt(unsigned int cipherText)
{
	if (g_key == 0)
	{
		srand((int)time(0));
		g_key = rand() % 10000;
	}
	unsigned int key = 1313143 + g_key;
	if (cipherText == _T('*'))
		return _T('*');
	return cipherText ^ key;
}

static void encrypt(tstring& plainText)
{
	for (int i = 0; i < plainText.length(); i++)
		plainText[i] = encrypt(plainText[i]);
}

// 解密算法也可以公开
static void decrypt(tstring& cipherText)
{
	for (int i = 0; i < cipherText.length(); i++)
		cipherText[i] = decrypt(cipherText[i]);
}

CEditUI::CEditUI()
{	
	SetStyle(_T("Edit"));
	SetMouseEnabled(true);
	RECT rect = { 4, 4, 4, 4 };
	SetInset(rect);
	SetEnabled(true);
	SetAttribute(_T("cursor"), _T("IBEAM"));
	ModifyFlags(UICONTROLFLAG_SETCURSOR | UICONTROLFLAG_TABSTOP | UICONTROLFLAG_SETFOCUS | UICONTROLFLAG_WANTRETURN, 0);

	/////////////////////////////
	m_bReadOnly = false;
	m_bPassword = false;
	m_bTransparent = false;
	m_bCaretEnable = true;
	m_hToolTipWnd = NULL;
	m_hParentWnd = NULL;

	m_strText = TEXT("");
	m_strPasswdText = TEXT("");
	m_tcPassChar = (TCHAR)0x0025CF;
	m_nPasswdCharWidth = 0;

	m_bInit = false;
	m_nMaxchar = -1;
	m_nMaxUTF8Char = -1;
	m_nCurUTF8Char = 0;
	m_bMaxSize = false;
	m_bAutoSize = false;
	ZeroMemory(&m_cxyMaxSize, sizeof(SIZE));

	m_uSelBegin = 0;
	m_uSelEnd = 0;
	m_uPrevSelEnd = 0;
	m_nPrevSelEndPixel = 0;
	m_nFirstVisiblePixel = 0;
	m_nTextPixel = 0;
	m_nAveCharWidth = 0;
	m_cy = 0;
	m_nLineHeight = 0;

	m_pTextStyle = NULL;
	m_pEmptyTextStyle = NULL;
	m_strEmptyTipsText = _T("");
	m_lpszEmptyTipsKey = NULL;
}

CEditUI::~CEditUI()
{
	if (m_lpszEmptyTipsKey)
	{
		delete[] m_lpszEmptyTipsKey;
		m_lpszEmptyTipsKey = NULL;
	}

	HideToolTip();
	m_bInit = false;
	this->DestroyCaret();
}

void CEditUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::SetAttribute(lpszName, lpszValue);
		return;
	}

	if (equal_icmp(lpszName, _T("readonly")))
	{
		m_bReadOnly = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("password")))
	{
		m_bPassword = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("password_visiblity")))
	{
		m_bPasswordVisiblity = _ttoi(lpszValue) ? true : false;
		if (this->GetWindow())
		{
			tstring strTemp;
			LPCTSTR lpszText = GetDrawText(strTemp);
			m_nTextPixel = GetStringLine(this->GetWindow()->GetPaintDC(), lpszText);
		}
	}
	else if (equal_icmp(lpszName, _T("emptytips")))
	{
		m_strEmptyTipsText = I18NSTR(lpszValue);
		if (lpszValue[0] == _T('#'))
			copy_str(m_lpszEmptyTipsKey, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("transparent")))
	{
		m_bTransparent = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("text")))
	{
		this->SetText(I18NSTR(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("maxchar")))
	{
		m_nMaxchar = _ttoi(lpszValue);
		if(m_nMaxchar < 0) m_nMaxchar = -1;
	}
	else if (equal_icmp(lpszName, _T("maxutf8char")))
	{
		m_nMaxUTF8Char = _ttoi(lpszValue);
		if(m_nMaxUTF8Char < 0) m_nMaxUTF8Char = -1;
	}
	else if (equal_icmp(lpszName, _T("maxwidth")))
	{
		m_bMaxSize = true;
		RECT rcInset = GetInset();
		m_cxyMaxSize.cx = (int)_ttoi(lpszValue);
		m_cxyMaxSize.cx -= rcInset.left + rcInset.right;
	}
	else if (equal_icmp(lpszName, _T("autosize")))
	{
		m_bAutoSize = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("menuenable")))
	{
		m_bMenuEnable = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("caretenable")))
	{
		m_bCaretEnable = (bool)!!_ttoi(lpszValue);
	}
	else
	{
		CControlUI::SetAttribute(lpszName, lpszValue);
	}
}

//void CEditUI::SetReadOnly(bool bReadOnly/*=true*/)
//{
//	m_bReadOnly = bReadOnly;
//}
//
//bool CEditUI::IsReadOnly()
//{
//	return m_bReadOnly;
//}

int CEditUI::GetTextPixel()
{
	return m_nTextPixel;
}

void CEditUI::Init()
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::Init();
		return;
	}

	m_hParentWnd = GetWindow()->GetHWND();

	if (m_hParentWnd != NULL)
	{
		HDC hDC = ::GetDC(m_hParentWnd);
		if (hDC != NULL)
		{
			TextStyle *pTextStyle = GetEditStyle();
			if (pTextStyle != NULL)
			{
				HFONT hOldFont = (HFONT)::SelectObject(hDC, pTextStyle->GetFontObj()->GetFont());
				TEXTMETRIC tm = {0};
				::GetTextMetrics(hDC,&tm);
				m_nLineHeight = tm.tmHeight + tm.tmExternalLeading;
				m_nAveCharWidth = tm.tmAveCharWidth;
				//if (m_bPassword)
				{
					TCHAR szText[10] = {0};
					_stprintf(szText, _T("%c"), m_tcPassChar);
					m_nPasswdCharWidth = GetStringLine(hDC, szText, 1);
				}

				if (m_strText.empty() == false)
				{
					if (m_nMaxchar != -1)
					{
						if ((tstring::size_type)m_nMaxchar < m_strText.size())
						{
							m_strText.erase(m_nMaxchar);
						}
					}
					if (m_nMaxUTF8Char != -1)
					{
						int nAddUTF8Char = 0;
						int nInsertCounts = CountUTF8Chars(m_strText.c_str(), m_nCurUTF8Char, m_nMaxUTF8Char, &nAddUTF8Char);
						if ((tstring::size_type)nInsertCounts < m_strText.size())
						{
							m_strText.erase(nInsertCounts);
						}
						m_nCurUTF8Char = nAddUTF8Char;
					}
					int nSize = m_strText.size();
					m_uSelEnd = nSize;
					int iLen = GetStringLine(hDC, m_strText.c_str());
					m_nTextPixel = iLen;
					m_uSelEnd = 0;
					m_uSelBegin = m_uSelEnd;
					m_uPrevSelEnd = 0;
					m_nPrevSelEndPixel = 0;
					m_nFirstVisiblePixel = 0;
					if(m_bAutoSize)
					{
						m_rcPrevItem = m_rcItem;
						RECT rcInset = GetInset();
						DPI_SCALE(rcInset);
						if (m_nTextPixel < DPI_SCALE(m_cxyMaxSize.cx))
							m_rcItem.right = m_rcItem.left + rcInset.left + m_nTextPixel + rcInset.right;
						else 
							m_rcItem.right = m_rcItem.left + rcInset.left + DPI_SCALE(m_cxyMaxSize.cx) + rcInset.right;
						this->SetStyleObjRect(m_rcItem);
					}
				}
				::SelectObject(hDC, hOldFont);
			}
			::ReleaseDC(m_hParentWnd, hDC);
		}
	}
	m_bInit = true;
}


void CEditUI::CreateCaret()
{
	POINT point = Index2Point(m_uSelEnd);
	RECT rc = this->GetInterRect();
	point.x += rc.left;
	point.y += rc.top;

	int iCaretHeight = m_nLineHeight;
	CControlUI *pParent = this->GetParent();
	if (pParent != NULL)
	{
		RECT rcParent = pParent->GetInterRect();
		if (point.y < rcParent.top)
		{
			iCaretHeight = m_nLineHeight - (rcParent.top-point.y);
			point.y = rcParent.top;
		}
		if (point.y+m_nLineHeight > rcParent.bottom)
		{
			iCaretHeight = rcParent.bottom - point.y;
		}
	}
	CCaretUI::GetInstance()->CreateCaret(GetWindow(), NULL, 1, iCaretHeight);
	SetCaretPos();
	if (m_bCaretEnable)
		CCaretUI::GetInstance()->ShowCaret();
}

void CEditUI::SetCaretPos()
{
	if (!IsFocused())
		return;
	RECT rc = this->GetInterRect();
	POINT point = Index2Point(m_uSelEnd);
	
	point.x += rc.left;
	point.y += rc.top;
	CControlUI *pParent = this->GetParent();
	if (pParent != NULL)
	{
		RECT rcParent = pParent->GetInterRect();
		if (point.y < rcParent.top)
		{
			point.y = rcParent.top;
		}
	}
	if (point.x>=rc.left && point.x<=rc.right)
	{
		CCaretUI::GetInstance()->SetCaretPos(point.x, point.y);
	}
}

void CEditUI::DestroyCaret()
{
	CCaretUI::GetInstance()->DestroyCaret(GetWindow());
}

//
//TextStyle* CEditUI::GetEditStyle()
//{
//	if (!m_pTextStyle)
//		m_pTextStyle = CControlUI::GetTextStyle(_T("txt"));
//	return m_pTextStyle;
//}
//
//TextStyle* CEditUI::GetEmptyEditStyle()
//{
//	if (!m_pEmptyTextStyle)
//		m_pEmptyTextStyle = CControlUI::GetTextStyle(_T("txt_empty"));
//	return m_pEmptyTextStyle;
//}

void  CEditUI::SetText(LPCTSTR lpszText)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::SetText(lpszText);
		return;
	}

	if (!lpszText)
	{
		return;
	}

	//如果是密码需要进行加密
	tstring strPwd;
	if (this->m_bPassword)
	{
		strPwd = lpszText;
		encrypt(strPwd);
		lpszText = strPwd.c_str();
	}

	bool bChange = !!_tcscmp(m_strText.c_str(), lpszText);
	if (m_bInit)
	{
		m_strText.erase(0);
		m_strPasswdText.erase(0);
		m_nTextPixel = 0;
		m_uSelEnd = 0;
		m_uSelBegin = m_uSelEnd;
		m_nCurUTF8Char = 0;
		if (!m_hParentWnd)
		{
			this->Init();
		}
		int result = Insert(lpszText);
		m_uSelEnd = 0;
		m_uSelBegin = m_uSelEnd;
		m_uPrevSelEnd = 0;
		m_nPrevSelEndPixel = 0;
		m_nFirstVisiblePixel = 0;
		if (!m_bReadOnly)
		{
			SetPrevSelEndInfo(result);
		}
		if (bChange)
			this->SendNotify(UINOTIFY_EN_CHANGE);
		this->Invalidate();
	}
	else
	{
		//若编辑框控件还未初始化，那么将设置信息推迟到init初始化中完成
		LPTSTR pStart = const_cast<LPTSTR>(lpszText);
		LPTSTR pEnd;
		pEnd = _tcsstr(pStart, crlf);
		if(pEnd) *pEnd = (TCHAR)0;
		m_strText = pStart;
		if(pEnd) *pEnd = crlf[0];
		m_nCurUTF8Char = 0;
		// 		this->SendNotify(UINOTIFY_EN_CHANGE);
		// 		this->Invalidate();
	}
}

tstring CEditUI::GetPasswordText()
{
	if (GetUIEngine()->IsRTLLayout())
	{
		return __super::GetPasswordText();
	}

	tstring str = m_strText;
	decrypt(str);
	return str;
}

void CEditUI::SetTextWithoutNotify(LPCTSTR lpszText)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::SetTextWithoutNotify(lpszText);
		return;
	}

	if(!lpszText)
	{
		return;
	}
	if(m_bInit)
	{
		m_strText.erase(0);
		m_strPasswdText.erase(0);
		m_nTextPixel = 0;
		m_uSelEnd = 0;
		m_uSelBegin = m_uSelEnd;
		m_nCurUTF8Char = 0;
		if(!m_hParentWnd)
		{
			this->Init();
		}
		int result = Insert(lpszText);
		m_uSelEnd = 0;
		m_uSelBegin = m_uSelEnd;
		m_uPrevSelEnd = 0;
		m_nPrevSelEndPixel = 0;
		m_nFirstVisiblePixel = 0;
		if(!m_bReadOnly)
		{
			SetPrevSelEndInfo(result);
		}
		this->Invalidate();
	}
	else
	{
		LPTSTR pStart = const_cast<LPTSTR>(lpszText);
		LPTSTR pEnd;
		pEnd = _tcsstr(pStart, crlf);
		if(pEnd) *pEnd = (TCHAR)0;
		m_strText = pStart;
		if(pEnd) *pEnd = crlf[0];
		m_nCurUTF8Char = 0;
	}
}

void CEditUI::SetPrevSelEndInfo(int offsetPixels)
{
	if (offsetPixels==0 || m_uPrevSelEnd==m_uSelEnd)
	{
		return;
	}
	if (m_uPrevSelEnd < m_uSelEnd)
	{
		m_nPrevSelEndPixel += offsetPixels;
		m_uPrevSelEnd = m_uSelEnd;
	}
	else if (m_uPrevSelEnd > m_uSelEnd)
	{
		m_nPrevSelEndPixel -= offsetPixels;
		m_uPrevSelEnd = m_uSelEnd;
	}
}

void CEditUI::SetPrevSelEndInfo()
{
	if(m_uSelEnd != m_uPrevSelEnd)
	{
		if (!m_hParentWnd) return;
		HDC hDC = ::GetDC(m_hParentWnd);
		if (!hDC || !GetEditStyle()) return;
		HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());
		tstring strTemp;
		LPCTSTR lpszText = GetDrawText(strTemp);
		if (m_uSelEnd < m_uPrevSelEnd)
		{
			m_nPrevSelEndPixel -= GetStringLine(hDC, lpszText+m_uSelEnd, (int)(m_uPrevSelEnd-m_uSelEnd));
			m_uPrevSelEnd = m_uSelEnd;
		}
		else if (m_uSelEnd > m_uPrevSelEnd)
		{
			m_nPrevSelEndPixel += GetStringLine(hDC, lpszText+m_uPrevSelEnd, (int)(m_uSelEnd-m_uPrevSelEnd));
			m_uPrevSelEnd = m_uSelEnd;
		}

		::SelectObject(hDC, hOldFont);
		::ReleaseDC(m_hParentWnd, hDC);
	}
}

int CEditUI::Insert(LPCTSTR lpszText)
{
	if(!m_hParentWnd) return 0;
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle()) return 0;
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());

	int iLen = 0;
	if(!m_bPassword)//是否为密码控件
	{
		LPTSTR pStart = const_cast<LPTSTR>(lpszText);
		LPTSTR pEnd = _tcsstr(pStart, crlf);
		if(pEnd) *pEnd = (TCHAR)0;
		int nCount = _tcslen(lpszText);
		if(m_nMaxchar != -1)
		{
			int nSize = m_strText.size();
			if(nSize >= m_nMaxchar) goto END;
			int nLen = m_nMaxchar - nSize;
			if (nLen >= nCount) nLen = nCount;
			if(!nLen) goto END;
			nCount = nLen;
		}
		if(m_nMaxUTF8Char != -1)
		{
			int nAddUTF8Chars = 0;
			int nInsertCounts = CountUTF8Chars(lpszText, m_nCurUTF8Char, m_nMaxUTF8Char, &nAddUTF8Chars);

			if(m_nMaxchar != -1)
			{
				int nAddUTF8Chars1 = CountUTF8Chars(lpszText, nCount);
				if(nAddUTF8Chars1 < nAddUTF8Chars)
				{
					m_nCurUTF8Char += nAddUTF8Chars1;
				}
				else
				{
					m_nCurUTF8Char += nAddUTF8Chars;
					nCount = nInsertCounts;
				}
			}
			else
			{
				m_nCurUTF8Char += nAddUTF8Chars;
				nCount = nInsertCounts;
			}
			if(!nCount) goto END;
		}
		m_strText.insert(m_uSelEnd, lpszText, nCount);
		m_uSelEnd += nCount;
		iLen = GetStringLine(hDC, lpszText, nCount);//计算要插入字符串的实际像素大小
		m_nTextPixel += iLen;  //将长度累加到总长度中
		m_uSelBegin = m_uSelEnd;
		if(pEnd) *pEnd = crlf[0];
	}
	else
	{
		LPTSTR pStart = const_cast<LPTSTR>(lpszText);
		LPTSTR pEnd = _tcsstr(pStart, crlf);
		if(pEnd) *pEnd = (TCHAR)0;
		int nCount = _tcslen(lpszText);
		if(m_nMaxchar != -1)
		{
			int nSize = m_strText.size();
			if(nSize >= m_nMaxchar) goto END;
			int nLen = m_nMaxchar - nSize;
			if (nLen >= nCount) nLen = nCount;
			if(!nLen) goto END;
			nCount = nLen;
		}
		if(m_nMaxUTF8Char != -1)
		{
			int nAddUTF8Chars = 0;
			int nInsertCounts = CountUTF8Chars(lpszText, m_nCurUTF8Char, m_nMaxUTF8Char, &nAddUTF8Chars);

			if(m_nMaxchar != -1)
			{
				int nAddUTF8Chars1 = CountUTF8Chars(lpszText, nCount);
				if(nAddUTF8Chars1 < nAddUTF8Chars)
				{
					m_nCurUTF8Char += nAddUTF8Chars1;
				}
				else
				{
					m_nCurUTF8Char += nAddUTF8Chars;
					nCount = nInsertCounts;
				}
			}
			else
			{
				m_nCurUTF8Char += nAddUTF8Chars;
				nCount = nInsertCounts;
			}		
		}
		m_strText.insert(m_uSelEnd, lpszText, nCount);
		m_strPasswdText.insert(m_uSelEnd, nCount, m_tcPassChar);
		m_uSelEnd += nCount;

		if (!m_bPasswordVisiblity)
		{
			iLen = m_nPasswdCharWidth * nCount;
		}
		else
		{
			tstring strDecrypt = lpszText;
			decrypt(strDecrypt);
			iLen = GetStringLine(hDC, strDecrypt.c_str(), nCount);//计算要插入字符串的实际像素大小
		}

		m_nTextPixel += iLen;
		m_uSelBegin = m_uSelEnd;
		if(pEnd) *pEnd = crlf[0];
	}
	if(m_bAutoSize)    //如果是可变大小编辑框控件，则重设编辑框大小
	{
		Invalidate();
		m_rcPrevItem = m_rcItem;
		RECT rcInset = GetInset();
		DPI_SCALE(rcInset);
		if (m_nTextPixel < DPI_SCALE(m_cxyMaxSize.cx))
			m_rcItem.right = m_rcItem.left + rcInset.left + m_nTextPixel + rcInset.right;
		else 
			m_rcItem.right = m_rcItem.left + rcInset.left + DPI_SCALE(m_cxyMaxSize.cx) + rcInset.right;

		this->SetStyleObjRect(m_rcItem);
	}
END:
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);
	return iLen;
}

int CEditUI::DelSelText()
{
	int iPixels = 0;

	if (m_uSelBegin != m_uSelEnd)
	{
		if (!m_hParentWnd) return FALSE;
		HDC hDC = ::GetDC(m_hParentWnd);
		if (!hDC || !GetEditStyle()) return FALSE;
		HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());

		if(!m_bPassword)
		{
			unsigned selBegin = min(m_uSelBegin, m_uSelEnd);
			unsigned selEnd = max(m_uSelBegin, m_uSelEnd);
			unsigned nCount = selEnd - selBegin;
			if(m_nMaxUTF8Char != -1)
			{
				int nDelUTF8Chars = CountUTF8Chars(m_strText.c_str()+selBegin, nCount);
				m_nCurUTF8Char -= nDelUTF8Chars;
			}
			int iLen = GetStringLine(hDC, m_strText.c_str()+selBegin, (unsigned)nCount);
			RemoveAt(m_strText, selBegin, (unsigned)nCount);
			m_uSelEnd = selBegin;
			m_uSelBegin = m_uSelEnd;
			m_nTextPixel -= iLen;
			iPixels = iLen;
		}
		else
		{
			unsigned selBegin = min(m_uSelBegin, m_uSelEnd);
			unsigned selEnd = max(m_uSelBegin, m_uSelEnd);
			int nCount = selEnd - selBegin;
			if(m_nMaxUTF8Char != -1)
			{
				int nDelUTF8Chars = CountUTF8Chars(m_strText.c_str()+selBegin, nCount);
				m_nCurUTF8Char -= nDelUTF8Chars;
			}

			tstring strTemp;
			LPCTSTR lpszText = GetDrawText(strTemp);
			int iLen = GetStringLine(hDC, lpszText + selBegin, (unsigned)nCount);
			RemoveAt(m_strText, selBegin, nCount);
			RemoveAt(m_strPasswdText, selBegin, nCount);

			m_uSelEnd = selBegin;
			m_uSelBegin = m_uSelEnd;
			m_nTextPixel -= iLen;
			iPixels = iLen;
		}
		if(m_bAutoSize)
		{
			m_rcPrevItem = m_rcItem;
			RECT rcInset = GetInset();
			DPI_SCALE(rcInset);
			if (m_nTextPixel < DPI_SCALE(m_cxyMaxSize.cx))
				m_rcItem.right = m_rcItem.left + rcInset.left + m_nTextPixel + rcInset.right;
			else 
				m_rcItem.right = m_rcItem.left + rcInset.left + DPI_SCALE(m_cxyMaxSize.cx) + rcInset.right;

			this->SetStyleObjRect(m_rcItem);
		}

		this->SendNotify(UINOTIFY_EN_CHANGE);

		::SelectObject(hDC, hOldFont);
		::ReleaseDC(m_hParentWnd, hDC);
	}

	return iPixels;
}

BOOL CEditUI::EnsureSelEndVisible()   //重新调整m_nFirstVisiblePixel大小，以使光标位置可见
{
	BOOL result = FALSE;

	RECT rcEdit = this->GetInterRect();
	int iEditWidth = rcEdit.right - rcEdit.left;

	if(m_nTextPixel < iEditWidth+1)
	{
		m_nFirstVisiblePixel = 0;
		return TRUE;
	}
	int offsetLen = iEditWidth / 3;  //编辑框宽度的3分之一 
	//当光标从编辑框左边界/右边界时若一次操作(向左/向右)使光标不在编辑框内
	//那么要使文本往前/后编辑框宽度的3分之一大小像素个字符,具体情况可以使用记事本参考

	POINT point = Index2Point(m_uSelEnd);  //计算出当前光标的实际在编辑框中的位置

	if (!m_hParentWnd) return FALSE;
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle())
	{
		return FALSE;
	}
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());

	if(!m_bAutoSize)   ///非可变宽度编辑框控件
	{
		if (offsetLen <= 0)
			goto END;
		if (point.x < 0)//若计算出的实际位置小于0也就是不在编辑框内
		{
			if (m_nFirstVisiblePixel < offsetLen)
				m_nFirstVisiblePixel = 0;
			else if ((-point.x) > offsetLen)
			{
				m_nFirstVisiblePixel += point.x;
				if (m_nFirstVisiblePixel > offsetLen)
					m_nFirstVisiblePixel += -offsetLen;
				else
					m_nFirstVisiblePixel = 0;
			}
			else
				m_nFirstVisiblePixel += -offsetLen;
			result = TRUE;
		}
		else if (point.x == 0)
		{
			goto END;
		}
		else if (point.x + m_nAveCharWidth > iEditWidth)//要给用户一个良好的使用习惯在当前光标还未超出编辑框右边界进行判断
		{																			
			if (m_uSelEnd == m_strText.size())            
			{
				m_nFirstVisiblePixel += point.x + m_nAveCharWidth - iEditWidth;
			}
			else
			{
				int iLen = (m_strText.size() - m_uSelEnd) * m_nAveCharWidth;
				if(iLen <= offsetLen)
					m_nFirstVisiblePixel += point.x+ iLen - iEditWidth;
				else
					m_nFirstVisiblePixel += point.x+ offsetLen - iEditWidth;
			}
			result = TRUE;
		}
	}
	else
	{
		if (m_nTextPixel<DPI_SCALE(m_cxyMaxSize.cx) && m_rcPrevItem.right<m_rcItem.right)
		{
			m_nFirstVisiblePixel = 0;
			result = TRUE;
		}
		else if (m_nTextPixel<DPI_SCALE(m_cxyMaxSize.cx) && m_rcPrevItem.right>m_rcItem.right)
		{
			if (point.x < 0)
			{
				m_nFirstVisiblePixel = 0;
				result = TRUE;
			}
		}
		else if (m_nTextPixel > DPI_SCALE(m_cxyMaxSize.cx))
		{
			if (point.x + m_nAveCharWidth > iEditWidth)
			{
				if (m_uSelEnd == m_strText.size())
				{
					m_nFirstVisiblePixel += point.x + m_nAveCharWidth - iEditWidth;
				}
				else
				{
					int iLen = (m_strText.size() - m_uSelEnd) * m_nAveCharWidth;
					if(iLen <= offsetLen)
						m_nFirstVisiblePixel += point.x+ iLen - iEditWidth;
					else
						m_nFirstVisiblePixel += point.x+ offsetLen - iEditWidth;
				}
				result = TRUE;
			}
			else if (point.x < 0)
			{
				if (m_nFirstVisiblePixel < offsetLen)
					m_nFirstVisiblePixel = 0;
				else if ((-point.x) > offsetLen)
				{
					m_nFirstVisiblePixel += point.x;
					if (m_nFirstVisiblePixel > offsetLen)
						m_nFirstVisiblePixel += -offsetLen;
					else
						m_nFirstVisiblePixel = 0;
				}
				else
					m_nFirstVisiblePixel += -offsetLen;
				result = TRUE;
			}
		}
	}
END:
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);

	return result;
}

POINT CEditUI::Index2Point(int index)
{
	POINT pt = {0};
	if(index > m_strText.size())
	{
		return pt;
	}
	if (!m_hParentWnd)
	{
		return pt;
	}
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle())
	{
		return pt;
	}
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());

	pt.x = GetCharsPixels(hDC, index) - m_nFirstVisiblePixel;

	RECT rcEdit = this->GetInterRect();
	int iEditWidth = rcEdit.right-rcEdit.left;
	if (m_nTextPixel < iEditWidth)
	{
		UINT uAlignFlag = GetEditStyle()->GetAlign();
		int x = pt.x;
		if (uAlignFlag & DT_RIGHT)
			x = pt.x + (iEditWidth-m_nTextPixel);
		else if (uAlignFlag & DT_CENTER)
			x = pt.x + (int)((iEditWidth-m_nTextPixel)/2);
		else
			x = pt.x;
		pt.x = x;
	}

	pt.y = m_cy;
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);

	return pt;
}

int CEditUI::GetCharsPixels(HDC hDC, int nCounts)
{
	int prevSelIndex = m_uPrevSelEnd;
	int iLen = m_nPrevSelEndPixel;

	tstring strTemp;
	LPCTSTR lpszText = GetDrawText(strTemp);
	if (prevSelIndex < nCounts)
	{
		iLen += GetStringLine(hDC, lpszText + prevSelIndex, (int)(nCounts - prevSelIndex));
	}
	else if (prevSelIndex > nCounts)
	{
		iLen -= GetStringLine(hDC, lpszText + nCounts, (int)(prevSelIndex - nCounts));
	}

	return iLen;
}

int CEditUI::Point2Index(LPPOINT ppt, bool bAdjust/*=false*/)
{
	int index = 0;
	if (!m_hParentWnd)
	{
		return index;
	}
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle())
	{
		return index;
	}
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());

	RECT rcEdit = this->GetInterRect();
	int iEditWidth = rcEdit.right-rcEdit.left;
	if (m_nTextPixel < iEditWidth)
	{
		UINT uAlignFlag = GetEditStyle()->GetAlign();
		int x = ppt->x;
		if (uAlignFlag & DT_RIGHT)
			x = ppt->x - (iEditWidth-m_nTextPixel); 
		else if (uAlignFlag & DT_CENTER)
			x = ppt->x - (int)((iEditWidth-m_nTextPixel)/2);
		else
				x = ppt->x;

		ppt->x = x;
	}

	if (m_nTextPixel <= (ppt->x+m_nFirstVisiblePixel)) 
	{
		ppt->x = m_nTextPixel - m_nFirstVisiblePixel;
	}
	int iLen = ppt->x + m_nFirstVisiblePixel;
	if (iLen < 0) iLen = 0; 
	index = GetCharsCounts(hDC, iLen, bAdjust);
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);

	tstring strTemp;
	LPCTSTR lpszText = GetDrawText(strTemp);

	int nStrLen = _tcslen(lpszText);
	if (index > nStrLen)
	{
		index = nStrLen;
	}

	return index;
}

int CEditUI::GetCharsCounts(HDC hDC, int nPixels, bool bAdjust/*=false*/)
{
	//assert(m_uPrevSelEnd >= 0);
	int nCounts = m_uPrevSelEnd;
	int iLen = m_nPrevSelEndPixel;

	tstring strTemp;
	LPCTSTR lpszText = GetDrawText(strTemp);
	while (TRUE)
	{
		if (iLen < nPixels)
		{
			int tmp1 = abs(iLen-nPixels);		
			iLen += GetStringLine(hDC, lpszText + (nCounts++), 1);
			if(iLen > nPixels)
			{
				RECT rcEdit = this->GetInterRect();
				int tmp2 = abs(iLen-nPixels);
				if (tmp1<tmp2 || ((iLen-m_nFirstVisiblePixel)>=(rcEdit.right-rcEdit.left) && bAdjust) )
					--nCounts;
				break;
			}
			if (nCounts > m_strText.size())
				break;
		}
		else if(iLen == nPixels)
			break;
		else if(iLen > nPixels)
		{
			int tmp1 = abs(iLen-nPixels);
			iLen -= GetStringLine(hDC, lpszText + (--nCounts), 1);
			if(iLen < nPixels)
			{
				int tmp2 = abs(iLen-nPixels);
				if (tmp1<tmp2 || (bAdjust && iLen<m_nFirstVisiblePixel))
					++nCounts;
				break;
			}
			if (nCounts <= 0)
				break;
		}
	}

	return nCounts;
}

int CEditUI::GetLeftClientWidthIndex(HDC hDC, int nPos) 
{	
	int index = nPos;
	int nCountPixels = 0;
	RECT rcClient = this->GetInterRect();
	int nClientWidth = rcClient.right - rcClient.left;
	tstring strTemp;
	LPCTSTR lpszText = GetDrawText(strTemp);
	while (--index > 0)
	{
		int iLen = GetStringLine(hDC, lpszText+index, 1);
		nCountPixels += iLen;
		if (nCountPixels >= nClientWidth)
		{
			break;
		}
	}
	index = index < 0 ? 0 : index;
	return index;
}

int CEditUI::GetRightClientWidthIndex(HDC hDC, int nPos) 
{	
	int index = nPos;
	int nCountPixels = 0;
	RECT rcClient = this->GetInterRect();
	int nClientWidth = rcClient.right - rcClient.left;
	tstring strTemp;
	LPCTSTR lpszText = GetDrawText(strTemp);
	int nText = m_strText.size();
	while (++index < nText)
	{
		int iLen = GetStringLine(hDC, lpszText+index-1, 1);
		nCountPixels += iLen;
		if (nCountPixels >= nClientWidth)
		{
			break;
		}
	}
	index = index > nText ? nText : index;
	return index;
}

void CEditUI::Render(IRenderDC* pRenderDC, RECT &rcPaint)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		 __super::Render(pRenderDC, rcPaint);
		return;
	}

	TextStyle *pTextStyle = GetEditStyle();
	if (pTextStyle == NULL)
	{
		return;
	}
	if (!m_bTransparent)
	{
		CLabelUI::Render(pRenderDC, rcPaint);
	}

	HDC hDestDC = pRenderDC->GetDC();
	ClipObj clip;
	ClipObj::GenerateClipWithAnd(hDestDC, GetInterRect(), clip);

	int oldMode = ::SetBkMode(hDestDC, TRANSPARENT);
 	HFONT hOldFont = (HFONT)::SelectObject(hDestDC, pTextStyle->GetFontObj()->GetFont());
	if (m_bReadOnly && !m_bTransparent)
	{
		HBRUSH hBr = CreateSolidBrush(COLOR_READONLY_BK);
		FillRect(hDestDC, &GetInterRect(), hBr);
		::DeleteObject(hBr);
	}

 	if (!IsFocused())
 	{
 		if (m_strText.empty() && !m_strEmptyTipsText.empty())
 		{
			TextStyle *pEmptyEditTextStyle = GetEmptyEditStyle();
			if (pEmptyEditTextStyle != NULL)
			{
				RECT rect = GetInterRect();
				UINT align = pTextStyle->GetAlign();
				pRenderDC->DrawText(pEmptyEditTextStyle->GetFontObj(), rect, m_strEmptyTipsText.c_str(), pEmptyEditTextStyle->GetTextColor(), align, pEmptyEditTextStyle->GetFontEffect());
			}
 		}
 		else
 		{
 			RECT rect = GetInterRect();
 			UINT align = pTextStyle->GetAlign();
			tstring strDecrypt;
			LPCTSTR lpszText = GetDrawText(strDecrypt);
 			pRenderDC->DrawText(pTextStyle->GetFontObj(), rect, lpszText, pTextStyle->GetTextColor(), align, pTextStyle->GetFontEffect(), pTextStyle->GetFontType());
 		}
 	}
 	else if (!m_strText.empty())
	{
		unsigned iBegin = GetLeftClientWidthIndex(hDestDC, m_uSelEnd);
		unsigned iEnd = GetRightClientWidthIndex(hDestDC, m_uSelEnd);
		unsigned selBegin = min(m_uSelBegin, m_uSelEnd);
		unsigned selEnd = max(m_uSelBegin, m_uSelEnd);
		selBegin = max(selBegin, iBegin);
		selEnd = min(selEnd, iEnd);

		COLORREF oldTextColor = ::SetTextColor(hDestDC, DUI_ARGB2RGB(pTextStyle->GetTextColor()));
		COLORREF oldBkColor = ::SetBkColor(hDestDC, COLOR_INPUT_BK);
		this->DrawRange(pRenderDC, iBegin, selBegin);
		::SetBkMode(hDestDC, OPAQUE);
		::SetBkColor(hDestDC, COLOR_SELECT_BK);
		::SetTextColor(hDestDC, COLOR_SELECT_TEXT);
		this->DrawRange(pRenderDC, selBegin, selEnd);
		::SetBkMode(hDestDC, TRANSPARENT);
		::SetBkColor(hDestDC, COLOR_INPUT_BK);
		::SetTextColor(hDestDC, DUI_ARGB2RGB(pTextStyle->GetTextColor()));
		this->DrawRange(pRenderDC, selEnd, iEnd);

		::SetTextColor(hDestDC, oldTextColor);
		::SetBkColor(hDestDC, oldBkColor);
	}
	if (IsFocused() && m_bCaretEnable)
	{
		CCaretUI::GetInstance()->Render(pRenderDC, rcPaint);
	}

	::SetBkMode(hDestDC, oldMode);
	::SelectObject(hDestDC, hOldFont);
}

bool CEditUI::Event(TEventUI& event)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		return __super::Event(event);
	}

	switch(event.nType)
	{
	case UIEVENT_SCROLLWHEEL:
		{
			if(GetParent())
				GetParent()->SendEvent(event);
		}
		break;
	case UIEVENT_SETCURSOR:
		{
			if (PtInRect(&GetInterRect(), event.ptMouse))
			{			
				return CLabelUI::Event(event);
			}
		}
		break;
	case UIEVENT_SETFOCUS:
		{	
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				//SendMessage(hWnd, WM_IME_NOTIFY, (WPARAM)IMN_CLOSECANDIDATE, (LPARAM)1);
				ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}

			m_bFocused = true;
			if (GetWindow()->GetEventClickControl() == this)
			{
				m_uSelBegin = m_uSelEnd = 0;
				m_nFirstVisiblePixel = 0;
				m_uPrevSelEnd = 0;
				m_nPrevSelEndPixel = 0;
			}
			else
			{
				tstring strDecrypt;
				LPCTSTR lpszText = GetDrawText(strDecrypt);
				m_uSelEnd = _tcslen(lpszText);
				m_uSelBegin = m_uSelEnd;
				this->SetPrevSelEndInfo();
				EnsureSelEndVisible();
			}			
			if (GetState() != UISTATE_OVER)
				SetState(UISTATE_OVER);
			OnSetFocus();
			Invalidate(true);
			this->SendNotify(UINOTIFY_SETFOCUS, event.wParam, event.lParam);

		/*	this->SetCaretPos();*/
		}
		break;
	
	case UIEVENT_KILLFOCUS:
		{
			HideToolTip();
			m_bFocused = false;
			SetState(UISTATE_NORMAL);
			m_uSelEnd = m_uSelBegin = 0;
			m_uPrevSelEnd = 0;
			m_nPrevSelEndPixel = 0;
			m_nFirstVisiblePixel = 0;
			EnsureSelEndVisible();
			OnKillFocus();
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				//SendMessage(hWnd, WM_IME_NOTIFY, (WPARAM)IMN_CLOSECANDIDATE, (LPARAM)1);
				ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}
			Invalidate(true);
			this->SendNotify(UINOTIFY_KILLFOCUS, event.wParam,event.lParam);//modify by hanzp.用于传出当前焦点
		}
		break;
	case UIEVENT_MOUSEENTER:
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
			if(!IsFocused())
			{
				SetState(UISTATE_NORMAL);
				this->Invalidate();
			}
		}
		break;
	case UIEVENT_KEYDOWN:
		{
			OnKeyDown(event.wParam, event.lParam);
			this->SetCaretPos();
		}
		break;
	case UIEVENT_IME_START:
		{
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				if (GetEditStyle())
				{
					POINT pt;
					GetCaretPos(&pt);
					COMPOSITIONFORM form;
					ImmGetCompositionWindow(hImc, &form);
					form.dwStyle = CFS_POINT;
					form.ptCurrentPos.x = pt.x;
					form.ptCurrentPos.y = pt.y;
					ImmSetCompositionWindow(hImc, &form);
					LOGFONT lf;
					GetEditStyle()->GetFontObj()->GetLogFont(&lf);
					::ImmSetCompositionFont(hImc, &lf);
				}
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}
		}
		break;
	case UIEVENT_IME_ING:
		{
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				POINT pt;
				GetCaretPos(&pt);
				COMPOSITIONFORM form;
				ImmGetCompositionWindow(hImc, &form);
				form.dwStyle = CFS_POINT;
				form.ptCurrentPos.x = pt.x;
				form.ptCurrentPos.y = pt.y;
				ImmSetCompositionWindow(hImc, &form);
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}
		}
		break;
	case UIEVENT_CHAR:
		{
			if(m_bReadOnly) break;
			OnChar(event.wParam, event.lParam);
			this->SetCaretPos();
		}
		break;
	case UIEVENT_RBUTTONUP:
		{
			POINT pt = event.ptMouse;
			RECT rcClient = this->GetInterRect();
			if (PtInRect(&rcClient, pt))
			{
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnRButtonUp(event.wParam, event.lParam, &pt);
			}
		}
		break;
	case UIEVENT_BUTTONDOWN:
		{
			HIMC hImc = ImmGetContext(GetWindow()->GetHWND());
			if(hImc)
			{
				//SendMessage(hWnd, WM_IME_NOTIFY, (WPARAM)IMN_CLOSECANDIDATE, (LPARAM)1);
				ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				ImmReleaseContext(GetWindow()->GetHWND(), hImc);
			}

			POINT pt = event.ptMouse;
			RECT rcClient = this->GetInterRect();
			if (PtInRect(&rcClient, pt))
			{
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnLButtonDown(event.wParam, event.lParam, &pt);
			}
			else
			{
				if (pt.x < rcClient.left) pt.x = rcClient.left;
				if (pt.x > rcClient.right) pt.x = rcClient.right;
				if (pt.y < rcClient.top) pt.y = rcClient.top;
				if (pt.y > rcClient.bottom) pt.y = rcClient.bottom;
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnLButtonDown(event.wParam, event.lParam, &pt);	
			}

			this->SetCaretPos();
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			POINT pt = event.ptMouse;
			OnLButtonUp(event.wParam, event.lParam, &pt);
		}
		break;
	case UIEVENT_LDBCLICK:
		{
			POINT pt = event.ptMouse;
			RECT rcClient = this->GetInterRect();
			if (PtInRect(&rcClient, pt))
			{
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnLButtonDblick(event.wParam, event.lParam, &pt);
			}
		}
		break;
	case UIEVENT_MOUSEMOVE:
		{
			POINT pt = event.ptMouse;
			RECT rcClient = this->GetInterRect();
			if (PtInRect(&rcClient, pt))
			{
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnMouseMove(event.wParam, event.lParam, &pt);
			}
			else
			{
				if (pt.x < rcClient.left) pt.x = rcClient.left - m_nAveCharWidth;
				if (pt.x > rcClient.right) pt.x = rcClient.right + m_nAveCharWidth;
				if (pt.y < rcClient.top) pt.y = rcClient.top;
				if (pt.y > rcClient.bottom) pt.y = rcClient.bottom;
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnMouseMove(event.wParam, event.lParam, &pt);	
			}
		}
		break;
	default:
		break;
	}
	return CLabelUI::Event(event);
}

bool CEditUI::OnMenuItemClick(TNotifyUI* pNotify)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		return __super::OnMenuItemClick(pNotify);
	}


	UIMENUINFO* pInfo = (UIMENUINFO*)pNotify->wParam;
	switch (pInfo->uID)
	{
	case IDR_MENU_UIEDIT_CUT:
		{
			this->Copy();
			int result = DelSelText();
			SetPrevSelEndInfo(result);
			RECT rcDraw = {0};
			if(result) rcDraw= this->GetDeleteReDrawRect();
			RECT rcClient = this->GetInterRect();
			OffsetRect(&rcDraw, rcClient.left, rcClient.top);
			GetWindow()->Invalidate(&rcDraw);
		}
		break;
	case IDR_MENU_UIEDIT_COPY:
		{
			this->Copy();
		}
		break;
	case IDR_MENU_UIEDIT_PASTE:
		{
			this->Paste();
			this->EnsureSelEndVisible();
			this->Invalidate();
		}
		break;
	case IDR_MENU_UIEDIT_SELALL:
		{
			m_uSelBegin = 0;
			m_uSelEnd = m_strText.size();
			SetPrevSelEndInfo();
			EnsureSelEndVisible();
			this->Invalidate();
		}
		break;	
	default:
		break;
	}
	return true;
}

void CEditUI::OnSetFocus()
{
	GetWindow()->EnableIME();
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnSetFocus();
		return;
	}

	this->CreateCaret();
}

void CEditUI::OnKillFocus()
{
	GetWindow()->DisableIME();
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnKillFocus();
		return;
	}
	this->DestroyCaret();	
}

void CEditUI::OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnChar(wParam, lParam, ppt);
		return;
	}

	if(m_bReadOnly)
	{
		return;
	}
	UINT nChar = (UINT)wParam;
	if (nChar<VK_SPACE && nChar!=VK_RETURN && nChar!=VK_ESCAPE && nChar!='\t')
	{
		return;
	}
	if (nChar==VK_TAB || nChar==VK_RETURN || nChar==VK_ESCAPE)
	{
		return;
	}
	SHORT value1 = GetKeyState(VK_LCONTROL);
	SHORT value2 = GetKeyState(VK_RCONTROL);
	if ((value1&0x8000) || (value2&0x8000))
	{
		return;
	}

	if (OnLimitChar(nChar))
	{
		return;
	}

	int result = DelSelText();
	SetPrevSelEndInfo(result);
	RECT rcDraw1 = {0};
	if (!m_bAutoSize && result)
	{
		rcDraw1 = this->GetDeleteReDrawRect();
	}
	else if (m_bAutoSize)
	{
		rcDraw1 = m_rcPrevItem; 
	}
	if (this->m_bPassword)
	{
		nChar = encrypt(nChar);
	}

	TCHAR buf[5] = {0};
	_stprintf(buf, _T("%c"), nChar);
	unsigned beforeInsertPos = m_uSelEnd;
	result = Insert(buf);
	SetPrevSelEndInfo(result);
	this->SendNotify(UINOTIFY_EN_CHANGE);
	RECT rcDraw2;
	if (!m_bAutoSize)
	{
		rcDraw2 = this->GetInsertReDrawRect(beforeInsertPos);
	}
	else if(m_bAutoSize)
	{
		rcDraw2 = m_rcPrevItem;
	}
	RECT rcDraw;
	::UnionRect(&rcDraw, &rcDraw1, &rcDraw2);
	BOOL ret = EnsureSelEndVisible();
	if (ret && !m_bAutoSize) 
	{
		this->Invalidate();
	}
	else if (!ret && !m_bAutoSize)
	{
		RECT rcClient = this->GetInterRect();
		OffsetRect(&rcDraw, rcClient.left, rcClient.top);
		GetWindow()->Invalidate(&rcDraw);
	}
	else if (m_bAutoSize)
	{
		RECT rcUnion;
		::UnionRect(&rcUnion, &m_rcItem, &rcDraw);
		GetWindow()->Invalidate(&rcUnion, true);
	}
}

void CEditUI::OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnKeyDown(wParam, lParam, ppt);
		return;
	}

	UINT nChar = (UINT)wParam;
	if (nChar == 'V')
	{
		if(m_bPassword) return;
		if(m_bReadOnly) return;
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
		{
			this->Paste();
			this->SendNotify(UINOTIFY_EN_CHANGE);
		}
		this->EnsureSelEndVisible();
		this->Invalidate();
		return;
	}
	else if (nChar == 'C')
	{
		if(m_bPassword) return;
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
		{
			this->Copy();
		}
		return;
	}
	else if (nChar == 'X')
	{
		if(m_bPassword) return;
		if(m_bReadOnly) return;
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
		{
			this->Copy();
			int result;
			result = DelSelText();
			SetPrevSelEndInfo(result);
			RECT rcDraw = {0};
			if(result) rcDraw= this->GetDeleteReDrawRect();
			RECT rcClient = this->GetInterRect();
			OffsetRect(&rcDraw, rcClient.left, rcClient.top);
			GetWindow()->Invalidate(&rcDraw);
			this->SendNotify(UINOTIFY_EN_CHANGE);
		}
		return;
	}
	else if (nChar == 'A')
	{
		if(m_bPassword) return;
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
		{
			m_uSelBegin = 0;
			m_uSelEnd = m_strText.size();
			SetPrevSelEndInfo();
			EnsureSelEndVisible();
			this->Invalidate();
		}
		return;
	}
	else if (nChar == VK_HOME || nChar == VK_END)
	{
		SHORT value = GetKeyState(VK_SHIFT);
		if (value & 0x8000)
		{
			unsigned beforeMovePos = m_uSelEnd;
			if (nChar == VK_HOME)
			{
				m_uSelBegin = m_uSelEnd;
				m_uSelEnd = 0;
			}
			else
			{
				m_uSelBegin = m_uSelEnd;
				m_uSelEnd = m_strText.size();
			}
			SetPrevSelEndInfo();
			RECT rcDraw = this->GetSelectReDrawRect(beforeMovePos);
			if(EnsureSelEndVisible()) this->Invalidate();
			else 
			{
				RECT rcClient = this->GetInterRect();
				OffsetRect(&rcDraw, rcClient.left, rcClient.top);
				GetWindow()->Invalidate(&rcDraw);
			}
		}
		else
		{
			if (nChar == VK_HOME)
			{
				m_uSelBegin = m_uSelEnd = 0;
				m_uPrevSelEnd = 0;
				m_nPrevSelEndPixel = 0;
				m_nFirstVisiblePixel = 0;
				this->Invalidate();
			}
			else
			{
				m_uSelEnd = m_strText.size();
				m_uSelBegin = m_uSelEnd;
				m_uPrevSelEnd = m_uSelEnd;
				m_nPrevSelEndPixel = m_nTextPixel;
				EnsureSelEndVisible();
				this->Invalidate();
			}
		}
		return;
	}
	else if (nChar == VK_BACK || nChar == VK_DELETE)
	{
		if(m_bReadOnly) return;
		if(m_uSelEnd == m_uSelBegin)
		{
			if(nChar == VK_BACK) 
			{
				if (m_uSelEnd > 0) --m_uSelEnd;
			}
			else 
			{
				if (m_uSelEnd < m_strText.size()) ++m_uSelEnd;
			}
			SetPrevSelEndInfo();
			int result = DelSelText();
			SetPrevSelEndInfo(result);
		}
		else
		{
			int result = DelSelText();
			SetPrevSelEndInfo(result);
		}
		RECT rcDraw = this->GetDeleteReDrawRect();
		if(EnsureSelEndVisible() && !m_bAutoSize) this->Invalidate();
		else 
		{
			if(!m_bAutoSize)
			{
				RECT rcClient = this->GetInterRect();
				OffsetRect(&rcDraw, rcClient.left, rcClient.top);
				GetWindow()->Invalidate(&rcDraw);
			}
			else
			{
				GetWindow()->Invalidate(&m_rcPrevItem);
			}
		}
		this->SendNotify(UINOTIFY_EN_CHANGE);
		return;
	}
	else if (nChar == VK_RETURN)
	{
	
		GetWindow()->KillFocus(this);
		this->SendNotify(UINOTIFY_EN_KEYDOWN, (WPARAM)nChar, NULL);
		
	}
	else if (nChar >= VK_LEFT && nChar <= VK_DOWN)
	{
		SHORT value = GetKeyState(VK_SHIFT);
		if (value & 0x8000)
		{
			if(m_bPassword) return;
			unsigned beforeMovePos = m_uSelEnd;
			switch(nChar)
			{
			case VK_LEFT:
				{
					if (m_uSelEnd > 0) --m_uSelEnd;
					SetPrevSelEndInfo();
				}
				break;
			case VK_RIGHT:
				{
					if (m_uSelEnd < m_strText.size()) ++m_uSelEnd;
					SetPrevSelEndInfo();
				}
				break;
			}
			RECT rcDraw = this->GetSelectReDrawRect(beforeMovePos);
			if(EnsureSelEndVisible()) this->Invalidate();
			else 
			{
				RECT rcClient = this->GetInterRect();
				OffsetRect(&rcDraw, rcClient.left, rcClient.top);
				GetWindow()->Invalidate(&rcDraw);
			}
		}
		else
		{
			if (m_uSelBegin != m_uSelEnd)
			{
				RECT rcDraw;
				unsigned beginPos= min(m_uSelBegin, m_uSelEnd);
				unsigned endPos = max(m_uSelBegin, m_uSelEnd);
				POINT beginPt = Index2Point(beginPos);
				POINT endPt = Index2Point(endPos);
				RECT rcClient = this->GetInterRect();
				unsigned nClientWidth = rcClient.right - rcClient.left;
				rcDraw.left = 0;
				rcDraw.top = beginPt.y;
				rcDraw.right = nClientWidth;
				rcDraw.bottom = endPt.y + m_nLineHeight;
				OffsetRect(&rcDraw, rcClient.left, rcClient.top);
				GetWindow()->Invalidate(&rcDraw);
			}
			if (m_uSelEnd < m_uSelBegin)
			{
				switch (nChar)
				{
				case VK_LEFT:
					{
						m_uSelBegin = m_uSelEnd;
					}
					break;
				case VK_RIGHT:
					{
						m_uSelEnd = m_uSelBegin;
						SetPrevSelEndInfo();
					}
					break;
				}
			}
			else if (m_uSelEnd > m_uSelBegin)
			{
				switch(nChar)
				{
				case VK_LEFT:
					{
						m_uSelEnd = m_uSelBegin;
						SetPrevSelEndInfo();
					}
					break;
				case VK_RIGHT:
					{
						m_uSelBegin = m_uSelEnd;
					}
					break;
				}
			}
			else
			{
				switch(nChar)
				{
				case VK_LEFT:
					{
						if (m_uSelEnd > 0) --m_uSelEnd;
						SetPrevSelEndInfo();
					}
					break;
				case VK_RIGHT:
					{
						if (m_uSelEnd < m_strText.size()) ++m_uSelEnd;
						SetPrevSelEndInfo();
					}
					break;
				}
				m_uSelBegin = m_uSelEnd;
			}
			if(this->EnsureSelEndVisible())
				this->Invalidate();
			/*else 
				this->ShowCaret();*/
		}
	}
}

void CEditUI::OnLButtonDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnLButtonDown(wParam, lParam, ppt);
		return;
	}

	bool  flag = m_bCaretEnable;
	if (m_uSelEnd != m_uSelBegin)
	{
		m_bCaretEnable = false;
		this->Invalidate();
	}

	if (!m_hParentWnd) return;
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle()) return;
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());
	m_uSelEnd = Point2Index(ppt, true);
	SetPrevSelEndInfo();
	m_uSelBegin = m_uSelEnd;
	m_bCaretEnable = flag;
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);
	if(m_bAutoSize)
		this->Invalidate();
}

void CEditUI::OnLButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnLButtonUp(wParam, lParam, ppt);
		return;
	}
}

void CEditUI::OnLButtonDblick(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnLButtonDblick(wParam, lParam, ppt);
		return;
	}

	m_uSelBegin = 0;
	m_uSelEnd = m_strText.size();
	SetPrevSelEndInfo();
	EnsureSelEndVisible();
	this->Invalidate();
}

void CEditUI::OnRButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnRButtonUp(wParam, lParam, ppt);
		return;
	}

	if(m_bPassword || !m_bMenuEnable) return;
	CMenuUI *pMenu = CMenuUI::Load(IDR_MENU_UIEDIT);
	if (pMenu == NULL) return;
	CMenuUI *pSubMenu = NULL;
	if(m_bReadOnly)
	{
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_CUT, BY_COMMAND);
		if(pSubMenu) pMenu->Remove(pSubMenu);
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_PASTE, BY_COMMAND);
		if(pSubMenu) pMenu->Remove(pSubMenu);
	}
	if(m_uSelEnd == m_uSelBegin)
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
}

void CEditUI::OnMouseMove(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnMouseMove(wParam, lParam, ppt);
		return;
	}


	UINT nFlags = (UINT)wParam;
	if(nFlags & MK_LBUTTON)
	{
		m_uSelEnd = Point2Index(ppt);
		if(m_uPrevSelEnd != m_uSelEnd)
		{
			unsigned beforeMovePos = m_uPrevSelEnd;
			SetPrevSelEndInfo();
			RECT rcDraw = this->GetSelectReDrawRect(beforeMovePos);
			if(EnsureSelEndVisible()) this->Invalidate();
			else 
			{
				RECT rcClient = this->GetInterRect();
				OffsetRect(&rcDraw, rcClient.left, rcClient.top);
				GetWindow()->Invalidate(&rcDraw);
			}
		}
	}
}

void CEditUI::SetRect(RECT& rectRegion)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::SetRect(rectRegion);
		return;
	}

	m_rcItem = rectRegion;
	if(m_bAutoSize)
	{
		RECT rcInset = GetInset();
		if(!m_bMaxSize)
		{
			if(GetParent())
			{
				RECT rcParent = GetParent()->GetInterRect();
				m_cxyMaxSize.cx = DPI_SCALE_BACK(rcParent.right - m_rcItem.left);
			}
			else
				m_cxyMaxSize.cx -= rcInset.left + rcInset.right;
		}
		m_rcPrevItem = m_rcItem;
		if (m_nTextPixel < DPI_SCALE(m_cxyMaxSize.cx))
			m_rcItem.right = m_rcItem.left + DPI_SCALE(rcInset.left) + m_nTextPixel + DPI_SCALE(rcInset.right);
		else 
			m_rcItem.right = m_rcItem.left + DPI_SCALE(rcInset.left) + DPI_SCALE(m_cxyMaxSize.cx) + DPI_SCALE(rcInset.right);

		this->SetStyleObjRect(m_rcItem);
	}
	RECT rcClient = this->GetInterRect();

	m_cy = (rcClient.bottom - rcClient.top - m_nLineHeight)/2;

	CControlUI::SetRect(m_rcItem);
	EnsureSelEndVisible();
	SetCaretPos();
}

void CEditUI::SetStyleObjRect(const RECT& rc)
{
	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
		if (pStyleObj)
			pStyleObj->OnSize(m_rcItem);
	}
}

void CEditUI::DrawRange(IRenderDC* pRenderDC, const unsigned &index1, const unsigned &index2)
{
	if (index2 == index1)
	{
		return;
	}
	RECT rcClient = this->GetInterRect();

	DibObj* pDibObj = pRenderDC->GetDibObj();
	bool bNeedChange = pRenderDC->GetEnableAlpha() && pDibObj;
	if (bNeedChange)
		CSSE::IncreaseAlpha(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(), rcClient);

	unsigned indexBegin = min(index1, index2);
	unsigned indexEnd = max(index1, index2);
	POINT ptBegin = Index2Point(indexBegin);
	tstring strDecrypt;
	LPCTSTR lpszText = GetDrawText(strDecrypt);
	::TextOut(pRenderDC->GetDC(), (int)rcClient.left+ptBegin.x, (int)rcClient.top+m_cy, lpszText+indexBegin, (int)(indexEnd-indexBegin));
	//pRenderDC->TextOut(GetEditStyle()->GetFontObj(), (int)rc.left+ptBegin.x, (int)rc.top+m_cy, lpszText+indexBegin, 
	//	(int)(indexEnd-indexBegin), GetEditStyle()->GetTextColor(), NULL, GetEditStyle()->GetFontType());

	if (bNeedChange)
		CSSE::DecreaseAlpha(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(),  rcClient);

}

RECT CEditUI::GetInsertReDrawRect(unsigned beforeInsertPos)
{
	RECT rcClient = this->GetInterRect();
	unsigned nClientWidth = rcClient.right - rcClient.left;
	RECT rc = {0, m_cy, nClientWidth, m_cy+m_nLineHeight};

	POINT pt = Index2Point(beforeInsertPos);
	rc.left = /*pt.x*/0;
	return rc;
}

RECT CEditUI::GetDeleteReDrawRect()
{
	RECT rcClient = this->GetInterRect();
	unsigned nClientWidth = rcClient.right - rcClient.left;
	RECT rc = {0, m_cy, nClientWidth, m_cy+m_nLineHeight};
	if(m_bAutoSize)
	{
		RECT rcInset = GetInset();
		rc.right = m_rcPrevItem.right - DPI_SCALE(rcInset.right);
	}

	POINT pt = Index2Point(m_uSelEnd);
	rc.left = /*pt.x*/0;
	rc.top = m_cy;
	return rc;
}

RECT CEditUI::GetSelectReDrawRect(unsigned beforeMovePos)
{
	RECT rcClient = this->GetInterRect();
	unsigned nClientWidth = rcClient.right - rcClient.left;
	RECT rc = {0, m_cy, nClientWidth, m_cy+m_nLineHeight};

	POINT ptBegin, ptEnd;
	if (beforeMovePos==m_uSelBegin && m_uSelBegin==m_uSelEnd)
	{
		return rc;
	}
	if (beforeMovePos==m_uSelBegin && beforeMovePos!=m_uSelEnd)
	{
		if (beforeMovePos > m_uSelEnd)
		{
			ptBegin = Index2Point(m_uSelEnd);
			ptEnd = Index2Point(beforeMovePos);
			rc.left = ptBegin.x;
			rc.right = ptEnd.x;
		}
		else if (beforeMovePos < m_uSelEnd)
		{
			ptBegin = Index2Point(beforeMovePos);
			ptEnd = Index2Point(m_uSelEnd);
			rc.left = ptBegin.x;
			rc.right = ptEnd.x;
		}
	}
	else if (beforeMovePos<m_uSelBegin && m_uSelEnd<beforeMovePos)
	{
		ptBegin = Index2Point(m_uSelEnd);
		ptEnd = Index2Point(beforeMovePos);
		rc.left = ptBegin.x;
		rc.right = ptEnd.x;
	}
	else if (beforeMovePos<m_uSelBegin && m_uSelEnd>beforeMovePos && m_uSelEnd<m_uSelBegin)
	{
		ptBegin = Index2Point(beforeMovePos);
		ptEnd = Index2Point(m_uSelEnd);
		rc.left = ptBegin.x;
		rc.right = ptEnd.x;
	}
	else if (beforeMovePos<m_uSelBegin && m_uSelEnd>=m_uSelBegin)
	{
		ptBegin = Index2Point(beforeMovePos);
		ptEnd = Index2Point(m_uSelEnd);
		rc.left = ptBegin.x;
		rc.right = ptEnd.x;
	}
	else if (beforeMovePos>m_uSelBegin && m_uSelEnd>beforeMovePos)
	{
		ptBegin = Index2Point(beforeMovePos);
		ptEnd = Index2Point(m_uSelEnd);
		rc.left = ptBegin.x;
		rc.right = ptEnd.x;
	}
	else if (beforeMovePos > m_uSelBegin && m_uSelEnd>m_uSelBegin && m_uSelEnd<beforeMovePos)
	{
		ptBegin = Index2Point(m_uSelEnd);
		ptEnd = Index2Point(beforeMovePos);
		rc.left = ptBegin.x;
		rc.right = ptEnd.x;
	}
	else if (beforeMovePos>m_uSelBegin && m_uSelEnd<=m_uSelBegin)
	{
		ptBegin = Index2Point(m_uSelEnd);
		ptEnd = Index2Point(beforeMovePos);
		rc.left = ptBegin.x;
		rc.right = ptEnd.x;
	}

	rc.left = 0;
	rc.right = nClientWidth;
	return rc;
}

void CEditUI::Copy()
{
	if(m_bPassword) return;
	if (m_uSelBegin != m_uSelEnd)
	{
		if (!OpenClipboard(m_hParentWnd))
			return;
		tstring selStr = GetSelText();
		int nCount = selStr.size();
		HGLOBAL hGlobalMem = GlobalAlloc(GMEM_MOVEABLE, (nCount+1)*sizeof(TCHAR));
		if (!hGlobalMem)
		{
			CloseClipboard();
			return;
		}
		EmptyClipboard();
		LPTSTR lpszStr= (LPTSTR)GlobalLock(hGlobalMem);
		if (lpszStr)
		{
			memcpy(lpszStr, selStr.c_str(), nCount* sizeof(TCHAR));
			lpszStr[nCount] = (TCHAR) 0;
		}
		GlobalUnlock(hGlobalMem);
#ifdef UNICODE
		SetClipboardData(CF_UNICODETEXT, hGlobalMem);
#else
		SetClipboardData(CF_TEXT, hGlobalMem);
#endif
		CloseClipboard();
	}
}

void CEditUI::Paste()
{
	OnPaste();
	if(m_bReadOnly || m_bPassword) return;
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
	if (hGlobalMem != NULL)
	{
		LPCTSTR lpszStr = (LPCTSTR)GlobalLock(hGlobalMem);
		if (lpszStr)
		{
			RECT rcDraw1, rcDraw2, rcDraw3;
			int result = DelSelText();
			if(m_bAutoSize)
				rcDraw1 = m_rcPrevItem;
			SetPrevSelEndInfo(result);
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
				result = Insert(lpszStr);
				if(m_bAutoSize)
					rcDraw2 = m_rcPrevItem;
				SetPrevSelEndInfo(result);
				EnsureSelEndVisible();
			}
			this->SendNotify(UINOTIFY_EN_CHANGE);
			if(!m_bAutoSize) 
				this->Invalidate();
			else
			{
				::UnionRect(&rcDraw3, &rcDraw1, &rcDraw2);
				RECT rc;
				::UnionRect(&rc, &m_rcItem, &rcDraw3);
				GetWindow()->Invalidate(&rc);
			}
		}
	}
	CloseClipboard();

	this->SetCaretPos();
	
}

void CEditUI::OnPaste()
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::OnPaste();
		return;
	}
}

tstring CEditUI::GetSelText()
{
	unsigned selBegin = min(m_uSelBegin, m_uSelEnd);
	unsigned selEnd = max(m_uSelBegin, m_uSelEnd);
	tstring strSelText = _T("");
	strSelText.assign(m_strText.c_str()+selBegin, selEnd-selBegin);
	return strSelText;
}

bool CEditUI::SetSelPos(int pos/*=-1*/)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		return __super::SetSelPos(pos);
	}

	GetWindow()->SetFocus(this);
	if(pos == -1)
	{
		m_uSelEnd = m_strText.size();
		m_uSelBegin = m_uSelEnd;
	}
	else if((pos<0 && pos!=-1) || pos>(int)m_strText.size())
	{
		return false;
	}
	else
	{
		m_uSelEnd = (unsigned)pos;
		m_uSelBegin = m_uSelEnd;	
	}
	this->SetPrevSelEndInfo();
	this->EnsureSelEndVisible();
	this->Invalidate();
	return true;
}

void CEditUI::SelAllText()
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::SelAllText();
		return;
	}

	if (!m_bReadOnly && !m_bPassword)
	{
		m_nFirstVisiblePixel = 0;
		m_uSelBegin = 0;
		m_uSelEnd = m_strText.size();
		SetPrevSelEndInfo();
		EnsureSelEndVisible();
		this->Invalidate();
	}
}

void CEditUI::HideToolTip()
{
	CWindowUI* pWindow = NULL;
	if (m_hToolTipWnd && IsWindow(m_hToolTipWnd))
	{
		pWindow = GetEngineObj()->GetWindow(m_hToolTipWnd);
	}
	if (pWindow)
	{
		pWindow->CloseWindow();
	}
}

bool CEditUI::OnLimitChar(UINT nChar)
{
	if (GetUIEngine()->IsRTLLayout())
	{
		return __super::OnLimitChar(nChar);
	}
	return false;
}



void CEditUI::SetFirstVisiblePixel(int nPixel)
{
	m_nFirstVisiblePixel = nPixel;
}

void CEditUI::SetPreSelEndPixel(int nPixel)
{
	m_nPrevSelEndPixel = nPixel;
}


void CEditUI::GetSel(int& nStartChar, int& nEndChar) const
{
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::GetSel(nStartChar, nEndChar);
		return;
	}
	nStartChar = min(m_uSelBegin, m_uSelEnd);
	nEndChar = max(m_uSelBegin, m_uSelEnd);
}

void CEditUI::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);

	if (!IsInternVisible()
		|| !IsVisible())
	{
		KillFocus();
	}
}

void CEditUI::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);

	if (!IsInternVisible()
		|| !IsVisible())
	{
		KillFocus();
	}
}

LPCTSTR CEditUI::GetDrawText(tstring& strTempUse)
{
	LPCTSTR lpszText;
	if (m_bPassword)
	{
		lpszText = m_strPasswdText.c_str();
		if (m_bPasswordVisiblity)
		{
			strTempUse = m_strText;
			decrypt(strTempUse);
			lpszText = strTempUse.c_str();
		}
	}
	else
	{
		lpszText = m_strText.c_str();
	}

	return lpszText;
}

void CEditUI::OnLanguageChange()
{
	__super::OnLanguageChange();

	if (m_lpszEmptyTipsKey)
		SetAttribute(_T("emptytips"), m_lpszEmptyTipsKey);


	if (m_hParentWnd != NULL)
	{
		HDC hDC = ::GetDC(m_hParentWnd);
		if (hDC != NULL)
		{
			TextStyle *pTextStyle = GetEditStyle();
			if (pTextStyle != NULL)
			{
				HFONT hOldFont = (HFONT)::SelectObject(hDC, pTextStyle->GetFontObj()->GetFont());
				TEXTMETRIC tm = { 0 };
				::GetTextMetrics(hDC, &tm);
				m_nLineHeight = tm.tmHeight + tm.tmExternalLeading;
				m_nAveCharWidth = tm.tmAveCharWidth;

				//initialize password char width regardless of whether the value of 'm_bPassword' is true or false.
				TCHAR szText[10] = { 0 };
				_stprintf(szText, _T("%c"), m_tcPassChar);
				m_nPasswdCharWidth = GetStringLine(hDC, szText, 1);

				::SelectObject(hDC, hOldFont);
			}
			::ReleaseDC(m_hParentWnd, hDC);
		}
	}
}



void CCssEditUI::Render(IRenderDC* pRenderDC, RECT &rcPaint)
{
	//__super::Render(pRenderDC, rcPaint);
	//if (!IsFocused() && m_strText.empty() && !m_strEmptyTipsText.empty())
	//{

	//	TextStyle *pEmptyEditTextStyle = GetEmptyEditStyle();
	//	if (pEmptyEditTextStyle != NULL)
	//	{
	//		RECT rect2 = GetRect();
	//		tstring str = _T("");
	//		RECT m_rcLinks[8];
	//		int n = 8;
	//		HtmlPaintParams paintParams;
	//		pRenderDC->DrawHtmlText(pEmptyEditTextStyle->GetFontObj(), rect2, m_strEmptyTipsText.c_str(), 0, &rect2, &str, n,
	//				_T(""), RGB(255, 0, 0), RGB(255, 0, 0), 1 | 0 | 32, paintParams, pEmptyEditTextStyle->GetFontType());
	//	}
	//}
	if (GetUIEngine()->IsRTLLayout())
	{
		__super::Render(pRenderDC, rcPaint);
		return;
	}

	TextStyle *pTextStyle = GetEditStyle();
	if (pTextStyle == NULL)
	{
		return;
	}
	if (!m_bTransparent)
	{
		CLabelUI::Render(pRenderDC, rcPaint);
	}

	HDC hDestDC = pRenderDC->GetDC();
	ClipObj clip;
	ClipObj::GenerateClipWithAnd(hDestDC, GetInterRect(), clip);

	int oldMode = ::SetBkMode(hDestDC, TRANSPARENT);
	HFONT hOldFont = (HFONT)::SelectObject(hDestDC, pTextStyle->GetFontObj()->GetFont());
	if (m_bReadOnly && !m_bTransparent)
	{
		HBRUSH hBr = CreateSolidBrush(COLOR_READONLY_BK);
		FillRect(hDestDC, &GetInterRect(), hBr);
		::DeleteObject(hBr);
	}

	if (!IsFocused())
	{

		TextStyle *pEmptyEditTextStyle = GetEmptyEditStyle();
		if (pEmptyEditTextStyle != NULL)
		{

			RECT rect = GetInterRect();
			RECT rect2{ 0, 0, 0, 0 };
			tstring str = _T("");
			RECT m_rcLinks[8];
			int n = 8;
			HtmlPaintParams paintParams;
			bool bEnd;
			pRenderDC->DrawHtmlText(pEmptyEditTextStyle->GetFontObj(), rect, m_strEmptyTipsText.c_str(), 0, &rect2, &str, n,
				_T(""), RGB(255, 255, 255), RGB(255, 255, 255), m_dwHAlign | m_dwVAlign | m_dwSingleLine, paintParams, bEnd, pEmptyEditTextStyle->GetFontType());//超链接暂时不考虑使用，颜色设死
		}
	}
	else if (!m_strText.empty())
	{
		unsigned iBegin = GetLeftClientWidthIndex(hDestDC, m_uSelEnd);
		unsigned iEnd = GetRightClientWidthIndex(hDestDC, m_uSelEnd);
		unsigned selBegin = min(m_uSelBegin, m_uSelEnd);
		unsigned selEnd = max(m_uSelBegin, m_uSelEnd);
		selBegin = max(selBegin, iBegin);
		selEnd = min(selEnd, iEnd);

		COLORREF oldTextColor = ::SetTextColor(hDestDC, DUI_ARGB2RGB(pTextStyle->GetTextColor()));
		COLORREF oldBkColor = ::SetBkColor(hDestDC, COLOR_INPUT_BK);
		this->DrawRange(pRenderDC, iBegin, selBegin);
		::SetBkMode(hDestDC, OPAQUE);
		::SetBkColor(hDestDC, COLOR_SELECT_BK);
		::SetTextColor(hDestDC, COLOR_SELECT_TEXT);
		this->DrawRange(pRenderDC, selBegin, selEnd);
		::SetBkMode(hDestDC, TRANSPARENT);
		::SetBkColor(hDestDC, COLOR_INPUT_BK);
		::SetTextColor(hDestDC, DUI_ARGB2RGB(pTextStyle->GetTextColor()));
		this->DrawRange(pRenderDC, selEnd, iEnd);

		::SetTextColor(hDestDC, oldTextColor);
		::SetBkColor(hDestDC, oldBkColor);
	}
	if (IsFocused() && m_bCaretEnable)
	{
		CCaretUI::GetInstance()->Render(pRenderDC, rcPaint);
	}

	::SetBkMode(hDestDC, oldMode);
	::SelectObject(hDestDC, hOldFont);
}

CCssEditUI::CCssEditUI()
{
	::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
	m_dwHAlign = DT_LEFT;
	m_dwVAlign = DT_TOP;
	m_dwSingleLine = DT_SINGLELINE;
	SetInset(RECT{ 4, 0, 4, 0 });
}

void CCssEditUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("halign")))
	{
		if (equal_icmp(lpszValue, _T("left")))
		{
			m_dwHAlign = DT_LEFT;
		}
		else if (equal_icmp(lpszValue, _T("center")))
		{
			m_dwHAlign = DT_CENTER;
		}
		else if (equal_icmp(lpszValue, _T("right")))
		{
			m_dwHAlign = DT_RIGHT;
		}
	}
	else if (equal_icmp(lpszName, _T("valign")))
	{
		if (equal_icmp(lpszValue, _T("top")))
		{
			m_dwVAlign = DT_TOP;
		}
		else if (equal_icmp(lpszValue, _T("center")))
		{
			m_dwVAlign = DT_VCENTER;
		}
		else if (equal_icmp(lpszValue, _T("bottom")))
		{
			m_dwVAlign = DT_BOTTOM;
		}
	}
	else if (equal_icmp(lpszName, _T("singleline")))
	{
		m_dwSingleLine = !!_ttoi(lpszValue) ? DT_SINGLELINE : 0;
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

CCssEditUI::~CCssEditUI()
{

}

void CCssEditUI::SetEmptyText(const tstring & sEmptyText)
{
	m_strEmptyTipsText = sEmptyText;
	Invalidate();
}

void CCssEditUI::OnSetFocus()
{
	__super::OnSetFocus();
}

#endif 