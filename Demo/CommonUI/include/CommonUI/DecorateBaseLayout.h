#pragma once

class CControlDecorate;
class CUI_API CDecorateBaseLayout :
	virtual public CLayoutUI
{
public:
	CDecorateBaseLayout(void);
	virtual			~CDecorateBaseLayout(void);
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)override;
	void			AddDecorate(shared_ptr<CControlDecorate> spLayoutDecorate);
	virtual void	Init() override;
	virtual void	SetVisible(bool bShow) override;
	virtual void	SetInternVisible(bool bShow)override;
protected:
	virtual void	SetRect(RECT& rectRegion);
private:
	vector<shared_ptr<CControlDecorate>> m_vecDecorates;
};
