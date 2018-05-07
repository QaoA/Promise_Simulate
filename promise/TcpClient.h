#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "Connector.h"
#include "TcpConnection.h"
#include <memory>

class TcpClient
{
public:
	TcpClient(EventLooper * loop);
	~TcpClient();

	void Connect(const char * strIp, unsigned short port);
	std::shared_ptr<TcpConnection> GetConnect()
	{
		return m_connect;
	}
	void SetOnConnectedCallback(std::function<void(std::shared_ptr<TcpConnection>)> func)
	{
		m_fnOnConnected = func;
	}

private:
	std::shared_ptr<TcpConnection> m_connect;
	Connector m_conn;
	EventLooper * m_pLooper;
	std::function<void(std::shared_ptr<TcpConnection>)> m_fnOnConnected;
};

#endif // !TCP_CLIENT_H