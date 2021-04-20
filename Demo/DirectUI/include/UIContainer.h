#pragma once

/*!
    \brief    �����ؼ�
*****************************************/
class  DUI_API CContainerUI : public CControlUI
{
	UI_OBJECT_DECLARE(CContainerUI, _T("Container"))
public:
	//{{
	CContainerUI();
	virtual ~CContainerUI();

public:
	// �����ӿؼ��ӿڣ���CControlUI�����Ľӿڣ�
	virtual int GetCount();
	virtual CControlUI* GetItem(int iIndex);
	virtual CControlUI* GetItem(LPCTSTR lpszId);
	virtual bool Add(CControlUI* pControlChild);
	virtual bool Insert(CControlUI* pControlChild, CControlUI* pControlAfter);
	virtual bool Remove(CControlUI* pControlChild);
	virtual void RemoveAll();
	// �����ӿؼ��ӿ�
	CStdPtrArray* GetItemArray();
	bool RemoveWithoutDelCtrl(CControlUI* pControlChild);  
	bool MoveItem(CControlUI* pControlChild, CControlUI* pControlAfter); // �ƶ��ӿؼ�
	bool Contain(CControlUI* pControlChild);
	virtual bool SelectItem(CControlUI* pControl);
	CControlUI* GetSelectedItem();

	//  ��������ؽӿ�
	void EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal);
	bool IsEnableVScrollbar();
	bool IsEnableHScrollbar();
	CScrollbarUI* GetVScrollbar();
	CScrollbarUI* GetHScrollbar();

	virtual void SetVisible(bool bShow);
	virtual void SetInternVisible(bool bShow);
	virtual bool IsContainer();

	virtual void SetRect(RECT& rectRegion);
	virtual void SetManager(CWindowUI* pManager, CControlUI* pParent);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	virtual void ReleaseImage();
	virtual RECT GetClientRect();
	virtual RECT GetScrollRect();
	virtual void SetINotifyUI(INotifyUI* pINotify);

	virtual void SetAlpha(int val);

	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual bool Event(TEventUI& event);
	// ��������ӿ��Ƕ���ģ�Ϊ��101ppt��ʱ����
	CControlUI* FindSubControl(LPCTSTR pstrSubControlName);
	virtual bool OnlyResizeChild();
	virtual void OnlyShowChild(CControlUI *pItem);
protected:
	virtual void OnProcessScrollbar(int cxRequired, int cyRequired);
	virtual SIZE OnChildSize(RECT& rcWinow, RECT& rcClient);
	virtual void OnLanguageChange();
	virtual	void CalcChildRect();
	bool SelectControl(CControlUI* pItem);
	CControlUI* GetCurSelControl();
private:
	void HandleGestureEvent(TEventUI& event);
	CStdPtrArray m_items;
	CControlUI* m_pCurSelItem;
	RECT m_rcClient; // �ͻ�������
	bool m_bEnableVertical;
	bool m_bEnableHorizontal;
	bool m_bShowVertical;        //��Ⱦʱ�Ƿ���ʾ��ֱ������
	bool m_bShowHorizontal;      //��Ⱦʱ�Ƿ���ʾˮƽ������
	bool m_bFolatVertical;       //��ֱ�������Ƿ�ռ���ӿؼ�����
	bool m_bFolatHorizontal;     //ˮƽ�������Ƿ�ռ���ӿؼ�����
	CScrollbarUI* m_pVerticalScrollbar;
	CScrollbarUI* m_pHorizontalScrollbar;
	bool m_bAutoFitHeight;
	bool m_bAutoFitWidth;
	/// ��ֹOnSizeѭ������
	bool m_bScrollProcess;
	//}}

};


