#pragma once

class CAnimationCtrlUI;
class CUI_API CLoadingButtonUI :
	public CContainerUI
{
	UI_OBJECT_DECLARE(CLoadingButtonUI, _T("LoadingButton"))

public:
	CLoadingButtonUI();
	~CLoadingButtonUI();

public:
	virtual void	Start();
	virtual void	Stop();
	void			SetBkImage(LPCTSTR lptcsImage);

protected:
	virtual void	DoAction();
	virtual void	Init();

private:
	void			UpdateState(bool bLoading);
	bool			OnBtnNotify(TNotifyUI& notify);

private:
	bool			m_bFirstPaint;
	CControlUI*		m_pBtn;
	CContainerUI*	m_pLayLoading;
	CAnimationCtrlUI*	m_pLoading;
};