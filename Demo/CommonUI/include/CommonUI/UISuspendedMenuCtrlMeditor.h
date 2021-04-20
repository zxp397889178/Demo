#pragma once

//��ʱ���ܲμ�CommonUI���̱��룬��������ñ���
class CSuspendedMenuCtrlMeditorUI :
	public CControlMediatorUI,
	public CWindowPtrHolder,
	public CTargetEventObserver<RequestCreateSuspendedMenuToTarget, CControlUI*>,
	public CTargetEventObserver<RequestPointHitTestToCtrl, CControlUI*>
{
public:
	CSuspendedMenuCtrlMeditorUI();
	virtual ~CSuspendedMenuCtrlMeditorUI();

public:
	virtual void	OnCreateMenu(std::shared_ptr<IWindowPtrMap> spWindowPtrMap, RequestCreateSuspendedMenuToTarget* pEvent);

private:
	virtual bool	OnCtrlEvent(TEventUI& msg);
	virtual void	OnEvent(RequestCreateSuspendedMenuToTarget* pEvent);
	virtual void	OnEvent(RequestPointHitTestToCtrl* pEvent);

	virtual CControlUI*
					GetTargetId();
};

