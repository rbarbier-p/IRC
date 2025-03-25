#ifndef CONN_HPP
# define CONN_HPP

# include "Log.hpp"
# include <arpa/inet.h>
# include <cerrno>
# include <cstdlib>
# include <cstring>
# include <fcntl.h>
# include <netinet/in.h>
# include <poll.h>
# include <sys/socket.h>
# include <unistd.h>

class Conn
{
  public:
	static bool InitSocket(int sock);
	static int NewListener(const char *listen_addr, unsigned short port);
};

#endif /* CONN_HPP */
