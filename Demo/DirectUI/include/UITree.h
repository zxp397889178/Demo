/*********************************************************************
*       Copyright (C) 2010,Ӧ���������
*********************************************************************
*   Date             Name                 Description
*   2010-11-17       hanzp				  Create.
*********************************************************************/
#pragma once



//////////////////////////////////////////////////////////////////////////
//{{
 #define UITVI_ROOT                ((CTreeNodeUI*)(ULONG_PTR)-0x10000)
 #define UITVI_FIRST               ((CTreeNodeUI*)(ULONG_PTR)-0x0FFFF)
 #define UITVI_LAST                ((CTreeNodeUI*)(ULONG_PTR)-0x0FFFE)
//}}
class ICheckNodeUI;
/*!
    \brief    ���ؼ�
*****************************************/
class DUI_API CTreeUI : public CContainerUI, public INodeOwner
{
	//{{
	friend class CTreeNodeUI;
	UI_OBJECT_DECLARE(CTreeUI, _T("Tree"))
public:
	CTreeUI();
	virtual ~CTreeUI();

public:
	virtual bool SelectItem(CControlUI* pControl);
	CTreeNodeUI* GetCurSel();
	CTreeNodeUI* GetRoot();

	//************************************
	// @brief:    ʹ�ؼ����ڿɼ�λ��
	// @note:     
	// @param: 	  CControlUI * pControl 
	// @return:   void 
	//************************************
	void EnsureVisible(CControlUI* pControl);

	//************************************
	// @brief:    ʹ�ؼ����ڿɼ�λ��
	// @note:     
	// @param: 	  CControlUI * pControl 
	// @return:   void 
	//************************************
	void EnsureVisibleFirst(CControlUI* pControl);

	//************************************
	// @brief:    ʹ�ؼ����ڿɼ�λ��-����
	// @note:     
	// @param: 	  CControlUI * pControl 
	// @param: 	  int & nBegin 
	// @param: 	  int & nEnd 
	// @param: 	  bool bRedraw 
	// @return:   int 
	//************************************
	int  EnsureVisibleCenter(CControlUI* pControl,int& nBegin, int &nEnd, bool bRedraw = true);

	//************************************
	// @brief:    ����״̬��ȡͼƬ
	// @note:     
	// @return:   ImageObj* 
	//************************************
	virtual ImageObj* GetStateImage();

	//************************************
	// @brief:    ��ȡ�ڵ㱳��ͼƬ
	// @note:     
	// @return:   ImageObj* 
	//************************************
	virtual ImageObj* GetItemBkgImage();

	bool IsEnableItemBkg();

	//************************************
	// @brief:    ���������ȡ�ؼ�
	// @note:     ֻ�Կɼ�����
	// @param: 	  POINT pt 
	// @return:   CControlUI* 
	//************************************
	CControlUI* ItemFromPos(POINT pt);

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

	//************************************
	// @brief:    ����
	// @note:     comapre pNodeA�ŵ��������ȣ�����false
	// @param: 	  bool 
	// @param: 	  * pCompare �ȽϺ���
	// @param: 	  CTreeNodeUI * pNodeA
	// @param: 	  CTreeNodeUI * pNodeB
	// @param: 	  CTreeNodeUI * node 
	// @return:   void 
	//************************************
	//{{
	void Sort(bool(* pCompare)(CTreeNodeUI* ,CTreeNodeUI* ), CTreeNodeUI* node = UITVI_ROOT);
	//}}
	//************************************
	// @brief:    �����ӽڵ��븸�ڵ��ƫ��
	// @note:     
	// @param: 	  int nMagin 
	// @return:   void 
	//************************************
	void SetMaginToParent(int nMagin); 

	//************************************
	// @brief:    ��ڵ��Ƿ�����ѡ��̬
	// @note:     
	// @return:   bool 
	//************************************
	bool EnableGroupItemSelect();

	//************************************
	// @brief:    ��ӽڵ�
	// @note:     
	// @param: 	  CTreeNodeUI * pControl 
	// @param: 	  CTreeNodeUI * pParent 
	// @param: 	  CTreeNodeUI * pInsertAfter 
	// @return:   CTreeNodeUI* 
	//************************************
	//{{
	virtual CTreeNodeUI* InsertItem(CTreeNodeUI* pControl, CTreeNodeUI* pParent = UITVI_ROOT, CTreeNodeUI* pInsertAfter = UITVI_LAST);
	//}}
	//************************************
	// @brief:    ��ӽڵ�
	// @note:     
	// @param: 	  CTreeNodeUI * pControl 
	// @param: 	  CTreeNodeUI * pParent 
	// @param: 	  pCompare �ȽϺ���
	// @return:   CTreeNodeUI* 
	//************************************
	//{{
	CTreeNodeUI* InsertItemByCompare(CTreeNodeUI* pControl, CTreeNodeUI* pParent, bool(* pCompare)(CTreeNodeUI* ,CTreeNodeUI* ));
	//}}
	//************************************
	// @brief:    ɾ���ڵ�
	// @note:     
	// @param: 	  CTreeNodeUI * pControl 
	// @return:   void 
	//************************************
	virtual void RemoveItem(CTreeNodeUI* pControl);


	//************************************
	// @brief:    ��ȡ��ǰ�����ؼ�
	// @note:     
	// @return:   CControlUI* 
	//************************************
	CControlUI* GetHotItem();

	bool IsEnableDragNode();

	bool IsEnableSelect();
	bool IsEnableItemOver();
	//�̳к���
	virtual bool Add(CControlUI* pControl);
	virtual bool Remove(CControlUI* pControl);
	virtual void RemoveAll();
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual bool ChildEvent(TEventUI& event);

	void SetCheckedAfterPartialChecked(bool bCheck){
		m_bCheckedAfterPartialChecked = bCheck;
	};
	bool IsCheckedAfterPartialChecked(){
		return m_bCheckedAfterPartialChecked;
	};

	//{{
protected:
	virtual void RenderItemBkg(ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint);
	/*!
	   \brief    ���ƽڵ�״̬ͼ
	   \param    ImageObj * pImgState 
	   \param    CControlUI * pControl 
	   \param    IRenderDC * pRenderDC 
	   \param    RECT & rcPaint 
	   \return   void 
	 ************************************/
	virtual void RenderItemState(ImageObj* pImgState, CControlUI* pControl, IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void SetItemInset(CTreeNodeUI* pControl, CTreeNodeUI* pParent);
	void SetItemVisible(CTreeNodeUI* pControl, bool bVisible);

	//�̳к���
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual SIZE OnChildSize(RECT& rcWinow, RECT& rcClient);
	//INodeOwner
	virtual void expanding(INode* pNode, bool bExpand);
	virtual void expandend(INode* pNode, bool bExpand);
	virtual void NeedUpdate();
	virtual INode* get_node(int nIndex);
	virtual void set_node(int nIndex, INode* pNode);

	virtual CControlUI* FindControl( FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags );
	virtual void Init();
protected:
	CControlUI* m_pHighlight;
	int  m_nMaginToParent;
	int  m_nFirstVisible;
	int  m_nLastVisible;
	bool m_bDragNode;				//�Ƿ�������ҷ
	bool m_bLimitGroupClick;		//�Ƿ�������ڵ���
	bool m_bEnableItemBkg;			//�Ƿ��нڵ㱳��
	bool m_bEnableItemOver;			//�Ƿ����б���������ƶ������еı���
	bool m_bEnableItemSelect;		//�Ƿ�����ڵ���ʾѡ��̬

private:
	bool m_bLockChildsRect;
	bool m_bCheckedAfterPartialChecked;
	//}}
};

/*!
    \brief    ���ڵ�ؼ�
*****************************************/
class DUI_API CTreeNodeUI : public CControlUI, public INode
{
	//{{
	friend class CTreeUI;
	friend class ICheckNodeUI;
	UI_OBJECT_DECLARE(CTreeNodeUI, _T("TreeNode"))
public:
	//}}

	CTreeNodeUI(bool bGroup = false);
	virtual ~CTreeNodeUI();

	static bool ms_bTreeNodeExpand;
public:
	virtual ImageObj* GetImage();
	virtual void SetImage(LPVOID lpData, long nSize);
	virtual void SetImage(ImageObj* pImageObj);
	virtual void SetImage(LPCTSTR lpszName);

	//************************************
	// @brief:    ���ö�������
	// @note:     ��������ʱ��ɾ�������������pData����������
	// @param: 	  void * pData 
	// @return:   void 
	//************************************
	void SetMyData(void *pData);
	void* GetMyData();

	void EnableDrag(bool bEnable);

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
	int find_child(CTreeNodeUI* node);
	CTreeNodeUI* get_child(int i);
	CTreeNodeUI* get_last_child();
	CTreeNodeUI* get_parent();
	CTreeNodeUI* get_nextsibling(CTreeNodeUI* pNode);
	CTreeNodeUI* get_prevsibling(CTreeNodeUI* pNode);
	CTreeNodeUI* find_child(UINT uData);
	void set_child( CTreeNodeUI* node, int nIndex );

	virtual void SetRect(RECT& rectRegion);
	virtual void CalcRect();
	bool EnableItemState() const { return m_bEnableItemState; }
	void EnableItemState(bool val) { m_bEnableItemState = val; }
	//checknode
	void Check(bool bCheck, UINT uFlags = UIITEMFLAG_CHECKED | UIITEMFLAG_ALL);
	bool IsCheck(UINT uFlags = UIITEMFLAG_CHECKED);
	void CheckLegal();
	void RenderCheck(IRenderDC* pRenderDC, RECT& rcPaint);
	void SetCheckRect(RECT &rc);
	int  GetCheckOffset();

	//�̳к���
	virtual bool Activate();
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual bool Add(CControlUI* pControl);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void CalcControl( RECT & rectCalc );
	virtual void set_node_index(int nIndex);

	//INode�̳�
	//{{
protected:
	virtual void node_show(bool bShow);
	virtual bool is_node_show();
	virtual int  get_node_index();

protected:

	void add_child(CTreeNodeUI* child, int nIndex);
	void remove_child(CTreeNodeUI* child);
	void remove_all();

	bool m_bEnableItemState;//�����ڵ����
	bool m_bEnableExpand;
	bool m_bShake;
	bool m_bCalc;
	bool m_bCheck;
	bool m_bCheckVisible;
	ICheckNodeUI* m_pCheckNode;

protected:
	void* m_pMyData;
	bool m_bCanDrag;
	//}}
};

/*!
    \brief    �������ڵ�ؼ�
*****************************************/
class DUI_API CTreeContainerNodeUI : public CTreeNodeUI
{
	//{{
	UI_OBJECT_DECLARE(CTreeContainerNodeUI, _T("TreeContainerNode"))
public:
	CTreeContainerNodeUI(bool bGroup = false);
	~CTreeContainerNodeUI();
	//}}

	CControlUI* GetItem(int iIndex);
	CControlUI* GetItem(LPCTSTR lpszId);
	int GetCount();
	bool Add(CControlUI* pControl);
	bool Remove(CControlUI* pControl);
	void SetINotifyUI(INotifyUI* pINotify);
	virtual void SetRect(RECT& rectRegion);
	//{{
protected:
	virtual bool ChildEvent(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual void CalcRect();
	virtual void SetManager(CWindowUI* pManager, CControlUI* pParent);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void SetVisible( bool bShow);
	virtual void SetInternVisible( bool bVisible );
private:
	CStdPtrArray m_items;
	bool m_bDrawSuper;
	int m_nMaskClick;
	//}}
};
 
