#include "Server.hpp"

std::string Server::password;
std::vector<Client *> Server::clients;
std::vector<Channel *> Server::channels;
std::vector<Membership *> Server::memberships;
time_t Server::start_time(time(0));

Server::Server(int argc, char **argv) : sock_(-1)
{
	if (argc != 3)
	{
		Log::Notice() << "Usage: " << argv[0] << " <port> <password>";
		Exit(EXIT_FAILURE);
	}
	unsigned short port(ParsePort(argv[1]));
	password = argv[2];
	Log::Info() << "Server starting ...";
	Cmd::Init();
	Sig::Init();
	this->sock_ = Conn::NewListener(LISTEN_ADDR, port);
	if (this->sock_ < 0)
		Exit(EXIT_FAILURE);
	clients.push_back(new Client(this->sock_, POLLIN | POLLPRI));
	clients.back()->setRegistered(true);
}

Server::~Server(void)
{
	if (this->sock_ >= 0)
	{
		close(this->sock_);
		Log::Info() << "Listening socket " << this->sock_ << " closed.\n";
	}
	Sig::Exit();
	Client::Exit();
	Channel::Exit();
	Membership::Exit();
}

std::string Server::getTime(void)
{
	char	time[64];

	strftime(time, 64, "%a %b %d %Y at %H:%M:%S", localtime(&start_time));
	return (std::string(time));
}

void Server::NewConnection(int sock)
{
	int	new_sock;

	Log::Info() << "Accepting new connection on socket " << this->sock_ << "...";
	new_sock = accept(this->sock_, 0, 0);
	if (new_sock < 0)
	{
		Log::Err() << "Can't accept connection on socket " << this->sock_ << ": " << strerror(errno) << '!';
		Exit(EXIT_FAILURE);
	}
	if (!Conn::InitSocket(new_sock))
		return ;
	clients.push_back(new Client(new_sock, POLLIN | POLLPRI));
	Log::Info() << "Accepted connection " << new_sock << " on socket " << sock << '.';
}

void Server::TimeOutCheck(void)
{
	time_t now(time(0));
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (!clients[i]->isRegistered() && clients[i]->getLastTime() < now
			- TIMEOUT)
		{
			Log::Info() << "Unregistered connection " << clients[i]->getFd() << " timed out ...";
			Client::Destroy(clients[i]);
		}
	}
}

void Server::ProcessRequest(Client *client)
{
	size_t pos(0);
	std::string command;
	std::string str = client->getReadBuffer();
	for (int i = 0; i < MAX_COMMANDS; i++)
	{
		if ((pos = str.find("\r\n")) != std::string::npos)
		{
			command = str.substr(0, pos);
			str = str.substr(pos + 2);
		}
		else if ((pos = str.find('\n')) != std::string::npos)
		{
			command = str.substr(0, pos);
			str = str.substr(pos + 1);
		}
		else
			break ;
		if (command.size() > COMMAND_LEN)
		{
			Log::Err() << "Request too long (connection " << client->getFd() << "): " << str.size() << " bytes (max. " << COMMAND_LEN << " expected)!";
			Client::Destroy(client);
			return ;
		}
		client->unsetReadBuffer();
		Parser().Request(client, command);
	}
}

void Server::ProcessBuffers(void)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (!clients[i]->getReadBuffer().empty())
			ProcessRequest(clients[i]);
	}
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (!clients[i]->getWriteBuffer().empty())
			clients[i]->setEvents(POLLOUT);
	}
}

void Server::Run(void)
{
	int	i;

	i = 0;
	while (!Sig::quit)
	{
		TimeOutCheck();
		ProcessBuffers();
		i = Dispatch();
		if (i == -1 && errno != EINTR)
		{
			Log::Err() << "Dispatch(): " << strerror(errno) << '!';
			Exit(1);
		}
	}
	Log::Info() << "Server going down NOW!";
}

int Server::Dispatch(void)
{
	int ret, fds_ready;
	std::vector<struct pollfd> pollfds(clients.size());
	for (size_t i = 0; i < clients.size(); ++i)
	{
		pollfds[i].fd = clients[i]->getFd();
		pollfds[i].events = clients[i]->getEvents();
	}
	ret = poll(pollfds.data(), pollfds.size(), 1000);
	if (ret <= 0)
		return (ret);
	fds_ready = ret;
	for (size_t i(0); i < pollfds.size(); ++i)
	{
		if (pollfds[i].revents & (POLLIN | POLLPRI | POLLOUT))
		{
			fds_ready--;
			if (pollfds[i].revents & (POLLIN | POLLPRI))
			{
				if (pollfds[i].fd == this->sock_)
					NewConnection(this->sock_);
				else
					ReadRequest(clients[i]);
			}
			if (pollfds[i].revents & POLLOUT)
				HandleWrite(clients[i]);
		}
		if (fds_ready <= 0)
			break ;
	}
	return (ret);
}

void Server::ReadRequest(Client *client)
{
	ssize_t	len;
	char	readbuf[READBUFFER_LEN];

	len = read(client->getFd(), readbuf, sizeof(readbuf));
	if (len == 0)
	{
		Client::Destroy(client);
		return ;
	}
	if (len < 0)
	{
		if (errno == EAGAIN)
			return ;
		Log::Err() << "Read error on connection " << client->getFd() << ": " << strerror(errno) << '!';
		Client::Destroy(client);
		return ;
	}
	readbuf[len] = '\0';
	client->setReadBuffer(readbuf);
}

void Server::HandleWrite(Client *client)
{
	ssize_t len(0);
	len = write(client->getFd(), client->getWriteBuffer().c_str(),
			client->getWriteBuffer().size());
	client->unsetEvent(POLLOUT);
	client->unsetWriteBuffer();
	if (len < 0)
	{
		if (errno == EAGAIN || errno == EINTR)
			return ;
		Log::Err() << "Write error on connection " << client->getFd() << ": " << strerror(errno) << '!';
		Client::Destroy(client);
	}
}

void Server::Exit(int status)
{
	delete (this);
	std::exit(status);
}

unsigned short Server::ParsePort(char *arg)
{
	char	*endptr;

	int port(strtol(arg, &endptr, 10));
	if (port > 0 && port < 0xFFFF && *endptr == '\0')
		return (static_cast<unsigned short>(port));
	else
	{
		Log::Notice() << "illegal port number " << arg << '!';
		Exit(EXIT_FAILURE);
	}
	return (0);
}
