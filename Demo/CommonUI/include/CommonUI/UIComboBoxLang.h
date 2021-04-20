#pragma once
#ifndef __DIRECTUI_COMBOBOXLANG_H_
#define __DIRECTUI_COMBOBOXLANG_H_

/*!
    \brief    ��Ͽ�ؼ�
*****************************************/
class CUI_API CComboBoxLangUI : public CComboBoxUI
{
	UI_OBJECT_DECLARE(CComboBoxLangUI, _T("ComboBoxLang"))
public:
	CComboBoxLangUI();
	virtual ~CComboBoxLangUI();

protected:
	/*!
	   \brief    ѡ��
	   \param    int index 
	   \param    bool bTextChange �����Ƿ�ı�
	   \return   void 
	 ************************************/
	virtual void SelectItem(int index, bool bTextChange);
};	

#endif //__DIRECTUI_COMBOBOX_H_