#include "TcpServer.h"

TcpServer::TcpServer(EventLooper * loop):
	m_threadsNum(std::thread::hardware_concurrency()),
	m_index(0),
	m_acceptor(loop),
	m_pLooper(loop),
	m_conns()
{
}

TcpServer::~TcpServer()
{
	for (int i = 0; i < m_threadsNum; ++i)
	{
		m_threads[i]->Stop();
	}
	for (int i = 0; i < m_threadsNum; ++i)
	{
		m_threads[i]->WaitForDie();
	}
}

void TcpServer::Listen(unsigned short port)
{
	m_acceptor.SetOnAcceptCallback([this](int fd) {onNewConnect(fd);});
	for (int i = 0; i < m_threadsNum; ++i)
	{
		m_threads.push_back(std::make_unique<EventLoopInThread>());
	}
	m_acceptor.Listen(port);
}

void TcpServer::onNewConnect(int fd)
{
	auto conn = std::make_shared<TcpConnection>(fd, &m_threads[m_index]->Looper());
	m_conns.push_back(conn);
	m_index++;
	if (m_index >= m_threadsNum)
	{
		m_index = 0;
	}
	m_fnOnConnected(conn);
}