#ifndef HANDLER_H
#define HANDLER_H

#include <sys/epoll.h>
#include <functional>
#include <cassert>
#include <iostream>
#include "Log.h"

class EventLooper;

class EventHandler
{
	friend class EventLooper;
public:
	EventHandler(int fd, EventLooper * loop) :
		m_fd(fd),
		m_event(0),
		m_pLooper(loop)
	{}
	EventHandler(){}
	virtual ~EventHandler() {}

	void HandleEvent(uint32_t events)
	{
		if ((events & EPOLLHUP) && !(events & EPOLLIN))
		{
			HandleClose();
		}
		if (events & EPOLLERR)
		{
			HandleError();
		}
		if (events & (EPOLLIN | EPOLLPRI | EPOLLHUP))
		{
			HandleRead();
		}
		if (events & EPOLLOUT)
		{
			HandleWrite();
		}
	}

protected:
	void Update();
	void EnableRead()
	{
		m_event |= (EPOLLIN | EPOLLPRI);
		Update();
	}
	void DisableRead()
	{
		m_event &= ~(EPOLLIN | EPOLLPRI);
		Update();
	}
	void EnableWrite()
	{
		m_event |= EPOLLOUT;
		Update();
	}
	void DisableWrite()
	{
		m_event &= ~EPOLLOUT;
		Update();
	}
	bool IsInReading()
	{
		return m_event & EPOLLIN;
	}
	bool IsInWriting()
	{
		return m_event & EPOLLOUT;
	}
	virtual void HandleRead() 
	{
		perror("defalut handle read");
		Log << "read event come, no override, fd is " << m_fd << endl;
	}
	virtual void HandleWrite() 
	{
		perror("defalut handle write");
		Log << "write event come, no override, fd is " << m_fd << endl;
	}
	virtual void HandleClose() 
	{
		perror("defalut handle close");
		Log << "close event come, no override, fd is " << m_fd << endl;
	}
	virtual void HandleError() 
	{
		perror("defalut handle error");
		Log << "error event come, no override, fd is " << m_fd << endl;
	}

protected:
	int m_fd;
	uint32_t m_event;
	EventLooper * m_pLooper;
};

#endif // !HANDLER_H