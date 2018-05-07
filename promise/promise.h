#ifndef PROMISE_H
#define PROMISE_H

#include <exception>
#include <functional>
#include <cassert>
#include "Core.h"
#include "Promise_help.h"
#include "NameSpace.h"

NAME_BEGIN

class Resolver;
class Rejector;

class Promise
{
public:
	Promise():
		m_pStorage(new Core())
	{}

	Promise(const Promise & rhs):
		m_pStorage(rhs.m_pStorage)
	{
		if (m_pStorage)
		{
			m_pStorage->IncRef();
		}
	}

	Promise(Promise && rhs):
		m_pStorage(rhs.m_pStorage)
	{
		rhs.m_pStorage = nullptr;
	}

	Promise & operator=(const Promise & rhs)
	{
		if (m_pStorage)
		{
			m_pStorage->DecRef();
		}
		m_pStorage = rhs.m_pStorage;
		if (m_pStorage)
		{
			m_pStorage->IncRef();
		}

		return *this;
	}

	Promise & operator=(Promise && rhs)
	{
		if (m_pStorage)
		{
			m_pStorage->DecRef();
		}
		m_pStorage = rhs.m_pStorage;
		rhs.m_pStorage = nullptr;

		return *this;
	}

	~Promise()
	{
		if (m_pStorage)
		{
			m_pStorage->DecRef();
		}
	}

public:
	//signature of func: void(Resolver &, Rejecter &)
	template<typename F, 
		std::enable_if_t<is_func_legal<F, Resolver, Rejector>::value, bool> = true>
	Promise(F && func):
		m_pStorage(new Core())
	{
		try
		{
			func(Resolver(m_pStorage), Rejector(m_pStorage));
		}
		catch (...)
		{
			m_pStorage->Reject(std::current_exception());
		}
	}

	//signature of func: void(Resolver &)	//never reject
	template<typename F, 
		std::enable_if_t<is_func_legal<F, Resolver>::value, bool> = true>
	Promise(F && func):
		m_pStorage(new Core())
	{
		try
		{
			func(Resolver(m_pStorage));
		}
		catch (...)
		{
			m_pStorage->Reject(std::current_exception());
		}
		
	}

	//signature of func: void(void)	//always reject by throwing exceptions
	template<typename F,
		std::enable_if_t<is_func_legal<F>::value, bool> = true>
	Promise(F && func):
		m_pStorage(new Core())
	{
		try
		{
			func();
		}
		catch (...)
		{
			m_pStorage->Reject(std::current_exception());
		}
	}
public:
	template<typename T>
	using remove_cvr_t = typename std::remove_cv<std::remove_reference<T>>::type;

	static void ResolvePromise(Promise & p, Promise && anotherP, bool)
	{
		anotherP.m_pStorage->AddCallBackFunc([p = p](Core::State state, Any value) mutable {
			try
			{
				switch (state)
				{
				case Core::FULFILLED:
					p.Resolve(std::move(value));
					break;
				case Core::REJECTED:
					p.Reject(std::move(value));
					break;
				default:
					assert("never reach here");
					break;
				}
			}
			catch (...)
			{
				p.Reject(std::current_exception());
			}
		});
	}

	template<typename T, typename std::enable_if_t<!std::is_same<remove_cvr_t<T>, Promise>::value, bool> = true>
	static void ResolvePromise(Promise & p, T && value, bool isFulfilled)
	{
		if (isFulfilled)
		{
			p.Resolve(std::forward<T>(value));
		}
		else
		{
			p.Reject(std::forward<T>(value));
		}
	}

public:
	//promise.then(onResolve,onReject)
	template<typename ResolveFunc, typename RejectFunc>
	Promise then(ResolveFunc && onResolve, RejectFunc && onReject)
	{
		static_assert(callable_with<ResolveFunc, Any>::value || callable_with<ResolveFunc>::value,
			"onResolve must looks like this: T onResolve(Any a) or T onResolve()");
		static_assert(callable_with<RejectFunc, std::exception_ptr>::value || callable_with<RejectFunc>::value,
			"onReject must looks like this: T onReject(std::exception_ptr ptr) or T onReject()");
		Promise p;
		m_pStorage->AddCallBackFunc([p, 
									onResolve = std::forward<ResolveFunc>(onResolve), 
									onReject = std::forward<ResolveFunc>(onReject)]
									(Core::State state, Any value) mutable
		{
			try
			{
				switch (state)
				{
				case Core::FULFILLED:
					ResolvePromise(p,
						invoker<ResolveFunc, Any>::invoke(std::move(onResolve), std::move(value)),
						true);
					break;
				case Core::REJECTED:
					ResolvePromise(p,
						invoker<RejectFunc, std::exception_ptr>::invoke(std::move(onReject), any_cast<std::exception_ptr>(value)),
						true);
					break;
				default:
					assert("never reach here");
					break;
				}
			}
			catch (...)
			{
				ResolvePromise(p, std::current_exception(), false);
			}
		});
		return p;
	}

	//promise.then(onResolve)
	template<typename ResolveFunc>
	Promise then(ResolveFunc && onResolve)
	{
		static_assert(callable_with<ResolveFunc, Any>::value || callable_with<ResolveFunc>::value,
			"onResolve must looks like this: T onResolve(Any a) or T onResolve()");
		Promise p;
		m_pStorage->AddCallBackFunc([p, onResolve = std::forward<ResolveFunc>(onResolve)]
			(Core::State state, Any value) mutable 
		{
			try
			{
				switch (state)
				{
				case Core::FULFILLED:
					ResolvePromise(p,
						invoker<ResolveFunc, Any>::invoke(std::move(onResolve), std::move(value)),
						true);
					break;
				case Core::REJECTED:
					ResolvePromise(p, std::move(value), false);
					break;
				default:
					assert("never reach here");
					break;
				}
			}
			catch (...)
			{
				ResolvePromise(p, std::current_exception(), false);
			}
		});
		return p;
	}

	//promise.then(onResolve,nullptr)
	template<typename ResolveFunc>
	Promise then(ResolveFunc && onResolve, std::nullptr_t)
	{
		return then(std::forward<ResolveFunc>(onResolve));
	}

	//promise.then(nullptr,onReject)
	template<typename RejectFunc>
	Promise then(std::nullptr_t , RejectFunc && onReject)
	{
		static_assert(callable_with<RejectFunc, std::exception_ptr>::value || callable_with<RejectFunc>::value,
			"onReject must looks like this: T onReject(std::exception_ptr ptr) or T onReject()");
		Promise p;
		m_pStorage->AddCallBackFunc([p = p,
			onReject = std::forward<RejectFunc>(onReject)
		](Core::State state, Any value) mutable {
			try
			{
				switch (state)
				{
				case Core::FULFILLED:
					ResolvePromise(p, std::move(value), true);
					break;
				case Core::REJECTED:
					ResolvePromise(p,
						invoker<RejectFunc, std::exception_ptr>::invoke(std::move(onReject), any_cast<std::exception_ptr>(value)),
						true);
					break;
				default:
					assert("never reach here");
					break;
				}
			}
			catch (...)
			{
				ResolvePromise(p, std::current_exception(), false);
			}
		});
		return p;
	}

	template<typename RejectFunc>
	Promise Catch(RejectFunc && onReject)
	{
		return then(nullptr, std::forward<RejectFunc>(onReject));
	}

public:
	template<typename T1>
	void Resolve(T1 && value)
	{
		if (!m_pStorage)
		{
			throw "empty promise";
		}
		m_pStorage->Accept(std::forward<T1>(value));
	}

	void Resolve()
	{
		if (!m_pStorage)
		{
			throw "empty promise";
		}
		m_pStorage->Accept(empty{});
	}

	template<typename T1>
	void Reject(T1 && value)
	{
		if (!m_pStorage)
		{
			throw "empty promise";
		}
		m_pStorage->Reject(std::forward<T1>(value));
	}

	void Reject()
	{
		if (!m_pStorage)
		{
			throw "empty promise";
		}
		m_pStorage->Reject(empty{});
	}
private:
	Core * m_pStorage;
};

class Resolver
{
public:
	Resolver(Core * p) :
		m_pCore(p)
	{
		m_pCore->IncRef();
	}
	~Resolver()
	{
		if (m_pCore)
		{
			m_pCore->DecRef();
		}
	}
	Resolver(Resolver && rhs):m_pCore(rhs.m_pCore)
	{
		rhs.m_pCore = nullptr;
	}
	Resolver & operator==(Resolver && rhs)
	{
		if (m_pCore)
		{
			m_pCore->DecRef();
		}
		m_pCore = rhs.m_pCore;
		rhs.m_pCore = nullptr;
		return *this;
	}

	Resolver(const Resolver & rhs)
	{
		rhs.m_pCore->IncRef();
		m_pCore = rhs.m_pCore;
	}

	Resolver & operator=(const Resolver & rhs)
	{
		if (m_pCore)
		{
			m_pCore->DecRef();
		}
		rhs.m_pCore->IncRef();
		m_pCore = rhs.m_pCore;
		return *this;
	}

public:
	template<typename T1>
	void operator()(T1 && value)
	{
		if (!m_pCore)
		{
			throw "resolver was moved";
		}
		m_pCore->Accept(std::forward<T1>(value));
	}
	
	void operator()()
	{
		if (!m_pCore)
		{
			throw "resolver was moved";
		}
		m_pCore->Accept(empty{});
	}

private:
	Core * m_pCore;
};

class Rejector
{
public:
	Rejector(Core * p) :
		m_pCore(p)
	{
		m_pCore->IncRef();
	}
	~Rejector() 
	{
		if (m_pCore)
		{
			m_pCore->DecRef();
		}
	}

	Rejector(Rejector && rhs) :m_pCore(rhs.m_pCore)
	{
		rhs.m_pCore = nullptr;
	}
	Rejector & operator==(Rejector && rhs)
	{
		if (m_pCore)
		{
			m_pCore->DecRef();
		}
		m_pCore = rhs.m_pCore;
		rhs.m_pCore = nullptr;
		return *this;
	}
	Rejector(const Rejector & rhs)
	{
		rhs.m_pCore->IncRef();
		m_pCore = rhs.m_pCore;
	}
	Rejector & operator=(const Rejector & rhs)
	{
		if (m_pCore)
		{
			m_pCore->DecRef();
		}
		rhs.m_pCore->IncRef();
		m_pCore = rhs.m_pCore;
		return *this;
	}

public:
	template<typename T1>
	void operator()(T1 && value)
	{
		if (!m_pCore)
		{
			throw "rejector was moved";
		}
		m_pCore->Reject(std::forward<T1>(value));
	}

	void operator()()
	{
		if (!m_pCore)
		{
			throw "rejector was moved";
		}
		m_pCore->Reject(empty{});
	}

private:
	Core * m_pCore;
};

NAME_END
#endif // !PROMISE_H