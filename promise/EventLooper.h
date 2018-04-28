#ifndef EVENT_LOOPER_H
#define EVENT_LOOPER_H

#include "Epoller.h"
#include "ThreadPool.h"
#include "DummyFile.h"
#include <thread>
#include <atomic>
#include <map>

typedef std::function<void(EventHandler * handler, uint32_t event)> OnEventFunc;

class EventLooper
{
public:
	EventLooper();
	~EventLooper();

public:
	void RegisterHandler(EventHandler & handler);
	void RemoveHandler(EventHandler & handler);
	void UpdateHandler(EventHandler & handler);
	void StartLoop(OnEventFunc func);
	void StopLoop();

private:
	static constexpr int maxPollEventSize = 100;

private:
	std::map<int, EventHandler*> m_handlerList;
	Epoller m_epoller;
	DummyFile m_stoper;
	std::atomic<bool> m_running;
};

#endif

