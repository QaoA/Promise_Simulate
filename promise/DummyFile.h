#ifndef DUMMY_FILE_H
#define DUMMY_FILE_H

#include <sys/eventfd.h>
#include <unistd.h>

#include "EventHandler.h"

class DummyFile:public EventHandler
{
public:
	DummyFile(EventLooper * looper);
	~DummyFile();
	virtual void HandleRead() override;
	void StartWork();
	void Write();

private:
	EventHandler m_eh;
};

#endif // !DUMMY_FILE_H

