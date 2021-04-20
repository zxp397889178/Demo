/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************
*   Date             Name                 Description
*   2010-11-5		 hanzp
*********************************************************************/
#pragma once


/*!
    \brief    消息框窗口
*****************************************/
class DUI_API CMessageBoxUI : public CWindowUI
{
	//{{
public:
	CMessageBoxUI();
	virtual ~CMessageBoxUI();
	//}}
	typedef enum _tagMessageBoxType
	{
		enMessageBoxTypeSucceed = 0,
		enMessageBoxTypeInfo,
		enMessageBoxTypeWarn,
		enMessageBoxTypeError,
		enMessageBoxTypeQuestion
	}ENMessageBoxType;

	UINT MessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, ENMessageBoxType enIconType, UINT uDefaultButton = 0, LPCTSTR lpszStyleId = NULL, int nWidth = DPI_SCALE(340), int nHeight = DPI_SCALE(150), bool bDoModal = true, LPCTSTR lpszId = NULL);

protected:
	virtual void OnControlNotify(TNotifyUI* pNotify);
};

DUI_API UINT  UIMessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, CMessageBoxUI::ENMessageBoxType enIconType = CMessageBoxUI::enMessageBoxTypeInfo, UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = DPI_SCALE(340), int nHeight = DPI_SCALE(150), bool bDoModal = true, LPCTSTR lpszId = NULL);