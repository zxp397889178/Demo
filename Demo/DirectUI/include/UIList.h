/*********************************************************************
*            Copyright (C) 2010, 网龙天晴程序应用软件部
**********************************************************************
*   Date        Name        Description
*   10/20/2010  hanzp		Create.
*   05/05/2011  hanzp       Add Class CListCtrlUI
*********************************************************************/
#pragma once


//{{
#define UITABLE_ROOT                ((CTableItemUI*)(ULONG_PTR)-0x10000)
#define UITABLE_FIRST               ((CTableItemUI*)(ULONG_PTR)-0x0FFFF)
#define UITABLE_LAST                ((CTableItemUI*)(ULONG_PTR)-0x0FFFE)
//}}


//////////////////////////////////////////////////////////////////////////
//展开至叶子节点
#define UINODE_EXPAND_TOLEAF		0x00000001
//追溯至根节点展开
#define UINODE_EXPAND_TOROOT		0x00000002
//更新
#define UINODE_EXPAND_UPDATE		0x00000004

/*!
    \brief    分组节点虚接口
*****************************************/
class DUI_API INode
{
	//{{
	friend class INodeOwner;
protected:
	//************************************
	// @brief:    被父节点显示或隐藏本节点
	// @note:     
	// @param: 	  bool bShow 
	// @return:   void 
	//************************************
	virtual void node_show(bool bShow) = 0;

	//************************************
	// @brief:    是否显示
	// @note:     
	// @return:   bool 
	//************************************
	virtual bool is_node_show() = 0;

	//************************************
	// @brief:    节点在父容器中的索引
	// @note:     
	// @return:   int 
	//************************************
	virtual int  get_node_index() = 0;

	virtual void set_node_index(int nIndex) = 0;

	//************************************
	// @brief:    展开节点
	// @note:     
	// @param: 	  bool bExpand 
	// @param: 	  UINT uFlags 
	// @return:   bool 
	//************************************
	bool	node_expand(bool bExpand, UINT uFlags = UINODE_EXPAND_TOROOT | UINODE_EXPAND_UPDATE);

	//************************************
	// @brief:    节点是否展开
	// @note:     
	// @return:   bool 
	//************************************
	bool	is_node_expand(UINT uFlags = UINODE_EXPAND_TOROOT);

	//************************************
	// @brief:    是否组节点
	// @note:     
	// @return:   bool 
	//************************************
	bool	is_node_group();

	//************************************
	// @brief:    获取子节点
	// @note:     
	// @param: 	  int nIndex 
	// @return:   INode* 
	//************************************
	INode*	get_child_node(int nIndex);

	//************************************
	// @brief:    获取父节点
	// @note:     
	// @return:   INode* 
	//************************************
	INode*	get_parent_node();

	//************************************
	// @brief:    获取最后一个子节点
	// @note:     若没有子节点，返回自身
	// @return:   INode* 
	//************************************
	INode*	get_last_child_node();

	//************************************
	// @brief:    子节点个数
	// @note:     
	// @return:   int 
	//************************************
	int		num_child_node();

	INode* get_nextsibling_node(INode* pNode);

	INode* get_prevsibling_node(INode* pNode);
	//////////////////////////////////////////////////////////////////////////
	//以下函数不建议调用
	void	remove_child_node(INode* child);
	void	remove_all_node();
	void	add_child_node(INode* child, int nIndex);
	void	set_child_node(INode* child, int nIndex);
	void	node_group(bool bGroup);
	INodeOwner* get_node_owner();
	void	set_node_owner(INodeOwner* pOwner);
	CStdPtrArray* GetNodeArray();
protected:
	INode(bool bGroup);
	virtual ~INode();
	CStdPtrArray*	m_pArrChildNode;
	INode*			m_pParentNode;
	INodeOwner*		m_pNodeOwner;
	bool			m_bNodeExpand;
	bool			m_bNodeGroup;
	//}}
};

/*!
    \brief    分组容器虚接口
*****************************************/
class DUI_API INodeOwner
{
public:
	//************************************
	// @brief:    刷新界面
	// @note:     
	// @return:   void 
	//************************************
	virtual void NeedUpdate() = 0;

	//************************************
	// @brief:    节点展开前回调
	// @note:     
	// @param: 	  INode * pNode 
	// @param: 	  bool bExpand 
	// @return:   void 
	//************************************
	virtual void expanding(INode* pNode, bool bExpand) = 0;

	//************************************
	// @brief:    节点展开后回调
	// @note:     
	// @param: 	  INode * pNode 
	// @param: 	  bool bExpand 
	// @return:   void 
	//************************************
	virtual void expandend(INode* pNode, bool bExpand) = 0;

	//************************************
	// @brief:    获取节点
	// @note:     
	// @param: 	  int nIndex 
	// @return:   INode* 
	//************************************
	virtual INode* get_node(int nIndex) = 0;

	//************************************
	// @brief:    节点是否显示的判断
	// @note:     默认为父节点可见并展开
	// @param: 	  INode * pNode 
	// @return:   bool 
	//************************************
	virtual bool node_show_ploy(INode* pNode);

	//************************************
	// @brief:    获取根节点
	// @note:     
	// @return:   INode* 
	//************************************
	INode* get_root();

	//{{
protected:
	INodeOwner();
	void set_root(INode* pRoot);
	virtual void set_node(int nIndex, INode* pNode) = 0;

	INode* m_pNodeRoot;
	//}}
};

//************************************
// @brief:    列表比较函数
// @note:     对于第n列，控件A和控件B比较。返回值为负值时表示控件A优先值高；为0时表示两个控件权值一样。若要将某控件A置顶，升序时返回负值，降序时返回正值。
// @param: 	  CListItemUI* pControlA
// @param: 	  CListItemUI* pControlB
// @param: 	  int nCurCol
// @param: 	  bool bAsc
// @return:   int
//************************************
//{{
typedef int (__stdcall * LISTSORTPROC)(CListItemUI* pControlA, CListItemUI* pControlB, int nCurCol, bool bAsc);
//}}
//////////////////////////////////////////////////////////////////////////
/*!
    \brief    列表控件
*****************************************/
class DUI_API CListUI : public CContainerUI, public INotifyUI
{
	//{{
	UI_OBJECT_DECLARE(CListUI, _T("List"))
public:
	CListUI();
	virtual ~CListUI();
	//}}

	virtual bool SelectItem(CControlUI* pControl);

	CListItemUI* GetCurSel();
	//************************************
	// @brief:    多选
	// @note:     会更改GetCurSel
	// @param: 	  CControlUI * pControl 
	// @return:   bool 
	//************************************
	bool SelectItemAdd(CControlUI* pControl);

	//************************************
	// @brief:    多选，选中GetCurSel和pControl之间的控件
	// @note:     不更改GetCurSel
	// @param: 	  CControlUI * pControl 
	// @return:   bool 
	//************************************
	bool SelectItemEnd(CControlUI* pControl);

	//************************************
	// @brief:    全选
	// @note:     不更改GetCurSel
	// @param: 	  bool bSelect 
	// @return:   bool 
	//************************************
	bool SelectItemAll(bool bSelect = true);

	//************************************
	// @brief:    使控件处于可见位置
	// @note:     
	// @param: 	  CControlUI * pControl 
	// @return:   void 
	//************************************
	virtual void EnsureVisible(CControlUI* pControl);

	virtual void EnsureVisibleFirst( CControlUI* pControl );

	virtual int EnsureVisibleCenter( CControlUI* pControl,int& nBegin, int &nEnd, bool bRedraw = true );

	//************************************
	// @brief:    依据坐标获取控件
	// @note:     只对可见部分
	// @param: 	  POINT pt 窗口内坐标
	// @return:   CControlUI* 
	//************************************
	CControlUI* ItemFromPos(POINT& pt);

	//************************************
	// @brief:    获取当前高亮控件
	// @note:     
	// @return:   CControlUI* 
	//************************************
	CControlUI* GetHotItem();

	//************************************
	// @brief:    获取下一个控件
	// @note:     
	// @param: 	  CControlUI * pControl 
	// @param: 	  UINT uFlags 
	// @return:   CControlUI* 
	//************************************
	CControlUI* GetNextItem(CControlUI* pControl, UINT uFlags = UIITEMFLAG_VISIBLE | UIITEMFLAG_ENABLED);

	//************************************
	// @brief:    获取上一个控件
	// @note:     
	// @param: 	  CControlUI * pControl 
	// @param: 	  UINT uFlags 
	// @return:   CControlUI* 
	//************************************
	CControlUI* GetPrevItem(CControlUI* pControl, UINT uFlags = UIITEMFLAG_VISIBLE | UIITEMFLAG_ENABLED);

	bool GetItemRect(CControlUI* pControl, RECT& rcChild);

	bool IsEnableMultiSelect();
	bool IsEnableItemSelect();
	bool IsEnableItemBkg();
	bool IsEnableItemOver();
	int CheckWidth() const;
	//////////////////////////////////////////////////////////////////////////
	//列操作
	void	SetItemText(int nIndex, int nCol, LPCTSTR lpszText);	//设置单元格文字
	LPCTSTR GetItemText(int nIndex, int nCol);						//获取单元格文字，空为NULL
	bool	SetHeaderText(int nCol, LPCTSTR lpszText);				//设置列标题文字
	int		GetSortCol();											//获取当前排序列
	void	SetSortCol(int nCol);									//设置当前排序列
	int		GetCols();
	bool	InsertCol(CListHeaderItemUI* pItem, int nIndex = -1);	//添加列
	CListHeaderUI*		GetHeader();
	CListHeaderItemUI*	GetCol(int nIndex);
	bool EnableHeaderItemSort();

	//************************************
	// @brief:    排序前须配置比较函数SetSortCallBack
	// @note:     
	// @param: 	  int nColumn 排序列，-1表示根据SetSortCol的值
	// @param: 	  int nAsc -1根据当前表头的顺序(若表头无排序，默认升序)，0降序，1升序
	// @param: 	  int nBegin 
	// @param: 	  int nSize 
	// @return:   bool 
	//************************************
	virtual bool Sort(int nColumn = -1, int nAsc = -1, int nBegin = 0, int nSize = -1);
	//{{
	void SetSortCallBack(LISTSORTPROC pCompare);					//设置排序比较函数
	//}}
	//////////////////////////////////////////////////////////////////////////
	//Check操作

	bool EnableCheck() const { return m_bEnableCheck; }
	void EnableCheck(bool val) { m_bEnableCheck = val; }

	CCheckBoxUI* GetCheck();
	/*!
	   \brief    检查表头check
	   \return   void 
	 ************************************/
	virtual void CheckLegal();

	//************************************
	// @brief:    设置check
	// @note:     
	// @param: 	  bool bCheck 
	// @param: 	  int nIndex 
	// @param: 	  UINT uFlag UIITEMFLAG_VISIBLE/UIITEMFLAG_ENABLED/UILIST_CHECK根据需求选择一个,使用UIITEMFLAG_CHECKED，可以判断是否通知UIITEMFLAG_NOTIFY
	// @return:   void 
	//************************************
	virtual void CheckItem(bool bCheck, int nIndex = -1, UINT uFlag = UIITEMFLAG_CHECKED | UIITEMFLAG_ALL | UIITEMFLAG_NOTIFY);//-1为全选
	
	//************************************
	// @brief:    检查check
	// @note:     
	// @param: 	  int nIndex 
	// @param: 	  UINT uFlag UIITEMFLAG_VISIBLE/UIITEMFLAG_ENABLED/UILIST_CHECK任意一个
	// @return:   bool 
	//************************************
	bool IsCheckItem(int nIndex, UINT uFlag = UIITEMFLAG_CHECKED);

	//////////////////////////////////////////////////////////////////////////
	//继承函数
	virtual bool Add(CControlUI* pControl);
	virtual bool Remove(CControlUI* pControl);
	virtual void RemoveAll();
	virtual bool Insert(CControlUI* pControl, CControlUI* pAfter);
	virtual bool Event(TEventUI& event);
	virtual bool ChildEvent(TEventUI& event);
	virtual void Init();
	virtual bool Resize();
	virtual void SetRect(RECT& rectRegion);
	virtual SIZE OnChildSize(RECT& rcWinow, RECT& rcClient);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual void OnLanguageChange();

	//////////////////////////////////////////////////////////////////////////
	//表头Check的操作
	//{{
	UIBEGIN_MSG_MAP
		UI_EVENT_HANDLER(UINOTIFY_CLICK, OnBtnClick)
	UIEND_MSG_MAP
	//}}
	//{{
protected:
	bool OnBtnClick(TNotifyUI* pNotify);
	bool m_bEnableCheck;//是否有check
private:
	CControlUI* m_pHighlight;
	CListHeaderUI* m_pHeader;
	CCheckBoxUI* m_pCheck;
	int m_nCurSortCol;
	int m_nHeaderHeight;//表头高度
	int m_nCheckWidth;
	RECT m_rcOldRectInset;
	bool m_bHeaderVisible;//表头是否显示
	bool m_bEnableHeaderItemSort;//表头是否可以点击排序
	bool m_bVisibleHeaderCheck;//是否显示表头的check
	LISTSORTPROC m_pListSortCompareProc;//nCol 当前选择的列数，-1为没有选择列
	
public:
protected:

	//************************************
	// @brief:    以pImgState为背景图
	// @note:     依据pControl的索引单双选择绘制
	// @param: 	  ImageObj * pImgState 
	// @param: 	  CControlUI * pControl 
	// @param: 	  IRenderDC * pRenderDC 
	// @param: 	  RECT & rcPaint 
	// @return:   void 
	//************************************
	virtual void RenderItemBkg(ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint);

	/*!
	\brief    绘制节点状态图
	\note     
	\param    ImageObj * pImgState 
	\param    CControlUI * pControl 
	\param    IRenderDC * pRenderDC 
	\param    RECT & rcPaint 
	\return   void 
	************************************/
	virtual void RenderItemState(ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint);

	ImageObj* GetItemBkgImage();

	/*!
	\brief    获取状态图片
	\note     
	\return   ImageObj* 
	************************************/
	ImageObj* GetStateImage();
protected:
	bool m_bMultiSelect;
	bool m_bEnableItemSelect;		//是否让列表项有选择效果
	bool m_bEnableItemBkg;		//是否让列表项有背景
	bool m_bEnableItemOver;		//是否让列表项有划过背景
	bool m_bEnableItemOverOnly; //划过就可以
	int m_nFirstVisible;		//第一个可见项的索引
	int m_nLastVisible;			//可见项之后第一个不可见项的索引
	bool m_bAreaSelecting;
	RECT m_rcAreaSelection;
	ImageObj* m_pBoundImage;

private:
	bool m_bLockChildsRect;
	//}}
};


//////////////////////////////////////////////////////////////////////////
//************************************
// @brief:    表格比较函数
// @note:     对于第n列，parent组节点下控件A和控件B比较。返回值为负值时表示控件A优先值高；为0时表示两个控件权值一样。若要将某控件A置顶，升序时返回负值，降序时返回正值。
// @param: 	  CTableItemUI* pControlA
// @param: 	  CTableItemUI* pControlB
// @param: 	  CTableItemUI* pParent
// @param: 	  int nCurCol
// @param: 	  bool bAsc
// @return:   int
//************************************
//{{
typedef int (__stdcall * TABLESORTPROC)(CTableItemUI* pControlA, CTableItemUI* pControlB, CTableItemUI* pParent, int nCurCol, bool bAsc);
//}}
//************************************
// @brief:    组结构节点是否显示
// @note:     
// @param: 	  CTableItemUI* pItem
// @param: 	  CTableUI* pTable
// @return:   bool
//************************************
//{{
typedef bool (__stdcall* NODESHOWPLOYFUNC)(CTableItemUI* pItem, CTableUI* pTable);
//}}

/*!
    \brief    分组控件
*****************************************/
class DUI_API CTableUI : public CListUI, public INodeOwner
{
	//{{
	UI_OBJECT_DECLARE(CTableUI, _T("Table"))
public:
	CTableUI();
	virtual ~CTableUI();
	//}}

	CTableItemUI* GetRoot();

	//************************************
	// @brief:    用于添加TableItem
	// @note:     
	// @param: 	  CTableItemUI * pControl 
	// @param: 	  CTableItemUI * pParent 
	// @param: 	  CTableItemUI * pInsertAfter 
	// @return:   CTableItemUI* 
	//************************************
	//{{
	CTableItemUI* InsertItem(CTableItemUI* pControl, 
		CTableItemUI* pParent = UITABLE_ROOT, CTableItemUI* pInsertAfter = UITABLE_LAST);
	//}}
	virtual bool Add(CControlUI* pControl);
	virtual bool Event(TEventUI& event);
	virtual void RemoveAll();

	virtual void EnsureVisible(CControlUI* pControl);

	virtual void EnsureVisibleFirst( CControlUI* pControl );

	virtual int EnsureVisibleCenter( CControlUI* pControl,int& nBegin, int &nEnd, bool bRedraw = true );


	virtual void CheckItem(bool bCheck, int nIndex = -1, UINT uFlag = UIITEMFLAG_CHECKED | UIITEMFLAG_ALL | UIITEMFLAG_NOTIFY);//-1为全选

	//************************************
	// @brief:    排序
	// @note:     nBegin和nSize应包含整个组的范围
	// @param: 	  int nColumn 
	// @param: 	  int nAsc 
	// @param: 	  int nBegin
	// @param: 	  int nSize 
	// @return:   bool 
	//************************************
	virtual bool Sort(int nColumn = -1, int nAsc = -1, int nBegin = 0, int nSize = -1);
	//{{

	void	SetSortCallBack(TABLESORTPROC pCompare);					//设置排序比较函数

	NODESHOWPLOYFUNC GetShowPloyFunc();
	void SetShowPloyFunc(NODESHOWPLOYFUNC pFunc);


protected:
	virtual bool node_show_ploy(INode* pNode);
	NODESHOWPLOYFUNC m_pShowPloyFunc;

	virtual void expanding(INode* pNode, bool bExpand);
	virtual void expandend(INode* pNode, bool bExpand);
	virtual void NeedUpdate();
	virtual INode* get_node(int nIndex);
	virtual void set_node(int nIndex, INode* pNode);
	void SetItemVisible(CTableItemUI* pControl, bool bVisible);
private:
	TABLESORTPROC m_pTableSortCompareProc;
	//}}

};

/*!
    \brief    表头容器控件
*****************************************/
class DUI_API CListHeaderUI : public CLayoutUI
{
	UI_OBJECT_DECLARE(CListHeaderUI, _T("ListHeader"))
public:
	CListHeaderUI();
	virtual ~CListHeaderUI();

public:
	SIZE GetChildSize();

protected:
	virtual SIZE OnChildSize(RECT& rcWinow, RECT& rcClient);

private:
	SIZE m_szChildSize;
//}}
};

/*!
    \brief    表头控件
*****************************************/
class DUI_API CListHeaderItemUI : public CControlUI
{
	//{{
	friend class CListHeaderUI;
	friend class CTableItemUI;
	friend class CListItemUI;
	friend class CTableUI;
	friend class CListUI;

	UI_OBJECT_DECLARE(CListHeaderItemUI, _T("ListHeaderItem"))
public:
	CListHeaderItemUI();
	virtual ~CListHeaderItemUI();

public:
	bool IsAscending();
	UINT GetColAlign();
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

//{{
protected:
	int GetPercentWidth();
	RECT GetArrowRect();
	int GetArrowRight(POINT& pt);
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);

private:
	bool	m_bDragStatus;
	bool	m_bDragable;
	bool	m_bEnableSort;
	bool	m_bAscending;
	POINT   m_ptLastMouse;
	UINT	m_uColAlignStyle;
	int		m_nPercentWidth;
//}}
};

//////////////////////////////////////////////////////////////////////////
class CListItemDataImpl;
/*!
    \brief    列表节点控件
*****************************************/
class DUI_API CListItemUI : public CControlUI
{
	//{{
	friend class CListUI;
	UI_OBJECT_DECLARE(CListItemUI, _T("ListItem"))
public:
	CListItemUI();
	virtual ~CListItemUI();
	//}}
	virtual bool Activate();
	CCheckBoxUI* GetCheck();
	bool EnableItemState() const { return m_bEnableItemState; }
	void EnableItemState(bool val) { m_bEnableItemState = val; }

	//////////////////////////////////////////////////////////////////////////
	//纯文字
	//************************************
	// @brief:    单元格文字
	// @note:     
	// @param: 	  int nCol 
	// @param: 	  LPCTSTR lpszText 
	// @return:   void 
	//************************************
	void SetSubItemText(int nCol, LPCTSTR lpszText);

	//************************************
	// @brief:    单元格文字
	// @note:     
	// @param: 	  int nCol 
	// @return:   LPCTSTR 
	//************************************
	LPCTSTR GetSubItemText(int nCol);

	//************************************
	// @brief:    单元格数据
	// @note:     
	// @param: 	  int nCol 
	// @param: 	  unsigned long long uUserData 
	// @return:   void 
	//************************************
	void SetSubItemUserdata(int nCol, unsigned long long uUserData);

	//************************************
	// @brief:    单元格数据
	// @note:     
	// @param: 	  int nCol 
	// @return:   unsigned long long 
	//************************************
	unsigned long long GetSubItemUserdata(int nCol);

	//////////////////////////////////////////////////////////////////////////
	//复合控件
public:
	//************************************
	// @brief:    需要重新计算复合控件的位置
	// @note:     
	// @return:   void 
	//************************************
	void	ReCalcRect();

	//////////////////////////////////////////////////////////////////////////
	//继承函数
	int		GetCount();
	bool	Add(CControlUI* pControl);
	bool	Remove(CControlUI* pControl);
	virtual void SetRect(RECT& rectRegion);
	CControlUI*		GetItem(int iIndex);
	CControlUI*		GetItem(LPCTSTR lpszId);
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void	SetManager(CWindowUI* pManager, CControlUI* pParent);
	virtual void	SetVisible( bool bShow );
	virtual void	SetInternVisible( bool bVisible );
//{{
	virtual RECT	GetClientRect();

protected:
	void SetCheck(CCheckBoxUI* pCheck);

	//************************************
	// @brief:    计算复合控件的位置
	// @note:     
	// @return:   void 
	//************************************
	virtual void CalcRect();

	//////////////////////////////////////////////////////////////////////////
	//继承函数
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual bool ChildEvent(TEventUI& event);
	virtual void SetINotifyUI(INotifyUI* pINotify);
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	bool EnableItems() const { return m_bEnableItems; }
	void EnableItems(bool val) { m_bEnableItems = val; }
	bool EnableColsText() const { return m_bEnableColsText; }
	void EnableColsText(bool val) { m_bEnableColsText = val; }
	bool EnableSuper() const { return m_bEnableSuper; }
	void EnableSuper(bool val) { m_bEnableSuper = val; }
	CCheckBoxUI*	m_pCheck;
private:
	CListItemDataImpl* m_pDataImpl;

	CStdPtrArray* m_pItems;
	bool m_bDrawSuper;		//绘制Item自身样式
	bool m_bEnableItemState;//开启节点高亮
	bool m_bEnableItems;	//开启子控件绘制
	bool m_bEnableColsText;	//开启列文字绘制
	bool m_bEnableSuper;	//开启自身样式绘制
	bool m_bCalc;
	int m_nMaskClick;
	bool m_bMaskCols;		//不依据列的位置摆放控件
//}}
};

/*!
    \brief    分组节点控件
*****************************************/
class DUI_API CTableItemUI : public CListItemUI, public INode
{
	//{{
	friend class CTableUI;
	UI_OBJECT_DECLARE(CTableItemUI, _T("TableItem"))
public:
	CTableItemUI(bool bGroup = false);
//}}
	virtual bool Add(CControlUI* pControl);
	virtual bool Activate();
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	//************************************
	// @brief:    展开节点
	// @note:     
	// @param: 	  bool bExpand 
	// @param: 	  UINT uFlags UINODE_EXPAND_TOLEAF/UINODE_EXPAND_TOROOT
	// @return:   bool 是否有刷新
	//************************************
	bool Expand(bool bExpand, UINT uFlags = UINODE_EXPAND_TOROOT | UINODE_EXPAND_UPDATE);
	bool IsExpand(UINT uFlags = UINODE_EXPAND_TOROOT);
	bool IsGroup();
	int  num_child();
	CTableItemUI* get_child(int i);
	CTableItemUI* get_last_child();
	CTableItemUI* get_parent();
	CTableItemUI* get_nextsibling(CTableItemUI* pNode);
	CTableItemUI* get_prevsibling(CTableItemUI* pNode);
	CTableItemUI* find_child(UINT uData);
	int find_child(CTableItemUI* node);
	void set_child(CTableItemUI* node, int nIndex);
	virtual void set_node_index(int nIndex);
//{{
protected:
	//INode继承
	virtual void node_show(bool bShow);
	virtual bool is_node_show();
	virtual int  get_node_index();
	virtual void CheckLegal();

	bool m_bEnableExpand;
	//}}
};

/*!
    \brief    列表节点容器控件
*****************************************/
class DUI_API CListContainerItemUI :public CListItemUI
{
	UI_OBJECT_DECLARE(CListContainerItemUI, _T("ListContainerItem"))
public:
	//{{
	CListContainerItemUI();
	virtual ~CListContainerItemUI();
	//}}
};


