#include "Bot.hpp" 	

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
	write(this->sock, msg, strlen(msg));
	std::cout << "Listening...\n";
	while(42) {
		listenServer();
		checkTimers();
	}
}

int myAtoi(const std::string& str)
{
    std::stringstream ss(str);
    int result;
    ss >> result;  // Extracts the integer from the string
	if (ss.fail())
		return 0;
    return result;
}

int Bot::parser(std::string str)
{
	std::cout << "$" << str << "$\n";
	size_t msg_start = str.find(":", 1) + 1;
	if (msg_start == std::string::npos)
		return -1;

	std::string tag = str.substr(msg_start, str.find(" ", msg_start) - msg_start);
	std::cout << "$" << tag << "$\n";
	for (size_t i = 0; i < tag.size(); i++)
		if (!std::isalpha(tag[i]) && tag[i] != '_') 
			return -2;

	if (tag.size() > 20) 
		return -3;

	std::string time = str.substr(str.find(" ", msg_start) + 1);
	std::cout << "$" << time << "$\n";
	if (time.size() > 2 || time.find_first_not_of("0123456789") != std::string::npos)
		return -4;
	
	return 0;
}

void Bot::listenServer(void) 
{
	int readlen;
	char buff[100]; //BUFFERSIZE

	readlen = read(this->sock, buff, sizeof(buff));	
	if (readlen < 0)
		exit(1);
	if (readlen == 0) 
		return ;
	std::string str(buff);
	str = str.substr(0, str.find("\n"));
	if (str.find("PRIVMSG") == std::string::npos) {
		std::cout << "not a privmsg\n";
		return ; // send nothing
	}
	int ret = parser(str);
	if (ret < 0) {
		std::cout << ret << " parsing error\n";
		return ; // send error message to user
	}

	// FORMAT: ":<tag> <seconds>"
	std::cout << "New Timer!\n";
	Timer new_timer; 
	new_timer.setTimerTime(myAtoi(str.substr(str.find_last_of(" ") + 1)));
	new_timer.setTimerTag(str.substr(str.find(":", 1) + 1, str.find_last_of(" ")));
	new_timer.setUserName(str.substr(1, str.find("!")));
	new_timer.setStartedTime(clock());
	this->timers.push_back(new_timer);

	std::cout << new_timer.getTimerTime() << "time\n";
	std::cout << new_timer.getStartedTime() << "started\n";
	std::cout << clock() << "clock\n";
	// write(this->sock, "Timer set!", 10);
}

void Bot::checkTimers(void) {
	for (size_t i = 0; i < this->timers.size(); i++) {
		if (clock() >= this->timers[i].getTimerTime() + this->timers[i].getStartedTime()) {
			std::cout << "TIMEER!\n";
			std::stringstream ss;
			ss << "PRIVMSG " << timers[i].getUserName() << " :Your " << timers[i].getTimerTime() 
			<< " seconds \"" << timers[i].getTimerTag() << "\" is off!\n";
			const char *msg = ss.str().c_str();
			write(this->sock, msg, strlen(msg));
			std::cout << ss.str() << "\n";
			// delete timers[i];
			timers.erase(timers.begin() + i);
		}
	}
}

int Bot::initConnection(void)
{
	//Create socket
	struct sockaddr_in addr;
	addr = getAddr();
	this->sock = socket(addr.sin_family, SOCK_STREAM, 0);
	if (this->sock < 0)
	{
		std::cerr << "Can't Create Socket : " << strerror(errno) << "!\n"; 
		return (-1);
	}
	//Connect to server
	if (connect(this->sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
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
