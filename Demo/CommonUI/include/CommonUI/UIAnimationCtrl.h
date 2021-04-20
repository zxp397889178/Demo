/*********************************************************************
 *       Copyright (C) 2010,Ӧ���������
 *********************************************************************
 *   Date             Name                 Description
 *   2017-06-27       zlg
*********************************************************************/
#pragma once
/*!
    \brief    gifͼƬ��ʾ�ؼ�
	\note     ֧��gif��png
*****************************************/

class CUI_API IAnimationUI
{
public:
	virtual ~IAnimationUI(){};

public:
	virtual void	SetLoop(bool bLoop)			= 0;
	virtual void	StartAnimation()			= 0;
	virtual void	StopAnimation()				= 0;
	virtual int		GetFrameCount()				= 0;
	virtual int		GetCurFrame()				= 0;
	virtual void	SetCurFrame(int nCurFrame)	= 0;
	virtual void	SetFrameRate(int nRate)		= 0;
};

//CAnimationUI ��DirectUIռ�ã�����...

class CUI_API CAnimationCtrlUI :
	public CControlUI,
	public IAnimationUI
{
	UI_OBJECT_DECLARE(CAnimationCtrlUI, _T("AnimationCtrl"))
public:
	CAnimationCtrlUI();
	virtual ~CAnimationCtrlUI();

public:
	//set animate and visible
	virtual void	SetAnimatable(bool bStartAndShow);

	//implement IAnimationUI
	virtual void	SetLoop(bool bLoop);
	virtual void	StartAnimation();
	virtual void	StopAnimation();

	virtual int		GetFrameCount();
	virtual int		GetCurFrame();
	virtual void	SetCurFrame(int nCurFrame);
	virtual void	SetFrameRate(int nRate);

	//override CControlUI
	virtual void	SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

protected:
	virtual void	Init();
	virtual bool	IsAnimationValid();
	virtual ViewAnimation*
					AllocViewAnimation();
	virtual int		GetDefaultFrameCount();

protected:
	ImageStyle*		m_pImageStyle;
	ViewAnimation*	m_pAnimation;
	int				m_nFrameRate;
	int				m_nFrameDelay;
	int				m_nFrameCount;
	bool			m_bLoop;
	bool			m_bAutoStart;
};
