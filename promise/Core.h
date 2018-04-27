#ifndef CORE_H
#define CORE_H

#include "Any.h"
#include "NameSpace.h"
#include <atomic>
#include <queue>

NAME_BEGIN

struct Core
{
	friend class Promise;
	enum State
	{
		PENDING,
		FULFILLED,
		REJECTED
	};

public:
	Core():m_refCount(1),m_state(PENDING){}
public:
	void IncRef()
	{
		m_refCount++;
	}
	void DecRef()
	{
		m_refCount--;
		if (m_refCount <= 0)
		{
			delete this;
		}
	}
	void SetState(State s)
	{
		m_state = s;
	}
	State GetState()
	{
		return m_state;
	}

	template<typename F>
	void AddCallBackFunc(F && func)
	{
		m_callbackLists.emplace(std::forward<F>(func));
		MayRunCallBackFunctions();
	}

	template<typename T1>
	void Accept(T1 && value)
	{
		SetState(FULFILLED);
		m_value.emplace<T1>(std::forward<T1>(value));
		MayRunCallBackFunctions();
	}

	void Accept(Any && any)
	{
		SetState(FULFILLED);
		m_value = std::move(any);
		MayRunCallBackFunctions();
	}

	template<typename T1>
	void Reject(T1 && value)
	{
		SetState(REJECTED);
		m_value.emplace<std::exception_ptr>(std::make_exception_ptr(std::forward<T1>(value)));
		MayRunCallBackFunctions();
	}

	void Reject(std::exception_ptr ptr)
	{
		SetState(REJECTED);
		m_value.emplace<std::exception_ptr>(ptr);
		MayRunCallBackFunctions();
	}

	void Reject(Any && any)
	{
		SetState(REJECTED);
		m_value = std::move(any);
		MayRunCallBackFunctions();
	}

	Any GetValue()
	{
		return m_value;
	}

private:
	void MayRunCallBackFunctions()
	{
		State s = GetState();
		if (s == FULFILLED || s == REJECTED)
		{
			while (!m_callbackLists.empty())
			{
				auto func = m_callbackLists.front();
				m_callbackLists.pop();
				func(m_state, m_value);
			}
		}
	}

private:
	Any m_value;
	int m_refCount;
	std::queue<std::function<void(State, Any)>> m_callbackLists;
	State m_state;
};

NAME_END

#endif // !CORE_H

