#include "stdafx.h"
#include "DecorateBaseLayout.h"
#include "ControlDecorate.h"


CDecorateBaseLayout::CDecorateBaseLayout(void)
{

}

CDecorateBaseLayout::~CDecorateBaseLayout(void)
{
	m_vecDecorates.clear();
}

void CDecorateBaseLayout::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
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

void CDecorateBaseLayout::AddDecorate(shared_ptr<CControlDecorate> spLayoutDecorate)
{
	if (spLayoutDecorate)
	{
		spLayoutDecorate->SetLayoutBase(this);
		m_vecDecorates.push_back(spLayoutDecorate);
	}
	
}

void CDecorateBaseLayout::Init()
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

void CDecorateBaseLayout::SetVisible(bool bShow)
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

void CDecorateBaseLayout::SetInternVisible(bool bShow)
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

void CDecorateBaseLayout::SetRect(RECT& rectRegion)
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
