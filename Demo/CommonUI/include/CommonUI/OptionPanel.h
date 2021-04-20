#pragma once

class CUI_API COptionPanelUI :
	public CLayoutUI,
	public INotifyUI
{
public:
	COptionPanelUI();
	virtual ~COptionPanelUI();

public:
	virtual void	
		Init();

	virtual bool
		OnApply();

	//should be restarted if return true
	virtual bool
		ShouldRestart();

};