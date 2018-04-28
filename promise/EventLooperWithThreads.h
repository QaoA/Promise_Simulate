#ifndef EVENT_LOOPER_WITH_THREADS_H
#define EVENT_LOOPER_WITH_THREADS_H

#include "EventLooper.h"
#include "ThreadPool.h"

class EventLooperWithThreads
{
public:
	EventLooperWithThreads();
	~EventLooperWithThreads();

public:
	void Start();
	void End();
	void RegisterHandler(EventHandler & handler);
	void RemoveHandler(EventHandler & handler);
	void UpdateHandler(EventHandler & handler);

private:
	ThreadPool m_pool;
	EventLooper m_looper;
};

#endif // !EVENT_LOOPER_WITH_THREADS_H