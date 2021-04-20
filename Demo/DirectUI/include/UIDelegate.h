#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

////////////////////////////////////////////////////////////////////////////////////////
class  DUI_API CDelegateBase
{
public:
	CDelegateBase(void* pObject, void* pFn);
	CDelegateBase(const CDelegateBase& rhs);
	virtual ~CDelegateBase();
	bool Equals(const CDelegateBase& rhs) const;

	void* GetFn();
	void* GetObject();
	bool operator() (void* param);

	virtual bool Invoke(void* param) = 0;

	virtual CDelegateBase* Copy() = 0;
private:
	void* m_pObject;
	void* m_pFn;
	//}}
};

template <class T>
class CDelegate : public CDelegateBase
{
	typedef bool (T::* Fn)(TUserEventUI*);
public:
	CDelegate(T* pObj, Fn pFn) : CDelegateBase(pObj, &pFn), m_pFn(pFn) { }
	CDelegate(const CDelegate& rhs) : CDelegateBase(rhs) { m_pFn = rhs.m_pFn; } 
protected:
	virtual bool Invoke(void* param)
	{
		T* pObject = (T*) GetObject();
		return (pObject->*m_pFn)(((TUserEventUI*)param)); 
	}  

	virtual CDelegateBase* Copy()
	{
		return new CDelegate(*this);
	}
private:
	Fn m_pFn;
};


class CDelegateStatic: public CDelegateBase
{
	typedef bool (* Fn)(TUserEventUI*);
public:
	CDelegateStatic(Fn pFn) : CDelegateBase(NULL, pFn) { } 
	CDelegateStatic(const CDelegateStatic& rhs) : CDelegateBase(rhs) { } 
	virtual CDelegateBase* Copy() { return new CDelegateStatic(*this); }

protected:
	virtual bool Invoke(void* param)
	{
		Fn pFn = (Fn)GetFn();
		return (*pFn)(((TUserEventUI*)param)); 
	}
};


//{{
template <class T, typename P>
class CEventDelegate : public CDelegateBase
{
	typedef bool (T::* Fn)(P&);
public:
	CEventDelegate(T* pObj, Fn pFn) : CDelegateBase(pObj, &pFn), m_pFn(pFn) { }
	CEventDelegate(const CEventDelegate& rhs) : CDelegateBase(rhs) { m_pFn = rhs.m_pFn; } 
protected:
	virtual bool Invoke(void* param)
	{
		T* pObject = (T*) GetObject();
		return (pObject->*m_pFn)(*((P*)param));
	}  

	virtual CDelegateBase* Copy()
	{
		return new CEventDelegate(*this);
	}
private:
	Fn m_pFn;
};

template <class T>
CDelegate<T>  MakeDelegate(T* pObject, bool (T::* pFn)(TUserEventUI*))
{
	return CDelegate<T>(pObject, pFn);
}


inline CDelegateStatic MakeDelegate(bool (*pFn)(TUserEventUI*))
{
	return CDelegateStatic(pFn); 
}



template <class T, typename P>
CEventDelegate<T, P>  MakeDelegate(T* pObject, bool (T::* pFn)(P&))
{
	return CEventDelegate<T, P>(pObject, pFn);
}

class DUI_API CEventSource
{
	//{{
	typedef bool (*FnType)(TUserEventUI*);
public:
	CEventSource();
	~CEventSource();
	//}}
	int GetCount();

	CDelegateBase* GetItem(int nIndex);
	//{{
	void operator+= (CDelegateBase& d);
	void operator-= (CDelegateBase& d);
	void Insert (int index, CDelegateBase& d);

	 void operator+= (FnType pFn);
	 void operator-= (FnType pFn);
	//}}
	bool operator() (void* param);


protected:
	//{{
	CStdPtrArray m_aDelegates;
	//}}
};
///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __UIDELEGATE_H__