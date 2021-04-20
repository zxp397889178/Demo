#pragma once

/*!
    \brief    �Զ�������ѡ��ؼ� 
*****************************************/
class CUI_API CAutoSizeCheckBoxUI : public CAutoSizeButtonUI
{
	UI_OBJECT_DECLARE(CAutoSizeCheckBoxUI, _T("AutoSizeCheckBox"))
public:
	//{{
	CAutoSizeCheckBoxUI();


	/*!
	   \brief    �Ƿ�ѡ��
	   \return   bool 
	 ************************************/
	bool IsChecked();

	/*!
	   \brief    ѡ��
	   \param    bool bChecked 
	   \param    UINT uFlags UIITEMFLAG_CHECKED�Ƿ�ѡ��/UIITEMFLAG_NOTIFY�Ƿ�֪ͨ
	   \return   void 
	 ************************************/
	virtual void SetChecked(bool bChecked, UINT uFlags = UIITEMFLAG_CHECKED|UIITEMFLAG_NOTIFY);

	/*!
	   \brief    ����ȫѡ��̬
	   \param    bool bIndef 
	   \return   void 
	 ************************************/

	bool Indef() const;
	/*!
	   \brief    ����ȫѡ��̬
	   \param    bool val 
	   \return   void 
	 ************************************/
	void Indef(bool val);

	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue); //���ػ���
protected:
	/// �̳к���
	virtual bool Event(TEventUI& event);

	//{{
private:
	bool m_bPartialChecked;
	//}}
};

