#pragma once


template<typename ID>
class __declspec(dllexport) CIdentifyTarget :
	public ITarget
{
public:
	virtual ~CIdentifyTarget(){};
public:
	virtual	ID			GetIdentity()	= 0;

private:
	virtual bool		IsEqual(ITarget* pTarget);
};

template<typename ID>
bool CIdentifyTarget<ID>::IsEqual(ITarget* pTarget)
{
	CIdentifyTarget<ID>* pIdedTarget = (CIdentifyTarget<ID>*)pTarget;
	if (this->GetIdentity() == pIdedTarget->GetIdentity())
	{
		return true;
	}

	return false;
}

//share target id for any event observer at the same object
template<typename ID>
class __declspec(dllexport) CTargetBase :
	public CIdentifyTarget<ID>
{
public:
	virtual ~CTargetBase(){};
protected:
	virtual	ID			GetTargetId()			= 0;

private:
	ID					GetIdentity();
};

template<typename ID>
ID CTargetBase<ID>::GetIdentity()
{
	return GetTargetId();
}


//specify event target id
template<typename E, typename ID>
class CEventTargetBase :
	public CIdentifyTarget<ID>
{
public:
	virtual ~CEventTargetBase(){};
protected:
	//pUnusedEvent£¬ the param not yet implemented
	virtual	ID			GetEventTargetId(E* pUnusedEvent) = 0;
private:
	ID					GetIdentity();
};

template<typename E, typename ID>
ID CEventTargetBase<E, ID>::GetIdentity()
{
	return GetEventTargetId(NULL);
}
