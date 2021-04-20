#pragma once

#include <windows.h>  
#include <algorithm>  
using std::min;
using std::max;
#include <gdiplus.h>  
using namespace Gdiplus;
//---------------------------------------------------------------------------  

#define ScanAllocFlag       0x00000100  
#define PixelAlphaFlag      0x00010000   
//---------------------------------------------------------------------------  

// �������ز�ֵ��ʽ  
typedef enum
{
	InterpolateModeDefault, // ȱʡΪ���β�ֵ  
	InterpolateModeNear,    // �ٽ���ֵ  
	InterpolateModeBilinear,// ���β�ֵ  
	InterpolateModeBicubic  // ˫������ֵ  
}InterpolateMode;

typedef enum
{
	//  PixelFormatInValid,  
	PixelFormat1bit,
	PixelFormat4bit,
	PixelFormat8bit,
	PixelFormat15bit,
	PixelFormat16bit,
	PixelFormat24bit,
	PixelFormat32bit
}ImagePixelFormat;

// ����ARGB���ؽṹ  
typedef union
{
	ARGB Color;
	struct
	{
		BYTE Blue;
		BYTE Green;
		BYTE Red;
		BYTE Alpha;
	};
}ARGBQuad, *PARGBQuad;
//---------------------------------------------------------------------------  

FORCEINLINE

VOID SetAlphaFlag(BitmapData *data, BOOL isAlpha)

{

	if (isAlpha) data->Reserved |= PixelAlphaFlag;

	else data->Reserved &= ~PixelAlphaFlag;

}

//---------------------------------------------------------------------------  

FORCEINLINE
BOOL HasAlphaFlag(CONST BitmapData *data)
{
	return (data->Reserved & PixelAlphaFlag) != 0;
}
//---------------------------------------------------------------------------  

FORCEINLINE
VOID SetInterpolateMode(BitmapData *data, InterpolateMode mode)
{
	data->Reserved = (data->Reserved & 0xffffff) | (mode << 24);
}
//---------------------------------------------------------------------------  

FORCEINLINE
InterpolateMode GetInterpolateMode(CONST BitmapData *data)

{

	return (InterpolateMode)(data->Reserved >> 24);

}

//---------------------------------------------------------------------------  


// ����GDI+32λλͼɨ���ߵ�data  
FORCEINLINE
VOID LockBitmap(Gdiplus::Bitmap *bmp, BitmapData *data)
{
	Gdiplus::Rect r(0, 0, bmp->GetWidth(), bmp->GetHeight());
	BOOL hasAlpha = bmp->GetPixelFormat() & PixelFormatAlpha;
	bmp->LockBits(&r, ImageLockModeRead | ImageLockModeWrite,
		PixelFormat32bppARGB, data);
	SetAlphaFlag(data, hasAlpha);
}
//---------------------------------------------------------------------------  

// GDI+λͼɨ���߽���  
FORCEINLINE
VOID UnlockBitmap(Gdiplus::Bitmap *bmp, BitmapData *data)
{
	data->Reserved &= 0xff;
	bmp->UnlockBits(data);
}
//---------------------------------------------------------------------------  

// �ø�����ͼ���������첢������GDI+���ݵ�32λλͼ���ݽṹ��  
// ��������ȣ��߶ȣ�ɨ���߿�ȣ�ɨ�����׵�ַ��alpha��ǣ����ص�λͼ���ݽṹָ�롣  
// ע�����stride=0���Զ�����ɨ���߿��  
FORCEINLINE
VOID GetBitmapData(INT width, INT height, INT stride, LPVOID scan0,
ImagePixelFormat format, BOOL isAlpha, BitmapData *data)
{
	INT bits[] = { 0x100, 0x400, 0x800, 0x1005, 0x1000, 0x1800, 0x2000 };

	data->Width = width;
	data->Height = height;
	data->Scan0 = scan0;
	data->PixelFormat = bits[format];
	if (stride)
		data->Stride = stride;
	else
		data->Stride = (INT)((width * (data->PixelFormat >> 8) + 31) & ~31) >> 3;
	SetAlphaFlag(data, isAlpha);
}
//---------------------------------------------------------------------------  

// �ø����Ŀ�ȡ��߶Ⱥ�����λ�����첢�����µ���GDI+���ݵ�λͼ���ݽṹ��  
// ������FreeBitmapData�ͷ�  
FORCEINLINE
VOID GetBitmapData(INT width, INT height, ImagePixelFormat format, BitmapData *data)
{
	GetBitmapData(width, height, 0, NULL, format, format == PixelFormat32bit, data);
	data->Scan0 = GlobalLock(GlobalAlloc(GHND, data->Height * data->Stride));
	if (data->Scan0) data->Reserved |= ScanAllocFlag;
}
//---------------------------------------------------------------------------  

// �ø����Ŀ�Ⱥ͸߶����첢�����µ���GDI+���ݵ�32λλͼ���ݽṹ��  
// ������FreeBitmapData�ͷ�  
FORCEINLINE
VOID GetBitmapData(INT width, INT height, BitmapData *data)
{
	GetBitmapData(width, height, PixelFormat32bit, data);
}
//---------------------------------------------------------------------------  

// ��ȡ32λ��λͼ���ݽṹ  
FORCEINLINE
BOOL GetBitmapData(CONST BitmapData *data, INT x, INT y, INT width, INT height, BitmapData *sub)
{
	width += x;
	height += y;
	if (width > (INT)data->Width)
		width = data->Width;
	if (height > (INT)data->Height)
		height = data->Height;
	INT ScanOffset = 0;
	if (x > 0)
	{
		width -= x;
		ScanOffset += (x << 2);
	}
	if (y > 0)
	{
		height -= y;
		ScanOffset += (y * data->Stride);
	}
	sub->Scan0 = (LPBYTE)data->Scan0 + ScanOffset;
	if (width <= 0 || height <= 0)
		return FALSE;
	sub->Width = width;
	sub->Height = height;
	sub->Stride = data->Stride;
	sub->PixelFormat = data->PixelFormat;
	sub->Reserved = data->Reserved & ~ScanAllocFlag;
	return TRUE;
}
//---------------------------------------------------------------------------  

// ���data������ɨ�����ڴ棬�ͷ�ɨ�����ڴ�  
FORCEINLINE
VOID FreeBitmapData(BitmapData *data)
{
	if ((data->Reserved & ScanAllocFlag) && data->Scan0)
	{
		HGLOBAL handle = GlobalHandle(data->Scan0);
		if (handle)
		{
			GlobalUnlock(handle);
			GlobalFree(handle);
		}
		data->Reserved = 0;
	}
}
//---------------------------------------------------------------------------  

// ��ȡ32λλͼ���ݿ�������  

// ������Ŀ�����ݣ�Դ���ݣ���ȣ��߶ȣ�Ŀ��ɨ���ߣ�Դɨ���ߣ�Ŀ��ƫ�ƣ�Դƫ��  

FORCEINLINE

VOID GetDataCopyParams(CONST BitmapData *dest, CONST BitmapData *source,

UINT &width, UINT &height, PARGBQuad &dstScan0, PARGBQuad &srcScan0,

INT &dstOffset, INT &srcOffset)

{

	width = dest->Width < source->Width ? dest->Width : source->Width;

	height = dest->Height < source->Height ? dest->Height : source->Height;
	dstScan0 = (PARGBQuad)dest->Scan0;
	srcScan0 = (PARGBQuad)source->Scan0;

	dstOffset = (dest->Stride >> 2) - (INT)width;
	srcOffset = (source->Stride >> 2) - (INT)width;
}
//---------------------------------------------------------------------------  

// ��ȡ������32λλͼ���ݽṹdata�ı߿���չͼ�����ݽṹ��Radius����չ�뾶  
FORCEINLINE
VOID GetExpendData(CONST BitmapData *data, UINT radius, BitmapData *exp)
{
	GetBitmapData(data->Width + (radius << 1), data->Height + (radius << 1),
		PixelFormat32bit, exp);
	SetAlphaFlag(exp, HasAlphaFlag(data));
	BitmapData sub;
	PARGBQuad pd, ps;
	UINT width, height;
	INT dstOffset, srcOffset;
	GetBitmapData(exp, radius, radius, data->Width, data->Height, &sub);
	GetDataCopyParams(&sub, data, width, height, pd, ps, dstOffset, srcOffset);
	PARGBQuad pt = pd - radius;
	UINT x, y;
	// ���ͼ�����ݺ�Alpha��ת��ΪPARGB���ظ�ʽ  
	if (HasAlphaFlag(data))
	{
		for (y = 0; y < height; y++, pd += dstOffset, ps += srcOffset)
		{
			for (x = 0; x < width; x++, pd++, ps++)
			{
				pd->Blue = (ps->Blue * ps->Alpha + 127) / 255;
				pd->Green = (ps->Green * ps->Alpha + 127) / 255;
				pd->Red = (ps->Red * ps->Alpha + 127) / 255;
				pd->Alpha = ps->Alpha;
			}
		}
	}
	// ����, ֱ�����ؿ���  
	else
	{
		for (y = 0; y < height; y++, pd += dstOffset, ps += srcOffset)
		{
			for (x = 0; x < width; *pd++ = *ps++, x++);
		}
	}
	// ��չ���ұ߿�����  
	for (y = 0, pd = pt; y < height; y++)
	{
		for (x = 0, ps = pd + radius; x < radius; *pd++ = *ps, x++);
		for (x = 0, pd += width, ps = pd - 1; x < radius; *pd++ = *ps, x++);
	}
	// ��չͷβ�߿�����  
	PARGBQuad pb = (PARGBQuad)((LPBYTE)pd - exp->Stride);
	PARGBQuad pd0 = (PARGBQuad)exp->Scan0;
	for (y = 0; y < radius; y++)
	{
		for (x = 0; x < exp->Width; *pd0++ = pt[x], *pd++ = pb[x], x++);
	}
}
//---------------------------------------------------------------------------  

// PARGB��ʽת����ARGB��ʽ  
FORCEINLINE
VOID PArgbConvertArgb(BitmapData *data)
{
	PARGBQuad p = (PARGBQuad)data->Scan0;
	INT dataOffset = (data->Stride >> 2) - (INT)data->Width;
	for (UINT y = 0; y < data->Height; y++, p += dataOffset)
	{
		for (UINT x = 0; x < data->Width; x++, p++)
		{
			p->Blue = p->Blue * 255 / p->Alpha;
			p->Green = p->Green * 255 / p->Alpha;
			p->Red = p->Red * 255 / p->Alpha;
		}
	}
}
//---------------------------------------------------------------------------  

// ARGB��ʽת����PARGB��ʽ  
FORCEINLINE
VOID ArgbConvertPArgb(BitmapData *data)
{
	PARGBQuad p = (PARGBQuad)data->Scan0;
	INT dataOffset = (data->Stride >> 2) - (INT)data->Width;
	for (UINT y = 0; y < data->Height; y++, p += dataOffset)
	{
		for (UINT x = 0; x < data->Width; x++, p++)
		{
			p->Blue = (p->Blue * p->Alpha + 127) / 255;
			p->Green = (p->Green * p->Alpha + 127) / 255;
			p->Red = (p->Red * p->Alpha + 127) / 255;
		}
	}
}
//---------------------------------------------------------------------------  