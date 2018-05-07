#ifndef EVENTLOOPER_H
#define EVENTLOOPER_H

#include "Epoller.h"
#include "DummyFile.h"
#include <thread>
#include <atomic>
#include <map>
#include <thread>
#include <mutex>
#include <queue>

typedef std::function<void()> Functor;

class EventLooper
{
public:
	EventLooper();
	~EventLooper();

public:
	void RemoveHandler(EventHandler & handler);
	void UpdateHandler(EventHandler & handler);
	void StartLoop();
	void StopLoop();
	void RunInLoop(Functor f);

private:
	void RegisterHandler(EventHandler & handler);
	bool IsInThread();
	void RunFunctors();
	void WakeUp();

private:
	static constexpr int maxPollEventSize = 100;

private:
	std::map<int, EventHandler *> m_handlerList;
	Epoller m_epoller;
	DummyFile m_wakeuper;
	std::atomic<bool> m_running;
	std::queue<std::function<void()>> m_pendingFunctions;
	std::queue<std::function<void()>> m_runningFunctions;
	std::mutex m_mutex;
	std::thread::id m_id;
};

#endif // !EVENTLOOPER_H