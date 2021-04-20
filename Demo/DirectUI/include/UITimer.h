/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
 *********************************************************************
 *   Date             Name                 Description
 *   2013-03-27       xqb
*********************************************************************/
#pragma once

class CUITimerBase;
typedef void (__stdcall*TimerCallback)(CUITimerBase* pTimer);
class DUI_API CUITimerBase
{

public:
	CUITimerBase();
	CUITimerBase(const CUITimerBase& rhs);
	virtual ~CUITimerBase();
	virtual CUITimerBase* Copy();
	//}}
public:
	void SetTimerParam(UINT iInterval,UINT iTotalTimer = 0, bool bLoop = false);
	void SetParam(UINT iInterval,UINT iTotalFrame = 0, bool bLoop = false);
	void SetTimerCallback(TimerCallback fn);
	void Start();
	void Stop();
	void Pause();
	void Resume();

	bool IsLoop();		//是否循环，例如：0,1,2,3,4;0,1,2,3,4
	bool IsRebound();	//是否反弹，例如：0,1,2,3,4,3,2,1,0,自动反弹到初始化值
	bool IsReverse();	//是否逆序，例如：4,3,2,1,0
	bool IsDone();
	bool IsRunning();

	void SetRebound(bool bRebound);
	void SetByStep(bool bByStep);
	void SetReverse(bool bReverse);
	void SetCurFrame(LONG nCurFrame);

	LONG GetInterval();
	LONG GetTotalTimer();
	LONG GetTotalFrame();
	LONG GetCurFrame();

	void SetTimerId(UINT uId);
	UINT GetTimerId();
	void SetUserData(LPVOID lpUserData);
	LPVOID GetUserData();
	//{{
protected:
	CUITimerBase(void* pObject, void* pFn);
	void* GetFn(){return m_pFn;};
	void* GetObject(){return m_pObject;};
	virtual void OnTimer();
private:
	static void CALLBACK TimerWndProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTimer);
	void CalInterval();
	bool FinishLoop();
	void Count();
	void Init();
	bool m_bByStep;
	bool m_bDone;
	bool m_bPaused;
	bool m_bRunning;
	bool m_bReverse;
	bool m_bLoop;
	bool m_bRebound;
	LONG m_iInterval;
	LONG m_iTotalTimer;
	LONG m_iCurFrame;
	LONG m_iTotalFrame;
	void* m_pObject;
	void* m_pFn;
	DWORD m_dwLastTickcount;
	UINT m_uId;
	LPVOID m_lpUserData;
	bool m_bAsc;
	//}}
};

//{{
template <class T>
class CUITimer : public CUITimerBase
{
	typedef void (T::* Fn)(CUITimerBase*);
public:
	CUITimer(T* pObj, Fn pFn) : CUITimerBase(pObj, &pFn), m_pFn(pFn) { }
	CUITimer(const CUITimer& rhs) : CUITimerBase(rhs) { m_pFn = rhs.m_pFn; } 
protected:
	void OnTimer()
	{
		T* pObject = (T*) GetObject();
		return (pObject->*m_pFn)(this); 
	}  
	virtual CUITimerBase* Copy()
	{
		return new CUITimer(*this);
	}
private:
	Fn m_pFn;
};


template <class T>
CUITimer<T>  MakeDelegate(T* pObject, void (T::* pFn)(CUITimerBase*))
{
	return CUITimer<T>(pObject, pFn);
}
//}}
class DUI_API CTimerSource
{
	//{{
public:
	CTimerSource();
	~CTimerSource();
public:
	void operator+= (CUITimerBase& d);
	CUITimerBase* GetTimerBase();
	//}}
	void SetTimerParam(UINT iInterval,UINT iTotalTimer = 0, bool bLoop = false);
	void Start();
	void Stop();
	//{{
private:
	CUITimerBase* m_pTimerBase;
	//}}
};

template <class T>
CTimerSource* CreateUITimer(T* pObject, void (T::* pFn)(CUITimerBase*))
{
	CTimerSource* pTimerSource = new CTimerSource;
	*pTimerSource += MakeDelegate(pObject, pFn);
	return pTimerSource;
}

