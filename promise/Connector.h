#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "EventHandler.h"
#include <functional>

class Connector : public EventHandler
{
public:
	Connector(EventLooper * loop);
	~Connector();

	bool Connect(const char * strIp, unsigned short port);
	void HandleClose() override;
	void HandleError() override;
	void HandleWrite() override;
	void HandleRead() override;
	void SetOnConnectedCallback(std::function<void(int)> func);

private:
	std::function<void(int)> m_fnOnConnected;
};

#endif // !CONNECTOR_H