#pragma once
#ifndef __DIRECTUI_VIEW_H_
#define __DIRECTUI_VIEW_H_

/*!
    \brief    全局基类
*****************************************/
class DUI_API BaseObject
{
public:
	/*!
	   \brief    设置ID 
	   \return   LPCTSTR 
	 ************************************/
	LPCTSTR GetId() const;

	/*!
	   \brief    获取ID
	   \param    LPCTSTR val 
	   \return   void 
	 ************************************/
	void SetId(LPCTSTR val);

	/*!
	\brief    设置属性
	\param    LPCTSTR lpszName 
	\param    LPCTSTR lpszValue 
	\return   void 
	************************************/
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);

protected:
	/// ID
	tstring m_strId;
private:
	//}}
};


/*!
    \brief    坐标
*****************************************/
typedef struct Coordinate
{
	/// 锚点
	UITYPE_ANCHOR anchor;
	/// 锚点矩形
	RECT rect;
} Coordinate;

//////////////////////////////////////////////////////////////////////////
/// 基类，具备基础信息
class DUI_API CViewUI : public BaseObject
{
public:
	// 设置属性
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);

	// 设置获取描点
	void SetAnchor(UITYPE_ANCHOR anchor);
	UITYPE_ANCHOR GetAnchor();

	// 设置获取描点坐标
	void SetCoordinate(int l, int t, int r, int b, UITYPE_ANCHOR anchor = UIANCHOR_UNCHANGED);
	void SetCoordinate(Coordinate& coordinate);
	void GetCoordinate(int& l, int& t, int& r, int& b);
	Coordinate* GetCoordinate();

	// 获取或设置状态
	void SetState(UITYPE_STATE nState);
	UITYPE_STATE GetState();

	// 获取或设置透明度.
	int  GetAlpha();
	virtual void SetAlpha(int val);

	//  设置获取可见性
	bool IsVisible();
	virtual void SetVisible(bool bVisible);

	// 获取或设置矩形大小
	RECT	GetRect();
	LPRECT	GetRectPtr();

	SIZE	GetMaxSize();
	void	SetMaxSize(SIZE& size);

	SIZE	GetMinSize();
	void	SetMinSize(SIZE& size);

	virtual void EnableRTLLayout(bool bEnableLayout);
	virtual bool IsRTLLayoutEnable();

	virtual void OnSize(const RECT& rectParent);
	virtual void SetRect(RECT& rectRegion);
	virtual bool Resize();

	// 刷新
	virtual void Invalidate(bool bUpdateWindow = false) = 0;
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint) = 0;
	//{{
protected:
	CViewUI();
	virtual ~CViewUI();
	/// 坐标
	
	/// 绘制区域
	RECT m_rcItem;
	/// 是否显示，重置该值时需刷新位置信息
	bool m_bVisible;
	/// 当前绘制状态
	UITYPE_STATE m_nState;
	/// 透明度
	int m_nAlpha;
	
	//}}

private:
	Coordinate m_coordinate;
	// 最大宽度高度
	SIZE m_cxyMax;
	// 最小宽度高度
	SIZE m_cxyMin;
	// 用于阿拉伯版本
	bool m_bFlipRect;
};

#endif