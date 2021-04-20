#pragma once

class CWndDecorate;
class CUI_API CDecorateBaseWindow :
	public CWindowUI
{
public:
	CDecorateBaseWindow(void);
	virtual			~CDecorateBaseWindow(void);
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)override;
	void			AddDecorate(shared_ptr<CWndDecorate> spWndDecorate);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void	OnCreate();
private:
	vector<shared_ptr<CWndDecorate>> m_vecDecorates;
};
