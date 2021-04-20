#pragma once

//for publish event to the specify target
class EVENTBUS_API ITarget
{
protected:
	virtual ~ITarget(){};
public:
	virtual	bool		IsEqual(ITarget* pTarget) = 0;
};
