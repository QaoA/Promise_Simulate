#include "EventLooper.h"

EventLooper::EventLooper() :
	m_epoller(),
	m_pool(maxQueueLength, std::thread::hardware_concurrency())
{
}

EventLooper::~EventLooper()
{
}

void EventLooper::RegisterHandler(EventHandler & handler)
{
	Handle & h = handler.GetHandle();
	h.event.data.ptr = &handler;
	m_epoller.Add(h.fd, h.event);
}

void EventLooper::RemoveHandler(EventHandler & handler)
{
}

void EventLooper::UpdateHandler(EventHandler & handler)
{
}
