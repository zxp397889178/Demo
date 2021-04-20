#pragma once

inline int GetWidth(RECT& rc) 
{
	return rc.right - rc.left;
}

inline int GetHeight(RECT& rc)
{
	return rc.bottom - rc.top;
}

struct ITrackerBoxEvent
{
	virtual void InvalidateRect(const RECT *lpRect, bool bErase) = 0;
};


class CUI_API CTrackerBox
{
public:
	CTrackerBox();
	virtual ~CTrackerBox();

public:

	enum HITPLACE
	{
		NOHIT,INBOX,
		TOPBOX,BOTTOMBOX,LEFTBOX,RIGHTBOX,
		TOPLEFTBOX,TOPRIGHTBOX,BUTTOMLEFTBOX,BUTTOMRIGHTBOX
	};

public:
	void			SetNotify(ITrackerBoxEvent* pEevnt){ m_pEvent = pEevnt; };
	/// hWnd == NULL, �ûص�
	bool			BenginTrack(POINT& point, HWND hWnd = NULL, bool bRedraw = true);
	bool			Track(POINT& point, HWND hWnd = NULL, bool bRedraw = true);
	bool			EndTrack(HWND hWnd = NULL, bool bRedraw = true);
	//void			Draw(HDC hDC);

	void			Clear();
	void			SetRect(RECT& rect);
	void			SetBoundRect(RECT& rect);
	void			SetLineColor(COLORREF color);
	RECT			GetRect();
	RECT			GetBoundRect();
	HITPLACE		HitTest(POINT& point);
	void			LockTracInBound(bool bLock){ m_bLockTracInBounder = bLock;}
	void			LockTracRatio(bool bLock){ m_bLockTracRatio = bLock; }
	void			SetRatio(float fRatio){ m_fRatio = fRatio; }
	float			GetRatio(){ return m_fRatio; }
	void			SetMinSize(int cx, int cy){ m_szMinSize.cx = cx; m_szMinSize.cy = cy; }
	SIZE			GetMinSize() { return m_szMinSize; }
	bool			IsTracking(){return m_bLButtonDown;}
	bool			IsExistBox(){return m_isExistBox;}
	void			SetCursor(POINT& pt);

protected:
	enum POINT_DEF
	{
		POINT_DEF_X = -1000,
		POINT_DEF_Y = -1000
	};

protected:
	/// NESW ָ��б������ƶ�
	void			GetRectByRatioByNESW(RECT& rc);
	bool			GetRectByMinSizeByNESW(RECT& rc);
	bool			GetRectByBoundByNESW(RECT& rc, POINT ptMove, POINT ptFix);
	/// WE ָ������, �����ƶ�, 
	void			GetRectByRatioByWE(RECT& rc);
	void			GetRectByMinSizeByWE(RECT& rc, POINT* ptFix = NULL);
	void			GetRectByBoundByWE(RECT& rc, POINT ptMove, POINT ptFix);
	void			CalcRect();
	void			OffSet(POINT&point);

protected:
	RECT			m_rcItem;
	RECT			m_rcBound;
	POINT			m_ptStart,m_ptEnd;
	POINT			m_ptMouseInit;	/// �����ʼ���°�ťʱ����λ��
	bool			m_isExistBox;
	HPEN			m_hPen;
	HBRUSH			m_hBrush;
	HITPLACE		m_hitPlace;
	HCURSOR			m_cursorArray[6];
	bool			m_bLButtonDown;
	ITrackerBoxEvent* m_pEvent;
	bool			m_bLockTracInBounder;	/// �Ƿ���Խ���߽�
	bool			m_bLockTracRatio;		/// �Ƿ����������
	float			m_fRatio;		/// �����
	SIZE			m_szMinSize;	/// ��С��С
	POINT			m_ptFixLast;	/// �ϴ�б���ƶ�ʱ���ֲ���ĵ�
};