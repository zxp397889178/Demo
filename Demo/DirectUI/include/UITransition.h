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
	   \brief    ���ù��̶���
	   \note     ����ǰ�����pAnimation��SetCurveID SetParam������
	   \param    TransitionAnimation * pAnimation 
	   \return   void 
	 ************************************/
	void SetTransitionAnimation(TransitionAnimation* pAnimation);

	/*!
	   \brief    ��ȡָ��
	   \return   TransitionAnimation* 
	 ************************************/
	TransitionAnimation* GetTransitionAnimation();

	/*!
	   \brief    ִ�й�������
	   \param    ImageObj * pOutImageObj 
	   \param    ImageObj * pInImageObj 
	   \param    WPARAM wParam 
	   \param    LPARAM lParam 
	   \return   void 
	 ************************************/
	virtual void Transition(ImageObj* pOutImageObj, ImageObj* pInImageObj, WPARAM wParam, LPARAM lParam);

	/*!
	   \brief    �����Ƿ�������
	   \return   bool 
	 ************************************/
	bool IsRunning(){
		return m_pAnimation && m_pAnimation->GetState() == CAction::ActionState_Running;
	}
	//{{
protected:

	/*!
	   \brief    ��������ִ��ǰ�Ļص��ж�
	   \param    WPARAM wParam ����Transition��wParam
	   \param    LPARAM lParam ����Transition��lParam
	   \return   bool ����false����ͣ����
	 ************************************/
	virtual bool OnTransition(WPARAM wParam, LPARAM lParam){return true;}
private:

	TransitionAnimation* m_pAnimation;
	//}}
};

/*!
    \brief    �����µĹ���������װ
	\note     ��һ���ؼ���ʾ����Ϊ����������(��8|0,0,0,0)
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

	// �̳к���
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