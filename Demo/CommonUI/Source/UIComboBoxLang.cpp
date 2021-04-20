
#include "stdafx.h"
#include "UIComboBoxLang.h"

CComboBoxLangUI::CComboBoxLangUI()
{
}

CComboBoxLangUI::~CComboBoxLangUI()
{
}

void CComboBoxLangUI::SelectItem(int index, bool bTextChange)
{
	if (GetList() == NULL)
		return;
	CControlUI* pControl = m_pList->GetItem(index);
	if (pControl)
	{
		if (bTextChange)
		{
			SetTextWithoutNotify(pControl->GetText());

			TextStyle* pStyle = (TextStyle*)this->GetStyle(_T("txt"));
			TextStyle* pControlStyle = (TextStyle*)pControl->GetStyle(_T("txt"));
			if (!pStyle || !pControlStyle)
			{
				return;
			}

			FontObj* pFont = pStyle->GetFontObj();
			FontObj* pControlFont = pControlStyle->GetFontObj();
			if (!pFont || !pControlFont)
			{
				return;
			}

			FontObj* pNewFont = new FontObj(*pFont);
			if (!pNewFont)
			{
				return;
			}

			pNewFont->SetCached(false);
			pNewFont->SetAttribute(_T("face"), pControlFont->GetFaceName());

			//pNewFont will be deleted on style release
			pStyle->SetFontObj(pNewFont);

			SetToolTip(pControl->GetText());
		}
		m_pList->SelectItem(pControl);
		//CWindowUI * pManager = this->GetWindow();
	}
	else
	{
		if (bTextChange)
		{
			SetTextWithoutNotify(_T(""));
			SetToolTip(_T(""));
		}
		m_pList->SelectItem(NULL);
	}
	SendNotify(UINOTIFY_SELCHANGE);
	this->Invalidate();
}
