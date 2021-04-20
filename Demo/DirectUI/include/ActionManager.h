#pragma once

/*!
    \brief    动画管理类
*****************************************/
class DUI_API ActionManager
{
public:
	//{{
	ActionManager();
	virtual ~ActionManager();
	//}}
	static ActionManager* GetInstance();
	static void ReleaseInstance();

	//////////////////////////////////////////////////////////////////////////
	// 可视元素的动画
	/*!
	   \brief    移除所有动画
	   \return   void 
	 ************************************/
	void removeAllActions();

	/*!
	   \brief    判断指针是否被移除
	   \param    CAction * pAction 
	   \return   bool 
	 ************************************/
	bool isAction(CAction* pAction);

	/*!
	   \brief    添加并开始执行动画
	   \note     
	   \param    ViewAnimation * pAction 
	   \param    CViewUI * pView 
	   \param    bool bStopOther 是否停止Target的其他动画，并调用停止函数
	   \return   void 
	 ************************************/
	void addViewAnimation(ViewAnimation* pAction, CViewUI* pView, bool bStopOther = true);

	/*!
	   \brief    
	   \note     
	   \param    ViewAnimation * pAction 
	   \param    CViewUI * pTarget 
	   \return   void 
	 ************************************/
	void addViewAnimationPair(ViewAnimation* pAction, CViewUI* pTarget);

	/*!
	   \brief    移除动画
	   \note     动画析构时默认调用
	   \param    ViewAnimation * pAction 
	   \return   void 
	 ************************************/
	void removeViewAnimation(ViewAnimation* pAction);

	/*!
	   \brief    析构动画
	   \note     Target析构时默认执行
	   \param    CViewUI * pTarget 
	   \param    bool bStop 是否调用动画停止函数
	   \return   void 
	 ************************************/
	void removeViewAnimationByTarget(CViewUI* pTarget, bool bStop = false);

	bool isViewAcioning(CViewUI* pTarget);
	
	//////////////////////////////////////////////////////////////////////////
	// 窗口动画
	/*!
	   \brief    添加并开始执行动画
	   \note     
	   \param    WindowAnimation * pAction 
	   \param    CWindowUI * pView 
	   \param    bool bStopOther 是否停止Target的其他动画，并调用停止函数
	   \return   void 
	 ************************************/
	void addWindowAnimation(WindowAnimation* pAction, CWindowUI* pView, bool bStopOther = true);

	/*!
	   \brief    
	   \note     
	   \param    WindowAnimation * pAction 
	   \param    CWindowUI * pTarget 
	   \return   void 
	 ************************************/
	void addWindowAnimationPair(WindowAnimation* pAction, CWindowUI* pTarget);

	/*!
	   \brief    移除动画
	   \note     动画析构时默认调用
	   \param    WindowAnimation * pAction 
	   \return   void 
	 ************************************/
	void removeWindowAnimation(WindowAnimation* pAction);

	/*!
	   \brief    析构动画
	   \note     Target析构时默认执行
	   \param    CWindowUI * pTarget 
	   \param    bool bStop 是否调用动画停止函数
	   \return   void 
	 ************************************/
	void removeWindowAnimationByTarget(CWindowUI* pTarget, bool bStop = false);

	bool isWindowAcioning(CWindowUI* pTarget);

	void addCloseWindowAnimation(WindowAnimation* pWindowAnimation, CWindowUI* pWindow);

	WindowAnimation* getCloseWindowAnimation(CWindowUI* pWindow, bool bRemove = true);
protected:
private:
};

