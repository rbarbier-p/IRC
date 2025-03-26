#include "Bot.hpp"

Bot::Bot(void)
{}

Bot::~Bot(void)
{}

void Bot::run(void)
{
	int sock; 
	sock = initConnection();
	if (sock < 0)
		return ;
    const char* message1 = "PASS pass\nUSER a a a a\nNICK bender\n";
	send(sock, message1, strlen(message1), 0);
	while(42)
		listenServer(sock);
}

void Bot::listenServer(int sock)
{
	// if (listen(sock, 10) < 0)
	(void)sock;
	// {
	// 	std::cerr << "Can't listen on Socket: " << strerror(errno) << "!\n"; 
	// 	close(sock);
	// 	exit(EXIT_FAILURE);
// }	
}

int Bot::initConnection(void)
{
	//Create socket
	struct sockaddr_in addr;
	addr = getAddr();
	int sock(socket(addr.sin_family, SOCK_STREAM, 0));
	if (sock < 0)
	{
		std::cerr << "Can't Create Socket : " << strerror(errno) << "!\n"; 
		return (-1);
	}
	//Connect to server
	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Can't Connect to Server: " << addr.sin_addr.s_addr << " Error: "<<  strerror(errno) << "!\n"; 
		return (-1);
	}
	return (sock);
}

struct sockaddr_in Bot::getAddr(void) const
{
	return (this->addr_);
}

void	Bot::setPass(std::string pass)
{
	this->pass_ = pass;
}

void	Bot::setPort(unsigned short sin_port)
{
	this->addr_.sin_port = sin_port;	
}

void	Bot::setIp(struct in_addr sin_addr)
{
	this->addr_.sin_addr = sin_addr;	
}

void	Bot::setFamily(short sin_family)
{
	this->addr_.sin_family = sin_family;	
}
