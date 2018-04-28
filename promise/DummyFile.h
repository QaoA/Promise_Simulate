#ifndef DUMMY_FILE_H
#define DUMMY_FILE_H

#include "EventHandler.h"
#include <sys/eventfd.h>
#include <unistd.h>

class DummyFile
{
public:
	DummyFile()
	{
		m_eh.m_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
		m_eh.m_readFunc = std::function<void(EventHandler * eh)>([](EventHandler * eh){
			uint64_t data = 1;
			write(eh->m_fd, &data, sizeof(data));
		});
		m_eh.m_writeFunc = std::function<void(EventHandler * eh)>([](EventHandler * eh) {
			uint64_t data;
			read(eh->m_fd, &data, sizeof(data));
		});
	}
	~DummyFile()
	{
		close(m_eh.m_fd);
	}
	EventHandler & GetHandler()
	{
		return m_eh;
	}

	void Write()
	{
		uint64_t data = 1;
		write(m_eh.m_fd, &data, sizeof(data));
	}
	void Read()
	{
		uint64_t data;
		read(m_eh.m_fd, &data, sizeof(data));
	}

private:
	EventHandler m_eh;
};

#endif // !DUMMY_FILE_H

