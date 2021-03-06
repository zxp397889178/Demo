#pragma once


#define  IActionInterval CUITimerBase
/*!
    \brief    透明度、位置的步进处理
*****************************************/
class DUI_API ActionInterval
{
public:
	static int FadeInterval(CViewUI* pView, CurveObj* pCurveObj, IActionInterval* pInterval, int startAlpha, int endAlpha);
	static RECT PosInterval(CViewUI* pView, CurveObj* pCurveObj, IActionInterval* pInterval, RECT& rcStart, RECT& rcEnd);
};


/*!
    \brief    动画监听接口
*****************************************/
class DUI_API IActionListener
{
public:
	/*!
	   \brief    动画步进回调
	   \param    CAction * pAction 
	   \param    IActionInterval* pActionInterval
	   \return   void 
	 ************************************/
	virtual void OnActionStep(CAction* pAction, IActionInterval* pActionInterval){};

	/*!
	   \brief    状态切换时回调
	   \note     当为ActionState_Finished状态，回调本函数之后将析构pAction
	   \param    CAction * pAction 
	   \return   void 
	 ************************************/
	virtual void OnActionStateChanged(CAction* pAction){}
};

/*!
    \brief    动画基类
*****************************************/
class DUI_API CAction : public BaseObject
{
public:
	//{{
	friend class ActionManager;
	/// 状态枚举
	typedef enum{
		ActionState_Ready = 0,		//准备状态
		ActionState_Running,		//运行中
		ActionState_Paused,			//暂停
		ActionState_Stop,			//停止（未结束时被析构）
		ActionState_Finished,		//结束状态（会执行析构）
		ActionState_Unknown = -1,
	}ActionState;
	//}}
public:

	/*!
	   \brief    暂停
	   \return   void 
	 ************************************/
	void Paused();

	/*!
	   \brief    继续
	   \return   void 
	 ************************************/
	void Resume();

	/*!
	\brief    停止
	\return   void
	************************************/
	void Stop();

	/*!
	   \brief    获取状态 
	   \note     const 
	   \return   CAction::ActionState 
	 ************************************/
	CAction::ActionState GetState() const;

	/*!
	   \brief    设置监听
	   \param    IActionListener * val 
	   \return   void 
	 ************************************/
	void SetActionListener(IActionListener* val);

	/*!
	   \brief    动画执行目标 
	   \note     const 
	   \return   BaseObject* 
	 ************************************/
	BaseObject* ActionTarget() const;

	/*!
	   \brief    设置动画曲线ID
	   \param    LPCTSTR val 
	   \return   void 
	 ************************************/
	void SetCurveID(LPCTSTR val);

	/*!
	   \brief    获取动画曲线类
	   \return   CurveObj* 
	 ************************************/
	CurveObj* GetCurveObj();

	/*!
	   \brief    设置动画参数
	   \param    UINT uInterval 间隔毫秒时间
	   \param    UINT uTotalFrame 总帧数
	   \param    bool bLoop 是否循环
	   \param    bool bRebound 是否反弹
	   \param    bool bByStep true:步进;false:实际时间
	   \return   void 
	 ************************************/
	void SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop = false, bool bRebound = false, bool bByStep = true);

	/*!
	   \brief    设置当前帧
	   \param    DWORD dwCurFrame 
	   \param    bool bReverse 逆序
	   \return   void 
	 ************************************/
	void SetCurFrame(DWORD dwCurFrame, bool bReverse = false);

	/*!
	   \brief    获取当前帧
	   \note     
	   \return   DWORD 
	 ************************************/
	DWORD GetCurFrame();

	DWORD GetTotalFrame();

	bool IsLoop() const;

	bool IsReverse() const;

	void FinishedOfRelease(bool val);
	bool IsFinishedOfRelease();
	/*!
	   \brief    动画别名
	   \return   LPCTSTR 
	 ************************************/
	virtual LPCTSTR GetClass() = 0;

	/*!
	   \brief    动画步进函数，状态为ActionState_Running
	   \param    CUITimerBase * pTimer 
	   \return   void 
	 ************************************/
	virtual void step(IActionInterval* pActionInterval) = 0;
	//{{
protected:

	/*!
	   \brief    由ActionMangaget::addAction函数调用
	   \param    BaseObject * pActionTarget 
	   \return   void 
	 ************************************/
	virtual void startWithTarget(BaseObject* pActionTarget);

	/*!
	   \brief    非正常结束，恢复初态
	   \return   void 
	 ************************************/
	virtual void onStop(){}

	/*!
	   \brief    正常结束，设置终态
	   \return   void 
	 ************************************/
	virtual void onFinished(){}

	/*!
	   \brief    设置状态
	   \param    ActionState val 
	   \return   void 
	 ************************************/
	void SetState(CAction::ActionState val);

	/*!
	   \brief    目标
	   \param    BaseObject * val 
	   \return   void 
	 ************************************/
	void ActionTarget(BaseObject* val);

	CAction();
	/*!
	   \brief    析构函数
	   \note     当状态设置为ActionState_Finished或者ActionManager::removeAction
	   \return    
	 ************************************/
	virtual ~CAction();
	
private:
	static void CALLBACK ActionTimerCallback(CUITimerBase* pTimer);
	void update(IActionInterval* pActionInterval);

	/// 状态
	ActionState m_stateAction;
	/// 动画曲线
	CurveObj* m_pCurveObj;
	/// 动画目标
	BaseObject* m_pActionTarget;
	/// 监听类
	IActionListener* m_pActionListener;
	/// 计数
	IActionInterval* m_pActionInterval;
	/// 
	bool m_bFinishedOfRelease;
	//}}
};

