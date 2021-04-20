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
	// 设置窗口属性
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue); 
	void SetUserAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	LPCTSTR GetUserAttribute(LPCTSTR lpszName);

	// 窗口样式id
	LPCTSTR GetId();

	// 设置和获取用户标识id,该标识是由用户自己任意设置，主要用于GetUIEngine()->GetWindow(LPCTSTR lpszId, LPCTSTR lpszSubId),查找窗口使用
	LPCTSTR GetSubId();
	void SetSubId(LPCTSTR lpszSubId);

	// 获取窗口的控件，多层用“."隔开，如GetItem(_T("control1.control2")),表示获取控件control1中的子控件control2
	CContainerUI* GetRoot();
	CControlUI * GetItem(LPCTSTR lpszName);

	// 创建窗口
	HWND Create(HWND hParent, LPCTSTR lpszId,  int x = 0, int y = 0, int nWidth = 0, int nHeight = 0);
	// 肤化一个已存在的窗口
	void SkinWindow(HWND  hWnd, LPCTSTR lpszId,  int x = 0, int y = 0, int nWidth = 0, int nHeight = 0);

	// 模式窗口和销毁
	virtual UINT DoModal(int nCmdShow = SW_SHOWNORMAL);
	bool IsModal();
	void EndDialog(UINT nResult = IDOK);

	// 非模式窗口和销毁
	virtual bool ShowWindow(int nCmdShow = SW_SHOW);
	virtual void CloseWindow(UINT nResult = IDOK);
	UINT GetCloseCode();

	// 最大化最小化还原窗口
	virtual void MaximizeWindow();
	virtual void MinimizeWindow();
	virtual void RestoreWindow();
	bool IsMaximized();
	bool IsMinimized();

	// 改变位置和居中
	void SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
	void CenterWindow(HWND hAlternateOwner = NULL);

	// 设置标题栏设置
	void SetTitle(LPCTSTR lpszText);
	LPCTSTR GetTitle();

	// 设置窗口大图标和小图标
	void SetIcon(LPCTSTR lpszIcon, bool bBigIcon = false);
	HICON GetIcon();
	HICON GetBigIcon();

	// 设置背景颜色和背景图片
	void SetBkColor(DWORD dwColor);
	void SetBkImage(LPCTSTR lpszFileName);

	// 设置窗口透明度
	void SetAlpha(int nAlpha);
	int  GetAlpha();

	// 窗口是否能改变大小
	void EnableResize(bool bEnableResize);
	bool IsEnableResize();

	// 窗口是否能够移动
	void EnableMove(bool bEnableMove);
	bool IsEnableMove();

	// 设置和销毁定时器
	int  SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
	bool KillTimer(CControlUI* pControl, UINT nTimerID);

	// 设置和获取焦点控件
	virtual void SetFocus(CControlUI* pControl);
	virtual void KillFocus(CControlUI* pControl);
	CControlUI* GetFocus();

	// 设置和获取窗口Capture
	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured();

	// 激活窗口和显示激活窗口
	void ActiveWindow();
	void ShowAndActiveWindow();

	// 发送控件通知
	bool SendNotify(CControlUI* pControl, int nType, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsyn = false);
	
	// 添加控件通知处理
	void AddNotifier(INotifyUI* pNotifyUI);
	void RemoveNotifier(INotifyUI* pNotifyUI);  

	// 添加消息过滤
	void AddMessageFilter(IMessageFilterUI* pFilter);
	void RemoveMessageFilter(IMessageFilterUI* pFilter);

	// 查找控件
	CControlUI* FindControl(POINT& pt);
	CControlUI* FindControl(POINT& pt, UINT uFlags);
	CControlUI* FindControl(LPCTSTR lpszId, CControlUI* pParent = NULL);

	// 绘制DC
	HDC		GetPaintDC();
	HDC		GetMemDC();
	DibObj* GetDibObj();

	// 刷新窗口
	void Invalidate(RECT* lpRect = NULL, bool bUpdateWindow = false);
	// 绘制到IRenderDC上
	void Draw(IRenderDC* pRenderDC, LPRECT lpClipRect = NULL);

	// 锁定/解锁绘制
	bool LockUpdate();
	void UnLockUpdate();

	// 设置层叠窗口
	void SetLayerWindow(bool bLayer, bool bRedraw = true);
	bool IsLayerWindow();
	void UpdateLayeredWindow(LPRECT lpRect);

	// 设置边缘阴影
	void SetShadow(bool bShadow);
	// 设置窗口边缘大小
	void SetBorder(RECT& rect);

	// 设置或获取窗口的最小宽度高度
	void SetMinWidth(int nMinWidth);
	int  GetMinWidth();
	void SetMinHeight(int nMinHeight);
	int  GetMinHeight();

	void SetMaxWidth(int nMaxWidth);
	int  GetMaxWidth();
	void SetMaxHeight(int nMaxHeight);
	int  GetMaxHeight();

	// 控件拖拉
	void SetEnableDragDrop(bool bDragDrop);
	HRESULT DoDrag(CControlUI* pControl, IN LPDATAOBJECT pDataObj,  IN DWORD dwOKEffects, OUT LPDWORD pdwEffect);

	// 打开关闭输入法
	void EnableIME();
	void DisableIME();

	bool IsWindowRender();
	// 鼠标点击引起激活本窗口焦点
	CControlUI* GetEventClickControl();
	CControlUI* GetEventHoverControl();
	
	void SetMsgHandled(bool bMsgHanled);
	void HideToolTips();
	void ShowToolTip(LPCTSTR lpszText, LPRECT lpRect);
	
    //Tab键是否可用
    void SetVKTabEnable(bool bEnable);
    bool IsVKTabEnable();

	// 设置默认的fonttype;
	void SetDefaultFontType(UITYPE_FONT eFontType);
	UITYPE_FONT GetDefaultFontType();

	void SetDefaultTextRenderingHint(int textRenderingHint);
	int GetDefaultTextRenderingHint();

public:
	// 以下函数全部没用接口难懂，即将全部处理掉
	virtual void OnShowFristSide();
	virtual void OnShowSecondSide();
	//{{
	virtual void OnLanguageChange();					// 语言变化通知
	virtual void OnSkinChange(bool bAnimation); 		// 皮肤变化通知

	// 虚函数重载
protected:
	virtual void OnCreate();
	virtual void OnClose();
	virtual bool OnClosing();
	virtual void OnOk();
	virtual void OnCancel();
	virtual void OnMinimize();
	virtual void OnMaximize();
	virtual void OnRestored();
	virtual void OnWindowInit();						// 窗口第一次显示
	virtual void OnSetWindowRgn();						// 设置窗口rgn
	virtual void OnControlNotify(TNotifyUI* pNotify);	// 所有控件的通知消息
	virtual bool    PreMessageHandler(const LPMSG pMsg, LRESULT& lRes);     //消息预处理过程函数
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam); //窗口过程函数

	virtual void OnDrawBefore(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint);
	virtual void OnDrawAfter(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint);
protected:
	TipWindow* m_pTipWindow;
	HWND m_hTipWindow;
	bool m_bModalDlg;    //是否是模式窗口
	UINT m_uModalResult;
private:
	bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	LRESULT HitTest(WPARAM   wParam,   LPARAM   lParam);
	LRESULT DecodeGesture(WPARAM wParam, LPARAM lParam);
	void MoveShadowWindow();
	void FlushToBackBuffer(RECT rcPaint, RECT rcClient);
	void FlushToForeBuffer(RECT rcPaint, HDC hDC);
	void ReapObjects(CControlUI* pControl); //控件pControl删除时，进行处理否则会崩溃
	bool SetNextTabControl(bool bForward = true);
	TipWindow* GetToolTipWnd();
	void SetIconFromResource(UINT uId, bool bBigIcon = false);
	//私有成员
	CWindowUI* m_pShadowWindow; //阴影窗口
	bool m_bShadow;      // 是否阴影
	RECT m_rectWork;   // 窗口最小化后最大化 rcwork会不准确，因此必须使用最小化之前的数据
	RECT m_rectUpdate; // 
	RECT m_rcBorder;
	bool m_bEnableResize;   //是否能够改变大小
	bool m_bEnableMove;     //是否能够移动位置
	bool m_bMaximized;
	bool m_bMinimized;
	RECT m_rcRestore;
	bool m_bLayerWindow; //是否是层叠窗口

	tstring m_strSubId;
	tstring m_strTitle, m_strSmallIcon, m_strBigIcon;
	LPTSTR  m_lpszTitleKey;
	tstring m_strOkControl; //按Enter回车键默认执行的控件
	int m_iMinWidth,m_iMinHeight,m_iMaxWidth,m_iMaxHeight,  m_iOrgWidth, m_iOrgHeight;
	HICON   m_hIcon, m_hBigIcon; //标题

	CContainerUI* m_pRoot;
	CControlUI* m_pFocus;
	CControlUI* m_pEventHover;
	CControlUI* m_pEventClick;
	CControlUI* m_pEventKey;
	CControlUI* m_pEventDrag;
	CControlUI* m_pControlDrag;
    CStdPtrArray m_aMessageFilters;
    CControlUI* m_pEventMouseInOut;
	CControlUI* m_pGuesture; // 当前手势的控件

	HDC m_hDcPaint;
	DibObj* m_pDibObj;

	IDropTarget* m_pDragTarget;
	bool m_bDraging;
	bool m_bEnableDragDrop;

	bool m_bMsgHandled;
	bool m_bAreaSelecting;	/// 框选
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
    bool m_bVKTabEnable; //Tab键

	HIMC   m_hImc; //开启关闭输入法
	UINT m_uTimerID;  // 定时器
	CStdPtrArray m_aTimers;// 定时器
	CStdPtrArray m_aNotifiers; //通知消息过滤
	map<tstring, tstring>m_mapUserAttribute;
	UITYPE_FONT m_eFontType; // 窗口可以设置默认的fonttype;
	int m_nTextRenderingHint;
};

//////////////////////////////////////////////////////////////////////////
// NoActiveWindow : 非激活，注意调用SetWindowPos注意加上SWP_NOACTIVATE, ShowWindow要使用SW_SHOWNOACTIVATE,
class  DUI_API CNoActiveWindowUI : public CWindowUI
{
	//{{
public:
	CNoActiveWindowUI();
	virtual ~CNoActiveWindowUI();

protected:
	//}
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnSetWindowRgn(){} //非激活窗口一般没有圆角
	virtual void OnCancel(){}       //非激活窗口按ESC一般不退出

	//{{
private:
	static CNoActiveWindowUI* ms_pModalWindow;
	//}
};





















