#include "stdafx.h"
//#include "ucPatchUserProfile.h"
//#include "ucGetUserProfile.h"
//#include "ucModifyPassword.h"
//#include "ucUser.h"
#include "UIDragImage.h"

#include "boost\format.hpp"
#include "boost\type_traits\add_rvalue_reference.hpp"


#include "UIEditEx.h"
#include "BmpData.h"



using namespace Gdiplus;

static const TCHAR StrPageForFormat[] = _T("{c #d24726}%d{/c}{c #666666}/%d{/c}");
static const TCHAR StrPageBkFormat[] = _T("%d/%d");

CDragImageUI::CDragImageUI()
{
	SetStyle(_T("Picture"));
	m_sizePicture = { 0, 0 };
}


CDragImageUI::~CDragImageUI()
{
	if (m_mapBitmaps.size())
	{
		for (auto it : m_mapBitmaps)
		{
			delete it.second;
			it.second = nullptr;
		}
		m_mapBitmaps.clear();
	}
}

void CDragImageUI::Init()
{
	__super::Init();
}

bool CDragImageUI::Event(TEventUI& msg)
{
	__super::Event(msg);
	if (msg.nType == UIEVENT_BUTTONDOWN)
	{
		m_pointLast = msg.ptMouse;
		m_bDrag = true;

	}
	else if (msg.nType == UIEVENT_BUTTONUP)
	{
		m_bDrag = false;
	}
	else if (msg.nType == UIEVENT_MOUSEMOVE && m_bDrag)
	{
		SIZE szMove;
		szMove.cx = msg.ptMouse.x - m_pointLast.x;//鼠标移动的水平距离
		szMove.cy = msg.ptMouse.y - m_pointLast.y;
		m_pointLast = msg.ptMouse;
		if (szMove.cx == 0
			&& szMove.cy == 0)
		{
			return true;
		}
		RECT rc = GetRect();
		if (m_rectPaint.Width > rc.right - rc.left)
		{
			m_rectPaint.X += szMove.cx;
		}
		if (m_rectPaint.Height > rc.bottom - rc.top)
		{
			m_rectPaint.Y += szMove.cy;
		}
		CheckOffet();
		Invalidate(false);
	}
	return true;
}

void CDragImageUI::SetImage(LPCTSTR sFile)
{
	m_strImagePath = sFile;
	if (m_mapBitmaps.size())
	{
		for (auto it : m_mapBitmaps)
		{
			delete it.second;
			it.second = nullptr;
		}
		m_mapBitmaps.clear();
	}
	m_nBitmapIndex = 0;
	m_mapBitmaps[0] = new Gdiplus::Bitmap(sFile);
	m_fAngle = 0;
	SetAttribute(_T("bk.image"), sFile);
	CalPicPos(sFile);
	Invalidate();
}

const tstring CDragImageUI::CalPicPos(const tstring & sFile)
{
	Gdiplus::Image* pImage = Gdiplus::Image::FromFile(sFile.c_str());
	if (!pImage)
	{
		return _T("");
	}

	m_fPercent = 1.0;
	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();
	m_sizePicture.Width = nWidth;
	m_sizePicture.Height = nHeight;
	delete pImage;
	pImage = NULL;

	RECT rcItem = GetRect();
	int nItemWidth = rcItem.right - rcItem.left;
	int nItemHeight = rcItem.bottom - rcItem.top;
	int nNewWidth = 0;
	int nNewHeight = 0;
	float fScale = (float)nWidth / (float)nHeight;//图片的正确比例
	float fLayoutScale = (float)nItemWidth / (float)nItemHeight;//图层的比例
	RECT rcDest;

		if (fScale > fLayoutScale)
		{
			nNewWidth = min(nItemWidth, nWidth);
			nNewHeight = (int)((float)nNewWidth / fScale);
		}
		else
		{
			nNewHeight = min(nItemHeight, nHeight);
			nNewWidth = (int)((float)nNewHeight * fScale);
		}

	rcDest.left = (nItemWidth - nNewWidth) / 2 + rcItem.left;
	rcDest.right = rcDest.left + nNewWidth;
	rcDest.top = (nItemHeight - nNewHeight) / 2 + rcItem.top;
	rcDest.bottom = rcDest.top + nNewHeight;


	m_rectPaint.X = rcDest.left;
	m_rectPaint.Y = rcDest.top;
	m_rectPaint.Width = rcDest.right - rcDest.left;
	m_rectPaint.Height = rcDest.bottom - rcDest.top;
	m_fPercent = 1.0;
	return _T("");

}

void CDragImageUI::ResizeImage(float fPercent)
{
	if (m_fnCb)
	{
		m_fnCb(fPercent);
	}
	if (fPercent == 1.0)
	{
		CalPicPos(m_strImagePath);
		Invalidate();
		return;
	}
	float fChange = fPercent / m_fPercent;
	m_fPercent = fPercent;
	int nWidthBegin = m_rectPaint.Width;
	int nHeightBegin = m_rectPaint.Height;
	m_rectPaint.Width *= fChange;
	m_rectPaint.Height *= fChange;
	int nWidthChange = m_rectPaint.Width * fChange - m_rectPaint.Width;
	int nHeightChange = m_rectPaint.Height * fChange - m_rectPaint.Height;
	RECT rc = GetRect();
	int nWidthContainer = rc.right - rc.left;
	int nHeightContainer = rc.bottom - rc.top;
	int nXCenter = (rc.left + rc.right) / 2;
	int nYCenter = (rc.top + rc.bottom) / 2;
	float fHPercent = (nXCenter - m_rectPaint.X) / m_rectPaint.Width;
	float fVPercent = (nYCenter - m_rectPaint.Y) / m_rectPaint.Height;
	if (m_rectPaint.Width <= nWidthContainer)
	{
		m_rectPaint.X = (nWidthContainer - m_rectPaint.Width) / 2 + rc.left;
	}
	else
	{
		if (fChange > 1)
		{
			m_rectPaint.X -= (fChange - 1) * nWidthBegin * fHPercent;
		}
		else
		{
			m_rectPaint.X += (1 - fChange) * nWidthBegin * fHPercent;
		}	 
	}

	if (m_rectPaint.Height <= nHeightContainer)
	{
		m_rectPaint.Y = (nHeightContainer - m_rectPaint.Height) / 2 + rc.top;
	}
	else
	{
		if (fChange > 1)
		{
			m_rectPaint.Y -= (fChange - 1) * nHeightBegin * fVPercent;
		}
		else
		{
			m_rectPaint.Y += (1 - fChange) * nHeightBegin * fVPercent;
		}
	}
	CheckOffet();
	Invalidate();

}

INT CheckValue(INT value)
{
	return (value & ~0xff) == 0 ? value : value > 255 ? 255 : 0;
}
//---------------------------------------------------------------------------  

// 线性亮度/对比度调整  
VOID LineBrightAndContrast(BitmapData *data, INT bright, INT contrast, BYTE threshold)
{
	if (bright == 0 && contrast == 0)
		return;

	FLOAT bv = bright <= -255 ? -1.0f : bright / 255.0f;
	if (bright > 0 && bright < 255)
		bv = 1.0f / (1.0f - bv) - 1.0f;

	FLOAT cv = contrast <= -255 ? -1.0f : contrast / 255.0f;
	if (contrast > 0 && contrast < 255)
		cv = 1.0f / (1.0f - cv) - 1.0f;

	BYTE values[256];
	for (INT i = 0; i < 256; i++)
	{
		INT v = contrast > 0 ? CheckValue(i + (INT)(i * bv + 0.5f)) : i;
		if (contrast >= 255)
			v = v >= threshold ? 255 : 0;
		else
			v = CheckValue(v + (INT)((v - threshold) * cv + 0.5f));
		values[i] = contrast <= 0 ? CheckValue(v + (INT)(v * bv + 0.5f)) : v;
	}

	PARGBQuad p = (PARGBQuad)data->Scan0;
	INT offset = data->Stride - data->Width * sizeof(ARGBQuad);

	for (UINT y = 0; y < data->Height; y++, p+= offset)
	{
		for (UINT x = 0; x < data->Width; x++, p++)
		{
			p->Blue = values[p->Blue];
			p->Green = values[p->Green];
			p->Red = values[p->Red];
		}
		auto byteP = (BYTE*)p;
	}
}


void CDragImageUI::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	auto it = m_mapBitmaps.find(m_nBitmapIndex);
	if (it == m_mapBitmaps.end())
	{
		return __super::Render(pRenderDC, rcPaint);
	}
	RECT rcItem = { (LONG)m_rectPaint.GetLeft(), (LONG)m_rectPaint.GetTop(), (LONG)m_rectPaint.GetRight(), (LONG)m_rectPaint.GetBottom() };
	RECT rcTemp;
	if (!::IntersectRect(&rcTemp, &rcPaint, &rcItem))
	{
		return;
	}
	
	ImageStyle * pStyle = GetImageStyle(_T("bk"));
	ImageObj * m_pImageObj = pStyle->GetImageObj();

	if (!m_pImageObj)
	{
		return;
	}
	
	RectF rc{ 0, 0, 0, 0 };
	rc.X = max(m_rectPaint.X, GetRect().left);
	rc.Width = min(m_rectPaint.GetRight(), GetRect().right) - rc.X;
	rc.Y = max(m_rectPaint.GetTop(), GetRect().top);
	rc.Height = min(m_rectPaint.GetBottom(), GetRect().bottom) - rc.Y;

	Graphics g(pRenderDC->GetDC());

	float fPercentY = m_sizePicture.Height / (m_rectPaint.Height);
	float fPercentX = m_sizePicture.Width / (m_rectPaint.Width);

	POINT imgRotateCenterPos = { m_rectPaint.X + m_rectPaint.Width / 2, m_rectPaint.Y + m_rectPaint.Height / 2 }; //旋转中心在图片坐标(相对于图片本身)
	//把 相对于图片的旋转中心坐标  转换为  绘制区域的坐标
	g.TranslateTransform(imgRotateCenterPos.x, imgRotateCenterPos.y); //源点移动到旋转中心
	g.RotateTransform(m_fAngle); //旋转
	g.TranslateTransform(-imgRotateCenterPos.x, -imgRotateCenterPos.y);//还原源点

	g.SetInterpolationMode(Gdiplus::InterpolationModeDefault);
	m_pImageObj->SetActiveFrame(pStyle->GetImgSeq());
	int n = m_pImageObj->GetFrameCount();

	if (n > 1)//动画资源
	{
		g.DrawImage(m_pImageObj->GetBitmap(), m_rectPaint,
			(REAL)0, (REAL)0, (REAL)(m_sizePicture.Width), (REAL)(m_sizePicture.Height),
			Gdiplus::UnitPixel);
	}
	else
	{
		g.DrawImage(it->second, m_rectPaint,
			(REAL)0, (REAL)0, (REAL)(m_sizePicture.Width), (REAL)(m_sizePicture.Height),
			Gdiplus::UnitPixel);
	}
	

	//g.DrawImage(m_pImageObj->GetBitmap(), m_rectPaint,
	//	(REAL)0, (REAL)0, (REAL)(m_sizePicture.Width ), (REAL)(m_sizePicture.Height ),
	//	Gdiplus::UnitPixel);
	g.ReleaseHDC(NULL);
	return;
}

void CDragImageUI::CheckOffet()
{
	RECT rc = GetRect();
	if (m_rectPaint.Width > rc.right - rc.left)
	{
		m_rectPaint.X = min(m_rectPaint.X, rc.left);
		m_rectPaint.X = max(m_rectPaint.X + m_rectPaint.Width, rc.right) - m_rectPaint.Width;
	}
	if (m_rectPaint.Height > rc.bottom - rc.top)
	{
		m_rectPaint.Y = min(m_rectPaint.Y, rc.top);
		m_rectPaint.Y = max(m_rectPaint.Y + m_rectPaint.Height, rc.bottom) - m_rectPaint.Height;
	}
}
//
//bool CompressImageQuality(
//	const WCHAR* pszOriFilePath,
//	const WCHAR* pszDestFilePah,
//	ULONG quality)
//{
//	// copy from http://msdn.microsoft.com/en-us/library/ms533844(v=VS.85).aspx  
//	// Initialize GDI+.  
//	GdiplusStartupInput gdiplusStartupInput;
//	ULONG_PTR gdiplusToken;
//	Status stat = GenericError;
//	stat = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
//	if (Ok != stat) {
//		return false;
//	}
//
//	// 重置状态  
//	stat = GenericError;
//
//	// Get an image from the disk.  
//	Image* pImage = new Image(pszOriFilePath);
//
//	do {
//		if (NULL == pImage) {
//			break;
//		}
//
//		// 获取长宽  
//		UINT ulHeight = pImage->GetHeight();
//		UINT ulWidth = pImage->GetWidth();
//		if (ulWidth < 1 || ulHeight < 1) {
//			break;
//		}
//
//		// Get the CLSID of the JPEG encoder.  
//		CLSID encoderClsid;
//		if (!GetEncoderClsid(_T("image/jpeg"), &encoderClsid)) {
//			break;
//		}
//
//		// The one EncoderParameter object has an array of values.  
//		// In this case, there is only one value (of type ULONG)  
//		// in the array. We will let this value vary from 0 to 100.  
//		EncoderParameters encoderParameters;
//		encoderParameters.Count = 1;
//		encoderParameters.Parameter[0].Guid = EncoderQuality;
//		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
//		encoderParameters.Parameter[0].NumberOfValues = 1;
//		encoderParameters.Parameter[0].Value = &quality;
//		stat = pImage->Save(pszDestFilePah, &encoderClsid, &encoderParameters);
//	} while (0);
//
//	if (pImage) {
//		delete pImage;
//		pImage = NULL;
//	}
//
//	GdiplusShutdown(gdiplusToken);
//
//	return ((stat == Ok) ? true : false);
//}

void CDragImageUI::RotateImage(REAL fAngle)
{
	if (fAngle < 0)
	{
		fAngle += 360;
	}
	m_fAngle += fAngle;
	m_fAngle = (int)m_fAngle % 360;
	ResizeImage(float(1.0));
	
}

void CDragImageUI::SetSizePercentChangeCallback(SizePercentChangeCallback cb)
{
	m_fnCb = cb;
}

int CDragImageUI::AdjustBright(int nBrightLevel)
{

	m_nBitmapIndex = min(nBrightLevel, 10);
	m_nBitmapIndex = max(m_nBitmapIndex, -10);
	auto it = m_mapBitmaps.find(m_nBitmapIndex);
	if (it == m_mapBitmaps.end())
	{
		BitmapData data;
		m_mapBitmaps[m_nBitmapIndex] = new Gdiplus::Bitmap(m_strImagePath.c_str());
		LockBitmap(m_mapBitmaps[m_nBitmapIndex], &data);
		LineBrightAndContrast(&data, m_nBitmapIndex * 10, 0, 100);
		UnlockBitmap(m_mapBitmaps[m_nBitmapIndex], &data);
	}
	Invalidate();
	return m_nBitmapIndex;
}

void CDragImageUI::OnDestroy()
{
	__super::OnDestroy();
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;                     // number of image encoders    
	UINT size = 0;                   // size of the image encoder array in bytes    
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;     //   Failure    

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;     //   Failure    

	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;     //   Success    
		}
	}
	free(pImageCodecInfo);
	return -1;     //   Failure    
}

bool CDragImageUI::SaveAsImage(const tstring & strFilePath, bool bQuality, tstring & strErr)
{
	auto it = m_mapBitmaps.find(m_nBitmapIndex);
	if (it == m_mapBitmaps.end())
	{
		return false;
	}
	CLSID encoderClsid;
	if (!GetEncoderClsid(_T("image/jpeg"), &encoderClsid)) {
		return false;
	}
	int nBizPercent = bQuality ? 90 : 30;
	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &nBizPercent;
	// The one EncoderParameter object has an array of values.  
	// In this case, there is only one value (of type ULONG)  
	// in the array. We will let this value vary from 0 to 100.  
	Status stat = it->second->Save(strFilePath.c_str(), &encoderClsid, &encoderParameters);
	if (stat == Ok)
	{
		return true;
	}
	else
	{
		return false;
	}
}
