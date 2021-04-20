#pragma once
#ifndef __DIRECTUI_VIEW_H_
#define __DIRECTUI_VIEW_H_

/*!
    \brief    ȫ�ֻ���
*****************************************/
class DUI_API BaseObject
{
public:
	/*!
	   \brief    ����ID 
	   \return   LPCTSTR 
	 ************************************/
	LPCTSTR GetId() const;

	/*!
	   \brief    ��ȡID
	   \param    LPCTSTR val 
	   \return   void 
	 ************************************/
	void SetId(LPCTSTR val);

	/*!
	\brief    ��������
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
    \brief    ����
*****************************************/
typedef struct Coordinate
{
	/// ê��
	UITYPE_ANCHOR anchor;
	/// ê�����
	RECT rect;
} Coordinate;

//////////////////////////////////////////////////////////////////////////
/// ���࣬�߱�������Ϣ
class DUI_API CViewUI : public BaseObject
{
public:
	// ��������
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR  lpszValue);

	// ���û�ȡ���
	void SetAnchor(UITYPE_ANCHOR anchor);
	UITYPE_ANCHOR GetAnchor();

	// ���û�ȡ�������
	void SetCoordinate(int l, int t, int r, int b, UITYPE_ANCHOR anchor = UIANCHOR_UNCHANGED);
	void SetCoordinate(Coordinate& coordinate);
	void GetCoordinate(int& l, int& t, int& r, int& b);
	Coordinate* GetCoordinate();

	// ��ȡ������״̬
	void SetState(UITYPE_STATE nState);
	UITYPE_STATE GetState();

	// ��ȡ������͸����.
	int  GetAlpha();
	virtual void SetAlpha(int val);

	//  ���û�ȡ�ɼ���
	bool IsVisible();
	virtual void SetVisible(bool bVisible);

	// ��ȡ�����þ��δ�С
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

	// ˢ��
	virtual void Invalidate(bool bUpdateWindow = false) = 0;
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint) = 0;
	//{{
protected:
	CViewUI();
	virtual ~CViewUI();
	/// ����
	
	/// ��������
	RECT m_rcItem;
	/// �Ƿ���ʾ�����ø�ֵʱ��ˢ��λ����Ϣ
	bool m_bVisible;
	/// ��ǰ����״̬
	UITYPE_STATE m_nState;
	/// ͸����
	int m_nAlpha;
	
	//}}

private:
	Coordinate m_coordinate;
	// ����ȸ߶�
	SIZE m_cxyMax;
	// ��С��ȸ߶�
	SIZE m_cxyMin;
	// ���ڰ������汾
	bool m_bFlipRect;
};

#endif