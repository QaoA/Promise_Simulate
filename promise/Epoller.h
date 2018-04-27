#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <unistd.h>
#include "EventHandler.h"

class Epoller
{
public:
	Epoller()
	{
		m_epollFd = epoll_create1(0);
	}

	~Epoller()
	{
		close(m_epollFd);
	}

	int PollEvent(int maxEvents, epoll_event * eventArray)
	{
		if (!eventArray || maxEvents <= 0)
		{
			return 0;
		}

		int num = epoll_wait(m_epollFd, eventArray, maxEvents, -1);
		return num;
	}

	void Add(int fd, epoll_event e)
	{
		epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &e);
	}

	void Remove(int fd)
	{
		epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL);
	}

	void Update(int fd, epoll_event e)
	{
		epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &e);
	}

private:
	int m_epollFd;
};

#endif // !EPOLLER_H