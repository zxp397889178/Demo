#include "stdafx.h"
#include "OptionPanel.h"

COptionPanelUI::COptionPanelUI()
{
	
}

COptionPanelUI::~COptionPanelUI()
{
	if (this->GetWindow())
	{
		this->GetWindow()->RemoveNotifier(this);
	}
}

void COptionPanelUI::Init()
{
	if (this->GetWindow())
	{
		this->GetWindow()->AddNotifier(this);
	}
}

bool COptionPanelUI::OnApply()
{
	return true;
}

bool COptionPanelUI::ShouldRestart()
{
	return false;
}
