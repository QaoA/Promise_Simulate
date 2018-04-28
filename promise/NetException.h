#ifndef NET_EXCEPTION_H
#define NET_EXCEPTION_H

#include <exception>

enum class NetError
{
	UNKOWN_HANDLER
};

class NetException:public std::exception
{
public:
	NetException(NetError e):e(e)
	{}
	~NetException()
	{}

	virtual const char* what() const noexcept override
	{
		return "bad net exception";
	}
private:
	NetError e;
};

#endif // !NET_EXCEPTION_H