/*********************************************************************
*            Copyright (C) 2010, �����������Ӧ�������
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
//չ����Ҷ�ӽڵ�
#define UINODE_EXPAND_TOLEAF		0x00000001
//׷�������ڵ�չ��
#define UINODE_EXPAND_TOROOT		0x00000002
//����
#define UINODE_EXPAND_UPDATE		0x00000004

/*!
    \brief    ����ڵ���ӿ�
*****************************************/
class DUI_API INode
{
	//{{
	friend class INodeOwner;
protected:
	//************************************
	// @brief:    �����ڵ���ʾ�����ر��ڵ�
	// @note:     
	// @param: 	  bool bShow 
	// @return:   void 
	//************************************
	virtual void node_show(bool bShow) = 0;

	//************************************
	// @brief:    �Ƿ���ʾ
	// @note:     
	// @return:   bool 
	//************************************
	virtual bool is_node_show() = 0;

	//************************************
	// @brief:    �ڵ��ڸ������е�����
	// @note:     
	// @return:   int 
	//************************************
	virtual int  get_node_index() = 0;

	virtual void set_node_index(int nIndex) = 0;

	//************************************
	// @brief:    չ���ڵ�
	// @note:     
	// @param: 	  bool bExpand 
	// @param: 	  UINT uFlags 
	// @return:   bool 
	//************************************
	bool	node_expand(bool bExpand, UINT uFlags = UINODE_EXPAND_TOROOT | UINODE_EXPAND_UPDATE);

	//************************************
	// @brief:    �ڵ��Ƿ�չ��
	// @note:     
	// @return:   bool 
	//************************************
	bool	is_node_expand(UINT uFlags = UINODE_EXPAND_TOROOT);

	//************************************
	// @brief:    �Ƿ���ڵ�
	// @note:     
	// @return:   bool 
	//************************************
	bool	is_node_group();

	//************************************
	// @brief:    ��ȡ�ӽڵ�
	// @note:     
	// @param: 	  int nIndex 
	// @return:   INode* 
	//************************************
	INode*	get_child_node(int nIndex);

	//************************************
	// @brief:    ��ȡ���ڵ�
	// @note:     
	// @return:   INode* 
	//************************************
	INode*	get_parent_node();

	//************************************
	// @brief:    ��ȡ���һ���ӽڵ�
	// @note:     ��û���ӽڵ㣬��������
	// @return:   INode* 
	//************************************
	INode*	get_last_child_node();

	//************************************
	// @brief:    �ӽڵ����
	// @note:     
	// @return:   int 
	//************************************
	int		num_child_node();

	INode* get_nextsibling_node(INode* pNode);

	INode* get_prevsibling_node(INode* pNode);
	//////////////////////////////////////////////////////////////////////////
	//���º������������
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
    \brief    ����������ӿ�
*****************************************/
class DUI_API INodeOwner
{
public:
	//************************************
	// @brief:    ˢ�½���
	// @note:     
	// @return:   void 
	//************************************
	virtual void NeedUpdate() = 0;

	//************************************
	// @brief:    �ڵ�չ��ǰ�ص�
	// @note:     
	// @param: 	  INode * pNode 
	// @param: 	  bool bExpand 
	// @return:   void 
	//************************************
	virtual void expanding(INode* pNode, bool bExpand) = 0;

	//************************************
	// @brief:    �ڵ�չ����ص�
	// @note:     
	// @param: 	  INode * pNode 
	// @param: 	  bool bExpand 
	// @return:   void 
	//************************************
	virtual void expandend(INode* pNode, bool bExpand) = 0;

	//************************************
	// @brief:    ��ȡ�ڵ�
	// @note:     
	// @param: 	  int nIndex 
	// @return:   INode* 
	//************************************
	virtual INode* get_node(int nIndex) = 0;

	//************************************
	// @brief:    �ڵ��Ƿ���ʾ���ж�
	// @note:     Ĭ��Ϊ���ڵ�ɼ���չ��
	// @param: 	  INode * pNode 
	// @return:   bool 
	//************************************
	virtual bool node_show_ploy(INode* pNode);

	//************************************
	// @brief:    ��ȡ���ڵ�
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
// @brief:    �б�ȽϺ���
// @note:     ���ڵ�n�У��ؼ�A�Ϳؼ�B�Ƚϡ�����ֵΪ��ֵʱ��ʾ�ؼ�A����ֵ�ߣ�Ϊ0ʱ��ʾ�����ؼ�Ȩֵһ������Ҫ��ĳ�ؼ�A�ö�������ʱ���ظ�ֵ������ʱ������ֵ��
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
    \brief    �б�ؼ�
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
	// @brief:    ��ѡ
	// @note:     �����GetCurSel
	// @param: 	  CControlUI * pControl 
	// @return:   bool 
	//************************************
	bool SelectItemAdd(CControlUI* pControl);

	//************************************
	// @brief:    ��ѡ��ѡ��GetCurSel��pControl֮��Ŀؼ�
	// @note:     ������GetCurSel
	// @param: 	  CControlUI * pControl 
	// @return:   bool 
	//************************************
	bool SelectItemEnd(CControlUI* pControl);

	//************************************
	// @brief:    ȫѡ
	// @note:     ������GetCurSel
	// @param: 	  bool bSelect 
	// @return:   bool 
	//************************************
	bool SelectItemAll(bool bSelect = true);

	//************************************
	// @brief:    ʹ�ؼ����ڿɼ�λ��
	// @note:     
	// @param: 	  CControlUI * pControl 
	// @return:   void 
	//************************************
	virtual void EnsureVisible(CControlUI* pControl);

	virtual void EnsureVisibleFirst( CControlUI* pControl );

	virtual int EnsureVisibleCenter( CControlUI* pControl,int& nBegin, int &nEnd, bool bRedraw = true );

	//************************************
	// @brief:    ���������ȡ�ؼ�
	// @note:     ֻ�Կɼ�����
	// @param: 	  POINT pt ����������
	// @return:   CControlUI* 
	//************************************
	CControlUI* ItemFromPos(POINT& pt);

	//************************************
	// @brief:    ��ȡ��ǰ�����ؼ�
	// @note:     
	// @return:   CControlUI* 
	//************************************
	CControlUI* GetHotItem();

	//************************************
	// @brief:    ��ȡ��һ���ؼ�
	// @note:     
	// @param: 	  CControlUI * pControl 
	// @param: 	  UINT uFlags 
	// @return:   CControlUI* 
	//************************************
	CControlUI* GetNextItem(CControlUI* pControl, UINT uFlags = UIITEMFLAG_VISIBLE | UIITEMFLAG_ENABLED);

	//************************************
	// @brief:    ��ȡ��һ���ؼ�
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
	//�в���
	void	SetItemText(int nIndex, int nCol, LPCTSTR lpszText);	//���õ�Ԫ������
	LPCTSTR GetItemText(int nIndex, int nCol);						//��ȡ��Ԫ�����֣���ΪNULL
	bool	SetHeaderText(int nCol, LPCTSTR lpszText);				//�����б�������
	int		GetSortCol();											//��ȡ��ǰ������
	void	SetSortCol(int nCol);									//���õ�ǰ������
	int		GetCols();
	bool	InsertCol(CListHeaderItemUI* pItem, int nIndex = -1);	//�����
	CListHeaderUI*		GetHeader();
	CListHeaderItemUI*	GetCol(int nIndex);
	bool EnableHeaderItemSort();

	//************************************
	// @brief:    ����ǰ�����ñȽϺ���SetSortCallBack
	// @note:     
	// @param: 	  int nColumn �����У�-1��ʾ����SetSortCol��ֵ
	// @param: 	  int nAsc -1���ݵ�ǰ��ͷ��˳��(����ͷ������Ĭ������)��0����1����
	// @param: 	  int nBegin 
	// @param: 	  int nSize 
	// @return:   bool 
	//************************************
	virtual bool Sort(int nColumn = -1, int nAsc = -1, int nBegin = 0, int nSize = -1);
	//{{
	void SetSortCallBack(LISTSORTPROC pCompare);					//��������ȽϺ���
	//}}
	//////////////////////////////////////////////////////////////////////////
	//Check����

	bool EnableCheck() const { return m_bEnableCheck; }
	void EnableCheck(bool val) { m_bEnableCheck = val; }

	CCheckBoxUI* GetCheck();
	/*!
	   \brief    ����ͷcheck
	   \return   void 
	 ************************************/
	virtual void CheckLegal();

	//************************************
	// @brief:    ����check
	// @note:     
	// @param: 	  bool bCheck 
	// @param: 	  int nIndex 
	// @param: 	  UINT uFlag UIITEMFLAG_VISIBLE/UIITEMFLAG_ENABLED/UILIST_CHECK��������ѡ��һ��,ʹ��UIITEMFLAG_CHECKED�������ж��Ƿ�֪ͨUIITEMFLAG_NOTIFY
	// @return:   void 
	//************************************
	virtual void CheckItem(bool bCheck, int nIndex = -1, UINT uFlag = UIITEMFLAG_CHECKED | UIITEMFLAG_ALL | UIITEMFLAG_NOTIFY);//-1Ϊȫѡ
	
	//************************************
	// @brief:    ���check
	// @note:     
	// @param: 	  int nIndex 
	// @param: 	  UINT uFlag UIITEMFLAG_VISIBLE/UIITEMFLAG_ENABLED/UILIST_CHECK����һ��
	// @return:   bool 
	//************************************
	bool IsCheckItem(int nIndex, UINT uFlag = UIITEMFLAG_CHECKED);

	//////////////////////////////////////////////////////////////////////////
	//�̳к���
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
	//��ͷCheck�Ĳ���
	//{{
	UIBEGIN_MSG_MAP
		UI_EVENT_HANDLER(UINOTIFY_CLICK, OnBtnClick)
	UIEND_MSG_MAP
	//}}
	//{{
protected:
	bool OnBtnClick(TNotifyUI* pNotify);
	bool m_bEnableCheck;//�Ƿ���check
private:
	CControlUI* m_pHighlight;
	CListHeaderUI* m_pHeader;
	CCheckBoxUI* m_pCheck;
	int m_nCurSortCol;
	int m_nHeaderHeight;//��ͷ�߶�
	int m_nCheckWidth;
	RECT m_rcOldRectInset;
	bool m_bHeaderVisible;//��ͷ�Ƿ���ʾ
	bool m_bEnableHeaderItemSort;//��ͷ�Ƿ���Ե������
	bool m_bVisibleHeaderCheck;//�Ƿ���ʾ��ͷ��check
	LISTSORTPROC m_pListSortCompareProc;//nCol ��ǰѡ���������-1Ϊû��ѡ����
	
public:
protected:

	//************************************
	// @brief:    ��pImgStateΪ����ͼ
	// @note:     ����pControl��������˫ѡ�����
	// @param: 	  ImageObj * pImgState 
	// @param: 	  CControlUI * pControl 
	// @param: 	  IRenderDC * pRenderDC 
	// @param: 	  RECT & rcPaint 
	// @return:   void 
	//************************************
	virtual void RenderItemBkg(ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint);

	/*!
	\brief    ���ƽڵ�״̬ͼ
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
	\brief    ��ȡ״̬ͼƬ
	\note     
	\return   ImageObj* 
	************************************/
	ImageObj* GetStateImage();
protected:
	bool m_bMultiSelect;
	bool m_bEnableItemSelect;		//�Ƿ����б�����ѡ��Ч��
	bool m_bEnableItemBkg;		//�Ƿ����б����б���
	bool m_bEnableItemOver;		//�Ƿ����б����л�������
	bool m_bEnableItemOverOnly; //�����Ϳ���
	int m_nFirstVisible;		//��һ���ɼ��������
	int m_nLastVisible;			//�ɼ���֮���һ�����ɼ��������
	bool m_bAreaSelecting;
	RECT m_rcAreaSelection;
	ImageObj* m_pBoundImage;

private:
	bool m_bLockChildsRect;
	//}}
};


//////////////////////////////////////////////////////////////////////////
//************************************
// @brief:    ���ȽϺ���
// @note:     ���ڵ�n�У�parent��ڵ��¿ؼ�A�Ϳؼ�B�Ƚϡ�����ֵΪ��ֵʱ��ʾ�ؼ�A����ֵ�ߣ�Ϊ0ʱ��ʾ�����ؼ�Ȩֵһ������Ҫ��ĳ�ؼ�A�ö�������ʱ���ظ�ֵ������ʱ������ֵ��
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
// @brief:    ��ṹ�ڵ��Ƿ���ʾ
// @note:     
// @param: 	  CTableItemUI* pItem
// @param: 	  CTableUI* pTable
// @return:   bool
//************************************
//{{
typedef bool (__stdcall* NODESHOWPLOYFUNC)(CTableItemUI* pItem, CTableUI* pTable);
//}}

/*!
    \brief    ����ؼ�
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
	// @brief:    �������TableItem
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


	virtual void CheckItem(bool bCheck, int nIndex = -1, UINT uFlag = UIITEMFLAG_CHECKED | UIITEMFLAG_ALL | UIITEMFLAG_NOTIFY);//-1Ϊȫѡ

	//************************************
	// @brief:    ����
	// @note:     nBegin��nSizeӦ����������ķ�Χ
	// @param: 	  int nColumn 
	// @param: 	  int nAsc 
	// @param: 	  int nBegin
	// @param: 	  int nSize 
	// @return:   bool 
	//************************************
	virtual bool Sort(int nColumn = -1, int nAsc = -1, int nBegin = 0, int nSize = -1);
	//{{

	void	SetSortCallBack(TABLESORTPROC pCompare);					//��������ȽϺ���

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
    \brief    ��ͷ�����ؼ�
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
    \brief    ��ͷ�ؼ�
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
    \brief    �б�ڵ�ؼ�
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
	//������
	//************************************
	// @brief:    ��Ԫ������
	// @note:     
	// @param: 	  int nCol 
	// @param: 	  LPCTSTR lpszText 
	// @return:   void 
	//************************************
	void SetSubItemText(int nCol, LPCTSTR lpszText);

	//************************************
	// @brief:    ��Ԫ������
	// @note:     
	// @param: 	  int nCol 
	// @return:   LPCTSTR 
	//************************************
	LPCTSTR GetSubItemText(int nCol);

	//************************************
	// @brief:    ��Ԫ������
	// @note:     
	// @param: 	  int nCol 
	// @param: 	  unsigned long long uUserData 
	// @return:   void 
	//************************************
	void SetSubItemUserdata(int nCol, unsigned long long uUserData);

	//************************************
	// @brief:    ��Ԫ������
	// @note:     
	// @param: 	  int nCol 
	// @return:   unsigned long long 
	//************************************
	unsigned long long GetSubItemUserdata(int nCol);

	//////////////////////////////////////////////////////////////////////////
	//���Ͽؼ�
public:
	//************************************
	// @brief:    ��Ҫ���¼��㸴�Ͽؼ���λ��
	// @note:     
	// @return:   void 
	//************************************
	void	ReCalcRect();

	//////////////////////////////////////////////////////////////////////////
	//�̳к���
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
	// @brief:    ���㸴�Ͽؼ���λ��
	// @note:     
	// @return:   void 
	//************************************
	virtual void CalcRect();

	//////////////////////////////////////////////////////////////////////////
	//�̳к���
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
	bool m_bDrawSuper;		//����Item������ʽ
	bool m_bEnableItemState;//�����ڵ����
	bool m_bEnableItems;	//�����ӿؼ�����
	bool m_bEnableColsText;	//���������ֻ���
	bool m_bEnableSuper;	//����������ʽ����
	bool m_bCalc;
	int m_nMaskClick;
	bool m_bMaskCols;		//�������е�λ�ðڷſؼ�
//}}
};

/*!
    \brief    ����ڵ�ؼ�
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
	// @brief:    չ���ڵ�
	// @note:     
	// @param: 	  bool bExpand 
	// @param: 	  UINT uFlags UINODE_EXPAND_TOLEAF/UINODE_EXPAND_TOROOT
	// @return:   bool �Ƿ���ˢ��
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
	//INode�̳�
	virtual void node_show(bool bShow);
	virtual bool is_node_show();
	virtual int  get_node_index();
	virtual void CheckLegal();

	bool m_bEnableExpand;
	//}}
};

/*!
    \brief    �б�ڵ������ؼ�
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


