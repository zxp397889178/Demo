#ifndef _ALPHABLEND_H
#define _ALPHABLEND_H

struct ImageData {
public:
	ImageData()
	{
		fPixels = NULL;
		fWidth = 0;
		fHeight = 0;
		fRef = 0;
	}
	~ImageData()
	{
		if (!fRef)
		{
			delete []((BYTE*)fPixels);
			fPixels = NULL;
		}
	}
	void SetPixels(void* pixels,  unsigned int   width, unsigned int   height,  bool bref)
	{
		fPixels = pixels;
		fWidth = width;
		fHeight = height;
		fRef = bref;
	}
public:
	// ±ØÌî²ÎÊý
	mutable void*   fPixels;
	unsigned int    fWidth;
	unsigned int    fHeight;
	bool fRef;
};
void AlphaBlendEx(__in  ImageData* pBitmapDest, __in int xoriginDest, __in int yoriginDest, __in int wDest, __in int hDest, __in ImageData* pBitmapSrc, __in int xoriginSrc, __in int yoriginSrc, __in int wSrc, __in int hSrc, __in unsigned int alpha, __in RECT* clipBounds = NULL, __in char* pbuffer = NULL);
#endif

