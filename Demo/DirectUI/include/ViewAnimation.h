#pragma once

/*!
    \brief    可视元素的动画，基类
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
    \brief    透明动画
	\note     针对Control、Style  
*****************************************/
class DUI_API AlphaChangeAnimation : public ViewAnimation
{
public:
	void SetKeyFrameAlpha(int startAlpha, int endAlpha);

	//继承函数
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
    \brief    位置动画
	\note     针对Control、Style  
*****************************************/
class DUI_API PosChangeAnimation : public ViewAnimation
{
public:
	void SetKeyFrameCoordinate(const Coordinate* pStartCoordinate, const Coordinate* pEndCoordinate);

	//继承函数
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
    \brief    序列动画
	\note     针对ImageStyle  
*****************************************/
class DUI_API ImageSequenceAnimation : public ViewAnimation
{
public:
	//继承函数
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
	//继承函数
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
    \brief    过场动画基类
	\note     针对控件中的两个图片样式
*****************************************/
class DUI_API TransitionAnimation : public ViewAnimation
{
public:
	//{{
	TransitionAnimation();
	//}}

	/*!
	   \brief    绑定
	   \param    CViewUI * pOut 
	   \param    CViewUI * pIn
	   \return   void 
	 ************************************/
	void BindObj(CViewUI* pOut, CViewUI* pIn);

	/*!
	   \brief    获取移出
	   \return   CViewUI* 
	 ************************************/
	CViewUI* GetOut();

	/*!
	   \brief    获取移入
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
    \brief    翻转动画
	\note     适用场景：控件下两个图片样式的旋转（两个图片位置重叠）
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
    \brief    滑动
	\note     适用场景：控件下两个图片样式的滑入滑出（位置为8|0,0,0,0）
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