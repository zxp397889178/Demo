#pragma once

/*!
   \brief    依据UIFIND_等值判断pControl是否是所要的控件
   \note     
   \param    CControlUI * pControl 
   \param    LPVOID pData 当uFlags包含UIFIND_HITTEST时，pData为LPPOINT
   \param    UINT uFlags 
   \return   bool 
 ************************************/
bool FindControlTest(CControlUI* pControl,  LPVOID pData, UINT uFlags);
void CalcParentPosWithAnchor(IN UITYPE_ANCHOR eAnchor, IN const RECT* lprcCoordinate, IN const RECT* lprcChild, OUT RECT* lprcParent);

typedef std::map<tstring, tstring> MAPAttr;

void PrasePosString(LPCTSTR lpszPosStr, int& l, int& t, int& r, int& b, UITYPE_ANCHOR& anchor);
// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric);
// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric);
// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx);
// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy);

#define UISTATE_UNKNOWN  -1
//定时器结构
typedef struct tagTIMERINFO
{
	CControlUI* pSender;
	UINT nLocalID;
	HWND hWnd;
	UINT uWinTimer;
} TIMERINFO;

typedef enum
{
	UIEVENT_SELECT = UIEVENT__LAST + 1,
	// 框选
	UIEVENT_AREA_SELECT,
	UIEVENT_AREA_SELECT_END,
}UITYPE_EVENT_INTER;


class ItemHighlightMessageFilter : public IMessageFilterUI
{
public:
	ItemHighlightMessageFilter(CControlUI* pOwner);
	virtual ~ItemHighlightMessageFilter();
	CControlUI* GetOwner();

	static void AddToMessageFilterList(CControlUI* pOwner, ItemHighlightMessageFilter* pItemHighlightMessageFilter = NULL);
	static void RemoveFromMessageFilterList(CControlUI* pOwner);
protected:
	virtual CControlUI* ItemFromPos(POINT& pt);
	virtual LRESULT MessageFilter( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

private:
	bool OnHighlightCtlEvent(TEventUI& event);
	CControlUI* m_pHighlight;
	CControlUI* m_pOwner;
};

class ColorObjExHelper
{
public:
	static DWORD GetColor(ColorObjEx* colorObj)
	{
		if (colorObj == NULL)
			return GetUIRes()->GetDefaultColor()->GetColor();
		return colorObj->GetColor();
	}

	static void SetColor(ColorObjEx** colorObj, LPCTSTR lpszValue)
	{
		if (lpszValue == NULL || _tcslen(lpszValue) == 0 || _tcsicmp(lpszValue, _T("-1")) == 0)
		{
			ClearColor(colorObj);
			return;
		}
		if (*colorObj == NULL)
		{
			*colorObj = new ColorObjEx();
		}
		(*colorObj)->SetColor(lpszValue);
	}

	static void SetColor(ColorObjEx** colorObj, DWORD dwValue)
	{
		if (*colorObj == NULL)
		{
			*colorObj = new ColorObjEx();
		}
		(*colorObj)->SetColor(dwValue);
	}

	static void SetColor(ColorObjEx** colorObj, ColorObj* objValue)
	{
		if (*colorObj == NULL)
		{
			*colorObj = new ColorObjEx();
		}
		(*colorObj)->SetColorObj(objValue);
	}

	static void ClearColor(ColorObjEx** colorObj)
	{
		if (*colorObj != NULL)
		{
			delete (*colorObj);
			*colorObj = NULL;
		}
	}

	static void CopyColor(ColorObjEx** dest, ColorObjEx** src)
	{
		if (*src == NULL)
		{
			if (*dest != NULL)
			{
				delete (*dest);
				*dest = NULL;
			}
		}
		else
		{
			if (*dest == NULL)
				*dest = new ColorObjEx();
			memcpy(*dest, *src, sizeof(ColorObjEx));
		}
	}
};

namespace StringHelper
{
	_inline void Utf8ToNativeStrPtr(tstring& strDest, const char* src)
	{
#ifdef UNICODE	
		int length = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
		if (length <= 0)
		{
			return;
		}
		strDest.resize(length - 1);
		MultiByteToWideChar(CP_UTF8, 0, src, -1, &strDest[0], length);
#else
		strDest = src;
#endif
	}

	_inline void Utf8ToNativePtrPtr(LPTSTR& pDest, const char* src)
	{
#ifdef UNICODE	
		int length = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
		if (length <= 0)
		{
			return;
		}
		wchar_t* dest = new wchar_t[length];
		length = MultiByteToWideChar(CP_UTF8, 0, src, -1, dest, length);
		if (length <= 0)
		{
			delete dest;
			dest = NULL;
		}

		pDest = dest;
#else
		int length = strlen(src);
		if (length <= 0)
		{
			return;
		}
		pDest = new char[length + 1];
		strncpy(pDest, src, length);
#endif
	}
}