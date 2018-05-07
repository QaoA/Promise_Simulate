#include "EventLoopInThread.h"

EventLoopInThread::EventLoopInThread()
{
	m_thread = std::move(std::thread([this]()mutable
	{
		this->m_looper.StartLoop();
	}));

	m_thread.detach();
}

EventLooper & EventLoopInThread::Looper()
{
	return m_looper;
}

void EventLoopInThread::Stop()
{
	m_looper.StopLoop();
}

void EventLoopInThread::WaitForDie()
{
	m_thread.join();
}