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
    \brief    资源类型
*****************************************/
typedef enum
{
	UIRES_FROM_FILE = 0,  /**< 从文件中加载 */
	UIRES_FROM_RESOURCE,  /**< 从资源中加载 */
	UIRES_FROM_RDB,       /**< 从RDB中加载 */
} UITYPE_RES;

/*!
    \brief    字体引擎
*****************************************/
typedef enum
{
	UIFONT_GDI = 0,    /**< GDI */
	UIFONT_GDIPLUS,    /**< GDI+ */
	UIFONT_DEFAULT,    
} UITYPE_FONT;

/*!
    \brief    字体特效
*****************************************/
typedef enum
{
	UIFONTEFFECT_NORMAL = 0,
	UIFONTEFFECT_GLOW_AUTO,           //发光特效 
	UIFONTEFFECT_GLOW_GOLD,
	UIFONTEFFECT_GLOW_BLACK,
} UITYPE_FONTEFFECT;

// 图片类型
typedef enum
{
	UIIAMGE_HBITMAP = 0, // 位图格式
	UIIAMGE_BITMAP,      // GDI+格式
	UIIAMGE_PIXEL,       // 像素格式
	UIIAMGE_HICON,       // ICON格式
	UIIAMGE_DEFAULT
} UITYPE_IMAGE;

// 绘图效率
typedef enum
{
	 UISMOOTHING_HighSpeed = 0, //高速度
	 UISMOOTHING_HighQuality, //高质量
} UITYPE_SMOOTHING;

// 图片拉伸类型
typedef enum
{
	UIPAINTMODE_NORMAL = 0, //不拉伸 normal
	UIPAINTMODE_STRETCH,    //拉伸绘制 fill
	UIPAINTMODE_TILE,		//平铺 tile
	UIPAINTMODE_XTILE,		//X平铺 xtile
	UIPAINTMODE_YTILE,		//Y平铺 ytile
	UIPAINTMODE_CENTER,     //居中绘制 center
	UIPAINTMODE_FIT,		//原比例充满 fit	
	UIPAINTMODE_CENTERCROP, //最大填满，多余的切割 centercrop
} UITYPE_PAINTMODE;

// 控件锚点类型
typedef enum
{
	UIANCHOR_LT = 9, //左上角, LT|x,y,width,height
	UIANCHOR_RT = 10,    //右上角, RT|x,y,width,height
	UIANCHOR_RB = 11,  //右下角,RB|x,y,width,height
	UIANCHOR_LB = 12,    //左下角,LB|x,y,width,height

	UIANCHOR_TOP = 13,   //上边 TOP|x1,y,x2,height
	UIANCHOR_RIGHT = 14,  //右边 RIGHT|x,y1,width,y2
	UIANCHOR_BOTTOM = 15,  //下边 BOTTOM|x1,y,x2,height
	UIANCHOR_LEFT = 16,   //左边 LEFT|x,y1,width,y2

	UIANCHOR_LTRB = 8,  //对角线 LTRB|x1,y1,x2,y2

	// 特别注意:以下几种方式是旧的坐标方式，由于很难理解，最好不要用，只是为了兼容旧代码，只要学习上面几种方式就可以了
	UIANCHOR_LT_OLD = 0, //左上角  0|x1,y1,X2,y2 
	UIANCHOR_RT_OLD = 1,    //右上角 1|x1,y1,X2,y2
	UIANCHOR_RB_OLD = 2,  //右下角 2|x1,y1,X2,y2
	UIANCHOR_LB_OLD = 3,    //左下角 3|x1,y1,X2,y2

	UIANCHOR_TOP_OLD = 4,   //上边 4|x1,y1,X2,y2 或 LTRT|x1,y1,X2,y2
	UIANCHOR_RIGHT_OLD = 5,  //右边 5|x1,y1,X2,y2 或 RTRB|x1,y1,X2,y2
	UIANCHOR_BOTTOM_OLD = 6, //下边 6|x1,y1,X2,y2 或 LBRB|x1,y1,X2,y2
	UIANCHOR_LEFT_OLD = 7,   //左边 7|x1,y1,X2,y2 或  LTLB|x1,y1,X2,y2
	//特别注意:0-4由于很难理解，最好不要用，可能会被废弃
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

// 控件布局类型
typedef enum
{
	UILAYOUT_NORMAL=0,   // 正常状态，不自动排列
	UILAYOUT_LEFTTORIGHT,// 从左向右排列
	UILAYOUT_RIGHTTOLEFT ,// 从右向左排列
	UILAYOUT_TOPTOBOTTOM ,//从上到下
	UILAYOUT_BOTTOMTOTOP ,//从下到上
	UILAYOUT_HORZFILL , //水平平均充满
	UILAYOUT_VERTFILL ,//垂直平均充满
	UILAYOUT_GRID ,    //网格
}UITYPE_LAYOUT;

// 控件状态
typedef enum
{
	UISTATE_NORMAL = 0, //正常
	UISTATE_OVER,     //鼠标经过
	UISTATE_DOWN,     //鼠标按下 
	UISTATE_FOCUS,    //获取光标（这种状态很少用，例如：编辑框会用到【当鼠标不在编辑框，但光标在编辑框时】）
	UISTATE_DISABLE,  //禁止状态

	// 以下五种状态和上面是一一对应的，checkbox经常用到【在选中状态下，控件对应的五种状态】
	UISTATE_CKNORMAL = 8, //checked, normal 
	UISTATE_CKOVER,//checked, mouse is hover 
	UISTATE_CKDOWN,//checked, the left button of mouse is press 
	UISTATE_CKFOCUS,//checked, the control get the focus of keyboard 
	UISTATE_CKDISABLE,//checked, disable 

}UITYPE_STATE;

// 外部事件
typedef enum
{
	UINOTIFY__FIRST,
	// 通用事件
	UINOTIFY_CLICK,
	UINOTIFY_CLICK_LINK,	//wParam:index of link; lParam:link; //for CHtmlLabelUI
	UINOTIFY_RCLICK,
	UINOTIFY_DBCLICK,
	UINOTIFY_SELCHANGE,
	UINOTIFY_SETFOCUS,
	UINOTIFY_KILLFOCUS,
	UINOTIFY_DROPFILES,		//wParam:HDROP; lParam:NULL;
	// 编辑框事件
	UINOTIFY_EN_CHANGE,
	UINOTIFY_EN_IMESTART,
	UINOTIFY_EN_IMEEND,
	UINOTIFY_EN_IMEING,
	UINOTIFY_EN_LIMIT,		//wParam:NULL; lParam:bool pointer(值为true时使用默认提示);
	UINOTIFY_EN_KEYDOWN,	//wParam:virtual-key code;
	UINOTIFY_EN_LISTCLICK,
	UINOTIFY_EN_LISTRETURN,
	//层级结构节点
	UINOTIFY_TVN_ITEMEXPENDED,	//wParam:bExpand; lParam:展开的控件指针;
	UINOTIFY_TVN_ITEMEXPENDING,	//wParam:bExpand; lParam:展开的控件指针;
	// 树形控件事件
	UINOTIFY_TVN_BEGINDRAG,	//wParam:NULL; lParam:拖曳的控件指针;
	UINOTIFY_TVN_ENDDRAG,	//wParam:拖曳目标控件指针; lParam:拖曳的控件指针;
	UINOTIFY_TVN_CHECK,		//wParam:bCheck; lParam:控件指针;
	UINOTIFY_TVN_KEYDOWN,	//wParam、lParam同WM_KEYDOWN
	// 列表控件
	UINOTIFY_LB_KEYDOWN,	//wParam、lParam同WM_KEYDOWN
	UINOTIFY_LB_AREASELECT_ENDED,
	UINOTIFY_LB_AREASELECT,
	// Slider事件
	UINOTIFY_TRBN_POSCHANGE,
	UINOTIFY_TRBN_SELCHANGE,
	UINOTIFY_TRBN_BEGINCHANGE,
	UINOTIFY_TRBN_ENDCHANGE,
	// richedit控件事件 
	UINOTIFY_RE_DBCLICK_IMAGE,
	UINOTIFY_RE_CLICK_IMAGE_THUMBNAIL_BTN,
	UINOTIFY_RE_CLICK_LINK,
	// ActiveX控件
	UINOTIFY_ACTIVEX_CREATE,
	UINOTIFY_ACTIVEX_SHOWCOMPLETE,		//wParam:NULL; lParam:URL;
	UINOTIFY_ACTIVEX_NAVIGATE_BEFORE,	//wParam:VARIANT_BOOL Pointer(是否取消); lParam:URL;
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
	//热键
	UINOTIFY_HOTKEY, 

	// 快捷菜单
	UINOTIFY_GET_CONTEXTMENU,

	// 拖拉事件
	UINOTIFY_DRAG_DROP_GROUPS = UINOTIFY__FIRST + 900, // 拖放事件集合
		//从“外面”拖数据到“控件”
		UINOTIFY_DRAG_ENTER ,// 进入 
		UINOTIFY_DRAG_OVER,  // 经过 
		UINOTIFY_DRAG_LEAVE, // 离开
		UINOTIFY_DROP,       // 放下
		//从“控件”拖数据到“外面”
		UINOTIFY_BEGIN_DRAG, //开始拖拉控件
		UINOTIFY_DRAG_QUERYDATA_DELAY,

	// 日历控件
	UINOTIFY_DATE_CHANGE,

	UINOTIFY__LAST = UINOTIFY__FIRST + 1000,
	UINOTIFY__USER,	/// 其他自定义控件消息的起点.
	UINOTIFY__USER_LAST = UINOTIFY__USER + 1000,
	// 皮肤事件

}UITYPE_NOTIFY;

// 内部事件
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
	UIEVENT_MOUSEENTERRECT,   //与UIEVENT_MOUSEENTER 区别在于,UIEVENT_MOUSEENTER在先进入子控件时不会触发，而UIEVENT_MOUSEENTERRECT会
	UIEVENT_MOUSELEAVERECT,   //与UIEVENT_MOUSEENTER 区别在于,UIEVENT_MOUSELEAVE在后进入子控件时会触发，UIEVENT_MOUSEENTERR
  
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

	UIEVENT_DESTORY, //控件虚构
	UIEVENT_ADD_CHILD, //添加子控件
	// 拖放 begin
	UIEVENT_DRAG_DROP_GROUPS = UINOTIFY_DRAG_DROP_GROUPS, // 拖放事件集合
		UIEVENT_DRAG_ENTER,
		UIEVENT_DRAG_OVER,
		UIEVENT_DRAG_LEAVE,
		UIEVENT_DROP,
		UIEVENT_BEGIN_DRAG,
		UIEVENT_END_DRAG,
		UIEVENT_DRAG_QUERYDATA_DELAY,
	// 拖放 end

	UIEVENT_ITEM_MOUSEHOVER_CHANGE, //wParam:旧高亮控件指针; lParam:新高亮控件指针;
	UIEVENT_ITEM_SEL_CHANGE, //wParam:旧选择控件指针; lParam:新选择控件指针;
	// 用来处理有窗口句柄的控件
	UIEVENT_WINDOW_PROC,

	UIEVENT_DPI_CHANGED,

	UIEVENT_GESTURENOTIFY, // 手势通知可以通过该消息取消手势
	UIEVENT_GESTURE, // 手势消息
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
#define UICONTROLFLAG_SETFOCUS      0x00000008 //能够设置光标
#define UICONTROLFLAG_ENABLE_DROP   0x00000010 
#define UICONTROLFLAG_ENABLE_DRAG   0x00000020
#define UICONTROLFLAG_WANTESCAPE    0x00000040
#define UICONTROLFLAG_ENABLE_AREAS  0x00000080
#define UICONTROLFLAG_ENABLE_DESTORY_EVENT  0x00000100
#define UICONTROLFLAG_CHECK_CTR_PTR_VALID  0x00000200 //可以判断指针是否有效

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

// Flags // 以下这些是给树形控件和列表控件使用的，改造完会去掉，这些很不符合规范
#define UIITEMFLAG_VISIBLE		0x00000001
#define UIITEMFLAG_ENABLED		0x00000002
#define UIITEMFLAG_CHECKED		0x00000004
#define UIITEMFLAG_ALL          0x00000010
#define UIITEMFLAG_NOTIFY		0x00000040

//时钟ID
#define DUI_TIMERID_BASE 0x1000
#define DUI_TIMERID_USER 0x2000
//}}

/////////////////////////////////////////////////////////////////////////////////////
//对内/对外事件通知结构
typedef struct tagTEventUI
{
	int nType;
	CControlUI* pSender;
	POINT	ptMouse;
	WPARAM	wParam;
	LPARAM	lParam;
} TEventUI, TNotifyUI;

//用户自定义是件
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


#define BY_COMMAND 			0			/**< 插入菜单位置指定方式为菜单项ID */
#define BY_POSITION 		1			/**< 插入菜单位置指定方式为菜单序号 */
//{{

typedef CControlUI* (__cdecl* CONTROLCREATEPROC)();
typedef CControlUI* (_stdcall* FINDCONTROLPROC)(CControlUI*, LPVOID);
typedef StyleObj* (__cdecl* STYLECREATEPROC)(CControlUI*);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 全局接口
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

//消息过滤
class IMessageFilterUI
{
public:
	virtual LRESULT MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

//消息过滤
class IPreMessageFilterUI
{
public:
	virtual bool PreMessageFilter(const LPMSG pMsg) = 0;
};

//xml解析过滤
class ITemplateFilter
{
public:
	virtual ~ITemplateFilter(){};
	//返回false 则忽略该style或者window，默认返回true
	virtual bool OnTemplateAttributes(LPCSTR lpcsName, LPCSTR lpcsValue) = 0;
};


//消息过滤
class IUIControlDestory
{
public:
	virtual void OnControlDestory(CControlUI* pControl) = 0;
};


class IUIRes
{
public:
	// 设置默认是否缓存XML,如果不缓存实时读取，这样改变配置程序重新启动就能反应出来，但效率较差，一般Debug默认为false,Release默认为true
	virtual void SetCacheXML(bool bCacheXML) = 0;

	// 设置界面语言,如ENG，中文（具体字符格式由程序自己定，只需要和xml配置保持一致）
	virtual void SetLanguage(LPCTSTR lpszLanguage) = 0;
	virtual LPCTSTR GetLanguage() = 0;

	// 该函数的目的是：同一个id可以对应不同图片，既可用于不同语言版本，也可用于不同界面皮肤
	virtual void SetImageLanguage(LPCTSTR lpszLanguage) = 0;

	// 设置默认字体名字，如宋体
	virtual void SetDefaultFontName(LPCTSTR lpszFontName) = 0;
	virtual LPCTSTR GetDefaultFontName() = 0;

	// 设置默认字体绘制引擎，GDI或GDI+引擎，未来将引入FreeType引擎
	virtual void SetDefaultFontType(UITYPE_FONT eFontType) = 0;
	virtual UITYPE_FONT GetDefaultFontType()= 0;

	virtual void SetDefaultTextRenderingHint(int textRenderingHint = 0) = 0;
	virtual int GetDefaultTextRenderingHint() = 0;

	// 设置默认的图片格式，可以使用pixel（使用汇编绘图，效率高且不生成GDI对象）、hbitmap格式（使用汇编绘图，效率高）、bitmap格式(使用gdi++绘图，质量高，程序中少数控件需要高质量绘图可使用）
	virtual void SetDefaultImageType(UITYPE_IMAGE eImageType) = 0;

	// 设置默认绘制质量
	virtual void SetDefaultImageQuality(UITYPE_SMOOTHING eImageQualityType) = 0;

	// 从路径中加载资源
	virtual void InitResDir(LPCTSTR lpszPath, bool bDefaultDir = false) = 0;
	// 从RDB中加载资源
	virtual void InitResDirFromRdb(LPCTSTR lpszPath, LPCTSTR lpszName, bool bDefaultDir = false) = 0;
	virtual void InitResDirFromRdb(LPBYTE lpData, DWORD dwSize, LPCTSTR lpszName, bool bDefaultDir = false) = 0;
	// 从RES加载资源
	virtual void InitResDirFromRes(UINT uResID, LPCTSTR lpszType) = 0;

	// 设置主题目录
	virtual void SetThemePath(LPCTSTR lpszPath) = 0;
	//bCoexistence = true 与SetThemePath方式共存
	virtual void SetThemePathFromRDB(LPCTSTR lpszPath, bool bCoexistence = false) = 0;
	virtual CRdbParser*  GetThemeRdbFile() = 0;
	virtual LPCTSTR GetThemePath() = 0;

	// 设置HDPI图片路径
	virtual void SetHDPIImagePath(LPCTSTR lpszPath) = 0;
	virtual void SetHDPIImagePathRDB(LPCTSTR lpszPath) = 0;
	virtual CRdbParser*  GetHDPIImageRdbFile() = 0;

	virtual void SetHDPI9ImageParamScale(float fScale) = 0;
	virtual void SetHDPIAutoStretchOnDrawing9Image(bool bAutoStretch) = 0;
	
	// 加载单独xml配置
	virtual bool LoadSkin(LPCTSTR lpszFile) = 0;
	virtual bool LoadSkin(const void* data, size_t data_len) = 0;
	virtual void SetTemplateFilter(ITemplateFilter* pFilter) = 0;

	// 从路径中导入图片
	virtual ImageObj*  LoadImage(LPCTSTR lpszPath, bool bCache = true, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT) = 0;
	// 从内存中导入图片
	virtual ImageObj*  LoadImage(LPVOID lpData, long nSize, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT) = 0;
	// 从资源中导入图片
	virtual ImageObj*  LoadImage(UINT uResID, LPCTSTR lpszType, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT) = 0;

	// 从文件加载光标,仅用于存储在文件上的光标
	virtual CursorObj* LoadCursor(LPCTSTR lpszPath) = 0;
	// 根据id获取图片全路径名，仅用于存储在文件上的图片
	virtual LPCTSTR GetImageFileFullPath(LPCTSTR lpszPath) = 0;

	// 字体资源相关
	virtual void  AddFont(FontObj* pFontObj) = 0;
	virtual FontObj* GetFont(LPCTSTR lpszId) = 0;
	virtual FontObj* GetDefaultFont() = 0;

	// 颜色资源相关
	virtual void AddColorObj(ColorObj* pColorObj) = 0;
	virtual ColorObj* GetColorObj(LPCTSTR lpszId) = 0;
	virtual ColorObj* GetDefaultColor() = 0;

	// 获取菜单项信息
	virtual UIMENUINFO GetMenu( UINT uMenuID, int nIndex = -1, UINT uFlag = BY_POSITION ) = 0;

	// 鼠标资源相关
	virtual void  AddCursor(CursorObj* pCursorObj) = 0;
	virtual CursorObj* GetCursor(LPCTSTR lpszId) = 0;
	virtual CursorObj* GetDefaultCursor() = 0;

	// 移除缓存的图片
	virtual void  RemoveImageFromTable(LPCTSTR lpszId) = 0;

	virtual LPCTSTR  GetUserData(LPCTSTR lpszId) = 0;
	virtual void  SetUserData(LPCTSTR lpszId, LPCTSTR lpszValue) = 0;

	// 获取模块的相关信息
	virtual int GetModuleCount() = 0;
	virtual LPCTSTR GetModulePath(int index) = 0;
	virtual LPCTSTR GetModuleName(int index) = 0;
};

class IUIEngine
{
public:
	// 1、初始化界面库
	virtual void InitSkin(bool bRTLayout = false) = 0;
	// 2、创建窗口，
	//new CWindowUI
	// 3、消息循环
	virtual int  MessageLoop() = 0;
	// 4、销毁界面库
	virtual void UnInitSkin() = 0;

	virtual void SetInstanceHandle(HINSTANCE hInstance) = 0;
	virtual HINSTANCE GetInstanceHandle()= 0;
	//设置窗口图标
	virtual void SetIconResource(UINT uId, bool bBigIcon) = 0;

	virtual void GetOSVersion(DWORD& dwOSVer, DWORD& dwOSMinorVer) = 0;
	virtual tstring  GetRunDir() = 0;

	virtual bool IsRTLLayout() = 0;

	// 查找窗口
	virtual int GetWindowCount() = 0;
	virtual CWindowUI* GetWindow(int nIndex) = 0;
	virtual CWindowUI* GetWindow(HWND hWnd) = 0;
	virtual CWindowUI* GetWindow(LPCTSTR lpszId) = 0;
	virtual CWindowUI* GetWindow(LPCTSTR lpszId, LPCTSTR lpszSubId) = 0;

	// 注册控件
	virtual void RegisterControl(LPCTSTR lpszType, CONTROLCREATEPROC lfCreateFn) = 0;
	// 从xml中添加控件
	virtual bool AddControl(CControlUI* pParent, CControlUI* pNext, LPCTSTR lpszId) = 0;

	virtual void RegisterPlugin(LPCTSTR guid, CONTROLCREATEPROC lfCreateFn) = 0;

	virtual void RegisterStyle(LPCTSTR lpszType, STYLECREATEPROC lfCreateFn) = 0;

	// 消息循环相关函数
	virtual bool PumpMessage() = 0;
	virtual bool TranslateMessage(const LPMSG pMsg) = 0;
	virtual void AddPreMessageFilter(IPreMessageFilterUI* pIMessageFilter) = 0;
	virtual void RemovePreMessageFilter(IPreMessageFilterUI* pIMessageFilter) = 0;

	virtual void SetPreNotify(INotifyUI* pNotify) = 0;

	// 改变皮肤背景,lpszSkinImage：背景图片；lpSkinColor：背景颜色，如果为NULL, 自动从背景图片获取平均颜色；bAnimation：改变皮肤是否有动画效果
	virtual void ChangeSkin(LPCTSTR lpszSkinImage, LPCOLORREF lpSkinColor = NULL, bool bAnimation = true) = 0;
	virtual LPCTSTR GetSkinImage() = 0;
	virtual DWORD GetSkinColor() = 0;

	// 改变透明度
	virtual void SetAlpha(int nAlpha) = 0;
	virtual int  GetAlpha() = 0;

	// 用户自定义是件
	virtual void RegisterUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule = NULL) = 0;
	virtual void UnRegisterUserEvent(UINT uEventID, CDelegateBase& d, HMODULE hModule = NULL) = 0;
	virtual void UnRegisterUserEvent(UINT uEventID, HMODULE hModule = NULL) = 0;
	virtual void FireUserEvent(UINT uEventID, WPARAM wParam = NULL, LPARAM lParam = NULL, bool bImmediately = true, HMODULE hModule = NULL) = 0;

	// 判断指针是否有效，用于回调判断控件是否已经虚构
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
// UICustomControlHelper主要用来帮助用户自绘“复合控件”时如何布局、绘制子控件
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
	\brief    通用的绘制子控件
	\param    IRenderDC * pRenderDC 绘制对象指针
	\param    RECT & rcPaint 本次刷新区域
	\param    RECT & rcContainer 控件区域
	\param    RECT & rcClient 控件客户区
	\param    CStdPtrArray & items 子控件存储
	\param    int nBegin 起始位置
	\param    int nEnd 结束位置(-1为至结束)
	\param    bool bDrawLimit true限制为刷新区域和客户区的交集，false限制为客户区
	\return   void 
	************************************/
	static void RenderChildControl(IRenderDC* pRenderDC, RECT& rcPaint, RECT& rcParentItem, RECT& rcParentClient, CStdPtrArray& arrChildList, int nBegin = 0, int nEnd = -1);
	static void RenderChildControl(IRenderDC* pRenderDC, RECT& rcPaint, RECT& rcParentItem, RECT& rcParentClient, CControlUI* pControlChild);

	/*!
	\brief    通用的添加函数
	\param    CControlUI * pControl 添加的控件指针
	\param    CControlUI * pParent 父控件
	\param    CStdPtrArray & items 子控件存储
	\return   bool 是否成功
	************************************/
	static bool AddChildControl(CControlUI* pControlChild, CControlUI* pControlParent, CStdPtrArray& arrChildList);

	/*!
	\brief    通用的插入函数
	\param    CControlUI * pControl 添加的控件指针
	\param    CControlUI * pAfter 插入至某个控件之后
	\param    CControlUI * pParent 父控件
	\param    CStdPtrArray & items 子控件存储
	\return   bool 是否成功
	************************************/
	static bool InsertChildControl(CControlUI* pControlChild, CControlUI* pControlAfter, CControlUI* pParent, CStdPtrArray& arrChildList);

	/*!
	\brief    通用的删除函数
	\param    CControlUI * pControl 要删除的控件指针
	\param    CControlUI * pParent 父控件
	\param    CStdPtrArray & items 子控件存储
	\return   bool 是否成功
	************************************/
	static bool RemoveChildControl(CControlUI* pControlChild, CStdPtrArray& arrChildList);

	/*!
	\brief    通用的全部删除函数
	\param    CStdPtrArray & items 子控件存储
	\return   void 
	************************************/
	static void RemoveAllChild(CStdPtrArray& arrChildList);
	/*!
	\brief    通用的布局子控件
	\param    RECT & rcWinow 控件区域
	\param    RECT & rcClient 控件客户区
	\param    CStdPtrArray & items 子控件存储
	\param    bool bEnableScroll 是否有开启滚动条
	\return   SIZE 
	************************************/
	static SIZE LayoutChildNormal(RECT& rcWinow, RECT& rcClient, CStdPtrArray& arrChildList, bool bEnableScroll = true);

	/*!
	\brief    从左往右布局
	\note     只针对客户区内的控件
	\param    RECT & rcWinow 控件区域
	\param    RECT & rcClient 控件客户区
	\param    const LayoutParams & params 布局参数
	\param    CStdPtrArray & items 子控件存储
	\return   SIZE 子控件布局所需的大小
	************************************/
	static SIZE LayoutChildLeftToRight(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    从右往左布局
	\note     只针对客户区内的控件
	\param    RECT & rcWinow 控件区域
	\param    RECT & rcClient 控件客户区
	\param    const LayoutParams & params 布局参数
	\param    CStdPtrArray & items 子控件存储
	\return   SIZE 子控件布局所需的大小
	************************************/
	static SIZE LayoutChildRightToLeft(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    从上至下布局
	\note     只针对客户区内的控件
	\param    RECT & rcWinow 控件区域
	\param    RECT & rcClient 控件客户区
	\param    const LayoutParams & params 布局参数
	\param    CStdPtrArray & items 子控件存储
	\return   SIZE 子控件布局所需的大小
	************************************/
	static SIZE LayoutChildTopToBottom(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    由下至上布局
	\note     只针对客户区内的控件
	\param    RECT & rcWinow 控件区域
	\param    RECT & rcClient 控件客户区
	\param    const LayoutParams & params 布局参数
	\param    CStdPtrArray & items 子控件存储
	\return   SIZE 子控件布局所需的大小
	************************************/
	static SIZE LayoutChildBottomToTop(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    横向平分布局
	\note     扣除未自动布局的大小，平分剩下的宽度
	\param    RECT & rcWinow 控件区域
	\param    RECT & rcClient 控件客户区
	\param    const LayoutParams & params 布局参数
	\param    CStdPtrArray & items 子控件存储
	\return   SIZE 子控件布局所需的大小
	************************************/
	static SIZE LayoutChildHorz(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    纵向平分布局
	\note     扣除未自动布局的大小，平分剩下的高度
	\param    RECT & rcWinow 控件区域
	\param    RECT & rcClient 控件客户区
	\param    const LayoutParams & params 布局参数
	\param    CStdPtrArray & items 子控件存储
	\return   SIZE 子控件布局所需的大小
	************************************/
	static SIZE LayoutChildVert(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);

	/*!
	\brief    格子化布局
	\note     控件大小可不一致
	\param    RECT & rcWinow 控件区域
	\param    RECT & rcClient 控件客户区
	\param    const LayoutParams & params 布局参数
	\param    CStdPtrArray & items 子控件存储
	\return   SIZE 子控件布局所需的大小
	************************************/
	static SIZE LayoutChildGrid(RECT& rcWinow, RECT& rcClient, const LayoutParams& params, CStdPtrArray& arrChildList);


	/*!
	\brief    通用的查找控件函数
	\param    CStdPtrArray & items 子控件存储
	\param    FINDCONTROLPROC Proc 查找的判断函数
	\param    LPVOID pData 函数参数
	\param    UINT uFlags 标识(标识中包含UIFIND_HITTEST时，pData必须为LPPOINT)
	\return   CControlUI* 查找结果
	************************************/
	static CControlUI* FindChildControl(CStdPtrArray& items, FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags = UIFIND_ENABLED | UIFIND_MOUSE_ENABLED | UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST, int nBegin = 0, int nEnd = -1);

	// 添加事件代理
	static void AddEventDelegate(CControlUI* pControl, CDelegateBase& d);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 全局函数
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
// 预定义和消息映射
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


//声明控件
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


//声明控件
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
	
//声明控件
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




//注册
#define UI_OBJECT_REGISTER(class_name)\
	GetUIEngine()->RegisterControl(class_name::GetStaticClassName(), &class_name::CreateObject);

#define UI_OBJECT_REGISTER_EX(text_name, class_name)\
	GetUIEngine()->RegisterControl(text_name, &class_name::CreateObject);

//注册
#define UI_PLUGIN_REGISTER(class_name)\
	GetUIEngine()->RegisterPlugin(class_name::GetStaticClassGuid(), &class_name::CreatePlugin);

//注册
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

// 高分辨屏转换
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
