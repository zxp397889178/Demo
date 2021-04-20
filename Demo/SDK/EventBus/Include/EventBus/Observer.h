#pragma once

class EVENTBUS_API IObserver
{
protected:
	virtual ~IObserver(){};
public:
	virtual void			Update(void* pArgs)		= 0;
	//is it non-UI observer
	virtual	bool			IsAsyc()				= 0;
	virtual LPCSTR			GetEventId()			= 0;
	virtual	ITarget*		AsTarget()				= 0;
};
