
#pragma once

/*!
    \brief    进度条控件
*****************************************/
class DUI_API CProgressBarUI:public CControlUI
{
	//{{
	UI_OBJECT_DECLARE(CProgressBarUI, _T("ProgressBar"))
public:
	CProgressBarUI();
	virtual ~CProgressBarUI();
	//}}
	void SetRange(const unsigned long long& firstpos,const unsigned long long& lastpos);
	virtual void SetPos(const unsigned long long& curpos, bool bUpdate = false);
	virtual void SetPos(const float fPercent, bool bUpdate = false);
	void GetRange(unsigned long long& firstpos ,unsigned long long& lastpos);
	unsigned long long GetPos();
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	/*!
	   \brief    前景图片是否裁剪显示 
	   \note     
	   \return   bool 
	 ************************************/
	bool IsClipRender() const;

	/*!
	   \brief    设置前景图片裁剪显示 
	   \note     
	   \param    bool val 
	   \return   void 
	 ************************************/
	void ClipRender(bool val);
	//{{
protected:
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	unsigned long long m_ulfirstPos;
	unsigned long long m_ullastPos;
	unsigned long long m_ulcurPos;
	bool m_bHorizontal;
	bool m_bClipRender;
	//}}
};

