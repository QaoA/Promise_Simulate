#ifndef EVENT_LOOPER_H
#define EVENT_LOOPER_H

#include "Epoller.h"
#include "ThreadPool.h"
#include <thread>

class EventLooper
{
public:
	EventLooper();
	~EventLooper();

public:
	void RegisterHandler(EventHandler & handler);
	void RemoveHandler(EventHandler & handler);
	void UpdateHandler(EventHandler & handler);

private:
	static constexpr int maxQueueLength = 100;

private:
	Epoller m_epoller;
	ThreadPool m_pool;
};

#endif

