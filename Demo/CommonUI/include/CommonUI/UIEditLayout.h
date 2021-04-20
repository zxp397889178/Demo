#pragma once

#include <regex>
#include <functional>

#ifdef UNICODE
typedef wregex tregex;
typedef wsmatch tsmatch;
#else
typedef regex tregex;
typedef smatch tsmatch;
#endif

class CEditExUI;
class  CUI_API CEditLayoutUI :
	public CLayoutUI
{
	UI_OBJECT_DECLARE(CEditLayoutUI, _T("EditLayout"))
		typedef std::function<bool(const tstring &, tstring &)> GetRegexStr;
public:
	CEditLayoutUI();
	~CEditLayoutUI();


public:
	void		SetText(LPCTSTR lpszText);
	LPCTSTR		GetText();
	tstring		GetPasswordText();
	void		SetTextMode(bool bPassword);
	bool		IsFocused();
	bool		GetRegexResult();
	tstring		GetRealText();
	void		SetExRegexCheck(GetRegexStr funExRegexCheck);
	void		SetEmptyErr(const tstring & sErr);
	void		SetFocus();
	bool		CheckEmptyAndRegex(bool bSetFocus = true/*true У�鲻��ȷʱ���ý���*/);

protected:
	virtual void	Init();
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	

private:
	bool			OnEditNotifyFunc(TNotifyUI& notify);
	bool			OnCloseBtnNotify(TNotifyUI& notify);

private:
	CEditExUI*		m_pEdit;
	CControlUI*		m_pBtnClose;
	CControlUI*		m_pLbTop;
	CControlUI*		m_pLbError;
	CControlUI*		m_pImageUnderLine;
	CContainerUI*	m_pLayButton;
	tstring			m_sRegex;
	tstring			m_sErrorMessage;
	tstring			m_sEmptyErr;//��ʾһ��
	tstring			m_sEmpty;//�洢����ʾ��Ϣ
	bool			m_bRegexResult;//������ʽУ����
	GetRegexStr		m_funExRegexCheck;
public:
	CEventSource	OnEditNotify;

};