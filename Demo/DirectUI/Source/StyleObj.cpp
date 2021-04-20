#include "stdafx.h"
#include "StyleObj.h"

DWORD add_alpha_to_color(DWORD dwColor, int alpha)
{
	alpha = (dwColor >> 24) * alpha / 255;
	return  (dwColor & 0x00FFFFFF) + ((DWORD)alpha << 24);
}

#define  ALL_STATE_VALUE 0xFFFF
WORD  GetStateValueFromStr(LPCTSTR lpszValue)
{
	if (equal_tstrcmp(lpszValue, _T("non")))
		return 0xFFFF;
	if (equal_tstrcmp(lpszValue, _T("all")))
		return 0x001F;
	if (equal_tstrcmp(lpszValue, _T("ckall")))
		return 0x1F00;

	if (equal_tstrcmp(lpszValue, _T("normal")))
		return 0x0001;
	if (equal_tstrcmp(lpszValue, _T("over")))
		return 0x0002;
	if (equal_tstrcmp(lpszValue, _T("down")))
		return 0x0004;
	if (equal_tstrcmp(lpszValue, _T("focus")))
		return 0x0008;
	if (equal_tstrcmp(lpszValue, _T("disable")))
		return 0x0010;

	if (equal_tstrcmp(lpszValue, _T("enable")))
		return 0x000F;

	if (equal_tstrcmp(lpszValue, _T("cknormal")))
		return 0x0100;
	if (equal_tstrcmp(lpszValue, _T("ckover")))
		return 0x0200;
	if (equal_tstrcmp(lpszValue, _T("ckdown")))
		return 0x0400;
	if (equal_tstrcmp(lpszValue, _T("ckfocus")))
		return 0x0800;
	if (equal_tstrcmp(lpszValue, _T("ckdisable")))
		return 0x1000;
	if (equal_tstrcmp(lpszValue, _T("ckenable")))
		return 0x0F00;
	return  0x0000;
}

LineStyleType  GetLineTypeValueFromStr(LPCTSTR lpszValue)
{
	if (equal_tstrcmp(lpszValue, _T("solid")))
		return LineStyleSolid;
	if (equal_tstrcmp(lpszValue, _T("dash")))
		return LineStyleDash;
	if (equal_tstrcmp(lpszValue, _T("dot")))
		return LineStyleDot;
	if (equal_tstrcmp(lpszValue, _T("dashdot")))
		return LineStyleDashDot;
	if (equal_tstrcmp(lpszValue, _T("dashdotdot")))
		return LineStyleDashDotDot;
	return LineStyleSolid;
}

bool  GetGradientModeValueFromStr(LPCTSTR lpszValue)
{
	if (equal_tstrcmp(lpszValue, _T("horz")))
		return false;
	if (equal_tstrcmp(lpszValue, _T("vert")))
		return true;
	return false;
}

UITYPE_PAINTMODE  GetPaintModeValueFromStr(LPCTSTR lpszValue)
{
	if (equal_tstrcmp(lpszValue, _T("normal")))
		return UIPAINTMODE_NORMAL;
	if (equal_tstrcmp(lpszValue, _T("fill")))
		return UIPAINTMODE_STRETCH;
	if (equal_tstrcmp(lpszValue, _T("tile")))
		return UIPAINTMODE_TILE;
	if (equal_tstrcmp(lpszValue, _T("xtile")))
		return UIPAINTMODE_XTILE;
	if (equal_tstrcmp(lpszValue, _T("ytile")))
		return UIPAINTMODE_YTILE;
	if (equal_tstrcmp(lpszValue, _T("center")))
		return UIPAINTMODE_CENTER;
	if (equal_tstrcmp(lpszValue, _T("fit")))
		return UIPAINTMODE_FIT;
	if (equal_tstrcmp(lpszValue, _T("centercrop")))
		return UIPAINTMODE_CENTERCROP;
	return UIPAINTMODE_STRETCH;
}


int  GetGDIPlusWarpModeStr(LPCTSTR lpszValue)
{
	if (equal_tstrcmp(lpszValue, _T("normal")))
		return -1;
	if (equal_tstrcmp(lpszValue, _T("tile")))
		return WrapModeTile;
	if (equal_tstrcmp(lpszValue, _T("titleflipx")))
		return WrapModeTileFlipX;
	if (equal_tstrcmp(lpszValue, _T("titleflipy")))
		return WrapModeTileFlipY;
	if (equal_tstrcmp(lpszValue, _T("titleflipxy")))
		return WrapModeTileFlipXY;
	if (equal_tstrcmp(lpszValue, _T("clamp")))
		return WrapModeClamp;
	return -1;
}

UITYPE_IMAGE  GeImageTypeValueFromStr(LPCTSTR lpszValue)
{
	if (equal_tstrcmp(lpszValue, _T("hbitmap")))
		return UIIAMGE_HBITMAP;
	if (equal_tstrcmp(lpszValue, _T("pixel")))
		return UIIAMGE_PIXEL;
	if (equal_tstrcmp(lpszValue, _T("bitmap")))
		return UIIAMGE_BITMAP;
	if (equal_tstrcmp(lpszValue, _T("hicon")))
		return UIIAMGE_HICON;
	return UIIAMGE_DEFAULT;
}

UITYPE_SMOOTHING  GetSmootingTypeValueFromStr(LPCTSTR lpszValue)
{
	if (equal_icmp(lpszValue, _T("HighSpeed")))
		return UISMOOTHING_HighSpeed;
	if (equal_icmp(lpszValue, _T("HighQuality")))
		return UISMOOTHING_HighQuality;
	return (UITYPE_SMOOTHING)_ttoi(lpszValue);
}

UITYPE_FONT  GetFontTypeValueFromStr(LPCTSTR lpszValue)
{
	if (equal_icmp(lpszValue, _T("GDI")))
		return UIFONT_GDI;
	if (equal_icmp(lpszValue, _T("GDI+")))
		return UIFONT_GDIPLUS;
	return (UITYPE_FONT)_ttoi(lpszValue);
}

UITYPE_FONTEFFECT  GetFontEffectValueFromStr(LPCTSTR lpszValue)
{
	if (equal_icmp(lpszValue, _T("glow")) || equal_icmp(lpszValue, _T("auto")))
		return UIFONTEFFECT_GLOW_AUTO;
	if (equal_icmp(lpszValue, _T("gold")))
		return UIFONTEFFECT_GLOW_GOLD;
	if (equal_icmp(lpszValue, _T("black")))
		return UIFONTEFFECT_GLOW_BLACK;
	return (UITYPE_FONTEFFECT)_ttoi(lpszValue);
}



StyleObj::StyleObj(CControlUI* pOwner):
m_pOwner(pOwner),
m_uMaskState(ALL_STATE_VALUE)
{
}

StyleObj::~StyleObj(void)
{

}


void StyleObj::SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue)//重载基类
{
	if (equal_icmp(lpszName, _T("state")))
	{
		tstring strSpliter = lpszValue;
		int i = strSpliter.find_first_of(_T('|'));
		int nState = 0;
		while (i >= 0)
		{
			nState |= GetStateValueFromStr(strSpliter.substr(0, i).c_str());
			strSpliter = strSpliter.substr(i + 1);
			i = strSpliter.find_first_of(_T('|'));
		}
		nState |= GetStateValueFromStr(strSpliter.c_str());
		m_uMaskState = nState;
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

void StyleObj::Copy(StyleObj* pStyleObj)
{
	// CViewUI 变量复制
	m_strId = pStyleObj->m_strId;
	m_bVisible = pStyleObj->m_bVisible;
	m_nAlpha = pStyleObj->m_nAlpha;
	EnableRTLLayout(pStyleObj->IsRTLLayoutEnable());
	int l, t, r, b;
	pStyleObj->GetCoordinate(l, t, r, b);
	SetCoordinate(l, t, r, b, pStyleObj->GetAnchor());

	// StyleObj 变量复制
    m_uMaskState = pStyleObj->m_uMaskState;
}

void StyleObj::ReleaseResource()
{
}

void StyleObj::OnLanguageChange()
{
}

void StyleObj::SetRect(RECT& rectRegion)
{
	__super::SetRect(rectRegion);

	if (!IsRTLLayoutEnable())
	{
		return;
	}

	if (GetUIEngine()->IsRTLLayout())
	{
		CControlUI* pOwner = GetOwner();
		if (pOwner)
		{
			RECT rcOwner;
			if (pOwner->IsContainer())
			{
				rcOwner = pOwner->GetRect();
			}
			else
			{
				rcOwner = pOwner->GetClientRect();
			}
			RtlRect(rcOwner, GetRectPtr());
		}

	}
}

void StyleObj::SetVisible( bool bVisible)
{
	if (IsVisible() == bVisible)
		return;
	__super::SetVisible(bVisible);
	Invalidate();
}


bool StyleObj::HitTest(POINT point)
{
	if(IsVisible() && PtInRect(GetRectPtr(),point)) 
		return true;
	return false;
}

CControlUI* StyleObj::GetOwner()
{
	return m_pOwner;
}

bool StyleObj::MaskState( UITYPE_STATE typeState )
{
	if (!IsVisible()) 
		return false;
	return ((1 << typeState) & m_uMaskState) != 0;
}

UITYPE_STATE StyleObj::GetMaskState()
{
	return (UITYPE_STATE)m_uMaskState;
}

UITYPE_STATE StyleObj::GetDrawState()
{
	CControlUI* pOwner = GetOwner();
	int uState = (int)GetState();
	if (uState == UISTATE_UNKNOWN && pOwner)
	{
		uState = pOwner->GetState();
		if (!pOwner->IsEnabled())
			uState = UISTATE_DISABLE;
		if (pOwner->IsSelected())
			uState += UISTATE_CKNORMAL;
	}
	return (UITYPE_STATE)uState;
}

int StyleObj::GetDrawAlpha()
{
	return (GetAlpha() == -1 ? GetOwner()->GetAlpha() : GetAlpha());
}


bool StyleObj::Resize()
{
	CControlUI* pOwner = GetOwner();
	if (pOwner)
	{
		RECT rcOwner;
		if (pOwner->IsContainer())
			rcOwner = pOwner->GetRect();
		else
			rcOwner = pOwner->GetClientRect();
		OnSize(rcOwner);
	}
	return true;
}

void StyleObj::Invalidate(bool bUpdateWindow)
{
	if (GetOwner())
		GetOwner()->Invalidate(bUpdateWindow);
}

///////////////////////////////////////////////////////////////////////////////////////////
ImageStyle::ImageStyle(CControlUI* pOwner):StyleObj(pOwner)
{
	m_pImageObj = NULL;
	memset(&m_arrStateImgSeq, 0, sizeof(m_arrStateImgSeq));
	m_ePaintMode = UIPAINTMODE_STRETCH;
	m_paintParams.bHole = false;
	m_bCache = true;
	m_strImageFile = _T("");
	m_paintParams.eSmoothingMode = ResObj::ms_eImageQualityType;
	m_eImageType = UIIAMGE_DEFAULT;
	m_nAlpha = -1;
	m_nImgSeq = -1;
	m_paintParams.eGDIPLUSWarpMode = WrapModeTileFlipXY;
	m_paintParams.bEnableHDPI = GetResObj()->GetHDPIAutoStretchOnDrawing9Image();
}

ImageStyle::~ImageStyle(void)
{
	ReleaseResource();
}

ImageObj* ImageStyle::GetImageObj()
{
	if (m_pImageObj == NULL)
		LoadImage();
	return m_pImageObj;
}

void ImageStyle::SetImage(LPCTSTR lpszPath)
{
	ReleaseResource();

	if (!lpszPath)
		return;
	m_strImageFile = lpszPath;
}


LPCTSTR ImageStyle::GetImageFile()
{
	return m_strImageFile.c_str();
}

void ImageStyle::SetImage(LPVOID lpData, long nSize)
{
	ReleaseResource();
	if (lpData)
	{
		m_pImageObj = GetResObj()->LoadImage(lpData, nSize, m_eImageType);
		m_strImageFile = m_pImageObj->GetFilePath();
	}
}

void ImageStyle::SetImage(ImageObj* pImg)
{
	ReleaseResource();
	if (pImg)
	{
		m_pImageObj = pImg->Get();
		m_strImageFile = pImg->GetFilePath();
	}
}

void ImageStyle::Render( IRenderDC* pRenderDC, RECT& rcPaint )
{
	if (!IsVisible())
		return;

	int nImgSeq = GetDrawImgSeq();
	if (nImgSeq == -1)
	{
		return;
	}

	if (!LoadImage())
		return;

	m_paintParams.nAlpha = GetDrawAlpha();

	RECT rcItem = GetRect();
	switch (m_ePaintMode)
	{
	case UIPAINTMODE_NORMAL:
		pRenderDC->DrawImage(m_pImageObj, rcItem.left, rcItem.top, nImgSeq, &m_paintParams);
		break;
	case UIPAINTMODE_STRETCH:
		pRenderDC->DrawImage(m_pImageObj, rcItem, nImgSeq, &m_paintParams);
		break;
	case UIPAINTMODE_TILE:
		{
			int nCellWidth = m_pImageObj->GetCellWidth();
			int nCellHeight= m_pImageObj->GetCellHeight();
			int nOffsetY = rcItem.top;
			while (nOffsetY < rcItem.bottom)
			{
				int nOffsetX = rcItem.left;
				while (nOffsetX < rcItem.right)
				{
					RECT dst = { nOffsetX, nOffsetY, nOffsetX + nCellWidth, nOffsetY + nCellHeight };
					::IntersectRect(&dst, &dst, &rcItem);
					pRenderDC->DrawImage(m_pImageObj, dst, nImgSeq, &m_paintParams);
					nOffsetX += nCellWidth;
				}
				nOffsetY += nCellHeight;
			}
		}
		break;
	case UIPAINTMODE_XTILE:
	{
		int nCellWidth = m_pImageObj->GetCellWidth();
		int nCellHeight = m_pImageObj->GetCellHeight();
		int nOffsetX = rcItem.left;
		while (nOffsetX < rcItem.right)
		{
			RECT dst = { nOffsetX, rcItem.top, nOffsetX + DPI_SCALE(nCellWidth), rcItem.bottom };
			//::IntersectRect(&dst, &dst, &rcItem);
			pRenderDC->DrawImage(m_pImageObj, dst, nImgSeq, &m_paintParams);
			nOffsetX += DPI_SCALE(nCellWidth);
		}
	}
	break;
	case UIPAINTMODE_YTILE:
	{
		int nCellWidth = m_pImageObj->GetCellWidth();
		int nCellHeight = m_pImageObj->GetCellHeight();
		int nOffsetY = rcItem.top;
		while (nOffsetY < rcItem.bottom)
		{
			RECT dst = { rcItem.left, nOffsetY, rcItem.right, nOffsetY + DPI_SCALE(nCellHeight) };
			//::IntersectRect(&dst, &dst, &rcItem);
			pRenderDC->DrawImage(m_pImageObj, dst, nImgSeq, &m_paintParams);
			nOffsetY += DPI_SCALE(nCellHeight);
		}
	}
	break;
	case UIPAINTMODE_CENTER:
		{
			int nWidth = DPI_SCALE(m_pImageObj->GetCellWidth());
			int nHeight = DPI_SCALE(m_pImageObj->GetCellHeight());
			int dx = 0;
			int dy = 0;
			RECT rect = rcItem;
			if (nWidth <= rcItem.right - rcItem.left && nHeight <= rcItem.bottom - rcItem.top)
			{
				dx = (rcItem.right - rcItem.left - nWidth) / 2;
				dy = (rcItem.bottom - rcItem.top - nHeight) / 2;
				rect.left += dx;
				rect.top += dy;
				rect.right = rect.left + nWidth;
				rect.bottom = rect.top + nHeight;
			}
			else if ((double)(rcItem.right - rcItem.left) / (rcItem.bottom - rcItem.top) >= (double)nWidth / nHeight)
			{
				int nNewHeight = rcItem.bottom - rcItem.top;
				int nNewWidth = nNewHeight * nWidth / nHeight;
				dx = (rcItem.right - rcItem.left - nNewWidth) / 2;
				rect.left += dx;
				rect.right = rect.left + nNewWidth;
			}
			else
			{
				int nNewWidth = rcItem.right - rcItem.left;
				int nNewHeight = nNewWidth * nHeight / nWidth;
				dy = (rcItem.bottom - rcItem.top - nNewHeight) / 2;
				rect.top += dy;
				rect.bottom = rect.top + nNewHeight;
			}
			pRenderDC->DrawImage(m_pImageObj, rect, nImgSeq, &m_paintParams); 
		}
		break;
	case UIPAINTMODE_FIT:
		{
			// 合并UIPAINTMODE_FIT  图片显示方式fit算法修复
			int nItemWidth = rcItem.right - rcItem.left;
			int nItemHeight = rcItem.bottom - rcItem.top;
			int nWidth = m_pImageObj->GetCellWidth();
			int nHeight = m_pImageObj->GetCellHeight();
			int nNewWidth = 0;
			int nNewHeight = 0;
			float fScale = (float)nWidth / (float)nHeight;//图片的正确比例
			float fLayoutScale = (float)nItemWidth / (float)nItemHeight;//图层的比例
			float fCalcScale;
			RECT rcDest;
			if (fScale > fLayoutScale)//调整高度
			{
				fCalcScale = (float)nWidth / (float)nItemWidth;
				nNewWidth = nItemWidth;
				nNewHeight = (int)((float)nHeight / fCalcScale);
				rcDest.left = rcItem.left;
				rcDest.right = rcItem.right;
				rcDest.top = rcItem.top - (nNewHeight - nItemHeight) / 2;
				rcDest.bottom = rcDest.top + nNewHeight;
			}
			else//调整宽度
			{
				fCalcScale = (float)nHeight / (float)nItemHeight;
				nNewWidth = (int)((float)nWidth / fCalcScale);
				nNewHeight = nItemHeight;
				rcDest.left = rcItem.left - (nNewWidth - nItemWidth) / 2;
				rcDest.top = rcItem.top;
				rcDest.right = rcDest.left + nNewWidth;
				rcDest.bottom = rcItem.bottom;
			}
			//int nNewWidth;
			//int nNewHeight;
			//RECT rcDest;
			//if (nWidth < nHeight)
			//{
			//	nNewHeight = nItemWidth*nHeight/nWidth;
			//	rcDest.left  = rcItem.left;
			//	rcDest.top   = rcItem.top - (nNewHeight - nItemHeight)/2;
			//	rcDest.right = rcItem.right;
			//	rcDest.bottom= rcDest.top + nNewHeight;
			//}
			//else if (nWidth > nHeight)
			//{
			//	nNewWidth  = nItemHeight*nWidth/nHeight;
			//	rcDest.left  = rcItem.left - (nNewWidth - nItemWidth)/2;
			//	rcDest.top   = rcItem.top;
			//	rcDest.right = rcDest.left + nNewWidth;
			//	rcDest.bottom= rcItem.bottom;
			//}
			//else
			//{
			//	rcDest = rcItem;
			//}

			RECT rcTemp = { 0 };
			IntersectRect(&rcTemp, &rcPaint, &rcItem);
			ClipObj childClip;
			ClipObj::GenerateClip(pRenderDC->GetDC(), rcTemp, childClip);
			pRenderDC->DrawImage(m_pImageObj, rcDest, nImgSeq, &m_paintParams);
		}
		break;

	case UIPAINTMODE_CENTERCROP:
	{
		int nItemWidth = rcItem.right - rcItem.left;
		int nItemHeight = rcItem.bottom - rcItem.top;
		int nWidth = m_pImageObj->GetCellWidth();
		int nHeight = m_pImageObj->GetCellHeight();
		int nNewWidth;
		int nNewHeight;
		float fRateW = (float)nItemWidth / nWidth;
		float fRateH = (float)nItemHeight / nHeight;
		RECT rcDest;
		if (fRateH < fRateW)
		{
			nNewWidth = nItemWidth;
			int nNewHeight = nItemWidth*nHeight / nWidth;
			rcDest.left = rcItem.left;
			rcDest.top = rcItem.top - (nNewHeight - nItemHeight) / 2;
			rcDest.right = rcItem.right;
			rcDest.bottom = rcDest.top + nNewHeight;
		}
		else if (fRateH > fRateW)
		{
			nNewHeight = nItemHeight;
			nNewWidth = nItemHeight*nWidth / nHeight;
			rcDest.left = rcItem.left - (nNewWidth - nItemWidth) / 2;
			rcDest.top = rcItem.top;
			rcDest.right = rcDest.left + nNewWidth;
			rcDest.bottom = rcItem.bottom;
		}
		else
		{
			rcDest = rcItem;
		}
		RECT rcTemp = { 0 };
		IntersectRect(&rcTemp, &rcPaint, &rcItem);
		ClipObj childClip;
		ClipObj::GenerateClip(pRenderDC->GetDC(), rcTemp, childClip);
		pRenderDC->DrawImage(m_pImageObj, rcDest, nImgSeq, &m_paintParams);
	}
	break;
	}
	return ;
}

int ImageStyle::GetDrawImgSeq()
{
	int nImgSeq = GetImgSeq();
	if (nImgSeq == -1)
	{
		int uState = GetDrawState();
		if (!MaskState((UITYPE_STATE)uState))
			return -1;

		if (uState >= UISTATE_CKNORMAL)
			uState -= UISTATE_CKNORMAL;

		// 无状态情况下只画第一张
		if (GetMaskState() == ALL_STATE_VALUE)
			uState = UISTATE_NORMAL;

		if (m_arrStateImgSeq[uState] == 255) //配置-1的序号不画
			return -1;

		nImgSeq = m_arrStateImgSeq[uState];
	}
	return nImgSeq;
}

void ImageStyle::SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue)//重载基类
{
	if (equal_icmp(lpszName, _T("image")))
	{
		m_strImageFile = lpszValue;
		ReleaseResource();
	}
	else if (equal_icmp(lpszName, _T("imgseq")))
	{
		tstring strSpliter = lpszValue;
		int i = strSpliter.find_first_of(_T(','));
		int index = 0; 
		while (i >= 0)
		{
			m_arrStateImgSeq[index] = _ttoi(strSpliter.substr(0, i).c_str());
			strSpliter = strSpliter.substr(i + 1);
			i = strSpliter.find_first_of(_T(','));
			index++;
		}
		m_arrStateImgSeq[index] = _ttoi(strSpliter.c_str());
	}
	else if (equal_icmp(lpszName, _T("paintmode")))
	{
		m_ePaintMode = GetPaintModeValueFromStr(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("angleheight")))
	{
		m_paintParams.nAngleHeight = _ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("hole")))
	{
		m_paintParams.bHole = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("cache")))
	{
		m_bCache = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("smooth")) || equal_icmp(lpszName, _T("HighQuality")))
	{
		m_paintParams.eSmoothingMode = GetSmootingTypeValueFromStr(lpszValue);
		if (m_paintParams.eSmoothingMode == UISMOOTHING_HighQuality)
		{
			m_eImageType = UIIAMGE_BITMAP;
		}
	}
	else if (equal_icmp(lpszName, _T("gdipluswarpmode")))
	{
		m_paintParams.eGDIPLUSWarpMode = GetGDIPlusWarpModeStr(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("enablehdpi")))
	{
		m_paintParams.bEnableHDPI = (bool)!!_ttoi(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("imagetype")))
	{
		m_eImageType = GeImageTypeValueFromStr(lpszValue);
	}

	else 
		StyleObj::SetAttribute(lpszName, lpszValue);
}

void ImageStyle::Copy(StyleObj* pStyleObj)
{
	ImageStyle *pImageStyle = (ImageStyle*)pStyleObj;
	m_strImageFile = pImageStyle->m_strImageFile;
	ReleaseResource();

	memcpy(m_arrStateImgSeq, pImageStyle->m_arrStateImgSeq, sizeof(m_arrStateImgSeq));
	m_ePaintMode = pImageStyle->m_ePaintMode;
	m_paintParams = pImageStyle->m_paintParams;
	m_bCache = pImageStyle->m_bCache;
	m_eImageType = pImageStyle->m_eImageType;

	StyleObj::Copy(pStyleObj);
}

bool ImageStyle::LoadImage()
{
	if (m_pImageObj == NULL)
	{
		if (m_strImageFile.empty())
			return false;
		m_pImageObj =  GetResObj()->LoadImage(m_strImageFile.c_str(), m_bCache, m_eImageType);
	}
	return m_pImageObj != NULL;
}

void ImageStyle::ReleaseResource()
{
	if (m_pImageObj)
		m_pImageObj->Release();
	m_pImageObj = NULL;
}

bool ImageStyle::IsImageValid()
{
	return m_pImageObj != NULL;
}

bool ImageStyle::IsImageCache()
{
	return m_bCache;

}
void ImageStyle::SetPaintMode( UITYPE_PAINTMODE PaintMode )
{
	m_ePaintMode = PaintMode;
}

UITYPE_PAINTMODE ImageStyle::GetPaintMode()
{
	return m_ePaintMode;
}

PaintParams* ImageStyle::GetPaintParams()
{
	return &m_paintParams;
}

int ImageStyle::GetImgSeq() const
{
	return m_nImgSeq;
}

void ImageStyle::SetImgSeq( int val )
{
	m_nImgSeq = val;
}

void ImageStyle::SetAngle( bool bAngle, int nAngle /*= 0*/ )
{
	m_paintParams.bAngle = bAngle;
	m_paintParams.nAngle = nAngle;
}

void ImageStyle::SetRoate(bool bRoate, int nAngle /*= 0*/)
{
	m_paintParams.bRotate = bRoate;
	m_paintParams.nRotateAngle = nAngle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
TextStyle::TextStyle(CControlUI* pOwner):StyleObj(pOwner)
{
	m_lpszText = NULL;
	m_uAlignStyle = DT_SINGLELINE|DT_VCENTER|DT_LEFT_EX|DT_NOPREFIX;
	m_nAlpha = -1;

	memset(m_pArrColorObj, 0, sizeof(m_pArrColorObj));

	if (GetUIEngine()->IsRTLLayout())
	{
		m_uAlignStyle |= DT_RTLREADING;
	}

	m_nFontEffect = UIFONTEFFECT_NORMAL;
	m_eFontType = UIFONT_DEFAULT;
	m_lpszTextKey = NULL;
	m_pFontObj = GetUIRes()->GetDefaultFont();

	//SetShadowColor(0x5500D7FF);
}

TextStyle::~TextStyle(void)
{
	ReleaseResource();

	if (m_pFontObj && !m_pFontObj->IsCached())
		delete m_pFontObj;
	m_pFontObj = NULL;

	for (int i = 0; i < 5; i++)
	{
		if (m_pArrColorObj[i])
			delete m_pArrColorObj[i];
	}
	
}

FontObj* TextStyle::GetFontObj()
{
	return m_pFontObj;
}

void TextStyle::SetFontObj(FontObj* pFontObj)
{
	if (!pFontObj)
	{
		return;
	}

	if (m_pFontObj && !m_pFontObj->IsCached())
		delete m_pFontObj;
	m_pFontObj = pFontObj;
}

void TextStyle::SetText(LPCTSTR lpszText)
{
	if (m_lpszText)
		delete[] m_lpszText;
	m_lpszText = NULL;
	if (lpszText != NULL)
	{
		int nLen = _tcslen(lpszText);
		//modify by hanzp. 只要有设置，就认为有
		//if (nLen > 0)
		{
			m_lpszText = new TCHAR[nLen + 1];
			 _tcsncpy(m_lpszText, lpszText, nLen);
			m_lpszText[nLen] = '\0';
		}
	}
}

LPCTSTR TextStyle::GetText()
{
	if (m_lpszText) 
		return m_lpszText;
	return _T("");
}

LPCTSTR TextStyle::GetDrawText()
{
	if (m_lpszText)
		return m_lpszText;
	else
		return GetOwner()->GetText();
}

UINT TextStyle::GetAlign()
{
	return m_uAlignStyle;
}

void TextStyle::SetAlign(UINT uAlignStyle)
{
	m_uAlignStyle = uAlignStyle;
}

void TextStyle::SetTextColor(DWORD dwColor, int nIndex)
{
	if (nIndex >= 0 && nIndex < 5)
	{
		ColorObjExHelper::SetColor(&m_pArrColorObj[nIndex], dwColor);
	}
}

DWORD TextStyle::GetTextColor(int nIndex)
{
	if (nIndex >= 0 && nIndex<5)
	{
		if (m_pArrColorObj[nIndex] == NULL)
			nIndex = 0;
		return ColorObjExHelper::GetColor(m_pArrColorObj[nIndex]); 
	}
	return GetUIRes()->GetDefaultColor()->GetColor();
}


void TextStyle::Render( IRenderDC* pRenderDC, RECT& rcPaint )
{
	int uState = GetDrawState();
	if (!MaskState((UITYPE_STATE)uState))
	{
		return;
	}

	/*int nIndex = uState - UISTATE_NORMAL;
	if (uState >= UISTATE_CKNORMAL)
	nIndex = uState - UISTATE_CKNORMAL;*/

	int nIndex = 0;
	if (uState & UISTATE_CKNORMAL)
	nIndex = 4;
	else if (uState & UISTATE_DISABLE)
	nIndex = 3;
	else if (uState & UISTATE_DOWN)
	nIndex = 2;
	else if (uState & UISTATE_OVER)
	nIndex = 1;
	else
	nIndex = 0;
	

	int nAlpha = GetDrawAlpha();
	DWORD dwColor = GetTextColor(nIndex);
	dwColor = add_alpha_to_color(dwColor, nAlpha);

	pRenderDC->DrawText(GetFontObj(), GetRect(), GetDrawText(), dwColor, GetAlign(), GetFontEffect(), GetFontType());
}

void TextStyle::OnLanguageChange()
{
	if (m_lpszTextKey)
		SetAttribute(_T("text"), m_lpszTextKey);

}

void TextStyle::SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue)//重载基类
{
	if (equal_icmp(lpszName, _T("text")))
	{
		bool bFind = false;
		tstring strValue = I18NSTR(lpszValue, &bFind);
		SetText(strValue.c_str());
		if (lpszValue[0] == _T('#')/* && bFind*/)
			copy_str(m_lpszTextKey, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("font")))
	{
		SetFontObj(GetUIRes()->GetFont(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("textcolor")) || equal_icmp(lpszName, _T("color")))
	{
		tstring strSpliter = lpszValue;
		int i = strSpliter.find_first_of(_T('|'));
		int index = 0;
		while (i >= 0 && index < 5)
		{
			tstring strValue = strSpliter.substr(0, i);
			if (!strValue.empty())
			{
				ColorObjExHelper::SetColor(&m_pArrColorObj[index], strValue.c_str());
			}
			else if (index > 0)
			{
				ColorObjExHelper::CopyColor(&m_pArrColorObj[index], &m_pArrColorObj[index - 1]); // 不配置使用上次颜色
			}
			strSpliter = strSpliter.substr(i + 1);
			i = strSpliter.find_first_of(_T('|'));
			index++;
		}
		if (index < 5)
		{
			tstring strValue = strSpliter.substr(0, i);
			if (!strValue.empty())
			{
				ColorObjExHelper::SetColor(&m_pArrColorObj[index], strValue.c_str());
			}
			else if (index > 0)
			{
				ColorObjExHelper::CopyColor(&m_pArrColorObj[index], &m_pArrColorObj[index - 1]); // 不配置使用上次颜色
			}
		}
	}
	else if (equal_icmp(lpszName, _T("halign")))
	{
		if(equal_icmp(lpszValue, _T("left"))) {

			if (GetUIEngine()->IsRTLLayout())
			{
				m_uAlignStyle &= ~(DT_CENTER | DT_LEFT);
				m_uAlignStyle |= DT_RIGHT;

			}
			else
			{
				m_uAlignStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uAlignStyle |= DT_LEFT;
			}
		}
		else if(equal_icmp(lpszValue, _T("center"))) 
		{
			m_uAlignStyle &= ~(DT_LEFT | DT_RIGHT);
			m_uAlignStyle |= DT_CENTER;
		}
		else if(equal_icmp(lpszValue, _T("right"))) 
		{

			if (GetUIEngine()->IsRTLLayout())
			{
				m_uAlignStyle &= ~(DT_RIGHT | DT_CENTER);
				m_uAlignStyle |= DT_LEFT;

			}
			else
			{
				m_uAlignStyle &= ~(DT_LEFT | DT_CENTER);
				m_uAlignStyle |= DT_RIGHT;
			}
		}
	}
	else if (equal_icmp(lpszName, _T("valign")))
	{
		if (equal_icmp(lpszValue, _T("top")))
		{
			m_uAlignStyle &= ~(DT_BOTTOM | DT_VCENTER);
			m_uAlignStyle |= DT_TOP;
		}
		else if (equal_icmp(lpszValue, _T("center")))
		{
			m_uAlignStyle &= ~(DT_BOTTOM | DT_TOP);
			m_uAlignStyle |= DT_VCENTER;
		}
		else if (equal_icmp(lpszValue, _T("bottom")))
		{
			m_uAlignStyle &= ~(DT_TOP | DT_VCENTER);
			m_uAlignStyle |= DT_BOTTOM;	
		}
	}
	else if (equal_icmp(lpszName, _T("singleline")))
	{
		if (!!_ttoi(lpszValue))
		{
			m_uAlignStyle |= DT_SINGLELINE;
		}
		else
		{
			m_uAlignStyle &= ~DT_SINGLELINE;
			m_uAlignStyle |= DT_WORDBREAK|DT_EDITCONTROL;
		}
	}
	else if (equal_icmp(lpszName, _T("prefix")))
	{
		if (!!_ttoi(lpszValue))
		{
			m_uAlignStyle &= ~DT_NOPREFIX;
		}
		else
		{
			m_uAlignStyle |= DT_NOPREFIX;
		}
	}
	else if (equal_icmp(lpszName, _T("endellipsis")))
	{
		if (!!_ttoi(lpszValue))
		{
			m_uAlignStyle &= ~DT_PATH_ELLIPSIS;
			m_uAlignStyle |= DT_END_ELLIPSIS;
		}
		else
		{
			m_uAlignStyle &= ~DT_END_ELLIPSIS;
		}
	}
	else if (equal_icmp(lpszName, _T("pathellipsis")))
	{
		if (!!_ttoi(lpszValue))
		{
			m_uAlignStyle &= ~DT_END_ELLIPSIS;
			m_uAlignStyle |= DT_PATH_ELLIPSIS;
		}
		else
		{
			m_uAlignStyle &= ~DT_PATH_ELLIPSIS;
		}
	}
	else if (equal_icmp(lpszName, _T("fonteffect")) || equal_icmp(lpszName, _T("grow")))
	{
		m_nFontEffect = GetFontEffectValueFromStr(lpszValue);
	}
	else if (equal_icmp(lpszName, _T("fonttype")))
	{
		m_eFontType = GetFontTypeValueFromStr(lpszValue);
	}
	else if (GetUIEngine()->IsRTLLayout() && equal_icmp(lpszName, _T("rtlreading")))
	{
		if (!_ttoi(lpszValue))
			m_uAlignStyle |= DT_RTLREADING;
		else
			m_uAlignStyle &= ~DT_RTLREADING;
	}
	else
	{
		StyleObj::SetAttribute(lpszName, lpszValue);
	}
}

void TextStyle::Copy(StyleObj* pStyleObj)
{
	TextStyle *pTextStyle = (TextStyle*)pStyleObj;

	LPCTSTR lpszTextKey = pTextStyle->m_lpszTextKey;
	if (lpszTextKey && lpszTextKey[0] == _T('#'))
	{
		this->SetAttribute(_T("text"), lpszTextKey);
	}
	else
	{
		LPCTSTR lpszValue = pTextStyle->m_lpszText;
		if (lpszValue) SetText(lpszValue);
	}

	m_pFontObj = pTextStyle->m_pFontObj;
	m_uAlignStyle = pTextStyle->m_uAlignStyle;
	m_nFontEffect = pTextStyle->m_nFontEffect;
	m_eFontType = pTextStyle->m_eFontType;
	
	for (int i = 0; i < 5; i++)
		ColorObjExHelper::CopyColor(&m_pArrColorObj[i], &pTextStyle->m_pArrColorObj[i]);

	StyleObj::Copy(pStyleObj);
}

UITYPE_FONTEFFECT TextStyle::GetFontEffect()
{
	return m_nFontEffect;
}

void TextStyle::SetFontEffect(UITYPE_FONTEFFECT eFontEffect)
{
	m_nFontEffect = eFontEffect;
}

UITYPE_FONT TextStyle::GetFontType()
{
	return m_eFontType;
}

void TextStyle::SetFontType(UITYPE_FONT eFontType)
{
	m_eFontType = eFontType;
}

void TextStyle::ReleaseResource()
{
	if (m_lpszText)
		delete[] m_lpszText;
	m_lpszText = NULL;

	if (m_lpszTextKey)
		delete[] m_lpszTextKey;
	m_lpszTextKey = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
FillStyle::FillStyle(CControlUI* pOwner):StyleObj(pOwner)
{
	m_pFillColor = NULL;
	m_pBorderColor = NULL;
	m_pGradientColor1 = NULL;
	m_pGradientColor2 = NULL;
	m_nBorderLineWidth = 0;
	m_nCornerRadius = 0;
	m_bGradient = false;
	m_bGradientVert = true;
	m_nAlpha = -1;
}

FillStyle::~FillStyle(void)
{
	ColorObjExHelper::ClearColor(&m_pFillColor);
	ColorObjExHelper::ClearColor(&m_pBorderColor);
	ColorObjExHelper::ClearColor(&m_pGradientColor1);
	ColorObjExHelper::ClearColor(&m_pGradientColor2);
}

void FillStyle::Render(IRenderDC* pRenderDC, RECT& rcPaint)
{
	int uState = GetDrawState();
	if (!MaskState((UITYPE_STATE)uState))
	{
		return;
	}

	int nAlpha = GetDrawAlpha();
	if (!IsGradient())
	{
		DWORD dwFillColor = GetFillColor();
		DWORD dwBorderColor = GetBorderColor();
		dwFillColor = add_alpha_to_color(dwFillColor, nAlpha);
		dwBorderColor = add_alpha_to_color(dwBorderColor, nAlpha);

		pRenderDC->DrawRoundRect(GetRect(), dwFillColor, dwBorderColor, DPI_SCALE(GetBorderWidth()), DPI_SCALE(GetCornerRadius()), DPI_SCALE(GetCornerRadius()));
	}
	else
	{
		DWORD dwGradientColor1 = GetGradientColor1();
		DWORD dwGradientColor2 = GetGradientColor2();
		DWORD dwBorderColor = GetBorderColor();
		dwGradientColor1 = add_alpha_to_color(dwGradientColor1, nAlpha);
		dwGradientColor2 = add_alpha_to_color(dwGradientColor2, nAlpha);
		dwBorderColor = add_alpha_to_color(dwBorderColor, nAlpha);
		pRenderDC->DrawGradientRoundRect(GetRect(), dwGradientColor1, dwGradientColor2, dwBorderColor, DPI_SCALE(GetBorderWidth()), DPI_SCALE(GetCornerRadius()), DPI_SCALE(GetCornerRadius()), IsGradientVert());
	}
}

void  FillStyle::SetFillColor(DWORD dwColor)
{
	ColorObjExHelper::SetColor(&m_pFillColor, dwColor);
}

DWORD FillStyle::GetFillColor()
{
	if (m_pFillColor == NULL)
		return 0x00FFFFFF;
	return ColorObjExHelper::GetColor(m_pFillColor);
}

void  FillStyle::SetBorderColor(DWORD dwColor)
{
	ColorObjExHelper::SetColor(&m_pBorderColor, dwColor);
}

DWORD FillStyle::GetBorderColor()
{
	return ColorObjExHelper::GetColor(m_pBorderColor);
}

void FillStyle::SetBorderWidth(int lineWidth)
{
	m_nBorderLineWidth = lineWidth;
}

int  FillStyle::GetBorderWidth()
{
	return m_nBorderLineWidth;
}

void FillStyle::SetCornerRadius(int cornerRadius)
{
	m_nCornerRadius = cornerRadius;
}

int  FillStyle::GetCornerRadius()
{
	return m_nCornerRadius;
}


void FillStyle::SetGradientColor1(DWORD dwColor1)
{
	ColorObjExHelper::SetColor(&m_pGradientColor1, dwColor1);
}

DWORD FillStyle::GetGradientColor1()
{
	return ColorObjExHelper::GetColor(m_pGradientColor1);
}

void FillStyle::SetGradientColor2(DWORD dwColor2)
{
	ColorObjExHelper::SetColor(&m_pGradientColor2, dwColor2);
}

DWORD FillStyle::GetGradientColor2()
{
	return ColorObjExHelper::GetColor(m_pGradientColor2);
}

void FillStyle::SetGradient(bool bGradient)
{
	m_bGradient = bGradient;
}

bool FillStyle::IsGradient()
{
	return m_bGradient;
}

void FillStyle::SetGradientVert(bool bVert)
{
	m_bGradientVert = bVert;
}

bool FillStyle::IsGradientVert()
{
	return m_bGradientVert;
}

void FillStyle::SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue)//重载基类
{
	if (equal_icmp(lpszName, _T("fillcolor")))
	{
		ColorObjExHelper::SetColor(&m_pFillColor, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("gradientcolor1")))
	{
		ColorObjExHelper::SetColor(&m_pGradientColor1, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("gradientcolor2")))
	{
		ColorObjExHelper::SetColor(&m_pGradientColor2, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("bordercolor")))
	{
		ColorObjExHelper::SetColor(&m_pBorderColor, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("gradient")))
	{
		SetGradient((bool)!!_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("gradientmode")))
	{
		SetGradientVert(GetGradientModeValueFromStr(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("borderwidth")))
	{
		SetBorderWidth(_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("cornerradius")))
	{
		SetCornerRadius(_ttoi(lpszValue));
	}
	else 
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

void FillStyle::Copy( StyleObj* pStyleObj )
{
	FillStyle* pFillStyle = static_cast<FillStyle*>(pStyleObj);
	ColorObjExHelper::CopyColor(&m_pFillColor, &pFillStyle->m_pFillColor);
	ColorObjExHelper::CopyColor(&m_pBorderColor, &pFillStyle->m_pBorderColor);
	ColorObjExHelper::CopyColor(&m_pGradientColor1, &pFillStyle->m_pGradientColor1);
	ColorObjExHelper::CopyColor(&m_pGradientColor2, &pFillStyle->m_pGradientColor2);

	m_nCornerRadius = pFillStyle->m_nCornerRadius;
	m_nBorderLineWidth = pFillStyle->m_nBorderLineWidth;
	m_bGradient = pFillStyle->m_bGradient;
	m_bGradientVert = pFillStyle->m_bGradientVert;
	StyleObj::Copy(pStyleObj);
}

//////////////////////////////////////////////////////////////////////////
LineStyle::LineStyle( CControlUI* pOwner )
:StyleObj(pOwner)
,m_uLineStyle(LineStyleSolid)
,m_uLineWidth(1)
, m_pColorObj(NULL)
,m_bVert(false)
{
	m_nAlpha = -1;
}

LineStyle::~LineStyle()
{
	ColorObjExHelper::ClearColor(&m_pColorObj);

}


void LineStyle::SetStyle(LineStyleType lineStyle)
{
	m_uLineStyle = lineStyle;
}

LineStyleType LineStyle::GetStyle()
{
	return m_uLineStyle;
}

void LineStyle::SetWidth(UINT LineWidth )
{
	m_uLineWidth = LineWidth;
}

UINT LineStyle::GetWidth()
{
	return m_uLineWidth;
}

void LineStyle::SetColor(DWORD lineColor )
{
	ColorObjExHelper::SetColor(&m_pColorObj, lineColor);
}

DWORD LineStyle::GetColor()
{
	return ColorObjExHelper::GetColor(m_pColorObj);
}


void LineStyle::SetVert(bool bVert)
{
	m_bVert = bVert;
}

bool LineStyle::IsVert()
{
	return m_bVert;
}


void LineStyle::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{
	if (equal_icmp(lpszName, _T("color")))
	{
		ColorObjExHelper::SetColor(&m_pColorObj, lpszValue);
	}
	else if (equal_icmp(lpszName, _T("width")))
	{
		SetWidth(_ttoi(lpszValue));
	}
	else if (equal_icmp(lpszName, _T("style")))
	{
		SetStyle(GetLineTypeValueFromStr(lpszValue));
	
	}
	else if (equal_icmp(lpszName, _T("vert")))
	{
		SetVert(!!_ttoi(lpszValue));
	}
	else 
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}

void LineStyle::Render( IRenderDC* pRenderDC, RECT& rcPaint )
{
	int uState = GetDrawState();
	if (!MaskState((UITYPE_STATE)uState))
	{
		return;
	}

	DWORD dwColor = GetColor();
	dwColor = add_alpha_to_color(dwColor, GetDrawAlpha());

	RECT rcItem = GetRect();
	POINT ptBegin;
	POINT ptEnd;
	if (!m_bVert)
	{
		int nMiddle = rcItem.top + (rcItem.bottom - rcItem.top)/2;
		ptBegin.x = rcItem.left;
		ptBegin.y = nMiddle;

		ptEnd.x = rcItem.right;
		ptEnd.y = nMiddle;
	}
	else
	{
		int nMiddle = rcItem.left + (rcItem.right - rcItem.left)/2;

		ptBegin.x = nMiddle;
		ptBegin.y = rcItem.top;

		ptEnd.x = nMiddle;
		ptEnd.y = rcItem.bottom;
	}

	pRenderDC->Line(&ptBegin, &ptEnd, m_uLineStyle, DPI_SCALE(m_uLineWidth), dwColor);
}

void LineStyle::Copy( StyleObj* pStyleObj )
{
	LineStyle* pLineStyle = static_cast<LineStyle*>(pStyleObj);
	m_bVert = pLineStyle->m_bVert;
	m_uLineStyle = pLineStyle->m_uLineStyle;
	m_uLineWidth = pLineStyle->m_uLineWidth;
	ColorObjExHelper::CopyColor(&m_pColorObj, &pLineStyle->m_pColorObj);
	StyleObj::Copy(pStyleObj);
}