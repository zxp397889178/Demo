#include "stdafx.h"
#include "UIComposite.h"

/////////////////////////////////////////////////////////////////////////////////////////
CCompositeUI::CCompositeUI()
{
	EnableScrollBar(false, false);
}


void CCompositeUI::OnDestroy()
{

}

void CCompositeUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)//ÖØÔØ»ùÀà
{
	if (equal_icmp(lpszName, _T("childstyle")))
	{
		GetEngineObj()->AddControl(this, NULL,  lpszValue);
		return;
	}
	else if (equal_icmp(lpszName, _T("template")))
	{
		m_strTemplateId = lpszValue;
	}
	else
	{
		tstring strStyle = lpszName;
		int i = strStyle.find(_T('.'));
		if (i > 0)
		{
			tstring strControlName = strStyle.substr(0, i);
			tstring strAttr = strStyle.substr(i + 1).c_str();
			if ( strControlName == _T("ext") )
			{
				if (!strAttr.empty() && lpszValue)
				{
					m_mapAttrExt[strAttr] = lpszValue;
				}
			}
			else
			{
				CControlUI* pControl = GetItem(strControlName.c_str());
				if (pControl)
				{
					pControl->SetAttribute(strAttr.c_str(), lpszValue);
					return;
				}
			}
		}
	}
	__super::SetAttribute(lpszName, lpszValue);
}

LPCTSTR CCompositeUI::GetExtAttribute(LPCTSTR lpszName)
{
	if (NULL == lpszName) return _T("");
	
	std::map<tstring, tstring>::iterator i = m_mapAttrExt.find(lpszName);
	if (i == m_mapAttrExt.end()) return _T("");

	return i->second.c_str();
}

void CCompositeUI::SetTemplateId(LPCTSTR lpszTemplateId)
{
	m_strTemplateId = lpszTemplateId;
}

bool CCompositeUI::Create(LPCTSTR lpszTemplateName, CContainerUI* pParentCtn)
{
	if (_tcslen(GetId()) == 0)
		SetId(lpszTemplateName);

	if ( pParentCtn )
	{
		pParentCtn->Add(this);
	}

	bool b = GetEngineObj()->AddControl(this, NULL,  lpszTemplateName);
	if ( !b )
	{
		return false;
	}

	SetINotifyUI(this);

	const MAPAttr* mapAttr = NULL;
	mapAttr = GetResObj()->GetTemplateObjAttr(lpszTemplateName);
	if (mapAttr)
	{
		for (MAPAttr::const_iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
			SetAttribute(pos->first.c_str(), pos->second.c_str());
	}
	OnCreate();

	return true;
}

void  CCompositeUI::Init()
{
	if (!m_strTemplateId.empty())
	{
		this->Create(m_strTemplateId.c_str(), NULL);
	}
}

bool CCompositeUI::CreateEx(LPCTSTR lpszTemplateName, CContainerUI* pParentCtn, LPCTSTR lpszCtrlId, 
	Coordinate* pCdt, const std::map<tstring, tstring>* pmapAttrExt)
{
	/*SetId(lpszCtrlId);*/
	if ( pParentCtn )
	{
		this->SetManager(pParentCtn->GetWindow(), pParentCtn);
	}
	
	const MAPAttr* mapAttr = GetResObj()->GetTemplateObjAttr(lpszTemplateName);
	if (mapAttr)
	{
		for (MAPAttr::const_iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
			SetAttribute(pos->first.c_str(), pos->second.c_str());
	}

	if (pmapAttrExt)
	{
		for (std::map<tstring, tstring>::const_iterator pos = pmapAttrExt->begin(); pos != pmapAttrExt->end(); ++pos)
		{
			m_mapAttrExt[pos->first.c_str()] = pos->second.c_str();
		}
	}
	if (pCdt)
	{
		SetCoordinate(*pCdt);
	}

	SetId(lpszCtrlId);

	if ( !GetEngineObj()->AddControl(this, NULL,  lpszTemplateName) ) 
		return false;

	if ( pParentCtn )
	{
		pParentCtn->Add(this);
	}

	SetINotifyUI(this);
	OnCreate();
/*	SetId(lpszCtrlId);*/

	return true;
}

void CCompositeUI::AddNotifier(INotifyUI* pNotifyUI)
{
	if (pNotifyUI)
	{
		RefCountedThreadSafe<INotifyUI>* pNotifySafe = pNotifyUI->GetINotifySafe();
		if (pNotifySafe)
		{
			pNotifySafe->AddRef();
			m_aNotifiers.Add(pNotifySafe);
		}
	}
}

void CCompositeUI::RemoveNotifier(INotifyUI* pNotifyUI)
{
	int nCnt = m_aNotifiers.GetSize();
	for( int i = 0; i < nCnt; i++ ) 
	{
		RefCountedThreadSafe<INotifyUI>* pNotifySafe = static_cast<RefCountedThreadSafe<INotifyUI>*>(m_aNotifiers.GetAt(i));
		if( pNotifySafe && pNotifySafe->IsValid() && pNotifySafe->GetCountedOwner() == pNotifyUI ) {
			m_aNotifiers.Remove(i);
			pNotifySafe->Release();
			break;
		}
	}
}

bool CCompositeUI::ProcessNotify(TNotifyUI* pNotify)
{
	int nCnt = m_aNotifiers.GetSize();
	for (int i = 0; i < nCnt; i++)
	{
		RefCountedThreadSafe<INotifyUI>* pNotifySafe = static_cast<RefCountedThreadSafe<INotifyUI>*>(m_aNotifiers.GetAt(i));
		if (pNotifySafe && pNotifySafe->IsValid()) {
			INotifyUI* pNotifyFunc = pNotifySafe->GetCountedOwner();
			if (pNotifyFunc && pNotifyFunc->ProcessNotify(pNotify))
				return true;
		}
	}
	return false;
}