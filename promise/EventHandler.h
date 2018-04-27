#ifndef HANDLER_H
#define HANDLER_H

#include "Handle.h"
#include <sys/epoll.h>
#include <functional>

class EventHandler
{
public:
	typedef std::function<void()> CALLBACK;
public:
	EventHandler(Handle handle):
		m_handle(handle)
	{}

public:
	void SetReadCallback(const CALLBACK & func)
	{
		m_readFunc = func;	
	}
	void SetWriteCallback(const CALLBACK & func)
	{
		m_writeFunc = func;	
	}
	void SetCloseCallback(const CALLBACK & func)
	{
		m_closeFunc = func;
	}
	void SetErrorCallback(const CALLBACK & func)
	{
		m_errorFunc = func;
	}
	void SetReadCallback(CALLBACK && func)
	{
		m_readFunc = std::move(func);
	}
	void SetWriteCallback(CALLBACK && func)
	{
		m_writeFunc = std::move(func);
	}
	void SetCloseCallback(CALLBACK && func)
	{
		m_closeFunc = std::move(func);
	}
	void SetErrorCallback(CALLBACK && func)
	{
		m_errorFunc = std::move(func);
	}

	Handle & GetHandle()
	{
		return m_handle;
	}

	void HandleRead()
	{
		if (m_readFunc)
		{
			m_readFunc();
		}
	}

	void HandleWrite()
	{
		if (m_writeFunc)
		{
			m_writeFunc();
		}
	}

	void HandleClose()
	{
		if (m_closeFunc)
		{
			m_closeFunc();
		}
	}

	void HandleError()
	{
		if (m_errorFunc)
		{
			m_errorFunc();
		}
	}

private:
	Handle m_handle;
	CALLBACK m_readFunc;
	CALLBACK m_writeFunc;
	CALLBACK m_closeFunc;
	CALLBACK m_errorFunc;
};

#endif // !HANDLER_H