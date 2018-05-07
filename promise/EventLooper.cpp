#include "EventLooper.h"

EventLooper::EventLooper() :
	m_handlerList(),
	m_epoller(),
	m_wakeuper(this),
	m_running(false),
	m_pendingFunctions(),
	m_runningFunctions(),
	m_mutex(),
	m_id(std::this_thread::get_id())
{
}

EventLooper::~EventLooper()
{
}

void EventLooper::RegisterHandler(EventHandler & handler)
{
	epoll_event event;
	event.data.ptr = &handler;
	event.events = handler.m_event;
	m_epoller.Add(handler.m_fd, event);
	m_handlerList.insert(std::make_pair(handler.m_fd, &handler));
}

void EventLooper::RemoveHandler(EventHandler & handler)
{
	assert(handler.m_pLooper == this);
	int fd = handler.m_fd;
	m_epoller.Remove(fd);
	m_handlerList.erase(handler.m_fd);
}

void EventLooper::UpdateHandler(EventHandler & handler)
{
	assert(handler.m_pLooper == this);

	int fd = handler.m_fd;
	auto it = m_handlerList.find(handler.m_fd);
	if (it == m_handlerList.end())
	{
		RegisterHandler(handler);
	}
	else
	{
		epoll_event event;
		event.data.ptr = &handler;
		event.events = handler.m_event;
		m_epoller.Modify(fd, event);
	}
}

void EventLooper::StartLoop()
{
	m_running = true;
	m_wakeuper.StartWork();
	epoll_event events[maxPollEventSize];
	while (m_running)
	{
		int num = m_epoller.PollEvent(maxPollEventSize, events);
		if (num > 0)
		{
			for (int i = 0; i < num; ++i)
			{
				static_cast<EventHandler *>(events[i].data.ptr)->HandleEvent(events[i].events);
			}
			RunFunctors();
		}
	}
}

void EventLooper::StopLoop()
{
	m_running = false;
	m_wakeuper.Write();
}

void EventLooper::RunInLoop(Functor f)
{
	if (IsInThread())
	{
		f();
	}
	else
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_pendingFunctions.push(f);
		WakeUp();
	}

}

void EventLooper::RunFunctors()
{
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_runningFunctions.swap(m_pendingFunctions);
	}
	while (!m_runningFunctions.empty())
	{
		auto f = m_runningFunctions.front();
		f();
		m_runningFunctions.pop();
	};
}

void EventLooper::WakeUp()
{
	m_wakeuper.Write();
}

bool EventLooper::IsInThread()
{
	return m_id == std::this_thread::get_id();
}
