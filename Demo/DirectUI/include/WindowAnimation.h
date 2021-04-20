#pragma once

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 窗口动画，基类                                                       */
/************************************************************************/
class DUI_API WindowAnimation : public CAction
{
	
public:
	enum WindowAnimationState
	{
		WindowAnimationState_Show = 0,
		WindowAnimationState_Hide,
		WindowAnimationState_Close,
		WindowAnimationState_None,
	};
	//{{
	WindowAnimation();

	virtual ~WindowAnimation();
	//}}

	void SetAnimationState(WindowAnimationState state);

	virtual LPCTSTR GetClass(){return _T("WindowAnimation");}
	
protected:
	//{{
	virtual void Start( CWindowUI *pWindow ){};

	virtual void startWithTarget(BaseObject* pActionTarget);

	virtual void onFinished();

	WindowAnimationState m_AnimationState;


	HWND m_hWnd;
	RECT m_rcWindow;
	SIZE m_szWindow;
	RECT m_rcAnimationDC;
	SIZE m_szAnimationDC;
	bool m_bLayerWindow;
	CWindowUI *m_pWindow;

	BLENDFUNCTION m_BlendFun;
	DibObj* m_pDibObj;
	//}}
};
 
class DUI_API CTurnObjectAnimation : public WindowAnimation
{
public:
	//{{
	CTurnObjectAnimation();
	~CTurnObjectAnimation();
	//}}

	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);
	//}}
};

class DUI_API CInflateAnimation : public WindowAnimation
{
public:
	//{{
	CInflateAnimation();
	~CInflateAnimation();
	//}}

	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);
	//}}
};

class DUI_API CScatterAnimation : public WindowAnimation
{
public:
	//{{
	CScatterAnimation();
	~CScatterAnimation();
	//}}

	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);

	POINT m_ptCenter;
	SIZE m_szWnd;
	//}}
};

class DUI_API CBlindAnimation : public WindowAnimation
{
public:
	//{{
	typedef enum{
		BlindTop = 0, BlindBottom, BlindLeft, BlindRight 
	}BlindType;

	CBlindAnimation( BlindType iBlindType = BlindTop, DWORD dwTimer = 50, bool bActive = false, bool bIn = true );
	~CBlindAnimation();
	//}}
	void onFinished();

	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);

	DWORD m_dwTimer;
	POINT m_ptCenter;
	SIZE m_szWnd;
	int m_iBlindType;
	bool m_bActive;
	bool m_bIn;

	//}}
};

class DUI_API CCloseAnimation : public WindowAnimation
{
public:
	//{{
	CCloseAnimation();
	~CCloseAnimation();
	//}}

	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);
	//}}
};

class DUI_API CFadeCloseAnimation : public WindowAnimation
{
public:
	//{{
	CFadeCloseAnimation();
	~CFadeCloseAnimation();
	//}}

	//{{
protected:

	void Start(CWindowUI *pWindow);
private:

	virtual void step(IActionInterval* pActionInterval);
	//}}
};

class DUI_API CPopupAnimation : public WindowAnimation
{
public:
	//{{
	CPopupAnimation();
	~CPopupAnimation();
	//}}

	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);
	//}}
};

class DUI_API CMenuAnimation :  public WindowAnimation
{
public:
	//{{
	typedef enum{
		BlindLT = 0, BlindRT, BlindLB, BlindRB
	}BlindType;
	CMenuAnimation( BlindType iType = BlindLT );
	~CMenuAnimation();
	//}}
	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);

	BlindType m_iBlindType;
	//}}
};

class DUI_API CTurnButtonIconAnimation : public WindowAnimation
{
public:
	//{{
	CTurnButtonIconAnimation();
	~CTurnButtonIconAnimation();
	//}}

	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);

	double m_dAcceleration;
	//}}
};

class DUI_API CPosChangeAnimation : public WindowAnimation
{
public:
	//{{
	CPosChangeAnimation( int iDisX, int iDisY );
	~CPosChangeAnimation();
	//}}

	//{{
protected:

	void Start( CWindowUI *pWindow );
private:

	virtual void step(IActionInterval* pActionInterval);

	int m_iDisX;
	int m_iDisY;
	//}}
};

class DUI_API CZoomAnimation : public CPopupAnimation
{
public:
	//{{
	CZoomAnimation();
	//}}

	RECT ZoomDest() const;

	void ZoomDest(const RECT& val);
private:
	//{{

	virtual void step(IActionInterval* pActionInterval);

	RECT m_rcZoomDest;
	//}}
};

