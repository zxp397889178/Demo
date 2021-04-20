#pragma once

#include "WindowBase.h"
class DUI_API TipWindow;
class DUI_API CWindowUI : public CWindowBase
{
	//{{
	friend class CControlUI;
public:
	CWindowUI();
	virtual ~CWindowUI();

public: 
	UIBEGIN_MSG_MAP
	UIEND_MSG_MAP
	//}}
public:
	// ���ô�������
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue); 
	void SetUserAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	LPCTSTR GetUserAttribute(LPCTSTR lpszName);

	// ������ʽid
	LPCTSTR GetId();

	// ���úͻ�ȡ�û���ʶid,�ñ�ʶ�����û��Լ��������ã���Ҫ����GetUIEngine()->GetWindow(LPCTSTR lpszId, LPCTSTR lpszSubId),���Ҵ���ʹ��
	LPCTSTR GetSubId();
	void SetSubId(LPCTSTR lpszSubId);

	// ��ȡ���ڵĿؼ�������á�."��������GetItem(_T("control1.control2")),��ʾ��ȡ�ؼ�control1�е��ӿؼ�control2
	CContainerUI* GetRoot();
	CControlUI * GetItem(LPCTSTR lpszName);

	// ��������
	HWND Create(HWND hParent, LPCTSTR lpszId,  int x = 0, int y = 0, int nWidth = 0, int nHeight = 0);
	// ����һ���Ѵ��ڵĴ���
	void SkinWindow(HWND  hWnd, LPCTSTR lpszId,  int x = 0, int y = 0, int nWidth = 0, int nHeight = 0);

	// ģʽ���ں�����
	virtual UINT DoModal(int nCmdShow = SW_SHOWNORMAL);
	bool IsModal();
	void EndDialog(UINT nResult = IDOK);

	// ��ģʽ���ں�����
	virtual bool ShowWindow(int nCmdShow = SW_SHOW);
	virtual void CloseWindow(UINT nResult = IDOK);
	UINT GetCloseCode();

	// �����С����ԭ����
	virtual void MaximizeWindow();
	virtual void MinimizeWindow();
	virtual void RestoreWindow();
	bool IsMaximized();
	bool IsMinimized();

	// �ı�λ�ú;���
	void SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
	void CenterWindow(HWND hAlternateOwner = NULL);

	// ���ñ���������
	void SetTitle(LPCTSTR lpszText);
	LPCTSTR GetTitle();

	// ���ô��ڴ�ͼ���Сͼ��
	void SetIcon(LPCTSTR lpszIcon, bool bBigIcon = false);
	HICON GetIcon();
	HICON GetBigIcon();

	// ���ñ�����ɫ�ͱ���ͼƬ
	void SetBkColor(DWORD dwColor);
	void SetBkImage(LPCTSTR lpszFileName);

	// ���ô���͸����
	void SetAlpha(int nAlpha);
	int  GetAlpha();

	// �����Ƿ��ܸı��С
	void EnableResize(bool bEnableResize);
	bool IsEnableResize();

	// �����Ƿ��ܹ��ƶ�
	void EnableMove(bool bEnableMove);
	bool IsEnableMove();

	// ���ú����ٶ�ʱ��
	int  SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
	bool KillTimer(CControlUI* pControl, UINT nTimerID);

	// ���úͻ�ȡ����ؼ�
	virtual void SetFocus(CControlUI* pControl);
	virtual void KillFocus(CControlUI* pControl);
	CControlUI* GetFocus();

	// ���úͻ�ȡ����Capture
	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured();

	// ����ں���ʾ�����
	void ActiveWindow();
	void ShowAndActiveWindow();

	// ���Ϳؼ�֪ͨ
	bool SendNotify(CControlUI* pControl, int nType, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsyn = false);
	
	// ��ӿؼ�֪ͨ����
	void AddNotifier(INotifyUI* pNotifyUI);
	void RemoveNotifier(INotifyUI* pNotifyUI);  

	// �����Ϣ����
	void AddMessageFilter(IMessageFilterUI* pFilter);
	void RemoveMessageFilter(IMessageFilterUI* pFilter);

	// ���ҿؼ�
	CControlUI* FindControl(POINT& pt);
	CControlUI* FindControl(POINT& pt, UINT uFlags);
	CControlUI* FindControl(LPCTSTR lpszId, CControlUI* pParent = NULL);

	// ����DC
	HDC		GetPaintDC();
	HDC		GetMemDC();
	DibObj* GetDibObj();

	// ˢ�´���
	void Invalidate(RECT* lpRect = NULL, bool bUpdateWindow = false);
	// ���Ƶ�IRenderDC��
	void Draw(IRenderDC* pRenderDC, LPRECT lpClipRect = NULL);

	// ����/��������
	bool LockUpdate();
	void UnLockUpdate();

	// ���ò������
	void SetLayerWindow(bool bLayer, bool bRedraw = true);
	bool IsLayerWindow();
	void UpdateLayeredWindow(LPRECT lpRect);

	// ���ñ�Ե��Ӱ
	void SetShadow(bool bShadow);
	// ���ô��ڱ�Ե��С
	void SetBorder(RECT& rect);

	// ���û��ȡ���ڵ���С��ȸ߶�
	void SetMinWidth(int nMinWidth);
	int  GetMinWidth();
	void SetMinHeight(int nMinHeight);
	int  GetMinHeight();

	void SetMaxWidth(int nMaxWidth);
	int  GetMaxWidth();
	void SetMaxHeight(int nMaxHeight);
	int  GetMaxHeight();

	// �ؼ�����
	void SetEnableDragDrop(bool bDragDrop);
	HRESULT DoDrag(CControlUI* pControl, IN LPDATAOBJECT pDataObj,  IN DWORD dwOKEffects, OUT LPDWORD pdwEffect);

	// �򿪹ر����뷨
	void EnableIME();
	void DisableIME();

	bool IsWindowRender();
	// ��������𼤻���ڽ���
	CControlUI* GetEventClickControl();
	CControlUI* GetEventHoverControl();
	
	void SetMsgHandled(bool bMsgHanled);
	void HideToolTips();
	void ShowToolTip(LPCTSTR lpszText, LPRECT lpRect);
	
    //Tab���Ƿ����
    void SetVKTabEnable(bool bEnable);
    bool IsVKTabEnable();

	// ����Ĭ�ϵ�fonttype;
	void SetDefaultFontType(UITYPE_FONT eFontType);
	UITYPE_FONT GetDefaultFontType();

	void SetDefaultTextRenderingHint(int textRenderingHint);
	int GetDefaultTextRenderingHint();

public:
	// ���º���ȫ��û�ýӿ��Ѷ�������ȫ�������
	virtual void OnShowFristSide();
	virtual void OnShowSecondSide();
	//{{
	virtual void OnLanguageChange();					// ���Ա仯֪ͨ
	virtual void OnSkinChange(bool bAnimation); 		// Ƥ���仯֪ͨ

	// �麯������
protected:
	virtual void OnCreate();
	virtual void OnClose();
	virtual bool OnClosing();
	virtual void OnOk();
	virtual void OnCancel();
	virtual void OnMinimize();
	virtual void OnMaximize();
	virtual void OnRestored();
	virtual void OnWindowInit();						// ���ڵ�һ����ʾ
	virtual void OnSetWindowRgn();						// ���ô���rgn
	virtual void OnControlNotify(TNotifyUI* pNotify);	// ���пؼ���֪ͨ��Ϣ
	virtual bool    PreMessageHandler(const LPMSG pMsg, LRESULT& lRes);     //��ϢԤ������̺���
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam); //���ڹ��̺���

	virtual void OnDrawBefore(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint);
	virtual void OnDrawAfter(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint);
protected:
	TipWindow* m_pTipWindow;
	HWND m_hTipWindow;
	bool m_bModalDlg;    //�Ƿ���ģʽ����
	UINT m_uModalResult;
private:
	bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	LRESULT HitTest(WPARAM   wParam,   LPARAM   lParam);
	LRESULT DecodeGesture(WPARAM wParam, LPARAM lParam);
	void MoveShadowWindow();
	void FlushToBackBuffer(RECT rcPaint, RECT rcClient);
	void FlushToForeBuffer(RECT rcPaint, HDC hDC);
	void ReapObjects(CControlUI* pControl); //�ؼ�pControlɾ��ʱ�����д����������
	bool SetNextTabControl(bool bForward = true);
	TipWindow* GetToolTipWnd();
	void SetIconFromResource(UINT uId, bool bBigIcon = false);
	//˽�г�Ա
	CWindowUI* m_pShadowWindow; //��Ӱ����
	bool m_bShadow;      // �Ƿ���Ӱ
	RECT m_rectWork;   // ������С������� rcwork�᲻׼ȷ����˱���ʹ����С��֮ǰ������
	RECT m_rectUpdate; // 
	RECT m_rcBorder;
	bool m_bEnableResize;   //�Ƿ��ܹ��ı��С
	bool m_bEnableMove;     //�Ƿ��ܹ��ƶ�λ��
	bool m_bMaximized;
	bool m_bMinimized;
	RECT m_rcRestore;
	bool m_bLayerWindow; //�Ƿ��ǲ������

	tstring m_strSubId;
	tstring m_strTitle, m_strSmallIcon, m_strBigIcon;
	LPTSTR  m_lpszTitleKey;
	tstring m_strOkControl; //��Enter�س���Ĭ��ִ�еĿؼ�
	int m_iMinWidth,m_iMinHeight,m_iMaxWidth,m_iMaxHeight,  m_iOrgWidth, m_iOrgHeight;
	HICON   m_hIcon, m_hBigIcon; //����

	CContainerUI* m_pRoot;
	CControlUI* m_pFocus;
	CControlUI* m_pEventHover;
	CControlUI* m_pEventClick;
	CControlUI* m_pEventKey;
	CControlUI* m_pEventDrag;
	CControlUI* m_pControlDrag;
    CStdPtrArray m_aMessageFilters;
    CControlUI* m_pEventMouseInOut;
	CControlUI* m_pGuesture; // ��ǰ���ƵĿؼ�

	HDC m_hDcPaint;
	DibObj* m_pDibObj;

	IDropTarget* m_pDragTarget;
	bool m_bDraging;
	bool m_bEnableDragDrop;

	bool m_bMsgHandled;
	bool m_bAreaSelecting;	/// ��ѡ
	int  m_nAlpha;

	POINT m_ptLastMousePos, m_ptLastMousePosClick;
	bool m_bMouseTracking;
	bool m_bMouseCapture;
	bool m_bFocusNeeded;
	bool m_bUpdateNeeded;
	bool m_bSizeChanged;
	bool m_bUnFirstPaint;
	bool m_bLockUpdate;
	bool m_bMouseWheel;
    bool m_bVKTabEnable; //Tab��

	HIMC   m_hImc; //�����ر����뷨
	UINT m_uTimerID;  // ��ʱ��
	CStdPtrArray m_aTimers;// ��ʱ��
	CStdPtrArray m_aNotifiers; //֪ͨ��Ϣ����
	map<tstring, tstring>m_mapUserAttribute;
	UITYPE_FONT m_eFontType; // ���ڿ�������Ĭ�ϵ�fonttype;
	int m_nTextRenderingHint;
};

//////////////////////////////////////////////////////////////////////////
// NoActiveWindow : �Ǽ��ע�����SetWindowPosע�����SWP_NOACTIVATE, ShowWindowҪʹ��SW_SHOWNOACTIVATE,
class  DUI_API CNoActiveWindowUI : public CWindowUI
{
	//{{
public:
	CNoActiveWindowUI();
	virtual ~CNoActiveWindowUI();

protected:
	//}
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnSetWindowRgn(){} //�Ǽ����һ��û��Բ��
	virtual void OnCancel(){}       //�Ǽ���ڰ�ESCһ�㲻�˳�

	//{{
private:
	static CNoActiveWindowUI* ms_pModalWindow;
	//}
};





















