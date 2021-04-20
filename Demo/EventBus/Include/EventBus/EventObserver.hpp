#pragma once

//the event observer base

template<class E>
class TEventObserverBase:
	public IObserver
{
public:
	TEventObserverBase();
	~TEventObserverBase();

protected:
	virtual	ITarget*	AsTarget();
	LPCSTR				GetEventId();
	virtual	bool		IsAsyc();
	virtual void		Update(void* pArgs);
};

template<class E>
void TEventObserverBase<E>::Update(void* pArgs)
{

}

template<class E>
bool TEventObserverBase<E>::IsAsyc()
{
	return false;
}

template<class E>
ITarget* TEventObserverBase<E>::AsTarget()
{
	return NULL;
}

template<class E>
LPCSTR TEventObserverBase<E>::GetEventId()
{
	return typeid(E).raw_name();
}

template<class E>
TEventObserverBase<E>::TEventObserverBase()
{
	if (GetEventbus())
	{
		GetEventbus()->ConnectToBus(this);
	}
}

template<class E>
TEventObserverBase<E>::~TEventObserverBase()
{
	if (GetEventbus())
	{
		GetEventbus()->DisconnectFromBus(this);
	}
}

//////////////////////////////////////////////////////////////////////////

template<class E>
class CEventObserver:
	virtual public TEventObserverBase<E>
{
protected:
	bool				IsAsyc();
	void				Update(void* pArgs);
	virtual void		OnEvent(E*  pEvent) = 0;

};

template<class E>
bool CEventObserver<E>::IsAsyc()
{
	return false;
}	

template<class E>
void CEventObserver<E>::Update( void* pArgs )
{
	this->OnEvent((E*)pArgs);
}

//////////////////////////////////////////////////////////////////////////

template<class E>
class CAsycEventObserver:
	public TEventObserverBase<E>
{
protected:
	bool				IsAsyc();
	void				Update(void* pArgs);
	virtual void		OnEventInBackground(const E* pEvent) = 0;

};

template<class E>
bool CAsycEventObserver<E>::IsAsyc()
{
	return true;
}

template<class E>
void CAsycEventObserver<E>::Update( void* pArgs )
{
	this->OnEventInBackground((E*)pArgs);
}

template<class E, typename ID>
class CTargetEventObserver :
	public CEventObserver<E>,
	virtual public CTargetBase<ID>
{
protected:
	virtual	ITarget*	AsTarget();

};

template<class E, typename ID>
ITarget* CTargetEventObserver<E, ID>::AsTarget()
{
	return static_cast<ITarget*>(this);
}

//////////////////////////////////////////////////////////////////////////

template<class E, typename ID>
class CIndependentTargetEventObserver :
	public CEventObserver<E>,
	public CEventTargetBase<E, ID>
{
protected:
	virtual	ITarget*	AsTarget();

};

template<class E, typename ID>
ITarget* CIndependentTargetEventObserver<E, ID>::AsTarget()
{
	return static_cast<ITarget*>(this);
}
