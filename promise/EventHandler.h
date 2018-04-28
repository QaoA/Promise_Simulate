#ifndef HANDLER_H
#define HANDLER_H

#include <sys/epoll.h>
#include <functional>
#include <cassert>

class EventLooper;

class EventHandler
{
public:
	typedef std::function<void(EventHandler * thisHandler)> CALLBACK;

public:
	void HandleEvent(uint32_t events)
	{
		if ((events & EPOLLHUP) && !(events & EPOLLIN))
		{
			if (m_closeFunc)
			{
				m_closeFunc(this);
			}
		}
		if (events & EPOLLERR)
		{
			if (m_errorFunc)
			{
				m_errorFunc(this);
			}
		}
		if (events & (EPOLLIN | EPOLLPRI | EPOLLHUP))
		{
			if (m_readFunc)
			{
				m_readFunc(this);
			}
		}
		if (events & EPOLLOUT)
		{
			if (m_writeFunc)
			{
				m_writeFunc(this);
			}
		}
	}

public:
	int m_fd;
	epoll_event m_event;
	EventLooper * m_pLooper;
	CALLBACK m_readFunc;
	CALLBACK m_writeFunc;
	CALLBACK m_closeFunc;
	CALLBACK m_errorFunc;
};

#endif // !HANDLER_H