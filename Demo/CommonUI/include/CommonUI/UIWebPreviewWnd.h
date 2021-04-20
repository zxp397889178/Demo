#pragma once

//��ʱ���ܲμ�CommonUI���̱��룬��������ñ���
class CCefUILayout;
class CompetitionCefBiz;
class CWebPreviewBiz;
struct RequestPreviewWebWnd;

class  CWebPreviewWndUI : public CWindowUI,
	public CWindowPtrRef,
	public CWebPreviewInterface
{
public:
	UIBEGIN_MSG_MAP
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_close"),   OnBtnClose);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_mini"),    OnBtnMini);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_backoff"), OnBtnBackOff);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_forward"), OnBtnForward);
		UI_EVENT_ID_HANDLER(UINOTIFY_CLICK, _T("btn_refresh"), OnBtnRefresh);
	UIEND_MSG_MAP

	CWebPreviewWndUI();
	virtual ~CWebPreviewWndUI();

public:
	HWND                CreateWebPreviewWnd(WebPreviewInfo* previewInfo, bool bShowMask = false);
	void				UpdateWndPos(HWND hParentWnd);
	void                SetParentWnd(HWND hParentWnd);//���ĸ�����
	void                ShowWebPreviewWnd();
	void                ReCreate(RequestPreviewWebWnd* pEvent);

	//Biz
	void                SetWebBiz(CWebPreviewBiz* pBiz);
	void                InitWebBiz();
	void                DestroyWebBiz();

	virtual LPCTSTR     GetLoadedUrl();
	virtual void        ReloadCefUrl(LPCTSTR lptcsUrl);
	virtual HWND        GetWebCefWnd();
	virtual HWND        GetPreviewHwnd();
	virtual void		SetReceiveStatusChange(bool bChange);
	virtual void		ShowCefUrl(const wstring& wstrUrl,
									int nType,
									int nQuestionType = 0,
									bool bFileReq = false,
									bool bCefHide = false);
	virtual void        SetErrorLanguage(bool bLocal);
protected:
	//cef
	virtual void     	ShowCefWnd(std::wstring& wstrUrl);//��ʾ
	virtual void        ExitCefWnd();//�˳�
	virtual void		BackOffCefWnd();//����
	virtual void		ForwardCefWnd();//ǰ��
	virtual void		ReloadCefWnd();//ˢ��
	virtual void        MiniPreviewWnd();//��С��
	virtual void		DoExecJs(std::wstring& wstrJs);//ִ��js���
	//

	//WM_COPYDATA
	virtual void        OnCefInvokeMethodPpt(LPCSTR pszJsonInfo);
	virtual void        OnCefInvokeAsync(LPCSTR pszJsonInfo);
	virtual void        OnCefInvoke(LPCSTR pszJsonInfo);
	virtual void        OnCefLoaded();
	virtual void        OnCefUrlPopUp(LPCTSTR lptcsUrl);
	virtual void        OnCefUrlChange(LPCTSTR lptcsUrl);
	virtual void        OnCefUrlFailed(LPCTSTR lptcsUrl);
	//

	virtual void        CloseWebPreview(); //�ر�
	virtual void        SetPreviewTitle(LPCTSTR lptcsTitle);//Title
	void                SetLoadingVisible(bool bVisible);//loading

	void				UpdateDirection(bool bCanGoBack, bool bCanGoForward);
	bool				CanGoBackCefWnd();
	bool				CanGoForwardCefWnd();
	virtual CControlUI* FindWndControl(LPCTSTR lptcsId);
	virtual void		AddWndNotifier(INotifyUI* pNotifier);
	virtual void		RemoveWndNotifier(INotifyUI* pNotifier);

protected:
	virtual void        OnCreate();
	virtual LRESULT     WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void        OnWindowInit();
	virtual void		OnClose();

protected:
	bool				OnBtnClose(TNotifyUI* msg);	
	bool				OnBtnMini(TNotifyUI* msg);
	bool				OnBtnBackOff(TNotifyUI* msg);
	bool				OnBtnForward(TNotifyUI* msg);
	bool				OnBtnRefresh(TNotifyUI* msg);

protected:
	tstring				m_strUrl;
	tstring				m_strLoadedUrl;

	CCefUILayout*   m_pWebControl;

	CLayoutUI*          m_pLayLoading;

	CRotateAnimationUI* m_pLoading;

	CWebPreviewBiz*     m_pWebPreviewBiz;

	CButtonUI*          m_pBtnBackOff;
	CButtonUI*          m_pBtnForward;
	float				m_fWidthPercent;
	float				m_fHeightPercent;
	float				m_fConstraintSspectRatio;
	RequestPreviewWebWnd*
						m_pEvent = NULL;
	bool				m_bShowMask;
	tstring				m_strWndId;
};