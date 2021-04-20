
#pragma once

/*!
    \brief    �������ؼ�
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
	   \brief    ǰ��ͼƬ�Ƿ�ü���ʾ 
	   \note     
	   \return   bool 
	 ************************************/
	bool IsClipRender() const;

	/*!
	   \brief    ����ǰ��ͼƬ�ü���ʾ 
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

