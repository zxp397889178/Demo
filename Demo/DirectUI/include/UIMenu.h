/*********************************************************************
*       Copyright (C) 2010,Ӧ���������
*********************************************************************
*   Date             Name                 Description
*   2010-12-15       hanzp				  Create.
*********************************************************************/
#pragma once
//{{
#define MENU_INSET			9			/**< �˵����� */
#define MENUSEP_HEIGHT	  	4			/**< �˵��ָ�����ռ�˵���߶� */
#define BY_LAST		 		3			/**< ����˵�λ��ĩβ */
#define BY_FIRST		 	2			/**< ����˵�λ�ÿ�ͷ */
#define MASK_MATCH(check,mask)	(((check) & (mask)) == (mask))
//}}
class CMenuWindowUI;
/*!
    \brief    �˵���Ϣ��
*****************************************/
class DUI_API UIMENUINFO
{
	//{{
	friend class CMenuUI;
public:
	UIMENUINFO(){
		uID = 0;
		szText = _T("");
		szIconFile = _T("");
		szKey = _T("");
		bCheck = false;
		bSeparator = false;
		bEnableItem = true;
		uKey = 0;
		uImageSize = 11;
		wParamNotify = NULL;
		lParamNotify = NULL;
	}
	//}}
	void Copy(const UIMENUINFO& menuInfo)
	{
		uID				= menuInfo.uID;
		uKey			= menuInfo.uKey;
		uImageSize		= menuInfo.uImageSize;
		szText			= menuInfo.szText;
		szIconFile		= menuInfo.szIconFile;
		szKey			= menuInfo.szKey;
		bCheck			= menuInfo.bCheck;
		bSeparator		= menuInfo.bSeparator;
		bEnableItem		= menuInfo.bEnableItem;

		/// ���˵��ķ��ز���
		wParamNotify	= menuInfo.wParamNotify;
		lParamNotify	= menuInfo.lParamNotify;
		/// ����ӿؼ���ID
		strSubID		= menuInfo.strSubID;
	}
	UINT uID;
	UINT uKey;
	UINT uImageSize;
	tstring szText;
	tstring szIconFile;
	tstring szKey;
	bool bCheck;
	bool bSeparator;
	bool bEnableItem;

	/// ���˵��ķ��ز���
	WPARAM wParamNotify;
	LPARAM lParamNotify;
	/// ����ӿؼ���ID
	tstring strSubID;
	//{{
private:
	CStdPtrArray MppSubWindow;
	//}}
};

/*!
    \brief    �˵��ؼ�
*****************************************/
class DUI_API CMenuUI : public CControlUI
{
	//{{
	friend class CMenuWindowUI;
	UI_OBJECT_DECLARE(CMenuUI, _T("MenuItem"))
public:
	CMenuUI();
	virtual ~CMenuUI();
	//}}


	typedef enum
	{
		UIMENU_NOHANDLE = 0x0000,
		UIMENU_CENTERALIGN = 0x0001,
		UIMENU_LEFTALIGN = 0x0002,
		UIMENU_RIGHTALIGN = 0x0004,
		UIMENU_BOTTOMALIGN = 0x0008,
		UIMENU_TOPALIGN = 0x0010,
		UIMENU_VCENTERALIGN = 0x0020
	};

	static void ReleaseInstance();

	static CMenuUI* Load(UINT uMenuID);

	HWND Show(HWND hWndParent, LPPOINT lpPoint, INotifyUI* pNotify);
	HWND ShowEx(HWND hWndParent, LPPOINT lpPoint, INotifyUI* pNotify, UINT uFlags);
	bool Insert(UINT uid, LPCTSTR lpszText, UINT uFlag = BY_LAST, UINT nIndex = -1);
	bool Insert(CMenuUI* pMenu, UINT uFlag = BY_LAST, UINT nIndex = -1);
	bool InsertSep(UINT nIndex = -1, UINT uFlag = BY_LAST);
	virtual bool Remove(CControlUI* pControl);
	virtual bool Remove(int nIndex, int uFlag = BY_POSITION);
	virtual void SetVisible( bool bShow);
	virtual void SetInternVisible( bool bVisible );

	CMenuUI* GetItem(int nIndex, int uFlag = BY_POSITION);
	UIMENUINFO& GetMenuInfo();
	void SetUID(UINT uID);
	UINT GetUID();
	bool IsEnableItem();
	void EnableItem(bool bEnable = true);
	bool IsCheck();
	void Check(bool bCheck = true);
	bool IsSeparator();
	void Separator(bool bSeparator = true);
	virtual void SetText(LPCTSTR lpszText);
	virtual LPCTSTR GetText();
	virtual void SetTextColor(DWORD dwTextColor = DUI_ARGB(255,0,0,0));
	void SetMenuWindow(CMenuWindowUI* pWindow){m_pMenuWindow = pWindow;}
	CMenuWindowUI* GetMenuWindow(){return m_pMenuWindow;}
	virtual void SetMenuAttribute(int nIndex, LPCTSTR lpszName, LPCTSTR lpszValue, int uFlag = BY_POSITION);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	int GetCount();
	void SetParam(WPARAM wParam, LPARAM lParam);
	//{{
protected:
	HWND Popup(HWND hParent, int x = -1, int y = -1, INotifyUI* pNotify = NULL, UINT uFlags = UIMENU_NOHANDLE);
	bool InsertAt(int iIndex, CMenuUI* pMenu);
	SIZE GetWindowSize(){return m_szMenuWindow;}
	WPARAM GetWParam();
	LPARAM GetLParam();

	int GetIndex(UINT uID);
	void UnLoadWindow();

	void SetMenuItemRect(RECT& rc);
	RECT GetMenuItemRect();

	int  HitItem(POINT ptMouse);
	bool SelectItem(int nIndex);
	void DrawItem(IRenderDC* pRenderDC, RECT& rcPaint);

 	virtual void Init();
	virtual bool Add(CControlUI* pControl);
	virtual void SetRect(RECT& rectRegion);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual bool Event(TEventUI& event);
private:
	int  m_nSelectItem;
	RECT m_rcMenuItem;
	SIZE m_szItem;
	int  m_nBaseItemWidth;
	SIZE m_szMenuWindow;
	tstring m_strBackGround;
	tstring m_strHotKey;
	tstring m_strResultText;
	bool m_bShowActive;
	CMenuWindowUI*  m_pMenuWindow;
	CStdPtrArray m_MenuItems;
	CStdPtrArray m_SubControls;
	UIMENUINFO* m_pMenuInfo;
	//}}
};
