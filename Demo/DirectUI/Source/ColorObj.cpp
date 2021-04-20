#include "stdafx.h"
#include "ColorObj.h"
#include "InterDef.h"

ColorObj::ColorObj()
{
	
}

ColorObj::~ColorObj()
{
	
}

DWORD ColorObj::GetColor() const
{
	return m_dwColor;
}

void ColorObj::SetColor( DWORD val )
{
	m_dwColor = val;
}

void ColorObj::SetAttribute( LPCTSTR lpszName, LPCTSTR lpszValue )
{
	if (equal_icmp(lpszName, _T("value")))
	{
		m_dwColor = CSSE::StringToColor(lpszValue);
	}
	else
	{
		__super::SetAttribute(lpszName, lpszValue);
	}
}


ColorObjEx::ColorObjEx()
{
	color_obj_ = GetUIRes()->GetDefaultColor();;
	color_value_ = color_obj_->GetColor();
}

void ColorObjEx::SetColor(LPCTSTR lpszValue)
{
	if (lpszValue[0] == '#')
	{
		color_obj_ = GetResObj()->GetColorObj(lpszValue + 1);
	}
	else
	{
		color_obj_ = NULL;
		color_value_ = CSSE::StringToColor(lpszValue);
	}
}

void ColorObjEx::SetColor(DWORD dwValue)
{
	color_obj_ = NULL;
	color_value_ = dwValue;
}

void ColorObjEx::SetColorObj(ColorObj* colorObj)
{
	color_obj_ = colorObj;
}

ColorObj* ColorObjEx::GetColorObj() const
{
	return color_obj_;
}

DWORD ColorObjEx::GetColor() const
{
	if (color_obj_)
		return color_obj_->GetColor();
	return color_value_;
}