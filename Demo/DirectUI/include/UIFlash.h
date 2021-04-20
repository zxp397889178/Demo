#pragma once

class CFlashImpl;
class CFlashDispatch;

/*!
    \brief    flash控件
*****************************************/
class DUI_API CFlashUI : public CControlUI
{
	//{{
	friend class CFlashImpl;
	friend class CFlashDispatch;
	UI_OBJECT_DECLARE(CFlashUI, _T("Flash"))
public:
	CFlashUI();
	virtual ~CFlashUI();
	//}}

	HRESULT RegisterEventHandler( bool inAdvise );
	void Show( LPCTSTR lpszUrl );
	HWND GetHostWindow();
	void SetFlashEventEnabled(bool bEnable = true /* bool bEnable = true*/) {m_bFlashEvent = bEnable;};//开启Flash事件响应
	bool GetFlashEventStaus() {return m_bFlashEvent;}
	virtual void Init();
	//{{

protected:
	void InitControl();
	bool DoCreateControl();
	void ReleaseControl();
	//Control继承函数
	void SetRect(RECT& rectRegion);
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual HRESULT OnReadyStateChange (long newState);
	virtual HRESULT OnProgress(long percentDone );
	virtual HRESULT FSCommand (LPCTSTR command, LPCTSTR args);
	virtual HRESULT FlashCall (LPCTSTR request );
	CFlashImpl* m_pFlashImpl;
	CFlashDispatch* m_pFlashDispatch;
	bool m_bCreated;
	DWORD m_dwCookie;
	bool m_bFlashEvent;
	//}}
};

