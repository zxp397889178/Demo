/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2010-10-22       xqb
*********************************************************************/
#pragma once


class DataObjImpl;
//////////////////////////////////////////////////////////
class DUI_API CDataObject : public IDataObject
{
	//{{
	friend class CDropSource;
public:
	// IUnknown members
	HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject);
	ULONG   __stdcall AddRef (void);
	ULONG   __stdcall Release (void);

	// IDataObject members
	HRESULT __stdcall GetData (FORMATETC *pFormatEtc, STGMEDIUM *pmedium);
	HRESULT __stdcall GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pmedium);
	HRESULT __stdcall QueryGetData (FORMATETC *pFormatEtc);
	HRESULT __stdcall GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut);
	HRESULT __stdcall SetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium,  BOOL fRelease);
	HRESULT __stdcall EnumFormatEtc (DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc);
	HRESULT __stdcall DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *, DWORD *);
	HRESULT __stdcall DUnadvise (DWORD      dwConnection);
	HRESULT __stdcall EnumDAdvise (IEnumSTATDATA **ppEnumAdvise);
		// Constructor / Destructor
	CDataObject ();
	virtual ~CDataObject ();
	//}}

	// 设置数据
	bool  SetTextData(LPCTSTR lpszText);
	bool  SetFileData(vector<tstring>& vtFileList);
	bool  SetHBitmapData(HBITMAP hBitmap);
	bool  SetHTMLData(LPCTSTR lpszHTML);
	bool  SetUserData(int nFormat, LPVOID lpData, int  nSize);
	void  RemoveAll();
	//{{
private:
	DataObjImpl* m_pDataImpl;
	LONG m_lRefCount;
	CControlUI* m_pDragControl;
	int LookupFormatEtc(FORMATETC *pFormatEtc);
	void CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc);
	//}}
};
