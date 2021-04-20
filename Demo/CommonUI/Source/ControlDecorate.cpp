#include "stdafx.h"
#include "ControlDecorate.h"

CControlDecorate::CControlDecorate()
{
}

CControlDecorate::~CControlDecorate()
{
}

void CControlDecorate::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
}

void CControlDecorate::Init()
{
}

void CControlDecorate::SetLayoutBase(CControlUI *pControl)
{
	m_pControl = pControl;
}

void CControlDecorate::SetVisible(bool bShow)
{
}

void CControlDecorate::SetInternVisible(bool bShow)
{
}

void CControlDecorate::SetRect(RECT& rectRegion)
{
}

void CControlDecorate::SetEnabled(bool bEnable)
{

}

bool CControlDecorate::Activate()
{
	return true;
}
