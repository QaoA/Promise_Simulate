#include "TcpConnection.h"
#include "EventLooper.h"
#include "Log.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

TcpConnection::TcpConnection(int fd, EventLooper * loop):
	EventHandler(fd,loop),
	m_state(CONNECTED),
	m_inputBuf(),
	m_outputBuf(),
	m_fnOnMessageSendOver([] {}),
	m_fnOnMessageCome([](Buffer &){})
{
	StartRead();
}

TcpConnection::~TcpConnection()
{
	if (m_state == CONNECTED || m_state == CONNECTING)
	{
		close(m_fd);
	}
}

void TcpConnection::StartRead()
{
	if (!IsInReading())
	{
		EnableRead();
	}
}

void TcpConnection::StopRead()
{
	if (IsInReading())
	{
		DisableRead();
	}
}

void TcpConnection::Send(const char * buffer, int size)
{
	m_outputBuf.In(buffer, size);
	if (!IsInWriting())
	{
		EnableWrite();
	}
}

void TcpConnection::Send(Buffer * buf)
{
	m_outputBuf.In(buf);
	if (!IsInWriting())
	{
		EnableWrite();
	}
}

void TcpConnection::HandleRead()
{
	int error;
	m_inputBuf.InputFromFile(m_fd, &error);
	m_fnOnMessageCome(m_inputBuf);
}

void TcpConnection::HandleWrite()
{
	int error;
	m_outputBuf.OutToFile(m_fd, &error);
	if (!m_outputBuf.OutPutSize())
	{
		DisableWrite();
		m_fnOnMessageSendOver();
	}
}

void TcpConnection::HandleClose()
{
	Log << "connection -- connect is closed" << endl;
	m_pLooper->RemoveHandler(*this);
}

void TcpConnection::HandleError()
{
	Log << "connection -- connect is error" << endl;
	int error;
	socklen_t len = sizeof(error);
	int ret = getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &len);
	error = ret < 0 ? ret : error;
	if (error != 0)
	{
		Log << "connection Handle close : " << strerror(error) << endl;
	}
	m_pLooper->RemoveHandler(*this);
}

void TcpConnection::Close()
{
	close(m_fd);
	m_state = SHUTDOWN;
}