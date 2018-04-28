#include "EventLooper.h"
#include "NetException.h"

EventLooper::EventLooper() :
	m_epoller(),
	m_running(false)
{
}

EventLooper::~EventLooper()
{
}

void EventLooper::RegisterHandler(EventHandler & handler)
{
	handler.m_pLooper = this;
	handler.m_event.data.ptr = &handler;
	m_epoller.Add(handler.m_fd, handler.m_event);
	m_handlerList.insert(std::make_pair(handler.m_fd,&handler));
}

void EventLooper::RemoveHandler(EventHandler & handler)
{
	if (handler.m_pLooper == this)
	{
		int fd = handler.m_fd;
		m_epoller.Remove(fd);
		m_handlerList.erase(fd);
		handler.m_pLooper = nullptr;
	}
}

void EventLooper::UpdateHandler(EventHandler & handler)
{
	assert(handler.m_pLooper == this);
	int fd = handler.m_fd;
	if (m_handlerList.find(fd) != m_handlerList.end())
	{
		m_epoller.Modify(fd, handler.m_event);
	}
	else
	{
		m_epoller.Add(fd, handler.m_event);
	}
}

void EventLooper::StartLoop(OnEventFunc func)
{
	RegisterHandler(m_stoper.GetHandler());
	epoll_event events[maxPollEventSize];
	while (m_running)
	{
		int num = m_epoller.PollEvent(maxPollEventSize, events);
		if (num > 0)
		{
			for (int i = 0; i < num; ++i)
			{
				func(static_cast<EventHandler *>(events[i].data.ptr), events[i].events);
			}
		}
	}
}

void EventLooper::StopLoop()
{
	m_running = false;
	m_stoper.Write();
}
