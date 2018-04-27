#include "ThreadPool.h"
#include <cassert>

ThreadPool::ThreadPool(int maxTaskQueueSize, int threadNum):
	m_isRunning(false),
	m_maxTaskNum(maxTaskQueueSize),
	m_currentTaskNum(0),
	m_threadNum(threadNum),
	m_tasks(),
	m_threads(),
	m_notFull(),
	m_notEmpty(),
	m_mutex()
{
	assert(m_maxTaskNum > 0 && m_threadNum > 0);
	m_threads.reserve(threadNum);
}

ThreadPool::~ThreadPool()
{
	if (m_isRunning)
	{
		stop();
	}
}

void ThreadPool::start()
{
	m_isRunning = true;
	
	for (int i = 0; i < m_threadNum; ++i)
	{
		m_threads.emplace_back([this]() {
			while (m_isRunning)
			{
				Task t(take());
				if (t)
				{
					t();
				}
			}
		});
	}
}

void ThreadPool::stop()
{
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_isRunning = false;
		m_notEmpty.notify_all();
	}
	for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
	{
		(*it).join();
	}
}

void ThreadPool::submit(const Task & task)
{
	std::unique_lock<std::mutex> lk(m_mutex);
	while (isFull())
	{
		m_notFull.wait(lk);
	}
	m_tasks.push(task);
	m_currentTaskNum++;
	m_notEmpty.notify_one();
}

void ThreadPool::submit(Task && task)
{
	std::unique_lock<std::mutex> lk(m_mutex);
	while (isFull())
	{
		m_notFull.wait(lk);
	}
	m_tasks.push(std::move(task));
	m_currentTaskNum++;
	m_notEmpty.notify_one();
}

ThreadPool::Task ThreadPool::take()
{
	std::unique_lock<std::mutex> lk(m_mutex);
	while (m_tasks.empty() && m_isRunning)
	{
		m_notEmpty.wait(lk);
	}
	Task task;
	if (!m_tasks.empty())
	{
		task = m_tasks.front();
		m_tasks.pop();
		m_currentTaskNum--;
		m_notFull.notify_one();
	}
	return task;
}

bool ThreadPool::isFull()
{
	return m_currentTaskNum >= m_maxTaskNum;
}

void ThreadPool::perThreadWork()
{
	while (m_isRunning)
	{
		Task task(take());
		if (task)
		{
			task();
		}
	}
}
