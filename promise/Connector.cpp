#include "Connector.h"
#include "Log.h"
#include "EventLooper.h"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>

Connector::Connector(EventLooper * loop):
	EventHandler(-1,loop)
{
}

Connector::~Connector()
{
}

bool Connector::Connect(const char * strIp, unsigned short port)
{
	sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htobe16(port);
	if (inet_pton(AF_INET,strIp, &servaddr.sin_addr) <= 0)
	{
		perror("connector ip address is error :¡¡");
		return false;
	}

	m_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (m_fd < 0)
	{
		perror("connector create connect socket error : ");
		return false;
	}

	int ret = connect(m_fd, (sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		if (errno != EINPROGRESS)
		{
			perror("connector connect error");
			return false;
		}
	}
	if (ret == 0)
	{
		m_fnOnConnected(m_fd);
		return true;
	}
	EnableWrite();
	return true;
}

void Connector::HandleClose()
{
	int error;
	socklen_t len = sizeof(error);
	int ret = getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &len);
	error = ret < 0 ? ret : error;
	if(error != 0)
	{
		Log << "connector Handle close : " << strerror(error) << endl;
	}
	else
	{
		Log << "connector Handle close" << endl;
	}
	m_pLooper->RemoveHandler(*this);
}

void Connector::HandleError()
{
	int error;
	socklen_t len = sizeof(error);
	getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &len);
	if (error != 0)
	{
		Log << "connector Handle error : " << strerror(error) << endl;
	}
	else
	{
		Log << "connector Handle error : remote may not listening port" << endl;
	}
	m_pLooper->RemoveHandler(*this);
}

void Connector::HandleWrite()
{
	int error;
	socklen_t len = sizeof(error);
	if (getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
	{
		perror("connector error socket connect : ");
	}
	else
	{
		if (error != 0)
		{
			perror("connector connect failed");
		}
		else
		{
			Log << "connector success connect, fd is " << m_fd << endl;
			m_fnOnConnected(m_fd);
		}
	}
}

void Connector::HandleRead()
{
	int error;
	socklen_t len = sizeof(error);
	getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &len);
	Log << "connector Handle read : " << strerror(error) << endl;;
}

void Connector::SetOnConnectedCallback(std::function<void(int)> func)
{
	m_fnOnConnected = func;
}
