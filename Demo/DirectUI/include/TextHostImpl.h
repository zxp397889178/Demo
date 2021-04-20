/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-22       xqb
*********************************************************************/
#ifndef _TEXTHOSTIMPL_H
#define _TEXTHOSTIMPL_H
#include <Richedit.h>
#include <richole.h>
#include <TextServ.h>
class TextHostDataImpl;
class DUI_API CTextHostBase : public ITextHost
{
public:
	CTextHostBase(CControlUI* pOwner);
	virtual ~CTextHostBase();

public:
	// -----------------------------
	//	ITextHost interface
	// -----------------------------
	virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
	virtual ULONG _stdcall AddRef(void);
	virtual ULONG _stdcall Release(void);
	virtual HDC TxGetDC();
	virtual INT TxReleaseDC(HDC hdc);
	virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
	virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
	virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
	virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
	virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual void TxViewChange(BOOL fUpdate);
	virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
	virtual BOOL TxShowCaret(BOOL fShow);
	virtual BOOL TxSetCaretPos(INT x, INT y);
	virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
	virtual void TxKillTimer(UINT idTimer);
	virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
	virtual void TxSetCapture(BOOL fCapture);
	virtual void TxSetFocus();
	virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
	virtual BOOL TxScreenToClient (LPPOINT lppt);
	virtual BOOL TxClientToScreen (LPPOINT lppt);
	virtual HRESULT TxActivate( LONG * plOldState );
	virtual HRESULT TxDeactivate( LONG lNewState );
	virtual HRESULT TxGetClientRect(LPRECT prc);
	virtual HRESULT TxGetViewInset(LPRECT prc);
	virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF);
	virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
	virtual COLORREF TxGetSysColor(int nIndex);
	virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT TxGetMaxLength(DWORD *plength);
	virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
	virtual HRESULT TxGetPasswordChar(WCHAR *pch);
	virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
	virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
	virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
	virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
	virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
	virtual HRESULT TxNotify(DWORD iNotify, void *pv);

	virtual HIMC TxImmGetContext(void);
	virtual void TxImmReleaseContext(HIMC himc);
	virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

public:
	HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult);
	bool Init(CHARFORMAT2* pcf,  PARAFORMAT2* ppf);
	void SetRich(bool bRich);
	void SetPassword(bool bPassword);
	WCHAR SetPasswordChar(WCHAR chPasswordChar);

	void SetMaxChar(int nMaxChar);
	int  GetMaxChar();

	void GetSel(CHARRANGE &cr);
	void SetSel(CHARRANGE &cr);
	void SetSel(long nStartChar, long nEndChar);
	void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo = false);

	long GetTextLength(DWORD dwFlags);
	void GetText(tstring& sText);
	void GetText(CHARRANGE &cr, tstring& sText);
	void GetSelText(tstring& sText);
	void SetText(LPCTSTR lpcszText);

	void SetReadOnly(bool bReadOnly);
    void SetTextColor(DWORD dwColor);
	void SetFontName(LPCTSTR lpszFontName);
	void SetFontSize(LONG size);//设置字体大小
	void SetBold(bool flag);//设置字体为粗体
	void SetItalic(bool flag);//设置字体为斜体
	void SetUnderline(bool flag);//设置字体为下划线


	ITextServices* GetTextServices(){return m_pServ;}
	IRichEditOle*  GetIRichEditOle(){return m_pRichOle;}
	IDropTarget*  GetIDropTarget(){return  m_pDropTarget;}
	HWND GetHWND(){return m_hWnd;}
	CControlUI* GetOwner(){return m_pOwner;}

	bool  SetCharFormat(CHARFORMAT2 &c);
	CHARFORMAT2* GetCharFormat();
	bool SetSelectionCharFormat(CHARFORMAT2 &cf);
	DWORD GetSelectionCharFormat(CHARFORMAT2 &cf);
	bool  SetParaFormat(PARAFORMAT2 &p);
	PARAFORMAT2* GetParaFormat();
	

	void RegisterDragDrop();
	void RevokeDragDrop(void);
	BOOL DoSetCursor(RECT *prc, POINT *pt);

	void SetRect(RECT& rect);//设置输入区大小
	HRESULT OnTxInPlaceDeactivate();
	HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
	void DrawCaret(IRenderDC* pRender, RECT& rcUpdate);

public:
	unsigned	fBorder;	// control has border
	unsigned	fInBottomless;	// inside bottomless callback
	unsigned	fEnableAutoWordSel;	// enable Word style auto word selection?
	unsigned	fVertical;	// vertical writing
	unsigned	fRegisteredForDrop; // whether host has registered for drop
	unsigned	fVisible;	// Whether window is visible or not.
	unsigned	fResized;	// resized while hidden
	unsigned	fWordWrap;	// Whether control should word wrap
	unsigned	fAllowBeep;	// Whether beep is allowed
	unsigned	fRich;	// Whether control is rich text
	unsigned	fSaveSelection;	// Whether to save the selection when inactive
	unsigned	fInplaceActive; // Whether control is inplace active
	unsigned	fTransparent; // Whether control is transparent
	unsigned	fTimer;	// A timer is set
	unsigned	fMultiLine;	
	unsigned	fReadOnly;
	unsigned	fPassword;
	unsigned	fHideSel;
	unsigned	fVerticalScrollBar;	
	unsigned	fShowCaret;	

	LONG		lSelBarWidth;			// Width of the selection bar
	LONG  		cchTextMost;			// maximum text size
	DWORD		dwEventMask;			// Event mask to pass on to parent window

	LONG		icf;
	LONG		ipf;

	SIZEL		sizelExtent;			// Extent array
	RECT		rcClient;				// Client Rect for this control
	RECT        rcInset;
	//SIZEL		sizelExtent;			// Extent array

	TextHostDataImpl* m_pTextHostDataImpl;
	LONG		laccelpos;				// Accelerator position
	WCHAR		chPasswordChar;		// Password character
protected:
	CControlUI*     m_pOwner;
	ITextServices	*m_pServ;		// pointer to Text Services object
	IRichEditOle    *m_pRichOle;     // 
	IDropTarget*   m_pDropTarget;
	HWND m_hWnd;
	ULONG	cRefs;					// Reference Count
	bool m_bFormatChanging;

};

#endif