#pragma once

class CControlDecorate;
class CUI_API CDecorateBaseButton :
	virtual public CAutoSizeButtonUI
{
public:
	CDecorateBaseButton(void);
	virtual			~CDecorateBaseButton(void);
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)override;
	void			AddDecorate(shared_ptr<CControlDecorate> spLayoutDecorate);
	virtual void	Init() override;
	virtual void	SetVisible(bool bShow) override;
	virtual void	SetInternVisible(bool bShow)override;
	virtual void	SetEnabled(bool bEnable) override;
	virtual bool	Activate()override;
protected:
	virtual void	SetRect(RECT& rectRegion);
private:
	vector<shared_ptr<CControlDecorate>> m_vecDecorates;
};
