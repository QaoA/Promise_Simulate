#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <unistd.h>
#include <cassert>

class Buffer
{
public:
	Buffer():m_buf(),m_readIdx(0),m_writeIdx(0){}
	~Buffer(){}

public:
	int In(const char * buffer, int size)
	{
		if (InputSize() < size)
		{
			m_buf.resize(m_writeIdx + size + 1);
		}
		std::copy(buffer, buffer + size, m_buf.data() + m_writeIdx);
		m_writeIdx += size;
		return size;
	}
	int In(Buffer * buf)
	{
		int size = buf->OutPutSize();
		const char * b = buf->m_buf.data() + buf->m_readIdx;
		In(b, size);
		buf->Retrive(size);
		return size;
	}
	int InputFromFile(int fd, int * error)
	{
		char buf[1024];
		int num = 0;
		num = read(fd, buf, 1024);
		if (num < 0)
		{
			*error = num;
			return 0;
		}
		In(buf, num);

		return num;
	}
	int Out(char * buffer, int size)
	{
		size = std::min(size, OutPutSize());
		std::copy(m_buf.data() + m_readIdx, m_buf.data() + m_readIdx + size, buffer);
		return size;
	}
	int OutToFile(int fd, int * error)
	{
		int total = OutPutSize();
		int num = write(fd, m_buf.data() + m_readIdx, total);
		if (num < 0)
		{
			*error = num;
		}
		m_readIdx += num;

		if (m_readIdx >= m_writeIdx)
		{
			m_readIdx = m_writeIdx = 0;
		}

		return num;
	}
	void Retrive(int len)
	{
		if (m_readIdx + len >= m_writeIdx)
		{
			m_readIdx = m_writeIdx = 0;
		}
		else
		{
			m_readIdx += len;
		}
	}
	int InputSize()
	{
		return m_buf.size() - m_writeIdx;
	}
	int OutPutSize()
	{
		return m_writeIdx - m_readIdx;
	}

private:
	std::vector<char> m_buf;
	int m_readIdx;
	int m_writeIdx;
};

#endif // !BUFFER_H