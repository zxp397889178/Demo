#pragma once
#ifndef __DIRECTUI_COMPOSITE_H_
#define __DIRECTUI_COMPOSITE_H_

/*!
    \brief    复合容器控件
*****************************************/
class DUI_API CCompositeUI: public CLayoutUI, public INotifyUI
{
	UI_OBJECT_DECLARE(CCompositeUI, _T("Composite"))
public:
	//{{
	CCompositeUI();
	//}}

	/*!
	   \brief    添加消息监听
	   \note     配合FilterProcessNotify
	   \param    INotifyUI * pNotifyUI 
	   \return   void 
	 ************************************/
	void AddNotifier(INotifyUI* pNotifyUI);

	/*!
	   \brief    移除消息监听
	   \param    INotifyUI * pNotifyUI 
	   \return   void 
	 ************************************/
	void RemoveNotifier(INotifyUI* pNotifyUI);

	/*!
	   \brief    创建复合控件
	   \param    LPCTSTR lpszTemplateName 容器样式名称
	   \param    CContainerUI * pParentCtn 父控件
	   \return   bool 
	 ************************************/
	virtual bool Create(LPCTSTR lpszTemplateName, CContainerUI* pParentCtn);
	/*!
	   \brief    创建复合控件
	   \param    LPCTSTR lpszTemplateName 容器样式名称
	   \param    CContainerUI * pParentCtn 
	   \param    LPCTSTR lpszCtrlId 自身ID
	   \return   bool 
	 ************************************/
	virtual bool CreateEx(LPCTSTR lpszTemplateName, CContainerUI* pParentCtn, LPCTSTR lpszCtrlId, 
		Coordinate* pCdt = NULL, const std::map<tstring, tstring>* pmapAttrExt = NULL);
	/*!
	   \brief    创建过程的回调
	   \return   void 
	 ************************************/
	virtual void OnCreate(){}

	/*!
	   \brief    消息监听默认处理
	   \param    TNotifyUI * pNotify 
	   \return   bool 
	 ************************************/
	virtual bool ProcessNotify(TNotifyUI* pNotify);

	//继承函数
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