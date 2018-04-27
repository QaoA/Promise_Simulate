#ifndef CONDITION_VARIABLE_H
#define CONDITION_VARIABLE_H

#include <condition_variable>
#include <mutex>

class Semaphore
{
public:
	Semaphore(unsigned int count = 0);
	Semaphore(const Semaphore &) = delete;
	Semaphore & operator=(const Semaphore &) = delete;
	Semaphore(Semaphore &&) = delete;
	Semaphore & operator=(Semaphore &&) = delete;

public:
	void NotifyOne();
	void NotifyAll();
	void Wait();

private:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	unsigned int m_count;
};

#endif