#include "Conn.hpp"

static bool	InitAddr(struct sockaddr_in *addr, const char *ip,
		unsigned short port)
{
	bzero(addr, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip);
	if (addr->sin_addr.s_addr == (unsigned)-1)
		return (false);
	addr->sin_port = htons(port);
	return (true);
}

static bool	SetNonBlock(int fd)
{
	int	flags;

	flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return (false);
	flags |= O_NONBLOCK;
	return (fcntl(fd, F_SETFL, flags) == 0);
}

bool Conn::InitSocket(int sock)
{
	if (!SetNonBlock(sock))
	{
		Log::Err() << "Can't enable non-blocking mode for socket: " << strerror(errno) << '!';
		close(sock);
		return (false);
	}
	int value(1);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &value,
			static_cast<socklen_t>(sizeof(value))) != 0)
		Log::Err() << "Can't set socket option SO_REUSEADDR: " << strerror(errno) << '!';
	return (true);
}

int Conn::NewListener(const char *listen_addr, unsigned short port)
{
	struct sockaddr_in	addr;

	if (!InitAddr(&addr, listen_addr, port))
	{
		Log::Err() << "Can't listen on [" << listen_addr << "]:" << port << ": Failed to parse IP address!";
		return (-1);
	}
	int af(addr.sin_family);
	int sock(socket(af, SOCK_STREAM, 0));
	if (sock < 0)
	{
		Log::Err() << "Can't create socket (af " << af << ") : " << strerror(errno) << '!';
		return (-1);
	}
	if (!InitSocket(sock))
		return (-1);
	if (bind(sock, reinterpret_cast<struct sockaddr *>(&addr),
			sizeof(addr)) != 0)
	{
		std::ostringstream oss;
		oss << "Can't bind socket to address " << listen_addr << ':';
		oss << port << " - " << strerror(errno) << '!';
		Log::Err() << oss.str();
		close(sock);
		return (-1);
	}
	if (listen(sock, 10) != 0)
	{
		Log::Err() << "Can't listen on socket: " << strerror(errno) << '!';
		close(sock);
		return (-1);
	}
	Log::Info() << "Now listening on [" << listen_addr << "]:" << port << " (socket " << sock << ").";
	return (sock);
}
