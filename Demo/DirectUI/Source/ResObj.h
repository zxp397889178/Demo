#ifndef _UIRESMANAGER_H
#define _UIRESMANAGER_H
#include "ColorObj.h"
#include "TemplateObj.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "RdbParser.h"
using namespace rapidxml;
#define  _BASE_SKIN_COLOR DUI_ARGB(255, 0, 159, 234)
// ��Դģ������
#define _RES_TYPE_WINDOW         0x00000001  //������Դ
#define _RES_TYPE_STYLE          0x00000002  //��ʽ��Դ
#define _RES_TYPE_MENU           0x00000004  //�˵���Դ
#define _RES_TYPE_IMAGE          0x00000008  //ͼ����Դ
#define _RES_TYPE_FONT           0x00000010  //������Դ
#define _RES_TYPE_STRING         0x00000020  //�ַ�����Դ
#define _RES_TYPE_COLOR          0x00000040  //��ɫ��Դ
#define _RES_TYPE_CURVE          0x00000080  //������Դ
#define _RES_TYPE_GLOBAL         0x00000100  //ȫ������
#define _RES_TYPE_USER           0x00000200     //�û�����
#define _RES_TYPE_ALL            0x0000FFFF  //������Դ

class ModuleObj
{
public:
	ModuleObj(LPCTSTR lpszModuleName, LPCTSTR lpszModulePath, UITYPE_RES nResType = UIRES_FROM_FILE);
	ModuleObj(LPCTSTR lpszModuleName, LPBYTE lpData, DWORD dwSize);
	~ModuleObj();
	void Open();
	void Close();
	LPCTSTR GetModuleName() const{return m_strModuleName.c_str();};
	LPCTSTR GetModulePath() const{return m_strModulePath.c_str();};
	UITYPE_RES GetResType() const{return m_nResType;};
	CRdbParser*   GetRdbFile();

private:
	ModuleObj(const ModuleObj&);
	UITYPE_RES m_nResType;
	tstring m_strModuleName;
	tstring m_strModulePath;
	CRdbParser*   m_pRdbFile;
};

class StringHolder{

public:
	StringHolder(){
	}

	StringHolder( StringHolder&  rhs){
		*this = rhs;
	}
	_inline StringHolder& operator =(StringHolder&  rhs){
		AttachPtr(rhs.GetPtr());
		rhs.SetPtr(NULL);
		return *this;
	}

	~StringHolder(){
		ReleasePtr();
	}

	_inline void ReleasePtr(){
		if (m_lpstr){
			delete m_lpstr;
			m_lpstr = NULL;
		}
	}

	_inline LPCTSTR GetPtr(){
		return (LPCTSTR)m_lpstr;
	}

	_inline void SetPtr(LPCTSTR lpstr){
		m_lpstr = (LPTSTR)lpstr;
	}

	_inline void AttachPtr(LPCTSTR lpstr){
		ReleasePtr();
		SetPtr(lpstr);
	}

	void DetachPtr(LPCTSTR lpstr){
		SetPtr(NULL);
	}

private:
	LPTSTR m_lpstr = NULL;

};

class ImagePathObj
{	
	typedef map<tstring, tstring> StringMapClass;
public:
	ImagePathObj();
	~ImagePathObj();
	void AddPath(LPCTSTR lpszPath, LPCTSTR lpszLanguaze = NULL);
	LPCTSTR GetPath(LPCTSTR lpszLanguaze = NULL);
	bool IsMutiPath();
	int GetResPriorityLevel();
	void SetResPriorityLevel(int nLevel);
	void SetModuleName(LPCTSTR lpszName);
	LPCTSTR GetModuleName();
private:
	tstring m_strPath;
	tstring m_strModuleName;
	StringMapClass* m_pMutiPath;
};



struct myCompare:public std::binary_function<tstring,tstring,bool>
{
	bool operator()(const tstring& left,const tstring& right) const
	{
		return _tcscmp(left.c_str(),right.c_str())<0;
	}
};


class ResObj:public IUIRes
{
public:
	static ResObj* GetInstance();
	static void ReleaseInstance();
public:
	virtual void SetCacheXML(bool bCacheXML);
	bool GetCacheXML();

	virtual void SetLanguage(LPCTSTR lpszLanguage);
	virtual LPCTSTR GetLanguage();
	virtual void SetImageLanguage(LPCTSTR lpszLanguage);

	virtual void SetDefaultFontName(LPCTSTR lpszFontName);
	virtual LPCTSTR GetDefaultFontName();


	virtual void SetDefaultFontType(UITYPE_FONT eFontType);
	virtual UITYPE_FONT GetDefaultFontType();

	virtual void SetDefaultImageType(UITYPE_IMAGE eImageType);
	virtual void SetDefaultImageQuality(UITYPE_SMOOTHING eImageQualityType);


	virtual void SetDefaultTextRenderingHint(int textRenderingHint  = 0);
	virtual int GetDefaultTextRenderingHint();

	virtual void InitResDir(LPCTSTR lpszPath, bool bDefaultDir = false);
	virtual void InitResDirFromRdb(LPCTSTR lpszPath, LPCTSTR lpszName, bool bDefaultDir = false);
	virtual void InitResDirFromRdb(LPBYTE lpData, DWORD dwSize, LPCTSTR lpszName, bool bDefaultDir = false);
	virtual void InitResDirFromRes(UINT uResID, LPCTSTR lpszType);

	// ��������Ŀ¼
	virtual void SetThemePath(LPCTSTR lpszPath);
	virtual void SetThemePathFromRDB(LPCTSTR lpszPath, bool bCoexistence = false);
	virtual CRdbParser*  GetThemeRdbFile();
	virtual LPCTSTR GetThemePath();
	
	virtual void SetHDPIImagePath(LPCTSTR lpszPath);
	virtual void SetHDPIImagePathRDB(LPCTSTR lpszPath);
	virtual CRdbParser*  GetHDPIImageRdbFile();
	LPCTSTR GetHDPIImagePath();

	virtual void SetHDPI9ImageParamScale(float fScale);
	float GetHDPI9ImageParamScale();

	virtual void SetHDPIAutoStretchOnDrawing9Image(bool bAutoStretch);
	bool GetHDPIAutoStretchOnDrawing9Image();

	virtual ImageObj*  LoadImage(LPCTSTR lpszPath, bool bCache = true, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT);
	virtual ImageObj*  LoadImage(UINT uResID, LPCTSTR lpszType, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT);
	virtual ImageObj*  LoadImage(LPVOID lpData, long nSize, UITYPE_IMAGE eImageType = UIIAMGE_DEFAULT);
	virtual CursorObj* LoadCursor(LPCTSTR lpszPath);

	virtual bool LoadSkin(LPCTSTR lpszFile);
	virtual bool LoadSkin(const void* data, size_t data_len);
	virtual void SetTemplateFilter(ITemplateFilter* pFilter);
	bool LoadSkin(LPCTSTR lpszFile, LPCTSTR lpszName, UINT uResType);

	RECT GetSystemScrollBarPosition(bool bVert); // ��ȡ������λ��
	RECT GetWindowShadowEdgeRect();//��ȡĬ�ϴ�����Ӱ��Ե��С
	void ReLoadImage();

	virtual LPCTSTR  GetUserData(LPCTSTR lpszId);
	virtual void  SetUserData(LPCTSTR lpszId, LPCTSTR lpszValue);

	virtual int GetModuleCount();
	virtual LPCTSTR GetModulePath(int index);
	virtual LPCTSTR GetModuleName(int index);
public:
	void  ChangeImageHSL(COLORREF clrBackground);
	virtual LPCTSTR GetImageFileFullPath(LPCTSTR lpszPath);
	
	virtual UIMENUINFO GetMenu( UINT uMenuID, int nIndex = -1, UINT uFlag = BY_POSITION );
	virtual bool AttachMenu(CControlUI* pControl, UINT uMenuID);
	virtual bool AddControl(CControlUI* pParent, CControlUI* pNext, LPCTSTR lpszId);

	bool AddSytleList(CControlUI* pCtl, CStdPtrArray** ppStyleList, LPCTSTR lpszId);
	LPCTSTR GetI18NStr(LPCTSTR lpszId, bool* bFind = NULL);
	ModuleObj* GetModuleObj(LPCTSTR lpszModuleName = NULL);

	virtual void  RemoveImageFromTable(LPCTSTR lpszId);

	TemplateObj* GetWindowTemplateObj(LPCTSTR lpszId);
	void DelWindowTemplateObj(LPCTSTR lpszId);

	const MAPAttr* GetTemplateObjAttr(LPCTSTR lpszTemplateID);
	void TemplateObjToControl(CControlUI* pParent, TemplateObj* pRoot, CControlUI* pNext = NULL);
	void DelStyleTemplateObj(LPCTSTR lpszId);

	virtual void AddFont(FontObj* pFontObj);
	virtual FontObj* GetFont(LPCTSTR lpszId);
	virtual FontObj* GetDefaultFont();
	void ReBulidFont();

	virtual void  AddCursor(CursorObj* pCursorObj);
	virtual CursorObj* GetCursor(LPCTSTR lpszId);
	virtual CursorObj* GetDefaultCursor();

	virtual void AddColorObj(ColorObj* pColorObj);
	virtual ColorObj* GetColorObj(LPCTSTR lpszId);
	virtual ColorObj* GetDefaultColor();

	void AddCurveObj(CurveObj* pCurveObj);
	CurveObj* GetCurveObj(LPCTSTR lpszId);

	static FontObj* ms_pDefaultFontObj;                             // ����Ĭ�ϵ����壬��Դ���õĵ�һ������ΪĬ������
	static ColorObj* ms_pDefaultColorObj;                            
	static CursorObj* ms_pDefaultCursorObj;                         // Ĭ�Ϲ��
	static UITYPE_SMOOTHING  ms_eImageQualityType;
private:
	ResObj(void);
	virtual ~ResObj(void);
	void InitResDir(xml_document<>* pXmlDoc);
	bool LoadSkin(xml_document<>* pXmlDoc, LPCTSTR lpszName, UINT uResType);
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	ImageObj*  LoadImage(tstring& strPath, UITYPE_IMAGE eImageType);
	LPCTSTR GetImagePathFromId(LPCTSTR lpszId);
	void ReloadImageById(LPCTSTR lpszId);
	ImageObj* LoadImageFromThemePath(ModuleObj* pModule, LPCTSTR lpszPath, UITYPE_IMAGE eImageType, bool& bHdpi);
	ImageObj* LoadImageFromHDPIPath(ModuleObj* pModule, LPCTSTR lpszPath, UITYPE_IMAGE eImageType);


	std::map<tstring, ModuleObj*>	m_mapModuleObjTable;	// ģ��ģ��·��
	ModuleObj* m_pDefModulePathObj;						    // ��������Դ·��
	typedef std::map<tstring, StringHolder> NameTableMap;
	NameTableMap	 m_mapNameTable;			// ����������Ӧ��
	typedef std::map<tstring, TemplateObj*, myCompare>::iterator TemplateIterator;
	//��Դ
	std::map<tstring, TemplateObj*, myCompare>	m_mapWindowTable;		// ���洰��ģ�� _RES_TYPE_WINDOW
	std::map<tstring, TemplateObj*, myCompare>	m_mapStyleTable;		// ������ʽģ�� _RES_TYPE_STYLE
	std::map<UINT, TemplateObj*>    m_mapMenuTable;			// ����˵��б� _RES_TYPE_MENU
	std::map<tstring, ImageObj*>	m_mapImageObjTable;			// �����ѵ����ͼƬ
	typedef std::map<tstring, ImagePathObj*> ImagePathObjMap;
	ImagePathObjMap m_mapImagePathObjTable;		// ����ͼƬid��·��
	typedef std::map<tstring, StringHolder> I18NStringMap;
	I18NStringMap      m_mapStringTable;		// �����������ַ����б� _RES_TYPE_STRING

	std::map<tstring, FontObj*>	m_mapFontsTable;		// ����������Դ _RES_TYPE_FONT
	std::map<tstring, ColorObj*>	m_mapColorsTable;		// ������ɫ��Դ _RES_TYPE_COLOR
	std::map<tstring, CurveObj*>	m_mapCurvesTable;		// ������Դ _RES_TYPE_CURVE
	std::map<tstring, CursorObj*>	m_mapCursorsTable;		// ��������Դ _RES_TYPE_CURSOR
	std::map<tstring, tstring>	m_mapUserDataTable;		// �û��Զ�����Դ
	
	static ResObj* m_pResManager;

	tstring m_strImageFileFullPath;
	tstring m_strLanguage;
	tstring m_strDefaultFontName;
	RECT m_rcHScrollPositon, m_rcVScrollPositon; // �������Ĵ�Сλ��
	RECT m_rcWindowShadowEdge;  // ���ڱ�Ե��С
	tstring m_strImageLanguage;
	double m_dHue, m_dSaturation, m_dLightness; //HSLֵ
	ITemplateFilter*	m_pTemplateFilter;
	bool m_bCacheXML;
	tstring m_strThemePath;
	CRdbParser*   m_pThemeRdbFile;
	CRdbParser*   m_pHDPIRdbFile;
	tstring m_strHDPIImagePath;
	float m_fHDPIImageNinePatchScale;
	bool m_bHDPIAutoStretchOnDrawing9Image;
};

ResObj* GetResObj();

#endif  //_UIRESMANAGER_H