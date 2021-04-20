#include "stdafx.h"

#include "UIEditLayout.h"

#include "UIEditEx.h"

const static TCHAR IdEdit[]		= _T("ed");
const static TCHAR IdIcon[]		= _T("icon");
const static TCHAR IdCloseBtn[] = _T("btn_close");
const static TCHAR IdBtnLayout[] = _T("btn_layout");
const static TCHAR IdTopLabel[] = _T("b_txt");
const static TCHAR IdErrorLabel[] = _T("error_txt");
const static TCHAR IdUnderLine[] = _T("under_line");

CEditLayoutUI::CEditLayoutUI()
{
	m_pEdit		= NULL;
	m_pBtnClose = NULL;
	m_pLbTop	= NULL;
	m_pLayButton = NULL;
	m_pImageUnderLine = NULL;
	m_pLbError = NULL;
	m_sRegex	= _T("");
	m_sEmptyErr = _T("");
	m_sEmpty	= _T("");
	m_bRegexResult = true;
}

CEditLayoutUI::~CEditLayoutUI()
{

}

void CEditLayoutUI::Init()
{
	__super::Init();
	
	m_pEdit		= dynamic_cast<CEditExUI*>(this->FindSubControl(IdEdit));
	m_pLayButton = dynamic_cast<CContainerUI*>(this->FindSubControl(IdBtnLayout));
	if (m_pLayButton)
	{
		m_pBtnClose = dynamic_cast<CControlUI*>(m_pLayButton->FindSubControl(IdCloseBtn));
	}
	m_pLbTop	= dynamic_cast<CControlUI*>(this->FindSubControl(IdTopLabel));
	m_pLbError  = dynamic_cast<CControlUI*>(this->FindSubControl(IdErrorLabel));
	m_pImageUnderLine = dynamic_cast<CControlUI*>(this->FindSubControl(IdUnderLine));

	if (m_pEdit)
	{
		m_pEdit->OnNotify += MakeDelegate(this, &CEditLayoutUI::OnEditNotifyFunc);
		m_pEdit->SetAttribute(_T("enablelimittips"), _T("0"));
	}

	if (m_pBtnClose)
	{
		m_pBtnClose->OnNotify += MakeDelegate(this, &CEditLayoutUI::OnCloseBtnNotify);
	}

}


bool CEditLayoutUI::OnEditNotifyFunc(TNotifyUI& notify)
{
	if (notify.nType == UINOTIFY_SETFOCUS)
	{
		if (m_pLbError && m_pLbTop)
		{
			if (m_sEmptyErr.empty() && ((_tcsicmp(m_pEdit->GetText(), _T("")) == 0 || m_bRegexResult)))
			{

				m_pLbTop->SetVisible(true);
				m_pLbError->SetVisible(false);
			}
			else
			{
				if (!m_sEmptyErr.empty())
				{
					m_pLbError->SetText(I18NSTR(m_sEmptyErr.c_str()));
					m_pLbError->SetToolTip(I18NSTR(m_sEmptyErr.c_str()));
				}
				
				if (m_bRegexResult || !m_sEmptyErr.empty())
				{
					m_pLbTop->SetVisible(false);
					m_pLbError->SetVisible(true);
				}
			}
		}

		if (m_pImageUnderLine)
		{
			m_pImageUnderLine->SetAttribute(_T("bk.imgseq"), _T("1,1,1"));
		}

		if (m_pEdit)
		{
			if (m_pLayButton)
			{
				m_pLayButton->SetVisible(_tcsicmp(m_pEdit->GetText(), _T("")) != 0);
			}

			//m_pEdit->SetAttribute(_T("bk.imgseq"), _T("1,1,1"));
		}

		this->Resize();
		this->Invalidate();
	}
	else if (notify.nType == UINOTIFY_EN_CHANGE)
	{
		if (m_pEdit)
		{
			if (m_pLayButton)
			{
				m_pLayButton->SetVisible(_tcsicmp(m_pEdit->GetText(), _T("")) != 0);
			}

			this->Resize();
			this->Invalidate();
		}
	}
	else if (notify.nType == UINOTIFY_KILLFOCUS)
	{
		m_sEmptyErr = _T("");
		if (m_pEdit)
		{
			if (m_pLbTop && m_pLbError
				&& _tcsicmp(m_pEdit->GetText(), _T("")) == 0)
			{
				m_pLbTop->SetVisible(false);
				m_pLbError->SetVisible(false);
			}
			else
			{
				tstring sText = m_pEdit->IsPassword() ? m_pEdit->GetPasswordText() : m_pEdit->GetText();
				if (m_pLbError)
				{
					if (!m_sRegex.empty())
					{
						tregex szTextRule(m_sRegex);
						tsmatch sResult;
						if (regex_search(sText, sResult, szTextRule))
						{
							m_bRegexResult = true;
							m_pLbTop->SetVisible(true);
							m_pLbError->SetVisible(false);
						}
						else
						{
							m_bRegexResult = false;
							m_pLbTop->SetVisible(false);
							m_pLbError->SetVisible(true);
							m_pLbError->SetText(I18NSTR(m_sErrorMessage.c_str()));
							m_pLbError->SetToolTip(I18NSTR(m_sErrorMessage.c_str()));
							return true;
						}
					}		
				}
				tstring sCheck = _T("");
				;
				if (m_funExRegexCheck != NULL && !(m_bRegexResult = m_funExRegexCheck(sText, sCheck)))
				{
					m_pLbTop->SetVisible(false);
					m_pLbError->SetVisible(true);
					m_pLbError->SetText(I18NSTR(sCheck.c_str()));
				}
				else
				{
					m_pLbTop->SetVisible(true);
					m_pLbError->SetVisible(false);
				}
			}

			//m_pEdit->SetAttribute(_T("bk.imgseq"), _T("0,0,0"));
		}

		if (m_pImageUnderLine)
		{
			m_pImageUnderLine->SetAttribute(_T("bk.imgseq"), _T("0,0,0"));
		}

		if (m_pLayButton)
		{
			m_pLayButton->SetVisible(false);
		}

		this->Resize();
		this->Invalidate();
	}

	if (OnEditNotify.GetCount() > 0)
	{
		OnEditNotify(&notify);
	}

	return true;
}

bool CEditLayoutUI::OnCloseBtnNotify(TNotifyUI& notify)
{
	if (notify.nType == UINOTIFY_CLICK)
	{
		if (m_pEdit)
		{
			m_pEdit->SetAttribute(_T("text"), _T(""));
			m_pEdit->SetFocus();
		}

		if (m_pLayButton)
		{ 
			m_pLayButton->SetVisible(false);
		}

	}
	return true;
}

LPCTSTR CEditLayoutUI::GetText()
{
	if (m_pEdit)
	{
		return m_pEdit->GetText();
	}

	return _T("");
}

tstring CEditLayoutUI::GetPasswordText()
{
	if (m_pEdit)
	{
		return m_pEdit->GetPasswordText();
	}

	return _T("");
}

void CEditLayoutUI::SetTextMode(bool bPassword)
{
	if (m_pEdit)
	{
		m_pEdit->SetAttribute(_T("password_visiblity"), bPassword ? _T("1") : _T("0"));
		m_pEdit->Invalidate();
	}
}

void CEditLayoutUI::SetText(LPCTSTR lpszText)
{
	if (m_pEdit)
	{
		m_pEdit->SetText(lpszText);
	}

	if (m_pLbTop)
	{
		m_pLbTop->SetVisible(_tcsicmp(lpszText, _T("")) != 0);
	}
}

bool CEditLayoutUI::IsFocused()
{
	return m_pEdit->IsFocused();
}

void CEditLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (!_tcsicmp(lpszName, _T("errorregex")))
	{
		m_sRegex = lpszValue;
	}
	else if (!_tcsicmp(lpszName, _T("errortxt")))
	{
		m_sErrorMessage = lpszValue;
	}
	else if (!_tcsicmp(lpszName, _T("emptytext")))
	{
		m_sEmpty = lpszValue;
	}
	
	else
		__super::SetAttribute(lpszName, lpszValue);
}

bool CEditLayoutUI::GetRegexResult()
{
	return m_bRegexResult;
}

tstring CEditLayoutUI::GetRealText()
{
	return m_pEdit->IsPassword() ? m_pEdit->GetPasswordText() : m_pEdit->GetText();
}

void CEditLayoutUI::SetExRegexCheck(GetRegexStr funExRegexCheck)
{
	m_funExRegexCheck = funExRegexCheck;
}

void CEditLayoutUI::SetEmptyErr(const tstring & sErr)
{
	if (m_pLbError)
	{
		m_sEmptyErr = sErr;
	}	
}

void CEditLayoutUI::SetFocus()
{
	if (m_pEdit)
	{
		m_pEdit->SetFocus();
	}
	else
		__super::SetFocus();
}

bool CEditLayoutUI::CheckEmptyAndRegex(bool bSetFocus /*= true/*true 校验不正确时设置焦点*/)
{

	if (GetRealText().empty())
	{
		if (!m_sEmpty.empty())
			SetEmptyErr(m_sEmpty);
		if (bSetFocus)
			SetFocus();
		return false;
	}
	
	if (!GetRegexResult() )
	{
		if (bSetFocus)
			SetFocus();
		return false;
	}
	
	if (m_funExRegexCheck )
	{
		tstring sCheck;
		if (!m_funExRegexCheck(GetRealText(), sCheck))
		{
			SetFocus();
			m_pLbTop->SetVisible(false);
			m_pLbError->SetVisible(true);
			m_pLbError->SetText(I18NSTR(sCheck.c_str()));
			m_pLbError->SetToolTip(I18NSTR(sCheck.c_str()));
			Invalidate();
			
			return false;
		}
	}
	
	return true;
}