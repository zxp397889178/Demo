#include "stdafx.h"
#include "ResObj.h"
/////////////////////////////////////////////////////////////////////////////////////////
DUI_API bool RtlRect( RECT & rcParent, PRECT pRectChild )
{
	bool bRet = false;
	if (GetUIEngine()->IsRTLLayout())
	{
		int nWidth = pRectChild->right - pRectChild->left;
		pRectChild->right = rcParent.right - pRectChild->left + rcParent.left;
		pRectChild->left  = pRectChild->right - nWidth;
	}
	return bRet;
}
bool PraseHdpiImagePath(ModuleObj* pModule, tstring& strPath);
bool PraseHdpiImagePathInTheme(ModuleObj* pModule, tstring& strPath);
ModuleObj* PraseImagePath(tstring strPathIn, tstring& strPathOut,  bool& bUpdateColor, SIZE& szCellNum, short* arrPart);
void XMLToTemplateObj(TemplateObj* pParent, xml_node<>* pRoot);
//先过滤window，style
void XMLToTemplateObjWithFilter(TemplateObj* pParent, xml_node<>* pRoot, ITemplateFilter* pFilter);
UINT GetResTypeFromStr(const char* lpszStr);
UINT GetResTypeFromStrInternal(const char* lpszStr);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ResObj* ResObj::m_pResManager = NULL;
static bool  ms_bLoadFromResource = false;
static UITYPE_IMAGE  ms_eImageType = UIIAMGE_HBITMAP;
static UITYPE_FONT  ms_eFontType = UIFONT_GDI;
static TextRenderingHint ms_eTextRenderingHintGlobal = TextRenderingHintSystemDefault;

FontObj* ResObj::ms_pDefaultFontObj = NULL;                            // 保存默认的字体，资源配置的第一种字体为默认字体
ColorObj* ResObj::ms_pDefaultColorObj = NULL;
CursorObj* ResObj::ms_pDefaultCursorObj = NULL;                         // 默认光标
UITYPE_SMOOTHING  ResObj::ms_eImageQualityType = UISMOOTHING_HighSpeed;

ResObj* GetResObj()
{
	return ResObj::GetInstance();
}
ResObj* ResObj::GetInstance()
{
	if (!m_pResManager)
		m_pResManager = new ResObj;
	return m_pResManager;
}
void ResObj::ReleaseInstance()
{
	if (ms_pDefaultFontObj)
		delete ms_pDefaultFontObj;
	ms_pDefaultFontObj = NULL;

	if (ms_pDefaultColorObj)
		delete ms_pDefaultColorObj;
	ms_pDefaultColorObj = NULL;

	if (m_pResManager != NULL)
		delete m_pResManager;
	m_pResManager = NULL;
}

ResObj::ResObj(void)
{
#ifdef _DEBUG
	m_bCacheXML = false;
#else
	m_bCacheXML = true;
#endif
	m_strLanguage = _T("CHN");
	m_dHue = 0.0;
	m_dSaturation = 100.0;
	m_dLightness = 100.0; //HSL值
	LOGFONT lf = {0};
	SystemParametersInfo(SPI_GETICONTITLELOGFONT,sizeof(LOGFONT),&lf,0);
	lf.lfHeight = -12;
	m_strDefaultFontName = lf.lfFaceName;
	ms_pDefaultFontObj = new FontObj();
	ms_pDefaultFontObj->SetLogFont(&lf);
	ms_pDefaultFontObj->SetCached(true);
	ms_pDefaultFontObj->SetUsedDefaultFaceName(true);

	ms_pDefaultColorObj = new ColorObj();
	ms_pDefaultColorObj->SetColor(0xFF000000);


	m_rcVScrollPositon.left = 10;
	m_rcVScrollPositon.top = 0;
	m_rcVScrollPositon.right = 0;
	m_rcVScrollPositon.bottom = 0;

	m_rcHScrollPositon.left = 0;
	m_rcHScrollPositon.top = 10;
	m_rcHScrollPositon.right = 0;
	m_rcHScrollPositon.bottom = 0; 

	m_rcWindowShadowEdge.left = 18;
	m_rcWindowShadowEdge.right = 18;
	m_rcWindowShadowEdge.top = 13;
	m_rcWindowShadowEdge.bottom = 23;

	m_pDefModulePathObj  = NULL;

	CursorObj* pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("ARROW"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_ARROW));
	m_mapCursorsTable[_T("ARROW")] = pCursorObj;
	ms_pDefaultCursorObj = pCursorObj;

	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("HAND"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_HAND));
	m_mapCursorsTable[_T("HAND")] = pCursorObj;

	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("IBEAM"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_IBEAM));
	m_mapCursorsTable[_T("IBEAM")] = pCursorObj;


	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("WAIT"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_WAIT));
	m_mapCursorsTable[_T("WAIT")] = pCursorObj;


	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("CROSS"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_CROSS));
	m_mapCursorsTable[_T("CROSS")] = pCursorObj;


	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("UPARROW"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_UPARROW));
	m_mapCursorsTable[_T("UPARROW")] = pCursorObj;


	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("SIZE"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_SIZE));
	m_mapCursorsTable[_T("SIZE")] = pCursorObj;


	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("SIZENWSE"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_SIZENWSE));
	m_mapCursorsTable[_T("SIZENWSE")] = pCursorObj;


	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("SIZENESW"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_SIZENESW));
	m_mapCursorsTable[_T("SIZENESW")] = pCursorObj;

	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("SIZEWE"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_SIZEWE));
	m_mapCursorsTable[_T("SIZEWE")] = pCursorObj;


	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("SIZENS"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_SIZENS));
	m_mapCursorsTable[_T("SIZENS")] = pCursorObj;

	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("SIZEALL"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_SIZEALL));
	m_mapCursorsTable[_T("SIZEALL")] = pCursorObj;

	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("NO"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_NO));
	m_mapCursorsTable[_T("NO")] = pCursorObj;

	pCursorObj = new CursorObj;
	pCursorObj->SetId(_T("HELP"));
	pCursorObj->SetHCurosr(::LoadCursor(NULL, IDC_HELP));
	m_mapCursorsTable[_T("HELP")] = pCursorObj;

	m_pTemplateFilter = NULL;
	m_pThemeRdbFile = NULL;
	m_pHDPIRdbFile = NULL;
	m_fHDPIImageNinePatchScale = 1.0f;
	m_bHDPIAutoStretchOnDrawing9Image = true;

}

ResObj::~ResObj(void)
{
	std::map<tstring, ImageObj*>::iterator pos;
	for (pos = m_mapImageObjTable.begin(); pos != m_mapImageObjTable.end(); ++pos)
	{
		ImageObj* pImage = pos->second;
		delete pImage;
	}
	m_mapImageObjTable.clear();


	TemplateIterator AttrIterator;			
	for(AttrIterator=m_mapWindowTable.begin();AttrIterator!=m_mapWindowTable.end();++AttrIterator)
	{
		TemplateObj* pAttr = static_cast<TemplateObj*>(AttrIterator->second);
		delete pAttr;
	}
	m_mapWindowTable.clear();

// 	for(AttrIterator=m_mapStyleTable.begin();AttrIterator!=m_mapStyleTable.end();++AttrIterator)
// 	{
// 		TemplateObj* pAttr = static_cast<TemplateObj*>(AttrIterator->second);
// 		delete pAttr;			
// 	}
// 	m_mapStyleTable.clear();

		
	for(map<UINT, TemplateObj*>::iterator menuIterator=m_mapMenuTable.begin();menuIterator!=m_mapMenuTable.end();++menuIterator)
		delete menuIterator->second;			
	m_mapMenuTable.clear();

	for (map<tstring, FontObj*>::iterator itFonts = m_mapFontsTable.begin(); itFonts != m_mapFontsTable.end(); ++itFonts)
		delete itFonts->second;
	m_mapFontsTable.clear();

	for (map<tstring, ColorObj*>::iterator itColors = m_mapColorsTable.begin(); itColors != m_mapColorsTable.end(); ++itColors)
		delete itColors->second;
	m_mapColorsTable.clear();

	for (map<tstring, CurveObj*>::iterator itCurves = m_mapCurvesTable.begin(); itCurves != m_mapCurvesTable.end(); ++itCurves)
		delete itCurves->second;
	m_mapCurvesTable.clear();

	for (map<tstring, CursorObj*>::iterator itCursors = m_mapCursorsTable.begin(); itCursors !=  m_mapCursorsTable.end(); ++itCursors)
		delete itCursors->second;
	m_mapCursorsTable.clear();
		
	for(map<tstring, ModuleObj*>::iterator moduleIterator=m_mapModuleObjTable.begin(); moduleIterator!=m_mapModuleObjTable.end();++moduleIterator)
		delete moduleIterator->second;		
	m_mapModuleObjTable.clear();

	for(map<tstring, ImagePathObj*>::iterator imageIterator=m_mapImagePathObjTable.begin(); imageIterator!=m_mapImagePathObjTable.end();++imageIterator)
		delete imageIterator->second;		
	m_mapImagePathObjTable.clear();

	if (m_pThemeRdbFile)
	{
		delete m_pThemeRdbFile;
	}

	if (m_pHDPIRdbFile)
	{
		delete m_pHDPIRdbFile;
	}
	
}

LPCTSTR ResObj::GetImagePathFromId(LPCTSTR lpszId)
{
	std::map<tstring, ImagePathObj*>::iterator imageIterator = m_mapImagePathObjTable.find(lpszId);
	if (imageIterator!=m_mapImagePathObjTable.end())
		return imageIterator->second->GetPath(m_strImageLanguage.c_str());
	return _T("");
}

void ResObj::ReloadImageById(LPCTSTR lpszId)
{
	std::map<tstring, ImageObj*>::iterator itImageObjTable = m_mapImageObjTable.find(lpszId);
	if (itImageObjTable != m_mapImageObjTable.end())
	{
		ImageObj* pImage = itImageObjTable->second;
		if (pImage)
		{
			pImage->Reload();
		}
	}
}

CursorObj* ResObj::LoadCursor(LPCTSTR lpszPath)
{
	tstring strPath = lpszPath;
	if (lpszPath[0] == _T('#'))
		strPath = GetImagePathFromId(lpszPath);

	bool bUpdateColor = false;
	SIZE szCellNum = {1,1};
	short arrPart[4] = {0};
	ModuleObj* pModule = PraseImagePath(strPath, strPath,  bUpdateColor, szCellNum, arrPart);
	if (pModule)
	{
		if (pModule->GetResType() == UIRES_FROM_FILE)
			strPath = pModule->GetModulePath()  + strPath;
	}

	HCURSOR hCursor = (HCURSOR)::LoadImage(NULL, strPath.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
	if (!hCursor) return NULL;
	CursorObj* pCursorObj = new CursorObj;
	pCursorObj->SetHCurosr(hCursor);
	return pCursorObj;
}

ImageObj* ResObj::LoadImageFromThemePath(ModuleObj* pModule, LPCTSTR lpszPath, UITYPE_IMAGE eImageType, bool& bHdpi)
{
	bHdpi = false;
	if (!pModule)
		return NULL;

	if (_tcslen(GetThemePath()) == 0)
		return NULL;

	tstring strPath = lpszPath;
	CRdbParser* pRdbFile = GetThemeRdbFile();
	do 
	{
		if (!pRdbFile)
		{
			break;
		}

		CRdbBuf rdbbuf;

		bHdpi = PraseHdpiImagePathInTheme(pModule, strPath);
		tstring strThemePath = pModule->GetModuleName() + tstring(_T("\\")) + strPath;

		pRdbFile->ReadFile(&rdbbuf, strThemePath.c_str());
		if (rdbbuf.GetSize() == 0)
		{
			if (pRdbFile->IsCoexistence())
			{
				break;
			}

			return NULL;
		}

		return LoadImage(rdbbuf.GetBuf(), rdbbuf.GetSize(), eImageType);

	} while (false);
	
	bHdpi = PraseHdpiImagePathInTheme(pModule, strPath);
	tstring strThemePath = GetResObj()->GetThemePath();
	strThemePath = strThemePath + pModule->GetModuleName() + _T("\\") + strPath;
	if (::PathFileExists(strThemePath.c_str()))
	{
		return LoadImage(strThemePath, eImageType);
	}

	return NULL;
}

ImageObj* ResObj::LoadImageFromHDPIPath(ModuleObj* pModule, LPCTSTR lpszPath, UITYPE_IMAGE eImageType)
{
	if (!pModule)
		return NULL;

	if (_tcslen(GetHDPIImagePath()) == 0)
		return NULL;

	if (_tcslen(GetUIEngine()->GetDPIName()) == 0)
		return NULL;

	tstring strImagePath = lpszPath;
	if (_tcsicmp(strImagePath.substr(0, 4).c_str(), _T("res\\")) != 0)
		return NULL;


	CRdbParser* pRdbFile = GetHDPIImageRdbFile();
	if (pRdbFile)
	{
		CRdbBuf rdbbuf;
		strImagePath = tstring(pModule->GetModuleName()) + _T("\\res_") + GetUIEngine()->GetDPIName() + _T("\\") + strImagePath.substr(4);
		pRdbFile->ReadFile(&rdbbuf, strImagePath.c_str());
		if (rdbbuf.GetSize() == 0) return NULL;
		return LoadImage(rdbbuf.GetBuf(), rdbbuf.GetSize(), eImageType);
	}
	else
	{

		strImagePath = GetResObj()->GetHDPIImagePath() + tstring(pModule->GetModuleName()) + _T("\\res_") + GetUIEngine()->GetDPIName() + _T("\\") + strImagePath.substr(4);
		if (::PathFileExists(strImagePath.c_str()))
		{
			return LoadImage(strImagePath, eImageType);
		}
	}
	return NULL;
}


ImageObj* ResObj::LoadImage(LPCTSTR lpszPath, bool bCache, UITYPE_IMAGE eImageType)
{
	if (!lpszPath || _tcslen(lpszPath) == 0)
		return NULL;
	if (eImageType == UIIAMGE_DEFAULT)
		eImageType = ms_eImageType;

	if (bCache)
	{
		std::map<tstring, ImageObj*>::iterator pos = m_mapImageObjTable.find(lpszPath);	
		if (pos != m_mapImageObjTable.end())
			return pos->second->Get();
	}

	//解析文件名
	tstring strPath = lpszPath;
	if (lpszPath[0] == _T('#'))
		strPath = GetImagePathFromId(lpszPath);

	bool bUpdateColor = false;
	SIZE szCellNum = { 1, 1 };
	short arrPart[4] = { 0 };
	ModuleObj* pModule = PraseImagePath(strPath, strPath, bUpdateColor, szCellNum, arrPart);

	UITYPE_RES eLoadType = UIRES_FROM_FILE;
	if (pModule) eLoadType = pModule->GetResType();

	ImageObj* pImageObj = NULL;
	bool bHdpi = false;
	// 先从从主题加载图片
	if (pModule && !pImageObj)
	{
		pImageObj = LoadImageFromThemePath(pModule, strPath.c_str(), eImageType, bHdpi);
		if (!pImageObj)
			bHdpi = false;
	}

	// 从高分屏目录中加载图片
	if (pModule && !pImageObj)
	{
		pImageObj = LoadImageFromHDPIPath(pModule, strPath.c_str(), eImageType);
		if (pImageObj)
		{
			bHdpi = true;
		}
	}

	if (!pImageObj)
	{
		// 从文件中导入
		if (eLoadType == UIRES_FROM_FILE)
		{
			if (pModule)
			{
				bHdpi = PraseHdpiImagePath(pModule, strPath);
				strPath = pModule->GetModulePath() + strPath;
			}
			pImageObj = LoadImage(strPath, eImageType);
		}
		else if (eLoadType == UIRES_FROM_RESOURCE) // 从资源中导入
		{
			UINT uResID = 0;
			tstring strResType;
			int i = strPath.find_last_of(_T('#'));
			if (i > 0)
			{
				uResID = _ttoi(strPath.substr(i + 1).c_str());
				strResType = strPath.substr(0, i);
			}
			else
			{
				uResID = _ttoi(strPath.c_str());
			}
			pImageObj = LoadImage(uResID, strResType.c_str(), eImageType);
		}
		else if (eLoadType == UIRES_FROM_RDB) // 从RDB文件中导入
		{
			CRdbBuf rdbbuf;
			CRdbParser* pRdbFile = pModule->GetRdbFile();
			if (!pRdbFile) return NULL;
			bHdpi = PraseHdpiImagePath(pModule, strPath);
			pRdbFile->ReadFile(&rdbbuf, strPath.c_str());
			if (rdbbuf.GetSize() == 0) return NULL;

			pImageObj = LoadImage(rdbbuf.GetBuf(), rdbbuf.GetSize(), eImageType);
		}
	}

	if (pImageObj)
	{
		//DUILOG(TRACE) << "LoadImage:" << Charset::UnicodeTochar(lpszPath) << " [" << __FUNCTION__ << ":" << __LINE__ << "]";
		// 修改9宫格参数
		if (bHdpi)
		{
			arrPart[0] = ceil((float)arrPart[0] * GetHDPI9ImageParamScale());
			arrPart[1] = ceil((float)arrPart[1] * GetHDPI9ImageParamScale());
			arrPart[2] = ceil((float)arrPart[2] * GetHDPI9ImageParamScale());
			arrPart[3] = ceil((float)arrPart[3] * GetHDPI9ImageParamScale());

		}

		RECT rcInset = {arrPart[0], arrPart[1], arrPart[2], arrPart[3]};
		pImageObj->SetInset(rcInset);
		pImageObj->SetCellNum(szCellNum.cx, szCellNum.cy);
		pImageObj->SetFilePath(lpszPath);
		pImageObj->SetAdjustColor(bUpdateColor);
		if (pImageObj->GetAdjustColor())
		{
			// 改变图片时只需要改变图片的色相，对比度和亮度不需要改变，所以默认都是100
			CImageLib::DoHLS(pImageObj, m_dHue, 100, 100);
		}
		if (bCache)
		{
			pImageObj->SetCached(true);
			m_mapImageObjTable[lpszPath] = pImageObj;
		}
	}
	return pImageObj;
	
}

ImageObj*  ResObj::LoadImage(tstring& strPath, UITYPE_IMAGE eImageType)
{
	ImageObj* pImageObj = NULL;
	Bitmap* bitmap = NULL;
	IStream* pStream = NULL;
	//icon图片处理
	if (eImageType  == UIIAMGE_HICON)
	{
		HICON hIcon = (HICON)::LoadImage(NULL,strPath.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);
		if (hIcon == NULL)
		{
			CImageLib::LoadImage(strPath.c_str(), &bitmap, &pStream);
			if (bitmap)
			{
				bitmap->GetHICON(&hIcon);
				delete bitmap;
				bitmap = NULL;
				if (pStream) pStream->Release();
				pStream = NULL;
			}
		}
		if (hIcon) pImageObj = new ImageObj(hIcon);
	}
	else // 其他图片格式
	{

		CImageLib::LoadImage(strPath.c_str(), &bitmap, &pStream);
		if (bitmap)
		{
			int nFrameCount = CImageLib::GetBitmapFrameCount(bitmap);
			if (eImageType  == UIIAMGE_HICON)
			{
				HICON hIcon = NULL;
				bitmap->GetHICON(&hIcon);
				delete bitmap;
				bitmap = NULL;
				if (pStream) pStream->Release();
				pStream = NULL;
				if (hIcon) pImageObj = new ImageObj(hIcon);
			}
			else if (nFrameCount > 1 || eImageType == UIIAMGE_BITMAP)
			{
				pImageObj = new ImageObj(bitmap, pStream);
			}
			else
			{
				if (eImageType  == UIIAMGE_HBITMAP)
					pImageObj = new ImageObj(CImageLib::BitmapToHBITMAP(bitmap));
				else if (eImageType  == UIIAMGE_PIXEL)
					pImageObj = new ImageObj(CImageLib::BitmapToPixels(bitmap), bitmap->GetWidth(), bitmap->GetHeight());
				delete bitmap;
				if (pStream) pStream->Release();
			}
		}
	}
	return pImageObj;
}

ImageObj*  ResObj::LoadImage(LPVOID lpData, long nSize, UITYPE_IMAGE eImageType)
{
	if (eImageType == UIIAMGE_DEFAULT)
		eImageType = ms_eImageType;
	ImageObj* pImage = NULL;
	Bitmap* bitmap = NULL;
	IStream* pStream = NULL;
	CImageLib::LoadImage(lpData, nSize, &bitmap, &pStream);
	if (bitmap)
	{
		int nFrameCount = CImageLib::GetBitmapFrameCount(bitmap);
		if (eImageType  == UIIAMGE_HICON)
		{
			HICON hIcon = NULL;
			bitmap->GetHICON(&hIcon);
			if (hIcon) pImage = new ImageObj(hIcon);
			delete bitmap;
			if (pStream) pStream->Release();
		}
		else if (nFrameCount > 1 ||  (eImageType == UIIAMGE_BITMAP))
		{
			pImage = new ImageObj(bitmap);
		}
		else
		{
			if (eImageType  == UIIAMGE_HBITMAP)
				pImage = new ImageObj(CImageLib::BitmapToHBITMAP(bitmap));
			else if (eImageType  == UIIAMGE_PIXEL)
				pImage = new ImageObj(CImageLib::BitmapToPixels(bitmap), bitmap->GetWidth(), bitmap->GetHeight());
			delete bitmap;
			if (pStream) pStream->Release();
		}
	}
	return pImage;
}


ImageObj* ResObj::LoadImage(UINT uResID, LPCTSTR lpszType, UITYPE_IMAGE eImageType)
{
	if (eImageType == UIIAMGE_DEFAULT)
		eImageType = ms_eImageType;
	if (eImageType == UIIAMGE_HICON)
	{
		HICON hIcon = ::LoadIcon(GetEngineObj()->GetInstanceHandle(),MAKEINTRESOURCE(uResID));
		if (hIcon) return new ImageObj(hIcon);
	}
	ImageObj* pImageObj = NULL;
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
			pImageObj = LoadImage(lpData, nSize, eImageType);
			delete [] lpData;
		}	
	}
	return pImageObj;
}

void ResObj::ChangeImageHSL(COLORREF clrBackground)
{
	double nHue, nSaturation, lightness;
	CSSE::RGBtoHSL(clrBackground, &nHue, &nSaturation, &lightness);
	double h, s, l;
	CSSE::RGBtoHSL(_BASE_SKIN_COLOR, &h, &s, &l); 

	m_dHue = nHue - h;
	m_dSaturation = nSaturation / s * 100;
	m_dLightness = lightness / l * 100;

	std::map<tstring, ImageObj*>::iterator pos;
	for (pos = m_mapImageObjTable.begin(); pos != m_mapImageObjTable.end(); ++pos)
	{
		ImageObj* pImage = pos->second; 
		if (!pImage || !pImage->GetAdjustColor())
			continue;
		pImage->Reload();
	}
}


void  ResObj::RemoveImageFromTable(LPCTSTR lpszId)
{
	std::map<tstring, ImageObj*>::iterator pos = GetResObj()->m_mapImageObjTable.find(lpszId);
	if (pos !=  GetResObj()->m_mapImageObjTable.end())
		GetResObj()->m_mapImageObjTable.erase(pos);

}

bool ResObj::LoadSkin(LPCTSTR lpszFile)
{
	return LoadSkin(lpszFile, NULL, _RES_TYPE_ALL);
}

bool ResObj::LoadSkin(const void* data, size_t data_len)
{
	xml_document<> document;
	rapidxml::file<char> file;

	if (!file.open((char*)data, data_len)) return false;
	if (!file.parse(document)) return false;

	return LoadSkin(&document, NULL, _RES_TYPE_ALL);
}

bool ResObj::LoadSkin(xml_document<>* pXmlDoc, LPCTSTR lpszName, UINT uResType)
{
	xml_node<>* pRootEle;
	pRootEle = pXmlDoc->first_node();
	if(pRootEle == NULL)
		return false;

	xml_node<>* pEle = NULL;
	xml_node<>* pEleSub = NULL;
	xml_attribute<>* attribute;

	if (uResType & _RES_TYPE_WINDOW)
	{
		tstring strNameTemp;
		tstring strValueTemp;
		tstring strIdTemp;
		TemplateIterator AttrIterator;
		TemplateObj* pTempObj;
		const char* pszId;
		const char* pszStore;
		const char* szName;
		const char* szValue;
		xml_attribute<>* pXmlAttr;
		TemplateObj* pAttr;

		for(pEleSub = pRootEle->first_node("Window"); pEleSub; pEleSub=pEleSub->next_sibling("Window"))
		{
			pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() :NULL;
			if (!pszId)
				continue;
			StringHelper::Utf8ToNativeStrPtr(strIdTemp, pszId);
			if ((lpszName
				&& _tcsicmp(strIdTemp.c_str(), lpszName) != 0))
				continue;

			if (!m_mapWindowTable.empty())
			{
				AttrIterator = m_mapWindowTable.find(strIdTemp.c_str());
				if (AttrIterator != m_mapWindowTable.end())
				{
					pAttr = static_cast<TemplateObj*>(AttrIterator->second);
					delete pAttr;
					m_mapWindowTable.erase(AttrIterator);
				}
			}

			pTempObj = new TemplateObj;

			pszStore =  (attribute = pEleSub->first_attribute("store")) ? attribute->value() :NULL;
			if (pszStore && strcmp(pszStore, "0") == 0)
				pTempObj->SetStore(false);
			if (!GetCacheXML())
				pTempObj->SetStore(false);
			
			pXmlAttr = pEleSub->first_attribute();
			while (pXmlAttr)
			{
				szName = pXmlAttr->name();
				szValue = pXmlAttr->value();
				if (m_pTemplateFilter)
				{
					if (!m_pTemplateFilter->OnTemplateAttributes(szName, szValue))
					{
						delete pTempObj;
						pTempObj = NULL;
						break;
					}
				}

				StringHelper::Utf8ToNativeStrPtr(strNameTemp, szName);
				StringHelper::Utf8ToNativeStrPtr(strValueTemp, szValue);
				pTempObj->SetAttribute(strNameTemp.c_str(), strValueTemp.c_str());

				pXmlAttr = pXmlAttr->next_attribute();
			}

			if (!pTempObj)
			{
				continue;
			}

			m_mapWindowTable[strIdTemp] = pTempObj;

			XMLToTemplateObjWithFilter(pTempObj, pEleSub, m_pTemplateFilter);
			if (lpszName)
				break;
		}
	}

	if (uResType & _RES_TYPE_STYLE)
	{
		pEle=pRootEle->first_node("Style");
		if (pEle)
		{
			tstring strNameTemp;
			tstring strValueTemp;
			tstring strIdTemp;
			const char* pszId;
			TemplateIterator AttrIterator;
			TemplateObj* pAttr;
			TemplateObj* pTempObj;
			const char* pszStore;
			xml_attribute<>* pXmlAttr;
			const char* szName;
			const char* szValue;

			for(pEleSub=pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() :NULL;
				if (!pszId)
					continue;
				StringHelper::Utf8ToNativeStrPtr(strIdTemp, pszId);
#ifdef _DEBUG
				if (lpszName
					&& _tcsicmp(strIdTemp.c_str(), lpszName) != 0)
					continue;
#endif

				if (!m_mapStyleTable.empty())
				{
					AttrIterator = m_mapStyleTable.find(strIdTemp.c_str());
					if (AttrIterator != m_mapStyleTable.end())
					{
						pAttr = static_cast<TemplateObj*>(AttrIterator->second);
						delete pAttr;
						m_mapStyleTable.erase(AttrIterator);
					}
				}

				pTempObj = new TemplateObj;

				pszStore = (attribute = pEleSub->first_attribute("store")) ? attribute->value() :NULL;
				if (pszStore && strcmp(pszStore, "0") == 0)
					pTempObj->SetStore(false);

				if (!GetCacheXML())
					pTempObj->SetStore(false);

				pXmlAttr = pEleSub->first_attribute();
				while (pXmlAttr)
				{
					szName = pXmlAttr->name();
					szValue = pXmlAttr->value();
					if (m_pTemplateFilter)
					{
						if (!m_pTemplateFilter->OnTemplateAttributes(szName, szValue))
						{
							delete pTempObj;
							pTempObj = NULL;
							break;
						}
					}

					StringHelper::Utf8ToNativeStrPtr(strNameTemp, szName);
					StringHelper::Utf8ToNativeStrPtr(strValueTemp, szValue);
					pTempObj->SetAttribute(strNameTemp.c_str(), strValueTemp.c_str());
					pXmlAttr = pXmlAttr->next_attribute();
				}

				if (!pTempObj)
				{
					continue;
				}

				m_mapStyleTable[strIdTemp] = pTempObj;

				XMLToTemplateObjWithFilter(pTempObj, pEleSub, m_pTemplateFilter);
#ifdef _DEBUG
				if (lpszName)
					break;
#endif
			}
		}
	}

	if (uResType & _RES_TYPE_MENU)
	{
		pEle=pRootEle->first_node("Menu");
		if (pEle)
		{
			for(pEleSub=pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				const char* pszId = (attribute = pEleSub->first_attribute("UID")) ? attribute->value() :NULL;
				if (!pszId || (lpszName && _tcsicmp(Charset::UTF8ToNative(pszId).c_str(), lpszName) != 0))
					continue;
				TemplateObj* pTempObj = new TemplateObj;

				if (!m_mapMenuTable.empty())
				{
					std::map<UINT, TemplateObj*>::iterator menuIterator = m_mapMenuTable.find(atoi(pszId));
					if (menuIterator != m_mapMenuTable.end())
					{
						TemplateObj* pAttr=(*menuIterator).second;
						delete pAttr;
						m_mapMenuTable.erase(menuIterator);
					}
				}
				m_mapMenuTable[atoi(pszId)] = pTempObj;
				xml_attribute<>* pAttr = pEleSub->first_attribute();
				while (pAttr)
				{
					const char* szName = pAttr->name();
					const char* szValue = pAttr->value();
					pTempObj->SetAttribute(Charset::UTF8ToNative(szName).c_str(), Charset::UTF8ToNative(szValue).c_str());
					pAttr = pAttr->next_attribute();
				}

				if (!pTempObj)
				{
					continue;
				}

				XMLToTemplateObj(pTempObj, pEleSub);
				if (lpszName)
					break;
			}
		}
	}
	if (uResType & _RES_TYPE_IMAGE)
	{
		
		pEle=pRootEle->first_node("Image");
		if (pEle)
		{
			const char* pszId;
			tstring strLanguage;
			const char* pszLanguage;
			const char* pszPath;
			tstring strImageID;
			tstring strImagePath;
			ImagePathObj* pImagePathObj;
			ImagePathObjMap::iterator imageIterator;

			for(pEleSub=pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() :NULL;
				if (!pszId) continue;

				pszLanguage = (attribute = pEleSub->first_attribute("language")) ? attribute->value() :NULL;
				// 语言不对不导入
				strLanguage = pszLanguage ? Charset::UTF8ToNative(pszLanguage) : _T("");
				pszPath = (attribute = pEleSub->first_attribute("path")) ? attribute->value() :NULL;
				if (pszPath)
				{
					strImageID = _T("#") + Charset::UTF8ToNative(pszId);
					strImagePath = Charset::UTF8ToNative(pszPath);
				
					pImagePathObj = NULL;
					imageIterator = m_mapImagePathObjTable.find(strImageID);
					if (imageIterator != m_mapImagePathObjTable.end())
					{
						pImagePathObj = imageIterator->second;
					}

					if (pImagePathObj)
					{
						pImagePathObj->AddPath(strImagePath.c_str(), strLanguage.c_str());
					}
					else
					{
						pImagePathObj = new ImagePathObj;
						pImagePathObj->AddPath(strImagePath.c_str(), strLanguage.c_str());
						m_mapImagePathObjTable[strImageID] = pImagePathObj;
					}
					this->ReloadImageById(strImageID.c_str());
				}
			}
		}
	}

	if (uResType & _RES_TYPE_FONT)
	{
		pEle=pRootEle->first_node("Font");
		if (pEle)
		{
			tstring strNameTemp;
			tstring strValueTemp;
			const char* pszId;
			tstring strID;
			FontObj* pFontObj;

			for(pEleSub=pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() :NULL;
				if (!pszId) continue;

				strID = Charset::UTF8ToNative(pszId);
				pFontObj = GetUIRes()->GetFont(strID.c_str());

				// 如果有已经存在判断优先级
				bool bExist = true;
				if (!pFontObj)
				{
					pFontObj = new FontObj;
					pFontObj->SetId(strID.c_str());
					bExist = false;
				}

				xml_attribute<>* pAttr = pEleSub->first_attribute();
				while (pAttr)
				{
					StringHelper::Utf8ToNativeStrPtr(strNameTemp, pAttr->name());
					StringHelper::Utf8ToNativeStrPtr(strValueTemp, pAttr->value());
					pFontObj->SetAttribute(strNameTemp.c_str(), strValueTemp.c_str());

					pAttr = pAttr->next_attribute();
				}

				if (!bExist)
				{
					GetUIRes()->AddFont(pFontObj);
				}
				else
				{
					pFontObj->RebuildFont();
				}

				if (_tcsicmp(pFontObj->GetId(), _T("defaultfont")) == 0)
				{
					if (GetDefaultFont())
					{
						LOGFONT lf;
						pFontObj->GetLogFont(&lf);
						GetDefaultFont()->SetLogFont(&lf);
					}
					GetUIRes()->SetDefaultFontName(pFontObj->GetFaceName());
				}
			}
		}
	}

	if (uResType & _RES_TYPE_STRING)
	{
		pEle=pRootEle->first_node("String");
		if (pEle)
		{
			LPCTSTR lpNewText;
			tstring strIdTemp;
			StringHolder strHolder;
			const char* pszId;
			const char* pszText;

			for(pEleSub=pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() :NULL;
				pszText = pEleSub->value();
				if (!pszId || !pszText)
					continue;

				StringHelper::Utf8ToNativeStrPtr(strIdTemp, pszId);
				lpNewText = NULL;
				StringHelper::Utf8ToNativePtrPtr((LPTSTR&)lpNewText, pszText);

				if (lpNewText)
				{
					strHolder.SetPtr(lpNewText);
					m_mapStringTable[strIdTemp] = strHolder;
				}
			}
		}
	}

	if (uResType & _RES_TYPE_COLOR)
	{
		pEle=pRootEle->first_node("Color");
		if (pEle)
		{
			tstring strNameTemp;
			tstring strValueTemp;
			const char* pszId;
			tstring strID;
			xml_attribute<>* pAttr;

			for(pEleSub=pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() :NULL;
				if (!pszId)
					continue;

				strID = Charset::UTF8ToNative(pszId);
				ColorObj* pColorObj = GetColorObj(strID.c_str());
				if (!pColorObj)
				{
					pColorObj = new ColorObj;
					pColorObj->SetId(strID.c_str());
					AddColorObj(pColorObj);
				}

			
				pAttr = pEleSub->first_attribute();

				while (pAttr)
				{
					StringHelper::Utf8ToNativeStrPtr(strNameTemp, pAttr->name());
					StringHelper::Utf8ToNativeStrPtr(strValueTemp, pAttr->value());

					pColorObj->SetAttribute(strNameTemp.c_str(), strValueTemp.c_str());
					pAttr = pAttr->next_attribute();
				}

				if (_tcsicmp(pColorObj->GetId(), _T("defaultcolor")) == 0)
				{
					GetDefaultColor()->SetColor(pColorObj->GetColor());
				}
			}
		}
	}

	if (uResType & _RES_TYPE_CURVE)
	{
		pEle=pRootEle->first_node("Curve");
		if (pEle)
		{
			const char* pszId;
			tstring strID;
			CurveObj* pCurveObj;
			const char* szName;
			const char* szValue;
			xml_attribute<>* pAttr;
			xml_node<>* pEleCtl;
			const char* szTp;
			const char* szSp;

			for(pEleSub=pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() :NULL;
				if (!pszId)
					continue;
				strID = Charset::UTF8ToNative(pszId);
				pCurveObj = GetCurveObj(strID.c_str());
				if (!pCurveObj)
				{
					pCurveObj = new CurveObj;
					pCurveObj->SetId(strID.c_str());
					AddCurveObj(pCurveObj);
				}
				pCurveObj->Reset();
				pAttr = pEleSub->first_attribute();
				while (pAttr)
				{
					szName = pAttr->name();
					szValue = pAttr->value();
					pCurveObj->SetAttribute(Charset::UTF8ToNative(szName).c_str(), Charset::UTF8ToNative(szValue).c_str());
					pAttr = pAttr->next_attribute();
				}

				pEleCtl = pEleSub->first_node();
				while (pEleCtl)
				{
					szTp = (attribute = pEleCtl->first_attribute("tp")) ? attribute->value() :NULL; 
					szSp = (attribute = pEleCtl->first_attribute("sp")) ? attribute->value() :NULL;
					if (!!szTp && !!szSp)
						pCurveObj->AddPoint((float)atof(szTp), (float)atof(szSp));
					pEleCtl = pEleCtl->next_sibling();
				}
			}
		}
	}

	if (uResType & _RES_TYPE_GLOBAL)
	{
		pEle=pRootEle->first_node("Global");
		if (pEle)
		{
			for(pEleSub=pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				const char* pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() :NULL;
				if (!pszId) continue;

				const char* pszValue = (attribute = pEleSub->first_attribute("value")) ? attribute->value() :NULL;
				if (!pszValue) continue;
				SetAttribute(Charset::UTF8ToNative(pszId).c_str(), Charset::UTF8ToNative(pszValue).c_str());	
			}
		}
	}

	if (uResType & _RES_TYPE_USER)
	{
		pEle = pRootEle->first_node("User");
		if (pEle)
		{
			for (pEleSub = pEle->first_node(); pEleSub; pEleSub = pEleSub->next_sibling())
			{
				const char* pszId = (attribute = pEleSub->first_attribute("id")) ? attribute->value() : NULL;
				if (!pszId) continue;

				const char* pszValue = (attribute = pEleSub->first_attribute("value")) ? attribute->value() : NULL;
				if (!pszValue) 
					continue;

				m_mapUserDataTable[Charset::UTF8ToNative(pszId)] = Charset::UTF8ToNative(pszValue);
			}
		}
	}


	return true;
}

void ResObj::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_tstrcmp(lpszName, _T("vscrollbar")))
	{
		  LPTSTR pstr = NULL;
		  m_rcVScrollPositon.left =  _tcstol(lpszValue, &pstr, 10); 
		  m_rcVScrollPositon.top =  _tcstol(pstr + 1, &pstr, 10); 
		  m_rcVScrollPositon.right =  _tcstol(pstr + 1, &pstr, 10); 
		  m_rcVScrollPositon.bottom =  _tcstol(pstr + 1, &pstr, 10); 
	}
	else if (equal_tstrcmp(lpszName, _T("hscrollbar")))
	{
		LPTSTR pstr = NULL;
		m_rcHScrollPositon.left =  _tcstol(lpszValue, &pstr, 10); 
		m_rcHScrollPositon.top =  _tcstol(pstr + 1, &pstr, 10); 
		m_rcHScrollPositon.right =  _tcstol(pstr + 1, &pstr, 10); 
		m_rcHScrollPositon.bottom =  _tcstol(pstr + 1, &pstr, 10); 
	}
	else if (equal_tstrcmp(lpszName, _T("shadow")))
	{
		LPTSTR pstr = NULL;
		m_rcWindowShadowEdge.left =  _tcstol(lpszValue, &pstr, 10); 
		m_rcWindowShadowEdge.top =  _tcstol(pstr + 1, &pstr, 10); 
		m_rcWindowShadowEdge.right =  _tcstol(pstr + 1, &pstr, 10); 
		m_rcWindowShadowEdge.bottom =  _tcstol(pstr + 1, &pstr, 10); 
	}
}

bool ResObj::LoadSkin(LPCTSTR lpszFile, LPCTSTR lpszName, UINT uResType)
{
	tstring strFilePath = lpszFile;
	// 判断是全路径
	int i = strFilePath.find(_T(':'));
	if (i > 0)
	{
		rapidxml::file<char> file;
		if (!file.open(strFilePath.c_str()))
			return false;
		xml_document<> document;
		if (!file.parse(document))
			return false;
		LoadSkin(&document, lpszName, uResType);
		return true;
	
	}
	
	tstring strModuleName;
	i = strFilePath.find(_T('#'));
	if (i > 0) 
	{
		strModuleName = strFilePath.substr(0, i);
		strFilePath = strFilePath.substr(i + 1);
	}

	UITYPE_RES eResType = UIRES_FROM_RESOURCE;
	ModuleObj* pModule = GetModuleObj(strModuleName.c_str());
	if (pModule)
		eResType = pModule->GetResType();

	xml_document<> document;
	rapidxml::file<char> file;
	// 从目录中加载
	if (eResType == UIRES_FROM_FILE && pModule)
	{
		strFilePath = pModule->GetModulePath() + strFilePath;
		if (!file.open(strFilePath.c_str())) return false;
		if (!file.parse(document)) return false;
	}
	else if (eResType == UIRES_FROM_RDB && pModule) // 从rdb中加载
	{
		CRdbBuf rdbbuf;
		CRdbParser* pRdbFile = pModule->GetRdbFile();
		if (!pRdbFile)
			return false;

		pRdbFile->ReadFile(&rdbbuf, strFilePath.c_str());
		if (rdbbuf.GetSize() == 0)
			return false;

		if (!file.open(rdbbuf.GetBuf(), rdbbuf.GetSize())) return false;
		if (!file.parse(document)) return false;
	}
	else if (eResType == UIRES_FROM_RESOURCE) // 从资源中加载
	{
		HRSRC hRsrc = ::FindResource(GetEngineObj()->GetInstanceHandle(), MAKEINTRESOURCE( _ttoi(strFilePath.c_str())), strModuleName.c_str());
		if (hRsrc)
		{
			HGLOBAL hImgData = ::LoadResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
			if (hImgData)
			{
				LPVOID lpVoid = ::LockResource(hImgData);
				DWORD dwSize = ::SizeofResource(GetEngineObj()->GetInstanceHandle(), hRsrc);

				if (!file.open((char*)lpVoid, dwSize))
				{
					::FreeResource(hImgData);
					return false;
				}
				::FreeResource(hImgData);
			}
		}
		if (!file.parse(document)) return false;
	}

	LoadSkin(&document, lpszName, uResType);
	return true;
}


void ResObj::InitResDir(LPCTSTR lpszPath,  bool bDefaultDir)
{
	TCHAR szModuleName[100];
	tstring strComponentIniPath = lpszPath;
	strComponentIniPath += _T("component.ini");
	::GetPrivateProfileString(_T("component_name"), _T("name"), _T(""), szModuleName, 100, strComponentIniPath.c_str());

	ModuleObj* pModule = new ModuleObj(szModuleName, lpszPath, UIRES_FROM_FILE);
	m_mapModuleObjTable[szModuleName] = pModule;

	if (bDefaultDir)
		m_pDefModulePathObj = pModule;

	tstring strFileName = lpszPath;
	strFileName += _T("load.cfg");

	rapidxml::file<char> file;
	if (!file.open(strFileName.c_str())) return;
	xml_document<> document;
	if (!file.parse(document)) return;
	InitResDir(&document);
}


void ResObj::InitResDirFromRdb(LPCTSTR lpszPath, LPCTSTR lpszName, bool bDefaultDir)
{
	ModuleObj* pModule = new ModuleObj(lpszName, lpszPath, UIRES_FROM_RDB);
	m_mapModuleObjTable[lpszName] = pModule;

	if (bDefaultDir)
		m_pDefModulePathObj = pModule;

	CRdbParser * pRdbFile = pModule->GetRdbFile();
	if (!pRdbFile) return;

	CRdbBuf rdbbuf;
	pRdbFile->ReadFile(&rdbbuf, _T("load.cfg"));
	if (rdbbuf.GetSize() == 0) return;
	
	rapidxml::file<char> file;
	if (!file.open(rdbbuf.GetBuf(), rdbbuf.GetSize())) return;
	xml_document<> document;
	if (!file.parse(document)) return;
	
	InitResDir(&document);
}

void ResObj::InitResDirFromRdb(LPBYTE lpData, DWORD dwSize, LPCTSTR lpszName, bool bDefaultDir)
{
	ModuleObj* pModule = new ModuleObj(lpszName, lpData, dwSize);
	m_mapModuleObjTable[lpszName] = pModule;

	if (bDefaultDir)
		m_pDefModulePathObj = pModule;

	CRdbParser * pRdbFile = pModule->GetRdbFile();
	if (!pRdbFile) return;

	CRdbBuf rdbbuf;
	pRdbFile->ReadFile(&rdbbuf, _T("load.cfg"));
	if (rdbbuf.GetSize() == 0) return;

	rapidxml::file<char> file;
	if (!file.open(rdbbuf.GetBuf(), rdbbuf.GetSize())) return;
	xml_document<> document;
	if (!file.parse(document)) return;

	InitResDir(&document);
}

void ResObj::InitResDirFromRes(UINT uResID, LPCTSTR lpszType)
{
	if (lpszType == NULL)
		return;

	ms_bLoadFromResource = true;

	TCHAR szPath[100];
	_stprintf(szPath, _T("%s#%u"), lpszType, uResID);
	TCHAR szModuleName[100];
	_stprintf(szModuleName, _T("%u"), uResID);
	ModuleObj* pModule = new ModuleObj(szModuleName, szPath, UIRES_FROM_RESOURCE);
	m_mapModuleObjTable[szModuleName] = pModule;
	m_pDefModulePathObj = pModule;

	rapidxml::file<char> file;
	HRSRC hRsrc = ::FindResource(GetEngineObj()->GetInstanceHandle(), MAKEINTRESOURCE(uResID), lpszType);
	if (hRsrc)
	{
		HGLOBAL hImgData = ::LoadResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
		if (hImgData)
		{
			LPVOID lpVoid = ::LockResource(hImgData);
			DWORD dwSize = ::SizeofResource(GetEngineObj()->GetInstanceHandle(), hRsrc);

			if (!file.open((char*)lpVoid, dwSize))
			{
				::FreeResource(hImgData);
				return;
			}
			::FreeResource(hImgData);
		}
	}
	else return;

	xml_document<> document;
	if (!file.parse(document)) return;
	InitResDir(&document);

}

void ResObj::InitResDir(xml_document<>* pXmlDoc)
{
	xml_node<>* pRootEle;
	pRootEle = pXmlDoc->first_node();
	if(!pRootEle) return;

	xml_node<>* pEle = pRootEle->first_node("nametable");
	string strLanguage = Charset::NativeToUTF8(m_strLanguage.c_str());
	xml_attribute<>* attribute;
	if (pEle)
	{
		tstring strTagTemp;
		LPTSTR lptcsPath;
		StringHolder strHolder;
		for(xml_node<>* pEleSub= pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
		{
			const char* pszTag = pEleSub->name();
			const char* pszPath = (attribute = pEleSub->first_attribute("path")) ? attribute->value() :NULL;
			if (!pszPath) continue;
			const char* pszLanguage = (attribute = pEleSub->first_attribute("language")) ? attribute->value() :NULL;
	
			// 语言不对不导入
			if (!strLanguage.empty() && pszLanguage)
			{
				string  strLanguageTmp = pszLanguage;
				if (string::npos == strLanguageTmp.find(strLanguage.c_str()))
					continue;
			}

			StringHelper::Utf8ToNativeStrPtr(strTagTemp, pszTag); 
			StringHelper::Utf8ToNativePtrPtr(lptcsPath, pszPath);
			if (lptcsPath)
			{
				strHolder.AttachPtr(lptcsPath);
				m_mapNameTable[strTagTemp] = strHolder;
			}
			
		}
	}


	pEle = pRootEle->first_node("loadfile");
	if (pEle)
	{
		for(xml_node<>* pEleSub= pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
		{
			const char* pszPath = (attribute = pEleSub->first_attribute("path")) ? attribute->value() :NULL;
			if (!pszPath) continue;
			const char* pszType = (attribute = pEleSub->first_attribute("type")) ? attribute->value() :NULL;

			if (pszType != NULL) // 普通的资源先导入，不然字体可能有问题
				continue;
			const char* pszLanguage = (attribute = pEleSub->first_attribute("language")) ? attribute->value() :NULL;
			// 语言不对不导入
			if (pszLanguage)
			{
				if (!strLanguage.empty())
				{
					string  strLanguageTmp = pszLanguage;
					if (string::npos == strLanguageTmp.find(strLanguage.c_str()))
						continue;
				}
				else
				{
					continue;
				}
			}
			LoadSkin(Charset::UTF8ToNative(pszPath).c_str(), NULL, GetResTypeFromStr(pszType));
		}
	}

	pEle = pRootEle->first_node("loadfile");
	if (pEle)
	{
		for (xml_node<>* pEleSub = pEle->first_node(); pEleSub; pEleSub = pEleSub->next_sibling())
		{
			const char* pszPath = (attribute = pEleSub->first_attribute("path")) ? attribute->value() : NULL;
			if (!pszPath) continue;
			const char* pszType = (attribute = pEleSub->first_attribute("type")) ? attribute->value() : NULL;
			const char* pszLanguage = (attribute = pEleSub->first_attribute("language")) ? attribute->value() : NULL;
			// 语言不对不导入
			if (pszType == NULL)
				continue;
			if (pszLanguage)
			{
				if (!strLanguage.empty())
				{
					string  strLanguageTmp = pszLanguage;
					if (string::npos == strLanguageTmp.find(strLanguage.c_str()))
						continue;
				}
				else
				{
					continue;
				}
			}
			LoadSkin(Charset::UTF8ToNative(pszPath).c_str(), NULL, GetResTypeFromStr(pszType));
		}
	}
}


void ResObj::SetThemePath(LPCTSTR lpszPath)
{
	m_strThemePath = _T("");
	if (m_pThemeRdbFile)
	{
		delete m_pThemeRdbFile;
		m_pThemeRdbFile = NULL;
	}
	if (PathIsDirectory(lpszPath) && PathFileExists(lpszPath))
	{
		m_strThemePath = lpszPath;
	}

	ReLoadImage();
}

void ResObj::SetThemePathFromRDB(LPCTSTR lpszPath, bool bCoexistence /*= false*/)
{
	if (!bCoexistence)
	{
		m_strThemePath = _T("");
	}
	
	if (m_pThemeRdbFile)
	{
		delete m_pThemeRdbFile;
		m_pThemeRdbFile = NULL;
	}
	if (!PathIsDirectory(lpszPath) && PathFileExists(lpszPath))
	{
		if (!bCoexistence)
		{
			m_strThemePath = lpszPath;
		}

		m_pThemeRdbFile = new CRdbParser();
		m_pThemeRdbFile->SetCoexistence(bCoexistence);
		m_pThemeRdbFile->Open(lpszPath);
	}

	ReLoadImage();
}

LPCTSTR ResObj::GetThemePath()
{
	return m_strThemePath.c_str();
}

CRdbParser*  ResObj::GetThemeRdbFile()
{
	return m_pThemeRdbFile;
}

void ResObj::SetHDPIImagePath(LPCTSTR lpszPath)
{
	m_strHDPIImagePath = lpszPath;
	ReLoadImage();
}

LPCTSTR ResObj::GetHDPIImagePath()
{
	return m_strHDPIImagePath.c_str();
}

void ResObj::SetHDPIImagePathRDB(LPCTSTR lpszPath)
{
	m_strHDPIImagePath = lpszPath;
	if (m_pHDPIRdbFile)
	{
		delete m_pHDPIRdbFile;
		m_pHDPIRdbFile = NULL;
	}
	if (!PathIsDirectory(lpszPath) && PathFileExists(lpszPath))
	{
		m_pHDPIRdbFile = new CRdbParser();
		m_pHDPIRdbFile->Open(lpszPath);
	}
}
CRdbParser* ResObj::GetHDPIImageRdbFile()
{
	return m_pHDPIRdbFile;
}

void ResObj::SetHDPI9ImageParamScale(float fScale)
{
	m_fHDPIImageNinePatchScale = fScale;
}

void ResObj::SetHDPIAutoStretchOnDrawing9Image(bool bAutoStretch)
{
	m_bHDPIAutoStretchOnDrawing9Image = bAutoStretch;
}

bool ResObj::GetHDPIAutoStretchOnDrawing9Image()
{
	return m_bHDPIAutoStretchOnDrawing9Image;
}

float ResObj::GetHDPI9ImageParamScale()
{
	return m_fHDPIImageNinePatchScale;
}

ModuleObj* ResObj::GetModuleObj(LPCTSTR lpszModuleName)
{
	if (!lpszModuleName || _tcslen(lpszModuleName) == 0)
		return m_pDefModulePathObj;
	std::map<tstring, ModuleObj*>::iterator it = m_mapModuleObjTable.find(lpszModuleName);
	if (it != m_mapModuleObjTable.end())
		return it->second;
	return  NULL;
}

LPCTSTR ResObj::GetI18NStr(LPCTSTR lpszId, bool* bFind)
{
	if (lpszId[0] == _T('#'))
		lpszId = lpszId + 1;

	I18NStringMap::iterator it = m_mapStringTable.find(lpszId);
	if (it != m_mapStringTable.end())
	{
		if (bFind) *bFind = true;
		return it->second.GetPtr();
	}

	if (bFind) *bFind = false;
	return lpszId;
}
bool ResObj::AttachMenu(CControlUI* pControl, UINT uMenuID)
{
	if (uMenuID == 0)
		return false;

	std::map<UINT, TemplateObj*>::iterator it = m_mapMenuTable.find(uMenuID);
	if (it != m_mapMenuTable.end())
	{
		TemplateObj* pTempObj = it->second;

		MAPAttr* mapAttr = pTempObj->GetAttr();
		if (mapAttr)
		{
			for (MAPAttr::iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
				pControl->SetAttribute(pos->first.c_str(), pos->second.c_str());
		}
		TemplateObjToControl(pControl, pTempObj);
		return true;
	}
	return false;
}
UIMENUINFO ResObj::GetMenu( UINT uMenuID, int nIndex /*= -1*/, UINT uFlag /*= BY_POSITION*/ )
{
	UIMENUINFO menuinfo;
	TemplateObj* pObj = NULL;
	TemplateObj* pResult = NULL;
	LPCTSTR pValue = NULL;
	if (uFlag == BY_COMMAND)
	{
		pObj = m_mapMenuTable[uMenuID];
		if (pObj != NULL)
		{
			int nCnt = pObj->NumChild();
			TemplateObj* pChild = NULL;
			for (int i = 0; i < nCnt; i++)
			{
				pChild = pObj->GetChild(i);
				pValue = pChild->GetAttr(_T("UID"));
				if (pValue)
				{
					if (_ttoi(pValue) == nIndex)
					{
						pResult = pChild;
						break;
					}
					else
					{
						UIMENUINFO ret = GetMenu(_ttoi(pValue), nIndex, uFlag);
						if (ret.uID != 0)
							return ret;
					}
				}
			}
		}
	}
	else if (uFlag == BY_POSITION)
	{
		if (nIndex < 0)
			pResult = m_mapMenuTable[uMenuID];
		else
			pResult = m_mapMenuTable[uMenuID]->GetChild(nIndex);
	}
	if (pResult)
	{
		pValue = pResult->GetAttr(_T("separator"));
		if (pValue) menuinfo.bSeparator = (bool)!!_ttoi(pValue);

		pValue = pResult->GetAttr(_T("UID"));
		if (pValue) menuinfo.uID = _ttoi(pValue);

		pValue = pResult->GetAttr(_T("check"));
		if (pValue) menuinfo.bCheck = (bool)!!_ttoi(pValue);

		pValue = pResult->GetAttr(_T("Image"));
		if (pValue) menuinfo.szIconFile = (pValue);

		pValue = I18NSTR(pResult->GetAttr(_T("text")));
		if (pValue)
		{
			if (equal_icmp(pValue, _T("-")))
				menuinfo.bSeparator = (bool)!!_ttoi(pValue);
			menuinfo.szText = (pValue);
		}
	}
	return menuinfo;
}

TemplateObj* ResObj::GetWindowTemplateObj(LPCTSTR lpszId)
{
	if (lpszId == NULL)
		return NULL;

	TemplateIterator it = m_mapWindowTable.find(lpszId);
	if (it != m_mapWindowTable.end())
		return static_cast<TemplateObj*>(it->second);

	NameTableMap::iterator it1 = m_mapNameTable.find(_T("W.") + tstring(lpszId));
	if (it1 == m_mapNameTable.end())
		return NULL;
	LoadSkin(it1->second.GetPtr(), lpszId, _RES_TYPE_WINDOW);
	it = m_mapWindowTable.find(lpszId);
	if (it != m_mapWindowTable.end())
		return static_cast<TemplateObj*>(it->second);
	return NULL;

}

void ResObj::DelWindowTemplateObj(LPCTSTR lpszId)
{
	TemplateObj* pTemplateObj = NULL;
	TemplateIterator it = m_mapWindowTable.find(lpszId);
	if (it != m_mapWindowTable.end())
		pTemplateObj = static_cast<TemplateObj*>(it->second);
	if (pTemplateObj)
	{
		delete pTemplateObj;
		pTemplateObj = NULL;
		m_mapWindowTable.erase(it);
	}
}


const MAPAttr* ResObj::GetTemplateObjAttr(LPCTSTR lpszTemplateID)
{
	if (lpszTemplateID != NULL)
	{
		
		TemplateIterator it;
		if (!GetCacheXML())
		{
			NameTableMap::iterator it1 = m_mapNameTable.find(_T("S.") + tstring(lpszTemplateID));
			if (it1 == m_mapNameTable.end())
				return NULL;

			LoadSkin(it1->second.GetPtr(), lpszTemplateID, _RES_TYPE_STYLE);
			it = m_mapStyleTable.find(lpszTemplateID);
			if (it == m_mapStyleTable.end())
				return NULL;
		}
		else
		{
			it = m_mapStyleTable.find(lpszTemplateID);
			if (it == m_mapStyleTable.end())
			{
				NameTableMap::iterator it1 = m_mapNameTable.find(_T("S.") + tstring(lpszTemplateID));
				if (it1 == m_mapNameTable.end())
					return NULL;

				LoadSkin(it1->second.GetPtr(), lpszTemplateID, _RES_TYPE_STYLE);
				it = m_mapStyleTable.find(lpszTemplateID);
				if (it == m_mapStyleTable.end())
					return NULL;
			}
		}
		
		TemplateObj* pTempObj = dynamic_cast<TemplateObj*>(it->second);
		if (pTempObj)
			return pTempObj->GetAttr();
	}
	return NULL;
}

void ResObj::DelStyleTemplateObj(LPCTSTR lpszId)
{
	TemplateObj* pTemplateObj = NULL;
	TemplateIterator it = m_mapStyleTable.find(lpszId);
	if (it != m_mapStyleTable.end())
		pTemplateObj = static_cast<TemplateObj*>(it->second);
	if (pTemplateObj)
	{
		delete pTemplateObj;
		pTemplateObj = NULL;
		m_mapStyleTable.erase(it);
	}
}


StyleObj* CreateStyle(LPCTSTR lpszName, CControlUI* pOwner)
{
	return  GetEngineObj()->CreateStyle(lpszName, pOwner);
}

StyleObj* CreateStyle(StyleObj* pStyleObj, CControlUI* pOwner)
{
	if (!pStyleObj)
		return nullptr;
	StyleObj* pStyleObjNew = pStyleObj->CreateObject(pOwner);
	if (pStyleObjNew)
		pStyleObjNew->Copy(pStyleObj);
	return pStyleObjNew;
}


bool ResObj::AddSytleList(CControlUI* pCtl, CStdPtrArray** ppStyleList, LPCTSTR lpszId)
{
	if (lpszId == NULL || lpszId[0] == _T('0'))
		return false;

#ifdef _DEBUG
	NameTableMap::iterator it1 = m_mapNameTable.find(_T("S.") + tstring(lpszId));
	if (it1 == m_mapNameTable.end())
		return false;

	LoadSkin(it1->second.GetPtr(), lpszId,  _RES_TYPE_STYLE);
	TemplateIterator it = m_mapStyleTable.find(lpszId);
	if (it == m_mapStyleTable.end())
		return false;
#else
	TemplateIterator it = m_mapStyleTable.find(lpszId);
	if (it == m_mapStyleTable.end())
	{
		NameTableMap::iterator it1 = m_mapNameTable.find(_T("S.") + tstring(lpszId));
		if (it1 == m_mapNameTable.end())
			return false;

		LoadSkin(it1->second.GetPtr(), lpszId, _RES_TYPE_STYLE);
		it = m_mapStyleTable.find(lpszId);
		if (it == m_mapStyleTable.end())
			return false;
	}

#endif
	
	TemplateObj* pTempObj = static_cast<TemplateObj*>(it->second);
	if (*ppStyleList == NULL) *ppStyleList = new CStdPtrArray(pTempObj->NumChild());

	//userdata 存放缓存列表，如果已经存在就直接从存在设置，省得重新调用SetAttribute
	if (pTempObj->GetCachedStyle())
	{
		CStdPtrArray* pStyleListBack = pTempObj->GetCachedStyle();
		for (int i = 0; i < pStyleListBack->GetSize(); i++)
		{
			StyleObj* pStyleBack = (StyleObj*)pStyleListBack->GetAt(i);
			StyleObj* pStyleObj = CreateStyle(pStyleBack, pCtl);
			if (pStyleObj) (*ppStyleList)->Add(pStyleObj);
		}
	}
	else
	{
		CStdPtrArray* pStyleListBack = NULL;

		//把样式缓存起来加快速度
		if (pTempObj->IsStore() && !pTempObj->GetCachedStyle())
		{
			pStyleListBack = new CStdPtrArray(pTempObj->NumChild());
			pTempObj->CacheStyle(pStyleListBack);
		}

		for (int i = 0; i < pTempObj->NumChild(); i++)
		{
			TemplateObj* pChildTempObj = pTempObj->GetChild(i);
			if (pChildTempObj)
			{
				StyleObj* pStyleObj = CreateStyle(pChildTempObj->GetType(), pCtl);
				if (pStyleObj == NULL)
					continue;
				MAPAttr* mapAttr = pChildTempObj->GetAttr();
				if (mapAttr)
				{
					for (MAPAttr::iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
					{
						pStyleObj->SetAttribute(pos->first.c_str(), pos->second.c_str());
					}
				}
				(*ppStyleList)->Add(pStyleObj);
				if (pStyleListBack)
				{
					StyleObj* pStyleObjBack = CreateStyle(pStyleObj, NULL);
					if (pStyleObjBack) pStyleListBack->Add(pStyleObjBack);
				}
			}
		}
		if (!pTempObj->IsStore())
		{
			delete pTempObj;
			m_mapStyleTable.erase(it);
		}
	}
	return true;
}


bool  ResObj::AddControl(CControlUI* pParent, CControlUI* pNext, LPCTSTR lpszId)
{
	if(!lpszId)return false;

	TemplateIterator it;
	if (!GetCacheXML())
	{
		NameTableMap::iterator it1 = m_mapNameTable.find(_T("S.") + tstring(lpszId));
		if (it1 == m_mapNameTable.end())
			return false;

		LoadSkin(it1->second.GetPtr(), lpszId, _RES_TYPE_STYLE);
		it = m_mapStyleTable.find(lpszId);
		if (it == m_mapStyleTable.end())
			return false;
	}
	else
	{
		it = m_mapStyleTable.find(lpszId);
		if (it == m_mapStyleTable.end())
		{
			NameTableMap::iterator it1 = m_mapNameTable.find(_T("S.") + tstring(lpszId));
			if (it1 == m_mapNameTable.end())
				return false;

			LoadSkin(it1->second.GetPtr(), lpszId, _RES_TYPE_STYLE);
			it = m_mapStyleTable.find(lpszId);
			if (it == m_mapStyleTable.end())
				return false;
		}

	}

	TemplateObj* pTemplateObj = static_cast<TemplateObj*>(it->second);
	MAPAttr* mapAttr = pTemplateObj->GetAttr();
	if (mapAttr)
	{
		for (MAPAttr::iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
		{
			if (!equal_icmp(pos->first.c_str(), _T("id")))
				pParent->SetAttribute(pos->first.c_str(), pos->second.c_str());
		}
	}

	TemplateObjToControl(pParent, pTemplateObj, pNext);
	if (!pTemplateObj->IsStore())
	{
		//m_mapStyleTable.erase(it);
		DelStyleTemplateObj(lpszId);
	}
	return true;
}

LPCTSTR ResObj::GetImageFileFullPath(LPCTSTR lpszPath)
{
	if (!lpszPath || _tcslen(lpszPath) == 0)
		return _T("");

	tstring strPath = lpszPath;
	if (lpszPath[0] == _T('#'))
		strPath = GetImagePathFromId(lpszPath);

	bool bUpdateColor = false;
	SIZE szCellNum = {1,1};
	short arrPart[4] = {0};
	ModuleObj* pModule = PraseImagePath(strPath, strPath, bUpdateColor, szCellNum, arrPart);
	if (pModule)
	{
		if (pModule->GetResType() == UIRES_FROM_FILE)
			strPath = pModule->GetModulePath()  + strPath;
	}
	m_strImageFileFullPath = strPath;
	return m_strImageFileFullPath.c_str();
}

void ResObj::AddFont(FontObj* pFontObj)
{
	pFontObj->SetCached(true);
	m_mapFontsTable[pFontObj->GetId()] = pFontObj;
}

FontObj* ResObj::GetFont(LPCTSTR lpszId)
{
	if (lpszId)
	{
		map<tstring, FontObj*>::iterator itFonts = m_mapFontsTable.find(lpszId);
		if (itFonts != m_mapFontsTable.end())
			return itFonts->second;
	}
	return NULL;
}

FontObj* ResObj::GetDefaultFont()
{
	return ms_pDefaultFontObj;
}


void ResObj::ReBulidFont()
{
	if (GetDefaultFont())
		GetDefaultFont()->RebuildFont();
	for (map<tstring, FontObj*>::iterator itFonts = m_mapFontsTable.begin(); itFonts != m_mapFontsTable.end(); ++itFonts)
	{
		FontObj* fontObj = itFonts->second;
		fontObj->RebuildFont();
	}
		
}

void  ResObj::AddCursor(CursorObj* pCursorObj)
{
	m_mapCursorsTable[pCursorObj->GetId()] = pCursorObj;
}

CursorObj* ResObj::GetCursor(LPCTSTR lpszId)
{
	if (lpszId)
	{
// 		map<tstring, CursorObj*>::iterator itCursors = m_mapCursorsTable.find(lpszId);
// 		if (itCursors != m_mapCursorsTable.end())
// 			return itCursors->second;
		for (map<tstring, CursorObj*>::iterator itCursors = m_mapCursorsTable.begin(); itCursors != m_mapCursorsTable.end(); ++itCursors)
		{
			if (equal_icmp(itCursors->first.c_str(), lpszId))
			{
				return itCursors->second;
			}
		}
	}
	return ms_pDefaultCursorObj;
}

CursorObj* ResObj::GetDefaultCursor()
{
	return ms_pDefaultCursorObj;
}

void ResObj::SetCacheXML(bool bCacheXML)
{
	m_bCacheXML = bCacheXML;
}

bool ResObj::GetCacheXML()
{
	return m_bCacheXML;
}

LPCTSTR ResObj::GetLanguage()
{
	return m_strLanguage.c_str();
}

void ResObj::SetLanguage(LPCTSTR lpszLanguage)
{
	m_strLanguage = lpszLanguage;
	xml_document<> document;
	
	for(std::map<tstring, ModuleObj*>::iterator moduleIterator=m_mapModuleObjTable.begin(); moduleIterator!=m_mapModuleObjTable.end();++moduleIterator)
	{
		rapidxml::file<char> file;
		ModuleObj* pModule = moduleIterator->second;
		if (pModule->GetResType() == UIRES_FROM_FILE)
		{
			tstring strFileName = pModule->GetModulePath();
			strFileName += _T("load.cfg");
		
			if (!file.open(strFileName.c_str())) continue;
			if (!file.parse(document)) continue;
		}
		else if (pModule->GetResType() == UIRES_FROM_RDB)
		{
			CRdbBuf rdbbuf;
			CRdbParser * pRdbFile = pModule->GetRdbFile();
			if (!pRdbFile) continue;

			pRdbFile->ReadFile(&rdbbuf, _T("load.cfg"));
			if (rdbbuf.GetSize() == 0) return;

			if (!file.open(rdbbuf.GetBuf(), rdbbuf.GetSize())) continue;
			if (!file.parse(document)) continue; 
		}
		else if (pModule->GetResType() == UIRES_FROM_RESOURCE)
		{
			tstring strFileName = pModule->GetModulePath();
			int i = strFileName.find(_T('#'));
			tstring strType = strFileName.substr(0, i);
			UINT uResID = _ttoi(strFileName.substr(i).c_str());

			HRSRC hRsrc = ::FindResource(GetEngineObj()->GetInstanceHandle(), MAKEINTRESOURCE(uResID), strType.c_str());
			if (hRsrc)
			{
				HGLOBAL hImgData = ::LoadResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
				if (hImgData)
				{
					LPVOID lpVoid = ::LockResource(hImgData);
					DWORD dwSize = ::SizeofResource(GetEngineObj()->GetInstanceHandle(), hRsrc);
	
					if (!file.open((char*)lpVoid, dwSize))
					{
						::FreeResource(hImgData);
						continue;
					}
					::FreeResource(hImgData);
					
				}

			}
			if (!file.parse(document)) continue; 
		}	

		string strLanguage = Charset::NativeToUTF8(m_strLanguage.c_str());
		
		xml_node<>* pRootEle = document.first_node();
		if(!pRootEle)
			continue;

		xml_node<>* pEle = pRootEle->first_node("loadfile");
		if (pEle)
		{
			for(xml_node<>* pEleSub= pEle->first_node(); pEleSub; pEleSub=pEleSub->next_sibling())
			{
				xml_attribute<>* attribute;
				const char* pszPath = (attribute = pEleSub->first_attribute("path")) ? attribute->value() :NULL; 
				if (!pszPath) continue;
				const char* pszType = (attribute = pEleSub->first_attribute("type")) ? attribute->value() :NULL; 
				if (!pszType) continue;
				UINT nType = GetResTypeFromStr(pszType);
				if ((nType & _RES_TYPE_STRING) == 0) continue;

				const char* pszLanguage = (attribute = pEleSub->first_attribute("language")) ? attribute->value() :NULL;
				// 语言不对不导入
				if (pszLanguage)
				{
					if (!strLanguage.empty())
					{
						string  strLanguageTmp = pszLanguage;
						if (string::npos == strLanguageTmp.find(strLanguage.c_str()))
							continue;
					}
					else
					{
						continue;
					}
				}
				LoadSkin(Charset::UTF8ToNative(pszPath).c_str(), NULL, _RES_TYPE_STRING|_RES_TYPE_FONT);
			}
		}
	};

	int nCount = GetEngineObj()->GetWindowCount();
	for (int i = 0; i < nCount; i++)
	{
		CWindowUI* pWindow = GetEngineObj()->GetWindow(i);
		pWindow->OnLanguageChange();

		CControlUI* pRoot = pWindow->GetRoot();
		if (pRoot)
		{
			pRoot->OnLanguageChange();
			pRoot->Resize();
		}

		if (IsWindowVisible(pWindow->GetHWND()))
		{
			pWindow->Invalidate();
		}
			
	}
	
}

void ResObj::SetImageLanguage(LPCTSTR lpszLanguage)
{
	if (equal_icmp(m_strImageLanguage.c_str(), lpszLanguage)) return;
	m_strImageLanguage = lpszLanguage;

	for (std::map<tstring, ImageObj*>::iterator pos = m_mapImageObjTable.begin(); pos != m_mapImageObjTable.end(); ++pos)
	{
		ImageObj* pImage = pos->second; 

		std::map<tstring, ImagePathObj*>::iterator imageIterator = m_mapImagePathObjTable.find(pImage->GetFilePath());
		if (imageIterator!=m_mapImagePathObjTable.end())
		{
			if (imageIterator->second->IsMutiPath())
				pImage->Reload();
		}
	}

	for (int i = 0; i < GetEngineObj()->GetWindowCount(); i++)
	{
		CWindowUI* pWindow = GetEngineObj()->GetWindow(i);
		if (IsWindowVisible(pWindow->GetHWND()))
			pWindow->Invalidate();
	}
}


int CALLBACK  EnumFontCallBack(CONST LOGFONTW * lplf,  CONST VOID * lpntm, DWORD FontType, LPARAM lpParam)
{
	if (FontType & RASTER_FONTTYPE)
		return 1;
	
	bool* bExits = (bool*)lpParam;
	*bExits = true;
	return 1; // Call me back	
}


void ResObj::SetDefaultFontName(LPCTSTR lpszFontName)
{
	bool bExist = false;
	EnumFonts(hdcGrobal, lpszFontName, (FONTENUMPROC)EnumFontCallBack, (LPARAM)&bExist);
	if (!bExist) return;
	m_strDefaultFontName = lpszFontName;

	if (GetDefaultFont())
		GetDefaultFont()->RebuildFont();

	for (map<tstring, FontObj*>::iterator itFonts = m_mapFontsTable.begin(); itFonts != m_mapFontsTable.end(); ++itFonts)
	{
		FontObj* fontObj = itFonts->second;
		if (fontObj->IsUsedDefaultFaceName())
			fontObj->RebuildFont();
	}
}

LPCTSTR ResObj::GetDefaultFontName()
{
	return m_strDefaultFontName.c_str();
}

void ResObj::SetDefaultFontType(UITYPE_FONT eFontType)
{
	ms_eFontType = eFontType;
}

UITYPE_FONT ResObj::GetDefaultFontType()
{
	return ms_eFontType;
}

void ResObj::SetDefaultTextRenderingHint(int textRenderingHint)
{
	ms_eTextRenderingHintGlobal = (TextRenderingHint)textRenderingHint;
}

int ResObj::GetDefaultTextRenderingHint()
{
	return ms_eTextRenderingHintGlobal;
}

void ResObj::SetDefaultImageType(UITYPE_IMAGE eImageType)
{
	ms_eImageType = eImageType;
}

void ResObj::SetDefaultImageQuality(UITYPE_SMOOTHING eImageQualityType)
{
	ms_eImageQualityType = eImageQualityType;
}

RECT ResObj::GetSystemScrollBarPosition(bool bVert)// 获取滚动条位置
{
	if (!bVert)
		return m_rcHScrollPositon;
	else
		return m_rcVScrollPositon;
}

RECT ResObj::GetWindowShadowEdgeRect()
{
	if (GetHDPIAutoStretchOnDrawing9Image())
		return DPI_SCALE(m_rcWindowShadowEdge);
	return m_rcWindowShadowEdge;
}

void ResObj::ReLoadImage()
{
	std::map<tstring, ImageObj*>::iterator pos;
	for (pos = m_mapImageObjTable.begin(); pos != m_mapImageObjTable.end(); ++pos)
	{
		ImageObj* pImage = pos->second;
		if (pImage)
		{
			pImage->Reload();
		}
		
	}
}

LPCTSTR  ResObj::GetUserData(LPCTSTR lpszId)
{
	map<tstring, tstring>::iterator it = m_mapUserDataTable.find(lpszId);
	if (it != m_mapUserDataTable.end())
		return it->second.c_str();
	return _T("");
}

void  ResObj::SetUserData(LPCTSTR lpszId, LPCTSTR lpszValue)
{
	m_mapUserDataTable[lpszId] = lpszValue;
}

int ResObj::GetModuleCount()
{
	return m_mapModuleObjTable.size();

}

LPCTSTR ResObj::GetModulePath(int index)
{
	if (index < 0 || index >= GetModuleCount())
		return _T("");
	std::map<tstring, ModuleObj*>::iterator it;
	int i = 0;
	for (it = m_mapModuleObjTable.begin(); it != m_mapModuleObjTable.end(); it++)
	{
		if (i == index)
		{
			return it->second->GetModulePath();
		}
		i++;
	}
	return _T("");
}

LPCTSTR ResObj::GetModuleName(int index)
{
	if (index < 0 || index >= GetModuleCount())
		return _T("");
	std::map<tstring, ModuleObj*>::iterator it;
	int i = 0;
	for (it = m_mapModuleObjTable.begin(); it != m_mapModuleObjTable.end(); it++)
	{
		if (i == index)
		{
			return it->second->GetModuleName();
		}
		i++;
	}
	return _T("");
}

IUIRes* GetUIRes()
{
	return GetResObj();
}

LPCTSTR I18NSTR(LPCTSTR lpszStr, bool* bFind)
{
	return GetResObj()->GetI18NStr(lpszStr, bFind);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

ModuleObj::ModuleObj(LPCTSTR lpszModuleName, LPCTSTR lpszModulePath, UITYPE_RES nResType)
{
	m_strModuleName = lpszModuleName;
	m_strModulePath = lpszModulePath;
	m_pRdbFile = NULL;
	m_nResType = nResType;
}

ModuleObj::ModuleObj(LPCTSTR lpszModuleName, LPBYTE lpData, DWORD dwSize)
{
	m_strModuleName = lpszModuleName;
	m_pRdbFile = new CRdbParser;
	if (!m_pRdbFile->Open(lpData, dwSize))
	{
		delete m_pRdbFile;
		m_pRdbFile = NULL;
	}
	m_nResType = UIRES_FROM_RDB;
}

ModuleObj::~ModuleObj()
{
	Close();
}

void ModuleObj::Open()
{
	if (m_pRdbFile && m_pRdbFile->IsOpen())
		return;
	if (!m_pRdbFile) m_pRdbFile = new CRdbParser;
	bool bSucc = !!m_pRdbFile->Open(m_strModulePath.c_str());
	if (!bSucc)
	{
		delete m_pRdbFile;
		m_pRdbFile = NULL;
	}

}

void ModuleObj::Close()
{
	if (m_pRdbFile && m_pRdbFile->IsOpen())
		m_pRdbFile->Close();
	delete m_pRdbFile;
	m_pRdbFile = NULL;
}

CRdbParser*  ModuleObj::GetRdbFile()
{
	Open();
	return m_pRdbFile;
}

////////////////////////////////////////////////////////////////////////////////////////////////

ImagePathObj::ImagePathObj()
{
	m_pMutiPath =  NULL;
}

ImagePathObj::~ImagePathObj()
{
	if (m_pMutiPath) delete m_pMutiPath;

}

void ImagePathObj::AddPath(LPCTSTR lpszPath, LPCTSTR lpszLanguaze)
{
	if (!lpszLanguaze || lpszLanguaze[0] == _T('\0'))
	{		
		m_strPath = lpszPath;
		return;
	}
	if (!m_pMutiPath) m_pMutiPath = new StringMapClass;
	(*m_pMutiPath)[lpszLanguaze] = lpszPath;
}

LPCTSTR ImagePathObj::GetPath(LPCTSTR lpszLanguaze)
{
	if (!lpszLanguaze || !m_pMutiPath || lpszLanguaze[0] == _T('\0'))
	{		
		return m_strPath.c_str();
	}
	StringMapClass::iterator it = m_pMutiPath->find(lpszLanguaze);
	if (it != m_pMutiPath->end()) return it->second.c_str();
	return m_strPath.c_str();
}

bool ImagePathObj::IsMutiPath()
{
	return m_pMutiPath != NULL;
}


void ImagePathObj::SetModuleName(LPCTSTR lpszName)
{
	m_strModuleName = lpszName;
}

LPCTSTR ImagePathObj::GetModuleName()
{
	return m_strModuleName.c_str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PraseHdpiImagePath(ModuleObj* pModule, tstring& strPath)
{
	if (pModule == NULL)
		return false;

	if (_tcslen(GetUIEngine()->GetDPIName()) == 0)
		return false;

	tstring strPathNew = _T("res_") + tstring(GetUIEngine()->GetDPIName()) + _T("\\") + strPath.substr(4);
	if (pModule->GetResType() == UIRES_FROM_FILE)
	{
		tstring strFullPath = pModule->GetModulePath() + strPathNew;
		if (::PathFileExists(strFullPath.c_str()))
		{
			strPath = strPathNew;
			return true;
		}
	}
	else if (pModule->GetResType() == UIRES_FROM_RDB && pModule->GetRdbFile())
	{
		if (pModule->GetRdbFile()->FileExist(strPathNew.c_str()))
		{
			strPath = strPathNew;
			return true;
		}
	}
	return false;
}

bool PraseHdpiImagePathInTheme(ModuleObj* pModule, tstring& strPath)
{
	if (pModule == NULL)
		return false;

	if (_tcslen(GetUIEngine()->GetDPIName()) == 0)
		return false;

	tstring strPathNew = _T("res_") + tstring(GetUIEngine()->GetDPIName()) + _T("\\") + strPath.substr(4);
	if (pModule->GetResType() == UIRES_FROM_FILE)
	{
		tstring strFullPath = GetResObj()->GetThemePath();
		strFullPath = strFullPath + pModule->GetModuleName() + _T("\\") + strPathNew;
		if (::PathFileExists(strFullPath.c_str()))
		{
			strPath = strPathNew;
			return true;
		}
	}
	else if (pModule->GetResType() == UIRES_FROM_RDB && pModule->GetRdbFile())
	{
		tstring strFullPath = pModule->GetModuleName() + tstring(_T("\\")) + strPathNew;
		if (pModule->GetRdbFile()->FileExist(strFullPath.c_str()))
		{
			strPath = strPathNew;
			return true;
		}
	}
	return false;
}

// 解析图片配置路径及参数
ModuleObj* PraseImagePath(tstring strPathIn, tstring& strPathOut, bool& bUpdateColor, SIZE& szCellNum, short* arrPart)
{
	bUpdateColor = false;
	int i =  strPathIn.find_last_of(_T('|'));
	if (i > 0)
	{
		//解析出文件名
		strPathOut = strPathIn.substr(i + 1);
		strPathIn = strPathIn.substr(0, i + 1);

		//解析出是否需要变色
		i =  strPathIn.find(_T("|U"));
		if (i >= 0)
		{
			bUpdateColor = true;
			strPathIn =  strPathIn.substr(i + 2);
		}

		//解析图片的行数和列数
		i =  strPathIn.find(_T("|C"));
		if (i >= 0)
		{
			strPathIn =  strPathIn.substr(i + 2);
			i =  strPathIn.find(_T('|'));
			szCellNum.cy =  _ttoi(strPathIn.substr(0, i).c_str());
			strPathIn = strPathIn.substr(i);
		}
		else
		{
			i =  strPathIn.find(_T("|R"));
			if (i >= 0)
			{
				strPathIn =  strPathIn.substr(i + 2);
				i =  strPathIn.find(_T('|'));
				szCellNum.cx =  _ttoi(strPathIn.substr(0, i).c_str());
				strPathIn = strPathIn.substr(i);
			}
		}

		//解析出图片的分隔
		tstring spliter = _T("");
		i = strPathIn.find(_T("|S"));
		if (i >= 0)
		{
			strPathIn =  strPathIn.substr(i + 2);
			i =  strPathIn.find(_T('|'));
			spliter =  strPathIn.substr(0, i);
			strPathIn = strPathIn.substr(i);

			i = spliter.find_first_of(_T(','));
			short index = 0; 
			while (i >= 0)
			{
				arrPart[index] = _ttoi(spliter.substr(0, i).c_str());
				spliter = spliter.substr(i + 1);
				i = spliter.find_first_of(_T(','));
				index++;
			}
			arrPart[index] = _ttoi(spliter.c_str());
		}

	}
	else strPathOut = strPathIn;

	i = strPathOut.find(_T(':'));
	if (i > 0) return NULL; // 全路径

	// 从资源加载图片的就不用解析了
	if (ms_bLoadFromResource) return GetResObj()->GetModuleObj(NULL);

	// 查找所属模块
	i = strPathOut.find(_T('#'));
	tstring strModuleName;
	if (i > 0)
	{
		strModuleName = strPathOut.substr(0, i);
		strPathOut = strPathOut.substr(i + 1);
	}
	
	ModuleObj* pModule = GetResObj()->GetModuleObj(strModuleName.c_str());
	strPathOut = _T("res\\") + strPathOut;

	return pModule;
	
}

//从文件中读取数据到模板
void XMLToTemplateObj(TemplateObj* pParent, xml_node<>* pRoot)
{
	if (!pRoot) return;
	//读取控件属性
	xml_node<>* pEleCtl = pRoot->first_node();
	while (pEleCtl)
	{
		const char * szTag  = pEleCtl->name();
		TemplateObj* pTemplate = new TemplateObj;
		//if (pTemplate)
		{
			pTemplate->SetType(Charset::UTF8ToNative(szTag).c_str());
			xml_attribute<>* pAttr = pEleCtl->first_attribute();
			while (pAttr)
			{
				const char* szName = pAttr->name();
				const char* szValue = pAttr->value();
				pTemplate->SetAttribute(Charset::UTF8ToNative(szName).c_str(), Charset::UTF8ToNative(szValue).c_str());
				pAttr = pAttr->next_attribute();
			}

			if (!pTemplate)
			{
				continue;
			}

			pParent->Add(pTemplate);

			if (pEleCtl->first_node())
				XMLToTemplateObj(pTemplate, pEleCtl);
		}
		pEleCtl = pEleCtl->next_sibling();
	}
}

void XMLToTemplateObjWithFilter(TemplateObj* pParent, xml_node<>* pRoot, ITemplateFilter* pFilter)
{
	if (!pRoot) return;
	//读取控件属性
	xml_node<>* pEleCtl = pRoot->first_node();
	for (;
		pEleCtl;
		pEleCtl = pEleCtl->next_sibling())
	{
		const char * szTag = pEleCtl->name();
		TemplateObj* pTemplate = new TemplateObj;
		//if (pTemplate)
		{
			pTemplate->SetType(Charset::UTF8ToNative(szTag).c_str());
			xml_attribute<>* pAttr = pEleCtl->first_attribute();
			while (pAttr)
			{
				const char* szName = pAttr->name();
				const char* szValue = pAttr->value();
				if (pFilter)
				{
					if (!pFilter->OnTemplateAttributes(szName, szValue))
					{
						delete pTemplate;
						pTemplate = NULL;
						break;
					}
				}
				pTemplate->SetAttribute(Charset::UTF8ToNative(szName).c_str(), Charset::UTF8ToNative(szValue).c_str());
				pAttr = pAttr->next_attribute();
			}

			if (!pTemplate)
			{
				continue;
			}

			pParent->Add(pTemplate);

			if (pEleCtl->first_node())
				XMLToTemplateObjWithFilter(pTemplate, pEleCtl, pFilter);
		}
	}
}
//从模板中读取数据到窗口
void ResObj::TemplateObjToControl(CControlUI* pParent, TemplateObj* pRoot, CControlUI* pNext)
{
	int nCount = pRoot->NumChild();
	for (int i = 0; i < nCount; i++)
	{
		TemplateObj* pTempObj = pRoot->GetChild(i);
		CControlUI* pControlUI = GetEngineObj()->CreateControl(pTempObj->GetType());
		// 如果控件不存在则尝试创建插件
		if (!pControlUI)
		{
			if (_tcsicmp(pTempObj->GetType(), _T("plugin")) == 0)
			{
				MAPAttr* mapAttr = pTempObj->GetAttr();
				if (mapAttr)
				{
					MAPAttr::iterator pos = mapAttr->find(_T("guid"));
					//设置样式
					if (pos != mapAttr->end())
					{

						/*GUID guid;
						if (S_OK == CLSIDFromString(LPOLESTR(pos->second.c_str()), &guid))
						{*/
						pControlUI = GetEngineObj()->CreatePlugin(pos->second.c_str());
						/*}*/

						
					}
				}
			}
		}

		if (pControlUI)
		{
			MAPAttr* mapAttr = pTempObj->GetAttr();
			if (mapAttr)
			{
				MAPAttr::iterator pos =  mapAttr->find(_T("style"));

				//设置样式
				if (pos != mapAttr->end())
					pControlUI->SetStyle(pos->second.c_str());
				else
				{
					if (pControlUI->GetStyle() == NULL)
						pControlUI->SetStyle(pTempObj->GetType());
				}
			}

			pControlUI->SetManager(pParent->GetWindow(), pParent);

			if (mapAttr)
			{
				for (MAPAttr::iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
					pControlUI->SetAttribute(pos->first.c_str(), pos->second.c_str());
			}

			if (!pNext)
				pParent->Add(pControlUI);
			else
				pParent->Insert(pControlUI, pNext);
			if (pTempObj->NumChild() > 0)
				TemplateObjToControl(pControlUI, pTempObj);
			pControlUI->Init();
		}
		else if (equal_icmp(_T("xml"), pTempObj->GetType()) || equal_icmp(_T("include"), pTempObj->GetType()))
		{
			LPCTSTR lpszId = pTempObj->GetAttr(_T("id"));
			if (lpszId)
			{
				GetResObj()->AddControl(pParent, NULL, lpszId);
				MAPAttr* mapAttr = pTempObj->GetAttr();
				if (mapAttr)
				{
					for (MAPAttr::iterator pos = mapAttr->begin(); pos != mapAttr->end(); ++pos)
					{
						int j = pos->first.find_last_of(_T('.'));
						if (j > 0)
						{
							tstring strControlName = pos->first.substr(0, j);
							CControlUI* pControl = pParent->GetItem(strControlName .c_str());
							if (pControl)
							{
								pControl->SetAttribute(pos->first.substr(j + 1).c_str(), pos->second.c_str());
							}
							else
							{
								j = strControlName.find_last_of(_T('.'));
								if (j > 0)
								{
									strControlName = pos->first.substr(0, j);
									pControl = pParent->GetItem(strControlName .c_str());
									if (pControl) pControl->SetAttribute(pos->first.substr(j + 1).c_str(), pos->second.c_str());
								}
							}
						}
					}
				}

			}

		}
	}

}

void ResObj::AddColorObj( ColorObj* pColorObj )
{
	m_mapColorsTable[pColorObj->GetId()] = pColorObj;
}

ColorObj* ResObj::GetColorObj( LPCTSTR lpszId )
{
	if (lpszId)
	{
		map<tstring, ColorObj*>::iterator itColors = m_mapColorsTable.find(lpszId);
		if (itColors != m_mapColorsTable.end())
			return itColors->second;
	}
	return NULL;
}

ColorObj* ResObj::GetDefaultColor()
{
	return ms_pDefaultColorObj;
}

void ResObj::AddCurveObj( CurveObj* pCurveObj )
{
	m_mapCurvesTable[pCurveObj->GetId()] = pCurveObj;
}

CurveObj* ResObj::GetCurveObj( LPCTSTR lpszId )
{
	if (lpszId)
	{
		map<tstring, CurveObj*>::iterator itCurves = m_mapCurvesTable.find(lpszId);
		if (itCurves != m_mapCurvesTable.end())
			return itCurves->second;
	}
	return NULL;
}
void ResObj::SetTemplateFilter(ITemplateFilter* pFilter)
{
	m_pTemplateFilter = pFilter;
}

UINT GetResTypeFromStr(const char* lpszStr)
{
	if (!lpszStr)
		return _RES_TYPE_ALL;

	const char cSplitter = '|';
	LPSTR lpcSplitter = (LPSTR)strchr(lpszStr, cSplitter);
	if (lpcSplitter == NULL)
	{
		return GetResTypeFromStrInternal(lpszStr);
	}
	
	//combine type
	LPCSTR lpcHead = lpszStr;
	UINT uType = 0;

	do 
	{
		*lpcSplitter = NULL;
		uType |= GetResTypeFromStrInternal(lpcHead);
		*lpcSplitter = cSplitter;
		lpcHead = ++lpcSplitter;
		lpcSplitter = (LPSTR)strchr(lpcHead, cSplitter);
	} while (lpcSplitter);

	if (*lpcHead != NULL)
	{
		uType |= GetResTypeFromStrInternal(lpcHead);
	}

	return uType;
}

UINT GetResTypeFromStrInternal(const char* lpszStr)
{
	if (equal_strcmp(lpszStr, "window"))
		return _RES_TYPE_WINDOW;

	if (equal_strcmp(lpszStr, "menu"))
		return _RES_TYPE_MENU;

	if (equal_strcmp(lpszStr, "style"))
		return _RES_TYPE_STYLE;

	if (equal_strcmp(lpszStr, "image"))
		return _RES_TYPE_IMAGE;

	if (equal_strcmp(lpszStr, "font"))
		return _RES_TYPE_FONT;

	if (equal_strcmp(lpszStr, "string"))
		return 	_RES_TYPE_STRING;

	if (equal_strcmp(lpszStr, "color"))
		return 	_RES_TYPE_COLOR;

	if (equal_strcmp(lpszStr, "curve"))
		return 	_RES_TYPE_CURVE;

	if (equal_strcmp(lpszStr, "global"))
		return 	_RES_TYPE_GLOBAL;

	if (equal_strcmp(lpszStr, "user"))
		return 	_RES_TYPE_USER;
	
	return _RES_TYPE_ALL;
}

