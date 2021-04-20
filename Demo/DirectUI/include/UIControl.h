/*********************************************************************
*       Copyright (C) 2010,Ӧ���������
*********************************************************************
*   Date             Name                 Description
*   2010-10-25       xqb
*********************************************************************/
#ifndef __DIRECTUI_CONTROL_H_
#define __DIRECTUI_CONTROL_H_


//{{

//}}
/*!
    \brief    �����ؼ�
*****************************************/
class DUI_API CControlUI : public CViewUI
{
	friend class CContainerUI;
	friend class UICustomControlHelper;
	UI_OBJECT_DECLARE(CControlUI, _T("Control"))
public:
	CControlUI();
	virtual ~CControlUI();

public:

	// ��������
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	bool IsClass(LPCTSTR lpszName);

	// �ؼ���ʶ
	UINT GetFlags();
	void ModifyFlags(UINT uAddFlags, UINT uDelFlags);
	
	// ��ʽ��ز���
	void		SetStyle(LPCTSTR lpszName);
	LPCTSTR		GetStyleName();
	int			GetStyleCount();			//��ȡ��ʽ����
	int			GetStyleIndex(LPCTSTR lpszId);
	StyleObj*	GetStyle(LPCTSTR lpszId);
	StyleObj*	GetStyle(int nIndex);
	ImageStyle* GetImageStyle(LPCTSTR lpszId);
	TextStyle*	GetTextStyle(LPCTSTR lpszId);
	CStdPtrArray* GetStyle();  // ��ȡ��ʽ����
	TextStyle* GetDefaultTextStyle();

	// ���û��ȡ�ı�
	virtual LPCTSTR GetText();
	virtual void	SetText(LPCTSTR lpszText);

	virtual LPCTSTR GetTextKey();

	// ������ʾ����
	void SetToolTip(LPCTSTR lpszTips);
	LPCTSTR GetToolTip();

	// ���ÿͻ����봰�����ļ��
	void SetInset(RECT& rect);
	RECT& GetInset();
	RECT GetInterRect();

	// ���ÿؼ������������е�λ��
	virtual void SetRect(RECT& rectRegion);

	// ���ý���
	bool IsFocused();
	virtual void SetFocus();
	void KillFocus();
	// ��ȡ������� 
	HCURSOR GetCursor();
	
	// �������Ƿ�ɼ�����������Ҫ���ڲ�ʹ�õģ�һ���������д��ھ���Ŀؼ�
	bool IsInternVisible();
	virtual void SetInternVisible(bool bVisible);
	virtual void SetVisible(bool bVisible); // ���ؿؼ��ɼ���

	bool IsExceptionGroup();
	// ������
	bool IsEnabled();
	virtual void SetEnabled(bool bEnable);

	// �Ƿ��������Ӧ��tooltip��ʾ
	bool IsMouseEnabled();
	void SetMouseEnabled(bool bEnable);

	// �Ƿ����ô���������
	bool IsGestureEnabled();
	void SetGestureEnabled(bool bEnable);

	// �Ƿ����ô���������
	virtual bool IsMouseWhellEnabled();
	void SetMouseWhellEnabled(bool bEnable);

    bool IsNeedMouseInOut();
    void SetNeedMouseInOut(bool bEnable);
	// �ؼ��ڸ��ؼ��ϵ�����
	int	 GetIndex();
	void SetIndex(int nIndex);

	// �ؼ��ڸ��ؼ��Ƿ��Զ�����
	bool IsAutoSize();
	void SetAutoSize(bool bAutoSize);

	bool IsSizeWithContent();
	void SetSizeWithContent(bool bSizeWidthContent);

	// �ؼ��ڸ��ؼ����Ƿ���Ҫ�����������
	bool IsNeedScroll();
	void SetNeedScroll(bool bNeedScroll);

	// �ؼ��Ƿ�ѡ��״̬
	bool IsSelected();
	virtual void Select(bool bSelect);

	// �û��Զ�������
	UINT GetUserData();
	void SetUserData(UINT uData);

	LPCTSTR GetTag();
	void	SetTag(LPCTSTR lpstrTag);
	
	// ���ø����ں͸��ؼ�
	virtual void SetManager(CWindowUI* pWindow, CControlUI* pParent);
	// ��ȡ������
	CWindowUI* GetWindow();
	// ��ȡ�ؼ�
	CControlUI* GetParent();
	void SetParent(CControlUI* pParent);

	// ��ȡӵ����
	CControlUI*	GetOwner();
	void SetOwner(CControlUI* pOwner);

	// ���û�ȡ�ؼ�Z��˳��
	uint8 GetZOrder();
	void SetZOrder(uint8 nZOrder);

	// �ⲿ�¼�
	INotifyUI* GetINotifyUI();
	virtual void SetINotifyUI(INotifyUI* pINotify);
	
	// ���ÿؼ�ɾ��֪ͨ�ӿ�
	void SetControlDestoryNotify(IUIControlDestory* pControlDestory);
	IUIControlDestory* GetControlDestoryNotify();

	bool  SendNotify(int type, WPARAM wParam = NULL, LPARAM lParam = NULL); 	// �����ⲿ�¼�֪ͨ
	bool  SendEvent(TEventUI& event); 	// �����ڲ��¼�֪ͨ

	// �ڲ��¼�
	bool EventToParent() const;
	void EventToParent(bool val);

	CEventSource OnEvent; // �ڲ��¼�����
	CEventSource OnNotify; // �ⲿ�¼�����

	// ��������ʱ��
	int  SetTimer(UINT nTimerID, UINT uElapse);
	bool KillTimer(UINT nTimerID);

	virtual RECT GetClientRect();
	virtual RECT GetScrollRect();
	bool HitTest(POINT& point); // �жϵ��Ƿ��ڿؼ�����
	bool HitClientTest(POINT& point); // �жϵ��Ƿ��ڿؼ��ͻ�������
	

	bool IsToolTipShow();
	void SetToolTipShow(bool bShow);
	void ShowToolTip( LPCTSTR lpszContent, UITYPE_TOOLTIP type = UITOOLTIP_Info, LPCTSTR lpszAlign = _T("TL"), LPPOINT lpPoint = NULL );
	// �ؼ���������ͼƬ����
	ImageObj* GenerateImage();

	virtual bool IsContainer();
	virtual bool Resize();
	virtual bool OnlyResizeChild();
	virtual void Invalidate(bool bUpdateWindow = false);
	virtual void OnSize(const RECT& rectParent);
	virtual bool Activate();
	// �ؼ���ʼ��
	virtual void Init();
	// �ؼ�����
	virtual void OnDestroy();
	// �ͷ���Դ
	virtual void ReleaseImage();
	// �����л�֪ͨ
	virtual void OnLanguageChange();
	// �ؼ�����
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	// �ؼ��ڲ��¼�
	virtual bool Event(TEventUI& event);
	// �ؼ��ⲿ�¼�
	virtual bool Notify(TNotifyUI* msg);
	//// �����ӿؼ����ڲ��¼�
	virtual bool ChildEvent(TEventUI& event);

	// ���º�������Container����
public:
	// �ӿؼ�����
	virtual int  GetCount();
	// ����ӿؼ�
	virtual bool Add(CControlUI* pControlChild);
	// �����ӿؼ�
	virtual bool Insert(CControlUI* pControlChild, CControlUI* pControlAfter);
	// �Ƴ��ӿؼ�
	virtual bool Remove(CControlUI* pControlChild);
	// ��ȡ�ӿؼ�
	virtual CControlUI* GetItem(int iIndex);
	// ��ȡ�ӿؼ�
	virtual CControlUI* GetItem(LPCTSTR lpszId);
	//����Proc�ĺ������ҿؼ�
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	CControlUI* FindItem(LPCTSTR lpszId);

protected:
	// ���ڰ������汾
	void FlipRect();
	// ������ʽ
	void RenderStyle(IRenderDC* pRenderDC, RECT& rcPaint, int nBegin = 0, int nEnd = -1);

	LPCTSTR GetTooltipKey();
	friend class CComboBoxUI;

	/// �ؼ�����������Ϣ
	tstring m_strText;
	/// �Ƿ񽹵�ؼ�
	bool m_bFocused;	
	//}}
private:
	/// �ؼ��������µĿ�϶
	RECT m_rcInset;
	CStdPtrArray* m_pStyleList;// ��ʽ�б�
	CControlUI* m_pParent; 	// ���ؼ�
	CControlUI* m_pOwner;   // ӵ����
	CWindowUI* m_pManager;  // ��������
	tstring m_strStyleName; // ��ʽ����
	LPTSTR m_lpszClassName;
	int m_nIndex;			// �ڸ������е����
	bool m_bEnable;			// �Ƿ����ؼ��¼���Ӧ
	bool m_bMouseEnable;	// ControlĬ�ϲ���Ӧ����ƶ��¼�, ��������������Ӧ����¼�
	bool m_bGestureEnable;	// �Ƿ����ô�������
	bool m_bMouseWhellEnable;	// �Ƿ�����mousewhell��Ϣ
	bool m_bInterVisible;	// �ؼ��Ƿ���ʾ�����ø�ֵʱ��ˢ�¸ÿؼ���λ����Ϣ
	bool m_bNeedScroll;		// �Ƿ���Ҫ�����������
	UINT m_uControlFlags;	// �ؼ���־
	bool m_bAutoSize;		// �ؼ��Ƿ�����������С���������С
	bool m_bSelect;			// �Ƿ�ѡ��
	UINT m_uUserData;		// �û�����
	tstring m_strTag;       // �û�����
	CursorObj* m_pCursorObj;// ���
	RefCountedThreadSafe<INotifyUI>* m_pINotifySafe;// �ɷ��¼�����
	bool m_bEventToParent;	// �����ؼ�����EventToParentΪtrue, ���ӿؼ�����EventToParentΪtrue;

	uint8  m_nZOrder;		// Z��˳��
	uint8  m_nZOrders;		// Z��������

	bool m_bAsyncNotify;	// ֪ͨ�첽
	HWND m_hToolTipWnd;
	// ��ʾ�������
	bool m_bShowToolTips;
	tstring m_strToolTips;

	LPTSTR m_lpszTextKey;   // ����m_strText��,���ڶ������л�
	LPTSTR m_lpszToolTipsKey;//����m_strToolTips��,���ڶ������л�

	bool	m_bOnSizeCallback;//
	bool	m_bExceptGroup;

	bool m_bSizeWithContent;
	TextStyle* m_pDefTextStyle;
	IUIControlDestory* m_pUIControlDestory;
	bool    m_bNeedMouseInOutEvent;
};


#endif //__DIRECTUI_CONTROL_H_