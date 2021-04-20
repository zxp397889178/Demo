/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2010-09-27       xqb
*********************************************************************/
#pragma once

#include <Richedit.h>
#include <richole.h>
#include <TextServ.h>
#include "TextHostImpl.h"

class CRichEditUI;
/**文本格式
*/
//{{
typedef struct tagTextFormat
{
	DWORD dwMask;//!<字体掩码
	DWORD dwEffects;//!<字体效果（粗体、斜体、下划线、删除线等）
	DWORD dwFontSize;//!<尺寸（以POINT为单位）
	COLORREF crTextColor;//!<字体颜色
	TCHAR  szFaceName[32];//!<字体名称
	LCID   lcid;
    // Locale ID
}TEXTFORMAT;
//}}


#define  LINK_URL 0
//{{
class DUI_API CLinkObj
{
public:
	CLinkObj();
	virtual ~CLinkObj();

public:
	DWORD m_dwType; //类型 LINK_TYPE
	tstring m_strShowTxt;//显示的内容
	tstring m_strLinkTxt;//操作的内容
	tstring m_strId;	/// 标识符
	tstring m_strReverse;	/// 保留
	LONG	m_cpMin;
	LONG	m_cpMax;
};

class DUI_API ReTextObj
{
	friend class CRichEditUI;
public:
	ReTextObj();
	virtual ~ReTextObj();

public:
	void SetText(LPCTSTR lpszText);
	LPCTSTR GetText();
	void GetRange(LONG& cpMin, LONG& cpMax);
private:
	tstring m_strText;//显示的内容
	LONG	m_cpMin;
	LONG	m_cpMax;
	bool    m_bIsUpdating;
};

//}}


class DUI_API COleObject: public IOleObject, public IViewObject2
{
	friend class CRichEditUI;
public:
	COleObject();
	virtual ~COleObject();

public:
	CRichEditUI* GetRichEdit();
	DWORD GetId();

	long GetCharPos();
	RECT GetBound();

	SIZE GetSize();
	void FireViewChange();

	virtual void LoadResource();
	virtual void ReleaseResource();

	virtual void SetSize(int nWidth,int nHeight);

protected:
	// IOleObject接口
	virtual HRESULT  __stdcall  QueryInterface(REFIID iid, void ** ppvObject);
	virtual ULONG  __stdcall  AddRef();
	virtual ULONG  __stdcall  Release();
	virtual HRESULT  __stdcall SetClientSite( IOleClientSite *pClientSite);
	virtual HRESULT  __stdcall GetClientSite( IOleClientSite **ppClientSite);
	virtual HRESULT  __stdcall SetHostNames( LPCOLESTR szContainerApp,  LPCOLESTR szContainerObj);
	virtual HRESULT  __stdcall Close( DWORD dwSaveOption);
	virtual HRESULT  __stdcall SetMoniker( DWORD dwWhichMoniker, IMoniker *pmk);
	virtual HRESULT  __stdcall GetMoniker( DWORD dwAssign,  DWORD dwWhichMoniker,IMoniker **ppmk);
	virtual HRESULT  __stdcall InitFromData( IDataObject *pDataObject, BOOL fCreation,  DWORD dwReserved);
	virtual HRESULT  __stdcall GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject);
	virtual HRESULT  __stdcall DoVerb(LONG iVerb, LPMSG lpmsg,  IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect);
	virtual HRESULT  __stdcall EnumVerbs(IEnumOLEVERB **ppEnumOleVerb);
	virtual HRESULT  __stdcall Update();
	virtual HRESULT  __stdcall IsUpToDate();
	virtual HRESULT  __stdcall GetUserClassID(CLSID *pClsid);
	virtual HRESULT  __stdcall GetUserType(DWORD dwFormOfType,  LPOLESTR *pszUserType);
	virtual HRESULT __stdcall  SetExtent(DWORD dwDrawAspect, SIZEL  *psizel);
	virtual HRESULT  __stdcall GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
	virtual HRESULT  __stdcall Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection   );
	virtual HRESULT  __stdcall Unadvise(DWORD dwConnection );
	virtual HRESULT  __stdcall EnumAdvise(IEnumSTATDATA **ppenumAdvise);
	virtual HRESULT  __stdcall GetMiscStatus( DWORD dwAspect, DWORD *pdwStatus );
	virtual HRESULT  __stdcall SetColorScheme(LOGPALETTE *pLogpal);

	// IViewObject接口
	virtual HRESULT WINAPI Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
		HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
		BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue);
	virtual HRESULT WINAPI GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
		DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet);
	virtual HRESULT WINAPI Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze);
	virtual HRESULT WINAPI Unfreeze(DWORD dwFreeze);
	virtual HRESULT WINAPI SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink);
	virtual HRESULT WINAPI GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink);

	// IViewObject2接口
	virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel);
	virtual HRESULT WINAPI GetRect(LPRECT lpRect);

	// 自定义接口事件
	virtual void OnMouseEnter(TEventUI& event);
	virtual void OnMouseLeave(TEventUI& event);
	virtual void OnMouseMove(TEventUI& event);
	virtual void OnClick(TEventUI& event);
	virtual void OnRClick(TEventUI& event);
	virtual void OnDbClick(TEventUI& event);
	virtual void OnRichEditSizeChanged(RECT& rectParent);
	virtual void OnPositonChanged(LPCRECT lpRect);

	IRenderDC*	 GetRenderDC();

	bool IsReLayoutOle();
private:
	void SetOwner(CRichEditUI* pOwner);
	void SetId(DWORD dwId);
	LONG m_dwRef;
	IOleClientSite *m_pOleClientSite;
	IAdviseSink* m_spAdviseSink;
	RECT m_rcBounds;
	CRichEditUI* m_pOwner;
	SIZE  m_sizeExtent;
	DWORD m_dwId;
	//}}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
class DUI_API COleImageObj:public COleObject
{
	//{{
	friend class CRichEditUI;
public:
	COleImageObj();
	virtual ~COleImageObj();
	//}}

	static COleImageObj* LoadImage(LPCTSTR lpszPath, bool bCache = true);
	static COleImageObj* LoadImage(HICON hIcon );

	ImageObj* GetImage();
	void SetImage(ImageObj* pImageObj);

	tstring GetFilePath();
	void SetFilePath(LPCTSTR lpszPath);

	int  GetImageType();
	void SetImageType(int nType);

	void SetKey(LPCTSTR lpszKey);
	LPCTSTR GetKey();

	// 是否显示缩略图
	void SetThumbnail(bool bThumbnail);
	bool GetThumbnail();

	bool IsCache(){return m_bCache;}

	int  GetFrameCount();
	double GetScale();

	virtual void LoadResource();
	virtual void ReleaseResource();
	//{{
protected:
	virtual void OnRichEditSizeChanged(RECT& rectParent);
	virtual void OnMouseLeave(TEventUI& event);
	virtual void OnMouseMove(TEventUI& event);
	virtual void OnClick(TEventUI& event);
	virtual void OnDbClick(TEventUI& event);
	virtual HRESULT WINAPI Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
		HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
		BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue);
	void NextFrame();
	int GetFrameDelay();
private:
	bool	IsThumbnailImage();
	int     m_nImageType;  
	int		m_nActiveFrame;  //当前帧
	int     m_nFrameDelay;
	ImageObj* m_ImageObj;
	ImageObj* m_pZoomImage;
	tstring m_strKey;
	tstring m_strPath;
	bool    m_bCache;
	bool m_bInThumbnail;
	//}}
};


class  DUI_API COleControlObj:public COleObject
{
public:
	COleControlObj(CControlUI* pControl);
	~COleControlObj();

public:
	void SetAutoSize(bool bAutoSize); // 控件随richedit宽度变化而变化
	CControlUI* GetControl();
	virtual void SetSize(int nWidth,int nHeight);

protected:
	virtual HRESULT WINAPI Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
		HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
		BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue);

	virtual void OnRichEditSizeChanged(RECT& rectParent);
	virtual void OnPositonChanged(LPCRECT lpRect);

private:
	bool IsSelected();
	CControlUI* m_pControl;
	RECT m_rcLastBounds;
	bool m_bAutoSize;
};



class DUI_API CRichEditUI:public CLabelUI, public INotifyUI
{
	//{{
	friend class CTextHost;
	friend class IExRichEditOleCallback;
	friend class COleObject;
	friend class CAnimationHandler;
	UI_OBJECT_DECLARE(CRichEditUI, _T("RichEdit"))
public:
	CRichEditUI();
	virtual ~CRichEditUI();

	IRichEditOle*  GetIRichEditOle();
	CTextHostBase* GetTextHost();

	//字体相关接口
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	void SetTextColor(DWORD dwTextColor);//设置字体颜色
	DWORD GetTextColor();

	void SetFontName(LPCTSTR lpszFontName);//设置字体格式
	LPCTSTR GetFontName();
	void SetFontSize(LONG size);//设置字体大小
	LONG GetFontSize();
	void SetBold(bool flag);//设置字体为粗体
	void SetItalic(bool flag);//设置字体为斜体
	void SetUnderline(bool flag);//设置字体为下划线
	bool IsBold();//是否为粗体？
	bool IsItalic();//斜体?
	bool IsUnderline();//下划线?
	void SetReadOnly(bool bReadOnly);
	bool IsReadOnly();

	void SetMaxChar(int nMax);
	int  GetMaxChar();

	void SetImageThumbnail(bool bThumbnail);  //是否显示缩略图
	bool GetImageThumbnail(); 

	//格式格式
	DWORD GetDefaultCharFormat(CHARFORMAT2 &cf);
	void  GetDefaultCharFormat(TEXTFORMAT &tf);
	bool  SetDefaultCharFormat(CHARFORMAT2 &cf);
	DWORD GetSelectionCharFormat(CHARFORMAT2 &cf);
	bool  SetSelectionCharFormat(CHARFORMAT2 &cf);
	DWORD GetParaFormat(PARAFORMAT2 &pf);
	bool  SetParaFormat(PARAFORMAT2 &pf);

	//位置相关接口
	int  GetLineCount();
	int  GetFirstVisibleLine();
	int  LineIndex(int nLine = -1);
	void GetCharPos(long lChar, POINT& pt);
	long LineFromChar(long nIndex);
	int  CharFromPos(POINT& pt);

	//选择相关接口
	void GetSel(CHARRANGE &cr);
	void GetSel(long& nStartChar, long& nEndChar);
	void SetSel(CHARRANGE &cr);
	void SetSel(long nStartChar, long nEndChar);
	void SetSelAll();
	void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo = false);
	void HideSelection(bool bHide);
	bool IsHideSelection();

	//文本相关接口
	long GetTextLength(DWORD dwFlags = GTL_DEFAULT);
	void GetText(tstring& sText);
	void GetText(CHARRANGE &cr, tstring& sText);
	void GetSelText(tstring& sText);
	LPCTSTR GetText();
	void SetText(LPCTSTR lpcszText);

	void InsertText(LPCTSTR lpcszText, int nOffsetPos = 0, bool bReplaceSel = false, TEXTFORMAT* pcf = NULL);
	void InsertLink(LPCTSTR lpcszText, CLinkObj* pLinkObj, int nOffsetPos = 0,  bool bReplaceSel = false, TEXTFORMAT* pcf = NULL);
	void CancelLink(CLinkObj* pLinkObj, TEXTFORMAT* pcf = NULL);
	void SetLink(CHARRANGE &cr, TEXTFORMAT* pcf = NULL);

	void InsertTextObj(ReTextObj* pTextObj, int nOffsetPos = 0, bool bReplaceSel = false, TEXTFORMAT* pcf = NULL);
	void UpdateTextObj(ReTextObj* pTextObj, TEXTFORMAT* pcf = NULL);

	void InsertObject(COleObject* pOleObject, long nCharPos = REO_CP_SELECTION, DWORD dwFlags = REO_BLANK | REO_BELOWBASELINE);
	void ResizeObject(COleObject* pOleObject, int nWidth, int nHeight);
	void InsertImage(COleImageObj* pImgObj, long nCharPos = REO_CP_SELECTION);

	// richedit插入控件操作
	COleObject* InsertControl(CControlUI* pControl, int nWidth, int nHeight, long nCharPos = REO_CP_SELECTION, DWORD dwFlags = REO_BLANK | REO_BELOWBASELINE | REO_INVERTEDSELECT); 
	void RemoveControl(CControlUI* pControl);
	int	 GetControlCharPos(CControlUI* pControl);
	COleObject* GetOleObjectByControl(CControlUI* pControl);

	virtual CControlUI* GetItem(LPCTSTR lpszId); //根据Id获取控件
	virtual CControlUI* GetItem(int iIndex);
	virtual int  GetCount();

	//滚动相关接口
	void EndDown();
	void SetScrollPos(int nPos);
	int  GetScrollPos();
	int  GetScrollRange();
	void EnableVScrollBar(bool bEnable);
	bool IsEnableVScrollBar();
	CScrollbarUI* GetVScrollbar();

	void EnableCaret(bool bEnable);
	bool IsEnableCaret();
	//编辑接口
    void Copy();
	void Cut();
	bool Redo();
	bool Undo();
	void Clear();
	void Paste();
	void Paste(IDataObject* pDataObject);

	void SetAutoURLDetect(bool bAutoURLDetect);
	bool IsAutoURLDetect();
	void URLDetect(int nStartPos = 0, int nEndPos = -1);

	long StreamIn(int nFormat, EDITSTREAM &es);
	long StreamOut(int nFormat, EDITSTREAM &es);
	HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult);

	void SetRich(BOOL bNew);
	bool GetZoom(int& nNum, int& nDen);
	bool SetZoom(int nNum, int nDen);

	UINT SetUndoLimit(UINT nLimit);
	void EmptyUndoBuffer();

	IOleObject* FindObject(POINT& pt);
	CLinkObj*	FindLink(POINT& pt);

	void SetStartIndent(int nStartIndent);
	int  GetStartIndent();

	LONG CalcTextHeight(LONG& nWidth);

	void ReLayoutOle();

	void SetMaxImageSize(int maxWidth, int maxHeight);

	virtual bool IsMouseWhellEnabled();
protected: 
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual bool Event(TEventUI& event);
	virtual void Init();
	virtual void SetRect(RECT& rectRegion);
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual bool ProcessNotify(TNotifyUI* pNotify);
	virtual void OnDrawAnimation(LPRECT lpRectClip);
	virtual void OnCharPosChanged(long nChangePos, long nChangeLen);

	virtual bool CanPaste();
	virtual void OnPaste(IDataObject* pDataObject);
	virtual bool OnCopy(IDataObject** ppDataObject);

	virtual void OnOleEvent(TEventUI&event, IOleObject* pOleObj);
	virtual void OnOleRelease(IOleObject* pOleObj);
	virtual void OnRequestResize(LPRECT lpRect);

	virtual void OnDetectURLs(LPCTSTR lpszText, CStdPtrArray& arrLinksOut);

	virtual RECT GetScrollRect();

	void GetDropTarget(IDropTarget** ppdt);
	RECT GetClientRect();
	void GetVisibleCharRang( long& nVFirstChar, long& nVEndChar );
	int  GetLastLineHeight();

	void InsertControl(COleControlObj* pControlObject, int nWidth, int nHeight, long nCharPos = REO_CP_SELECTION, DWORD dwFlags = REO_BLANK | REO_BELOWBASELINE | REO_INVERTEDSELECT); 
//}}
private:
	bool ChildEvent(TEventUI& event);
	bool Remove(CControlUI* pControlChild);
	bool ReadHTMLFromClipboard(HGLOBAL hMem);
	void RemoveInvalidImageObj();
	bool ScrollBarEventDelegate(TEventUI& event);
	bool OnMenuClick(TNotifyUI* pNotifyUI);
	void ResizeOle(RECT rect);
	void UpdateOlePosition();
	void RemoveLinkObj(CHARRANGE& range);
	void RemoveAllLinkObj();

	void RemoveTextObj(CHARRANGE& range);
	void RemoveAllTextObj();
	bool FindTextObj(ReTextObj* pTextObj);
	void HandleGestureEvent(TEventUI& event);

	IRichEditOleCallback* m_pRichEditOleCallback;
	CTextHostBase* m_pTextHost;
	CScrollbarUI* m_pVerticalScrollbar;
	bool m_bEnableVScrollbar;
	bool m_bEnableCaret;

	tstring m_strFontName;
	LONG m_nFontSize;
	bool m_bBold;
	bool m_bItalic;
	bool m_bUnderline;
	bool m_bReadOnly;
	COLORREF m_txtColor;

	CStdPtrArray m_listLinkObj;     //链接列表
	CStdPtrArray m_listTextObj;     //链接列表
	bool m_bResizing;
	bool m_bImageThumbnail;
	int  m_nStartIndent;
	IRenderDC* m_pRenderDC;

	IOleObject* m_pOleObjHover;
	bool m_bEnabeImage;
	bool m_bHideSelection;
	DWORD m_dwObjectID;
	CStdPtrArray m_items; //插入控件
	bool m_bNeedUpdateOlePositon;
	bool m_bNeedReLayout;
	bool m_bAutoURLDetect;
	bool m_bReLayoutOle;
	int m_nMaxImageWidth;
	int m_nMaxImageHeight;
	bool m_bEnableMenu;
};
