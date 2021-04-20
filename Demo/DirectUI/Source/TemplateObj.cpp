#include "stdafx.h"
#include "TemplateObj.h"

TemplateObj::TemplateObj(void)
{
	m_bStore = true;
	m_lpCachedStyle = NULL;
}

TemplateObj::~TemplateObj(void)
{
	for (int i = 0; i < NumChild(); i++)
	{
		TemplateObj* pTemplateObj = static_cast<TemplateObj*>(m_arrChild[i]);
		delete pTemplateObj;
	}
	m_arrChild.Empty();

	if (m_lpCachedStyle)
	{
		int nCnt = m_lpCachedStyle->GetSize();
		for (int i = 0; i < nCnt; i++)
		{
			StyleObj* pStyleObj = static_cast<StyleObj*>(m_lpCachedStyle->GetAt(i));
			if (pStyleObj)
				delete pStyleObj;
		}
			
		delete m_lpCachedStyle;
		m_lpCachedStyle = NULL;
	}
}

void TemplateObj::Add(TemplateObj* pTemplate)
{
	m_arrChild.Add(pTemplate);
}

void TemplateObj::Insert(int nIndex, TemplateObj* pTemplate)
{
	m_arrChild.InsertAt(nIndex, pTemplate);
}
void TemplateObj::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (!lpszName || !lpszValue)
		return;
	m_mapAttr[lpszName] = lpszValue;
}
int TemplateObj::GetAttrCount()
{
	return m_mapAttr.size();
}
LPCTSTR TemplateObj::GetAttr(LPCTSTR lpszName)
{
	std::map<tstring, tstring>::iterator pos = m_mapAttr.find(lpszName);
	if (pos != m_mapAttr.end())
		return (pos->second).c_str();
	return NULL;
}
MAPAttr* TemplateObj::GetAttr()
{
	return &m_mapAttr;
}

LPCTSTR TemplateObj::GetId() const
{
	return m_strId.c_str();
}

void TemplateObj::SetId( LPCTSTR val )
{
	m_strId = val;
}

void TemplateObj::SetType(LPCTSTR lpszType)
{
	m_strType = lpszType;
}

LPCTSTR TemplateObj::GetType()
{
	return m_strType.c_str();
}

TemplateObj* TemplateObj::GetChild(int nIndex)
{
	if( nIndex < 0 || nIndex >= m_arrChild.GetSize() ) return NULL;
	return static_cast<TemplateObj*>(m_arrChild[nIndex]);
}

int TemplateObj::NumChild()
{
	return m_arrChild.GetSize();
}

void TemplateObj::SetStore(bool bStore)
{
	m_bStore = bStore;
}

bool TemplateObj::IsStore()
{
	return m_bStore;
}


void TemplateObj::CacheStyle(CStdPtrArray* lpCachedStyle)
{
	m_lpCachedStyle = lpCachedStyle;
}

CStdPtrArray* TemplateObj::GetCachedStyle()
{
	return m_lpCachedStyle;
}