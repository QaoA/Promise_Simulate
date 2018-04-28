#include "EventLooperWithThreads.h"

EventLooperWithThreads::EventLooperWithThreads():
	m_pool(100,std::thread::hardware_concurrency()),
	m_looper()
{
}


EventLooperWithThreads::~EventLooperWithThreads()
{
}

void EventLooperWithThreads::Start()
{
	m_pool.start();
	m_looper.StartLoop([this](EventHandler * handler, uint32_t event) 
	{
		handler->HandleEvent(event);
		m_pool.submit([handler,event]() 
		{
			handler->HandleEvent(event);
		});
	});
}

void EventLooperWithThreads::End()
{
	m_looper.StopLoop();
	m_pool.stop();
}

void EventLooperWithThreads::RegisterHandler(EventHandler & handler)
{
	m_looper.RegisterHandler(handler);
}

void EventLooperWithThreads::RemoveHandler(EventHandler & handler)
{
	m_looper.RemoveHandler(handler);
}

void EventLooperWithThreads::UpdateHandler(EventHandler & handler)
{
	m_looper.UpdateHandler(handler);
}
