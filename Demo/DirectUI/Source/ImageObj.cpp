///////////////////////////////////////////////////////
//文件修改履历：
///////////////////////////////////////////////////////
#include "stdafx.h"
#include "ImageObj.h"

static int image_obj_count_ = 0;
ImageObj* ImageObj::Get()
{
	::InterlockedIncrement(&m_reflong);
	return this;
}

void ImageObj::Release()
{
	if (::InterlockedDecrement(&m_reflong) <= 0)
	{
		if (m_bCached) 
			GetResObj()->RemoveImageFromTable(m_strFilePath.c_str());
		delete this;
	}
}

LPCTSTR ImageObj::GetFilePath()
{
	return m_strFilePath.c_str();
}


void  ImageObj::SetFilePath(LPCTSTR lpszPath)
{
	m_strFilePath = lpszPath;
}

#ifdef _GDIPLUS_SUPPORT
static GUID g_GUIDFrameDimensionTime = FrameDimensionTime;
ImageObj::ImageObj(Bitmap* bitmap, IStream* pStream)
{
	image_obj_count_++;
	//DUILOG(TRACE) << "ImageObj Count:" << image_obj_count_ << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
	Init();
	SetBitmap(bitmap, pStream);

}

void ImageObj::SetBitmap(Bitmap* bitmap, IStream* pStream)
{
	Desotry();
	if (!bitmap) return;
	// 获取帧数、设置当前帧
	m_nFrameCount = CImageLib::GetBitmapFrameCount(bitmap);
	m_nWidth = bitmap->GetWidth();
	m_nHeight = bitmap->GetHeight();
	m_iCellHeight = m_nHeight / m_iNumCellRows;
	m_iCellWidth = m_nWidth / m_iNumCellColumns;
	m_pBitmap = bitmap;
	m_pStream = pStream;
	if (m_nFrameCount > 1)
	{
		int nSize = m_pBitmap->GetPropertyItemSize(PropertyTagFrameDelay);
		m_pPropertyItem = (PropertyItem*) malloc(nSize);
		m_pBitmap->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
	}
	m_eImageType = UIIAMGE_BITMAP;
}

Bitmap*  ImageObj::GetBitmap()
{
	return  m_pBitmap;
}

IStream* ImageObj::GetIStream()
{
	return m_pStream;
}
#endif

ImageObj::ImageObj(HBITMAP hBitmap)
{
	image_obj_count_++;
	//DUILOG(TRACE) << "ImageObj Count:" << image_obj_count_ << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
	Init();
	SetHBitmap(hBitmap);
}

void ImageObj::SetHBitmap(HBITMAP hBitmap)
{
	Desotry();
	if (!hBitmap) return;
	BITMAP bitmap; 
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	m_nWidth = bitmap.bmWidth;
	m_nHeight = bitmap.bmHeight;
	m_hBitmap = hBitmap;
	m_iCellHeight = m_nHeight / m_iNumCellRows;
	m_iCellWidth = m_nWidth / m_iNumCellColumns;
	m_eImageType = UIIAMGE_HBITMAP;

}

HBITMAP  ImageObj::GetHBitmap()
{
	return m_hBitmap;
}

ImageObj::ImageObj(HICON hIcon)
{
	image_obj_count_++;
	//DUILOG(TRACE) << "ImageObj Count:" << image_obj_count_ << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
	Init();
	SetHIcon(hIcon);	
}

void ImageObj::SetHIcon(HICON hIcon)
{
	Desotry();
	if (!hIcon) return;
	m_hIcon = hIcon;
	ICONINFO icInfo = { 0 };
	::GetIconInfo(hIcon, &icInfo);
	BITMAP bitmap;
	GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);
	if (icInfo.hbmColor)
		DeleteObject(icInfo.hbmColor);
	icInfo.hbmColor = NULL;

	if (icInfo.hbmMask)
		DeleteObject(icInfo.hbmMask);
	icInfo.hbmMask = NULL;

	m_nWidth = bitmap.bmWidth;
	m_nHeight = bitmap.bmHeight;
	m_iCellHeight = m_nHeight / m_iNumCellRows;
	m_iCellWidth = m_nWidth / m_iNumCellColumns;
	m_eImageType = UIIAMGE_HICON;


}

HICON  ImageObj::GetHIcon()  
{
	return m_hIcon;
}

ImageObj::ImageObj(BYTE* pPixels, int nWidth, int nHeight)
{
	image_obj_count_++;
	//DUILOG(TRACE) << "ImageObj Count:" << image_obj_count_ << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
	Init();
	SetPixels(pPixels, nWidth, nHeight);
}

void ImageObj::SetPixels(BYTE* pPixels, int nWidth, int nHeight)
{
	Desotry();
	if (!pPixels) return;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_pPixels = pPixels;
	m_iCellHeight = m_nHeight / m_iNumCellRows;
	m_iCellWidth = m_nWidth / m_iNumCellColumns;
	m_eImageType = UIIAMGE_PIXEL;

}

BYTE*  ImageObj::GetPixels()
{
	return m_pPixels;
}

void  ImageObj:: Init()
{
	m_reflong = 0;
	::InterlockedIncrement(&m_reflong);
	m_nFrameCount = 1;
	m_hBitmap = NULL;
#ifdef _GDIPLUS_SUPPORT
	m_pBitmap = NULL;
	m_pPropertyItem = NULL;
#endif
	m_hIcon = NULL;
	m_bNeedAdjustColor = false;
	memset(&m_rcInset, 0, sizeof(m_rcInset));
	m_nWidth = 0;
	m_nHeight = 0;
	m_iNumCellRows = 1;
	m_iNumCellColumns = 1;
	m_pPixels = NULL;
	m_uFlags = 0;
	m_pStream = NULL;
	m_eImageType = UIIAMGE_PIXEL;
	m_bCached = false;
}

void  ImageObj::Desotry()
{
	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	m_hBitmap = NULL;

	if (m_hIcon)
		DestroyIcon(m_hIcon);
	m_hIcon = NULL;
#ifdef _GDIPLUS_SUPPORT
	if (m_pPropertyItem)
		free(m_pPropertyItem);
	m_pPropertyItem = NULL;
	if (m_pBitmap)
		delete m_pBitmap;
	m_pBitmap = NULL;

	if (m_pStream)
		m_pStream->Release();
	m_pStream = NULL;
#endif
	if (m_pPixels)
		delete []m_pPixels;
	m_pPixels = NULL;
	m_nFrameCount = 1;
}

ImageObj::~ImageObj(void)
{
	image_obj_count_--;
	//DUILOG(TRACE) << "ImageObj Count:" << image_obj_count_ << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
	//if (m_strFilePath != _T(""))
	//{
	//	DUILOG(TRACE) << "ImageObj release:" << Charset::UnicodeTochar(GetFilePath()) << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
	//}
	Desotry();
}

bool  ImageObj::Reload()
{
	Desotry();
	ImageObj*pImageObj = (ImageObj*)GetUIRes()->LoadImage(GetFilePath(), false, m_eImageType);
	if (!pImageObj) return false;

	m_rcInset = pImageObj->m_rcInset;
	m_bNeedAdjustColor = pImageObj->m_bNeedAdjustColor;
	Copy(pImageObj);
	SetCellNum(pImageObj->m_iNumCellRows, pImageObj->m_iNumCellColumns);	
	m_uFlags = 0;
	pImageObj->Detach();
	pImageObj->Release(); 
	return true;
}

void ImageObj::Copy(ImageObj* pImageObj)
{
	if (!pImageObj) return;
	if (pImageObj->GetHBitmap())
		SetHBitmap(pImageObj->GetHBitmap());
	else if (pImageObj->GetPixels())
		SetPixels(pImageObj->GetPixels(), pImageObj->GetWidth(), pImageObj->GetHeight());
	else if (pImageObj->GetBitmap())
		SetBitmap(pImageObj->GetBitmap(), pImageObj->GetIStream());
	else if (pImageObj->GetHIcon())
		SetHIcon(pImageObj->GetHIcon());
}

void ImageObj::Copy(DibObj* pDibObj)
{
	if (!pDibObj) return;
	if (m_eImageType == UIIAMGE_HBITMAP)
		SetHBitmap(pDibObj->Detach());
	else
	{
		BYTE* pixels = new BYTE[pDibObj->GetWidth() * pDibObj->GetHeight() * 4];
		memcpy(pixels, pDibObj->GetBits(), pDibObj->GetWidth() * pDibObj->GetHeight() * 4);
		SetPixels(pixels, pDibObj->GetWidth(), pDibObj->GetHeight());
	}

}

void ImageObj::Detach()
{
#ifdef _GDIPLUS_SUPPORT
	if (m_pPropertyItem)
		free(m_pPropertyItem);
	m_pPropertyItem = NULL;
	m_pBitmap = NULL;
	m_pStream = NULL;
#endif
	m_hBitmap = NULL;
	m_hIcon = NULL;
	m_pPixels = NULL;
}

BYTE* ImageObj::LockBits(LPVOID* lppvoid)
{
	*lppvoid = NULL;
	if (GetPixels()) return GetPixels();
	if (GetHBitmap())
	{
		DIBSECTION dibSrc = {0};
		if (::GetObject (GetHBitmap(), sizeof (DIBSECTION), &dibSrc) != sizeof (DIBSECTION) ||
			dibSrc.dsBm.bmBits == NULL)
		{
			RT_ASSERT(dibSrc.dsBm.bmBits != NULL, _T("ImageObj::LockBits failed"));
			return NULL;
		}
		return (LPBYTE)dibSrc.dsBm.bmBits;
	}
	if (GetBitmap())
	{
		BitmapData* pBitmapData = new BitmapData;
		BYTE* pData = CImageLib::LockBitmapBits(GetBitmap(), *pBitmapData);
		if (!pData)
		{
			delete pBitmapData;
			pBitmapData = NULL;
		}
		*lppvoid = pBitmapData;
		return pData;
	}
	return NULL;
}

void  ImageObj::UnLockBits(LPVOID lpvoid)
{
	if (!lpvoid) return;
	if (GetBitmap())
	{
		BitmapData* pBitmapData = (BitmapData*)lpvoid;
		GetBitmap()->UnlockBits(pBitmapData);
		delete pBitmapData;
	}
}


UINT ImageObj::GetFlags()
{
	return m_uFlags;
}
void ImageObj::SetFlags(UINT uFlag)
{
	m_uFlags = uFlag;
}


void ImageObj::SetActiveFrame(int nFrame)
{
#ifdef _GDIPLUS_SUPPORT
	if (!m_pBitmap) return;
	m_pBitmap->SelectActiveFrame(&g_GUIDFrameDimensionTime, nFrame);
#endif
}


long ImageObj::GetFrameDelay(int nFrame)
{
#ifdef _GDIPLUS_SUPPORT
	if (!m_pBitmap) return 0;
	if (!m_pPropertyItem) return 0;
	return ((long*) m_pPropertyItem->value)[nFrame] * 10;
#endif
	return 0;
}

void ImageObj::SetInset(RECT& rect)
{
	m_rcInset = rect;

}

void ImageObj::SetCellNum(int nRows, int nCols)
{
	 m_iNumCellRows = nRows;
	 m_iNumCellColumns = nCols;
	 m_iCellHeight = m_nHeight / m_iNumCellRows;
	 m_iCellWidth = m_nWidth / m_iNumCellColumns;
}

void  ImageObj::SetCached(bool bCached)
{
	m_bCached = bCached;
}
bool  ImageObj::IsCached()
{
	return m_bCached;
}

void ImageObj::SetAdjustColor(bool bAdjustColor)
{
	m_bNeedAdjustColor = bAdjustColor;
}

void ImageObj::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
	m_iCellWidth = m_nWidth / m_iNumCellColumns;

}

void ImageObj::SetHeight(int nHeight)
{
	m_nHeight = nHeight;
	m_iCellHeight = m_nHeight / m_iNumCellRows;
}

int ImageObj::GetCellNum()
{
	return max(m_iNumCellRows, m_iNumCellColumns);
}

void ImageObj::SetRightOrientation()
{
	if (!m_pBitmap)
		return;
	if (GetFrameCount() > 1)
		return;

	int nSize = m_pBitmap->GetPropertyItemSize(PropertyTagOrientation);
	if (nSize <= 0)
		return;

	PropertyItem* item = (PropertyItem*) malloc(nSize);
	m_pBitmap->GetPropertyItem(PropertyTagOrientation, nSize, item);
	unsigned short* value = (unsigned short*)item->value;
	unsigned short orientation = value[0];
	switch(orientation)
	{

	case 1:
		{

		}
		break;
	case 2:
		{
			m_pBitmap->RotateFlip(RotateFlipType::RotateNoneFlipX);

		}
		break;
	case 3:
		{
			m_pBitmap->RotateFlip(RotateFlipType::Rotate180FlipNone);

		}
		break;
	case 4:
		{
			m_pBitmap->RotateFlip(RotateFlipType::Rotate180FlipX);
		}
		break;
	case 5:
		{
			m_pBitmap->RotateFlip(RotateFlipType::Rotate90FlipX);
		}
		break;
	case 6:
		{
			m_pBitmap->RotateFlip(RotateFlipType::Rotate90FlipNone);

		}
		break;
	case 7:
		{
			m_pBitmap->RotateFlip(RotateFlipType::Rotate270FlipX);
		}
		break;
	case 8:
		{
			m_pBitmap->RotateFlip(RotateFlipType::Rotate270FlipNone);
		}
		break;
	}
	free(item);
	m_pBitmap->RemovePropertyItem(PropertyTagOrientation);
	SetWidth(m_pBitmap->GetWidth());
	SetHeight(m_pBitmap->GetHeight());
}