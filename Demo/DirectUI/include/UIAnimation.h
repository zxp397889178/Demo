/*********************************************************************
 *       Copyright (C) 2010,Ӧ���������
 *********************************************************************
 *   Date             Name                 Description
 *   2010-09-27       xqb
*********************************************************************/
#pragma once
#ifndef __DIRECTUI_ANIMATION_H_
#define __DIRECTUI_ANIMATION_H_

/*!
    \brief    gifͼƬ��ʾ�ؼ�
	\note     ֧��gif��png
*****************************************/
class DUI_API CAnimationUI:public CControlUI
{
	UI_OBJECT_DECLARE(CAnimationUI, _T("Animation"))
public:
	CAnimationUI();
	virtual ~CAnimationUI();

public:
	void SetImage(LPCTSTR lpszImage);
	LPCTSTR GetImageFileName();

	ImageObj* GetImage();
	void SetImage(ImageObj* pImageObj);

	void SetLoop(bool bLoop);
	void StartAnimation();
	void StopAnimation();

	int  GetFrameCount();
	int  GetCurFrame();
	void SetCurFrame(int nCurFrame);
	//�̳к���
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void ReleaseImage();
	//{{
protected:
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);

private:
	ImageStyle* GetAnimationImageStyle();
	ImageStyle* m_pImageStyle;
	/// ��֡��
	UINT m_nFrameCount;
	/// ��ǰ��ʾ֡
	UINT m_nActiveFrame;
	/// ֡����ʾ���
	UINT m_nTimerElapse;
	int m_nFrameDelay;
	int m_nCurrFrameDelay;
	bool m_bLoop;
	bool m_bAutoStart;
	bool m_bRuning;
	//}}
};

#endif