#pragma once

//publish events to any observers

template<class M>
class CEventPublisher:
	public IEvent,
	public IDispatcher
{
public:
	//event on main thread
	virtual void			Post();
	virtual void			Send();

	//event on work thread
	//callback on publish event thread self except the main thread
	virtual void			Sending();

	//callback on a separate thread, for a long time or exclusive biz
	virtual void			PostInBackground();

	//callback on a thread pool, for frequently requested biz
	virtual void			PostInWorkPool();

	virtual LPCSTR			GetId();
protected:
	virtual IEvent*			Copy();
	virtual	ITarget*		GetTarget();
};

template<class M>
ITarget* CEventPublisher<M>::GetTarget()
{
	return NULL;
}

template<class M>
LPCSTR CEventPublisher<M>::GetId()
{
	return typeid(M).raw_name();
}

template<class M>
IEvent* CEventPublisher<M>::Copy()
{
	return new_nothrow M(*(M*)this);
}

template<class M>
void CEventPublisher<M>::Post()
{
	if (GetEventbus())
	{
		GetEventbus()->Post(this);
	}
}

template<class M>
void CEventPublisher<M>::Send()
{
	if (GetEventbus())
	{
		GetEventbus()->Send(this);
	}
}

template<class M>
void CEventPublisher<M>::Sending()
{
	if (GetEventbus())
	{
		GetEventbus()->Sending(this);
	}
}

template<class M>
void CEventPublisher<M>::PostInBackground()
{
	if (GetEventbus())
	{
		GetEventbus()->PostInBackground(this);
	}
}

template<class M>
void CEventPublisher<M>::PostInWorkPool()
{
	if (GetEventbus())
	{
		GetEventbus()->PostInWorkPool(this);
	}
}

template<class M, typename ID>
class CTargetEventPublisher :
	public IEvent,
	public CIdentifyTarget<ID>
{

public:
	virtual ~CTargetEventPublisher(){ };
	
public:
	//event on main thread
	virtual void			PostToTarget(ID targetId);
	virtual void			SendToTarget(ID targetId);
	virtual LPCSTR			GetId();

protected:
	virtual ITarget*		GetTarget();
	virtual IEvent*			Copy();
	virtual ID				GetIdentity();

private:
	ID	m_tId;
};

template<class M, typename ID>
ID CTargetEventPublisher<M, ID>::GetIdentity()
{
	return m_tId;
}

template<class M, typename ID>
ITarget* CTargetEventPublisher<M, ID>::GetTarget()
{
	return static_cast<ITarget*>(this);
}

template<class M, typename ID>
LPCSTR CTargetEventPublisher<M, ID> ::GetId()
{
	return typeid(M).raw_name();
}

template<class M, typename ID>
IEvent* CTargetEventPublisher<M, ID>::Copy()
{
	return new_nothrow M(*(M*)this);
}

template<class M, typename ID>
void CTargetEventPublisher<M, ID>::PostToTarget(ID targetId)
{
	m_tId = targetId;
	if (GetEventbus())
	{
		GetEventbus()->Post(this);
	}
}

template<class M, typename ID>
void CTargetEventPublisher<M, ID>::SendToTarget(ID targetId)
{
	m_tId = targetId;
	if (GetEventbus())
	{
		GetEventbus()->Send(this);
	}
}

