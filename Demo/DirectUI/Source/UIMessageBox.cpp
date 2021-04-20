/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************/

#include "StdAfx.h"
#include "UIMessageBox.h"

CMessageBoxUI::CMessageBoxUI()
{
	SetWndStyle(GWL_EXSTYLE, WS_EX_CONTROLPARENT | WS_EX_TOOLWINDOW);
}
CMessageBoxUI::~CMessageBoxUI()
{
}
void CMessageBoxUI::OnControlNotify(TNotifyUI* pNotify)
{
	if (pNotify->nType == UINOTIFY_CLICK)
	{	
		tstring strItem = pNotify->pSender->GetId();
		if (strItem.length() >= 9 && strItem.substr(0, 9) == _T("msgboxbtn"))
			EndDialog(ID_MSGBOX_BTN + _ttoi(strItem.substr(_tcslen(_T("msgboxbtn"))).c_str()) - 1);	
	}
}

UINT CMessageBoxUI::MessageBox( HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, ENMessageBoxType enIconType,  UINT uDefaultButton/* = 0*/, LPCTSTR lpszStyleId /*= NULL*/, int nWidth /*= 340*/, int nHeight /*= 150*/ ,bool bDoModal,LPCTSTR lpszId /* =NULL */)
{
	Create(hParent, _T("MessageBox"), 0, 0, nWidth, nHeight);
	SetTitle(lpszCaption);
	CenterWindow(hParent);
	if (lpszId != NULL)
		SetSubId(lpszId);
	if (lpszStyleId && _tcslen(lpszStyleId) > 0)
	{
		GetRoot()->Remove(GetItem(_T("infoicon")));
		GetRoot()->Remove(GetItem(_T("info")));
		GetEngineObj()->AddControl(GetRoot(), NULL, lpszStyleId);
	}
	else
	{
		CControlUI* pInfoControl = GetItem(_T("info"));
		RT_CLASS_ASSERT(pInfoControl, CLabelUI, _T("MessageBox info"));
		CLabelUI* pInfo = static_cast<CLabelUI*>(pInfoControl);
		if (pInfo)
		{
			pInfo->SetAttribute(L"EnableMouse",L"1");
			pInfo->SetVisible(true);
			pInfo->SetText(lpszText);
			pInfo->SetToolTip(lpszText);
		}

		CControlUI* pIcon = GetItem(_T("infoicon"));
		if (pIcon)
		{
			pIcon->SetState((UITYPE_STATE)enIconType);
			pIcon->Invalidate();
		}
	}

	// 分解按钮名字
	if (lpszBtnType != NULL)
	{
		int index = 0;
		TCHAR szBtnId[20];
		tstring strBtnType = lpszBtnType;
		TCHAR c = _T(',');
		int i = strBtnType.find_first_of(c);
		if (i <= 0)
		{
			c = _T('|');
			i = strBtnType.find_first_of(c);
		/*	if (i <= 0)
			{
				c = _T(';');
				i = strBtnType.find_first_of(c);
				if (i <= 0)
				{
					c = _T(':');
					i = strBtnType.find_first_of(c);
				}
			}*/
		}
		
		while (i > 0)
		{
			_stprintf(szBtnId, _T("msgboxbtn%d"), index + 1);
			GetItem(szBtnId)->SetVisible(true);
			GetItem(szBtnId)->SetText(strBtnType.substr(0, i).c_str());
			strBtnType = strBtnType.substr(i + 1);
			i = strBtnType.find_first_of(c);
			index++;
		}
		if (strBtnType != _T(""))
		{
			_stprintf(szBtnId, _T("msgboxbtn%d"), index + 1);
			GetItem(szBtnId)->SetVisible(true);
			GetItem(szBtnId)->SetText(strBtnType.substr(0, i).c_str());
		}
	}

	if (uDefaultButton != 0)
	{
		TCHAR szBtnId[20];
		_stprintf(szBtnId, _T("msgboxbtn%d"), uDefaultButton);
		SetAttribute(_T("ok"), szBtnId);
	}
	if (bDoModal)
		return CWindowUI::DoModal();
	ShowAndActiveWindow();
	return 0;
}

DUI_API UINT  UIMessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType,CMessageBoxUI::ENMessageBoxType enIconType/* = CMessageBoxUI::enMessageBoxTypeInfo*/,  UINT nDefaultButtonIndex/* = 0*/, LPCTSTR lpszStyleId /*= NULL*/, int nWidth/* = 340*/, int nHeight/* = 150*/,bool bDoModal/* = true */,LPCTSTR lpszId /* =NULL */)
{
	CMessageBoxUI *pMsgBox = new CMessageBoxUI;
	UINT nResult = pMsgBox->MessageBox(hParent, lpszText, lpszCaption, lpszBtnType, enIconType, nDefaultButtonIndex, lpszStyleId, nWidth, nHeight,bDoModal,lpszId);
	if (bDoModal)
		delete pMsgBox;
	return nResult;

}