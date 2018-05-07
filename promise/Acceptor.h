#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "EventHandler.h"
#include <functional>

class Acceptor: public EventHandler
{
public:
	Acceptor(EventLooper * looper);
	~Acceptor();

public:
	bool Listen(unsigned short port);
	void HandleRead() override;
	void HandleClose() override;
	void SetOnAcceptCallback(std::function<void(int)> func);

private:
	std::function<void(int)> m_fnOnAccept;
};

#endif // !ACCEPTOR_H