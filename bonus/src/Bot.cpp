#include "Bot.hpp" 	

int myAtoi(const std::string& str)
{
	std::stringstream ss(str);
	int result;
	ss >> result;
	if (ss.fail())
		return 0;
	return result;
}

void setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error getting flags: " << strerror(errno) << std::endl;
		exit(1);
	}

	flags |= O_NONBLOCK;

	if (fcntl(fd, F_SETFL, flags) == -1) {
		std::cerr << "Error setting non-blocking: " << strerror(errno) << std::endl;
		exit(1);
	}
}

Bot::Bot(void)
{}

Bot::~Bot(void)
{}


void Bot::run(void)
{
	if (initConnection() < 0)
		return ;
	std::string str = "PASS " + this->pass_ + "\nUSER a a a a\nNICK " + this->nick_ + "\n";
	const char* msg = str.c_str();
	write(this->sock_fd, msg, strlen(msg));
	std::cout << "Listening...\n";
	setNonBlocking(this->sock_fd);
	while(42) {
		listenServer();
		checkTimers();
	}
}

int Bot::parser(std::string str)
{
	size_t msg_start = str.find(":", 1) + 1;
	if (msg_start == std::string::npos)
		return -1;

	std::string tag = str.substr(msg_start, str.find(" ", msg_start) - msg_start);
	for (size_t i = 0; i < tag.size(); i++)
		if (!std::isalpha(tag[i]) && tag[i] != '_') 
			return -2;

	if (tag.size() > 20) 
		return -3;

	std::string time = str.substr(str.find(" ", msg_start) + 1);
	if (time.size() > 10 || time.find_first_not_of("0123456789") != std::string::npos)
		return -4;
	
	return 0;
}

void Bot::processMsg(const char *buff)
{
	std::string str(buff);
	str = str.substr(0, str.find("\n"));
	if (str.find("PRIVMSG") == std::string::npos) {
		return ;
	}
	Timer new_timer; 
	new_timer.setUserName(str.substr(str.find("PRIVMSG") + 8, (str.find(":", 1) - 1) - (str.find("PRIVMSG") + 8)));
	int ret = parser(str);
	if (ret < 0) {
		std::string tmp = "PRIVMSG " + new_timer.getUserName() + " :Usage: <tag> <seconds>\n";
		const char *msg = tmp.c_str();
		write(this->sock_fd, msg, strlen(msg));
		return ; 
	}

	new_timer.setTimerTime(myAtoi(str.substr(str.find_last_of(" ") + 1)));
	new_timer.setTimerTag(str.substr(str.find(":", 1) + 1, str.find_last_of(" ") - (str.find(":", 1) + 1)));
	new_timer.setStartedTime(clock() / CLOCKS_PER_SEC);
	this->timers.push_back(new_timer);

	std::string tmp = "PRIVMSG " + new_timer.getUserName() + " :Timer \"" + new_timer.getTimerTag() + "\" set!\n";
	const char *msg = tmp.c_str();
	write(this->sock_fd, msg, strlen(msg));
}

void Bot::listenServer(void) 
{
	int readlen;
	char buff[100]; 
	
	readlen = read(this->sock_fd, buff, sizeof(buff));	
	if (readlen < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		else {
			std::cout << "Error reading fd\n";
			exit(1);
		}
	}
	if (readlen > 0) 
		processMsg(buff);
}

void Bot::checkTimers(void) {
	for (size_t i = 0; i < this->timers.size(); i++) {
		if (clock() / CLOCKS_PER_SEC >= this->timers[i].getTimerTime() + this->timers[i].getStartedTime()) {
			std::stringstream ss;
			ss << "PRIVMSG " << timers[i].getUserName() << " :Your " << timers[i].getTimerTime() 
			<< " seconds \"" << timers[i].getTimerTag() << "\" timer is off!\n";
			std::string tmp = ss.str();
			const char *msg = tmp.c_str();
			write(this->sock_fd, msg, strlen(msg));
			timers.erase(timers.begin() + i);
		}
	}
}

int Bot::initConnection(void)
{
	struct sockaddr_in addr;
	addr = getAddr();
	this->sock_fd = socket(addr.sin_family, SOCK_STREAM, 0);
	if (this->sock_fd < 0)
	{
		std::cerr << "Can't Create Socket : " << strerror(errno) << "!\n"; 
		return (-1);
	}
	if (connect(this->sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Can't Connect to Server: " << addr.sin_addr.s_addr << " Error: "<<  strerror(errno) << "!\n"; 
		return (-1);
	}
	return (0);
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
