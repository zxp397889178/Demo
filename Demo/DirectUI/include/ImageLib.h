#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////
//{{
#ifdef _GDIPLUS_SUPPORT
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
#endif
//}}


struct PaintParams;
class DUI_API CImageLib
{
// 图片导入导出
public:
	 // 图片导入成HBITMAP
	 static void LoadImage(LPCTSTR lpszPath, OUT HBITMAP* hBitmap);
	 static void LoadImage(LPVOID lpData, long nSize, OUT HBITMAP* hBitmapp);
	 static void LoadImage(UINT uResID, LPCTSTR lpszType, OUT HBITMAP* hBitmapp);

	 // 图像导入成像素
	 static void LoadImage(LPCTSTR lpszPath, OUT LPBYTE* lppDataOut, OUT int* nWidth, OUT int* nHeight);
	 static void LoadImage(LPVOID lpData, long nSize, OUT LPBYTE* lppDataOut, OUT int* nWidth, OUT int* nHeight);
	 static void LoadImage(UINT uResID, LPCTSTR lpszType, OUT LPBYTE* lppDataOut, OUT int* nWidth, OUT int* nHeight);

	 // 保存图片
	 static bool SaveImage(HBITMAP hBitmap, LPCTSTR lpszPath);
	 static bool SaveImage(LPBYTE lpData, int nWidth, int nHeight, LPCTSTR lpszPath);
	 static bool SaveImage(ImageObj* imageObj, LPCTSTR lpszPath);

	 // 图片导入成Bitmap
	 //{{
	#ifdef _GDIPLUS_SUPPORT
	 //}}
	 static void LoadImage(LPCTSTR lpszPath, OUT Bitmap** ppBitmap, OUT IStream**ppStream);
	 static void LoadImage(LPVOID lpData, long nSize, OUT Bitmap** ppBitmap, OUT IStream**ppStream);
	 static void LoadImage(UINT uResID, LPCTSTR lpszType, OUT Bitmap** ppBitmap, OUT IStream**ppStream);
	 static bool SaveImage(Image* bitmap, LPCTSTR lpszPath);

	 static HBITMAP BitmapToHBITMAP(Bitmap* bitmap);
	 static Bitmap* HBITMAPToBitmap(HBITMAP hBitmap);
	 static BYTE* BitmapToPixels(Bitmap* bitmap);
	 static Bitmap* PixelsToBitmap(BYTE* pPixels, int nWidth, int nHeight);

	 static BYTE*  LockBitmapBits(Bitmap* bitmap, BitmapData& data);
	 static int  GetBitmapFrameCount(Bitmap* bitmap);
	//{{	
	#endif
	 //}}

 // 图片处理
public:
	static void DoGray(ImageObj* pImageObj);
	static void DoMask(ImageObj* pImageObj, ImageObj* pImageMaskObj);
	static void DoMaskForRoundRect(ImageObj* pImageObj, ImageObj* pImageMaskObj, int nRadius);
	static void DoResize(ImageObj* pImageObj, int nWidth, int nHeight, PaintParams* params = NULL);
	static void DoHLS(ImageObj* pImageObj, double nHue, double nSaturation, double lightness);

};


