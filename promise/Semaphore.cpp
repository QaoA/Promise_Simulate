#include "Semaphore.h"

Semaphore::Semaphore(unsigned int count):
	m_mutex(),
	m_cv(),
	m_count(count)
{
}

void Semaphore::NotifyOne()
{
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		m_count++;
	}

	m_cv.notify_one();
}

void Semaphore::NotifyAll()
{
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		m_count++;
	}

	m_cv.notify_all();
}

void Semaphore::Wait()
{
	std::unique_lock<std::mutex> lk(m_mutex);
	while (m_count == 0)
	{
		m_cv.wait(lk);
	}
	m_count--;
}