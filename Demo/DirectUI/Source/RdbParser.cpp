#include "StdAfx.h"
#include "RdbParser.h"


#define KEY 1487555 

using namespace std;

static void encrypt(char* plainText, int size, int key)
{
	for (int i = 0; i < size; i++)
	{
		int c = plainText[i];
		c = c ^ key;
		plainText[i] = c;
	}
}

static void decrypt(char* cipherText, int size, int key)
{
	for (int i = 0; i < size; i++)
	{
		int c = cipherText[i];
		c = c ^ key;
		cipherText[i] = c;
	}
}

CRdbBuf::CRdbBuf()
{
	m_pBuf = NULL;
	m_nSize = 0;
}

CRdbBuf::~CRdbBuf()
{
	Destroy();
}

void CRdbBuf::Destroy()
{
	if( m_pBuf )
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
	}
	m_nSize = 0;
}


bool CRdbBuf::Create(const void* data, size_t data_len)
{
	Destroy();
	m_pBuf = new char[data_len + 1];
// 	if( NULL == m_pBuf )
// 		return false;

	memcpy(m_pBuf, data, data_len);
	m_pBuf[data_len] = '\0';

	m_nSize = data_len;
	return true;
}

size_t	
SafeStrncpy(TCHAR* dest, const TCHAR* src, size_t n)
{
	size_t i = 0;
	TCHAR* ret = dest;

	if(n == 0 || NULL == src){
		dest[0] = 0; return 0;
	}

	for( i = 0; i < n; ++i ){
		if( 0 == (ret[i] = src[i]) ) return (i);
	}

	dest[n] = 0;	
	return n;
}


bool 
DirExist(const TCHAR *path)
{
	bool b = false;
	TCHAR tmp[MAX_PATH];

	size_t n = SafeStrncpy(tmp, path, sizeof(tmp) / sizeof(TCHAR) -2);

	for( int i=0; i< (int)n; ++i){
		if(tmp[i] == '/') 
			tmp[i] = '\\';
	}

	if( n > 1 && tmp[n-1] == '\\' ) {
		tmp[n-1] = 0;
	}

	WIN32_FIND_DATA fd; 
	HANDLE hFind = FindFirstFile(tmp, &fd);

	if ( hFind != INVALID_HANDLE_VALUE ) {

		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			b = true;
	}

	FindClose(hFind);
	return b;
}

bool 
DirCreate(const TCHAR * path)
{
	if( DirExist(path) )
		return true;

	int i, n;
	TCHAR szPath[MAX_PATH] = {0};
	TCHAR szTmp[MAX_PATH] = {0};

	n = (int)SafeStrncpy( szTmp, path, sizeof(szTmp)-1 );

	for( i=0; i<n; i++ ) {		

		if( szTmp[i] == '\\' || szTmp[i] == '/' ) {

			SafeStrncpy( szPath, szTmp, i );
			::CreateDirectory( szPath, NULL );
		}
	}

	return DirExist(path);
}


CRdbParser::CRdbParser(void)
{
	m_hFile       = INVALID_HANDLE_VALUE;
	m_dwSize      = INVALID_FILE_SIZE;
	m_hFileMap    = NULL;
	m_pMapAddress = NULL;
	m_pFileHead = NULL;
}

CRdbParser::~CRdbParser(void)
{
	Close();
}

BOOL  CRdbParser::Open(LPBYTE lpData, DWORD dwSize)
{
	Close();
	m_pMapAddress = new BYTE[dwSize];
	if (!m_pMapAddress) return false;
	memcpy(m_pMapAddress, lpData, dwSize);
	m_dwSize = dwSize;
	return RefreshResList();
}

BOOL CRdbParser::Open(LPCTSTR lpszFileName, DWORD dwShare)
{
	Close();

	BOOL bIsOpen = FALSE;
	m_hFile = ::CreateFile(lpszFileName, GENERIC_READ, dwShare, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_READONLY, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return bIsOpen;

	m_dwSize = ::GetFileSize(m_hFile,NULL);

	m_hFileMap = ::CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (NULL == m_hFileMap)
	{
		::CloseHandle (m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		return bIsOpen;
	}

	m_pMapAddress = (LPBYTE)::MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
	if (NULL == m_pMapAddress)
	{
		Close();
		return bIsOpen;
	}

	bIsOpen = RefreshResList();

	return bIsOpen;
}

void CRdbParser::Close(void)
{
	// 从文件中读取
	if (m_hFileMap)
	{
		if (m_pMapAddress != NULL)
			UnmapViewOfFile (m_pMapAddress) ;
		if (m_hFileMap != NULL)
			CloseHandle (m_hFileMap) ;
		if (m_hFile != INVALID_HANDLE_VALUE)
			CloseHandle (m_hFile);
	}
	else // 从内存中读取
	{
		delete []m_pMapAddress;
	}

	m_hFileMap    = NULL ;
	m_pMapAddress = NULL ;
	m_hFile       = INVALID_HANDLE_VALUE ;
	m_dwSize      = INVALID_FILE_SIZE;
	FreeResList();
}

BOOL CRdbParser::RefreshResList(void)
{
	FreeResList();
	if( !IsOpen() )
		return FALSE;

	LPFILEHEAD lpFileHead = GetFileHead();
	if( NULL == lpFileHead )
		return FALSE;

	if (0 != memcmp(lpFileHead->Head, RDB_HEADER, RDB_HEAD_SIZE))
	{
		if (0 != memcmp(lpFileHead->Head, RDB_HEADER_ENCRYPT, RDB_HEAD_SIZE))
		{
			return FALSE;
		}
		else
		{
			decrypt((char*)lpFileHead + RDB_HEAD_SIZE, sizeof(FILEHEAD) - RDB_HEAD_SIZE, KEY);
		}
	}

	DWORD dwPos = sizeof(FILEHEAD);
	LPBYTE lpBaseStart = m_pMapAddress + (lpFileHead->BaseAddr + lpFileHead->Offset);
	LPRESHEAD lpResHead = NULL;
	for( DWORD i = 0; i < lpFileHead->IndexNum; ++i )
	{
		lpResHead = (LPRESHEAD)malloc(sizeof(RESHEAD));
		if ( NULL == lpResHead )
			return FALSE;

		lpResHead->lwpName = LPWSTR(m_pMapAddress + dwPos);
		dwPos += (wcslen(lpResHead->lwpName) + 1 ) * 2;

		lpResHead->SartAddr= lpBaseStart+(*((INT64*)(m_pMapAddress+dwPos)));
		dwPos += sizeof(INT64);

		lpResHead->nSize = *((INT64*)(m_pMapAddress+dwPos));
		dwPos += sizeof(INT64);

		tstring strName = lpResHead->lwpName;
		StrUtil::ToLowerCase(strName);
		m_mapRes[strName] = lpResHead;
	}

	return TRUE;
}

void CRdbParser::FreeResList(void)
{
	TmapResHead mapTemp;
	mapTemp.swap(m_mapRes);
	TmapResHead::iterator i = mapTemp.begin();
	for ( ; i != mapTemp.end(); ++i )
	{
		if ( i->second )
		{
			free(i->second);
		}
	}
	if (m_pFileHead)
		delete m_pFileHead;
	m_pFileHead = NULL;
}

LPFILEHEAD CRdbParser::GetFileHead()
{ 
	if (m_pFileHead == nullptr)
	{
		m_pFileHead = new FILEHEAD;
		memcpy((LPBYTE)m_pFileHead, m_pMapAddress, sizeof(FILEHEAD));
	}
	return m_pFileHead; 
}

tstring CRdbParser::GetFileExt(LPBYTE lpDate)
{
	tstring strExt;

	if ( 0 == memcmp(lpDate, PNG_HEADER, sizeof(PNG_HEADER)) )
		strExt = _T(".png");
	else if ( 0 == memcmp(lpDate, BMP_HEADER, sizeof(BMP_HEADER)) )
		strExt = _T(".bmp");
	else if ( 0 == memcmp(lpDate, GIF_HEADER, sizeof(GIF_HEADER)) )
		strExt = _T(".gif");
	else if ( 0 == memcmp(lpDate, JPG_HEADER, sizeof(JPG_HEADER)) )
		strExt = _T(".jpg");
	else if ( 0 == memcmp(lpDate, ICO_HEADER, sizeof(ICO_HEADER)) )
		strExt = _T(".ico");
	else if ( 0 == memcmp(lpDate, CUR_HEADER, sizeof(CUR_HEADER)) )
		strExt = _T(".cur");

	return strExt;
}

BOOL CRdbParser::SaveResFile(LPCTSTR lpszFileName, LPRESHEAD lpResHead,BOOL SaveTGFInfo)
{
	if ( NULL == m_pMapAddress ) return FALSE;
	if ( NULL == lpResHead ) return FALSE;
	
	HANDLE hFile = NULL;

	//		if(lpResHead->nSize>0)
	
	tstring strFileName = lpszFileName;
	LPBYTE lpSartAddr = lpResHead->SartAddr;
	INT64  nSize  = lpResHead->nSize;
	LPTGFHEAD lpTGFHead = (LPTGFHEAD)lpSartAddr;

	//不保存TGF文件头信息,并且文件头信息是TGF文件
	if (!SaveTGFInfo && memcmp(lpTGFHead->TGF, TGF_HEADER, TGF_HEAD_SIZE)==0)
	{
		lpSartAddr	+=	lpTGFHead->Offset;
		nSize		-=	lpTGFHead->Offset;
		strFileName	+=	GetFileExt(lpSartAddr);
	}

	hFile = ::CreateFile(strFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile && hFile != INVALID_HANDLE_VALUE)
	{
		DWORD nNumberToWrite = 0;
		WriteFile(hFile,lpSartAddr,(DWORD)nSize,&nNumberToWrite,NULL);
		CloseHandle (hFile);
		return nNumberToWrite==(DWORD)nSize;
	}

	
	return FALSE;
}

IStream* CRdbParser::SaveToStream(LPRESHEAD lpResHead,BOOL SaveTGFInfo)
{
	IStream* pStream = NULL;
	if(m_pMapAddress && lpResHead)
	{
		LPBYTE lpSartAddr = lpResHead->SartAddr;
		INT64  nSize  = lpResHead->nSize;
		LPTGFHEAD lpTGFHead = (LPTGFHEAD)lpSartAddr;
		//不保存TGF文件头信息,并且文件头信息是TGF文件
		if (!SaveTGFInfo && memcmp(lpTGFHead->TGF,TGF_HEADER,TGF_HEAD_SIZE)==0)
		{
			lpSartAddr +=lpTGFHead->Offset;
			nSize      -=lpTGFHead->Offset;
		}
		if (GetFileExt(lpSartAddr).empty()) return pStream;

		HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, (DWORD)nSize);
		if (hMem)
		{
			LPBYTE lpDate = (LPBYTE)GlobalLock(hMem);
			if (lpDate)
			{
				memcpy(lpDate,lpSartAddr,(size_t)nSize);
				CreateStreamOnHGlobal(hMem, TRUE, &pStream);
			}
			GlobalUnlock(hMem);
		}

		if (!pStream) GlobalFree(hMem);
	}
	return pStream;
}


BOOL CRdbParser::Unpack(LPCTSTR lpszDbFileName, LPCTSTR lpszReleaseFolder)
{
	if ( !Open(lpszDbFileName) )
		return FALSE;	

	tstring strFolderPath = lpszReleaseFolder;
	tstring strFileName;
	tstring strPath;

	if( !m_mapRes.empty() )
	{
		LPRESHEAD lpResHead = NULL;
		TmapResHead::iterator pos = m_mapRes.begin();
		for ( ; pos != m_mapRes.end(); ++pos )
		{
			lpResHead = (LPRESHEAD)pos->second;
			if ( NULL == lpResHead ) continue;

			strFileName = strFolderPath + lpResHead->lwpName;

			/*strPath = StrUtil::Left(strFileName, strFileName.rfind(L'\\')+1).c_str();*/
			strPath = strFileName.substr(0, strFileName.rfind(L'\\')+1);

			if ( DirCreate( strPath.c_str() ) )
			{
				if(SaveResFile(strFileName.c_str(), lpResHead))
				{
					continue;
				}
			}

			strFileName = lpResHead->lwpName;

// 			if (AfxMessageBox(L"保存"+FileName+L"文件失败!\r\n是否继续导出?",MB_YESNO)==IDYES)
// 				continue;
// 			else
// 				break;

		}
		/*AfxMessageBox(_T("导出文件完成!"));*/
	}

	return TRUE;
}

void CRdbParser::FormatFileName(tstring& strFileName)
{
	StrUtil::Trim(strFileName);
	/*util::StrUtil::ToLowerCase(strFileName);*/
	StrUtil::Replace(strFileName, _T("/"), _T("\\"));
}

INT64 CRdbParser::GetFileSize(LPCTSTR lpszFileName)
{
	LPRESHEAD lpResHead = NULL;
	if ( !GetResHead(&lpResHead, lpszFileName) ) return 0;

	return lpResHead->nSize;
}

BOOL CRdbParser::FileExist(LPCTSTR lpszFileName)
{
	if ( NULL == lpszFileName || NULL == lpszFileName[0] ) return FALSE;

	tstring strFileName = lpszFileName;
	StrUtil::ToLowerCase(strFileName);
	FormatFileName(strFileName);
	TmapResHead::iterator i = m_mapRes.find(strFileName);
	if ( i != m_mapRes.end())
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CRdbParser::ReadFile(CRdbBuf* pRdbBuf, LPCTSTR lpszFileName)
{
	if ( NULL == pRdbBuf )
	{
		return FALSE;
	}
	pRdbBuf->Destroy();

	LPRESHEAD lpResHead = NULL;
	if ( !GetResHead(&lpResHead, lpszFileName) ) return FALSE;

	if ( NULL == m_pMapAddress ) return FALSE;

	LPBYTE lpSartAddr = lpResHead->SartAddr;
	INT64  nSize  = lpResHead->nSize;
	LPTGFHEAD lpTGFHead = (LPTGFHEAD)lpSartAddr;

	pRdbBuf->Create(lpSartAddr, nSize);

	return TRUE;
}

BOOL CRdbParser::ReadFile(LPCTSTR lpszFileName, char* lpBuf, int nLen)
{
	LPRESHEAD lpResHead = NULL;
	if ( !GetResHead(&lpResHead, lpszFileName) ) return FALSE;

	if ( NULL == m_pMapAddress ) return FALSE;

	LPBYTE lpSartAddr = lpResHead->SartAddr;
	INT64  nSize  = lpResHead->nSize;
	LPTGFHEAD lpTGFHead = (LPTGFHEAD)lpSartAddr;

	if ( nLen < nSize ) return FALSE;

	memcpy(lpBuf, lpSartAddr, (size_t)nSize);
	
	return TRUE;
}

BOOL CRdbParser::GetResHead(LPRESHEAD* ppOut, LPCTSTR lpszFileName)
{
	if ( NULL == ppOut ) return FALSE;
	if ( NULL == lpszFileName || NULL == lpszFileName[0] ) return FALSE;

	tstring strFileName = lpszFileName;
	StrUtil::ToLowerCase(strFileName);
	FormatFileName(strFileName);
	TmapResHead::iterator i = m_mapRes.find(strFileName);
	if ( i != m_mapRes.end())
	{
		*ppOut = i->second;
		return TRUE;
	}
	return FALSE;
}

void CRdbParser::SetCoexistence(bool bCoexistence)
{
	m_bCoexistence = bCoexistence;
}

bool CRdbParser::IsCoexistence()
{
	return m_bCoexistence;
}
