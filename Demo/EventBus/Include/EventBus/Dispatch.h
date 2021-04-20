#pragma once

#define DeclareDispatchClass(cls, type)\
class EVENTBUS_API cls\
{\
protected:\
	virtual ~cls(){};\
public:\
	virtual void\
		Send(type) = 0;\
\
	virtual void\
		Sending(type) = 0;\
\
	virtual void\
		Post(type) = 0;\
\
	virtual void\
		PostInBackground(type) = 0;\
\
	virtual void\
		PostInWorkPool(type) = 0;\
};


DeclareDispatchClass(IEventDispatcher, IEvent* pEvent);
DeclareDispatchClass(IDispatcher, void);


class EVENTBUS_API IEventBus :
	public IEventDispatcher
{
protected:
	virtual ~IEventBus(){};

public:
	virtual void
		ConnectToBus(IObserver* pObserver) = 0;

	virtual	void
		DisconnectFromBus(IObserver* pObserver) = 0;
};
