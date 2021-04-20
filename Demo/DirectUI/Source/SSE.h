#ifndef _SSE_H
#define _SSE_H

#define GetH(c) ((c) >> 16)
#define GetS(c) (((c) >> 8) & 0xff)
#define GetL(c)	((c) & 0xff)

class  CSSE
{
public:
	static BOOL IsSupportSSE();
	static BOOL DoXor(DWORD dwKey, char * pBuff, int nLen);
	static void DoAnd(DWORD dwKey, char * pBuff, int nLen);
	static void MemSetDWord(LPVOID pBuff, DWORD dwVal, int nSize);
	static void MemCopy(LPVOID pDest, LPVOID pSrc, int nSize);
    static void DoGray(LPVOID pBuff, int nSize);
	static void DoAlphaXor(LPVOID pBuff, int nSize);
	static void DoAlphaXor(LPVOID pBuff,int nWidth, RECT& rect);
	static void DoAlpha(LPVOID pBuff, int nWidth, RECT& rect);
	static void DoAlpha(LPVOID pBuff, int nSize);

	static void IncreaseAlpha(LPBYTE pBuff, int nWidth, int nHeight, RECT& rect);
	static void DecreaseAlpha(LPBYTE pBuff, int nWidth, int nHeight, RECT& rect);
	static void ClearAlphaPixel(LPBYTE pBuff, int nWidth, int nHeight, RECT& rect);
	static void RestoreAlphaColor(LPBYTE pBuff, int nWidth, int nHeight, RECT& rect);
	static void CheckAlphaColor(DWORD& dwColor);
	static DWORD StringToColor(LPCTSTR lpszColor);

	static void SetColorKey(LPVOID pBuff, int nSize, DWORD dwColor);
	static void DoOr(DWORD dwKey, LPVOID pBuff, int nLen);
	static void MirrorX(LPVOID pBuff, int nWidth, int nHeight);
	static void MirrorY(LPVOID pBuff, int nWidth, int nHeight);
	static void AdjustAlpha(DWORD &dwColor);
	static void Stretch(LPVOID pBuff, int nWidth, int nHeight, LPVOID pDest, int cx, int cy);
	static void RGBtoHSL(COLORREF rgb, double * H,double * S, double * L);
	static COLORREF HLStoRGB( double H, double L, double S);
	static void AdjustHLS(LPBYTE lpBuff, int nSize, double nHue, double nSaturation, double lightness);
private:
	static void MemCopySSE(LPVOID pDest, LPVOID pSrc, int nSize);
	static void SetColorKeySSE(LPVOID pBuff, int nSize, DWORD dwColor);
	static void SetColorKeyNormal(LPVOID pBuff, int nSize, DWORD dwColor);
	static BOOL DoXorNormal(DWORD dwKey, char * pBuff, int nLen);
	static BOOL DoXorSSE(DWORD dwKey, char * pBuff, int nLen);
	static void DoOrNormal(DWORD dwKey, LPVOID pBuff, int nLen);
	static void DoOrSSE(DWORD dwKey, LPVOID pBuff, int nLen);
	static void DoAndNormal(DWORD dwKey, char * pBuff, int nLen);
	static void DoAndSSE(DWORD dwKey, char * pBuff, int nLen);
	static void MemSetDWordNormal(LPVOID pBuff, DWORD dwVal, int nSize);
	static void MemSetDWordSSE(LPVOID pBuff, DWORD dwVal, int nSize);
	static void DoGrayNormal(LPVOID pBuff, int nSize);
	static void DoGraySSE(LPVOID pBuff, int nSize);
	static void DoAlphaXorNormal(LPVOID pBuff, int nSize);
	static void DoAlphaXorSSE(LPVOID pBuff, int nSize);
	static void DoAlphaNormal(LPVOID pBuff, int nSize);
	static void DoAlphaSSE(LPVOID pBuff, int nSize);
	static BYTE HueToRGB(double rm1,double rm2,double rh);
	static int m_nSupportSSE;
};
#endif