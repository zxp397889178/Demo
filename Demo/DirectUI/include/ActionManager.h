#pragma once

/*!
    \brief    ����������
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
	// ����Ԫ�صĶ���
	/*!
	   \brief    �Ƴ����ж���
	   \return   void 
	 ************************************/
	void removeAllActions();

	/*!
	   \brief    �ж�ָ���Ƿ��Ƴ�
	   \param    CAction * pAction 
	   \return   bool 
	 ************************************/
	bool isAction(CAction* pAction);

	/*!
	   \brief    ��Ӳ���ʼִ�ж���
	   \note     
	   \param    ViewAnimation * pAction 
	   \param    CViewUI * pView 
	   \param    bool bStopOther �Ƿ�ֹͣTarget������������������ֹͣ����
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
	   \brief    �Ƴ�����
	   \note     ��������ʱĬ�ϵ���
	   \param    ViewAnimation * pAction 
	   \return   void 
	 ************************************/
	void removeViewAnimation(ViewAnimation* pAction);

	/*!
	   \brief    ��������
	   \note     Target����ʱĬ��ִ��
	   \param    CViewUI * pTarget 
	   \param    bool bStop �Ƿ���ö���ֹͣ����
	   \return   void 
	 ************************************/
	void removeViewAnimationByTarget(CViewUI* pTarget, bool bStop = false);

	bool isViewAcioning(CViewUI* pTarget);
	
	//////////////////////////////////////////////////////////////////////////
	// ���ڶ���
	/*!
	   \brief    ��Ӳ���ʼִ�ж���
	   \note     
	   \param    WindowAnimation * pAction 
	   \param    CWindowUI * pView 
	   \param    bool bStopOther �Ƿ�ֹͣTarget������������������ֹͣ����
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
	   \brief    �Ƴ�����
	   \note     ��������ʱĬ�ϵ���
	   \param    WindowAnimation * pAction 
	   \return   void 
	 ************************************/
	void removeWindowAnimation(WindowAnimation* pAction);

	/*!
	   \brief    ��������
	   \note     Target����ʱĬ��ִ��
	   \param    CWindowUI * pTarget 
	   \param    bool bStop �Ƿ���ö���ֹͣ����
	   \return   void 
	 ************************************/
	void removeWindowAnimationByTarget(CWindowUI* pTarget, bool bStop = false);

	bool isWindowAcioning(CWindowUI* pTarget);

	void addCloseWindowAnimation(WindowAnimation* pWindowAnimation, CWindowUI* pWindow);

	WindowAnimation* getCloseWindowAnimation(CWindowUI* pWindow, bool bRemove = true);
protected:
private:
};

