/*********************************************************************
*       Copyright (C) 2010,Ӧ���������
*********************************************************************
*   Date             Name                 Description
*   2010-12-1		 hanzp				  Create.
*********************************************************************/
#pragma once


/*!
    \brief    �ָ�ؼ�
*****************************************/
class DUI_API CSplitterUI :public CControlUI
{
	UI_OBJECT_DECLARE(CSplitterUI, _T("Splitter"))
public:
	CSplitterUI();
	virtual ~CSplitterUI();
	//}}

	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void SetRect(RECT& rectRegion);
	//{{
protected:
	bool Event(TEventUI& event);
	bool m_bHorizontal; 
	tstring m_szFirstName, m_szSecondName; //ǰһ���ؼ��ͺ�һ���ؼ�������
	CControlUI* m_pFirst, *m_pSecond;  //��һ���ؼ��͵ڶ��ؼ�
	int m_nMinFirst, m_nMinSecond;     //��һ���ؼ��͵ڶ����ؼ�����Сֵ
	POINT m_ptLastMouse;
	RECT m_rcFirstCfg, m_rcSecondCfg, m_rcSefCfg; //���ڱ��ݵ�һ���ڶ����ؼ�������ĳ�ʼ����
	//}}
};
