/*********************************************************************
*       Copyright (C) 2010,应用软件开发
*********************************************************************
*   Date             Name                 Description
*   2010-10-25       xqb
*********************************************************************/
#ifndef __DIRECTUI_CONTROL_H_
#define __DIRECTUI_CONTROL_H_


//{{

//}}
/*!
    \brief    基础控件
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

	// 属性设置
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	bool IsClass(LPCTSTR lpszName);

	// 控件标识
	UINT GetFlags();
	void ModifyFlags(UINT uAddFlags, UINT uDelFlags);
	
	// 样式相关操作
	void		SetStyle(LPCTSTR lpszName);
	LPCTSTR		GetStyleName();
	int			GetStyleCount();			//获取样式个数
	int			GetStyleIndex(LPCTSTR lpszId);
	StyleObj*	GetStyle(LPCTSTR lpszId);
	StyleObj*	GetStyle(int nIndex);
	ImageStyle* GetImageStyle(LPCTSTR lpszId);
	TextStyle*	GetTextStyle(LPCTSTR lpszId);
	CStdPtrArray* GetStyle();  // 获取样式数组
	TextStyle* GetDefaultTextStyle();

	// 设置或获取文本
	virtual LPCTSTR GetText();
	virtual void	SetText(LPCTSTR lpszText);

	virtual LPCTSTR GetTextKey();

	// 设置提示文字
	void SetToolTip(LPCTSTR lpszTips);
	LPCTSTR GetToolTip();

	// 设置客户区与窗口区的间隔
	void SetInset(RECT& rect);
	RECT& GetInset();
	RECT GetInterRect();

	// 设置控件在所属窗口中的位置
	virtual void SetRect(RECT& rectRegion);

	// 设置焦点
	bool IsFocused();
	virtual void SetFocus();
	void KillFocus();
	// 获取鼠标手势 
	HCURSOR GetCursor();
	
	// 父容器是否可见，该属性主要给内部使用的，一般是用于有窗口句柄的控件
	bool IsInternVisible();
	virtual void SetInternVisible(bool bVisible);
	virtual void SetVisible(bool bVisible); // 重载控件可见性

	bool IsExceptionGroup();
	// 可用性
	bool IsEnabled();
	virtual void SetEnabled(bool bEnable);

	// 是否开启鼠标响应、tooltip显示
	bool IsMouseEnabled();
	void SetMouseEnabled(bool bEnable);

	// 是否启用触摸屏手势
	bool IsGestureEnabled();
	void SetGestureEnabled(bool bEnable);

	// 是否启用触摸屏手势
	virtual bool IsMouseWhellEnabled();
	void SetMouseWhellEnabled(bool bEnable);

    bool IsNeedMouseInOut();
    void SetNeedMouseInOut(bool bEnable);
	// 控件在父控件上的索引
	int	 GetIndex();
	void SetIndex(int nIndex);

	// 控件在父控件是否自动布局
	bool IsAutoSize();
	void SetAutoSize(bool bAutoSize);

	bool IsSizeWithContent();
	void SetSizeWithContent(bool bSizeWidthContent);

	// 控件在父控件上是否需要随滚动条滚动
	bool IsNeedScroll();
	void SetNeedScroll(bool bNeedScroll);

	// 控件是否选中状态
	bool IsSelected();
	virtual void Select(bool bSelect);

	// 用户自定义数据
	UINT GetUserData();
	void SetUserData(UINT uData);

	LPCTSTR GetTag();
	void	SetTag(LPCTSTR lpstrTag);
	
	// 设置父窗口和父控件
	virtual void SetManager(CWindowUI* pWindow, CControlUI* pParent);
	// 获取父窗口
	CWindowUI* GetWindow();
	// 获取控件
	CControlUI* GetParent();
	void SetParent(CControlUI* pParent);

	// 获取拥有者
	CControlUI*	GetOwner();
	void SetOwner(CControlUI* pOwner);

	// 设置获取控件Z轴顺序
	uint8 GetZOrder();
	void SetZOrder(uint8 nZOrder);

	// 外部事件
	INotifyUI* GetINotifyUI();
	virtual void SetINotifyUI(INotifyUI* pINotify);
	
	// 设置控件删除通知接口
	void SetControlDestoryNotify(IUIControlDestory* pControlDestory);
	IUIControlDestory* GetControlDestoryNotify();

	bool  SendNotify(int type, WPARAM wParam = NULL, LPARAM lParam = NULL); 	// 发送外部事件通知
	bool  SendEvent(TEventUI& event); 	// 发送内部事件通知

	// 内部事件
	bool EventToParent() const;
	void EventToParent(bool val);

	CEventSource OnEvent; // 内部事件代理
	CEventSource OnNotify; // 外部事件代理

	// 设置设置时钟
	int  SetTimer(UINT nTimerID, UINT uElapse);
	bool KillTimer(UINT nTimerID);

	virtual RECT GetClientRect();
	virtual RECT GetScrollRect();
	bool HitTest(POINT& point); // 判断点是否在控件区域
	bool HitClientTest(POINT& point); // 判断点是否在控件客户区区域
	

	bool IsToolTipShow();
	void SetToolTipShow(bool bShow);
	void ShowToolTip( LPCTSTR lpszContent, UITYPE_TOOLTIP type = UITOOLTIP_Info, LPCTSTR lpszAlign = _T("TL"), LPPOINT lpPoint = NULL );
	// 控件区域生成图片对象
	ImageObj* GenerateImage();

	virtual bool IsContainer();
	virtual bool Resize();
	virtual bool OnlyResizeChild();
	virtual void Invalidate(bool bUpdateWindow = false);
	virtual void OnSize(const RECT& rectParent);
	virtual bool Activate();
	// 控件初始化
	virtual void Init();
	// 控件析构
	virtual void OnDestroy();
	// 释放资源
	virtual void ReleaseImage();
	// 语言切换通知
	virtual void OnLanguageChange();
	// 控件绘制
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	// 控件内部事件
	virtual bool Event(TEventUI& event);
	// 控件外部事件
	virtual bool Notify(TNotifyUI* msg);
	//// 处理子控件的内部事件
	virtual bool ChildEvent(TEventUI& event);

	// 以下函数留给Container重载
public:
	// 子控件个数
	virtual int  GetCount();
	// 添加子控件
	virtual bool Add(CControlUI* pControlChild);
	// 插入子控件
	virtual bool Insert(CControlUI* pControlChild, CControlUI* pControlAfter);
	// 移除子控件
	virtual bool Remove(CControlUI* pControlChild);
	// 获取子控件
	virtual CControlUI* GetItem(int iIndex);
	// 获取子控件
	virtual CControlUI* GetItem(LPCTSTR lpszId);
	//根据Proc的函数查找控件
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	CControlUI* FindItem(LPCTSTR lpszId);

protected:
	// 用于阿拉伯版本
	void FlipRect();
	// 绘制样式
	void RenderStyle(IRenderDC* pRenderDC, RECT& rcPaint, int nBegin = 0, int nEnd = -1);

	LPCTSTR GetTooltipKey();
	friend class CComboBoxUI;

	/// 控件基本文字信息
	tstring m_strText;
	/// 是否焦点控件
	bool m_bFocused;	
	//}}
private:
	/// 控件边沿留下的空隙
	RECT m_rcInset;
	CStdPtrArray* m_pStyleList;// 样式列表
	CControlUI* m_pParent; 	// 父控件
	CControlUI* m_pOwner;   // 拥有者
	CWindowUI* m_pManager;  // 所属窗口
	tstring m_strStyleName; // 样式名称
	LPTSTR m_lpszClassName;
	int m_nIndex;			// 在父容器中的序号
	bool m_bEnable;			// 是否开启控件事件响应
	bool m_bMouseEnable;	// Control默认不响应鼠标移动事件, 设置这个后可以响应鼠标事件
	bool m_bGestureEnable;	// 是否启用触摸手势
	bool m_bMouseWhellEnable;	// 是否启用mousewhell消息
	bool m_bInterVisible;	// 控件是否显示，重置该值时需刷新该控件的位置信息
	bool m_bNeedScroll;		// 是否需要随滚动条滚动
	UINT m_uControlFlags;	// 控件标志
	bool m_bAutoSize;		// 控件是否依据容器大小调整自身大小
	bool m_bSelect;			// 是否被选中
	UINT m_uUserData;		// 用户数据
	tstring m_strTag;       // 用户数据
	CursorObj* m_pCursorObj;// 光标
	RefCountedThreadSafe<INotifyUI>* m_pINotifySafe;// 派发事件过滤
	bool m_bEventToParent;	// 若父控件设置EventToParent为true, 则子控件设置EventToParent为true;

	uint8  m_nZOrder;		// Z轴顺序
	uint8  m_nZOrders;		// Z轴总数，

	bool m_bAsyncNotify;	// 通知异步
	HWND m_hToolTipWnd;
	// 提示文字相关
	bool m_bShowToolTips;
	tstring m_strToolTips;

	LPTSTR m_lpszTextKey;   // 保存m_strText的,用于多语言切换
	LPTSTR m_lpszToolTipsKey;//保存m_strToolTips的,用于多语言切换

	bool	m_bOnSizeCallback;//
	bool	m_bExceptGroup;

	bool m_bSizeWithContent;
	TextStyle* m_pDefTextStyle;
	IUIControlDestory* m_pUIControlDestory;
	bool    m_bNeedMouseInOutEvent;
};


#endif //__DIRECTUI_CONTROL_H_