#pragma once

class EVENTBUS_API IEvent
{
public:
	virtual ~IEvent(){};

public:
	virtual LPCSTR			GetId()		= 0;
	virtual IEvent*			Copy()		= 0;
	virtual	ITarget*		GetTarget() = 0;
};
