#pragma once

/*!
    \brief    滚动条控件
*****************************************/
class DUI_API CScrollbarUI:public CControlUI
{
	UI_OBJECT_DECLARE(CScrollbarUI, _T("ScrollBar"))
public:
	CScrollbarUI();
	virtual ~CScrollbarUI();
	//}}

public:
	bool IsHorizontal();
	void SetHorizontal(bool bHorizontal = false);
	int	 GetScrollRange();
	void SetScrollRange(int nRange);
	int  GetScrollPos();
	void SetLineSize(int nLineSize);
	int  GetLineSize();
	void PageUp();
	void PageDown();
	void LineUp();
	void LineDown();
	void SetScrollPos(int nPos,  bool bRedraw = true);
	void OffsetScrollPos(int nOffset, bool bTween = false);

	SIZE GetFixedSize();
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue); //重载基类
	void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	void SetRect(RECT& rectRegion);
protected:
	virtual bool Event(TEventUI& event);
private:
	void CalcRect();
	RECT GetThumbRect();//rtl版本下用m_rcthum是相反的值
	void HandleGestureEvent(TEventUI& event);
	bool m_bHorizontal;
	long m_nRange,m_nScrollPos,m_nLastScrollPos,m_nLineSize;
	int m_nScrollRepeatDelay;
	long m_nLastScrollOffset;
	POINT m_ptLastMouse;
	SIZE m_cxyFixed;
	UITYPE_STATE m_iStatusBegin,m_iStatusEnd,m_iStatusThumb;
	RECT m_rcThumb;
	//滚动滚动条中间滚动使用
	LONG  m_dwDelayDeltaY;
	DWORD m_dwDelayNum;
	DWORD m_dwDelayLeft;
	bool m_bTweenScrolling;
	int m_nTweenLastScrollPos;
	int m_nTweenOffset;
	int m_nTweenCount;
	//}}
};
