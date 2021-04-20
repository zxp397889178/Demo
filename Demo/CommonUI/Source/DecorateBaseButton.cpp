#include "stdafx.h"
#include "UIAutoSizeButton.h"
#include "DecorateBaseButton.h"
#include "ControlDecorate.h"


CDecorateBaseButton::CDecorateBaseButton(void)
{

}

CDecorateBaseButton::~CDecorateBaseButton(void)
{
	m_vecDecorates.clear();
}

void CDecorateBaseButton::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->SetAttribute(lpszName, lpszValue);
		}	
	}
	return __super::SetAttribute(lpszName, lpszValue);
}

void CDecorateBaseButton::AddDecorate(shared_ptr<CControlDecorate> spLayoutDecorate)
{
	if (spLayoutDecorate)
	{
		spLayoutDecorate->SetLayoutBase(this);
		m_vecDecorates.push_back(spLayoutDecorate);
	}
	
}

void CDecorateBaseButton::Init()
{
	__super::Init();
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->Init();
		}	
	}
}

void CDecorateBaseButton::SetVisible(bool bShow) 
{
	__super::SetVisible(bShow);
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->SetVisible(bShow);
		}
	}
}

void CDecorateBaseButton::SetInternVisible(bool bShow)
{
	__super::SetInternVisible(bShow);
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->SetInternVisible(bShow);
		}
	}
}

void CDecorateBaseButton::SetEnabled(bool bEnable)
{
	__super::SetEnabled(bEnable);
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->SetEnabled(bEnable);
		}
	}
}

bool CDecorateBaseButton::Activate()
{
	bool bRes = __super::Activate();
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->Activate();
		}
	}
	return bRes;
}

void CDecorateBaseButton::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);
	for (auto it : m_vecDecorates)
	{
		if (it)
		{
			it->SetRect(rectRegion);
		}
	}
}
