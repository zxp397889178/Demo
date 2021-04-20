#include "stdafx.h"

#include "UIEditEx.h"

#include <limits>
#include <assert.h>

bool IsChinese(TCHAR ch)
{
	if((unsigned short)ch >= 0x0391 && (unsigned short)ch <= 0xFFE5)
		return true;
	return false;
}

CEditExUI::CEditExUI()
{
	SetStyle(_T("Edit"));

	m_bLimit = false;
	m_uLimitType = enEditLimitNULL;
	m_strLimitString = _T("");

	m_bEnableLimitTipe = true;
	m_bNumericLimit = false;
	m_nLimitMin = INT_MIN;
	m_nLimitMax = INT_MAX;
}

CEditExUI::~CEditExUI()
{
}

void CEditExUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("limit")) == 0)
	{
		m_bLimit = true;
		m_uLimitType = enEditLimitNULL;
		m_strLimitString = _T("");

		tstring strValue = lpszValue;
		tstring strSpliter = strValue;
		int i = strValue.find_first_of(_T(' '));
		if (i > 0)
			strSpliter = strValue.substr(0, i);
		do
		{
			if (_tcsicmp(strSpliter.c_str(), _T("number")) == 0)
			{
				m_uLimitType |= enEditLimitNumber;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("letter")) == 0)
			{
				m_uLimitType |= enEditLimitLetter;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("chinese")) == 0)
			{
				m_uLimitType |= enEditLimitChinese;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("space")) == 0)
			{
				m_uLimitType |= enEditLimitString;
				m_strLimitString += _T(" ");
			}
			else
			{
				m_uLimitType |= enEditLimitString;
				m_strLimitString += strSpliter;
			}
			if (i < 0)
				break;
			strValue = strValue.substr(i + 1);
			if (strValue.empty())
				break;
			strSpliter = strValue;
			i = strValue.find_first_of(_T(' '));
			if (i > 0)
				strSpliter = strValue.substr(0, i);
		}while (1);
	}
	else if (_tcsicmp(lpszName, _T("exceptlimit")) == 0)
	{
		m_bLimit = true;
		m_uLimitType = enEditLimitNULL;
		m_strLimitString = _T("");

		tstring strValue = lpszValue;
		tstring strSpliter = strValue;
		int i = strValue.find_first_of(_T(' '));
		if (i > 0)
			strSpliter = strValue.substr(0, i);
		do
		{
			if (_tcsicmp(strSpliter.c_str(), _T("number")) == 0)
			{
				m_uLimitType |= enEditExceptNumber;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("letter")) == 0)
			{
				m_uLimitType |= enEditExceptLetter;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("chinese")) == 0)
			{
				m_uLimitType |= enEditExceptChinese;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("space")) == 0)
			{
				m_uLimitType |= enEditExceptString;
				m_strLimitString += _T(" ");
			}
			else
			{
				m_uLimitType |= enEditExceptString;
				m_strLimitString += strSpliter;
			}
			if (i < 0)
				break;
			strValue = strValue.substr(i + 1);
			if (strValue.empty())
				break;
			strSpliter = strValue;
			i = strValue.find_first_of(_T(' '));
			if (i > 0)
				strSpliter = strValue.substr(0, i);
		}while (1);
	}
	else if (_tcsicmp(lpszName, _T("unexceptlimit")) == 0)
	{
		tstring strValue = lpszValue;
		tstring strSpliter = strValue;
		int i = strValue.find_first_of(_T(' '));
		if (i > 0)
			strSpliter = strValue.substr(0, i);
		if (_tcsicmp(strSpliter.c_str(), _T("space")) == 0)
		{
			m_strLimitString.erase(m_strLimitString.find(L" "), 1);
		}
	}
	else if (_tcsicmp(lpszName, _T("enablelimittips")) == 0)
	{
		m_bEnableLimitTipe = (bool)!!_ttoi(lpszValue);
	}	
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

bool CEditExUI::Event(TEventUI& event)
{

	return __super::Event(event);
}

bool CEditExUI::OnLimitChar(UINT nChar)
{
	if (nChar == 0)
	{
		return true;
	}

	bool bCanInput = true;
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
					strLimit += m_strLimitString;
				bPrev = true;
			}
			bool bDefault = true;
			SendNotify(UINOTIFY_EN_LIMIT, NULL, (LPARAM)&bDefault);
			if (m_bEnableLimitTipe && bDefault)
			{
				ShowToolTip(strLimit.c_str(), UITOOLTIP_Warn);
			}
		}
		else
		{
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
					strLimit += m_strLimitString;
				bPrev = true;
			}
			bool bDefault = true;
			SendNotify(UINOTIFY_EN_LIMIT, NULL, (LPARAM)&bDefault);
			if (m_bEnableLimitTipe && bDefault)
			{
				ShowToolTip(strLimit.c_str(), UITOOLTIP_Warn);
			}
		}

	}

	if (true == bCanInput && true == m_bNumericLimit)
	{
		bCanInput = OnLimitString(nChar);
	}

	return !bCanInput;
}

bool CEditExUI::OnLimitString(UINT nChar)
{
	tstring tResult = GetText();
	TCHAR buf[5] = {0};
	wsprintf(buf, _T("%c"), nChar);
	int selBegin = 0;
	int selEnd = 0;

	GetSel(selBegin, selEnd);

	tResult.erase(selBegin, selEnd - selBegin);
	if (selEnd > (int)tResult.size())
		tResult.insert(tResult.size(), buf, 1);
	else
		tResult.insert(selEnd, buf, 1);

	if (m_uLimitType & enEditLimitNumber)
	{
		return IsLimitNumber(tResult);
	}

	return false;
}

void CEditExUI::OnSetFocus()
{
	if (m_uLimitType & enEditExceptChinese)
	{
		GetWindow()->DisableIME();
	}
	else
	{
		GetWindow()->EnableIME();
	}
	__super::OnSetFocus();
}

void CEditExUI::OnKillFocus()
{
	if (m_uLimitType & enEditExceptChinese)
	{
		GetWindow()->EnableIME();
	}
	__super::OnKillFocus();
}

void CEditExUI::SetNumericRange(int nMin, int nMax)
{
	assert(nMax >= nMin);

	m_nLimitMin = nMin;
	m_nLimitMax = nMax;
	m_bNumericLimit = true;
}

bool CEditExUI::IsLimitNumber(const tstring& strTest)
{
	tstring strLengthCheck = strTest;
	strLengthCheck.erase(0, strLengthCheck.find_first_not_of(_T('0')));
	if (strTest.length() > strLengthCheck.length() && L"0" != strTest)
	{
		return false;
	}

	int nValue = _wtoi(strTest.c_str());
	bool bResult = nValue >= m_nLimitMin && nValue <= m_nLimitMax;
	if (m_bEnableLimitTipe && false == bResult)
	{
		TCHAR strLimit[256] = { L'\0' };
		wsprintf(strLimit, I18NSTR(_T("#LIMIT_NUMERIC_RANGE")), m_nLimitMin, m_nLimitMax);
		ShowToolTip(strLimit, UITOOLTIP_Warn);
	}

	if (nValue > m_nLimitMax)
	{
		wchar_t strNumber[56] = { L'\0' };
		wsprintf(strNumber, L"%d", m_nLimitMax);
		SetText(strNumber);
	}

	return bResult;
}

bool CEditExUI::IsPassword()
{
	return m_bPassword;
}
////////////////////////////////////////////////////

CPasswordEditUI::CPasswordEditUI()
{

}

CPasswordEditUI::~CPasswordEditUI()
{

}

//////////////////////////////////////////////////////////////////////////

CComboBoxExUI::CComboBoxExUI()
{
	SetStyle(_T("ComboBox"));

	m_bLimit = false;
	m_uLimitType = enEditLimitNULL;
	m_bEnableLimitTipe = true;
	m_strLimitString = _T("");
}

CComboBoxExUI::~CComboBoxExUI()
{
}

void CComboBoxExUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("limit")) == 0)
	{
		m_bLimit = true;
		m_uLimitType = enEditLimitNULL;
		m_strLimitString = _T("");

		tstring strValue = lpszValue;
		tstring strSpliter = strValue;
		int i = strValue.find_first_of(_T(' '));
		if (i > 0)
			strSpliter = strValue.substr(0, i);
		do
		{
			if (_tcsicmp(strSpliter.c_str(), _T("number")) == 0)
			{
				m_uLimitType |= enEditLimitNumber;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("letter")) == 0)
			{
				m_uLimitType |= enEditLimitLetter;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("chinese")) == 0)
			{
				m_uLimitType |= enEditLimitChinese;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("space")) == 0)
			{
				m_uLimitType |= enEditLimitString;
				m_strLimitString += _T(" ");
			}
			else
			{
				m_uLimitType |= enEditLimitString;
				m_strLimitString += strSpliter;
			}
			if (i < 0)
				break;
			strValue = strValue.substr(i + 1);
			if (strValue.empty())
				break;
			strSpliter = strValue;
			i = strValue.find_first_of(_T(' '));
			if (i > 0)
				strSpliter = strValue.substr(0, i);
		}while (1);
	}
	else if (_tcsicmp(lpszName, _T("exceptlimit")) == 0)
	{
		m_bLimit = true;
		m_uLimitType = enEditLimitNULL;
		m_strLimitString = _T("");

		tstring strValue = lpszValue;
		tstring strSpliter = strValue;
		int i = strValue.find_first_of(_T(' '));
		if (i > 0)
			strSpliter = strValue.substr(0, i);
		do
		{
			if (_tcsicmp(strSpliter.c_str(), _T("number")) == 0)
			{
				m_uLimitType |= enEditExceptNumber;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("letter")) == 0)
			{
				m_uLimitType |= enEditExceptLetter;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("chinese")) == 0)
			{
				m_uLimitType |= enEditExceptChinese;
			}
			else if (_tcsicmp(strSpliter.c_str(), _T("space")) == 0)
			{
				m_uLimitType |= enEditExceptString;
				m_strLimitString += _T(" ");
			}
			else
			{
				m_uLimitType |= enEditExceptString;
				m_strLimitString += strSpliter;
			}
			if (i < 0)
				break;
			strValue = strValue.substr(i + 1);
			if (strValue.empty())
				break;
			strSpliter = strValue;
			i = strValue.find_first_of(_T(' '));
			if (i > 0)
				strSpliter = strValue.substr(0, i);
		}while (1);
	}
	else if (_tcsicmp(lpszName, _T("enablelimittips")) == 0)
	{
		m_bEnableLimitTipe = (bool)!!_ttoi(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

bool CComboBoxExUI::OnLimitChar(UINT nChar)
{
	if (nChar == 0)
	{
		return true;
	}

	bool bCanInput = true;
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
					strLimit += m_strLimitString;
				bPrev = true;
			}
			bool bDefault = true;
			SendNotify(UINOTIFY_EN_LIMIT, NULL, (LPARAM)&bDefault);
			if (m_bEnableLimitTipe && bDefault)
			{
				ShowToolTip(strLimit.c_str(), UITOOLTIP_Warn);
			}
		}
		else
		{
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
					strLimit += m_strLimitString;
				bPrev = true;
			}
			bool bDefault = true;
			SendNotify(UINOTIFY_EN_LIMIT, NULL, (LPARAM)&bDefault);
			if (m_bEnableLimitTipe && bDefault)
			{
				ShowToolTip(strLimit.c_str(), UITOOLTIP_Warn);
			}
		}

	}
	return !bCanInput;
}

void CComboBoxExUI::OnSetFocus()
{
	if (m_uLimitType & enEditExceptChinese)
	{
		GetWindow()->DisableIME();
	}
	else
	{
		GetWindow()->EnableIME();
	}
	__super::OnSetFocus();
}

void CComboBoxExUI::OnKillFocus()
{
	if (m_uLimitType & enEditExceptChinese)
	{
		GetWindow()->EnableIME();
	}
	__super::OnKillFocus();
}