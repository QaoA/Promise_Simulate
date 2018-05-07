#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

using std::cerr;
using std::endl;

class Epoller
{
public:
	Epoller()
	{
		m_epollFd = epoll_create(1);
		if (m_epollFd < 0)
		{
			perror("error : ");
		}
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
		if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &e) < 0)
		{
			cerr << "error in epoll add" << endl;
			perror("error : ");
		}
	}

	void Remove(int fd)
	{
		if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
		{
			cerr << "error in epoll del" << endl;
		}
	}

	void Modify(int fd, epoll_event e)
	{
		if (epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &e) < 0)
		{
			cerr << "error in epoll mod" << endl;
		}
	}

private:
	int m_epollFd;
};

#endif // !EPOLLER_H