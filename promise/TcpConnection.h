#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <functional>
#include "EventHandler.h"
#include "Buffer.h"

class Accptor;
class Connector;

class TcpConnection : public EventHandler
{
	friend class Accptor;
	friend class Connector;

	enum State
	{
		SHUTDOWN,
		CONNECTED,
		CONNECTING
	};
public:
	TcpConnection(int fd, EventLooper * loop);
	~TcpConnection();

public:
	void StartRead();
	void StopRead();
	void Send(const char * buffer, int size);
	void Send(Buffer * buffer);
	void Close();
	void SetMessageSendOverCallBack(std::function<void()> func)
	{
		m_fnOnMessageSendOver = func;
	}
	void SetOnMessageCome(std::function<void(Buffer & buf)> func)
	{
		m_fnOnMessageCome = func;
	}

private:
	virtual void HandleRead() override;
	virtual void HandleWrite() override;
	virtual void HandleClose() override;
	virtual void HandleError() override;

private:
	State m_state;
	Buffer m_inputBuf;
	Buffer m_outputBuf;
	std::function<void()> m_fnOnMessageSendOver;
	std::function<void(Buffer & buf)> m_fnOnMessageCome;
};

#endif // !TCP_CONNECTION_H