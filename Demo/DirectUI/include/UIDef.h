#pragma once

#define DT_LEFT_EX GET_DT_LEFT_EX_VALUE()
#define DT_RIGHT_EX GET_DT_RIGHT_EX_VALUE()
	
//{{
#ifndef tstring
#ifdef UNICODE	
//}}
typedef std::wstring tstring;
//{{
#else
typedef std::string tstring;
#endif
#endif
//}}

#ifndef uint8
typedef unsigned char uint8;
#endif

class CRdbParser;
class DUI_API StyleObj;
class DUI_API CControlUI;
class DUI_API CListUI;
class DUI_API CListItemUI;
class DUI_API CListHeaderUI;
class DUI_API CListHeaderItemUI;
class DUI_API CTableUI;
class DUI_API CTableItemUI;
class DUI_API UIMENUINFO;
class DUI_API CWebBrowserUI;
class DUI_API CAction;
class DUI_API ColorObj;
class DUI_API CursorObj;
class DUI_API FontObj;
class DUI_API ImageObj;
class DUI_API ClipObj;
class DUI_API DibObj;
class DUI_API IRenderDC;
class DUI_API CStdPtrArray;
class DUI_API CDelegateBase;
class DUI_API CWindowUI;
class CDPI;

//namespace{

/*!
    \brief    ��Դ����
*****************************************/
typedef enum
{
	UIRES_FROM_FILE = 0,  /**< ���ļ��м��� */
	UIRES_FROM_RESOURCE,  /**< ����Դ�м��� */
	UIRES_FROM_RDB,       /**< ��RDB�м��� */
} UITYPE_RES;

/*!
    \brief    ��������
*****************************************/
typedef enum
{
	UIFONT_GDI = 0,    /**< GDI */
	UIFONT_GDIPLUS,    /**< GDI+ */
	UIFONT_DEFAULT,    
} UITYPE_FONT;

/*!
    \brief    ������Ч
*****************************************/
typedef enum
{
	UIFONTEFFECT_NORMAL = 0,
	UIFONTEFFECT_GLOW_AUTO,           //������Ч 
	UIFONTEFFECT_GLOW_GOLD,
	UIFONTEFFECT_GLOW_BLACK,
} UITYPE_FONTEFFECT;

// ͼƬ����
typedef enum
{
	UIIAMGE_HBITMAP = 0, // λͼ��ʽ
	UIIAMGE_BITMAP,      // GDI+��ʽ
	UIIAMGE_PIXEL,       // ���ظ�ʽ
	UIIAMGE_HICON,       // ICON��ʽ
	UIIAMGE_DEFAULT
} UITYPE_IMAGE;

// ��ͼЧ��
typedef enum
{
	 UISMOOTHING_HighSpeed = 0, //���ٶ�
	 UISMOOTHING_HighQuality, //������
} UITYPE_SMOOTHING;

// ͼƬ��������
typedef enum
{
	UIPAINTMODE_NORMAL = 0, //������ normal
	UIPAINTMODE_STRETCH,    //������� fill
	UIPAINTMODE_TILE,		//ƽ�� tile
	UIPAINTMODE_XTILE,		//Xƽ�� xtile
	UIPAINTMODE_YTILE,		//Yƽ�� ytile
	UIPAINTMODE_CENTER,     //���л��� center
	UIPAINTMODE_FIT,		//ԭ�������� fit	
	UIPAINTMODE_CENTERCROP, //���������������и� centercrop
} UITYPE_PAINTMODE;

// �ؼ�ê������
typedef enum
{
	UIANCHOR_LT = 9, //���Ͻ�, LT|x,y,width,height
	UIANCHOR_RT = 10,    //���Ͻ�, RT|x,y,width,height
	UIANCHOR_RB = 11,  //���½�,RB|x,y,width,height
	UIANCHOR_LB = 12,    //���½�,LB|x,y,width,height

	UIANCHOR_TOP = 13,   //�ϱ� TOP|x1,y,x2,height
	UIANCHOR_RIGHT = 14,  //�ұ� RIGHT|x,y1,width,y2
	UIANCHOR_BOTTOM = 15,  //�±� BOTTOM|x1,y,x2,height
	UIANCHOR_LEFT = 16,   //��� LEFT|x,y1,width,y2

	UIANCHOR_LTRB = 8,  //�Խ��� LTRB|x1,y1,x2,y2

	// �ر�ע��:���¼��ַ�ʽ�Ǿɵ����귽ʽ�����ں�����⣬��ò�Ҫ�ã�ֻ��Ϊ�˼��ݾɴ��룬ֻҪѧϰ���漸�ַ�ʽ�Ϳ�����
	UIANCHOR_LT_OLD = 0, //���Ͻ�  0|x1,y1,X2,y2 
	UIANCHOR_RT_OLD = 1,    //���Ͻ� 1|x1,y1,X2,y2
	UIANCHOR_RB_OLD = 2,  //���½� 2|x1,y1,X2,y2
	UIANCHOR_LB_OLD = 3,    //���½� 3|x1,y1,X2,y2

	UIANCHOR_TOP_OLD = 4,   //�ϱ� 4|x1,y1,X2,y2 �� LTRT|x1,y1,X2,y2
	UIANCHOR_RIGHT_OLD = 5,  //�ұ� 5|x1,y1,X2,y2 �� RTRB|x1,y1,X2,y2
	UIANCHOR_BOTTOM_OLD = 6, //�±� 6|x1,y1,X2,y2 �� LBRB|x1,y1,X2,y2
	UIANCHOR_LEFT_OLD = 7,   //��� 7|x1,y1,X2,y2 ��  LTLB|x1,y1,X2,y2
	//�ر�ע��:0-4���ں�����⣬��ò�Ҫ�ã����ܻᱻ����
	UIANCHOR_UNCHANGED = -1,
}UITYPE_ANCHOR;

#define UIANCHOR_LEFTTOP  UIANCHOR_LT_OLD
#define UIANCHOR_RIGHTTOP UIANCHOR_RT_OLD
#define UIANCHOR_RIGHTBOTTOM UIANCHOR_RB_OLD
#define UIANCHOR_LEFTBOTTOM UIANCHOR_LB_OLD

#define UIANCHOR_LTRT  UIANCHOR_TOP_OLD
#define UIANCHOR_RTRB UIANCHOR_RIGHT_OLD
#define UIANCHOR_LBRB UIANCHOR_BOTTOM_OLD
#define UIANCHOR_LTLB UIANCHOR_LEFT_OLD

// �ؼ���������
typedef enum
{
	UILAYOUT_NORMAL=0,   // ����״̬�����Զ�����
	UILAYOUT_LEFTTORIGHT,// ������������
	UILAYOUT_RIGHTTOLEFT ,// ������������
	UILAYOUT_TOPTOBOTTOM ,//���ϵ���
	UILAYOUT_BOTTOMTOTOP ,//���µ���
	UILAYOUT_HORZFILL , //ˮƽƽ������
	UILAYOUT_VERTFILL ,//��ֱƽ������
	UILAYOUT_GRID ,    //����
}UITYPE_LAYOUT;

// �ؼ�״̬
typedef enum
{
	UISTATE_NORMAL = 0, //����
	UISTATE_OVER,     //��꾭��
	UISTATE_DOWN,     //��갴�� 
	UISTATE_FOCUS,    //��ȡ��꣨����״̬�����ã����磺�༭����õ�������겻�ڱ༭�򣬵�����ڱ༭��ʱ����
	UISTATE_DISABLE,  //��ֹ״̬

	// ��������״̬��������һһ��Ӧ�ģ�checkbox�����õ�����ѡ��״̬�£��ؼ���Ӧ������״̬��
	UISTATE_CKNORMAL = 8, //checked, normal 
	UISTATE_CKOVER,//checked, mouse is hover 
	UISTATE_CKDOWN,//checked, the left button of mouse is press 
	UISTATE_CKFOCUS,//checked, the control get the focus of keyboard 
	UISTATE_CKDISABLE,//checked, disable 

}UITYPE_STATE;

// �ⲿ�¼�
typedef enum
{
	UINOTIFY__FIRST,
	// ͨ���¼�
	UINOTIFY_CLICK,
	UINOTIFY_CLICK_LINK,	//wParam:index of link; lParam:link; //for CHtmlLabelUI
	UINOTIFY_RCLICK,
	UINOTIFY_DBCLICK,
	UINOTIFY_SELCHANGE,
	UINOTIFY_SETFOCUS,
	UINOTIFY_KILLFOCUS,
	UINOTIFY_DROPFILES,		//wParam:HDROP; lParam:NULL;
	// �༭���¼�
	UINOTIFY_EN_CHANGE,
	UINOTIFY_EN_IMESTART,
	UINOTIFY_EN_IMEEND,
	UINOTIFY_EN_IMEING,
	UINOTIFY_EN_LIMIT,		//wParam:NULL; lParam:bool pointer(ֵΪtrueʱʹ��Ĭ����ʾ);
	UINOTIFY_EN_KEYDOWN,	//wParam:virtual-key code;
	UINOTIFY_EN_LISTCLICK,
	UINOTIFY_EN_LISTRETURN,
	//�㼶�ṹ�ڵ�
	UINOTIFY_TVN_ITEMEXPENDED,	//wParam:bExpand; lParam:չ���Ŀؼ�ָ��;
	UINOTIFY_TVN_ITEMEXPENDING,	//wParam:bExpand; lParam:չ���Ŀؼ�ָ��;
	// ���οؼ��¼�
	UINOTIFY_TVN_BEGINDRAG,	//wParam:NULL; lParam:��ҷ�Ŀؼ�ָ��;
	UINOTIFY_TVN_ENDDRAG,	//wParam:��ҷĿ��ؼ�ָ��; lParam:��ҷ�Ŀؼ�ָ��;
	UINOTIFY_TVN_CHECK,		//wParam:bCheck; lParam:�ؼ�ָ��;
	UINOTIFY_TVN_KEYDOWN,	//wParam��lParamͬWM_KEYDOWN
	// �б�ؼ�
	UINOTIFY_LB_KEYDOWN,	//wParam��lParamͬWM_KEYDOWN
	UINOTIFY_LB_AREASELECT_ENDED,
	UINOTIFY_LB_AREASELECT,
	// Slider�¼�
	UINOTIFY_TRBN_POSCHANGE,
	UINOTIFY_TRBN_SELCHANGE,
	UINOTIFY_TRBN_BEGINCHANGE,
	UINOTIFY_TRBN_ENDCHANGE,
	// richedit�ؼ��¼� 
	UINOTIFY_RE_DBCLICK_IMAGE,
	UINOTIFY_RE_CLICK_IMAGE_THUMBNAIL_BTN,
	UINOTIFY_RE_CLICK_LINK,
	// ActiveX�ؼ�
	UINOTIFY_ACTIVEX_CREATE,
	UINOTIFY_ACTIVEX_SHOWCOMPLETE,		//wParam:NULL; lParam:URL;
	UINOTIFY_ACTIVEX_NAVIGATE_BEFORE,	//wParam:VARIANT_BOOL Pointer(�Ƿ�ȡ��); lParam:URL;
	UINOTIFY_ACTIVEX_NAVIGATE_COMPLETE,	//wParam:NULL; lParam:URL;
	UINOTIFY_ACTIVEX_NAVIGATE_ERROR,	//wParam:nStatusCode;
	UINOTIFY_ACTIVEX_LODING,
	UINOTIFY_ACTIVEX_LODE_DONE,
	UINOTIFY_ACTIVEX_CLOSE,
	// Animation
	UINOTIFY_ANIMATION_END,
	// ComboBox
	UINOTIFY_CBN_BEFOREDROPDOWN,
	UINOTIFY_CBN_DROPDOWN,
	UINOTIFY_CBN_CHECK,
	// Tab

	UINOTIFY_TAB_CLOSE,
	//�ȼ�
	UINOTIFY_HOTKEY, 

	// ��ݲ˵�
	UINOTIFY_GET_CONTEXTMENU,

	// �����¼�
	UINOTIFY_DRAG_DROP_GROUPS = UINOTIFY__FIRST + 900, // �Ϸ��¼�����
		//�ӡ����桱�����ݵ����ؼ���
		UINOTIFY_DRAG_ENTER ,// ���� 
		UINOTIFY_DRAG_OVER,  // ���� 
		UINOTIFY_DRAG_LEAVE, // �뿪
		UINOTIFY_DROP,       // ����
		//�ӡ��ؼ��������ݵ������桱
		UINOTIFY_BEGIN_DRAG, //��ʼ�����ؼ�
		UINOTIFY_DRAG_QUERYDATA_DELAY,

	// �����ؼ�
	UINOTIFY_DATE_CHANGE,

	UINOTIFY__LAST = UINOTIFY__FIRST + 1000,
	UINOTIFY__USER,	/// �����Զ���ؼ���Ϣ�����.
	UINOTIFY__USER_LAST = UINOTIFY__USER + 1000,
	// Ƥ���¼�

}UITYPE_NOTIFY;

// �ڲ��¼�
typedef enum
{
UIEVENT__FIRST = 0,
UIEVENT__MOUSEBEGIN,
	UIEVENT_MOUSEMOVE,
	UIEVENT_MOUSELEAVE,
	UIEVENT_MOUSEENTER,
	UIEVENT_MOUSEHOVER,
	UIEVENT_BUTTONDOWN,
	UIEVENT_BUTTONUP,
	UIEVENT_LDBCLICK,
	UIEVENT_RBUTTONDOWN,
	UIEVENT_RBUTTONUP,
	UIEVENT_RDBCLICK,
    UIEVENT_SCROLLWHEEL,
	UIEVENT_MOUSEENTERRECT,   //��UIEVENT_MOUSEENTER ��������,UIEVENT_MOUSEENTER���Ƚ����ӿؼ�ʱ���ᴥ������UIEVENT_MOUSEENTERRECT��
	UIEVENT_MOUSELEAVERECT,   //��UIEVENT_MOUSEENTER ��������,UIEVENT_MOUSELEAVE�ں�����ӿؼ�ʱ�ᴥ����UIEVENT_MOUSEENTERR
  
UIEVENT__MOUSEEND,
UIEVENT__KEYBEGIN,
	UIEVENT_KEYDOWN,
	UIEVENT_KEYUP,
	UIEVENT_CHAR,
	UIEVENT_IME_CHAR,
	UIEVENT_IME_START,
	UIEVENT_IME_END,
	UIEVENT_IME_ING,
	UIEVENT_SYSKEY,
UIEVENT__KEYEND,
	UIEVENT_KILLFOCUS,
	UIEVENT_SETFOCUS,
	UIEVENT_VSCROLL,
	UIEVENT_HSCROLL,
	UIEVENT_SETCURSOR,
	UIEVENT_TIMER,
	UIEVENT_SIZE,

	UIEVENT_DESTORY, //�ؼ��鹹
	UIEVENT_ADD_CHILD, //����ӿؼ�
	// �Ϸ� begin
	UIEVENT_DRAG_DROP_GROUPS = UINOTIFY_DRAG_DROP_GROUPS, // �Ϸ��¼�����
		UIEVENT_DRAG_ENTER,
		UIEVENT_DRAG_OVER,
		UIEVENT_DRAG_LEAVE,
		UIEVENT_DROP,
		UIEVENT_BEGIN_DRAG,
		UIEVENT_END_DRAG,
		UIEVENT_DRAG_QUERYDATA_DELAY,
	// �Ϸ� end

	UIEVENT_ITEM_MOUSEHOVER_CHANGE, //wParam:�ɸ����ؼ�ָ��; lParam:�¸����ؼ�ָ��;
	UIEVENT_ITEM_SEL_CHANGE, //wParam:��ѡ��ؼ�ָ��; lParam:��ѡ��ؼ�ָ��;
	// ���������д��ھ���Ŀؼ�
	UIEVENT_WINDOW_PROC,

	UIEVENT_DPI_CHANGED,

	UIEVENT_GESTURENOTIFY, // ����֪ͨ����ͨ������Ϣȡ������
	UIEVENT_GESTURE, // ������Ϣ
	UIEVENT_UNUSED,
UIEVENT__LAST,

}UITYPE_EVENT;

typedef enum
{
	UITOOLTIP_Succeed = 0,
	UITOOLTIP_Info,
	UITOOLTIP_Warn,
	UITOOLTIP_Error,
	UITOOLTIP_Question
}UITYPE_TOOLTIP;

//{{

#define ID_MSGBOX_BTN  50


// Flags for CControlUI::GetFlags()
#define UICONTROLFLAG_TABSTOP       0x00000001
#define UICONTROLFLAG_SETCURSOR     0x00000002
#define UICONTROLFLAG_WANTRETURN    0x00000004
#define UICONTROLFLAG_SETFOCUS      0x00000008 //�ܹ����ù��
#define UICONTROLFLAG_ENABLE_DROP   0x00000010 
#define UICONTROLFLAG_ENABLE_DRAG   0x00000020
#define UICONTROLFLAG_WANTESCAPE    0x00000040
#define UICONTROLFLAG_ENABLE_AREAS  0x00000080
#define UICONTROLFLAG_ENABLE_DESTORY_EVENT  0x00000100
#define UICONTROLFLAG_CHECK_CTR_PTR_VALID  0x00000200 //�����ж�ָ���Ƿ���Ч

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_MOUSE_ENABLED 0x00000010
#define UIFIND_MOUSE		 0x00000020
#define UIFIND_MOUSEINOUT	 0x00000040
#define UIFIND_GESTURE_ENABLED 0x00000080
#define UIFIND_MOUSEWHELL_ENABLED 0x00000100
#define UIFIND_ME_FIRST      0x80000000

// Flags // ������Щ�Ǹ����οؼ����б�ؼ�ʹ�õģ��������ȥ������Щ�ܲ����Ϲ淶
#define UIITEMFLAG_VISIBLE		0x00000001
#define UIITEMFLAG_ENABLED		0x00000002
#define UIITEMFLAG_CHECKED		0x00000004
#define UIITEMFLAG_ALL          0x00000010
#define UIITEMFLAG_NOTIFY		0x00000040

//ʱ��ID
#define DUI_TIMERID_BASE 0x1000
#define DUI_TIMERID_USER 0x2000
//}}

/////////////////////////////////////////////////////////////////////////////////////
//����/�����¼�֪ͨ�ṹ
typedef struct tagTEventUI
{
	int nType;
	CControlUI* pSender;
	POINT	ptMouse;
	WPARAM	wParam;
	LPARAM	lParam;
} TEventUI, TNotifyUI;

//�û��Զ����Ǽ�
typedef struct tagTUserEventUI
{
	UINT uEventID;
	WPARAM	wParam;
	LPARAM	lParam;
} TUserEventUI;


//{{
typedef struct tagDragDropParams
{
	IDataObject * pDataObject;
	DWORD grfKeyState;
	POINTL pt;
	DWORD * pdwEffect;
	bool  bFiles;
} DragDropParams;
//}}


#define BY_COMMAND 			0			/**< ����˵�λ��ָ����ʽΪ�˵���ID */
#define BY_POSITION 		1			/**< ����˵�λ��ָ����ʽΪ�˵���� */
//{{

typedef CControlUI* (__cdecl* CONTROLCREATEPROC)();
typedef CControlUI* (_stdcall* FINDCONTROLPROC)(CControlUI*, LPVOID);
typedef StyleObj* (__cdecl* STYLECREATEPROC)(CControlUI*);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ȫ�ֽӿ�
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class RefCountedThreadSafe
{
public:
	RefCountedThreadSafe() : m_ref(0),m_pCountedOwner(NULL),m_bValid(false) {}
	virtual ~RefCountedThreadSafe() { m_ref = 0; m_pCountedOwner = NULL; }

	bool IsValid(){return m_bValid;}

	void SetValid(bool bValid){m_bValid = bValid;}

	void AddRef() { ::InterlockedIncrement(reinterpret_cast<volatile long*>(&m_ref)); }

	void Release() 
	{ 
		if (0 == ::InterlockedDecrement(reinterpret_cast<volatile long*>(&m_ref)))
			delete this;
	}

	T* GetCountedOwner() const { return m_pCountedOwner; }

	void SetCountedOwner(T* val) { m_pCountedOwner = val; }

private:
	long m_ref;
	bool m_bValid;
	T* m_pCountedOwner;
};

class DUI_API INotifyUI
{
public:
	INotifyUI();
	virtual ~INotifyUI();
	RefCountedThreadSafe<INotifyUI>* GetINotifySafe();

	virtual bool ProcessNotify(TNotifyUI* pNotify) = 0;

private:
	RefCountedThreadSafe<INotifyUI>* m_INotifySafe;
};

//��Ϣ����
class IMessageFilterUI
{
public:
	virtual LRESULT MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

//��Ϣ����
class IPreMessageFilterUI
{
public:
	virtual bool PreMessageFilter(const LPMSG pMsg) = 0;
};

//xml��������
class ITemplateFilter
{
public:
	virtual ~ITemplateFilter(){};
	//����false ����Ը�style����window��Ĭ�Ϸ���true
	virtual bool OnTemplateAttributes(LPCSTR lpcsName, LPCSTR lpcsValue) = 0;
};


//��Ϣ����
class IUIControlDestory
{
public:
	virtual void OnControlDestory(CControlUI* pControl) = 0;
};


class IUIRes
{
public:
	// ����Ĭ���Ƿ񻺴�XML,���������ʵʱ��ȡ�������ı����ó��������������ܷ�Ӧ��������Ч�ʽϲһ��DebugĬ��Ϊfalse,ReleaseĬ��Ϊtrue
	virtual void SetCacheXML(bool bCacheXML) = 0;

	// ���ý�������,��ENG�����ģ������ַ���ʽ�ɳ����Լ�����ֻ��Ҫ��xml���ñ���һ�£�
	virtual void SetLanguage(LPCTSTR lpszLanguage) = 0;
	virtual LPCTSTR GetLanguage() = 0;

	// �ú�����Ŀ���ǣ�ͬһ��id���Զ�Ӧ��ͬͼƬ���ȿ����ڲ�ͬ���԰汾��Ҳ�����ڲ�ͬ����Ƥ��
	virtual void SetImageLanguage(LPCTSTR lpszLanguage) = 0;

	// ����Ĭ���������֣�������
	virtual void SetDefaultFontName(LPCTSTR lpszFontName) = 0;
	virtual LPCTSTR GetDefaultFontName() = 0;

	// ����Ĭ������������棬GDI��GDI+���棬δ��������FreeType����
	virtual void SetDefaultFontType(UITYPE_FONT eFontType) = 0;
	virtual UITYPE_FONT GetDefaultFontType()= 0;

	virtual void SetDefaultTextRenderingHint(int textRenderingHint = 0) = 0;
	virtual int GetDefaultTextRenderingHint() = 0;

	// ����Ĭ�ϵ�ͼƬ��ʽ������ʹ��pixel��ʹ�û���ͼ��Ч�ʸ��Ҳ�����GDI���󣩡�hbitmap��ʽ��ʹ�û���ͼ��Ч�ʸߣ���bitmap��ʽ(ʹ��gdi++��ͼ�������ߣ������������ؼ���Ҫ��������ͼ��ʹ�ã�
	virtual void SetDefaultImageType(UITYPE_IMAGE eImageType) = 0;

	// ����Ĭ�ϻ�������
	virtual void SetDefaultImageQuality(UITYPE_SMOOTHING eImageQualityType) = 0;

	// ��·���м�����Դ
	virtual void InitResDir(LPCTSTR lpszPath, bool bDefaultDir = false) = 0;
	// ��RDB�м�����Դ
	virtual void InitResDirFromRdb(LPCTSTR lpszPath, LPCTSTR lpszName, bool bDefaultDir = false) = 0;
	virtual void InitResDirFromRdb(LPBYTE lpData, DWORD dwSize, LPCTSTR lpszName, bool bDefaultDir = false) = 0;
	// ��RES������Դ
	virtual void InitResDirFromRes(UINT uResID, LPCTSTR lpszType) = 0;

	// ��������Ŀ¼
	virtual void SetThemePath(LPCTSTR lpszPath) = 0;
	//bCoexistence = true ��SetThemePath��ʽ����
	virtual void SetThemePathFromRDB(LPCTSTR lpszPath, bool bCoexistence = false) = 0;
	virtual CRdbParser*  GetThemeRdbFile() = 0;
	virtual LPCTSTR GetThemePath() = 0;

	// ����HDPIͼƬ·��
	virtual void SetHDPIImagePath(LPCTSTR lpszPath) = 0;
	virtual void SetHDPIImagePathRDB(LPCTSTR lpszPath) = 0;
	virtual CRdbParser*  GetHDPIImageRdbFile() = 0;

	virtual void SetHDPI9ImageParamScale(float fScale) = 0;
	virtual void SetHDPIAutoStretchOnDrawing9Image(bool bAutoStretch) = 0;
	
	// ���ص���xml����
	virtual bool LoadSkin(LPCTSTR lpszFile) = 0;
	virtual bool LoadSkin(const void* data, size_t data_len) = 0;
	virtual void SetTemplateFilter(ITemplateFilter* pFilter) = 0;

	// ��·���е���ͼƬ
	virtual ImageObj*  LoadImage(LPCTSTR lpszPath, bool bCache = true, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT) = 0;
	// ���ڴ��е���ͼƬ
	virtual ImageObj*  LoadImage(LPVOID lpData, long nSize, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT) = 0;
	// ����Դ�е���ͼƬ
	virtual ImageObj*  LoadImage(UINT uResID, LPCTSTR lpszType, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT) = 0;

	// ���ļ����ع��,�����ڴ洢���ļ��ϵĹ��
	virtual CursorObj* LoadCursor(LPCTSTR lpszPath) = 0;
	// ����id��ȡͼƬȫ·�����������ڴ洢���ļ��ϵ�ͼƬ
	virtual LPCTSTR GetImageFileFullPath(LPCTSTR lpszPath) = 0;

	// ������Դ���
	virtual void  AddFont(FontObj* pFontObj) = 0;
	virtual FontObj* GetFont(LPCTSTR lpszId) = 0;
	virtual FontObj* GetDefaultFont() = 0;

	// ��ɫ��Դ���
	virtual void AddColorObj(ColorObj* pColorObj) = 0;
	virtual ColorObj* GetColorObj(LPCTSTR lpszId) = 0;
	virtual ColorObj* GetDefaultColor() = 0;

	// ��ȡ�˵�����Ϣ
	virtual UIMENUINFO GetMenu( UINT uMenuID, int nIndex = -1, UINT uFlag = BY_POSITION ) = 0;

	// �����Դ���
	virtual void  AddCursor(CursorObj* pCursorObj) = 0;
	virtual CursorObj* GetCursor(LPCTSTR lpszId) = 0;
	virtual CursorObj* GetDefaultCursor() = 0;

	// �Ƴ������ͼƬ
	virtual void  RemoveImageFromTable(LPCTSTR lpszId) = 0;

	virtual LPCTSTR  GetUserData(LPCTSTR lpszId) = 0;
	virtual void  SetUserData(LPCTSTR lpszId, LPCTSTR lpszValue) = 0;

	// ��ȡģ��������Ϣ
	virtual int GetModuleCount() = 0;
	virtual LPCTSTR GetModulePath(int index) = 0;
	virtual LPCTSTR GetModuleName(int index) = 0;
};

class IUIEngine
{
public:
	// 1����ʼ�������
	virtual void InitSkin(bool bRTLayout = false) = 0;
	// 2���������ڣ�
	//new CWindowUI
	// 3����Ϣѭ��
	virtual int  MessageLoop() = 0;
	// 4�����ٽ����
	virtual void UnInitSkin() = 0;

	virtual void SetInstanceHandle(HINSTANCE hInstance) = 0;
	virtual HINSTANCE GetInstanceHandle()= 0;
	//���ô���ͼ��
	virtual void SetIconResource(UINT uId, bool bBigIcon) = 0;

	virtual void GetOSVersion(DWORD& dwOSVer, DWORD& dwOSMinorVer) = 0;
	virtual tstring  GetRunDir() = 0;

	virtual bool IsRTLLayout() = 0;

	// ���Ҵ���
	virtual int GetWindowCount() = 0;
	virtual CWindowUI* GetWindow(int nIndex) = 0;
	virtual CWindowUI* GetWindow(HWND hWnd) = 0;
	virtual CWindowUI* GetWindow(LPCTSTR lpszId) = 0;
	virtual CWindowUI* GetWindow(LPCTSTR lpszId, LPCTSTR lpszSubId) = 0;

	// ע��ؼ�
	virtual void RegisterControl(LPCTSTR lpszType, CONTROLCREATEPROC lfCreateFn) = 0;
	// ��xml����ӿؼ�
	virtual bool AddControl(CControlUI* pParent, CControlUI* pNext, LPCTSTR lpszId) = 0;

	virtual void RegisterPlugin(LPCTSTR guid, CONTROLCREATEPROC lfCreateFn) = 0;

	virtual void RegisterStyle(LPCTSTR lpszType, STYLECREATEPROC lfCreateFn) = 0;

	// ��Ϣѭ����غ���
	virtual bool PumpMessage() = 0;
	virtual bool TranslateMessage(const LPMSG pMsg) = 0;
	virtual void AddPreMessageFilter(IPreMessageFilterUI* pIMessageFilter) = 0;
	virtual void RemovePreMessageFilter(IPreMessageFilterUI* pIMessageFilter) = 0;

	virtual void SetPreNotify(INotifyUI* pNotify) = 0;

	// �ı�Ƥ������,lpszSkinImage������ͼƬ��lpSkinColor��������ɫ�����ΪNULL, �Զ��ӱ���ͼƬ��ȡƽ����ɫ��bAnimation���ı�Ƥ���Ƿ��ж���Ч��
	virtual void ChangeSkin(LPCTSTR lpszSkinImage, LPCOLORREF lpSkinColor = NULL, bool bAnimation = true) = 0;
	virtual LPCTSTR GetSkinImage() = 0;
	virtual DWORD GetSkinColor() = 0;

	// �ı�͸����
	virtual void SetAlpha(int nAlpha) = 0;
	virtual int  GetAlpha() = 0;

	// �û��Զ����Ǽ�
	virtual void RegisterUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule = NULL) = 0;
	virtual void UnRegisterUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule = NULL) = 0;
	virtual void UnRegisterUserEvent(UINT uEventID, HMODULE hModule = NULL) = 0;
	virtual void FireUserEvent(UINT uEventID, WPARAM wParam = NULL, LPARAM lParam = NULL, bool bImmediately = true, HMODULE hModule = NULL) = 0;

	// �ж�ָ���Ƿ���Ч�����ڻص��жϿؼ��Ƿ��Ѿ��鹹
	virtual bool CheckControlPtrValid(CControlUI* pControl) = 0;

	virtual  void EnableHDPI() = 0;
	virtual  bool IsEnableHDPI() = 0;
	virtual  int GetDPI() = 0;
	virtual  void SetDPI(int nDPI = 96, bool bAutoResizeWindow = true) = 0;
	virtual  CDPI* GetDPIObj() = 0;
	virtual  void SetDPIName(LPCTSTR lpszDPIName = _T("")) = 0;
	virtual  LPCTSTR GetDPIName() = 0;
	virtual  bool SetLogPath(LPCTSTR lpszPath) = 0;
};

///////////////////////////////////////////////////////////////////////////////////
// UICustomControlHelper��Ҫ���������û��Ի桰���Ͽؼ���ʱ��β��֡������ӿؼ�
/////////////////////////////////////////////////////////////////////////////////////
class DUI_API UICustomControlHelper
{
public:
	//{{
	struct LayoutParams
	{
		int nStartX_;
		int nStartY_;
		int nSpaceX_;
		int nSpaceY_;

		int nBegin_;
		int nEnd_;
		bool bEnableScroll_;
		LayoutParams()
		{
			nStartX_ = 0;
			nStartY_ = 0;
			nSpaceX_ = 0;
			nSpaceY_ = 0;
			nBegin_  = 0;
			nEnd_    = -1;
			bEnableScroll_ = true;
		}
	};
	//}}
public:
	/*!
	\brief    ͨ�õĻ����ӿؼ�
	\param    IRenderDC * pRenderDC ���ƶ���ָ��
	\param    RECT & rcPaint ����ˢ������
	\param    RECT & rcContainer �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\param    int nBegin ��ʼλ��
	\param    int nEnd ����λ��(-1Ϊ������)
	\param    bool bDrawLimit true����Ϊˢ������Ϳͻ����Ľ�����false����Ϊ�ͻ���
	\return   void 
	************************************/
	static void RenderChildControl(IRenderDC* pRenderDC, RECT& rcPaint, RECT& rcParentItem, RECT& rcParentClient, CStdPtrArray& arrChildList, int nBegin = 0, int nEnd = -1);
	static void RenderChildControl(IRenderDC* pRenderDC, RECT& rcPaint, RECT& rcParentItem, RECT& rcParentClient, CControlUI* pControlChild);

	/*!
	\brief    ͨ�õ���Ӻ���
	\param    CControlUI * pControl ��ӵĿؼ�ָ��
	\param    CControlUI * pParent ���ؼ�
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   bool �Ƿ�ɹ�
	************************************/
	static bool AddChildControl(CControlUI* pControlChild, CControlUI* pControlParent, CStdPtrArray& arrChildList);

	/*!
	\brief    ͨ�õĲ��뺯��
	\param    CControlUI * pControl ��ӵĿؼ�ָ��
	\param    CControlUI * pAfter ������ĳ���ؼ�֮��
	\param    CControlUI * pParent ���ؼ�
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   bool �Ƿ�ɹ�
	************************************/
	static bool InsertChildControl(CControlUI* pControlChild, CControlUI* pControlAfter, CControlUI* pParent, CStdPtrArray& arrChildList);

	/*!
	\brief    ͨ�õ�ɾ������
	\param    CControlUI * pControl Ҫɾ���Ŀؼ�ָ��
	\param    CControlUI * pParent ���ؼ�
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   bool �Ƿ�ɹ�
	************************************/
	static bool RemoveChildControl(CControlUI* pControlChild, CStdPtrArray& arrChildList);

	/*!
	\brief    ͨ�õ�ȫ��ɾ������
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   void 
	************************************/
	static void RemoveAllChild(CStdPtrArray& arrChildList);
	/*!
	\brief    ͨ�õĲ����ӿؼ�
	\param    RECT & rcWinow �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\param    bool bEnableScroll �Ƿ��п���������
	\return   SIZE 
	************************************/
	static SIZE LayoutChildNormal(RECT& rcWinow, RECT& rcClient, CStdPtrArray& arrChildList, bool bEnableScroll = true);

	/*!
	\brief    �������Ҳ���
	\note     ֻ��Կͻ����ڵĿؼ�
	\param    RECT & rcWinow �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    const LayoutParams & params ���ֲ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   SIZE �ӿؼ���������Ĵ�С
	************************************/
	static SIZE LayoutChildLeftToRight(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    �������󲼾�
	\note     ֻ��Կͻ����ڵĿؼ�
	\param    RECT & rcWinow �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    const LayoutParams & params ���ֲ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   SIZE �ӿؼ���������Ĵ�С
	************************************/
	static SIZE LayoutChildRightToLeft(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    �������²���
	\note     ֻ��Կͻ����ڵĿؼ�
	\param    RECT & rcWinow �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    const LayoutParams & params ���ֲ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   SIZE �ӿؼ���������Ĵ�С
	************************************/
	static SIZE LayoutChildTopToBottom(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    �������ϲ���
	\note     ֻ��Կͻ����ڵĿؼ�
	\param    RECT & rcWinow �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    const LayoutParams & params ���ֲ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   SIZE �ӿؼ���������Ĵ�С
	************************************/
	static SIZE LayoutChildBottomToTop(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    ����ƽ�ֲ���
	\note     �۳�δ�Զ����ֵĴ�С��ƽ��ʣ�µĿ��
	\param    RECT & rcWinow �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    const LayoutParams & params ���ֲ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   SIZE �ӿؼ���������Ĵ�С
	************************************/
	static SIZE LayoutChildHorz(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    ����ƽ�ֲ���
	\note     �۳�δ�Զ����ֵĴ�С��ƽ��ʣ�µĸ߶�
	\param    RECT & rcWinow �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    const LayoutParams & params ���ֲ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   SIZE �ӿؼ���������Ĵ�С
	************************************/
	static SIZE LayoutChildVert(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    ���ӻ�����
	\note     �ؼ���С�ɲ�һ��
	\param    RECT & rcWinow �ؼ�����
	\param    RECT & rcClient �ؼ��ͻ���
	\param    const LayoutParams & params ���ֲ���
	\param    CStdPtrArray & items �ӿؼ��洢
	\return   SIZE �ӿؼ���������Ĵ�С
	************************************/
	static SIZE LayoutChildGrid(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);


	/*!
	\brief    ͨ�õĲ��ҿؼ�����
	\param    CStdPtrArray & items �ӿؼ��洢
	\param    FINDCONTROLPROC Proc ���ҵ��жϺ���
	\param    LPVOID pData ��������
	\param    UINT uFlags ��ʶ(��ʶ�а���UIFIND_HITTESTʱ��pData����ΪLPPOINT)
	\return   CControlUI* ���ҽ��
	************************************/
	static CControlUI* FindChildControl(CStdPtrArray& items, FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags = UIFIND_ENABLED | UIFIND_MOUSE_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST, int nBegin = 0, int nEnd = -1);

	// ����¼�����
	static void AddEventDelegate(CControlUI* pControl, CDelegateBase& d);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ȫ�ֺ���
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//{{
extern "C"
{
//}}
	DUI_API IUIRes*		GetUIRes();
	DUI_API IUIEngine*  GetUIEngine();

	DUI_API LPCTSTR I18NSTR(LPCTSTR lpszStr, bool* bFind = NULL);
	DUI_API void	GetAnchorPos(UITYPE_ANCHOR nAnchor, const RECT* lprcParent, RECT* lprcChild);
	DUI_API bool	RtlRect(RECT & rcParent, LPRECT pRectChild);
	DUI_API UINT GET_DT_LEFT_EX_VALUE();
	DUI_API UINT GET_DT_RIGHT_EX_VALUE();
};
//}}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ԥ�������Ϣӳ��
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//{{
#define UIBEGIN_MSG_MAP virtual bool ProcessNotify(TNotifyUI* pNotify) { 
#define SUPER_HANDLER_MSG if (__super::ProcessNotify(pNotify)) return true;
#define UI_ID_HANDLER(id, func) if(pNotify->pSender && _tcsicmp(pNotify->pSender->GetId(), id) == 0 )\
{ \
	if (func(pNotify)) \
	return true; \
}

#define UI_EVENT_HANDLER(event, func) \
	if(pNotify->nType == event) \
{ \
	if (func(pNotify)) \
	return true; \
}

#define UI_EVENT_ID_HANDLER(event, id, func) if(pNotify->nType == event && pNotify->pSender && _tcsicmp(pNotify->pSender->GetId(), id) == 0 ) \
{ \
	if (func(pNotify)) \
	return true; \
}

#define UI_EVENT_MENU_ID_HANDLER(event, id, func) if(pNotify->nType == event && pNotify->lParam && (UINT)pNotify->lParam == id) \
{ \
	if (func(pNotify)) \
	return true; \
}

#define UI_TYPE_HANDLER(type, func) if(pNotify->pSender && pNotify->pSender->TypeOf(type)) \
{ \
	if (func(pNotify)) \
	return true; \
}

#define UI_EVENT_TYPE_HANDLER(event, type, func) if(pNotify->nType == event && pNotify->pSender && pNotify->pSender->TypeOf(type)) \
{ \
	if (func(pNotify)) \
	return true; \
}

#define UI_TYPE_ID_HANDLER(type, id, func) if(pNotify->pSender && pNotify->pSender->TypeOf(type) && _tcsicmp(pNotify->pSender->GetId(), id) == 0) \
{ \
	if (func(pNotify)) \
	return true; \
}

#define UIEND_MSG_MAP return false;} 


//�����ؼ�
#define UI_OBJECT_DECLARE(thisclass, class_name) \
	public:\
	static CControlUI* CreateObject()\
{\
	return new thisclass; \
}\
	static LPCTSTR GetStaticClassName()\
{ \
	return class_name;\
}\
	virtual LPCTSTR GetClassName()\
{\
	return class_name;\
}\


//�����ؼ�
#define UI_STYLE_DECLARE(thisclass, class_name) \
	public:\
	static StyleObj* CreateObjectStatic(CControlUI* pOwner)\
{\
	return new thisclass(pOwner); \
}\
	static LPCTSTR GetStaticClassName()\
{ \
	return class_name;\
}\
virtual StyleObj* CreateObject(CControlUI* pOwner)\
{\
return new thisclass(pOwner); \
}\
	
//�����ؼ�
#define UI_PLUGIN_DECLARE(thisclass, guid) \
	public:\
	static CControlUI* CreatePlugin()\
{\
	return new thisclass; \
}\
	static LPCTSTR GetStaticClassGuid()\
{ \
	return guid;\
}\




//ע��
#define UI_OBJECT_REGISTER(class_name)\
	GetUIEngine()->RegisterControl(class_name::GetStaticClassName(), &class_name::CreateObject);

#define UI_OBJECT_REGISTER_EX(text_name, class_name)\
	GetUIEngine()->RegisterControl(text_name, &class_name::CreateObject);

//ע��
#define UI_PLUGIN_REGISTER(class_name)\
	GetUIEngine()->RegisterPlugin(class_name::GetStaticClassGuid(), &class_name::CreatePlugin);

//ע��
#define UI_STYLE_REGISTER(class_name)\
	GetUIEngine()->RegisterStyle(class_name::GetStaticClassName(), &class_name::CreateObjectStatic);

#define UI_STYLE_REGISTER_EX(text_name, class_name)\
	GetUIEngine()->RegisterStyle(text_name, &class_name::CreateObjectStatic)


inline HMODULE ModuleHandleByAddr(const void* ptrAddr)
{
	MEMORY_BASIC_INFORMATION info;
	::VirtualQuery(ptrAddr, &info, sizeof(info));
	return (HMODULE)info.AllocationBase;
}

inline HMODULE ThisModuleHandle()
{
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);
	return sInstance;
}

// �߷ֱ���ת��
#define DPI_SCALE(v) GetUIEngine()->GetDPIObj()->Scale(v)
#define DPI_SCALE_BACK(v) GetUIEngine()->GetDPIObj()->ScaleBack(v)


#define CHECK_CONTROL_PTR_VALID(p) GetUIEngine()->CheckControlPtrValid(p)
#define ADD_CONTROL_PTR_TO_CHECK(p) p->ModifyFlags(UICONTROLFLAG_CHECK_CTR_PTR_VALID, 0);
#define REMOVE_CONTROL_PTR_TO_CHECK(p) 	p->ModifyFlags(0, UICONTROLFLAG_CHECK_CTR_PTR_VALID);

	

/////////////////////////////////////////////////////////////////////////////////////////////////
#define  IDR_MENU_UIEDIT			9191900
#define  IDR_MENU_UIEDIT_CUT		9191901
#define  IDR_MENU_UIEDIT_COPY		9191902
#define  IDR_MENU_UIEDIT_PASTE		9191903
#define  IDR_MENU_UIEDIT_SELALL		9191904
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//}}
