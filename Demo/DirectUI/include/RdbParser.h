#pragma once

#include "RdbDef.h"

#include <map>
#include <vector>

class DUI_API CRdbBuf
{;
public:
	CRdbBuf();
	 ~CRdbBuf();
	bool Create(const void* data, size_t data_len);
	void Destroy();
	char* GetBuf(){return m_pBuf;};
	size_t GetSize(){return  m_nSize;};

protected:
	char* m_pBuf;
	size_t m_nSize;
};


class DUI_API CRdbParser
{
public:
	CRdbParser(void);
	virtual ~CRdbParser(void);

public:
	/// 
	BOOL Open(LPCTSTR lpszFileName, DWORD dwShare = FILE_SHARE_READ);
	BOOL Open(LPBYTE lpData, DWORD dwSize);
	/// 
	void Close();
	/// 
	BOOL IsOpen() { return m_pMapAddress != NULL; }
	/// 
	BOOL FileExist(LPCTSTR lpszFileName);
	/// 
	BOOL ReadFile(CRdbBuf* pRdbBuf, LPCTSTR lpszFileName);

	void SetCoexistence(bool bCoexistence);

	bool IsCoexistence();

protected:
	/// 
	INT64 GetFileSize(LPCTSTR lpszFileName);
	/// 
	BOOL ReadFile(LPCTSTR lpszFileName, char* lpBuf, int nLen);
	/// 
	BOOL Unpack(LPCTSTR lpszDbFileName, LPCTSTR lpszReleaseFolder);
	/// 
	BOOL RefreshResList();
	/// 
	void FreeResList();
	/// 
	LPFILEHEAD GetFileHead();
	/// 
	BOOL SaveResFile(LPCTSTR FileName, LPRESHEAD lpResHead,BOOL SaveTGFInfo=TRUE);
	/// 
	IStream* SaveToStream(LPRESHEAD lpResHead,BOOL SaveTGFInfoF=TRUE);
	/// 
	BOOL GetResHead(LPRESHEAD* ppOut, LPCTSTR lpszFileName);
	/// 
	tstring GetFileExt(LPBYTE lpDate);
	/// 
	void FormatFileName(tstring& strFileName);

private:
	DWORD m_dwSize;
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	LPBYTE m_pMapAddress;
	bool m_bCoexistence = false;
	LPFILEHEAD m_pFileHead;

	typedef std::map<tstring, LPRESHEAD> TmapResHead;
	TmapResHead m_mapRes;
};
