#include "stdafx.h"
#include "ImageLib.h"
#include "CheckImageHeader.h"

#ifdef _GDIPLUS_SUPPORT
struct gdiStartup  
{
	GdiplusStartupInput mGdiplusStartupInput;
	ULONG_PTR mGdiplusToken; 
	gdiStartup()
	{
		GdiplusStartup(&mGdiplusToken, &mGdiplusStartupInput, NULL);
	}
	~gdiStartup()
	{
		GdiplusShutdown(mGdiplusToken);
	}
};
//gdiStartup startup;

//return null if alloc failed
#ifndef new_nothrow
#define new_nothrow  new(std::nothrow)
#endif

void CImageLib::LoadImage(LPCTSTR lpszPath, OUT Bitmap** ppBitmap, OUT IStream**ppStream)
{
	*ppBitmap = NULL;
	*ppStream = NULL;

	if (!IsImage(lpszPath))
		return;
	
	HANDLE hFile = CreateFile(lpszPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return;
	DWORD nSize = GetFileSize(hFile, NULL);
	if (nSize == 0)
	{
		CloseHandle(hFile);
		return;
	}
	BYTE* lpData = new_nothrow BYTE[nSize];
	if (lpData == NULL)
	{
		CloseHandle(hFile);
		return;
	}

	::ReadFile(hFile, lpData, nSize, &nSize, NULL);
	CloseHandle(hFile);

	CImageLib::LoadImage(lpData, nSize, ppBitmap, ppStream);

	if (*ppBitmap == NULL && *ppStream != NULL)
	{
		(*ppStream)->Release();
		DUILOG(TRACE) << "CImageLib::LoadImage error:" << Charset::UnicodeTochar(lpszPath) << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
	}
	delete []lpData;

}

void CImageLib::LoadImage(LPVOID lpData, long nSize,  OUT Bitmap** ppBitmap, OUT IStream**ppStream)
{
	*ppBitmap = NULL;
	*ppStream = NULL;
	Bitmap* bitmap = NULL;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hGlobal)
	{
		LPVOID pBuffer = GlobalLock(hGlobal);
		memcpy(pBuffer, lpData, nSize);
		GlobalUnlock(hGlobal);
		IStream* pStream = NULL;
		if(CreateStreamOnHGlobal(hGlobal, true, &pStream) == S_OK)
		{
			bitmap = new Bitmap(pStream);
			if (bitmap && bitmap->GetLastStatus() != 0)
			{
				delete bitmap;
				pStream->Release();
				pStream = NULL;
				bitmap = NULL;
			}
			*ppStream = pStream;
		}
		else
		{
			GlobalFree(hGlobal);
		}
	}
	// 强制转换成PixelFormat32bppARGB
	/*	if (bitmap  && bitmap->GetPixelFormat() !=  PixelFormat32bppARGB && GetBitmapFrameCount(bitmap) < 2)
	   {
	   BYTE* lpImageData = BitmapToPixels(bitmap);
	   Bitmap* bitmapNew = PixelsToBitmap(lpImageData, bitmap->GetWidth(), bitmap->GetHeight());

	   int nSize = bitmap->GetPropertyItemSize(PropertyTagOrientation);
	   if (nSize > 0)
	   {
	   PropertyItem* item = (PropertyItem*) malloc(nSize);
	   bitmap->GetPropertyItem(PropertyTagOrientation, nSize, item);
	   bitmapNew->SetPropertyItem(item);
	   free(item);
	   }

	   delete bitmap;
	   delete  []lpImageData;
	   bitmap = bitmapNew;

	   if (bitmap && (bitmap->GetWidth() == 0 || bitmap->GetHeight() == 0))
	   {
	   delete bitmap;
	   bitmap = NULL;
	   }
	   }*/

	do
	{
		if (!bitmap)
		{
			break;
		}

		if (bitmap->GetPixelFormat() == PixelFormat32bppARGB)
		{
			break;
		}

		Bitmap* bmpTransformed = bitmap->Clone(0, 0, bitmap->GetWidth(), bitmap->GetHeight(), PixelFormat32bppPARGB);
		if (!bmpTransformed)
		{
			delete bitmap;
			bitmap = NULL;
			DUILOG(TRACE) << "bitmap->Clone error:" << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
			break;
		}

		delete bitmap;
		bitmap = bmpTransformed;

	} while (false);


	*ppBitmap = bitmap; 
}

void CImageLib::LoadImage(UINT uResID, LPCTSTR lpszType, OUT Bitmap** ppBitmap, OUT  IStream** ppStream)
{
	*ppStream = NULL;
	*ppBitmap = NULL;
	//Bitmap* bitmap = NULL;
	HRSRC hRsrc = ::FindResource(GetEngineObj()->GetInstanceHandle(), MAKEINTRESOURCE(uResID), lpszType);
	if (hRsrc)
	{
		HGLOBAL hImgData = ::LoadResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
		if (hImgData)
		{
			LPVOID lpVoid = ::LockResource(hImgData);
			DWORD nSize = ::SizeofResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
			LPBYTE lpData = new BYTE[nSize];
			::memcpy(lpData, lpVoid, nSize);
			::FreeResource(hImgData);
			CImageLib::LoadImage(lpData, nSize, ppBitmap, ppStream);
			delete [] lpData;
		}	
	}
}

bool CImageLib::SaveImage(Image* bitmap, LPCTSTR lpszPath)
{
	if (!lpszPath || !bitmap)
		return false;
	tstring strExt =  lpszPath;
	int i = strExt.rfind(L'.');
	if (i > 0)
		strExt = strExt.substr(i + 1);
	else
		strExt = _T("");

	int nRet = -1;
	if (equal_icmp(strExt.c_str(), _T("jpg")))
	{
		long nQuality = 90;
		EncoderParameters     encoderParameters; 
		encoderParameters.Count   =   1; 
		encoderParameters.Parameter[0].Guid   =   EncoderQuality; 
		encoderParameters.Parameter[0].Type   =   EncoderParameterValueTypeLong; 
		encoderParameters.Parameter[0].NumberOfValues   =   1; 
		encoderParameters.Parameter[0].Value   =   &nQuality;
		GUID clsid = {0x557CF401, 0x1A04, 0x11D3, 0x9A, 0x73, 0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E};
		nRet = bitmap->Save(Charset::TCHARToUnicode(lpszPath).c_str(), &clsid, &encoderParameters);//
	}
	else if (equal_icmp(strExt.c_str(), _T("png")))
	{
		GUID clsid = {0x557cf406, 0x1a04, 0x11d3, 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e};
		nRet = bitmap->Save(Charset::TCHARToUnicode(lpszPath).c_str(), &clsid);
	}
	else if (equal_icmp(strExt.c_str(),  _T("gif")))
	{
		GUID clsid = {0x557cf402, 0x1a04, 0x11d3, 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e};
		nRet = bitmap->Save(Charset::TCHARToUnicode(lpszPath).c_str(), &clsid);
	}
	else 
	{
		GUID clsid = {0x557cf400, 0x1A04, 0x11D3, 0x9A, 0x73, 0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E};
		nRet = bitmap->Save(Charset::TCHARToUnicode(lpszPath).c_str(), &clsid);
	}
	return  nRet == 0;
}

bool CImageLib::SaveImage(LPBYTE lpData, int nWidth, int nHeight, LPCTSTR lpszPath)
{
	Bitmap* bitmap = PixelsToBitmap(lpData, nWidth, nHeight);
	if (!bitmap) return false;
	bool bSucc = SaveImage(bitmap, lpszPath);
	delete bitmap;
	return bSucc;
}

 bool CImageLib::SaveImage(ImageObj* imageObj, LPCTSTR lpszPath)
 {
	 if (!imageObj) return false;
	 if (imageObj->GetHBitmap()) return SaveImage(imageObj->GetHBitmap(), lpszPath);
	 if (imageObj->GetBitmap()) return SaveImage(imageObj->GetBitmap(), lpszPath);
	 if (imageObj->GetPixels()) return SaveImage(imageObj->GetPixels(), imageObj->GetWidth(), imageObj->GetHeight(), lpszPath);
	 return false;

 }
HBITMAP CImageLib::BitmapToHBITMAP(Bitmap* bitmap)
{
	if (!bitmap) return NULL;
	BYTE* bmpData = NULL;
	int w = bitmap->GetWidth();
	int h = bitmap->GetHeight();
	BITMAPINFO info = {0};     
	info.bmiHeader.biSize = sizeof(info.bmiHeader);     
	info.bmiHeader.biWidth = w;     
	info.bmiHeader.biHeight = -h;     
	info.bmiHeader.biPlanes = 1;     
	info.bmiHeader.biBitCount = 32;     
	info.bmiHeader.biCompression = BI_RGB;     
	info.bmiHeader.biSizeImage = w * h * 4;     
	HBITMAP hBmpSection = ::CreateDIBSection(NULL, &info, DIB_RGB_COLORS, (void**)&bmpData, NULL, 0);     
	if (!hBmpSection) return NULL;
	BitmapData data;   
	data.Scan0 = NULL;
	data.Height = h;     
	data.PixelFormat = bitmap->GetPixelFormat();    
	data.Scan0 = bmpData;
	data.Stride = (w * 32 / 8);     
	data.Width = w  ; 
	Rect rct(0, 0, w, h);     
	bitmap->LockBits(&rct, ImageLockModeRead|ImageLockModeUserInputBuf, PixelFormat32bppPARGB, &data);  
	if (bitmap->GetLastStatus() != Ok)
	{
		DeleteObject(hBmpSection);
		return NULL;
	}
	bitmap->UnlockBits(&data); 
	return hBmpSection;	

}

Bitmap* CImageLib::HBITMAPToBitmap(HBITMAP hBitmap)
{
	if (!hBitmap) return NULL;
	BITMAP bitmap; 
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	return PixelsToBitmap((BYTE*)bitmap.bmBits, bitmap.bmWidth, bitmap.bmHeight);
}

BYTE*  CImageLib::BitmapToPixels(Bitmap* bitmap)
 {
	 if (!bitmap) return NULL;
	 BYTE* bmpData = NULL;
	 int w = bitmap->GetWidth();
	 int h = bitmap->GetHeight();
	
	 bmpData = new BYTE[w * h * 4];
	 BitmapData data;   
	 data.Scan0 = NULL;
	 data.Height = h;     
	 data.PixelFormat = bitmap->GetPixelFormat();    
	 data.Scan0 = bmpData;
	 data.Stride = (w * 32 / 8);     
	 data.Width = w  ; 
	 Rect rct(0, 0, w, h);     
	 bitmap->LockBits(&rct, ImageLockModeRead|ImageLockModeUserInputBuf, PixelFormat32bppPARGB, &data);  
	 if (bitmap->GetLastStatus() != Ok)
	 {
		 delete [] bmpData;
		 return NULL;
	 }
	 bitmap->UnlockBits(&data); 
	 return  bmpData;	

 }

BYTE* CImageLib::LockBitmapBits(Bitmap* bitmap, BitmapData& data)
{
	data.Scan0 = NULL;
	if (!bitmap) return NULL;
	int w = bitmap->GetWidth();
	int h = bitmap->GetHeight();
	data.Height = h;     
	data.PixelFormat = bitmap->GetPixelFormat();    
	data.Stride = (w * 32 / 8);     
	data.Width = w ; 
	Rect rct(0, 0, w, h);     
	bitmap->LockBits(&rct, ImageLockModeRead|ImageLockModeWrite, PixelFormat32bppPARGB, &data);  
	if (bitmap->GetLastStatus() != Ok)
	{
		data.Scan0 = NULL;
		return NULL;
	}
	return (BYTE*)data.Scan0;
}

int  CImageLib::GetBitmapFrameCount(Bitmap* bitmap)
{
	if (!bitmap) return 0;

	UINT nCount = bitmap->GetFrameDimensionsCount();
	GUID* pDimensionIDs = new GUID[nCount];
	bitmap->GetFrameDimensionsList(pDimensionIDs, nCount);
	int nFrameCount = bitmap->GetFrameCount(pDimensionIDs);
	delete[] pDimensionIDs;
	return nFrameCount;
}

Bitmap* CImageLib::PixelsToBitmap(BYTE* pPixels, int nWidth, int nHeight)
{
	if (!pPixels) return NULL;
	Bitmap* bitmap = new Bitmap(nWidth, nHeight, PixelFormat32bppPARGB);
	//if (!bitmap) return NULL;
	BitmapData data = {0};
	BYTE* bmpData = LockBitmapBits(bitmap, data);
	if (bmpData)
	{
		memcpy(bmpData, pPixels, nWidth * nHeight * 4);
		bitmap->UnlockBits(&data); 
	}
	return  bitmap;	
}
#endif

void CImageLib::LoadImage(LPCTSTR lpszPath, OUT HBITMAP* phBitmap)
{
	*phBitmap = NULL;
#ifdef _GDIPLUS_SUPPORT
	Bitmap* bitmap = NULL;
	bitmap = Gdiplus::Bitmap::FromFile(lpszPath);
	if (bitmap && bitmap->GetLastStatus() != 0) 
	{	
		delete bitmap;
		return;
	}
	*phBitmap = BitmapToHBITMAP(bitmap);
	delete bitmap;
#else
#endif
}

void CImageLib::LoadImage(LPCTSTR lpszPath,  OUT LPBYTE* lppDataOut, OUT int* nWidth, OUT int* nHeight)
{
	*nWidth = 0;
	*nHeight = 0;
	*lppDataOut = NULL;
#ifdef _GDIPLUS_SUPPORT
	Bitmap* bitmap = NULL;
	bitmap = Gdiplus::Bitmap::FromFile(lpszPath);
	if (!bitmap)
		return;

	if (bitmap && bitmap->GetLastStatus() != 0) 
	{	
		delete bitmap;
		return;
	}

	BYTE* bmpData = NULL;
	int w = bitmap->GetWidth();
	int h = bitmap->GetHeight();
	BITMAPINFO info = {0};     
	info.bmiHeader.biSize = sizeof(info.bmiHeader);     
	info.bmiHeader.biWidth = w;     
	info.bmiHeader.biHeight = -h;     
	info.bmiHeader.biPlanes = 1;     
	info.bmiHeader.biBitCount = 32;     
	info.bmiHeader.biCompression = BI_RGB;     
	info.bmiHeader.biSizeImage = w * h * 4;    

	bmpData =  new BYTE[w * h * 4];

	BitmapData data;   
	data.Scan0 = NULL;
	data.Height = h;     
	data.PixelFormat = bitmap->GetPixelFormat();    
	data.Scan0 = bmpData;
	data.Stride = (w * 32 / 8);     
	data.Width = w  ; 
	Rect rct(0, 0, w, h);     
	bitmap->LockBits(&rct, ImageLockModeRead|ImageLockModeUserInputBuf, PixelFormat32bppPARGB, &data);  
	if (bitmap->GetLastStatus() != Ok)
	{
		delete[] bmpData;
		return;
	}
	bitmap->UnlockBits(&data); 
	*nWidth = w;
	*nHeight = h;
	*lppDataOut = bmpData;
#else
#endif
}


void CImageLib::LoadImage(LPVOID lpData, long nSize, OUT LPBYTE* lppDataOut,  OUT int* nWidth, OUT int* nHeight)
{
	*nWidth = 0;
	*nHeight = 0;
	*lppDataOut = NULL;
#ifdef _GDIPLUS_SUPPORT
	Bitmap* bitmap = NULL;
	IStream* pStream = NULL;
	LoadImage(lpData, nSize, &bitmap, &pStream);
	if (!bitmap) return;
	BYTE* bmpData = NULL;
	int w = bitmap->GetWidth();
	int h = bitmap->GetHeight();
	BITMAPINFO info = {0};     
	info.bmiHeader.biSize = sizeof(info.bmiHeader);     
	info.bmiHeader.biWidth = w;     
	info.bmiHeader.biHeight = -h;     
	info.bmiHeader.biPlanes = 1;     
	info.bmiHeader.biBitCount = 32;     
	info.bmiHeader.biCompression = BI_RGB;     
	info.bmiHeader.biSizeImage = w * h * 4;    

	bmpData =  new BYTE[w * h * 4];

	BitmapData data;   
	data.Scan0 = NULL;
	data.Height = h;     
	data.PixelFormat = bitmap->GetPixelFormat();    
	data.Scan0 = bmpData;
	data.Stride = (w * 32 / 8);     
	data.Width = w  ; 
	Rect rct(0, 0, w, h);     
	bitmap->LockBits(&rct, ImageLockModeRead|ImageLockModeUserInputBuf, PixelFormat32bppPARGB, &data);  
	if (bitmap->GetLastStatus() != Ok)
	{
		if (pStream) pStream->Release();
		delete[] bmpData;
		return;
	}
	bitmap->UnlockBits(&data); 
	if (pStream) pStream->Release();
	*nWidth = w;
	*nHeight = h;
	*lppDataOut = bmpData;
#else
	LPBYTE pImage = NULL;
	int x,y,n;
	pImage = stbi_load_from_memory((const unsigned char *)lpData, nSize, &x, &y, &n, 4);

	if( !pImage ) return;

	BITMAPINFO bmi;
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = x;
	bmi.bmiHeader.biHeight = -y;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = x * y * 4;

	//bool bAlphaChannel = false;
	LPBYTE pDest = new BYTE[x * y * 4];
	for( int i = 0; i < x * y; i++ ) 
	{
		pDest[i*4 + 3] = pImage[i*4 + 3];
		if( pDest[i*4 + 3] < 255 )
		{
			pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
			pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
			pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
			//bAlphaChannel = true;
		}
		else
		{
			pDest[i*4] = pImage[i*4 + 2];
			pDest[i*4 + 1] = pImage[i*4 + 1];
			pDest[i*4 + 2] = pImage[i*4]; 
		}

		/*if( *(DWORD*)(&pDest[i*4]) == mask ) {
		pDest[i*4] = (BYTE)0;
		pDest[i*4 + 1] = (BYTE)0;
		pDest[i*4 + 2] = (BYTE)0; 
		pDest[i*4 + 3] = (BYTE)0;
		bAlphaChannel = true;
		}*/
	}
	stbi_image_free(pImage);
	*nWidth = x;
	*nHeight = y;
	*lppDataOut = pDest;
#endif

}

void CImageLib::LoadImage(UINT uResID, LPCTSTR lpszType, OUT LPBYTE* lppDataOut,  OUT int* nWidth, OUT int* nHeight)
{
	*nWidth = 0;
	*nHeight = 0;
	*lppDataOut = NULL;

	HRSRC hRsrc = ::FindResource(GetEngineObj()->GetInstanceHandle(), MAKEINTRESOURCE(uResID), lpszType);
	if (hRsrc)
	{
		HGLOBAL hImgData = ::LoadResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
		if (hImgData)
		{
			LPVOID lpVoid = ::LockResource(hImgData);
			DWORD dwSize = ::SizeofResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
			LoadImage(lpVoid, dwSize, lppDataOut, nWidth, nHeight);
			::FreeResource(hImgData);
		}
		
	}
}

void CImageLib::LoadImage(LPVOID lpData, long nSize, OUT HBITMAP* phBitmap)
{
	*phBitmap = NULL;
#ifdef _GDIPLUS_SUPPORT
	Bitmap* bitmap = NULL;
	IStream* pStream = NULL;
	CImageLib::LoadImage(lpData, nSize, &bitmap, &pStream);
	if (bitmap)
	{
		*phBitmap = BitmapToHBITMAP(bitmap);
		delete bitmap;
		if (pStream) pStream->Release();
	}
#else
	
#endif
}

void CImageLib::LoadImage(UINT uResID, LPCTSTR lpszType, OUT HBITMAP* hBitmap)
{
	HRSRC hRsrc = ::FindResource(GetEngineObj()->GetInstanceHandle(), MAKEINTRESOURCE(uResID), lpszType);
	if (hRsrc)
	{
		HGLOBAL hImgData = ::LoadResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
		if (hImgData)
		{
			LPVOID lpVoid = ::LockResource(hImgData);
			DWORD dwSize = ::SizeofResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
			LoadImage(lpVoid, dwSize, 	hBitmap);		
			::FreeResource(hImgData);
		}
	
	}
}

bool CImageLib::SaveImage(HBITMAP hBitmap, LPCTSTR lpszPath)
{
	if (!lpszPath || !hBitmap)
		return false;

#ifdef _GDIPLUS_SUPPORT
	Bitmap* bitmap = Bitmap::FromHBITMAP(hBitmap, NULL);
	bool bSucc = SaveImage(bitmap, lpszPath);
	if (bitmap) delete bitmap;
	return bSucc;
#endif
	return false;
}

void CImageLib::DoGray(ImageObj* pImageObj)
{
	if (!pImageObj)
		return;
	LPVOID lpLockData;
	LPBYTE piexl = pImageObj->LockBits(&lpLockData);
	if (!piexl) return;
	CSSE::DoGray((LPBYTE)piexl, pImageObj->GetWidth() * pImageObj->GetHeight() * 4);
	pImageObj->UnLockBits(lpLockData);
}

void CImageLib::DoMask(ImageObj* pImageObj, ImageObj* pImageMaskObj)
{
	if (!pImageObj || !pImageMaskObj)
		return;

	int w = pImageObj->GetWidth();
	int h = pImageObj->GetHeight();
	int wMask = pImageMaskObj->GetWidth();
	int hMask = pImageMaskObj->GetHeight();
	if ((w != wMask)|| (h != hMask)) return;

	// 获取蒙版像素
	LPVOID lpLockDataMask;
	LPBYTE piexlMask = pImageMaskObj->LockBits(&lpLockDataMask);
	if (!piexlMask) return;

	LPVOID lpLockData;
	LPBYTE piexl = pImageObj->LockBits(&lpLockData);
	if (!piexl)
	{
		pImageMaskObj->UnLockBits(lpLockDataMask);
		return;
	}

	// 两张图片一样大小的情况
	if ((w == wMask) && (h == hMask))
	{
		int nSize = w * h * 4;
		for (int i = 0; i < nSize; i+=4)
		{
			piexl[3] = piexlMask[3] * piexl[3] / 255;
			piexl[0] = piexl[0] * piexl[3] / 255;
			piexl[1] = piexl[1] * piexl[3] / 255;
			piexl[2] = piexl[2] * piexl[3] / 255;
			piexl += 4;
			piexlMask += 4;
		}
	}
	pImageObj->UnLockBits(lpLockData);
	pImageMaskObj->UnLockBits(lpLockDataMask);
}

void CImageLib::DoMaskForRoundRect(ImageObj* pImageObj, ImageObj* pImageMaskObj, int nRadius)
{
	LPVOID lpLockData;
	LPVOID lpLockDataMask;
	LPBYTE piexl = pImageObj->LockBits(&lpLockData);
	BYTE*  piexl1 = pImageMaskObj->LockBits(&lpLockDataMask);

	int w = pImageObj->GetWidth();
	int h = pImageObj->GetHeight();
	int nWideBytes = pImageObj->GetWidth() * 4;

	int w1 = pImageMaskObj->GetWidth();
	int h1 = pImageMaskObj->GetHeight();
	int nWideBytes1 = w1 * 4;

	unsigned char alpha;
	unsigned char div = 255;
	BYTE* p;
	BYTE* p1;
	int index;


	for (int j = 0; j < nRadius; j++)
	{
		p = piexl + nWideBytes * j;
		p1 = piexl1 +  nWideBytes1 * j;
		for (int i = 0; i < nRadius; i++)
		{
			index = i; index <<= 2;
			alpha = p1[i * 4 + 3];
			alpha = alpha * p[index + 3 ] / 255;

			p[index + 3 ] = alpha;
			p[index ] = p[index ] * alpha / div;
			p[index + 1] = p[index + 1] * alpha / div;
			p[index + 2] =p[index + 2] * alpha / div;	
		}
	}

	for (int j = h - nRadius; j < h; j++)
	{
		p = piexl + nWideBytes * j;
		p1 = piexl1 +  nWideBytes1 * (h1 - ( h - j));
		for (int i = 0; i < nRadius; i++)
		{
			index = i; index <<= 2;
			alpha = p1[i * 4 + 3];
			alpha = alpha * p[index + 3 ] / 255;

			p[index + 3 ] = alpha;
			p[index ] = p[index ] * alpha / div;
			p[index + 1] = p[index + 1] * alpha / div;
			p[index + 2] =p[index + 2] * alpha / div;

		}
	}

	for (int j = 0; j < nRadius; j++)
	{
		p = piexl + nWideBytes * j;
		p1 = piexl1 +  nWideBytes1 * j;
		for (int i = w - nRadius; i < w; i++)
		{
			index = i; index <<= 2;
			alpha = p1[(w1 - (w - i)) * 4 + 3];
			alpha = alpha * p[index + 3 ] / 255;

			p[index + 3 ] = alpha;
			p[index ] = p[index ] * alpha / div;
			p[index + 1] = p[index + 1] * alpha / div;
			p[index + 2] =p[index + 2] * alpha / div;

		}
	}

	for (int j = h -nRadius; j < h; j++)
	{
		p = piexl + nWideBytes * j;
		p1 = piexl1 +  nWideBytes1 * (h1 - ( h - j));
		for (int i = w - nRadius; i < w; i++)
		{
			index = i; index <<= 2;
			alpha = p1[(w1 - (w - i)) * 4 + 3];
			alpha = alpha * p[index + 3 ] / 255;

			p[index + 3 ] = alpha;
			p[index ] = p[index ] * alpha / div;
			p[index + 1] = p[index + 1] * alpha / div;
			p[index + 2] =p[index + 2] * alpha / div;
		}
	}

	pImageObj->UnLockBits(lpLockData);
	pImageMaskObj->UnLockBits(lpLockDataMask);
}


void  CImageLib::DoHLS(ImageObj* pImageObj, double nHue, double nSaturation, double lightness)
{
	if (!pImageObj)
		return;
	LPVOID lpLockData;
	LPBYTE piexl = pImageObj->LockBits(&lpLockData);
	if (!piexl) return;
	CSSE::AdjustHLS(piexl, pImageObj->GetWidth() * pImageObj->GetHeight() * 4, nHue, nSaturation, lightness);
	pImageObj->UnLockBits(lpLockData);

}

void CImageLib::DoResize(ImageObj* pImageObj, int nWidth, int nHeight, PaintParams* params/* = NULL*/)
{
	if (!pImageObj)
		return;
	if (pImageObj->GetWidth() == nWidth && pImageObj->GetHeight() == nHeight) return;

	DibObj dibobj;
	HDC hDC = GetDC(NULL);
	dibobj.Create(hDC, nWidth, nHeight);
	dibobj.Fill(RGB(0, 0, 0), 0);

	IRenderDC render;
	render.SetDevice(&dibobj);
	RECT rectDst = {0, 0, nWidth, nHeight};
	RECT rectSrc = {0, 0, pImageObj->GetWidth(), pImageObj->GetHeight()};
	render.DrawImage(pImageObj, rectDst, rectSrc, params);

	if (pImageObj->GetHBitmap())
		pImageObj->SetHBitmap(dibobj.Detach());
	else if (pImageObj->GetPixels())
	{
		BYTE* pData = new BYTE[nWidth * nHeight * 4];
		memcpy(pData, dibobj.GetBits(), nWidth * nHeight * 4);
		pImageObj->SetPixels(pData, nWidth, nHeight);
	}
	else if (pImageObj->GetBitmap())
	{
		Bitmap* bitmap = PixelsToBitmap(dibobj.GetBits(), nWidth, nHeight);
		pImageObj->SetBitmap(bitmap);
	}

	::ReleaseDC(NULL, hDC);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
