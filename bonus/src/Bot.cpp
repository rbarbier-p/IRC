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
	std::string str = "PASS " + this->pass_ + "\nUSER a a a a\nNICK " + this->nick_ + "\n";
	const char* msg = str.c_str();
	send(sock, msg, strlen(msg), 0);
	while(42)
		listenServer(sock);
}

int myAtoi(const std::string& str)
{
    std::stringstream ss(str);
    int result;
    ss >> result;  // Extracts the integer from the string
    return result;
}

int Bot::parser(std::string str)
{
	size_t msg_start = str.find(":", 1);
	if (msg_start == std::npos || msg_start + 1 < size(str))	
		return -1;

	std::string tag = str.substr(msg_start + 1, str.find(" ", str.find(":", 1)));
	for (char c : tag)
		if (!std::isalpha(c) && c != '_')
			return -1;

	if (size(tag) > 20) 
		return -1;

	std::string time = str.substr(str.find(" ", str.find(":", 1)) + 1)
	if (size(time) > 2 || time.find_first_not_of("0123456789") != std::npos)
		return -1;
	
	return 0;
}

void Bot::listenServer(int sock, std::vector<Timer> timers)
{
	int readlen;
	char buff[100]; //BUFFERSIZE

	readlen = read(sock, buff, sizeof(buff));	
	if (readlen < 0)
		exit(1);
	if (readlen == 0)
		return ;
	std::string str(buff);
	if (str.find("PRIVMSG") == std::npos)
		return ; // send nothing
	if (parser(str) == -1)
		return ; // send error message to user

	Timer new_timer();
	new_timer.setTimerTag(str.substr(str.find_last_of(" "));
	new_timer.setTimerTime(myAtoi(str.substr(str.find(":", 1) + 1, str.find_last_of(" "))));
	new_timer.setUserName(str.substr(1, std.find("!")));
	new_timer.setStartedTime(clock());
	timers.push_back(new_timer);
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

void	Bot::setNick(std::string nick)
{
	this->nick_ = nick;
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
