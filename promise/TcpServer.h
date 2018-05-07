#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoopInThread.h"
#include <memory>
#include <functional>
#include <vector>

class TcpServer
{
public:
	TcpServer(EventLooper * loop);
	~TcpServer();

public:
	void Listen(unsigned short port);
	void SetOnConnected(std::function<void(std::shared_ptr<TcpConnection>)> func)
	{
		m_fnOnConnected = func;
	}

private:
	void onNewConnect(int fd);

private:
	unsigned int m_threadsNum;
	unsigned int m_index;
	std::vector<std::unique_ptr<EventLoopInThread>> m_threads;
	Acceptor m_acceptor;
	EventLooper * m_pLooper;
	std::function<void(std::shared_ptr<TcpConnection>)> m_fnOnConnected;
	std::vector<std::shared_ptr<TcpConnection>> m_conns;
};

#endif // !TCP_SERVER_H