#include "stdafx.h"
#include "CursorObj.h"

CursorObj::CursorObj()
{
	m_hCursor = NULL;
}

CursorObj::~CursorObj()
{
	if (m_hCursor) ::DestroyCursor(m_hCursor);
}

HCURSOR CursorObj::GetHCursor() const
{
	return m_hCursor;

}
void CursorObj::SetHCurosr(HCURSOR val)
{
	if (m_hCursor) ::DestroyCursor(m_hCursor);
	m_hCursor = val;
}
