#include "StdAfx.h"
#include "SSE.h"

int CSSE::m_nSupportSSE = -1;

BOOL CSSE::IsSupportSSE()
{
	if(m_nSupportSSE == -1)
	{
		__try 
		{
			__asm 
			{
				xorpd xmm0, xmm0        // executing SSE instruction
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) 
			{
				m_nSupportSSE = 0;
			}
			m_nSupportSSE = 0;
		}
		if(m_nSupportSSE == -1)
		{
			m_nSupportSSE = 1;
		}
	}
	return m_nSupportSSE == 1;
}

BOOL CSSE::DoXorSSE(DWORD dwKey, char * pBuff, int nLen)
{
	int nSSELen = nLen >> 4;
	if(nSSELen)
	{
		__asm
		{
			push ecx
			push eax
			movss xmm1, [dwKey]
			shufps xmm1, xmm1, 0
			mov	eax, [pBuff]
			mov ecx, [nSSELen]		
xor_process:
			movups xmm0, [eax]
			xorps xmm0, xmm1
			movups [eax], xmm0
			add eax, 10h
			dec ecx
			jnz xor_process
			pop eax
			pop ecx
			emms
		}
	}
	DoXorNormal(dwKey, pBuff + (nSSELen << 4), nLen - (nSSELen << 4));
	return TRUE;
}

BOOL CSSE::DoXorNormal(DWORD dwKey, char * pBuff, int nLen)
{
	if(nLen <= 0)
	{
		return FALSE;
	}
	char szKey[4];
	memcpy(szKey, &dwKey, sizeof(szKey));
	int j = 0;
	for(int i = 0; i < nLen; i++)
	{
		pBuff[i] ^= szKey[j];
		j = (j + 1) % sizeof(szKey);
	}
	return TRUE;
}

BOOL CSSE::DoXor(DWORD dwKey, char * pBuff, int nLen)
{
	if(IsSupportSSE())
	{
		return DoXorSSE(dwKey, pBuff, nLen);
	}
	else
	{
		return DoXorNormal(dwKey, pBuff, nLen);
	}
}

void CSSE::MemSetDWord(LPVOID pBuff, DWORD dwVal, int nSize)
{
	if(IsSupportSSE())
	{
		return MemSetDWordSSE(pBuff, dwVal, nSize);
	}
	else
	{
		return MemSetDWordNormal(pBuff, dwVal, nSize);
	}
}

void CSSE::MemCopy(LPVOID pDest, LPVOID pSrc, int nSize)
{
	if(IsSupportSSE())
	{
		MemCopySSE(pDest, pSrc, nSize);
	}
	else
	{
		memcpy(pDest, pSrc, nSize);
	}
}

void CSSE::MemCopySSE(LPVOID pDest, LPVOID pSrc, int nSize)
{
	int nSSESize = nSize >> 4;
	int nTemp = nSSESize << 4;
	nSize -= nTemp;
	if(nSSESize)
	{
		__asm
		{
			push ecx
			push edx
			push eax
			mov	eax, [pSrc]
			mov edx, [pDest]
			mov ecx, [nSSESize]		
memcpy_process:
			movups xmm0, [eax]
			add eax, 10h
			movups [edx], xmm0
			add edx, 10h
			dec ecx
			jnz memcpy_process
			pop eax
			pop edx
			pop ecx
			emms
		}
	}
	if(nSize)
	{
		memcpy((BYTE *)pDest + nTemp, (BYTE *)pSrc + nTemp, nSize);
	}
}

void CSSE::MemSetDWordNormal(LPVOID pBuff, DWORD dwVal, int nSize)
{
	if(nSize <= 0 || nSize % 4)
	{
		return;
	}
	for(DWORD * p = (DWORD *)pBuff; nSize > 0; nSize -= 4, p++)
	{
		*p = dwVal;
	}
}

void CSSE::MemSetDWordSSE(LPVOID pBuff, DWORD dwVal, int nSize)
{
	int nSSESize = nSize >> 4;
	if(nSSESize)
	{	
		__asm
		{
			push ecx
			push eax
			movss xmm1, [dwVal]
			shufps xmm1, xmm1, 0
			mov	eax, [pBuff]
			mov ecx, [nSSESize]		
xor_process:
			movups [eax], xmm1
			add eax, 10h
			dec ecx
			jnz xor_process
			pop eax
			pop ecx
			emms
		}
	}
	MemSetDWordNormal((BYTE *)pBuff + (nSSESize << 4), dwVal, nSize - (nSSESize << 4));

}

void CSSE::DoGray(LPVOID pBuff, int nSize)
{
	if(IsSupportSSE())
	{
		DoGraySSE(pBuff, nSize);
	}
	else
	{
		DoGrayNormal(pBuff, nSize);
	}
}

void CSSE::DoGrayNormal(LPVOID pBuff, int nSize)
{
	for(DWORD * p = (DWORD *)pBuff; nSize > 0; nSize -= 4, p++)
	{
		BYTE c = HIBYTE(77 * GetRValue(*p) + 151 * GetGValue(*p) + 28 * GetBValue(*p));
		BYTE * pSrc = (BYTE *)p;
		*pSrc++ = c;
		*pSrc++ = c;
		*pSrc++ = c;
	}
}

void CSSE::DoGraySSE(LPVOID pBuff, int nSize)
{
	DWORD dwMask = 0xff;
	DWORD dwDiv3 = 85;
	int nSSESize = nSize >> 4;
	if(nSSESize)
	{
		__asm
		{
			push ecx
			push eax
			mov ecx, [nSSESize]
			movss xmm1, [dwMask]
			shufps xmm1, xmm1, 0
			movss xmm2, [dwDiv3]
			shufps xmm2, xmm2, 0
			movaps xmm3, xmm1
			pslld xmm3, 24
			mov	eax, [pBuff]			
gray_process:
 			movups xmm0, [eax]
 			movaps xmm4, xmm0
 			movaps xmm5, xmm0
 			movaps xmm6, xmm0
 			andps xmm4, xmm1
 			psrld xmm5, 8
 			andps xmm5, xmm1
 			psrld xmm6, 16
 			andps xmm6, xmm1
 			paddd xmm4, xmm5
 			paddd xmm4, xmm6
 			pmullw xmm4, xmm2
 			psrld xmm4, 8
            movaps xmm5, xmm4
 			pslld xmm5, 8
 			orps xmm4, xmm5
 			pslld xmm5, 8
			orps xmm4, xmm5
 			andps xmm0, xmm3
 			orps xmm0, xmm4
 			movups [eax], xmm0
			add eax, 10h
			dec ecx
			jnz gray_process
			pop eax
			pop ecx
			emms
		}
	}
	DoGrayNormal((BYTE *)pBuff + (nSSESize << 4), nSize - (nSSESize << 4));
}

void CSSE::DoAlphaXor(LPVOID pBuff, int nSize)
{
	if(IsSupportSSE())
	{
		DoAlphaXorSSE(pBuff, nSize);
	}
	else
	{
		DoAlphaXorNormal(pBuff, nSize);
	}
}

void CSSE::DoAlphaXor(LPVOID pBuff,int nWidth, RECT& rect)
{
	DWORD * pSrc = (DWORD *)pBuff;
	DWORD * p;
	for (int j = rect.top; j < rect.bottom; j++)
	{
		int temp = j * nWidth;
		for (int i = rect.left; i < rect.right; i++)
		{
			p = pSrc + temp + i;
			if (!(*p & 0xff000000))
				*p |= 0xff000000;
			else
				*p &= 0x00000000;
		}
	}

}

void CSSE::DoAlpha(LPVOID pBuff, int nSize)
{
	if(IsSupportSSE())
	{
		DoAlphaSSE(pBuff, nSize);
	}
	else
	{
		DoAlphaNormal(pBuff, nSize);
	}
}

void CSSE::DoAlphaXorSSE(LPVOID pBuff, int nSize)
{
 //目标：对于一个32位的argb值，若a为0,则rgb不变,a改255，否则，置空argb
 //modify：若a不为0，则a置为255，rbg置空
 	int nSizeSSE = nSize >> 4;
 	DWORD dwMask = 0xff000000;
 	if(nSizeSSE)
 	{
 		__asm
 		{
 			push eax
 			push ecx
 			movss xmm1, [dwMask]
 			shufps xmm1, xmm1, 0	// 0xff00000000寄存 //ff000000ff000000ff000000ff000000  对于128位处理32位的掩码
 			xorps xmm3, xmm3		// 0寄存
 			mov eax, [pBuff]		// 源色指针
 			mov ecx, [nSizeSSE]		// 对于128位的处理次数
 open_alpha:
 			movups xmm0, [eax]		//移动数据
 			movaps xmm2, xmm0		//移动数据
 			andps xmm2, xmm1		//掩码与运算 即保留alpha值
 			pcmpeqd xmm2, xmm3		//如果alpha值为0，则为ffffffffH(32 bit),否则0
 			andps xmm0, xmm2		//若源alpha为0，则保留rgb，否则去掉rgb
   			andps xmm2, xmm1		//掩码与运算，保留源alpha为0的掩码为ff
   			orps xmm0, xmm2			//对于已修改rgb的目标色，置源alpha为0的目标色为alpha255
 			movups [eax], xmm0		//移动数据
 			add eax, 10h
 			dec ecx
 			jnz open_alpha
 			pop ecx
 			pop eax
 			emms
 		}
 	}
 	DoAlphaXorNormal((BYTE *)pBuff + (nSizeSSE << 4), nSize - (nSizeSSE << 4));
}

void CSSE::DoAlphaXorNormal(LPVOID pBuff, int nSize)
{
	if(nSize <= 0 || nSize % 4)
	{
		return;
	}
	DWORD * pSrc = (DWORD *)pBuff;
	for(int i = 0; i < nSize; i += 4)
	{
		if(*pSrc & 0xff000000)
		{
			pSrc = 0;
		}
		else
		{
			*pSrc |= 0xff000000;
		}
		pSrc++;
	}
}


void CSSE::DoAlphaSSE(LPVOID pBuff, int nSize)
{
	int nSizeSSE = nSize >> 4;
	DWORD dwMask = 0xff000000;
	if(nSizeSSE)
	{
		__asm
		{
			push eax
				push ecx
				movss xmm1, [dwMask]
			shufps xmm1, xmm1, 0	// 0xff00000000寄存 //ff000000ff000000ff000000ff000000  对于128位处理32位的掩码
				xorps xmm3, xmm3		// 0寄存
				mov eax, [pBuff]		// 源色指针
			mov ecx, [nSizeSSE]		// 对于128位的处理次数
open_alpha:
			movups xmm0, [eax]		//移动数据
			movaps xmm2, xmm0		//移动数据
				andps xmm2, xmm1		//掩码与运算 即保留alpha值
				pcmpeqd xmm2, xmm3		//如果alpha值为0，则为ffffffffH(32 bit),否则0
				andps xmm2, xmm1		//掩码与运算，保留源alpha为0的掩码为ff
				orps xmm0, xmm2			//对于目标色，置源alpha为0的目标色为alpha255
				movups [eax], xmm0		//移动数据
				add eax, 10h
				dec ecx
				jnz open_alpha
				pop ecx
				pop eax
				emms
		}
	}
}


void CSSE::DoAlphaNormal(LPVOID pBuff, int nSize)
{
	if(nSize <= 0 || nSize % 4)
	{
		return;
	}
	DWORD * pSrc = (DWORD *)pBuff;
	for(int i = 0; i < nSize; i += 4)
	{
		if(!(*pSrc & 0xff000000))
		{
			*pSrc |= 0xff000000;
		}
		pSrc++;
	}

}


void CSSE::DoAlpha(LPVOID pBuff, int nWidth, RECT& rect)
{
	DWORD * pSrc = (DWORD *)pBuff;
	DWORD * p;
	for (int j = rect.top; j < rect.bottom; j++)
	{
		int temp = j * nWidth;
		for (int i = rect.left; i < rect.right; i++)
		{
			p = pSrc + temp + i;
			if (!(*p & 0xff000000))
				*p |= 0xff000000;
		}
	}
}


void CSSE::SetColorKey(LPVOID pBuff, int nSize, DWORD dwColor)
{
	dwColor &= 0x00ffffff;
	if(IsSupportSSE())
	{
		CSSE::SetColorKeySSE(pBuff, nSize, dwColor);
	}
	else
	{
		CSSE::SetColorKeyNormal(pBuff, nSize, dwColor);
	}
}

void CSSE::SetColorKeyNormal(LPVOID pBuff, int nSize, DWORD dwColor)
{
	if(nSize <= 0 || nSize % 4)
	{
		return;
	}
	for(DWORD * pSrc = (DWORD *)pBuff; nSize > 0; nSize -= 4, pSrc++)
	{
		if((*pSrc & 0xffffff) == (dwColor & 0xffffff))
		{
			*pSrc = 0;
		}
	}
}

void CSSE::SetColorKeySSE(LPVOID pBuff, int nSize, DWORD dwColor)
{
	int nSSESize = nSize >> 4;
	DWORD dwMask = 0xffffff;
	if(nSSESize)
	{
		__asm
		{
			push eax
			push ecx
			movss xmm1, [dwColor]
			shufps xmm1, xmm1, 0
			movss xmm3, [dwMask]
			shufps xmm3, xmm3, 0
			mov eax, [pBuff]
			mov ecx, [nSSESize]
color_key:
			movups xmm0, [eax]
			movaps xmm2, xmm0
			andps xmm2, xmm3
			pcmpeqd xmm2, xmm1
			pandn xmm2, xmm0
			movups [eax], xmm2
			add eax, 10h
			dec ecx
			jnz color_key
			pop ecx
			pop eax
			emms
		}
	}
	SetColorKeyNormal((BYTE *)pBuff + (nSSESize << 4), nSize - (nSSESize << 4), dwColor);
}

void CSSE::DoOr(DWORD dwKey, LPVOID pBuff, int nLen)
{
	if(IsSupportSSE())
	{
		CSSE::DoOrSSE(dwKey, pBuff, nLen);
	}
	else
	{
		CSSE::DoOrNormal(dwKey, pBuff, nLen);
	}
}

void CSSE::DoOrSSE(DWORD dwKey, LPVOID pBuff, int nLen)
{
	int nSSELen = nLen >> 4;
	if(nSSELen)
	{
		__asm
		{
			push ecx
			push eax
			movss xmm1, [dwKey]
			shufps xmm1, xmm1, 0
			mov	eax, [pBuff]
			mov ecx, [nSSELen]		
xor_process:
			movups xmm0, [eax]
			orps xmm0, xmm1
			movups [eax], xmm0
			add eax, 10h
			dec ecx
			jnz xor_process
			pop eax
			pop ecx
			emms
		}
	}
	CSSE::DoOrNormal(dwKey, (BYTE *)pBuff + (nSSELen << 4), nLen - (nSSELen << 4));
}

void CSSE::DoOrNormal(DWORD dwKey, LPVOID pBuff, int nLen)
{
	if(nLen <= 0 || nLen % 4)
	{
		return;
	}
	for(DWORD * pSrc = (DWORD *)pBuff; nLen > 0; nLen -= 4, pSrc++)
	{
		*pSrc |= dwKey;
	}
}


void CSSE::MirrorX(LPVOID pBuff, int nWidth, int nHeight)
{
	for(int i = 0; i < nHeight; i++)
	{
		DWORD * pLeft = (DWORD *)pBuff + i * nWidth;
		DWORD * pRight = (DWORD *)pBuff + (i + 1) * nWidth - 1;
		while(pLeft < pRight)
		{
			DWORD dwTemp = *pLeft;
			*pLeft = *pRight;
			*pRight = dwTemp;
			pLeft++;
			pRight--;
		}
	}
}

void CSSE::MirrorY(LPVOID pBuff, int nWidth, int nHeight)
{
	DWORD * pTop = (DWORD *)pBuff;
	DWORD * pBottom = (DWORD *)pBuff + nWidth * (nHeight - 1);
	while (pTop < pBottom)
	{
		DWORD * p1 = pTop;
		DWORD * p2 = pBottom;
		for(int i = 0; i < nWidth; i++)
		{
			DWORD dwTemp = *p1;
			*p1 = *p2;
			*p2 = dwTemp;
			p1++;
			p2++;
		}
		pTop += nWidth;
		pBottom -= nWidth;
	}
}

void CSSE::AdjustAlpha(DWORD &dwColor)
{
	BYTE * pSrc = (BYTE *)&dwColor;
	//BYTE bAlpha = *(pSrc + 3);
	BYTE bAlpha  = 255;
	if(bAlpha == 0)
	{
		*(DWORD *)pSrc = 0;
	}
	else if (bAlpha > 0)
	{
		*pSrc = (*pSrc * bAlpha) >> 8;
		*(pSrc + 1) = (*(pSrc + 1) * bAlpha) >> 8;
		*(pSrc + 2) = (*(pSrc + 2) * bAlpha) >> 8;
	}
}

void CSSE::Stretch(LPVOID pBuff, int nWidth, int nHeight, LPVOID pDest, int cx, int cy)
{
	DWORD * p = (DWORD *)pDest;
	int nFx = (nWidth << 10) / cx;
	int nFy = (nHeight << 10) / cy;

	for(int i = 0; i < cy; i++)
	{
		for(int j = 0;j < cx; j++)
		{
			*p++ = *((DWORD *)pBuff + ((i * nFy) >> 10) * nWidth + ((j * nFx) >> 10));
		}
	}
}

void CSSE::DoAnd(DWORD dwKey, char * pBuff, int nLen)
{
	if(IsSupportSSE())
	{
		DoAndSSE(dwKey, pBuff, nLen);
	}
	else
	{
		DoOrNormal(dwKey, pBuff, nLen);
	}
}

void CSSE::DoAndNormal(DWORD dwKey, char * pBuff, int nLen)
{
	if(nLen <= 0 || nLen % 4)
	{
		return;
	}
	for(DWORD * pSrc = (DWORD *)pBuff; nLen > 0; nLen -= 4, pSrc++)
	{
		*pSrc &= dwKey;
	}
}

void CSSE::DoAndSSE(DWORD dwKey, char * pBuff, int nLen)
{
	int nSSELen = nLen >> 4;
	if(nSSELen)
	{
		__asm
		{
			push ecx
			push eax
			movss xmm1, [dwKey]
			shufps xmm1, xmm1, 0
			mov	eax, [pBuff]
			mov ecx, [nSSELen]		
and_process:
			movups xmm0, [eax]
			andps xmm0, xmm1
			movups [eax], xmm0
			add eax, 10h
			dec ecx
			jnz and_process
			pop eax
			pop ecx
			emms
		}
	}
	CSSE::DoAndNormal(dwKey, (char *)pBuff + (nSSELen << 4), nLen - (nSSELen << 4));
}

void CSSE::RGBtoHSL(COLORREF rgb, double * H,double * S, double * L)
{
	byte  r = GetRValue(rgb);
	byte  g = GetGValue(rgb);
	byte  b = GetBValue(rgb);   
	byte  cmax = max(r, max(g, b));
	byte  cmin = min(r, min(g, b)); 

	double mdiff = double(cmax) - double(cmin);
	double msum  = double(cmax) + double(cmin);	

	*L = msum / 510.0f;

	if(cmax==cmin) 
	{
		*S = 0;
		*H = 0;
	} 
	else 
	{
		double rnorm = (cmax - r) / mdiff;      
		double gnorm = (cmax - g) / mdiff;
		double bnorm = (cmax - b) / mdiff;   

		*S = (*L <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

		if(r == cmax) 
			*H = 60.0f * (6.0f + bnorm - gnorm);
		if(g == cmax) 
			*H = 60.0f * (2.0f + rnorm - bnorm);
		if(b == cmax) 
			*H = 60.0f * (4.0f + gnorm - rnorm);
		if (*H > 360.0f) 
			*H -= 360.0f;
	}
}


COLORREF CSSE::HLStoRGB( double H, double L, double S)
{
	BYTE R,G,B;

	L = min(1,L);
	S = min(1,S);

	if(fabs(S) < 0.000001)
	{
		R = G = B = (BYTE)(255 * L);
	} 
	else 
	{
		double rm1, rm2;

		if (L <= 0.5f) 
			rm2 = L + L * S;
		else
			rm2 = L + S - L * S;
		rm1 = 2.0f * L - rm2;   

		R = HueToRGB(rm1, rm2, H + 120.0f);
		G = HueToRGB(rm1, rm2, H);
		B = HueToRGB(rm1, rm2, H - 120.0f);
	}	
	return RGB(R, G, B);
}

BYTE CSSE::HueToRGB(double rm1,double rm2,double rh)
{
	while(rh > 360.0f)
		rh -= 360.0f;
	while(rh < 0.0f)
		rh += 360.f;

	if(rh < 60.0f)
		rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
	else if(rh < 180.0f)
		rm1 = rm2;
	else if(rh < 240.0f)
		rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;

	double n = rm1*255;
	int m = min((int)n,255);
	m = max(0,m);
	return (BYTE)m;//(rm1 * 255);
}

void CSSE::AdjustHLS(LPBYTE lpBuff, int nSize, double nHue, double nSaturation, double lightness)
{
	if(nSize <= 0 || nSize % 4)
		return;	

	double h, s, l;
	for (int i = 0; i < nSize; i+=4)
	{
		LPBYTE lpByte = lpBuff;
		//if (lpByte[2] != lpByte[1] || lpByte[1] != lpByte[0] || lpByte[2] != lpByte[0])
		{
			COLORREF clr = RGB (lpByte[2], lpByte[1], lpByte[0]);
			CSSE::RGBtoHSL(clr, &h,&s,&l); 
			h += nHue; 
			s = s * nSaturation/100;
			l = l * lightness/100;
			clr = CSSE::HLStoRGB(h, l, s);

			*lpByte = GetBValue(clr);
			*(lpByte + 1) = GetGValue(clr);
			*(lpByte + 2) = GetRValue(clr);
		}
		lpBuff++;
		lpBuff++;
		lpBuff++;
		lpBuff++;
	}
	
}

void CSSE::IncreaseAlpha(LPBYTE pBuff,  int nWidth, int nHeight, RECT& rect)
{
	int nWideBytes = nWidth * 4;
	RECT rectsubset = {0, 0, nWidth, nHeight};
	if (!IntersectRect(&rectsubset, &rectsubset, &rect)) return;


	BYTE* p;
	for (int j = rectsubset.top; j < rectsubset.bottom; j++)
	{
		for (int i = rectsubset.left; i < rectsubset.right; i++)
		{
			p = pBuff + j * nWideBytes + i * 4 + 3;
			(*p) ++;
		}
	}
}

void CSSE::DecreaseAlpha(LPBYTE pBuff, int nWidth, int nHeight, RECT& rect)
{
	int nWideBytes = nWidth * 4;
	RECT rectsubset = {0, 0, nWidth, nHeight};
	if (!IntersectRect(&rectsubset, &rectsubset,&rect)) return;
	BYTE* p;
	for (int j = rectsubset.top; j < rectsubset.bottom; j++)
	{
		for (int i = rectsubset.left; i < rectsubset.right; i++)
		{
			p = pBuff + j * nWideBytes + i * 4 + 3;
			(*p) --;
		}
	}

}

void CSSE::ClearAlphaPixel(LPBYTE pBuff, int nWidth, int nHeight, RECT& rect)
{
	if(!pBuff)
		return;

	int nWideBytes = nWidth * 4;
	RECT rectsubset = {0, 0, nWidth, nHeight};
	if (!IntersectRect(&rectsubset, &rectsubset,&rect)) return;
	BYTE* p;
	for (int j = rectsubset.top; j < rectsubset.bottom; j++)
	{
		for (int i = rectsubset.left; i < rectsubset.right; i++)
		{
			p = pBuff + j * nWideBytes + i * 4 + 3;
			(*p)  = 0;
		}
	}

}

void CSSE::RestoreAlphaColor(LPBYTE pBuff, int nWidth, int nHeight, RECT& rect)
{
	if(!pBuff)
		return;

	int nWideBytes = nWidth * 4;
	RECT rectsubset = {0, 0, nWidth, nHeight};
	if (!IntersectRect(&rectsubset, &rectsubset,&rect)) return;
	BYTE* p;
	for (int j = rectsubset.top; j < rectsubset.bottom; j++)
	{
		for (int i = rectsubset.left; i < rectsubset.right; i++)
		{
			p = pBuff + j * nWideBytes + i * 4;
			if((p[3] == 0)&& (p[0] != 0 || p[1] != 0|| p[2] != 0))
				p[3] = 255;	
		}
	}
}

void CSSE::CheckAlphaColor(DWORD& dwColor)
{
	//RestoreAlphaColor认为0x00000000是真正的透明，其它都是GDI绘制导致的
	//所以在GDI绘制中不能用0xFF000000这个颜色值，现在处理是让它变成RGB(0,0,1)
	//RGB(0,0,1)与RGB(0,0,0)很难分出来
	if((0x00FFFFFF & dwColor) == 0)
	{
		dwColor += 1;
	}
}



DWORD CSSE::StringToColor(LPCTSTR lpszValue)
{
	int len = _tcslen(lpszValue);
	if (len == 8)
	{
		DWORD color = _tcstoul(lpszValue, NULL, 16);
		DWORD A = (color & 0xFF000000) >> 24;
		BYTE R =  GetBValue(color);    //先取出表示颜色分量R，移位得到真实的颜色分量值
		BYTE G =  GetGValue(color);
		BYTE B =  GetRValue(color);
		return DUI_ARGB(A, R, G, B);
	}
	else if(len == 6)
	{
		DWORD color = _tcstoul(lpszValue,NULL,16);     //先将有用的字符串转换成长整型赋给变量
		BYTE R = (color & 0xFF0000 )>>16;    //先取出表示颜色分量R，移位得到真实的颜色分量值
		BYTE G = (color & 0x00FF00 )>>8;
		BYTE B =  color & 0x0000FF;
		return DUI_ARGB(0xFF, R, G, B);
	}
	return 0xFF000000;
}