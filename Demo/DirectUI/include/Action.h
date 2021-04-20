#pragma once


#define  IActionInterval CUITimerBase
/*!
    \brief    ͸���ȡ�λ�õĲ�������
*****************************************/
class DUI_API ActionInterval
{
public:
	static int FadeInterval(CViewUI* pView, CurveObj* pCurveObj, IActionInterval* pInterval, int startAlpha, int endAlpha);
	static RECT PosInterval(CViewUI* pView, CurveObj* pCurveObj, IActionInterval* pInterval, RECT& rcStart, RECT& rcEnd);
};


/*!
    \brief    ���������ӿ�
*****************************************/
class DUI_API IActionListener
{
public:
	/*!
	   \brief    ���������ص�
	   \param    CAction * pAction 
	   \param    IActionInterval* pActionInterval
	   \return   void 
	 ************************************/
	virtual void OnActionStep(CAction* pAction, IActionInterval* pActionInterval){};

	/*!
	   \brief    ״̬�л�ʱ�ص�
	   \note     ��ΪActionState_Finished״̬���ص�������֮������pAction
	   \param    CAction * pAction 
	   \return   void 
	 ************************************/
	virtual void OnActionStateChanged(CAction* pAction){}
};

/*!
    \brief    ��������
*****************************************/
class DUI_API CAction : public BaseObject
{
public:
	//{{
	friend class ActionManager;
	/// ״̬ö��
	typedef enum{
		ActionState_Ready = 0,		//׼��״̬
		ActionState_Running,		//������
		ActionState_Paused,			//��ͣ
		ActionState_Stop,			//ֹͣ��δ����ʱ��������
		ActionState_Finished,		//����״̬����ִ��������
		ActionState_Unknown = -1,
	}ActionState;
	//}}
public:

	/*!
	   \brief    ��ͣ
	   \return   void 
	 ************************************/
	void Paused();

	/*!
	   \brief    ����
	   \return   void 
	 ************************************/
	void Resume();

	/*!
	\brief    ֹͣ
	\return   void
	************************************/
	void Stop();

	/*!
	   \brief    ��ȡ״̬ 
	   \note     const 
	   \return   CAction::ActionState 
	 ************************************/
	CAction::ActionState GetState() const;

	/*!
	   \brief    ���ü���
	   \param    IActionListener * val 
	   \return   void 
	 ************************************/
	void SetActionListener(IActionListener* val);

	/*!
	   \brief    ����ִ��Ŀ�� 
	   \note     const 
	   \return   BaseObject* 
	 ************************************/
	BaseObject* ActionTarget() const;

	/*!
	   \brief    ���ö�������ID
	   \param    LPCTSTR val 
	   \return   void 
	 ************************************/
	void SetCurveID(LPCTSTR val);

	/*!
	   \brief    ��ȡ����������
	   \return   CurveObj* 
	 ************************************/
	CurveObj* GetCurveObj();

	/*!
	   \brief    ���ö�������
	   \param    UINT uInterval �������ʱ��
	   \param    UINT uTotalFrame ��֡��
	   \param    bool bLoop �Ƿ�ѭ��
	   \param    bool bRebound �Ƿ񷴵�
	   \param    bool bByStep true:����;false:ʵ��ʱ��
	   \return   void 
	 ************************************/
	void SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop = false, bool bRebound = false, bool bByStep = true);

	/*!
	   \brief    ���õ�ǰ֡
	   \param    DWORD dwCurFrame 
	   \param    bool bReverse ����
	   \return   void 
	 ************************************/
	void SetCurFrame(DWORD dwCurFrame, bool bReverse = false);

	/*!
	   \brief    ��ȡ��ǰ֡
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
	   \brief    ��������
	   \return   LPCTSTR 
	 ************************************/
	virtual LPCTSTR GetClass() = 0;

	/*!
	   \brief    ��������������״̬ΪActionState_Running
	   \param    CUITimerBase * pTimer 
	   \return   void 
	 ************************************/
	virtual void step(IActionInterval* pActionInterval) = 0;
	//{{
protected:

	/*!
	   \brief    ��ActionMangaget::addAction��������
	   \param    BaseObject * pActionTarget 
	   \return   void 
	 ************************************/
	virtual void startWithTarget(BaseObject* pActionTarget);

	/*!
	   \brief    �������������ָ���̬
	   \return   void 
	 ************************************/
	virtual void onStop(){}

	/*!
	   \brief    ����������������̬
	   \return   void 
	 ************************************/
	virtual void onFinished(){}

	/*!
	   \brief    ����״̬
	   \param    ActionState val 
	   \return   void 
	 ************************************/
	void SetState(CAction::ActionState val);

	/*!
	   \brief    Ŀ��
	   \param    BaseObject * val 
	   \return   void 
	 ************************************/
	void ActionTarget(BaseObject* val);

	CAction();
	/*!
	   \brief    ��������
	   \note     ��״̬����ΪActionState_Finished����ActionManager::removeAction
	   \return    
	 ************************************/
	virtual ~CAction();
	
private:
	static void CALLBACK ActionTimerCallback(CUITimerBase* pTimer);
	void update(IActionInterval* pActionInterval);

	/// ״̬
	ActionState m_stateAction;
	/// ��������
	CurveObj* m_pCurveObj;
	/// ����Ŀ��
	BaseObject* m_pActionTarget;
	/// ������
	IActionListener* m_pActionListener;
	/// ����
	IActionInterval* m_pActionInterval;
	/// 
	bool m_bFinishedOfRelease;
	//}}
};

