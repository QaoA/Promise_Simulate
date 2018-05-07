#include "DummyFile.h"

DummyFile::DummyFile(EventLooper * looper)
{
	m_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	m_pLooper = looper;
}
DummyFile::~DummyFile()
{
	close(m_fd);
}

void DummyFile::HandleRead()
{
	uint64_t data;
	read(m_fd, &data, sizeof(data));
}

void DummyFile::StartWork()
{
	EnableRead();
	DisableWrite();
}

void DummyFile::Write()
{
	uint64_t data;
	write(m_fd, &data, sizeof(data));
}