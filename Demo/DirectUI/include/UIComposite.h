#pragma once
#ifndef __DIRECTUI_COMPOSITE_H_
#define __DIRECTUI_COMPOSITE_H_

/*!
    \brief    ���������ؼ�
*****************************************/
class DUI_API CCompositeUI: public CLayoutUI, public INotifyUI
{
	UI_OBJECT_DECLARE(CCompositeUI, _T("Composite"))
public:
	//{{
	CCompositeUI();
	//}}

	/*!
	   \brief    �����Ϣ����
	   \note     ���FilterProcessNotify
	   \param    INotifyUI * pNotifyUI 
	   \return   void 
	 ************************************/
	void AddNotifier(INotifyUI* pNotifyUI);

	/*!
	   \brief    �Ƴ���Ϣ����
	   \param    INotifyUI * pNotifyUI 
	   \return   void 
	 ************************************/
	void RemoveNotifier(INotifyUI* pNotifyUI);

	/*!
	   \brief    �������Ͽؼ�
	   \param    LPCTSTR lpszTemplateName ������ʽ����
	   \param    CContainerUI * pParentCtn ���ؼ�
	   \return   bool 
	 ************************************/
	virtual bool Create(LPCTSTR lpszTemplateName, CContainerUI* pParentCtn);
	/*!
	   \brief    �������Ͽؼ�
	   \param    LPCTSTR lpszTemplateName ������ʽ����
	   \param    CContainerUI * pParentCtn 
	   \param    LPCTSTR lpszCtrlId ����ID
	   \return   bool 
	 ************************************/
	virtual bool CreateEx(LPCTSTR lpszTemplateName, CContainerUI* pParentCtn, LPCTSTR lpszCtrlId, 
		Coordinate* pCdt = NULL, const std::map<tstring, tstring>* pmapAttrExt = NULL);
	/*!
	   \brief    �������̵Ļص�
	   \return   void 
	 ************************************/
	virtual void OnCreate(){}

	/*!
	   \brief    ��Ϣ����Ĭ�ϴ���
	   \param    TNotifyUI * pNotify 
	   \return   bool 
	 ************************************/
	virtual bool ProcessNotify(TNotifyUI* pNotify);

	//�̳к���
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void OnDestroy();

	LPCTSTR GetExtAttribute(LPCTSTR lpszName);

	void SetTemplateId(LPCTSTR lpszTemplateId);
	//{{
protected:
	virtual void  Init();

private:
	CStdPtrArray m_aNotifiers;
	std::map<tstring, tstring> m_mapAttrExt;
	tstring m_strTemplateId;
	//}}
};

#endif //__DIRECTUI_COMPOSITE_H_