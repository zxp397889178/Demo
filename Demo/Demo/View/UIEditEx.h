#ifndef _UIEDITEX_H_
#define _UIEDITEX_H_

enum
{
	enEditLimitNULL    = 0x0000,
	enEditLimitNumber  = 0x0001,
	enEditLimitLetter  = 0x0002,
	enEditLimitChinese = 0x0008,
	enEditLimitString  = 0x0010,
	enEditExceptNumber = 0x0020,
	enEditExceptLetter = 0x0040,
	enEditExceptChinese= 0x0080,
	enEditExceptString = 0x0100,
};

class CEditExUI : public CEditUI
{
	UI_OBJECT_DECLARE(CEditExUI, _T("EditEx"))
public:
	CEditExUI();
	virtual ~CEditExUI();

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual bool OnLimitChar(UINT nChar);
	virtual bool OnLimitString(UINT nChar);
	virtual void OnSetFocus();
	virtual void OnKillFocus();
	bool IsPassword();

public:
	void SetNumericRange(int nMin, int nMax);

protected:
	bool IsLimitNumber(const tstring& strTest);
	virtual bool Event(TEventUI& event);

private:
	bool m_bLimit;
	UINT m_uLimitType;
	tstring m_strLimitString;

	bool m_bNumericLimit;
	int m_nLimitMin;
	int m_nLimitMax;
	bool m_bEnableLimitTipe;
};


class CPasswordEditUI : public CEditExUI
{
	UI_OBJECT_DECLARE(CPasswordEditUI, _T("PasswordEdit"))
public:
	CPasswordEditUI();
	virtual ~CPasswordEditUI();

private:

};

class CComboBoxExUI : public CComboBoxUI
{
	UI_OBJECT_DECLARE(CComboBoxExUI, _T("ComboBoxEx"))
public:
	CComboBoxExUI();
	virtual ~CComboBoxExUI();

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual bool OnLimitChar(UINT nChar);
	virtual void OnSetFocus();
	virtual void OnKillFocus();

private:
	bool m_bLimit;
	UINT m_uLimitType;
	tstring m_strLimitString;
	bool m_bEnableLimitTipe;
};

#endif //_UIEDITEX_H_