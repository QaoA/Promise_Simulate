#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <condition_variable>

class ThreadPool
{
public:
	using Task = std::function<void()>;
public:
	ThreadPool(int maxTaskQueueSize, int threadNum);
	~ThreadPool();

	void start();
	void stop();
	void submit(const Task & task);
	void submit(Task && task);

private:
	Task take();
	bool isFull();
	void perThreadWork();
	
private:
	bool m_isRunning;
	int m_maxTaskNum;
	int m_currentTaskNum;
	int m_threadNum;
	std::queue<Task> m_tasks;
	std::vector<std::thread> m_threads;
	std::condition_variable m_notFull;
	std::condition_variable m_notEmpty;
	std::mutex m_mutex;
};

#endif // !THREAD_POOL_H