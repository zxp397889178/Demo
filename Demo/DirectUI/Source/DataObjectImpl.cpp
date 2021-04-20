/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "DataObjectImpl.h"



class DataObjImpl
{
public:
	~DataObjImpl()
	{
	}
	void RemoveAll()
	{
		size_t nFormatEtcSize = m_pFormatEtc.size();
		for (size_t i = 0; i < nFormatEtcSize; i++)
			delete m_pFormatEtc[i];
		m_pFormatEtc.clear();

		size_t nStgMediumSize = m_pStgMedium.size();
		for (size_t i = 0; i < nStgMediumSize; i++)
		{
			ReleaseStgMedium(m_pStgMedium[i]);
			delete m_pStgMedium[i];
		}
		m_pStgMedium.clear();
	}
	std::vector<FORMATETC*> m_pFormatEtc;
	std::vector<STGMEDIUM*> m_pStgMedium;
};
///////////////////////////////////////////////////////////////////////////////////////////
class CEnumFormatEtc : public IEnumFORMATETC 
{ 
public: 
	// 
	// IUnknown members 
	// 
	HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject); 
	ULONG __stdcall AddRef (void); 
	ULONG __stdcall Release (void); 
	// 
	// IEnumFormatEtc members 
	// 
	HRESULT __stdcall Next (ULONG celt, FORMATETC * rgelt, ULONG * pceltFetched); 
	HRESULT __stdcall Skip (ULONG celt);  
	HRESULT __stdcall Reset (void); 
	HRESULT __stdcall Clone (IEnumFORMATETC ** ppEnumFormatEtc); 

	// 
	// Construction / Destruction 
	// 
	CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats); 
	~CEnumFormatEtc(); 
	static HRESULT CreateEnumFormatEtc (UINT cfmt, FORMATETC *afmt, IEnumFORMATETC **ppEnumFormatEtc);

private: 

	void DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source);
	LONG m_lRefCount; // Reference count for this COM interface 
	ULONG m_nIndex; // current enumerator index 
	ULONG m_nNumFormats; // number of FORMATETC members 
	FORMATETC * m_pFormatEtc; // array of FORMATETC objects 
}; 


void CEnumFormatEtc::DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source) 
{ 
	// copy the source FORMATETC into dest 
	*dest = *source; 

	if(source->ptd) 
	{ 
		// allocate memory for the DVTARGETDEVICE if necessary 
		dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE)); 

		// copy the contents of the source DVTARGETDEVICE into dest->ptd 
		if (dest->ptd)
		{
			*(dest->ptd) = *(source->ptd); 
		}
	} 
} 

HRESULT __stdcall CEnumFormatEtc::QueryInterface (REFIID iid, void ** ppvObject)
{
	// Simplified version...
	// TEST_ NULL 
	if(ppvObject == NULL)
		return E_POINTER;  

	// Supports IID_IUnknown and IID_IDropTarget 
	if(iid == IID_IUnknown || 
		iid == IID_IEnumFORMATETC)
	{
		*ppvObject = (void *) this;
		AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE; 

}

ULONG   __stdcall CEnumFormatEtc::AddRef (void)
{
	return ++m_lRefCount;
}
ULONG   __stdcall CEnumFormatEtc::Release (void)
{
	ULONG c_Refs = --m_lRefCount;
	if (c_Refs == 0)
	{
		delete this;
	}
	return c_Refs;

}

CEnumFormatEtc::CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats) 
{ 
	m_lRefCount = 1; 
	m_nIndex = 0; 
	m_nNumFormats = nNumFormats; 
	m_pFormatEtc = new FORMATETC[nNumFormats]; 
	// make a new copy of each FORMATETC structure 
	for(int i = 0; i < nNumFormats; i++) 
	{ 
		DeepCopyFormatEtc (&m_pFormatEtc[i], &pFormatEtc[i]); 
	} 
} 

CEnumFormatEtc::~CEnumFormatEtc() 
{ 
	// first free any DVTARGETDEVICE structures 
	for(ULONG i = 0; i < m_nNumFormats; i++) 
	{ 
		if(m_pFormatEtc[i].ptd) 
			CoTaskMemFree(m_pFormatEtc[i].ptd); 
	} 
	// now free the main array 
	delete[] m_pFormatEtc; 
} 


HRESULT CEnumFormatEtc::Reset (void){ 
	m_nIndex = 0; 
	return S_OK;} 


HRESULT CEnumFormatEtc::Skip (ULONG celt){ 
	m_nIndex += celt; 
	return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;} 


HRESULT CEnumFormatEtc::Clone(IEnumFORMATETC **ppEnumFormatEtc) 
{ 
	HRESULT hResult; 
	// make a duplicate enumerator 
	hResult = CreateEnumFormatEtc(m_nNumFormats, m_pFormatEtc, ppEnumFormatEtc); 
	if(hResult == S_OK) 
	{ 
		// manually set the index state 
		((CEnumFormatEtc *)*ppEnumFormatEtc)->m_nIndex = m_nIndex; 
	} 
	return hResult; 
} 



HRESULT CEnumFormatEtc::Next(ULONG celt, FORMATETC *pFormatEtc, ULONG *pceltFetched) 
{ 
	ULONG copied = 0; 
	// copy the FORMATETC structures into the caller's buffer 
	while (m_nIndex < m_nNumFormats && copied < celt)  
	{ 
		DeepCopyFormatEtc (&pFormatEtc [copied], &m_pFormatEtc [m_nIndex]); 
		copied++; 
		m_nIndex++; 
	} 
	// store result 
	if(pceltFetched != 0)  
		*pceltFetched = copied; 
	// did we copy all that was requested? 
	return (copied == celt) ? S_OK : S_FALSE; 
} 



HRESULT CEnumFormatEtc::CreateEnumFormatEtc (UINT cfmt, FORMATETC *afmt, IEnumFORMATETC **ppEnumFormatEtc) 
{ 
	if (cfmt == 0 || afmt == 0 || ppEnumFormatEtc == 0) 
		return E_INVALIDARG; 
	*ppEnumFormatEtc = new CEnumFormatEtc (afmt, cfmt); 
	return (*ppEnumFormatEtc) ? S_OK: E_OUTOFMEMORY;} 
/////////////////////////////////////////////////////////////////////////////////////////
CDataObject::CDataObject ()
{
	// reference count must ALWAYS start at 1
	m_lRefCount    = 1;
	m_pDragControl = NULL;
	m_pDataImpl = new DataObjImpl;
}

CDataObject::~CDataObject ()
{
	RemoveAll();
	if (m_pDataImpl)
		delete m_pDataImpl;
	m_pDataImpl = NULL;
}

HRESULT __stdcall CDataObject::QueryInterface (REFIID iid, void ** ppvObject)
{
	// Simplified version...
	// TEST_ NULL 
	if(ppvObject == NULL)
		return E_POINTER;  

	// Supports IID_IUnknown and IID_IDropTarget 
	if(iid == IID_IUnknown || 
		iid == IID_IDataObject)
	{
		*ppvObject = (void *) this;
		AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE; 

}

ULONG   __stdcall CDataObject::AddRef (void)
{
	return ++m_lRefCount;
}
ULONG   __stdcall CDataObject::Release (void)
{
	ULONG c_Refs = --m_lRefCount;
	if (c_Refs == 0)
	{
		delete this;
	}
	return c_Refs;

}



HRESULT __stdcall CDataObject::QueryGetData(FORMATETC *pFormatEtc)
{
	return (LookupFormatEtc(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
}


int CDataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
{
	// 轮流检查格式看是否能找到匹配的格式
	int nCnt = (int)m_pDataImpl->m_pFormatEtc.size();
	for(int i = 0; i < nCnt; i++)
	{
		FORMATETC* p = m_pDataImpl->m_pFormatEtc[i];
		if(( p->tymed    &  pFormatEtc->tymed)   &&
			p->cfFormat == pFormatEtc->cfFormat &&
			p->dwAspect == pFormatEtc->dwAspect)
		{
			// return index of stored format
			return i;
		}
	}

	// error, format not found
	return -1;
}



void CDataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
{
	switch(pMedSrc->tymed)
	{
	case TYMED_HGLOBAL:
		pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_GDI:
		pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_MFPICT:
		pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_ENHMF:
		pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_FILE:
		pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_ISTREAM:
		pMedDest->pstm = pMedSrc->pstm;
		pMedSrc->pstm->AddRef();
		break;
	case TYMED_ISTORAGE:
		pMedDest->pstg = pMedSrc->pstg;
		pMedSrc->pstg->AddRef();
		break;
	case TYMED_NULL:
	default:
		break;
	}
	pMedDest->tymed = pMedSrc->tymed;
	pMedDest->pUnkForRelease = NULL;
	if(pMedSrc->pUnkForRelease != NULL)
	{
		pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
		pMedSrc->pUnkForRelease->AddRef();
	}
}


HRESULT __stdcall CDataObject::GetData (FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium)
{
	if (m_pDragControl)
	{
		TEventUI event = {0};
		event.nType = UIEVENT_DRAG_DROP_GROUPS;
		event.pSender = m_pDragControl;
		event.wParam = (WPARAM)UIEVENT_DRAG_QUERYDATA_DELAY;
		event.lParam = (LPARAM)this;
		m_pDragControl = NULL;
		event.pSender->SendEvent(event);
	};
	int idx;
	// try to match the specified FORMATETC with one of our supported formats
	if((idx = LookupFormatEtc(pFormatEtc)) == -1)
		return DV_E_FORMATETC;
	// found a match - transfer data into supplied storage medium
	pStgMedium->tymed           = m_pDataImpl->m_pFormatEtc[idx]->tymed;
	pStgMedium->pUnkForRelease  = 0;
	// copy the data into the caller's storage medium
	FORMATETC* pFmtSrc = m_pDataImpl->m_pFormatEtc[idx];
	CopyMedium(pStgMedium, m_pDataImpl->m_pStgMedium[idx], pFmtSrc);
	return S_OK;
}


HRESULT __stdcall CDataObject::EnumFormatEtc (DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
{
	// OLE仅仅支持得到方向成员
	if(dwDirection == DATADIR_GET)
	{
		// 在WIN2K下，你可以调用AIP函数SHCreateStdEnumFmtEtc来完成，但为了支持//所有的window平台，我们需要实现IEnumFormatEtc。
		FORMATETC* afmt = new FORMATETC[m_pDataImpl->m_pFormatEtc.size()];
		int nCnt = (int)m_pDataImpl->m_pFormatEtc.size();
		for (int i = 0; i < nCnt; i++)
			afmt[i] = *(m_pDataImpl->m_pFormatEtc[i]);
		HRESULT hr =  CEnumFormatEtc::CreateEnumFormatEtc(m_pDataImpl->m_pFormatEtc.size(), afmt, ppEnumFormatEtc);
		delete[] afmt;
		return hr;
	}
	else
	{
		// the direction specified is not supported for drag+drop
		return E_NOTIMPL;
	}
}


HRESULT CDataObject::DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT CDataObject::DUnadvise (DWORD dwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT CDataObject::EnumDAdvise (IEnumSTATDATA **ppEnumAdvise)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT CDataObject::GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
	return DATA_E_FORMATETC;
}

HRESULT CDataObject::GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
{
	// Apparently we have to set this field to NULL even though we don't do anything else
	pFormatEtcOut->ptd = NULL;
	return E_NOTIMPL;
}

HRESULT CDataObject::SetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium,  BOOL fRelease)
{
	if(pFormatEtc == NULL || pMedium == NULL)
		return E_INVALIDARG;

	FORMATETC* pLocalFormatEtc   = new FORMATETC;
	*pLocalFormatEtc = *pFormatEtc;
	m_pDataImpl->m_pFormatEtc.push_back(pLocalFormatEtc);

	STGMEDIUM* pLocalStgMedium   = new STGMEDIUM;
	if (fRelease)
	{
		*pLocalStgMedium = *pMedium;
	}
	else
	{
		CopyMedium(pLocalStgMedium, pMedium, pFormatEtc);
	}
	m_pDataImpl->m_pStgMedium.push_back(pLocalStgMedium);
	return S_OK;
}


bool  CDataObject::SetTextData(LPCTSTR lpszText)
{
	if (!lpszText) return false;
	FORMATETC fmtetc = {CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};  
	STGMEDIUM stgmed = {TYMED_HGLOBAL, {0}, 0}; 
	wstring  str16 = Charset::TCHARToUnicode(lpszText);
    int bytes = static_cast<int>(str16.size() + 1) * sizeof(wstring::value_type);
    HGLOBAL hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, bytes);
	if (!hgDrop) return false;
	void* data = GlobalLock(hgDrop);
	if (data)
	{
		size_t allocated = static_cast<size_t>(GlobalSize(hgDrop ));
		memcpy(data, str16.c_str(), allocated);
		static_cast<wstring::value_type*>(data)[str16.size()] = '\0';
	}
	GlobalUnlock(hgDrop);
	stgmed.hGlobal = hgDrop;
	SetData(&fmtetc, &stgmed, true);
	string  str = Charset::NativeToUTF8(lpszText);
	bytes = static_cast<int>(str.size() + 1) * sizeof(string::value_type);
	hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, bytes);
	if (!hgDrop) return false;
	data = GlobalLock(hgDrop);
	if (data)
	{
		size_t allocated = static_cast<size_t>(GlobalSize(hgDrop ));
		memcpy(data, str.c_str(), allocated);
		static_cast<string::value_type*>(data)[str.size()] = '\0';
	}
	GlobalUnlock(hgDrop);
	fmtetc.cfFormat = CF_UNICODETEXT;
	stgmed.hGlobal = hgDrop;
    return SetData(&fmtetc, &stgmed, true) == S_OK;
}

bool  CDataObject::SetHTMLData(LPCTSTR lpszHTML)
{
	string html = Charset::NativeToUTF8(lpszHTML);
	char *buf = new char [400 + strlen(html.c_str())];
	//if(!buf) return false;

	// Get clipboard id for HTML format...
	// Create a template string for the HTML header...
	strcpy(buf,
		"Version:0.9\r\n"
		"StartHTML:00000000\r\n"
		"EndHTML:00000000\r\n"
		"StartFragment:00000000\r\n"
		"EndFragment:00000000\r\n"
		"<!doctype html><html><body>\r\n"
		"<!--StartFragment --><DIV>\r\n");

	// Append the HTML...
	strcat(buf, html.c_str());
	strcat(buf, "\r\n");
	// Finish up the HTML format...
	strcat(buf,
		"</DIV><!--EndFragment-->\r\n"
		"</body>\r\n"
		"</html>");

	// Now go back, calculate all the lengths, and write out the
	// necessary header information. Note, wsprintf() truncates the
	// string when you overwrite it so you follow up with code to replace
	// the 0 appended at the end with a '\r'...
	char *ptr = strstr(buf, "StartHTML");
	sprintf(ptr+10, "%08u", strstr(buf, "<html>") - buf);
	*(ptr+10+8) = '\r';

	ptr = strstr(buf, "EndHTML");
	sprintf(ptr+8, "%08u", strlen(buf));
	*(ptr+8+8) = '\r';

	ptr = strstr(buf, "StartFragment");
	sprintf(ptr+14, "%08u", strstr(buf, "<!--StartFrag") - buf);
	*(ptr+14+8) = '\r';

	ptr = strstr(buf, "EndFragment");
	sprintf(ptr+12, "%08u", strstr(buf, "<!--EndFrag") - buf);
	*(ptr+12+8) = '\r';

	html = buf;
	delete [] buf;


	int bytes = static_cast<int>(html.size() + 1) * sizeof(string::value_type);
	HGLOBAL hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, bytes);
	if (!hgDrop) 
		return false;

	void* data = GlobalLock(hgDrop);
	if (data)
	{
		size_t allocated = static_cast<size_t>(GlobalSize(hgDrop ));
		memcpy(data, html.c_str(), allocated);
		static_cast<string::value_type*>(data)[html.size()] = '\0';
	}
	GlobalUnlock(hgDrop);

	FORMATETC fmtetc = {RegisterClipboardFormat(_T("HTML Format")), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};  
	STGMEDIUM stgmed = {TYMED_HGLOBAL, {0}, 0}; 
	stgmed.hGlobal = hgDrop;

	return SetData(&fmtetc, &stgmed, true) == S_OK;
}
bool  CDataObject::SetFileData(vector<tstring>& vtFileList)
{
	if (vtFileList.size() <= 0) return false;
	HGLOBAL        hgDrop;
	DROPFILES*     pDrop;
	UINT           uBuffSize = 0;
	TCHAR*         pszBuff;
	int nCnt = (int)vtFileList.size();
	for (int i = 0; i < nCnt; i++)
	{
		LPCTSTR lpszPath = vtFileList[i].c_str();
		 uBuffSize += lstrlen ( lpszPath ) + 1;
	}

	uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);
    hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );
	if (!hgDrop) return false;
	pDrop = (DROPFILES*) GlobalLock ( hgDrop );

	if (!pDrop )
	{
		GlobalFree ( hgDrop );
		return false;
	}
	pDrop->pFiles = sizeof(DROPFILES);
#ifdef _UNICODE
	pDrop->fWide = TRUE;
#endif
	 pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));
	 int nFileCnt = (int)vtFileList.size();
	 for (int i = 0; i < nFileCnt; i++)
	 {
		 LPCTSTR lpszPath = vtFileList[i].c_str();
		 lstrcpy ( pszBuff, lpszPath );
		 pszBuff = 1 + _tcschr ( pszBuff, '\0' );
	 }
	 GlobalUnlock ( hgDrop );

	 FORMATETC fmtetc = {CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};  
	 STGMEDIUM stgmed = {TYMED_HGLOBAL, {0}, 0};  
	 stgmed.hGlobal = hgDrop;
	 return SetData(&fmtetc, &stgmed, true) == S_OK;

}

bool CDataObject::SetHBitmapData(HBITMAP hBitmap)
{
	if (!hBitmap) return false;
	FORMATETC fmtetc = {CF_BITMAP, 0, DVASPECT_CONTENT, -1, TYMED_GDI};  
	STGMEDIUM stgmed = {0};
	stgmed.tymed = TYMED_GDI; // Storage medium = HBITMAP handle 
	stgmed.pUnkForRelease = NULL; // Use ReleaseStgMedium
	stgmed.hBitmap = hBitmap;//(HBITMAP)OleDuplicateData(hBitmap, fmtetc.cfFormat, NULL);
	return SetData(&fmtetc, &stgmed, false) == S_OK;
}

bool  CDataObject::SetUserData(int nFormat, LPVOID lpData, int  nSize)
{
	HGLOBAL hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, nSize);
	if (!hgDrop) return false;

	void* data = GlobalLock(hgDrop);
	//size_t allocated = static_cast<size_t>(GlobalSize(hgDrop ));
	memcpy(data, lpData, nSize);
	GlobalUnlock(hgDrop);

	FORMATETC fmtetc = {nFormat, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};  
	STGMEDIUM stgmed = {TYMED_HGLOBAL, {0}, 0};  
	stgmed.hGlobal = hgDrop;
	return SetData(&fmtetc, &stgmed, true) == S_OK;

}
void  CDataObject::RemoveAll()
{
	if (m_pDataImpl)
		m_pDataImpl->RemoveAll();
}