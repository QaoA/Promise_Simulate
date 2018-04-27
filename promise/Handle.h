#ifndef HANDLE_H
#define HANDLE_H

#include <sys/epoll.h>

struct Handle
{
	int fd;
	epoll_event event;
};

#endif // !HANDLE_H

