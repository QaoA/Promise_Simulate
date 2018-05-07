#include "Acceptor.h"
#include "EventLooper.h"
#include "Log.h"
#include <iostream>
#include <arpa/inet.h>
#include <cstring>

Acceptor::Acceptor(EventLooper * loop):
	m_fnOnAccept([](int) {})
{
	m_fd = -1;
	m_pLooper = loop;
}


Acceptor::~Acceptor()
{
	if (m_fd != -1)
	{
		close(m_fd);
	}
}

bool Acceptor::Listen(unsigned short port)
{
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htobe16(port);

	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

	m_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_fd < 0)
	{
		perror("create listen socket error : ");
		return false;
	}
	if (bind(m_fd, (sockaddr *)(&servaddr), sizeof(servaddr)) < 0)
	{
		perror("bind socket error : ");
		return false;
	}
	if (listen(m_fd, 20) < 0)
	{
		perror("listen socket error : ");
		return false;
	}
	EnableRead();
	return true;
}

void Acceptor::HandleRead()
{
	int connfd = accept(m_fd, nullptr, nullptr);
	if (connfd < 0)
	{
		perror("error accept");
	}
	else
	{
		Log << "success accept, fd is " << connfd << endl;
		m_fnOnAccept(connfd);
	}
}

void Acceptor::HandleClose()
{
	m_pLooper->RemoveHandler(*this);
	close(m_fd);
	m_fd = -1;
}

void Acceptor::SetOnAcceptCallback(std::function<void(int)> func)
{
	m_fnOnAccept = func;
}