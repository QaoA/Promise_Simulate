#include "TcpClient.h"

TcpClient::TcpClient(EventLooper * loop):
	m_connect(nullptr),
	m_conn(loop),
	m_pLooper(loop),
	m_fnOnConnected([](std::shared_ptr<TcpConnection>){})
{
}


TcpClient::~TcpClient()
{
}

void TcpClient::Connect(const char * strIp, unsigned short port)
{
	m_conn.SetOnConnectedCallback([this](int fd)
	{
		m_connect = std::make_shared<TcpConnection>(fd, m_pLooper);
		m_fnOnConnected(m_connect);
	});
	m_conn.Connect(strIp, port);
}