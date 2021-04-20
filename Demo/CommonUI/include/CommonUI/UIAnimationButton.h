/************************************************************************** 
    *  @Copyright (c) 2017, ChenCY, All rights reserved. 
 
    *  @file     : UIAnimationButton.h 
    *  @version  : ver 1.0 
 
    *  @author   : ChenCY 
    *  @date     : 2017/08/02 11:17 
    *  @brief    : 鼠标移入移出动效 
**************************************************************************/

#pragma once

#include <vector>
#include <memory>

class CControlUI;
class CUI_API CUIAnimation
{
public:
    CUIAnimation(){};

    virtual void SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId){ m_pControl = pControl; };

    virtual void Pause(){};

    virtual void Resume(){};
protected:

    CControlUI *m_pControl = nullptr;
};

typedef std::shared_ptr<CUIAnimation> SP_ANIMATION;

class  CUI_API CAnimationManager
{
public:

    virtual ~CAnimationManager(){};
    template<class T>
    std::shared_ptr<T> AddAnimation(CControlUI *pControl, LPCTSTR lpszStyleId);

private:

    std::vector<SP_ANIMATION>  m_vectAnimation;
};

template<class T>
std::shared_ptr<T> CAnimationManager::AddAnimation(CControlUI *pControl, LPCTSTR lpszStyleId)
{
    auto spAnimation = std::make_shared<T>();
    spAnimation->SetAnimationControl(pControl, lpszStyleId);
    m_vectAnimation.push_back(spAnimation);
    return spAnimation;
}

class CUI_API CUIGifAnimation : public CUIAnimation
{
public:
    virtual ~CUIGifAnimation(){};
    virtual void SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId);

    virtual void Pause();

    virtual void Resume();

	virtual void Stop();

    virtual void SetFrameCount(int nCount);

	void         SetAnimationId(LPCTSTR lpctId);

	void		 SetCurFrame(DWORD dwCurFrame, bool bReverse = false);

	void		 SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop = false, bool bRebound = false, bool bByStep = true);

	void		 SetActionListener(IActionListener* val);
private:

    std::shared_ptr<ViewAnimation>  m_spViewAnimation;

    ImageStyle* m_pImageStyle = nullptr;
};


class CUI_API CUIMarqueeAnimation 
	: public CUIAnimation
{
public:
	CUIMarqueeAnimation();
	virtual ~CUIMarqueeAnimation(){};
	virtual void SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId);

	virtual void Pause();

	virtual void Resume();

	virtual void SetFrameCount(int nCount);

	void         SetAnimationInfo(int nTxtWidth);
	void         SetMoveRate(float fMoveRate){ m_fMoveRate = fMoveRate; }
	void         SetInterval(UINT uInterval){ m_uInterval = uInterval; }
	void         InitPosAnimation();
	bool         ShowAnimation();

private:
	std::shared_ptr<PosChangeAnimation>
				m_spPosAnimation;

	TextStyle*  m_pTextStyle;
	RECT		m_rtTxtOriginal;
	RECT		m_rtItem;
	int			m_nTxtWidth;
	int			m_nLastTxtWidth;
	float       m_fMoveRate;
	UINT		m_uInterval;
};

class CUI_API CUIAnimationMouseEnter : public CUIAnimation
{
public:
    virtual ~CUIAnimationMouseEnter();
    virtual void SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId);
	virtual	void Resume();
	void		 SetCurFrame(DWORD dwCurFrame, bool bReverse = false);
	void		 SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop = false, bool bRebound = false, bool bByStep = true);
private:

    virtual bool EventMouse(TEventUI& event);

    std::shared_ptr<ViewAnimation>  m_spViewAnimation;

    StyleObj* m_pImageStyle = nullptr;
};

class CUI_API CUITreeNodeAnimation : public CUIAnimation
{
public:
	virtual ~CUITreeNodeAnimation();
	virtual	void	SetAnimationControl(CControlUI *pControl, LPCTSTR lpszStyleId);

	virtual	void	Pause();

	virtual	void	Resume();
	void			OnStop();
	std::shared_ptr<ViewAnimation>  m_spViewAnimation;

	StyleObj* m_pImageStyle = nullptr;
};


enum ResAnimation{ InsertSuccess, SaveSuccess, InsertFail, DownloadFail, SaveFail}; //下载动画
class ImageSequenceAnimation;

class CUI_API CInsertResourseAnimation :
    public CLayoutUI,
    public IActionListener
{
    UI_OBJECT_DECLARE(CInsertResourseAnimation, _T("InsertResourseAnimation"));
public:
    CInsertResourseAnimation();
    virtual ~CInsertResourseAnimation();

    virtual void  Init();

    void ShowResult(ResAnimation resAniType);

    virtual void OnActionStateChanged(CAction* pAction);

    virtual bool Event(TEventUI& event);

private:

    std::shared_ptr<ImageSequenceAnimation>  m_spImageAnimation;

    CControlUI*                              m_pAnimationControl = nullptr;

    CControlUI*                              m_pResultLabel = nullptr;
};

class  CUI_API CAnimationManagerEx
{
public:

	virtual ~CAnimationManagerEx(){};
	template<class T>
	std::shared_ptr<T> AddAnimation(CControlUI *pControl, LPCTSTR lpszStyleId);


	std::shared_ptr<CUITreeNodeAnimation> GetTreeAnimationEx(CControlUI *pControl);

	bool RemoveAnimationEx(CControlUI* pControl);
private:

	std::map<CControlUI*, std::shared_ptr<CUITreeNodeAnimation>>  m_mapAnimation;
};

template<class T>
std::shared_ptr<T> CAnimationManagerEx::AddAnimation(CControlUI *pControl, LPCTSTR lpszStyleId)
{
	auto spAnimation = std::make_shared<T>();
	spAnimation->SetAnimationControl(pControl, lpszStyleId);
	m_mapAnimation.insert(make_pair(pControl, spAnimation));
	return spAnimation;
}