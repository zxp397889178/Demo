#pragma once

#define  DUI_ARGB(a, r, g, b) (((DWORD)(b) << 16) | ((DWORD)(g) << 8) | ((DWORD)(r) << 0) | ((DWORD)(a) << 24))
#define  DUI_ARGB2RGB(argb) ((DWORD)argb & 0x00FFFFFF)
#define  DUI_RGB2ARGB(rgb) ((DWORD)rgb | 0xFF000000)

//常用文字光晕色
#define SHADOW_COLOR_WHITE		0xFFFFFFFF
#define SHADOW_COLOR_BLACK		0x55000000
#define SHADOW_COLOR_GOLDEN		0x5500D7FF				//Color::Gold            = 0xFFFFD700,

class DUI_API ColorObj : public BaseObject
{
public:
	//{{
	ColorObj();
	~ColorObj();
	//}}
	DWORD GetColor() const;
	void SetColor(DWORD val);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);
	//{{
protected:
	DWORD m_dwColor;
private:
	//}}
};

class DUI_API ColorObjEx
{
public:
	ColorObjEx();
	void SetColor(LPCTSTR lpszValue);
	void SetColor(DWORD dwValue);
	DWORD GetColor()const;
	void SetColorObj(ColorObj* colorObj);
	ColorObj* GetColorObj() const;
private:
	ColorObj* color_obj_;
	DWORD color_value_;
};
