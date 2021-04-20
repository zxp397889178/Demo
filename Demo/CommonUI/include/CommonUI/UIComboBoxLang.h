#pragma once
#ifndef __DIRECTUI_COMBOBOXLANG_H_
#define __DIRECTUI_COMBOBOXLANG_H_

/*!
    \brief    组合框控件
*****************************************/
class CUI_API CComboBoxLangUI : public CComboBoxUI
{
	UI_OBJECT_DECLARE(CComboBoxLangUI, _T("ComboBoxLang"))
public:
	CComboBoxLangUI();
	virtual ~CComboBoxLangUI();

protected:
	/*!
	   \brief    选中
	   \param    int index 
	   \param    bool bTextChange 文字是否改变
	   \return   void 
	 ************************************/
	virtual void SelectItem(int index, bool bTextChange);
};	

#endif //__DIRECTUI_COMBOBOX_H_