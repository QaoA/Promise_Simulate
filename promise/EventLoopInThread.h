#ifndef EVENT_LOOP_IN_THREAD_H
#define EVENT_LOOP_IN_THREAD_H

#include "EventLooper.h"
#include <thread>

class EventLoopInThread
{
public:
	explicit EventLoopInThread();
	EventLooper & Looper();
	void Stop();
	void WaitForDie();

private:
	std::thread m_thread;
	EventLooper m_looper;
};

#endif // !EVENT_LOOP_IN_THREAD_H