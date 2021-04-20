#pragma once

/*!
    \brief    自动增长复选框控件 
*****************************************/
class CUI_API CAutoSizeCheckBoxUI : public CAutoSizeButtonUI
{
	UI_OBJECT_DECLARE(CAutoSizeCheckBoxUI, _T("AutoSizeCheckBox"))
public:
	//{{
	CAutoSizeCheckBoxUI();


	/*!
	   \brief    是否选中
	   \return   bool 
	 ************************************/
	bool IsChecked();

	/*!
	   \brief    选中
	   \param    bool bChecked 
	   \param    UINT uFlags UIITEMFLAG_CHECKED是否选中/UIITEMFLAG_NOTIFY是否通知
	   \return   void 
	 ************************************/
	virtual void SetChecked(bool bChecked, UINT uFlags = UIITEMFLAG_CHECKED|UIITEMFLAG_NOTIFY);

	/*!
	   \brief    不完全选中态
	   \param    bool bIndef 
	   \return   void 
	 ************************************/

	bool Indef() const;
	/*!
	   \brief    不完全选中态
	   \param    bool val 
	   \return   void 
	 ************************************/
	void Indef(bool val);

	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue); //重载基类
protected:
	/// 继承函数
	virtual bool Event(TEventUI& event);

	//{{
private:
	bool m_bPartialChecked;
	//}}
};

