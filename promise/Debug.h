#ifndef DEBUG_H
#define DEBUG_H

#include "TcpConnection.h"
#include "EventLooper.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "promise.h"
#include <iostream>

using std::cout;
using std::endl;

USE_NAMESPACE;

void StdInAndOut()
{
	EventLooper e;
	TcpConnection in(STDIN_FILENO, &e);
	TcpConnection out(STDOUT_FILENO, &e);
	out.StopRead();
	out.SetMessageSendOverCallBack([]() {});
	in.SetOnMessageCome([&out](Buffer & buf) {
		out.Send(&buf);
	});
	e.StartLoop();
}

void EchoClient()
{
	EventLooper e;
	TcpConnection in(STDIN_FILENO, &e);
	TcpConnection out(STDOUT_FILENO, &e);
	out.StopRead();

	TcpClient c(&e);
	c.SetOnConnectedCallback([&in, &out](std::shared_ptr<TcpConnection> conn)
	{
		in.SetOnMessageCome([conn](Buffer & buf) mutable {
			conn->Send(&buf);
		});
		conn->SetOnMessageCome([&out](Buffer & buf)
		{
			out.Send(&buf);
		});
	});
	c.Connect("127.0.0.1", 8080);
	e.StartLoop();
}

void EchoServer()
{
	EventLooper e;
	TcpServer s(&e);

	s.SetOnConnected([](std::shared_ptr<TcpConnection> conn)
	{
		conn->SetOnMessageCome([conn](Buffer & buf) mutable
		{
			conn->Send(&buf);
		});
	});

	s.Listen(8080);
	e.StartLoop();
}

void EchoClientPromise()
{
	EventLooper e;
	TcpConnection in(STDIN_FILENO, &e);
	TcpConnection out(STDOUT_FILENO, &e);
	out.StopRead();

	TcpClient c(&e);
	Promise p = Promise([&c](Resolver resolve) {
		c.SetOnConnectedCallback(resolve);
	}).then([&in, &out](Any a)
	{
		auto conn = any_cast<std::shared_ptr<TcpConnection>>(a);
		in.SetOnMessageCome([conn](Buffer & buf) mutable {
			conn->Send(&buf);
		});
		conn->SetOnMessageCome([&out](Buffer & buf)
		{
			out.Send(&buf);
		});
	});
	c.Connect("127.0.0.1", 8080);
	e.StartLoop();
}

void EchoServerPromise()
{
	EventLooper e;
	TcpServer s(&e);

	s.SetOnConnected([](std::shared_ptr<TcpConnection> conn)
	{
		//Promise p = Promise([conn](Resolver resolve) mutable 
		//{
		//	conn->SetOnMessageCome(resolve);
		//}).then([conn](Any a)
		//{
		//	Buffer & buf = any_cast<Buffer &>(a);
		//	conn->Send(&buf);
		//});

		Promise p = Promise([conn](Resolver resolve) mutable
		{
			conn->SetOnMessageCome([conn](Buffer & buf)
			{
				conn->Send(&buf);
			});
		});
	});

	s.Listen(8080);
	e.StartLoop();
}

void Echo(int argc, char ** argv)
{
	if (argc != 2)
	{
		return;
	}
	if (argv[1][0] == 'c')
	{
		EchoClient();
	}
	else
	{
		EchoServer();
	}
}

void EchoPromise(int argc, char ** argv)
{
	if (argc != 2)
	{
		return;
	}
	if (argv[1][0] == 'c')
	{
		EchoClientPromise();
	}
	else
	{
		EchoServerPromise();
	}
}

void Test(int argc, char **argv)
{
	EchoPromise(argc, argv);
}

#endif // !DEBUG_H
