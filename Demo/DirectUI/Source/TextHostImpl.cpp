/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "TextHostImpl.h"
#include "UICaret.h"



void InitDefaultCharFormat(CControlUI* pControl, CHARFORMAT2* pcf, DWORD dwColor, bool bRTLReading) 
{
	LOGFONT lf;
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfCharSet = DEFAULT_CHARSET;
	memset(pcf, 0, sizeof(CHARFORMAT2));
	pcf->cbSize = sizeof(CHARFORMAT2);
	pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

	FontObj* fontobj = GetResObj()->GetDefaultFont();
	TextStyle* pText = pControl->GetTextStyle(_T("txt"));
	if (pText) fontobj = pText->GetFontObj();

/*	int yPerInch = ::GetDeviceCaps(pControl->GetWindow()->GetPaintDC(), LOGPIXELSY);*/ 
	pcf->yHeight = /*DPI_SCALE*/(fontobj->GetHeight()) * LY_PER_INCH / 96;
	pcf->yOffset = 0;
	pcf->dwEffects = 0;
	_tcscpy(pcf->szFaceName, fontobj->GetFaceName());
	pcf->crTextColor = dwColor;

	if(fontobj->IsBold())
		pcf->dwEffects |= CFE_BOLD;
	if(fontobj->IsItalic())
		pcf->dwEffects |= CFE_ITALIC;
	if(fontobj->IsUnderline())
		pcf->dwEffects |= CFE_UNDERLINE;

	if (GetUIEngine()->IsRTLLayout())
	{
		pcf->bCharSet = ARABIC_CHARSET;
	}
	else
	{
		pcf->bCharSet = lf.lfCharSet;

	}

	// 有些阿拉伯编辑框是不需要RTLRealing
	if (!bRTLReading)
		pcf->bCharSet = lf.lfCharSet;

	pcf->bPitchAndFamily = lf.lfPitchAndFamily;
}

void InitDefaultParaFormat(CControlUI* pControl, PARAFORMAT2* ppf, bool bRTLReading) 
{
	memset(ppf, 0, sizeof(PARAFORMAT2));
	ppf->cbSize = sizeof(PARAFORMAT2);
	ppf->cTabCount = 1;
	ppf->rgxTabs[0] = lDefaultTab;
	ppf->dwMask = PFM_ALL;

	if (GetUIEngine()->IsRTLLayout())
	{
		ppf->wAlignment = PFA_RIGHT;
		if (bRTLReading)
		{
			ppf->dwMask  |= PFM_EFFECTS;
			ppf->wEffects = PFE_RTLPARA;
		}
	}
	else
	{
		ppf->wAlignment = PFA_LEFT;
	}

}

class TextHostDataImpl
{
public:
	CHARFORMAT2	cf;					// Default character format
	PARAFORMAT2	pf;					// Default paragraph format
};

CTextHostBase:: CTextHostBase(CControlUI* pOwner)
{	
	cchTextMost = cInitTextMax;
	memset(&rcClient, 0, sizeof(rcClient));
	memset(&rcInset, 0, sizeof(rcInset));
	m_pServ = NULL;
	cRefs = 1;
	m_pOwner = pOwner;
	fRegisteredForDrop = 0;
	m_pDropTarget = NULL;
	fWordWrap = TRUE;
	fMultiLine = TRUE;
	fReadOnly = FALSE;
	fPassword = FALSE;
	fHideSel = TRUE;
	fRich = TRUE;
	m_bFormatChanging = false;

	//初始化变量 add by hanzp
	fBorder = 1;	// control has border
	fInBottomless = 1;	// inside bottomless callback
	fEnableAutoWordSel = 1;	// enable Word style auto word selection?
	fVertical = 1;	// vertical writing
	fVisible = 1;	// Whether window is visible or not.
	fResized = 1;	// resized while hidden
	fAllowBeep = 1;	// Whether beep is allowed
	fSaveSelection = 1;	// Whether to save the selection when inactive
	fInplaceActive = 1; // Whether control is inplace active
	fTransparent = 1; // Whether control is transparent
	fTimer = 1;// A timer is set
	fVerticalScrollBar = 1;
	fShowCaret = 1;
	m_pTextHostDataImpl = new TextHostDataImpl;

}

CTextHostBase::~CTextHostBase()
{
	RevokeDragDrop();
	if (m_pRichOle)
		m_pRichOle->Release();
	if (m_pServ)
	{
		OnTxInPlaceDeactivate();
		m_pServ->Release();
	}
	if (m_pTextHostDataImpl)
		delete m_pTextHostDataImpl;
	m_pTextHostDataImpl = NULL;

}


typedef HRESULT(_stdcall *_CTS)(
								IUnknown *punkOuter,
								ITextHost *pITextHost,
								IUnknown **ppUnk);
static _CTS g_cts = NULL;

bool CTextHostBase::Init(CHARFORMAT2* pcf,  PARAFORMAT2* ppf)
{
	if (m_pServ)
		return true;
	m_hWnd = m_pOwner->GetWindow()->GetHWND();

	// 初始化格式
	m_pTextHostDataImpl->cf = *pcf;
	/*m_pTextHostDataImpl->cf.yHeight = DPI_SCALE(m_pTextHostDataImpl->cf.yHeight);*/
	m_pTextHostDataImpl->pf = *ppf;
	
	fInplaceActive = true;

	if (g_cts == NULL)
	{
		
		HINSTANCE richHandle = LoadLibraryW(L"Riched20.dll");

		if (richHandle == NULL)
			return false;
		else
		{
			g_cts = (_CTS)GetProcAddress(richHandle, "CreateTextServices");
			if (NULL == g_cts)
			{
				FreeLibrary(richHandle);
				return false;
			}
		}
	}

	IUnknown *pUnk = NULL;
	g_cts(NULL, this, &pUnk);
	if (!pUnk)
		return false;

	pUnk->QueryInterface(IID_ITextServices,(void **)&m_pServ);
	pUnk->Release();
	if (!m_pServ)
		return false;

	if (!fReadOnly)
		RegisterDragDrop();

	//获取Ole接口
	LRESULT lres=0;
	m_pServ->TxSendMessage(EM_GETOLEINTERFACE, 0, int(&m_pRichOle),&lres);

	//设置链接事件
	LRESULT mask=0;
	m_pServ->TxSendMessage(EM_GETEVENTMASK,0,0,&mask);
	m_pServ->TxSendMessage(EM_SETEVENTMASK,0, mask | ENM_CHANGE,&lres);	
	m_pServ->TxSendMessage(EM_SETLANGOPTIONS, 0, 0,&lres);

	//if (GetUIEngine()->IsRTLLayout())
	{
		//if (fMultiLine)
		//	m_pServ->TxSendMessage( EM_SETLANGOPTIONS , 0, IMF_AUTOFONT|IMF_AUTOKEYBOARD|IMF_IMECANCELCOMPLETE|IMF_IMEALWAYSSENDNOTIFY|IMF_AUTOFONTSIZEADJUST/*|IMF_UIFONTS*/,  &lres);

		m_pServ->TxSendMessage( EM_SETLANGOPTIONS , 0, IMF_AUTOFONT /*| IMF_DUALFONT*/,  &lres);
	}

	OnTxInPlaceActivate(NULL);

	SetFontName(GetUIRes()->GetDefaultFontName());
	return true;
}


//////////////////////////////////////////////////////
////
HRESULT CTextHostBase::TxGetSelectionBarWidth(LONG *lSelBarWidth)
{
	*lSelBarWidth = 0;
	return S_OK;
}

bool  CTextHostBase::SetCharFormat(CHARFORMAT2 &cfNew)
{
	m_bFormatChanging = true;
	cfNew.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cfNew, &lResult);
	if( (BOOL)lResult == TRUE ) {
		m_pTextHostDataImpl->cf.cbSize = sizeof(CHARFORMAT2);
		TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&(m_pTextHostDataImpl->cf), 0);
		return true;
	}
	return false;
}

CHARFORMAT2*  CTextHostBase::GetCharFormat()
{
	return &(m_pTextHostDataImpl->cf);
}

bool CTextHostBase::SetSelectionCharFormat(CHARFORMAT2 &cfNew)
{
	m_bFormatChanging = true;
	cfNew.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfNew, &lResult);
	return (BOOL)lResult == TRUE;
}

DWORD CTextHostBase::GetSelectionCharFormat(CHARFORMAT2 &cfOut)
{
	memset(&cfOut, 0, sizeof(CHARFORMAT2));
	cfOut.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfOut, &lResult);
	return (DWORD)lResult;
}


bool  CTextHostBase::SetParaFormat(PARAFORMAT2 &pfNew)
{
	m_bFormatChanging = true;
	pfNew.cbSize = sizeof(PARAFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pfNew, &lResult);
	if( (BOOL)lResult == TRUE ) {
		m_pTextHostDataImpl->pf = pfNew;
		return true;
	}
	return false;
}

PARAFORMAT2* CTextHostBase::GetParaFormat()
{
	return &(m_pTextHostDataImpl->pf);
}


//////////////////////////// ITextHost Interface  ////////////////////////////

HRESULT CTextHostBase::QueryInterface(REFIID riid, void **ppvObject)
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) 
		|| IsEqualIID(riid, IID_ITextHost)) 
	{
		AddRef();
		*ppvObject = (ITextHost *) this;
		hr = S_OK;
	}

	return hr;
}

ULONG CTextHostBase::AddRef(void)
{
	return ++cRefs;
}

ULONG CTextHostBase::Release(void)
{
	ULONG c_Refs = --cRefs;

	if (c_Refs == 0)
	{
		delete this;
	}

	return c_Refs;
}

void CTextHostBase::RegisterDragDrop()
{
	m_pServ->TxGetDropTarget(&m_pDropTarget);
}

void CTextHostBase::RevokeDragDrop(void)
{
	if (m_pDropTarget)
		m_pDropTarget->Release();
	m_pDropTarget = NULL;
}


HDC CTextHostBase::TxGetDC()
{
	return m_pOwner->GetWindow()->GetPaintDC();
}

INT CTextHostBase::TxReleaseDC(HDC hdc)
{
	return 1;
}

BOOL CTextHostBase::DoSetCursor(RECT *prc, POINT *pt)
{
	RECT *prcClient = (!fInplaceActive || prc) ? &rcClient : NULL;
	m_pServ->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL,  m_pOwner->GetWindow()->GetPaintDC(),
		NULL, prcClient, pt->x, pt->y);
	return TRUE;
}


HRESULT	CTextHostBase::OnTxInPlaceDeactivate()
{
	HRESULT hr = m_pServ->OnTxInPlaceDeactivate();

	if (SUCCEEDED(hr))
	{
		fInplaceActive = FALSE;
	}

	return hr;
}

HRESULT	CTextHostBase::OnTxInPlaceActivate(LPCRECT prcClient)
{
	fInplaceActive = TRUE;
	HRESULT hr =  m_pServ->OnTxInPlaceActivate(prcClient);
	if (FAILED(hr))
	{
		fInplaceActive = FALSE;
	}
	return hr;
}

void CTextHostBase::SetRect(RECT& rect) 
{
	/*if (rect.bottom <= 0 || rect.right <= 0) return;*/ //xqb如果不注释掉，刚开始弹出聊天框，排版会有问题，SetSel不准确
	

	rcClient = rect;

	sizelExtent.cx = DXtoHimetricX(rcClient.right - rcClient.left);
	sizelExtent.cy = DYtoHimetricY(rcClient.bottom - rcClient.top);

	if (m_pServ) m_pServ->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);

}


BOOL CTextHostBase::TxShowScrollBar(INT fnBar, BOOL fShow)
{
	return TRUE;
}

BOOL CTextHostBase::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
{
	return TRUE;
}

BOOL CTextHostBase::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
	return TRUE;
}

BOOL CTextHostBase::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
{
	return TRUE;
}

void CTextHostBase::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll,
									 LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll)
{

}

HRESULT CTextHostBase::TxGetScrollBars(DWORD *pdwScrollBar)
{
	DWORD dwStyle= 0;//ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | ES_DISABLENOSCROLL;
	if (fMultiLine) dwStyle |= ES_MULTILINE;
	if (fReadOnly) dwStyle |= ES_READONLY;
	if (fPassword) dwStyle |= ES_PASSWORD;
	if (!fHideSel) dwStyle |= ES_NOHIDESEL;
	/*if (fVerticalScrollBar) */dwStyle |= WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL;
	if (m_pTextHostDataImpl->pf.wAlignment == PFA_RIGHT) dwStyle |= ES_RIGHT;
	else if (m_pTextHostDataImpl->pf.wAlignment == PFA_CENTER) dwStyle |= ES_CENTER;

	*pdwScrollBar = dwStyle | (ES_DISABLENOSCROLL);


	return S_OK;
}

void CTextHostBase::TxViewChange(BOOL fUpdate)
{
	/*if (fUpdate)
		::UpdateWindow(m_hWnd);*/
}

void CTextHostBase::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	if (prc == NULL)
	{
		m_pOwner->Invalidate(false);
	}
	else
	{
		m_pOwner->GetWindow()->Invalidate((RECT*)prc);
	}
}


BOOL CTextHostBase::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
	BOOL bRet =  CCaretUI::GetInstance()->CreateCaret(m_pOwner->GetWindow(), hbmp, xWidth, yHeight);//::CreateCaret(m_hWnd,hbmp,xWidth,yHeight);
	TxInvalidateRect(NULL, FALSE);
	return bRet;
}

BOOL CTextHostBase::TxShowCaret(BOOL fShow)
{
	if (!fShowCaret)
		return CCaretUI::GetInstance()->HideCaret();
	if (fShow)
	{
		return CCaretUI::GetInstance()->ShowCaret();
	}
	else
	{
		return CCaretUI::GetInstance()->HideCaret();
	}
}

BOOL CTextHostBase::TxSetCaretPos(INT x, INT y)
{
	return CCaretUI::GetInstance()->SetCaretPos(x, y);
}

BOOL CTextHostBase::TxSetTimer(UINT idTimer, UINT uTimeout)
{
	return m_pOwner->GetWindow()->SetTimer(m_pOwner, idTimer, uTimeout);
}

void CTextHostBase::TxKillTimer(UINT idTimer)
{
	m_pOwner->GetWindow()->KillTimer(m_pOwner, idTimer);
}


void CTextHostBase::TxSetCapture(BOOL fCapture)
{
	if (fCapture)
		m_pOwner->GetWindow()->SetCapture();
	else
		m_pOwner->GetWindow()->ReleaseCapture();
}

void CTextHostBase::TxSetFocus()
{
	m_pOwner->SetFocus();
}

void CTextHostBase::TxSetCursor(HCURSOR hcur, BOOL fText)
{
	::SetCursor(hcur);
}

BOOL CTextHostBase::TxScreenToClient(LPPOINT lppt)
{
	return ::ScreenToClient(m_hWnd,lppt);
}

BOOL CTextHostBase::TxClientToScreen(LPPOINT lppt)
{
	return ClientToScreen(m_hWnd,lppt);
}

HRESULT CTextHostBase::TxActivate(LONG* plOldState)
{
	return S_OK;
}

HRESULT CTextHostBase::TxDeactivate(LONG lNewState)
{
	return S_OK;
}

HRESULT CTextHostBase::TxGetClientRect(LPRECT prc)
{
	*prc = rcClient;
	return S_OK;
}

HRESULT CTextHostBase::TxGetViewInset(LPRECT prc)
{
	*prc = rcInset;
	return S_OK;
}

HRESULT CTextHostBase::TxGetCharFormat(const CHARFORMATW **ppCF)
{
	m_bFormatChanging = true;
	*ppCF=&(m_pTextHostDataImpl->cf);
	return NOERROR;
}

HRESULT CTextHostBase::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	m_bFormatChanging = true;
	*ppPF=&(m_pTextHostDataImpl->pf);
	return NOERROR;
}

COLORREF CTextHostBase::TxGetSysColor(int nIndex)
{
	return ::GetSysColor(nIndex);
}

HRESULT CTextHostBase::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	*pstyle = TXTBACK_TRANSPARENT;
	return S_OK;
}

HRESULT CTextHostBase::TxGetMaxLength(DWORD *plength)
{
	*plength = cchTextMost;
	return S_OK;
}


HRESULT CTextHostBase::TxGetPasswordChar(WCHAR *pch)
{
#ifdef _UNICODE
	*pch = chPasswordChar;
#else
	::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
	return NOERROR;
}

HRESULT CTextHostBase::TxGetAcceleratorPos(LONG *pcp)
{
	*pcp = -1;
	return S_OK;
}

HRESULT CTextHostBase::TxGetExtent(LPSIZEL lpExtent)
{
	*lpExtent = sizelExtent;
	return S_OK;
}

HRESULT CTextHostBase::OnTxCharFormatChange(const CHARFORMATW * pcf)
{
	memcpy(&(m_pTextHostDataImpl->cf),pcf,sizeof(CHARFORMATW));
	return S_OK;
}

HRESULT CTextHostBase::OnTxParaFormatChange(const PARAFORMAT * ppf)
{
	memcpy(&(m_pTextHostDataImpl->pf),ppf,sizeof(PARAFORMAT));
	return S_OK;
}

HRESULT CTextHostBase::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
{
	DWORD dwProperties = 0;

	if (fRich)
	{
		dwProperties = TXTBIT_RICHTEXT;
	}

	if (fMultiLine)
	{
		dwProperties |= TXTBIT_MULTILINE;
	}

	if (fReadOnly)
	{
		dwProperties |= TXTBIT_READONLY;
	}

	if (fPassword)
	{
		dwProperties |= TXTBIT_USEPASSWORD;
	}

	if (fHideSel)
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}

	if (fEnableAutoWordSel)
	{
		dwProperties |= TXTBIT_AUTOWORDSEL;
	}

	if (fWordWrap)
	{
		dwProperties |= TXTBIT_WORDWRAP;
	}

	if (fAllowBeep)
	{
		dwProperties |= TXTBIT_ALLOWBEEP;
	}

	if (fSaveSelection)
	{
		dwProperties |= TXTBIT_SAVESELECTION;
	}

	*pdwBits = dwProperties & dwMask; 
	return NOERROR;

}

HRESULT CTextHostBase::TxNotify(DWORD iNotify, void *pv)
{
	switch (iNotify)
	{
	case EN_CHANGE:
		{
			if (fReadOnly)
				break;
			
			if (!pv) break;
			CHANGENOTIFY * penChangeNotify = (CHANGENOTIFY*)pv;
		

			if (m_bFormatChanging)
			{
				m_bFormatChanging = false;
				if (penChangeNotify->dwChangeType < 1)
					break;
			}
			GETTEXTLENGTHEX textLenEx;
			textLenEx.flags = GTL_DEFAULT;
			textLenEx.codepage = 1200;
			LRESULT lResult;
			TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
			m_pOwner->SendNotify(UINOTIFY_EN_CHANGE, (WPARAM)lResult);
		}
		break;
	}	return S_OK;
}

HIMC CTextHostBase::TxImmGetContext()
{
	return ::ImmGetContext(m_hWnd);
}

void CTextHostBase::TxImmReleaseContext(HIMC himc)
{
	::ImmReleaseContext(m_hWnd, himc);
}

void CTextHostBase::SetMaxChar(int nMaxChar)
{
	cchTextMost = nMaxChar;
	if (nMaxChar <= 0) cchTextMost = cInitTextMax;
	if (GetTextServices()) GetTextServices()->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

int CTextHostBase::GetMaxChar()
{
	return cchTextMost;
}

void CTextHostBase::ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo)
{
	if (cchTextMost < cInitTextMax)
	{
		CHARRANGE cr;
		GetSel(cr);
		long lLen = GetTextLength(GTL_DEFAULT);
		long nSelLen = cr.cpMax - cr.cpMin;
		if (nSelLen < 0) nSelLen = 0;
		long nStrLen = _tcslen(lpszNewText);

		long nCanInsetLen = cchTextMost - lLen + nSelLen;
		if (nCanInsetLen < nStrLen)
		{
			tstring strNew = lpszNewText;
			strNew = strNew.substr(0, nCanInsetLen);
			TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)strNew.c_str(), 0); 
			return;
		}
	}
	TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
}

void CTextHostBase::GetSel(CHARRANGE &cr)
{
	TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
}

long CTextHostBase::GetTextLength(DWORD dwFlags)
{
	GETTEXTLENGTHEX textLenEx;
	textLenEx.flags = dwFlags;
	textLenEx.codepage = 1200;
	LRESULT lResult;
	TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
	return (long)lResult;
}

void CTextHostBase::SetText(LPCTSTR lpcszText)
{
	SetSel(0, -1);
	ReplaceSel(lpcszText, false);
}

void CTextHostBase::SetSel(CHARRANGE &cr)
{
	TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, 0); 
}

void CTextHostBase::SetSel(long nStartChar, long nEndChar)
{
	CHARRANGE cr;
	cr.cpMin = nStartChar;
	cr.cpMax = nEndChar;
	SetSel(cr);
}

void CTextHostBase::GetText(tstring& sText)
{
	long lLen = GetTextLength(GTL_DEFAULT);
	LPTSTR lpText = NULL;
	GETTEXTEX gt;
	gt.flags = GT_DEFAULT;
	gt.cb = sizeof(TCHAR) * (lLen + 1) ;
	gt.codepage = 1200;
	lpText = new TCHAR[lLen + 1];
	::ZeroMemory(lpText, (lLen + 1) * sizeof(TCHAR));

	gt.lpDefaultChar = NULL;
	gt.lpUsedDefChar = NULL;
	TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, 0);
	sText = lpText;
	delete[] lpText;
}

void CTextHostBase::GetText(CHARRANGE &cr, tstring& sText)
{
	if (cr.cpMax < cr.cpMin)
		return;
	LPWSTR lpText = NULL;
	lpText = new TCHAR[cr.cpMax - cr.cpMin + 1];
	::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(TCHAR));
	TEXTRANGE tr;
	tr.chrg = cr;
	tr.lpstrText = lpText;

	TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
	sText = (LPCWSTR)lpText;
	delete[] lpText;

}

void CTextHostBase::GetSelText(tstring& sText)
{
	CHARRANGE cr;
	cr.cpMin = cr.cpMax = 0;
	TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
	LPWSTR lpText = NULL;
	lpText = new WCHAR[cr.cpMax - cr.cpMin + 1];
	::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR));
	TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, 0);
	sText = (LPCWSTR)(lpText);
	delete[] lpText;
}

void CTextHostBase::SetReadOnly(bool bReadOnly)
{
	fReadOnly = bReadOnly;
	if (GetTextServices() != NULL)
	{
		GetTextServices()->OnTxPropertyBitsChange(TXTBIT_READONLY, 
		fReadOnly ? TXTBIT_READONLY : 0);
	}
}

void CTextHostBase::SetTextColor(DWORD dwColor)
{
	m_pTextHostDataImpl->cf.crTextColor = dwColor;
	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);
}

void CTextHostBase::SetFontName(LPCTSTR lpszFontName)
{
	tstring strFaceName = lpszFontName;
	if (strFaceName.length() < LF_FACESIZE)
		_tcscpy(m_pTextHostDataImpl->cf.szFaceName, strFaceName.c_str());
	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);

}
void CTextHostBase::SetFontSize(LONG size)//设置字体大小
{
	m_pTextHostDataImpl->cf.yHeight = size;
	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);

}
void CTextHostBase::SetBold(bool flag)//设置字体为粗体
{
	if (flag) m_pTextHostDataImpl->cf.dwEffects |= CFE_BOLD;
	else m_pTextHostDataImpl->cf.dwEffects &= ~CFE_BOLD;

	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);
}
void CTextHostBase::SetItalic(bool flag)//设置字体为斜体
{
	if (flag) m_pTextHostDataImpl->cf.dwEffects |= CFE_ITALIC;
	else m_pTextHostDataImpl->cf.dwEffects &= ~CFE_ITALIC;

	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);

}
void CTextHostBase::SetUnderline(bool flag)//设置字体为下划线
{
	if (flag) m_pTextHostDataImpl->cf.dwEffects |= CFE_UNDERLINE;
	else m_pTextHostDataImpl->cf.dwEffects &= ~CFE_UNDERLINE;

	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);
}

void CTextHostBase::SetRich(bool bRich)
{
	fRich = bRich;
	if (!GetTextServices()) return;
	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
		bRich ? TXTBIT_RICHTEXT : 0);
}



void CTextHostBase::SetPassword(bool bPassword)
{
	fPassword = bPassword;
	if (!GetTextServices()) return;
	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
		bPassword ? TXTBIT_USEPASSWORD : 0);
}
WCHAR CTextHostBase::SetPasswordChar(WCHAR  ch_PasswordChar)
{
	WCHAR chOldPasswordChar = chPasswordChar;

	chPasswordChar = ch_PasswordChar;

	if (!GetTextServices()) return chOldPasswordChar;
	GetTextServices()->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
		(chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

	return chOldPasswordChar;
}

HRESULT CTextHostBase::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult)
{
	if (GetTextServices())
	{
		HRESULT hr =  GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
		return hr;
	}
	return S_FALSE;
}

void CTextHostBase::DrawCaret(IRenderDC* pRender, RECT& rcUpdate)
{
	CCaretUI::GetInstance()->Render(pRender, rcUpdate);
	
}