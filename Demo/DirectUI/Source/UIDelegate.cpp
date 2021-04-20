#include "StdAfx.h"
#include "UIDelegate.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
CDelegateBase::CDelegateBase(void* pObject, void* pFn) 
{
	m_pObject = pObject;
	m_pFn = pFn; 
	if (m_pObject)
		m_pFn = *((void**)pFn);
}

CDelegateBase::CDelegateBase(const CDelegateBase& rhs) 
{
	m_pObject = rhs.m_pObject;
	m_pFn = rhs.m_pFn; 
}

CDelegateBase::~CDelegateBase()
{

}

bool CDelegateBase::Equals(const CDelegateBase& rhs) const 
{
	return m_pObject == rhs.m_pObject && m_pFn == rhs.m_pFn; 
}

bool CDelegateBase::operator() (void* param) 
{
	return this->Invoke(param); 
}

void* CDelegateBase::GetFn() 
{
	return m_pFn; 
}

void* CDelegateBase::GetObject() 
{
	return m_pObject; 
}

////////////////////////////////////////////
CEventSource::CEventSource()
{
}
CEventSource::~CEventSource()
{
	for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
		CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
		if( pObject) delete pObject;
	}
	m_aDelegates.Empty();
}

void CEventSource::operator+= (CDelegateBase& d)
{ 
	for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
		CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
		if( pObject && pObject->Equals(d) ) return;
	}

	m_aDelegates.Add(d.Copy());
}


void CEventSource::operator-= (CDelegateBase& d) 
{
	for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
		CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
		if( pObject && pObject->Equals(d) ) {
			delete pObject;
			m_aDelegates.Remove(i);
			break;
		}
	}
	if (m_aDelegates.GetSize() == 0)
		m_aDelegates.Empty();
}


void CEventSource::operator+= (FnType pFn)
{ 
	(*this) += MakeDelegate(pFn);
}


void CEventSource::operator-= (FnType pFn)
{ 
	(*this) -= MakeDelegate(pFn);
	if (m_aDelegates.GetSize() == 0)
		m_aDelegates.Empty();
}

void CEventSource::Insert(int index, CDelegateBase& d)
{
	for (int i = 0; i < m_aDelegates.GetSize(); i++) {
		CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
		if (pObject && pObject->Equals(d)) return;
	}

	m_aDelegates.InsertAt(index, d.Copy());
}


bool CEventSource::operator()(void* param)
{
	int nCnt = m_aDelegates.GetSize();
	for( int i = 0; i < nCnt; i++ ) {
		CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
		if( pObject)
		{
			if ( !(*pObject)(param) )
				return false;
		}
	}
	return true;
}

int CEventSource::GetCount()
{
	return m_aDelegates.GetSize();
}

CDelegateBase* CEventSource::GetItem(int nIndex)
{
	int nCnt = GetCount();
	if (nIndex < 0 || nIndex >= nCnt) return NULL;
	return static_cast<CDelegateBase*>(m_aDelegates.GetAt(nIndex));
}
