#pragma once

/*!
    \brief    ����Ԫ�صĶ���������
*****************************************/
class DUI_API ViewAnimation : public CAction
{
public:

	ViewAnimation();

	virtual ~ViewAnimation();
protected:
private:
};
/*!
    \brief    ͸������
	\note     ���Control��Style  
*****************************************/
class DUI_API AlphaChangeAnimation : public ViewAnimation
{
public:
	void SetKeyFrameAlpha(int startAlpha, int endAlpha);

	//�̳к���
	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void startWithTarget(BaseObject* pActionTarget);

	virtual void step(IActionInterval* pActionInterval);

	virtual void onStop();

	virtual void onFinished();
private:
	int m_nStartAlpha;
	int m_nEndAlpha;
	//}}
};

/*!
    \brief    λ�ö���
	\note     ���Control��Style  
*****************************************/
class DUI_API PosChangeAnimation : public ViewAnimation
{
public:
	void SetKeyFrameCoordinate(const Coordinate* pStartCoordinate, const Coordinate* pEndCoordinate);

	//�̳к���
	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void startWithTarget( BaseObject* pActionTarget );

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();

	Coordinate m_startCoordinate;
	Coordinate m_endCoordinate;
	//}}
};

/*!
    \brief    ���ж���
	\note     ���ImageStyle  
*****************************************/
class DUI_API ImageSequenceAnimation : public ViewAnimation
{
public:
	//�̳к���
	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void startWithTarget( BaseObject* pActionTarget );

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();
private:
	//}}
};

class DUI_API ImageRotateAnimation : public ViewAnimation
{
public:
	//�̳к���
	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void startWithTarget(BaseObject* pActionTarget);

	virtual void step(IActionInterval* pActionInterval);

	virtual void onStop();

	virtual void onFinished();
private:
	//}}
};

/*!
    \brief    ������������
	\note     ��Կؼ��е�����ͼƬ��ʽ
*****************************************/
class DUI_API TransitionAnimation : public ViewAnimation
{
public:
	//{{
	TransitionAnimation();
	//}}

	/*!
	   \brief    ��
	   \param    CViewUI * pOut 
	   \param    CViewUI * pIn
	   \return   void 
	 ************************************/
	void BindObj(CViewUI* pOut, CViewUI* pIn);

	/*!
	   \brief    ��ȡ�Ƴ�
	   \return   CViewUI* 
	 ************************************/
	CViewUI* GetOut();

	/*!
	   \brief    ��ȡ����
	   \return   CViewUI* 
	 ************************************/
	CViewUI* GetIn();
	//{{
protected:

	virtual void startWithTarget( BaseObject* pActionTarget );
private:
	/// 
	CViewUI* m_pOutView;
	/// 
	CViewUI* m_pInView;
	//}}
};

/*!
    \brief    ��ת����
	\note     ���ó������ؼ�������ͼƬ��ʽ����ת������ͼƬλ���ص���
*****************************************/
class DUI_API TurnTransition : public TransitionAnimation
{
public:

	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();
	//}}
};

/*!
    \brief    ����
	\note     ���ó������ؼ�������ͼƬ��ʽ�Ļ��뻬����λ��Ϊ8|0,0,0,0��
*****************************************/
class DUI_API SlideTransition : public TransitionAnimation
{
public:
	//{{
	SlideTransition();
	//}}
	typedef enum
	{
		SlideAnimation_Left = 0,
		SlideAnimation_Top,
		SlideAnimation_Right,
		SlideAnimation_Bottom,
		SlideAnimation_LeftTop,
		SlideAnimation_RightTop,
		SlideAnimation_LeftBottom,
		SlideAnimation_RightBottom,
	}SlideType;

	void SetSlideType(SlideType slideType);

	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();
private:
	SlideType m_SlideType;
	//}}
};

/*!
    \brief    
*****************************************/
class DUI_API FadeTransition : public TransitionAnimation
{
public:

	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();
private:
	//}}
};