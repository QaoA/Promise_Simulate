#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include "Semaphore.h"
#include <queue>
#include <mutex>

template<typename T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue(int maxLength):
		m_queue(),
		m_empty(maxLength),
		m_full(0),
		m_mutex()
	{}

	void Push(T & t)
	{
		m_empty.Wait();
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			queue.push(t);
		}
		m_full.NotifyOne();
		
	}
	void Push(T && t)
	{
		m_empty.Wait();
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			queue.push(std::move(t));
		}
		m_full.NotifyOne();

	}

	t Pop()
	{
		T t;
		m_full.Wait();
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			t = queue.front();
			queue.pop();
		}
		m_empty.Wait();
	}

private:
	std::queue m_queue;
	Semaphore m_empty;
	Semaphore m_full;
	std::mutex m_mutex;
};

#endif // !THREAD_SAFE_QUEUE_H

