/********************************************************************
*            Copyright (C) 2010, 网龙天晴程序应用软件部
*********************************************************************/

#include "stdafx.h"

//#ifndef DIRECTUI_LAYOUT_RTL
#if 1

#include "UIEdit.h"
#include <stdio.h>
#include <ctype.h>
#include<time.h>
#include "UICaret.h"


#define COLOR_INPUT_BK		RGB(255, 255, 255)
#define COLOR_SELECT_TEXT	RGB(255, 255, 255)
#define COLOR_SELECT_BK		RGB(55, 104, 199)
#define COLOR_READONLY_BK	RGB(245, 245, 245)
#define COLOR_EMPTY_TEXT	RGB(128, 128 ,128)

typedef vector<LPLINEINFO> CLineInfoArray;

class MultiLineEditDataImpl
{
public:
	CLineInfoArray m_lineInfoArray;
};

/////////////////////////////////////////////

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
	inline bool operator < (const CharPos &pos1, const CharPos &pos2)
	{
		return (pos1.LineIndex<pos2.LineIndex || (pos1.LineIndex==pos2.LineIndex && pos1.CharIndex<pos2.CharIndex));
	}

	inline bool operator == (const CharPos &pos1, const CharPos &pos2)
	{
		return (pos1.CharIndex==pos2.CharIndex && pos1.LineIndex==pos2.LineIndex);
	}

	inline bool operator > (const CharPos &pos1, const CharPos &pos2)
	{
		return pos2 < pos1;
	}

	inline bool operator != (const CharPos &pos1, const CharPos &pos2)
	{
		return !(pos1 == pos2);
	}

	inline bool operator >= (const CharPos &pos1, const CharPos &pos2)
	{
		return !(pos1 < pos2);
	}

	inline bool operator <= (const CharPos &pos1, const CharPos &pos2)
	{
		return !(pos2 < pos1);
	}

	void del(LPLINEINFO &pLineInfo)
	{
		if(pLineInfo)
		{
			delete pLineInfo;
			pLineInfo = NULL;
		}
	}

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

/////////////////////////////////////////////////////////////
CMultiLineEditUI::CMultiLineEditUI()
{
	SetEnabled(true);
	RECT rcInset = {4, 4, 4, 4};
	SetInset(rcInset);
	SetStyle(_T("Edit"));
	SetMouseEnabled(true);
	ModifyFlags(UICONTROLFLAG_SETCURSOR | UICONTROLFLAG_TABSTOP | UICONTROLFLAG_SETFOCUS | UICONTROLFLAG_WANTRETURN, 0);
	/////////////////////////////

	m_pEditDataImpl = new MultiLineEditDataImpl;
	m_hCursor = NULL;
	m_hParentWnd = NULL;

	m_strText = TEXT("");

	LPLINEINFO pLineInfo = new LINEINFO();
	m_pEditDataImpl->m_lineInfoArray.push_back(pLineInfo);
	memset(&m_cpSelBegin, 0, sizeof(CharPos));
	memset(&m_cpSelEnd, 0, sizeof(CharPos));
	memset(&m_cpPrevSelEnd, 0, sizeof(CharPos));
	
	m_uClientWidth = 0;
	m_uClientHeight = 0;
	m_nFirstVisibleLine = 0;

	m_bhasCaret = true;
	m_bKillFocus = true;
	m_bReadOnly = false;
	m_nTransparent = 0;
	m_bAutoSize = false;
	m_bMenuEnable = true;
	m_bMaxSize = false;
	m_nMaxchar = -1;
	m_nMaxUTF8Char = -1;
	m_nCurUTF8Char = 0;
	m_nPreCaretHeight = 0;
	ZeroMemory(&m_cxyMaxSize, sizeof(SIZE));
	ZeroMemory(&m_rcPrevItem, sizeof(RECT));

	m_bInit = false;
	m_pVerticalScrollbar = NULL;

	SetAttribute(_T("cursor"), _T("IBEAM"));
	m_strEmptyTipsText = _T("");
	m_pTextStyle = NULL;
	m_pEmptyTextStyle = NULL;

	GetEditStyle()->SetAlign(DT_LEFT|DT_TOP);
}

CMultiLineEditUI::~CMultiLineEditUI()
{
	if(m_hCursor)
	{
		::DeleteObject(m_hCursor);
		m_hCursor = NULL;
	}
	if (m_pVerticalScrollbar)
	{
		delete m_pVerticalScrollbar;
		m_pVerticalScrollbar = NULL;
	}

	if (m_pEditDataImpl)
		for_each(m_pEditDataImpl->m_lineInfoArray.begin(), m_pEditDataImpl->m_lineInfoArray.end(), del);

	this->DestroyCaret();
	if (m_pEditDataImpl)
	{
		delete m_pEditDataImpl;
	}
	m_pEditDataImpl = NULL;
}

void CMultiLineEditUI::Init()
{
	m_hParentWnd = GetWindow()->GetHWND();
	m_hCursor = ::LoadCursor(NULL, IDC_IBEAM);

	CalcTextMetrics();
	
	if(!m_strText.empty())
	{
		if (m_nMaxchar != -1)
		{
			if(m_strText.size() > (tstring::size_type)m_nMaxchar)
				m_strText.erase(m_nMaxchar);
		}
		if (m_nMaxUTF8Char != -1)
		{
			m_nCurUTF8Char = 0;
			int nAddUTF8Chars = 0;
			int nInsertCounts = CountUTF8Chars(m_strText.c_str(), m_nCurUTF8Char, m_nMaxUTF8Char, &nAddUTF8Chars);
			m_nCurUTF8Char += nAddUTF8Chars;
			m_strText.erase(nInsertCounts);
		}
		if(m_uClientWidth)
		{
			AdjustAllLineInfo();
		}
		if(m_bAutoSize)
		{
			int nTextHeight = this->GetTextHeight();
			if (nTextHeight < m_cxyMaxSize.cy)
				m_rcItem.bottom = m_rcItem.top + nTextHeight + m_rcInset.top + m_rcInset.bottom;
			else
				m_rcItem.bottom = m_rcItem.top + m_cxyMaxSize.cy + m_rcInset.top + m_rcInset.bottom;

			this->SetStyleObjRect(m_rcItem);
			RECT rcClient = this->GetInterRect();
			m_uClientWidth = rcClient.right - rcClient.left;
			m_uClientHeight = rcClient.bottom - rcClient.top;
		}
	}
	m_bInit = true;
}

void CMultiLineEditUI::CalcTextMetrics()
{
	if(m_hParentWnd)
	{
		HDC hDC = ::GetDC(m_hParentWnd);
		if (hDC)
		{
			if(GetEditStyle())
			{
				HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());
				TEXTMETRIC tm = {0};
				::GetTextMetrics(hDC,&tm);
				m_nLineHeight = tm.tmHeight + tm.tmExternalLeading;
				m_nAveCharWidth = tm.tmAveCharWidth;
				::SelectObject(hDC, hOldFont);
			}
			::ReleaseDC(m_hParentWnd, hDC);
		}
	}
}

void CMultiLineEditUI::SetRect(RECT& rectRegion)
{
	RECT rcItem = rectRegion;
	RECT rcClient = this->GetInterRect();
	m_uClientWidth = rcClient.right - rcClient.left;
	m_uClientHeight = rcClient.bottom - rcClient.top;
	if(!m_strText.empty())
	{
		unsigned indexBegin, indexEnd;
		CharPos2Index(m_cpSelBegin, indexBegin);
		CharPos2Index(m_cpSelEnd, indexEnd);
		AdjustAllLineInfo();
		if (m_bAutoSize)
		{
			if (!m_bMaxSize)
			{
				CControlUI* pParent = GetParent();
				if (pParent)
				{
					RECT rcParent = pParent->GetInterRect();
					m_cxyMaxSize.cy = rcParent.bottom - rcItem.top;
				}
				else if (GetWindow())
				{
					RECT rcManager;
					::GetClientRect(GetWindow()->GetHWND(), &rcManager);
					int nMaxHeight = rcManager.bottom - rcManager.top;
					m_cxyMaxSize.cy = nMaxHeight - rcItem.top;
				}
				else
					m_cxyMaxSize.cy -= m_rcInset.bottom + m_rcInset.top;
			}
			int nTextHeight = this->GetTextHeight();
			if (nTextHeight < m_cxyMaxSize.cy)
				rcItem.bottom = rcItem.top + nTextHeight + m_rcInset.top + m_rcInset.bottom;
			else
				rcItem.bottom = rcItem.top + m_cxyMaxSize.cy + m_rcInset.top + m_rcInset.bottom;

			this->SetStyleObjRect(rcItem);
		}
		Index2CharPos(indexBegin, m_cpSelBegin);
		Index2CharPos(indexEnd, m_cpSelEnd);
	}
	else
	{
		if (m_bAutoSize)
		{
			if (!m_bMaxSize)
			{
				if (GetParent())
				{
					RECT rcParent = GetParent()->GetInterRect();
					m_cxyMaxSize.cy = rcParent.bottom - rcItem.top;
				}
				else if (!GetParent())
				{
					RECT rcManager;
					::GetClientRect(GetWindow()->GetHWND(), &rcManager);
					int nMaxHeight = rcManager.bottom - rcManager.top;
					m_cxyMaxSize.cy = nMaxHeight - rcItem.top;
				}
				else
					m_cxyMaxSize.cy -= m_rcInset.bottom + m_rcInset.top;
			}
			rcItem.bottom = rcItem.top + m_rcInset.top + m_nLineHeight + m_rcInset.bottom;
			this->SetStyleObjRect(rcItem);
		}
	}
	__super::SetRect(rcItem);
}

void CMultiLineEditUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("readonly")))
	{
		m_bReadOnly = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("transparent")))
	{
		m_nTransparent = !!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("maxchar")))
	{
		m_nMaxchar = _ttoi(lpszValue);
		if(m_nMaxchar < 0)
		{
			m_nMaxchar = 0;
		}
	}
	else if (equal_icmp(lpszName, _T("maxutf8char")))
	{
		m_nMaxUTF8Char = _ttoi(lpszValue);
		if(m_nMaxUTF8Char < 0)
		{
			m_nMaxUTF8Char = -1;
		}
	}
	else if (equal_icmp(lpszName, _T("maxheight")))
	{
		m_bMaxSize = true;
		m_cxyMaxSize.cy = (int)_ttoi(lpszValue);
		m_cxyMaxSize.cy -= m_rcInset.top + m_rcInset.bottom;
	}
	else if (equal_icmp(lpszName, _T("autosize")))
	{
		m_bAutoSize = (bool)!!_ttoi(lpszValue);
		//m_nTransparent = 1;
	}
	else if (equal_icmp(lpszName, _T("menuenable")))
	{
		m_bMenuEnable = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("emptytips")))
	{
		m_strEmptyTipsText = I18NSTR(lpszValue);
	}
	else
	{
		CControlUI::SetAttribute(lpszName, lpszValue);
	}
}


void CMultiLineEditUI::CreateCaret()
{
	POINT point;
	CharPos2Point(m_cpSelEnd, point);
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
	CCaretUI::GetInstance()->ShowCaret();
}

void CMultiLineEditUI::SetCaretPos()
{
	if (!IsFocused())
		return;
	POINT point;
	CharPos2Point(m_cpSelEnd, point);
	RECT rc = this->GetInterRect();
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

void CMultiLineEditUI::DestroyCaret()
{
	CCaretUI::GetInstance()->DestroyCaret(GetWindow());
}


void CMultiLineEditUI::SetClientWidth(unsigned width)
{
	m_uClientWidth = width;
	unsigned indexBegin, indexEnd;
	CharPos2Index(m_cpSelBegin, indexBegin);
	CharPos2Index(m_cpSelEnd, indexEnd);

	this->AdjustAllLineInfo();

	Index2CharPos(indexBegin, m_cpSelBegin);
	Index2CharPos(indexEnd, m_cpSelEnd);
	EnsureSelEndVisible();
	this->Invalidate();
}

void CMultiLineEditUI::SetReadOnly(bool bReadOnly)
{
	m_bReadOnly = bReadOnly;
}

void CMultiLineEditUI::SetText(LPCTSTR lpszText)
{
	if(m_bInit)
	{
		m_strText.assign(lpszText);
		if (m_nMaxchar != -1)
		{
			if(m_strText.size() > (tstring::size_type)m_nMaxchar)
				m_strText.erase(m_nMaxchar);
		}
		if (m_nMaxUTF8Char != -1)
		{
			m_nCurUTF8Char = 0;
			int nAddUTF8Chars = 0;
			int nInsertCounts = CountUTF8Chars(m_strText.c_str(), m_nCurUTF8Char, m_nMaxUTF8Char, &nAddUTF8Chars);
			m_nCurUTF8Char += nAddUTF8Chars;
			m_strText.erase(nInsertCounts);
		}
		if(m_uClientWidth) 
		{
			AdjustAllLineInfo();
		}
		if(m_bAutoSize)
		{
			m_rcPrevItem = m_rcItem;
			int nTextHeight = this->GetTextHeight();
			if (nTextHeight < m_cxyMaxSize.cy)
				m_rcItem.bottom = m_rcItem.top + nTextHeight + m_rcInset.top + m_rcInset.bottom;
			else
				m_rcItem.bottom = m_rcItem.top + m_cxyMaxSize.cy + m_rcInset.top + m_rcInset.bottom;
			this->SetStyleObjRect(m_rcItem);
			RECT rcClient = this->GetInterRect();
			m_uClientWidth = rcClient.right - rcClient.left;
			m_uClientHeight = rcClient.bottom - rcClient.top;
		}
		//this->SendNotify(UINOTIFY_EN_CHANGE);
		this->Invalidate();
	}
	else
	{
		m_nCurUTF8Char = 0;
		m_strText.assign(lpszText);
		this->SendNotify(UINOTIFY_EN_CHANGE);
		this->Invalidate();
	}
}

void CMultiLineEditUI::SetFirstVisibleLine(unsigned uLineNum)
{
	unsigned uMaxLineNum = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
	if(uLineNum > uMaxLineNum)
	{
		uLineNum = uMaxLineNum;
	}
	m_nFirstVisibleLine = uLineNum;
	this->Invalidate();
}

void CMultiLineEditUI::SetTextColor(DWORD dwClrText)
{
	if(GetEditStyle() != NULL)
	{
		GetEditStyle()->SetTextColor(dwClrText);
	}
}

void CMultiLineEditUI::SetFontName(LPCTSTR lpszFontName)
{
	if (GetEditStyle() != NULL)
	{
		FontObj* pFontObj = GetEditStyle()->GetFontObj();
		if (equal_icmp(lpszFontName, pFontObj->GetFaceName())) return;

		FontObj* pFontObjNew = new FontObj(*pFontObj);
		pFontObjNew->SetFaceName(lpszFontName);
		GetEditStyle()->SetFontObj(pFontObjNew);

		CalcTextMetrics();
	}
}

void CMultiLineEditUI::SetFontSize(int nFontSize)
{
	if (GetEditStyle() != NULL)
	{
		FontObj* pFontObj = GetEditStyle()->GetFontObj();
		if (nFontSize == pFontObj->GetHeight())
		{
			return;
		}
		FontObj* pFontObjNew = new FontObj(*pFontObj);
		pFontObjNew->SetFontSize(nFontSize);
		GetEditStyle()->SetFontObj(pFontObjNew);

		CalcTextMetrics();
	}
}

void CMultiLineEditUI::SetBold(bool bBold)
{
	if (GetEditStyle() != NULL)
	{
		FontObj* pFontObj = GetEditStyle()->GetFontObj();
		if (bBold == pFontObj->IsBold())
		{
			return;
		}
		FontObj* pFontObjNew = new FontObj(*pFontObj);
		pFontObjNew->SetBold(bBold);
		GetEditStyle()->SetFontObj(pFontObjNew);

		CalcTextMetrics();
	}
}

void CMultiLineEditUI::SetItalic(bool bItalic)
{
	if (GetEditStyle() != NULL)
	{
		FontObj* pFontObj = GetEditStyle()->GetFontObj();
		if (bItalic == pFontObj->IsItalic())
		{
			return;
		}
		FontObj* pFontObjNew = new FontObj(*pFontObj);
		pFontObjNew->SetItalic(bItalic);
		GetEditStyle()->SetFontObj(pFontObjNew);

		CalcTextMetrics();
	}
}

void CMultiLineEditUI::SetUnderline(bool bUnderline)
{
	if (GetEditStyle() != NULL)
	{
		FontObj* pFontObj = GetEditStyle()->GetFontObj();
		if (bUnderline == pFontObj->IsUnderline())
		{
			return;
		}
		FontObj* pFontObjNew = new FontObj(*pFontObj);
		pFontObjNew->SetUnderline(bUnderline);
		GetEditStyle()->SetFontObj(pFontObjNew);

		CalcTextMetrics();
	}
}

void CMultiLineEditUI::SetAlign(UINT uAlignStyle)
{
	if(GetEditStyle() != NULL)
	{
		GetEditStyle()->SetAlign(uAlignStyle);
	}
}

void CMultiLineEditUI::Insert(LPCTSTR lpszText)
{
	if (!lpszText || *lpszText==0) return;
	if (!m_hParentWnd) return;
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle()) return;
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());

	TCHAR *pStart = NULL;
	TCHAR *pNext = NULL;
	TCHAR *pEnd = NULL;
	TCHAR *pTmp = NULL;    // maxchar  
	TCHAR ch = 0;
	TCHAR *pTmp1 = NULL;   // maxutf8char
	TCHAR ch1 = 0;

	int nInsertCharsCount = _tcslen(lpszText);
	int nSize = m_strText.size();
	if (m_nMaxchar != -1)
	{
		if (nSize >= m_nMaxchar) goto END;
		int nLen = m_nMaxchar - nSize;
		if(nLen > nInsertCharsCount) nLen = nInsertCharsCount;
		if(!nLen) goto END;
		nInsertCharsCount = nLen;
		pTmp = const_cast<LPTSTR>(lpszText) + nInsertCharsCount;
		ch = *pTmp;
		*pTmp = (TCHAR)0;
	}
	if (m_nMaxUTF8Char != -1)
	{
		int nAddUTF8Chars = 0;
		int nInsertCounts = CountUTF8Chars(lpszText, m_nCurUTF8Char, m_nMaxUTF8Char, &nAddUTF8Chars);
		if (!nInsertCounts)
		{
			if(pTmp) *pTmp = ch;
			goto END;
		}
		pTmp1 = const_cast<LPTSTR>(lpszText) + nInsertCounts;
		m_nCurUTF8Char += nAddUTF8Chars;
		nInsertCharsCount = nInsertCounts;
		ch1 = *pTmp1;
		*pTmp1 = (TCHAR)0;
	}
	unsigned nStartIndex = 0;
	unsigned nEndIndex = 0;
	unsigned tmpIndex = 0;
	CharPos2Index(m_cpSelEnd, nStartIndex);

	if (nStartIndex > nSize)
	{
		return;
	}

	tmpIndex = nStartIndex;
	unsigned curParaLastLineNum = FindNextCrlfLine(m_cpSelEnd.LineIndex);   //从当前光标所在的行开始，查找下一个含有回车的行号
	CharPos pos;                                                         //一定要在未删除无效行信息时，计算出下一个含有回车行的最后位置字符的索引值
	pos.CharIndex = 0;												//然后加上你要插入的字符个数，就是插入完后原来含有回车行最后个字符的索引位置.
	pos.LineIndex = curParaLastLineNum;
	CharPos2Index(pos, nEndIndex);
	nEndIndex += m_pEditDataImpl->m_lineInfoArray[curParaLastLineNum]->CharsCount; //求出含有回车行最后个字符的索引位置.
	nEndIndex += nInsertCharsCount;   //加上要插入字符的个数，求出插入完后含有回车行最后个字符的索引位置.

	unsigned nDelLinesCount = curParaLastLineNum - m_cpSelEnd.LineIndex + 1;  //计算出待插入时，使哪些行的信息无效。
	RemoveLineInfo(m_cpSelEnd.LineIndex, nDelLinesCount);  //删除无效的行信息

	m_strText.insert(nStartIndex, lpszText, nInsertCharsCount);   //插入要插入的字符串
	nStartIndex -= m_cpSelEnd.CharIndex;            //求出未插入时光标所在位置的行的第一个字符的索引值。
																		  //至此待调整行信息的字符串中索引范围已获得:  [nStartIndex, nEndIndex);
																		 //接下来是对改字符串索引范围的字符串重新调整行信息
	pStart = const_cast<LPTSTR>(m_strText.c_str()) + nStartIndex;          //根据待调整索引范围得到待调整字符串的指针区间: [pStart, pEnd);
	pEnd = const_cast<LPTSTR>(m_strText.c_str()) + nEndIndex;
	unsigned nStartInsertLine = m_cpSelEnd.LineIndex;  //开始调整行信息的行号
	int nCrlfCounts = 0;
	while(TRUE)
	{
		pNext = _tcsstr(pStart, crlf);   //在字符串中查找含有回车字符位置

		if(pNext) *pNext = (TCHAR)0;  //将查找到的回车位置暂时置为0，方便以下操作
		LPCTSTR lpString = pStart;
		while (TRUE)
		{
			int tmp = CutStringForOneLine(hDC, lpString);   //对一个字符串进行统计，以符合编辑框的宽带，返回其一行的个数
			LPLINEINFO pLineInfo = new LINEINFO;
			pLineInfo->CharsCount = tmp;

			if (tmp == _tcslen(lpString))   //若统计的长度与字符串本身长度相等则这行含有回车信息。
			{
				if(pNext) pLineInfo->bParaEnd = MYTRUE;
				InsertAt(m_pEditDataImpl->m_lineInfoArray, nStartInsertLine++, pLineInfo);
				break;
			}
			InsertAt(m_pEditDataImpl->m_lineInfoArray, nStartInsertLine++, pLineInfo);//插入行信息
			lpString += tmp;  
		}
		if(pNext) *pNext = crlf[0]; //将回车置换回来
		if (!pNext) break;   //若无回车 则退出循环
		if(pNext == pEnd) break;  //若pNext与pEnd相等 则说明待分行的字符串指针移到中止位置，退出循环
		pStart = pNext + crlf_Count;
	}
	tmpIndex += nInsertCharsCount;                   //以下为重置光标位置
	Index2CharPos(tmpIndex, m_cpSelEnd);

	if(nInsertCharsCount >= crlf_Count)
	{
		pStart = const_cast<LPTSTR>(lpszText) + nInsertCharsCount - crlf_Count;
		if(equal_icmp(pStart, crlf))
		{
			m_cpSelEnd.LineIndex += 1;
			m_cpSelEnd.CharIndex = 0;
		}
	}
	m_cpSelBegin = m_cpSelEnd;
	if(pTmp) *pTmp = ch;
	if(pTmp1) *pTmp1 = ch1;

	if(m_bAutoSize)    //若为可变大小编辑框，则调整其大小。
	{
		m_rcPrevItem = m_rcItem;
		int nTextHeight = this->GetTextHeight();
		if (nTextHeight < m_cxyMaxSize.cy)
			m_rcItem.bottom = m_rcItem.top + nTextHeight + m_rcInset.top + m_rcInset.bottom;
		else
			m_rcItem.bottom = m_rcItem.top + m_cxyMaxSize.cy + m_rcInset.top + m_rcInset.bottom;

		this->SetStyleObjRect(m_rcItem);
		RECT rcClient = this->GetInterRect();
		m_uClientWidth = rcClient.right - rcClient.left;
		m_uClientHeight = rcClient.bottom - rcClient.top;
	}
END:
	this->SendNotify(UINOTIFY_EN_CHANGE);
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);
}

bool CMultiLineEditUI::DelSelRange()
{
	if (m_cpSelBegin == m_cpSelEnd)	 return false;

	unsigned indexBegin, indexEnd;
	CharPos selBegin, selEnd;
	selBegin = min(m_cpSelBegin, m_cpSelEnd);            //确定选定范围的先后保证selBegin在前，selEnd在后
	selEnd = max(m_cpSelBegin, m_cpSelEnd);
	CharPos2Index(selBegin, indexBegin);
	CharPos2Index(selEnd, indexEnd);   //转换为索引位置
						
	if (m_nMaxUTF8Char != -1)
	{
		int nDelCounts = indexEnd - indexBegin;
		LPTSTR lpszText = const_cast<LPTSTR>(m_strText.c_str()) + indexBegin;
		int nDelUTF8Chars = CountUTF8Chars(lpszText, nDelCounts);
		m_nCurUTF8Char -= nDelUTF8Chars;
	}
	unsigned paraLastLineNum = FindNextCrlfLine(selEnd.LineIndex);
	unsigned nCount = paraLastLineNum - selBegin.LineIndex + 1;
	RemoveLineInfo(selBegin.LineIndex, nCount);  //删除无效的行信息
	RemoveAt(m_strText, indexBegin, indexEnd-indexBegin);  //删除选定的字符串
	AdjustFromLineToParaEnd(selBegin.LineIndex, INSERT_ONLY);//因为前面已经删除，所以现在没有无效的行信息，所以只需添加
	m_cpSelEnd = selBegin;
	m_cpSelBegin = m_cpSelEnd;
	if(m_bAutoSize)
	{
		m_rcPrevItem = m_rcItem;
		int nTextHeight = this->GetTextHeight();
		if (nTextHeight < m_cxyMaxSize.cy)
			m_rcItem.bottom = m_rcItem.top + nTextHeight + m_rcInset.top + m_rcInset.bottom;
		else
			m_rcItem.bottom = m_rcItem.top + m_cxyMaxSize.cy + m_rcInset.top + m_rcInset.bottom;

		this->SetStyleObjRect(m_rcItem);
		RECT rcClient = this->GetInterRect();
		m_uClientWidth = rcClient.right - rcClient.left;
		m_uClientHeight = rcClient.bottom - rcClient.top;
	}
	this->SendNotify(UINOTIFY_EN_CHANGE);
	return true;
}

bool CMultiLineEditUI::EnsureSelEndVisible()
{
	bool result = false;
	POINT point;
	CharPos2Point(m_cpSelEnd, point);

	RECT rcEdit=this->GetInterRect();
	int iEditHeight = abs(rcEdit.bottom - rcEdit.top);

	if (point.y < 0)
	{
		m_nFirstVisibleLine -= (-point.y + m_nLineHeight-1) / m_nLineHeight;
		result = true;
	}
	else if (point.y + m_nLineHeight > iEditHeight)
	{
		if (m_nLineHeight > iEditHeight)
		{
			m_nFirstVisibleLine += (point.y + m_nLineHeight - iEditHeight - 1) / m_nLineHeight;
		}
		else
		{
			m_nFirstVisibleLine += (point.y + m_nLineHeight - iEditHeight + m_nLineHeight - 1) / m_nLineHeight;
		}
		result = true;
	}

	int iTextHeight = GetTextHeight();
	if (iTextHeight < iEditHeight)
	{
		m_nFirstVisibleLine = 0;
	}

	return result;
}

void CMultiLineEditUI::TextOut(IRenderDC* pRenderDC, int x, int y, LPCTSTR lpszText, int c)
{
	RECT rc = this->GetInterRect();

	DibObj* pDibObj = pRenderDC->GetDibObj();
	bool bNeedChange = pRenderDC->GetEnableAlpha() && pDibObj;
	if (bNeedChange)
		CSSE::IncreaseAlpha(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(), rc);

	 ::TextOut(pRenderDC->GetDC(), x+(int)rc.left, y+(int)rc.top, lpszText, c);

	if (bNeedChange)
		CSSE::DecreaseAlpha(pDibObj->GetBits(), pDibObj->GetWidth(), pDibObj->GetHeight(),  rc);
}

void CMultiLineEditUI::DrawRange(IRenderDC* pRenderDC, const CharPos &pos1, const CharPos &pos2)
{
	if (pos1 == pos2) return;
	CharPos selBegin, selEnd;
	selBegin = min(pos1, pos2);
	selEnd = max(pos1, pos2);

	unsigned indexBegin, indexEnd;
	CharPos2Index(selBegin, indexBegin);
	CharPos2Index(selEnd, indexEnd);
	POINT ptBegin;
	CharPos2Point(selBegin, ptBegin);

	LPTSTR lpszText = const_cast<LPTSTR>(m_strText.c_str()) + indexBegin;
	LPTSTR pEnd = lpszText + (indexEnd - indexBegin);
	TCHAR tmpChar = *pEnd;
	*pEnd = (TCHAR)0;

	if (selBegin.LineIndex == selEnd.LineIndex)
	{
		this->TextOut(pRenderDC, ptBegin.x, ptBegin.y, lpszText, (int)(indexEnd-indexBegin));
	}
	else
	{
		int nCount = (int)(m_pEditDataImpl->m_lineInfoArray[selBegin.LineIndex]->CharsCount - selBegin.CharIndex);
		this->TextOut(pRenderDC, ptBegin.x, ptBegin.y, lpszText, nCount);
		lpszText += (nCount + (int)(m_pEditDataImpl->m_lineInfoArray[selBegin.LineIndex]->bParaEnd));
		POINT pt = {0};
		pt.y = ptBegin.y + m_nLineHeight;
		for (unsigned i=selBegin.LineIndex+1; i!=selEnd.LineIndex; ++i)
		{
			CharPos pos;
			pos.LineIndex = i;
			pos.CharIndex = 0;
			CharPos2Point(pos, pt);
			nCount = m_pEditDataImpl->m_lineInfoArray[i]->CharsCount;
			this->TextOut(pRenderDC, pt.x, pt.y, lpszText, nCount);
			lpszText += (nCount + (int)(m_pEditDataImpl->m_lineInfoArray[i]->bParaEnd));
			pt.y += m_nLineHeight;
		}
		CharPos pos;
		pos.LineIndex = (int)selEnd.LineIndex;
		pos.CharIndex = 0;
		CharPos2Point(pos, pt);
		this->TextOut(pRenderDC, pt.x, pt.y, lpszText, (int)selEnd.CharIndex);
	}

	*pEnd = tmpChar;
}

inline int CMultiLineEditUI::GetTextHeight()
{
	return (m_nLineHeight > 0 ? (int)(m_nLineHeight*m_pEditDataImpl->m_lineInfoArray.size()) : -1);
}

void CMultiLineEditUI::SetStyleObjRect(const RECT& rc)
{
	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
		if (pStyleObj)
			pStyleObj->OnSize(m_rcItem);
	}
}

bool CMultiLineEditUI::Event(TEventUI& event)
{
	switch(event.nType)
	{
	case UIEVENT_SCROLLWHEEL:
		{
			short zDelta = (short)HIWORD(event.wParam);
			if (zDelta > 0)
			{
				UpSelEnd();
			}
			else
			{
				DownSelEnd();
			}
			m_cpSelBegin = m_cpSelEnd;
			if(!EnsureSelEndVisible())
			{
				this->SetCaretPos();
			}
			this->Invalidate();

			//if(m_pParent)
			//	::Event(event, m_pParent);
		}
		break;
	case UIEVENT_SETCURSOR:
		{
			if (PtInRect(&GetInterRect(), event.ptMouse))
			{
				::SetCursor(m_hCursor);
				return true;
			}
		}
		break;
	case UIEVENT_SETFOCUS:
		{
			m_bFocused = true;
			

			m_bKillFocus = false;
			m_cpSelEnd.LineIndex = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			m_cpSelEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount;
			m_cpSelBegin = m_cpSelEnd;
			this->EnsureSelEndVisible();
			if (GetState() != UISTATE_OVER)
			{
				SetState(UISTATE_OVER);
				this->Invalidate(true);	
			}
		
			this->CreateCaret();
			this->SetCaretPos();
			
		}
		break;

	case UIEVENT_KILLFOCUS:
		{
			m_bFocused = false;
			m_bKillFocus = true;
			this->DestroyCaret();
			SetState(UISTATE_NORMAL);
			this->DestroyCaret();
			m_cpSelEnd.CharIndex = 0;
			m_cpSelEnd.LineIndex = 0;
			m_cpSelBegin = m_cpSelEnd;
			this->EnsureSelEndVisible();
			this->Invalidate(true);
			this->SendNotify(UINOTIFY_KILLFOCUS);
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
			if(m_bKillFocus)
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
			if (!GetEditStyle()) break;
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
				LOGFONT lf;
				GetEditStyle()->GetFontObj()->GetLogFont(&lf);
				::ImmSetCompositionFont(hImc, &lf);
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
			POINT pt;
			pt = event.ptMouse;
			RECT rcClient = this->GetInterRect();
			if (PtInRect(&rcClient, pt))
			{
				OnRButtonUp(event.wParam, pt);
			}
		}
		break;
	case UIEVENT_BUTTONDOWN:
		{
			POINT pt;
			pt = event.ptMouse;
			RECT rcClient = this->GetInterRect();
			if (PtInRect(&rcClient, pt))
			{
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnLButtonDown(event.wParam, pt);
			}
		}
		break;
	case UIEVENT_BUTTONUP:
		{
			POINT pt;
			pt = event.ptMouse;
			OnLButtonUp(event.wParam, pt);
		}
		break;
	case UIEVENT_LDBCLICK:
		{
			POINT pt;
			pt = event.ptMouse;
			RECT rcClient = this->GetInterRect();
			if (PtInRect(&rcClient, pt))
			{
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnLButtonDblClk(event.wParam, pt);
			}
		}
		break;
	case UIEVENT_MOUSEMOVE:
		{
			POINT pt;
			pt = event.ptMouse;
			RECT rcClient = this->GetInterRect();
			if (PtInRect(&rcClient, pt))
			{
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnMouseMove(event.wParam, pt);
			}
			else
			{
				if (pt.x < rcClient.left) pt.x = rcClient.left;
				if (pt.x > rcClient.right) pt.x = rcClient.right;
				if (pt.y < rcClient.top) pt.y = rcClient.top - m_nLineHeight;
				if (pt.y > rcClient.bottom) pt.y = rcClient.bottom;
				pt.x -= rcClient.left;
				pt.y -= rcClient.top;
				OnMouseMove(event.wParam, pt);	
			}
		}
		break;
	default:
		break;
	}
	return __super::Event(event);;
}

bool CMultiLineEditUI::OnMenuItemClick(TNotifyUI* pNotify)
{
	UIMENUINFO* pInfo = (UIMENUINFO*)pNotify->wParam;
	switch (pInfo->uID)
	{
	case IDR_MENU_UIEDIT_CUT:
		{
			if(m_cpSelEnd != m_cpSelBegin)
			{
				this->Copy();
				CharPos BeforeInsertPos = max(m_cpSelEnd, m_cpSelBegin);
				unsigned uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
				DelSelRange();
				unsigned uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
				RECT rcDraw = this->GetDeleteReDrawRect(BeforeInsertPos, m_cpSelEnd, 
					!!(uAfterInsertLineCount-uBeforeInsertLineCount));
				EnsureSelEndVisible();
				this->InvalidateRect(&rcDraw);
			}
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
			EnsureSelEndVisible();
			this->Invalidate();
		}
		break;
	case IDR_MENU_UIEDIT_SELALL:
		{
			m_cpSelBegin.CharIndex = 0;
			m_cpSelBegin.LineIndex = 0;
			m_cpSelEnd.LineIndex = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			m_cpSelEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount;
			EnsureSelEndVisible();
			this->Invalidate();
		}
		break;
	default:
		break;
	}
	return true;
}

void CMultiLineEditUI::OnLButtonUp(UINT nFlags, POINT pt)
{
}

void CMultiLineEditUI::OnLButtonDown(UINT nFlags, POINT pt)
{
	bool flag = m_bhasCaret;
	if (m_cpSelBegin != m_cpSelEnd)
	{
		RECT rcDraw;
		POINT beginPt, endPt;
		CharPos beginPos, endPos;
		beginPos = min(m_cpSelBegin, m_cpSelEnd);
		endPos = max(m_cpSelBegin, m_cpSelEnd);
		CharPos2Point(beginPos, beginPt);
		CharPos2Point(endPos, endPt);
		rcDraw.left = 0;
		rcDraw.top = 0/*beginPt.y - m_nLineHeight*/;
		rcDraw.right = m_uClientWidth;
		rcDraw.bottom = endPt.y + m_nLineHeight;
		m_bhasCaret =false;
		this->InvalidateRect(&rcDraw);
	}
	CharPos pos;
	Point2CharPos(pt, pos);
	m_cpSelEnd = pos;
	m_cpSelBegin = m_cpSelEnd;
	m_bhasCaret = flag;
	this->SetCaretPos();
	this->Invalidate();
}

void CMultiLineEditUI::OnRButtonUp(UINT nFlags, POINT pt)
{
	if(!m_bMenuEnable) return;
	CMenuUI* pMenu = CMenuUI::Load(IDR_MENU_UIEDIT);
	if (pMenu == NULL) return;
	CMenuUI *pSubMenu = NULL;
	if(m_bReadOnly)
	{
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_CUT, BY_COMMAND);
		if(pSubMenu) pMenu->Remove(pSubMenu);
		pSubMenu = pMenu->GetItem(IDR_MENU_UIEDIT_PASTE, BY_COMMAND);
		if(pSubMenu) pMenu->Remove(pSubMenu);
	}
	if(m_cpSelBegin == m_cpSelEnd)
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

void CMultiLineEditUI::OnLButtonDblClk(UINT nFlags, POINT pt)
{
	CharPos pos;
	Point2CharPos(pt, pos);
	m_cpSelBegin.CharIndex = 0;
	m_cpSelBegin.LineIndex = FindPrevCrlfLine(pos.LineIndex) + 1;
	m_cpSelEnd.LineIndex = FindNextCrlfLine(pos.LineIndex);
	m_cpSelEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount;

	RECT rcDraw;
	if(this->EnsureSelEndVisible()){
		this->Invalidate();
	} else {
		POINT ptBegin, ptEnd;
		CharPos2Point(m_cpSelBegin, ptBegin);
		CharPos2Point(m_cpSelEnd, ptEnd);
		rcDraw.left = 0;
		rcDraw.top = 0;//ptBegin.y
		rcDraw.right = m_uClientWidth;
		rcDraw.bottom = ptEnd.y + m_nLineHeight;
		this->InvalidateRect(&rcDraw);
	}
}

void CMultiLineEditUI::OnMouseMove(UINT nFlags, POINT pt)  //鼠标拖动
{
	if(nFlags & MK_LBUTTON)
	{
		m_cpPrevSelEnd = m_cpSelEnd;
		Point2CharPos(pt, m_cpSelEnd);
		if(m_cpPrevSelEnd != m_cpSelEnd)
		{
			RECT rcDraw;	
			rcDraw = this->GetSelectReDrawRect();
			rcDraw.top = 0;
			if(EnsureSelEndVisible()) this->Invalidate();
			else this->InvalidateRect(&rcDraw);
		}
	}
}

void CMultiLineEditUI::OnKeyDown(UINT nChar, UINT nFlags)
{
	if (nChar == 'V')
	{
		if(m_bReadOnly) return;
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
			this->Paste();
		return;
	}
	else if (nChar == 'C')
	{
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
			this->Copy();
		return;
	}
	else if (nChar == 'X')
	{
		if(m_bReadOnly) return;
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
		{
			if(m_cpSelEnd != m_cpSelBegin)
			{
				this->Copy();
				CharPos BeforeInsertPos = max(m_cpSelEnd, m_cpSelBegin);
				unsigned uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
				DelSelRange();
				unsigned uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
				RECT rcDraw = this->GetDeleteReDrawRect(BeforeInsertPos, m_cpSelEnd, 
					!!(uAfterInsertLineCount-uBeforeInsertLineCount));
				if(!m_bAutoSize)
				{
					if(EnsureSelEndVisible()) this->Invalidate();
					else this->InvalidateRect(&rcDraw);
				}
				else
				{
					EnsureSelEndVisible();
					this->InvalidateRect(&rcDraw, true);
				}
			}
		}
		return;
	}
	else if (nChar == 'A')
	{
		SHORT value1 = GetKeyState(VK_LCONTROL);
		SHORT value2 = GetKeyState(VK_RCONTROL);
		if ((value1 & 0x8000) || (value2 & 0x8000))
		{
			m_cpSelBegin.CharIndex = 0;
			m_cpSelBegin.LineIndex = 0;
			m_cpSelEnd.LineIndex = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			m_cpSelEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount;
			EnsureSelEndVisible();
			this->Invalidate();
		}
		return;
	}
	else if (nChar == VK_HOME)
	{
		m_cpSelEnd.CharIndex = 0;
		m_cpSelBegin = m_cpSelEnd;
		EnsureSelEndVisible();
		this->Invalidate();
		return;
	}
	else if (nChar == VK_END)
	{
		m_cpSelEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount;
		m_cpSelBegin = m_cpSelEnd;
		EnsureSelEndVisible();
		this->Invalidate();
		return;
	}
	else if (nChar == VK_BACK || nChar == VK_DELETE)
	{
		if (m_bReadOnly) return;
		if(m_cpSelEnd == m_cpSelBegin)
		{
			CharPos BeforeInsertPos = m_cpSelEnd;
			unsigned uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			if(nChar == VK_BACK) LeftSelEnd();
			else RightSelEnd();
			DelSelRange();
			unsigned uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			RECT rcDraw = this->GetDeleteReDrawRect(BeforeInsertPos, m_cpSelEnd, 
				!!(uAfterInsertLineCount-uBeforeInsertLineCount));
			if(!m_bAutoSize)
			{
				if(EnsureSelEndVisible()) this->Invalidate();
				else this->InvalidateRect(&rcDraw);
			}
			else
			{
				EnsureSelEndVisible();
				this->InvalidateRect(&rcDraw, true);
			}
		}
		else
		{
			CharPos BeforeInsertPos = m_cpSelEnd;
			unsigned uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			DelSelRange();
			unsigned uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			RECT rcDraw = this->GetDeleteReDrawRect(BeforeInsertPos, m_cpSelEnd, 
				!!(uAfterInsertLineCount-uBeforeInsertLineCount));
			if(!m_bAutoSize)
			{
				if(EnsureSelEndVisible()) this->Invalidate();
				else this->InvalidateRect(&rcDraw);
			}
			else
			{
				EnsureSelEndVisible();
				this->InvalidateRect(&rcDraw, true);
			}
		}
		return;
	}

	if (nChar >= VK_LEFT && nChar <= VK_DOWN)
	{
		SHORT value = GetKeyState(VK_SHIFT);
		if (value & 0x8000)
		{
			m_cpPrevSelEnd = m_cpSelEnd;
			switch(nChar)
			{
			case VK_LEFT:
				LeftSelEnd();
				break;
			case VK_RIGHT:
				RightSelEnd();
				break;
			case VK_UP:
				UpSelEnd();
				break;
			case VK_DOWN:
				DownSelEnd();
				break;
			}
			if(this->EnsureSelEndVisible())
				this->Invalidate();
			else
			{
				RECT rcDraw;
				rcDraw = this->GetSelectReDrawRect();
				this->InvalidateRect(&rcDraw);
			}
			return;
		}
		else
		{
			if (m_cpSelBegin != m_cpSelEnd)
			{
				RECT rcDraw;
				POINT beginPt, endPt;
				CharPos beginPos, endPos;

				beginPos= min(m_cpSelBegin, m_cpSelEnd);
				endPos = max(m_cpSelBegin, m_cpSelEnd);
				CharPos2Point(beginPos, beginPt);
				CharPos2Point(endPos, endPt);
				rcDraw.left = 0;
				rcDraw.top = beginPt.y;
				rcDraw.right = m_uClientWidth;
				rcDraw.bottom = endPt.y + m_nLineHeight;
				this->InvalidateRect(&rcDraw);
			}
			if (m_cpSelEnd < m_cpSelBegin)
			{
				switch (nChar)
				{
				case VK_LEFT:
					{
						m_cpSelBegin = m_cpSelEnd;
					}
					break;
				case VK_RIGHT:
					{
						m_cpSelEnd = m_cpSelBegin;
					}
					break;
				case VK_UP:
					{
						UpSelEnd();
						m_cpSelBegin = m_cpSelEnd;
					}
					break;
				case VK_DOWN:
					{
						DownSelEnd();
						m_cpSelBegin = m_cpSelEnd;
					}
					break;
				}
			}
			else if (m_cpSelEnd > m_cpSelBegin)
			{
				switch(nChar)
				{
				case VK_LEFT:
					{
						m_cpSelEnd = m_cpSelBegin;
					}
					break;
				case VK_RIGHT:
					{
						m_cpSelBegin = m_cpSelEnd;
					}
					break;
				case VK_UP:
					{
						UpSelEnd();
						m_cpSelBegin = m_cpSelEnd;
					}
					break;
				case VK_DOWN:
					{
						DownSelEnd();
						m_cpSelBegin = m_cpSelEnd;
					}
					break;
				}
			}
			else
			{
				switch(nChar)
				{
				case VK_LEFT:
					LeftSelEnd();
					break;
				case VK_RIGHT:
					RightSelEnd();
					break;
				case VK_UP:
					UpSelEnd();
					break;
				case VK_DOWN:
					DownSelEnd();
					break;
				}
				m_cpSelBegin = m_cpSelEnd;
			}
			EnsureSelEndVisible();
			Invalidate();
		}
	}
}

void CMultiLineEditUI::OnChar(UINT nChar, UINT nFlags)
{
	if (m_bReadOnly) return;
	if (nChar < VK_SPACE && nChar != VK_RETURN && nChar != VK_ESCAPE && nChar != '\t')
		return;
	if (nChar == VK_TAB || nChar == VK_ESCAPE)
		return;
	SHORT value1 = GetKeyState(VK_LCONTROL);
	SHORT value2 = GetKeyState(VK_RCONTROL);
	if ((value1 & 0x8000) || (value2 & 0x8000))
		return;
	RECT rcDraw1 = {0}, rcPrevDelItem;
	if(m_cpSelBegin != m_cpSelEnd)
	{
		CharPos BeforeInsertPos = max(m_cpSelEnd, m_cpSelBegin);
		unsigned uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
		DelSelRange();
		rcPrevDelItem = m_rcPrevItem;
		unsigned uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
		rcDraw1 = this->GetDeleteReDrawRect(BeforeInsertPos, m_cpSelEnd, 
			!!(uAfterInsertLineCount-uBeforeInsertLineCount));
	}
	TCHAR buf[10] = {0};
	if (nChar == VK_RETURN) _stprintf(buf, _T("%s"), crlf);
	else _stprintf(buf, _T("%c"), nChar);

	CharPos BeforeInsertPos = m_cpSelEnd;
	unsigned uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
	Insert(buf);
	unsigned uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
	RECT rcDraw2 = this->GetInsertReDrawRect(BeforeInsertPos, m_cpSelEnd, 
		!!(uAfterInsertLineCount-uBeforeInsertLineCount));
	RECT rcDraw;
	::UnionRect(&rcDraw, &rcDraw1, &rcDraw2);
	if(!m_bAutoSize)
	{
		if(EnsureSelEndVisible()) this->Invalidate();
		else this->InvalidateRect(&rcDraw);
	}
	else
	{
		if(rcDraw1.bottom - rcDraw1.top)
		{
			EnsureSelEndVisible();
			GetWindow()->Invalidate(&rcPrevDelItem, true);
		}
		else
		{
			if(EnsureSelEndVisible()) this->Invalidate(true);
			else this->InvalidateRect(&rcDraw, true);
		}
	}
}

void CMultiLineEditUI::LeftSelEnd()
{
	if (m_cpSelEnd.CharIndex == 0 && m_cpSelEnd.LineIndex == 0)
	{
		return;
	}
	else if (m_cpSelEnd.CharIndex == 0 && m_cpSelEnd.LineIndex > 0 )
	{
		m_cpSelEnd.LineIndex -= 1;
		m_cpSelEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount;
	}
	else
	{
		m_cpSelEnd.CharIndex -= 1;
	}
}

void CMultiLineEditUI::RightSelEnd()
{
	if (m_cpSelEnd.LineIndex == GetUpperBound(m_pEditDataImpl->m_lineInfoArray)
		&& m_cpSelEnd.CharIndex == m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount)
	{
		return;
	}
	else if (m_cpSelEnd.CharIndex == m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount)
	{
		m_cpSelEnd.CharIndex = 0;
		m_cpSelEnd.LineIndex += 1;
	}
	else
	{
		m_cpSelEnd.CharIndex += 1;
	}
}

void CMultiLineEditUI::UpSelEnd()
{
	if (m_cpSelEnd.LineIndex == 0)
	{
		//m_cpSelEnd.CharIndex = 0;
	}
	else if (m_cpSelEnd.LineIndex > 0 &&
		m_cpSelEnd.CharIndex > m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex-1]->CharsCount)
	{
		m_cpSelEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex-1]->CharsCount;
		m_cpSelEnd.LineIndex -= 1;
	}
	else if (m_cpSelEnd.LineIndex > 0 &&
		m_cpSelEnd.CharIndex <= m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex-1]->CharsCount)
	{
		m_cpSelEnd.LineIndex -= 1;
	}
}

void CMultiLineEditUI::DownSelEnd()
{
	if (m_cpSelEnd.LineIndex == GetUpperBound(m_pEditDataImpl->m_lineInfoArray))
	{
		//m_cpSelEnd.CharIndex = m_lineInfoArray[m_cpSelEnd.LineIndex]->CharsCount;
	}
	else if (m_cpSelEnd.LineIndex < GetUpperBound(m_pEditDataImpl->m_lineInfoArray)
		&& m_cpSelEnd.CharIndex > m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex+1]->CharsCount)
	{
		m_cpSelEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex+1]->CharsCount;
		m_cpSelEnd.LineIndex += 1;
	}
	else if (m_cpSelEnd.LineIndex < GetUpperBound(m_pEditDataImpl->m_lineInfoArray)
		&& m_cpSelEnd.CharIndex <= m_pEditDataImpl->m_lineInfoArray[m_cpSelEnd.LineIndex+1]->CharsCount)
	{
		m_cpSelEnd.LineIndex += 1;
	}
}

void CMultiLineEditUI::CharPos2Point(const CharPos &pos, POINT &pt)
{
	memset(&pt, 0, sizeof(POINT));
	if (!m_hParentWnd)
	{
		return;
	}
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle())
	{
		return;
	}
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());
	UINT uAlignFlag = GetEditStyle()->GetAlign();

	RECT rcEdit=this->GetInterRect();
	int iEditWidth = rcEdit.right - rcEdit.left;
	int iEditHeight = rcEdit.bottom - rcEdit.top;
	int iTextHeight = GetTextHeight();

	//int x,y;
	pt.y = (pos.LineIndex - m_nFirstVisibleLine) * m_nLineHeight;
	if (iTextHeight < iEditHeight)
	{
		int y;
		if (uAlignFlag & DT_TOP)
		{
			y = pt.y;
		}
		else if (uAlignFlag & DT_VCENTER)
		{
			y = pt.y+(int)((iEditHeight-iTextHeight)/2);
		}
		else if (uAlignFlag & DT_BOTTOM)
		{
			y = pt.y+(iEditHeight-iTextHeight);
		}
		else
		{
			y = pt.y;
		}
		pt.y = y;
	}

	CharPos tmpPos;
	tmpPos = pos;
	tmpPos.CharIndex = 0;
	unsigned beginIndex;
	CharPos2Index(tmpPos, beginIndex);

	LPCTSTR pStart = m_strText.c_str() + beginIndex;
	pt.x = GetStringLine(hDC, pStart, pos.CharIndex);
	int iTextWidth = GetStringLine(hDC, pStart, m_pEditDataImpl->m_lineInfoArray[pos.LineIndex]->CharsCount);

	int x;
	if (uAlignFlag & DT_LEFT)
	{
		x = pt.x;
	}
	else if (uAlignFlag & DT_CENTER)
	{
		x = pt.x+(int)((iEditWidth-iTextWidth)/2);
	}
	else if (uAlignFlag & DT_RIGHT)
	{
		x = pt.x+(iEditWidth-iTextWidth);
	}
	else
	{
		x = pt.x;
	}
	pt.x = x;

	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);
	return;
}

void CMultiLineEditUI::Point2CharPos(const POINT &pt, CharPos &pos)
{
	memset(&pos, 0, sizeof(CharPos));
	if (!m_hParentWnd)
	{
		return;
	}
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle())
	{
		return;
	}
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());
	UINT uAlignFlag = GetEditStyle()->GetAlign();

	RECT rcEdit=this->GetInterRect();
	int iEditWidth = rcEdit.right - rcEdit.left;
	int iEditHeight = rcEdit.bottom - rcEdit.top;
	int iTextHeight = GetTextHeight();

	int y = pt.y;
	if (iTextHeight < iEditHeight)
	{
		if (uAlignFlag & DT_TOP)
		{
			y = pt.y;
		}
		else if (uAlignFlag & DT_VCENTER)
		{
			y = pt.y-(int)((iEditHeight-iTextHeight)/2);
		}
		else if (uAlignFlag & DT_BOTTOM)
		{
			y = pt.y-(iEditHeight-iTextHeight);
		}
		else
		{
			y = pt.y;
		}
	}
	int lineCount = 0;
	lineCount = y / m_nLineHeight;
	lineCount += m_nFirstVisibleLine;
	if (lineCount > (int)GetUpperBound(m_pEditDataImpl->m_lineInfoArray))
	{
		lineCount = (int)GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
	}
	else if (lineCount < 0)
	{
		lineCount = 0;
	}
	pos.LineIndex = lineCount;

	CharPos beginPos;
	beginPos.LineIndex = pos.LineIndex;
	beginPos.CharIndex = 0;
	unsigned beginIndex;
	CharPos2Index(beginPos, beginIndex);

	LPTSTR pStart = const_cast<LPTSTR>(m_strText.c_str()) + beginIndex;
	LPTSTR pEnd = pStart + (m_pEditDataImpl->m_lineInfoArray[pos.LineIndex]->CharsCount);
	LPTSTR pNext;
	TCHAR tmpChar = *pEnd;
	*pEnd = (TCHAR)0;
	int iTextWidth = GetStringLine(hDC, pStart, m_pEditDataImpl->m_lineInfoArray[pos.LineIndex]->CharsCount);
	int x = pt.x;
	if (uAlignFlag & DT_LEFT)
	{
		x = pt.x;
	}
	else if (uAlignFlag & DT_CENTER)
	{
		x = pt.x-(int)((iEditWidth-iTextWidth)/2);
	}
	else if (uAlignFlag & DT_RIGHT)
	{
		x = pt.x-(iEditWidth-iTextWidth);
	}
	int iLen = x;
	while (TRUE)
	{
		pNext = CharNext(pStart);
		int charCount = pNext - pStart;
		if(!charCount) break;
		int w = GetStringLine(hDC, pStart, charCount);
		if(iLen < w/2) break;
		iLen -= w;
		pos.CharIndex += charCount;
		pStart = pNext;
	}
	*pEnd = tmpChar;

	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);
	return;
}

void CMultiLineEditUI::CharPos2Index(const CharPos &pos, unsigned &nIndex)
{
	memset(&nIndex, 0, sizeof(unsigned));
	CLineInfoArray::const_iterator it = m_pEditDataImpl->m_lineInfoArray.begin();
	CLineInfoArray::const_iterator itEnd = it + pos.LineIndex;
	for (; it!=itEnd; ++it)
		nIndex += ((*it)->CharsCount + (unsigned)((*it)->bParaEnd));
	nIndex += pos.CharIndex;
	return;
}

void CMultiLineEditUI::Index2CharPos(const unsigned &nIndex, CharPos &pos)
{
	memset(&pos, 0, sizeof(CharPos));
	int tmp = nIndex;
	CLineInfoArray::const_iterator it = m_pEditDataImpl->m_lineInfoArray.begin();
	for (; it!=m_pEditDataImpl->m_lineInfoArray.end(); ++it)
	{
		int LineCharCounts = (int)((*it)->CharsCount + (unsigned)((*it)->bParaEnd));
		if ((tmp -= LineCharCounts) > 0)
		{
			++pos.LineIndex;
		}
		else if (tmp <= 0)
		{
			pos.CharIndex = (tmp + LineCharCounts);
			break;
		}
	}
	return;
}

void CMultiLineEditUI::AdjustFromLineToParaEnd(const unsigned LineNum, ADJUSTMODE mode)
{
	unsigned CharsCount = 0;
	for (int i=0; i!=LineNum; ++i)
		CharsCount += (m_pEditDataImpl->m_lineInfoArray[i]->CharsCount + (unsigned)(m_pEditDataImpl->m_lineInfoArray[i]->bParaEnd));

	if(mode == DEL_AND_INSERT)
	{
		unsigned paraLastLineNum = FindNextCrlfLine(LineNum);
		unsigned nCount = paraLastLineNum - LineNum + 1;
		RemoveLineInfo(LineNum, nCount);
	}

	LPCTSTR lpszText = m_strText.c_str() + CharsCount;
	LPTSTR pStart = const_cast<LPTSTR>(lpszText);
	LPTSTR pEnd = NULL;
	pEnd = _tcsstr(pStart, crlf);

	{
		if (pEnd) *pEnd = (TCHAR)0; 

		if (!m_hParentWnd) return;
		HDC hDC = ::GetDC(m_hParentWnd);
		if (!hDC || !GetEditStyle()) return;
		HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());

		unsigned nStartInsertLine = LineNum;
		while (TRUE)
		{
			int tmp = CutStringForOneLine(hDC, lpszText);
			LPLINEINFO pLineInfo = new LINEINFO;
			pLineInfo->CharsCount = tmp;

			if (tmp == _tcslen(lpszText))
			{
				if(pEnd) pLineInfo->bParaEnd = MYTRUE;
				InsertAt(m_pEditDataImpl->m_lineInfoArray, nStartInsertLine++, pLineInfo);
				break;
			}
			InsertAt(m_pEditDataImpl->m_lineInfoArray, nStartInsertLine++, pLineInfo);
			lpszText += tmp;
		}

		::SelectObject(hDC, hOldFont);
		::ReleaseDC(m_hParentWnd, hDC);

		if(pEnd) *pEnd = crlf[0];
	}
}

void CMultiLineEditUI::AdjustAllLineInfo()
{
	if (!m_hParentWnd) return;
	HDC hDC = ::GetDC(m_hParentWnd);
	if (!hDC || !GetEditStyle()) return;
	HFONT hOldFont = (HFONT)::SelectObject(hDC, GetEditStyle()->GetFontObj()->GetFont());

	RemoveLineInfo();
	LPTSTR pStart = const_cast<LPTSTR>(m_strText.c_str());
	LPTSTR pEnd;
	unsigned nStartInsertLine = 0;
	while ((pEnd = _tcsstr(pStart, crlf))!=NULL)
	{
		TCHAR tmpChar = *pEnd;
		*pEnd = (TCHAR)0;
		while (TRUE)
		{
			int tmp = CutStringForOneLine(hDC, pStart);
			LPLINEINFO pLineInfo = new LINEINFO;
			pLineInfo->CharsCount = tmp;
			if (tmp == _tcslen(pStart))
			{
				pLineInfo->bParaEnd = MYTRUE;
				InsertAt(m_pEditDataImpl->m_lineInfoArray, nStartInsertLine++, pLineInfo);
				break;
			}
			InsertAt(m_pEditDataImpl->m_lineInfoArray, nStartInsertLine++, pLineInfo);
			pStart += tmp;
		}
		*pEnd = tmpChar;
		pStart = pEnd + crlf_Count;
	}
	while (TRUE)
	{
		int tmp = CutStringForOneLine(hDC, pStart);
		LPLINEINFO pLineInfo = new LINEINFO;
		pLineInfo->CharsCount = tmp;
		if (tmp == _tcslen(pStart))
		{
			InsertAt(m_pEditDataImpl->m_lineInfoArray, nStartInsertLine++, pLineInfo);
			break;
		}
		InsertAt(m_pEditDataImpl->m_lineInfoArray, nStartInsertLine++, pLineInfo);
		pStart += tmp;
	}
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hParentWnd, hDC);
}

int CMultiLineEditUI::CutStringForOneLine(HDC hDC, const TCHAR *lpszText)
{
	assert(hDC);
	LPCTSTR pStart = lpszText;
	LPCTSTR pNext = NULL;
	int nCount = 0;
	tstring::size_type nLen = 0;
	while(true)
	{
		pNext = CharNext(pStart);
		if (pNext == pStart) break;
		int x = GetStringLine(hDC, pStart, 1);
		nLen += x;
		++nCount;
		if (nLen < m_uClientWidth)
		{
			pStart = pNext;
			continue;
		}
		else if (nLen == m_uClientWidth)
		{
			break;
		}
		else if (nLen > m_uClientWidth)
		{
			if (x < m_uClientWidth)
			{
				--nCount;
			}
			break;
		}
	}
	return nCount;
}

void CMultiLineEditUI::RemoveLineInfo(unsigned nStartLine, unsigned nCount)
{
	if(!nCount) return;
	CLineInfoArray::iterator itBegin = m_pEditDataImpl->m_lineInfoArray.begin() + nStartLine;
	CLineInfoArray::iterator itEnd = itBegin + nCount;
	for_each(itBegin, itEnd, del);
	RemoveAt(m_pEditDataImpl->m_lineInfoArray, nStartLine, nCount);
}

void CMultiLineEditUI::RemoveLineInfo(unsigned nStartLine)
{
	CLineInfoArray::iterator itBegin = m_pEditDataImpl->m_lineInfoArray.begin() + nStartLine;
	for_each(itBegin, m_pEditDataImpl->m_lineInfoArray.end(), del);
	RemoveAt(m_pEditDataImpl->m_lineInfoArray, nStartLine);
}

unsigned CMultiLineEditUI::FindNextCrlfLine(unsigned nStartLine)
{
	CLineInfoArray::const_iterator itBegin = m_pEditDataImpl->m_lineInfoArray.begin();
	CLineInfoArray::const_iterator it = m_pEditDataImpl->m_lineInfoArray.begin() + nStartLine;
	for (; it!=m_pEditDataImpl->m_lineInfoArray.end(); ++it)
	{
		if ((*it)->bParaEnd == MYTRUE)
			return (unsigned)(it - itBegin);
	}
	return (unsigned)(it - itBegin - 1);
}

int CMultiLineEditUI::FindPrevCrlfLine(unsigned nStartLine)
{
	int lineNum = -1;
	for (int i=nStartLine-1; i!=-1; --i)
	{
		if(m_pEditDataImpl->m_lineInfoArray[i]->bParaEnd == MYTRUE){
			lineNum = i;
			break;
		}
	}
	return lineNum;
}

void CMultiLineEditUI::Render(IRenderDC* pRenderDC, RECT &rcPaint)
{
	if (!GetEditStyle()) return;
	HDC hDestDC = pRenderDC->GetDC();

	if(m_nTransparent != 1 && (m_nTransparent == 0 || !(m_nTransparent == 2 && IsKillFocus())))
		__super::Render(pRenderDC, rcPaint);

	ClipObj clip;
	ClipObj::GenerateClipWithAnd(hDestDC, GetInterRect(), clip);
	
	RECT rcClient;
	rcClient = this->GetInterRect();
	HFONT hOldFont = (HFONT)::SelectObject(hDestDC, GetEditStyle()->GetFontObj()->GetFont());

	if(m_bReadOnly && m_nTransparent == 0)
	{
		HBRUSH hBr = CreateSolidBrush(COLOR_READONLY_BK);
		FillRect(hDestDC, &GetInterRect(), hBr);
		::DeleteObject(hBr);
	}

	if (IsKillFocus() && m_strText.empty() && !m_strEmptyTipsText.empty())
	{
		//UINT align = DT_WORDBREAK|DT_EDITCONTROL|DT_TOP|DT_LEFT_EX|DT_NOPREFIX;
		if (GetEmptyEditStyle() != NULL)
		{
			UINT align = GetEditStyle()->GetAlign();
			pRenderDC->DrawText(GetEmptyEditStyle()->GetFontObj(), rcClient, m_strEmptyTipsText.c_str(), GetEmptyEditStyle()->GetTextColor(), align, GetEmptyEditStyle()->GetFontEffect());
		}
	}
	else
	{
		int oldMode = ::SetBkMode(hDestDC, TRANSPARENT);
		RECT rcDraw;
		::IntersectRect(&rcDraw, &rcClient, &rcPaint);
		::OffsetRect(&rcDraw, -(int)rcClient.left, -(int)rcClient.top);
		CharPos selBegin, selEnd;

		bool flag = (m_cpSelBegin == m_cpSelEnd);
		if(!flag)
		{
			selBegin = min(m_cpSelBegin, m_cpSelEnd);
			selEnd = max(m_cpSelBegin, m_cpSelEnd);
		}
		unsigned nLineBegin = rcDraw.top / m_nLineHeight;
		nLineBegin += m_nFirstVisibleLine;
		unsigned nLineEnd = rcDraw.bottom / m_nLineHeight;
		nLineEnd += m_nFirstVisibleLine;
		unsigned lastLineNum = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
		nLineBegin = min(nLineBegin, lastLineNum);
		nLineEnd = min(nLineEnd, lastLineNum);
		CharPos iBegin, iEnd;
		iBegin.CharIndex = 0;
		iBegin.LineIndex = nLineBegin;
		iEnd.LineIndex = nLineEnd;
		iEnd.CharIndex = m_pEditDataImpl->m_lineInfoArray[nLineEnd]->CharsCount;

		if(flag)
		{
			COLORREF oldTextColor = ::SetTextColor(hDestDC, GetEditStyle()->GetTextColor());
			COLORREF oldBkColor = ::SetBkColor(hDestDC, COLOR_INPUT_BK);
			this->DrawRange(pRenderDC, iBegin, iEnd);
			::SetTextColor(hDestDC, oldTextColor);
			::SetBkColor(hDestDC, oldBkColor);
		}
		else
		{
			selBegin = max(selBegin, iBegin);
			selEnd = min(selEnd, iEnd);
			COLORREF oldTextColor = ::SetTextColor(hDestDC, GetEditStyle()->GetTextColor());
			COLORREF oldBkColor = ::SetBkColor(hDestDC, COLOR_INPUT_BK);
			this->DrawRange(pRenderDC, iBegin, selBegin);
			::SetBkMode(hDestDC, oldMode);
			::SetBkColor(hDestDC, COLOR_SELECT_BK);
			::SetTextColor(hDestDC, COLOR_SELECT_TEXT);
			this->DrawRange(pRenderDC, selBegin, selEnd);
			oldMode = ::SetBkMode(hDestDC, TRANSPARENT);
			::SetTextColor(hDestDC, GetEditStyle()->GetTextColor());
			::SetBkColor(hDestDC, COLOR_INPUT_BK);
			this->DrawRange(pRenderDC, selEnd, iEnd);
			::SetTextColor(hDestDC, oldTextColor);
			::SetBkColor(hDestDC, oldBkColor);
		}
		::SetBkMode(hDestDC, oldMode);
	}
	if(m_bhasCaret && IsFocused())
	{
		CCaretUI::GetInstance()->Render(pRenderDC, rcPaint);
	}

	::SelectObject(hDestDC, hOldFont);
}

RECT CMultiLineEditUI::GetInsertReDrawRect(const CharPos &beforeChangePos, const CharPos &afterChangePos, const bool bAddLine)
{
	RECT rc = {0};
	if (bAddLine)
	{
		POINT pt;
		CharPos2Point(beforeChangePos, pt);
		if(!m_bAutoSize)
		{
			rc.left = 0;
			rc.top = /*pt.y*/0;
			rc.right = m_uClientWidth;
			rc.bottom = m_uClientHeight;
		}
		else
		{
			rc.left = 0 - m_rcInset.left;
			rc.top = /*pt.y*/0;
			rc.right = m_uClientWidth + m_rcInset.right;
			rc.bottom = m_uClientHeight + m_rcInset.bottom;
		}
	}
	else
	{
		POINT ptBegin, ptEnd;
		if (m_pEditDataImpl->m_lineInfoArray[beforeChangePos.LineIndex]->bParaEnd == MYTRUE
			&& beforeChangePos.LineIndex == afterChangePos.LineIndex
			&& afterChangePos.CharIndex == m_pEditDataImpl->m_lineInfoArray[afterChangePos.LineIndex]->CharsCount)
		{
			CharPos2Point(beforeChangePos, ptBegin);
			CharPos2Point(afterChangePos, ptEnd);
			rc.left = ptBegin.x;
			rc.top = ptBegin.y;
			rc.right = ptEnd.x;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
		else
		{
			CharPos pos;
			pos.CharIndex = 0;
			pos.LineIndex = FindNextCrlfLine(afterChangePos.LineIndex);
			CharPos2Point(beforeChangePos, ptBegin);
			CharPos2Point(pos, ptEnd);
			rc.left = 0;
			rc.top = ptBegin.y;
			rc.right = m_uClientWidth;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
	}
	rc.left = 0;
	rc.top = 0;
	rc.right = m_uClientWidth + m_rcInset.right;
	rc.bottom = m_uClientHeight + m_rcInset.bottom;
	return rc;
}

RECT CMultiLineEditUI::GetDeleteReDrawRect(const CharPos &beforeChangePos, const CharPos &afterChangePos, const bool bSubLine)
{
	RECT rc = {0};
	if (bSubLine)
	{
		POINT pt;
		CharPos2Point(afterChangePos, pt);
		if(!m_bAutoSize)
		{
			rc.left = 0;
			rc.top = /*pt.y*/0;
			rc.right = m_uClientWidth;
			rc.bottom = m_uClientHeight;
		}
		else
		{
			rc.left = 0 - m_rcInset.left;
			rc.top = /*pt.y*/0;
			rc.right =m_uClientWidth + m_rcInset.right;
			rc.bottom = m_rcPrevItem.bottom - m_rcPrevItem.top ;
		}
	}
	else
	{
		if (beforeChangePos.LineIndex == afterChangePos.LineIndex
			&& m_pEditDataImpl->m_lineInfoArray[afterChangePos.LineIndex]->bParaEnd == MYTRUE)
		{
			POINT pt;
			CharPos2Point(afterChangePos, pt);
			rc.left = /*pt.x*/0;
			rc.top = /*pt.y*/0;
			rc.right = m_uClientWidth;
			rc.bottom = pt.y + m_nLineHeight;
		}
		else
		{
			CharPos pos;
			POINT ptBegin, ptEnd;
			pos.CharIndex = 0;
			pos.LineIndex = FindNextCrlfLine(afterChangePos.LineIndex);
			CharPos2Point(afterChangePos, ptBegin);
			CharPos2Point(pos, ptEnd);
			rc.left = 0;
			rc.top = /*ptBegin.y*/0;
			rc.right = m_uClientWidth;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
	}
	if (rc.top < 0)
		rc.top = 0;
	return rc;
}

RECT CMultiLineEditUI::GetSelectReDrawRect()
{
	RECT rc = {0};
	POINT ptBegin, ptEnd;
	if (m_cpPrevSelEnd.LineIndex == m_cpSelEnd.LineIndex
		&& m_cpSelEnd.LineIndex == m_cpSelBegin.LineIndex)
	{
		if (m_cpPrevSelEnd.CharIndex == m_cpSelBegin.CharIndex)
		{
			if (m_cpSelEnd.CharIndex < m_cpSelBegin.CharIndex)
			{
				CharPos2Point(m_cpSelEnd, ptBegin);
				CharPos2Point(m_cpSelBegin, ptEnd);
				rc.left = ptBegin.x;
				rc.top = ptBegin.y;
				rc.right = ptEnd.x;
				rc.bottom = ptEnd.y + m_nLineHeight;
			}
			else
			{
				CharPos2Point(m_cpSelBegin, ptBegin);
				CharPos2Point(m_cpSelEnd, ptEnd);
				rc.left = ptBegin.x;
				rc.top = ptBegin.y;
				rc.right = ptEnd.x;
				rc.bottom = ptEnd.y + m_nLineHeight;
			}
		}
		else if (m_cpPrevSelEnd.CharIndex < m_cpSelBegin.CharIndex
			&& m_cpPrevSelEnd.CharIndex > m_cpSelEnd.CharIndex)
		{
			CharPos2Point(m_cpSelEnd, ptBegin);
			CharPos2Point(m_cpPrevSelEnd, ptEnd);
			rc.left = ptBegin.x;
			rc.top = ptBegin.y;
			rc.right = ptEnd.x;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
		else if (m_cpPrevSelEnd.CharIndex < m_cpSelBegin.CharIndex
			&& m_cpPrevSelEnd.CharIndex < m_cpSelEnd.CharIndex)
		{
			CharPos2Point(m_cpPrevSelEnd, ptBegin);
			CharPos2Point(m_cpSelEnd, ptEnd);
			rc.left = ptBegin.x;
			rc.top = ptBegin.y;
			rc.right = ptEnd.x;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
		else if (m_cpPrevSelEnd.CharIndex > m_cpSelBegin.CharIndex
			&& m_cpPrevSelEnd.CharIndex > m_cpSelEnd.CharIndex)
		{
			CharPos2Point(m_cpSelEnd, ptBegin);
			CharPos2Point(m_cpPrevSelEnd, ptEnd);
			rc.left = ptBegin.x;
			rc.top = ptBegin.y;
			rc.right = ptEnd.x;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
		else if (m_cpPrevSelEnd.CharIndex > m_cpSelBegin.CharIndex
			&& m_cpPrevSelEnd.CharIndex < m_cpSelEnd.CharIndex)
		{
			CharPos2Point(m_cpPrevSelEnd, ptBegin);
			CharPos2Point(m_cpSelEnd, ptEnd);
			rc.left = ptBegin.x;
			rc.top = ptBegin.y;
			rc.right = ptEnd.x;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
	}
	else if (m_cpPrevSelEnd.LineIndex == m_cpSelBegin.LineIndex
		&& m_cpPrevSelEnd.LineIndex != m_cpSelEnd.LineIndex)
	{
		if (m_cpSelEnd.LineIndex < m_cpPrevSelEnd.LineIndex)
		{
			CharPos2Point(m_cpSelEnd, ptBegin);
			CharPos2Point(m_cpPrevSelEnd, ptEnd);
			rc.left = 0;
			rc.top = ptBegin.y;
			rc.right = m_uClientWidth;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
		else if (m_cpSelEnd.LineIndex > m_cpPrevSelEnd.LineIndex)
		{
			CharPos2Point(m_cpPrevSelEnd, ptBegin);
			CharPos2Point(m_cpSelEnd, ptEnd);
			rc.left = 0;
			rc.top = ptBegin.y;
			rc.right = m_uClientWidth;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
	}
	else if (m_cpPrevSelEnd.LineIndex > m_cpSelBegin.LineIndex
		&& m_cpSelEnd.LineIndex > m_cpPrevSelEnd.LineIndex)
	{
		CharPos2Point(m_cpPrevSelEnd, ptBegin);
		CharPos2Point(m_cpSelEnd, ptEnd);
		rc.left = 0;
		rc.top = ptBegin.y;
		rc.right = m_uClientWidth;
		rc.bottom = ptEnd.y + m_nLineHeight;
	}
	else if (m_cpPrevSelEnd.LineIndex > m_cpSelBegin.LineIndex
		&& m_cpSelEnd.LineIndex >= m_cpSelBegin.LineIndex
		&& m_cpSelEnd.LineIndex < m_cpPrevSelEnd.LineIndex)
	{
		CharPos2Point(m_cpSelEnd, ptBegin);
		CharPos2Point(m_cpPrevSelEnd, ptEnd);
		rc.left = 0;
		rc.top = ptBegin.y;
		rc.right = m_uClientWidth;
		rc.bottom = ptEnd.y + m_nLineHeight;
	}
	else if (m_cpPrevSelEnd.LineIndex > m_cpSelBegin.LineIndex
		&& m_cpSelEnd.LineIndex < m_cpSelBegin.LineIndex)
	{
		CharPos2Point(m_cpSelEnd, ptBegin);
		CharPos2Point(m_cpPrevSelEnd, ptEnd);
		rc.left = 0;
		rc.top = ptBegin.y;
		rc.right = m_uClientWidth;
		rc.bottom = ptEnd.y + m_nLineHeight;
	}
	else if (m_cpPrevSelEnd.LineIndex < m_cpSelBegin.LineIndex
		&& m_cpSelEnd.LineIndex < m_cpPrevSelEnd.LineIndex)
	{
		CharPos2Point(m_cpSelEnd, ptBegin);
		CharPos2Point(m_cpPrevSelEnd, ptEnd);
		rc.left = 0;
		rc.top = ptBegin.y;
		rc.right = m_uClientWidth;
		rc.bottom = ptEnd.y + m_nLineHeight;
	}
	else if (m_cpPrevSelEnd.LineIndex < m_cpSelBegin.LineIndex
		&& m_cpSelEnd.LineIndex > m_cpPrevSelEnd.LineIndex
		&& m_cpSelEnd.LineIndex <= m_cpSelBegin.LineIndex)
	{
		CharPos2Point(m_cpPrevSelEnd, ptBegin);
		CharPos2Point(m_cpSelEnd, ptEnd);
		rc.left = 0;
		rc.top = ptBegin.y;
		rc.right = m_uClientWidth;
		rc.bottom = ptEnd.y + m_nLineHeight;
	}
	else if (m_cpPrevSelEnd.LineIndex < m_cpSelBegin.LineIndex
		&& m_cpSelEnd.LineIndex > m_cpSelBegin.LineIndex)
	{
		CharPos2Point(m_cpPrevSelEnd, ptBegin);
		CharPos2Point(m_cpSelEnd, ptEnd);
		rc.left = 0;
		rc.top = ptBegin.y;
		rc.right = m_uClientWidth;
		rc.bottom = ptEnd.y + m_nLineHeight;
	}
	else if (m_cpPrevSelEnd.LineIndex != m_cpSelBegin.LineIndex
		&& m_cpSelEnd.LineIndex == m_cpPrevSelEnd.LineIndex)
	{
		if (m_cpPrevSelEnd.CharIndex < m_cpSelEnd.CharIndex)
		{
			CharPos2Point(m_cpPrevSelEnd, ptBegin);
			CharPos2Point(m_cpSelEnd, ptEnd);
			rc.left = ptBegin.x;
			rc.top= ptBegin.y;
			rc.right = ptEnd.x;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
		else if (m_cpPrevSelEnd.CharIndex > m_cpSelEnd.CharIndex)
		{
			CharPos2Point(m_cpSelEnd, ptBegin);
			CharPos2Point(m_cpPrevSelEnd, ptEnd);
			rc.left = ptBegin.x;
			rc.top= ptBegin.y;
			rc.right = ptEnd.x;
			rc.bottom = ptEnd.y + m_nLineHeight;
		}
	}
	rc.left = 0;
	rc.top = 0;
	rc.right = m_uClientWidth + m_rcInset.right;
	rc.bottom = m_uClientHeight + m_rcInset.bottom;
	return rc;
}

void CMultiLineEditUI::InvalidateRect(LPRECT lpRect, bool bFlag)
{
	RECT rcClient = this->GetInterRect();
	::OffsetRect(lpRect, (int)rcClient.left, (int)rcClient.top);
	this->GetWindow()->Invalidate(lpRect, bFlag);
}

void CMultiLineEditUI::GetSel(tstring &str)
{
	CharPos selBegin, selEnd;
	unsigned indexBegin, indexEnd;
	selBegin = min(m_cpSelBegin, m_cpSelEnd);
	selEnd = max(m_cpSelBegin, m_cpSelEnd);
	CharPos2Index(selBegin, indexBegin);
	CharPos2Index(selEnd, indexEnd);
	LPCTSTR lpszText = m_strText.c_str() + indexBegin;
	str.assign(lpszText, indexEnd-indexBegin);
}

void CMultiLineEditUI::Copy()
{
	if (m_cpSelBegin != m_cpSelEnd)
	{
		if (!OpenClipboard(m_hParentWnd))
			return;
		tstring selStr;
		GetSel(selStr);
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

void CMultiLineEditUI::Paste()
{
	OnPaste();
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
	if (hGlobalMem != NULL)
	{
		LPCTSTR lpszStr = (LPCTSTR)GlobalLock(hGlobalMem);
		if (lpszStr)
		{
			RECT rcDraw1 = {0}, rcPrevDeItem;
			if(m_cpSelEnd != m_cpSelBegin)
			{
				CharPos BeforeInsertPos = max(m_cpSelEnd, m_cpSelBegin);
				unsigned uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
				DelSelRange();
				rcPrevDeItem = m_rcPrevItem;
				unsigned uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
				rcDraw1 = this->GetDeleteReDrawRect(BeforeInsertPos, m_cpSelEnd, 
					!!(uAfterInsertLineCount-uBeforeInsertLineCount));
			}
			CharPos BeforeInsertPos = m_cpSelEnd;
			unsigned uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			Insert(lpszStr);
			unsigned uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
			RECT rcDraw2 = this->GetInsertReDrawRect(BeforeInsertPos, m_cpSelEnd, 
				!!(uAfterInsertLineCount-uBeforeInsertLineCount));
			RECT rcDraw;
			::UnionRect(&rcDraw, &rcDraw1, &rcDraw2);
			if(!m_bAutoSize)
			{
				if(EnsureSelEndVisible()) this->Invalidate();
				else this->InvalidateRect(&rcDraw);
			}
			else
			{
				if(rcDraw1.bottom - rcDraw1.top)
				{
					EnsureSelEndVisible();
					GetWindow()->Invalidate(&rcPrevDeItem, true);
				}
				else
				{
					if(EnsureSelEndVisible()) this->Invalidate(true);
					else this->InvalidateRect(&rcDraw, true);
				}
			}
		}
	}
	CloseClipboard();
	this->SetCaretPos();
}

void CMultiLineEditUI::Clear()
{
	RECT rcDraw;
	CharPos BeforeInsertPos;
	unsigned uBeforeInsertLineCount;

	if(m_uClientWidth)
	{
		BeforeInsertPos = max(m_cpSelEnd, m_cpSelBegin);
		uBeforeInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
	}
	DelSelRange();
	if(m_uClientWidth)
	{
		unsigned uAfterInsertLineCount;
		uAfterInsertLineCount = GetUpperBound(m_pEditDataImpl->m_lineInfoArray);
		rcDraw = this->GetDeleteReDrawRect(BeforeInsertPos, m_cpSelEnd, 
			!!(uAfterInsertLineCount-uBeforeInsertLineCount));
		this->InvalidateRect(&rcDraw);
	}
}

bool CMultiLineEditUI::IsKillFocus()
{
	return m_bKillFocus;
}

TextStyle* CMultiLineEditUI:: GetEditStyle()
{
	if (!m_pTextStyle)
		m_pTextStyle = CControlUI::GetTextStyle(_T("txt"));
	return m_pTextStyle;
}

TextStyle* CMultiLineEditUI::GetEmptyEditStyle()
{
	if (!m_pEmptyTextStyle)
		m_pEmptyTextStyle = CControlUI::GetTextStyle(_T("txt_empty"));
	return m_pEmptyTextStyle;
}

///////////////////////////////////////////////////////////////////////
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
	m_rcInset.left  = 4;
	m_rcInset.top = 4;
	m_rcInset.right = 4;
	m_rcInset.bottom = 4;
	SetEnabled(true);
	SetAttribute(_T("cursor"), _T("IBEAM"));
	ModifyFlags(UICONTROLFLAG_SETCURSOR | UICONTROLFLAG_TABSTOP | UICONTROLFLAG_SETFOCUS | UICONTROLFLAG_WANTRETURN, 0);

	/////////////////////////////
	m_bReadOnly = false;
	m_bPassword = false;
	m_bTransparent = false;
	m_bMenuEnable = true;
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
}

CEditUI::~CEditUI()
{
	HideToolTip();
	m_bInit = false;
}


void CEditUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("readonly")))
	{
		m_bReadOnly = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("password")))
	{
		m_bPassword = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("emptytips")))
	{
		m_strEmptyTipsText = I18NSTR(lpszValue);
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
		m_cxyMaxSize.cx = (int)_ttoi(lpszValue);
		m_cxyMaxSize.cx -= m_rcInset.left + m_rcInset.right;
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

void CEditUI::SetReadOnly(bool bReadOnly/*=true*/)
{
	m_bReadOnly = bReadOnly;
}

bool CEditUI::IsReadOnly()
{
	return m_bReadOnly;
}

int CEditUI::GetTextPixel()
{
	return m_nTextPixel;
}

void CEditUI::Init()
{
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
				if (m_bPassword)
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
						if (m_nTextPixel < m_cxyMaxSize.cx)
							m_rcItem.right = m_rcItem.left + m_rcInset.left + m_nTextPixel + m_rcInset.right;
						else 
							m_rcItem.right = m_rcItem.left + m_rcInset.left + m_cxyMaxSize.cx + m_rcInset.right;
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


TextStyle* CEditUI::GetEditStyle()
{
	if (!m_pTextStyle)
		m_pTextStyle = CControlUI::GetTextStyle(_T("txt"));
	return m_pTextStyle;
}

TextStyle* CEditUI::GetEmptyEditStyle()
{
	if (!m_pEmptyTextStyle)
		m_pEmptyTextStyle = CControlUI::GetTextStyle(_T("txt_empty"));
	return m_pEmptyTextStyle;
}

void  CEditUI::SetText(LPCTSTR lpszText)
{
	if (!lpszText)
	{
		return;
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
	tstring str = m_strText;
	decrypt(str);
	return str;
}

void CEditUI::SetTextWithoutNotify(LPCTSTR lpszText)
{
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

		LPCTSTR lpszText = m_bPassword ? m_strPasswdText.c_str() : m_strText.c_str();
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
		iLen = m_nPasswdCharWidth * nCount;
		m_nTextPixel += iLen;
		m_uSelBegin = m_uSelEnd;
		if(pEnd) *pEnd = crlf[0];
	}
	if(m_bAutoSize)    //如果是可变大小编辑框控件，则重设编辑框大小
	{
		Invalidate();
		m_rcPrevItem = m_rcItem;
		if (m_nTextPixel < m_cxyMaxSize.cx)
			m_rcItem.right = m_rcItem.left + m_rcInset.left + m_nTextPixel + m_rcInset.right;
		else 
			m_rcItem.right = m_rcItem.left + m_rcInset.left + m_cxyMaxSize.cx + m_rcInset.right;

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
			int iLen = m_nPasswdCharWidth * nCount;
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
			if (m_nTextPixel < m_cxyMaxSize.cx)
				m_rcItem.right = m_rcItem.left + m_rcInset.left + m_nTextPixel + m_rcInset.right;
			else 
				m_rcItem.right = m_rcItem.left + m_rcInset.left + m_cxyMaxSize.cx + m_rcInset.right;

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
		if(m_nTextPixel<m_cxyMaxSize.cx && m_rcPrevItem.right<m_rcItem.right)
		{
			m_nFirstVisiblePixel = 0;
			result = TRUE;
		}
		else if (m_nTextPixel<m_cxyMaxSize.cx && m_rcPrevItem.right>m_rcItem.right)
		{
			if (point.x < 0)
			{
				m_nFirstVisiblePixel = 0;
				result = TRUE;
			}
		}
		else if (m_nTextPixel > m_cxyMaxSize.cx)
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

	LPCTSTR lpszText = m_bPassword ? m_strPasswdText.c_str() : m_strText.c_str();
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

	return index;
}

int CEditUI::GetCharsCounts(HDC hDC, int nPixels, bool bAdjust/*=false*/)
{
	//assert(m_uPrevSelEnd >= 0);
	int nCounts = m_uPrevSelEnd;
	int iLen = m_nPrevSelEndPixel;

	LPCTSTR lpszText = m_bPassword ? m_strPasswdText.c_str() : m_strText.c_str();
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
	LPCTSTR lpszText = m_bPassword ? m_strPasswdText.c_str() : m_strText.c_str();
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
	LPCTSTR lpszText = m_bPassword ? m_strPasswdText.c_str() : m_strText.c_str();
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
	TextStyle *pTextStyle = GetEditStyle();
	if (pTextStyle == NULL)
	{
		return;
	}
	if (!m_bTransparent)
	{
		__super::Render(pRenderDC, rcPaint);
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
 			LPCTSTR lpszText = m_bPassword ? m_strPasswdText.c_str() : m_strText.c_str();
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

		COLORREF oldTextColor = ::SetTextColor(hDestDC, pTextStyle->GetTextColor());
		COLORREF oldBkColor = ::SetBkColor(hDestDC, COLOR_INPUT_BK);
		this->DrawRange(pRenderDC, iBegin, selBegin);
		::SetBkMode(hDestDC, OPAQUE);
		::SetBkColor(hDestDC, COLOR_SELECT_BK);
		::SetTextColor(hDestDC, COLOR_SELECT_TEXT);
		this->DrawRange(pRenderDC, selBegin, selEnd);
		::SetBkMode(hDestDC, TRANSPARENT);
		::SetBkColor(hDestDC, COLOR_INPUT_BK);
		::SetTextColor(hDestDC, pTextStyle->GetTextColor());
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
				return __super::Event(event);
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
				m_uSelEnd = m_strText.size();
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
	case UIEVENT_SCROLLWHEELFOCUS:
		{
			GetWindow()->KillFocus(this);
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
	return __super::Event(event);
}

bool CEditUI::OnMenuItemClick(TNotifyUI* pNotify)
{
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
	this->CreateCaret();
}

void CEditUI::OnKillFocus()
{
	this->DestroyCaret();
}

void CEditUI::OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
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
}

void CEditUI::OnLButtonDblick(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
	m_uSelBegin = 0;
	m_uSelEnd = m_strText.size();
	SetPrevSelEndInfo();
	EnsureSelEndVisible();
	this->Invalidate();
}

void CEditUI::OnRButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt/* = NULL*/)
{
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
	m_rcItem = rectRegion;
	if(m_bAutoSize)
	{
		if(!m_bMaxSize)
		{
			if(GetParent())
			{
				RECT rcParent = GetParent()->GetInterRect();
				m_cxyMaxSize.cx = rcParent.right - m_rcItem.left;
			}
			else
				m_cxyMaxSize.cx -= m_rcInset.left + m_rcInset.right;
		}
		m_rcPrevItem = m_rcItem;
		if (m_nTextPixel < m_cxyMaxSize.cx)
			m_rcItem.right = m_rcItem.left + m_rcInset.left + m_nTextPixel + m_rcInset.right;
		else 
			m_rcItem.right = m_rcItem.left + m_rcInset.left + m_cxyMaxSize.cx + m_rcInset.right;

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
	LPCTSTR lpszText = m_bPassword ? m_strPasswdText.c_str() : m_strText.c_str();	
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
		rc.right = m_rcPrevItem.right - m_rcInset.right;
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
	return false;
}

bool CEditUI::IsCaretEnable()
{
	return m_bCaretEnable;
}

void CEditUI::SetFirstVisiblePixel(int nPixel)
{
	m_nFirstVisiblePixel = nPixel;
}

void CEditUI::SetPreSelEndPixel(int nPixel)
{
	m_nPrevSelEndPixel = nPixel;
}

#endif 