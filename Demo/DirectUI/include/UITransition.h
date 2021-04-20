#ifndef __DIRECTUI_TRANSITION_H_
#define __DIRECTUI_TRANSITION_H_

/*!
    \brief    
*****************************************/
class DUI_API ITransition : public IActionListener
{
public:
	//{{
	ITransition();
	//}}

	/*!
	   \brief    设置过程动画
	   \note     设置前需调用pAnimation的SetCurveID SetParam函数。
	   \param    TransitionAnimation * pAnimation 
	   \return   void 
	 ************************************/
	void SetTransitionAnimation(TransitionAnimation* pAnimation);

	/*!
	   \brief    获取指针
	   \return   TransitionAnimation* 
	 ************************************/
	TransitionAnimation* GetTransitionAnimation();

	/*!
	   \brief    执行过场动画
	   \param    ImageObj * pOutImageObj 
	   \param    ImageObj * pInImageObj 
	   \param    WPARAM wParam 
	   \param    LPARAM lParam 
	   \return   void 
	 ************************************/
	virtual void Transition(ImageObj* pOutImageObj, ImageObj* pInImageObj, WPARAM wParam, LPARAM lParam);

	/*!
	   \brief    动画是否运行中
	   \return   bool 
	 ************************************/
	bool IsRunning(){
		return m_pAnimation && m_pAnimation->GetState() == CAction::ActionState_Running;
	}
	//{{
protected:

	/*!
	   \brief    过场动画执行前的回调判断
	   \param    WPARAM wParam 来自Transition的wParam
	   \param    LPARAM lParam 来自Transition的lParam
	   \return   bool 返回false则暂停动画
	 ************************************/
	virtual bool OnTransition(WPARAM wParam, LPARAM lParam){return true;}
private:

	TransitionAnimation* m_pAnimation;
	//}}
};

/*!
    \brief    容器下的过场动画封装
	\note     仅一个控件显示，且为充满该容器(即8|0,0,0,0)
*****************************************/
class DUI_API CTransitionUI : public CContainerUI, public ITransition
{
	UI_OBJECT_DECLARE(CTransitionUI, _T("Transition"))
public:
	//{{
	CTransitionUI();
	virtual ~CTransitionUI();

	int	GetCurSel();
	virtual bool SelectItem(CControlUI* pControl);

	// 继承函数
	virtual void Init();
	virtual bool Remove(CControlUI* pControl);
	virtual void RemoveAll();
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	virtual void OnActionStateChanged( CAction* pAction );
	//{{
protected:

	virtual bool OnTransition( WPARAM wParam, LPARAM lParam );
private:
	tstring		m_szCurSel;
	CControlUI* m_pCurSel;
	//}}
};

#endif