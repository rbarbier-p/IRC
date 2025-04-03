#ifndef BOT_HPP
# define BOT_HPP

# include <cerrno>
# include <cstring>
# include <fstream>
# include <unistd.h>
# include <stdlib.h>
# include <iostream>
# include <sys/types.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <vector>
# include <sstream>
# include <poll.h>
#include <fcntl.h>
#include <unistd.h>
# include "Timer.hpp"

# define NICK "Bender" 
# define USER "botsito 0 * :bender"

class Bot
{
	private :
		struct sockaddr_in 	addr_;
		const std::string	user_;
		std::string			pass_;
		std::string			nick_;
		std::vector<Timer > timers;
		int					sock_fd;

		void listenServer(void);	
		void checkTimers(void);
		void processMsg(const char *buff);
		int initConnection(void);	
		int parser(std::string str);

	public :
		Bot(void);
		~Bot(void);

		void	run(void);

		struct sockaddr_in getAddr(void) const;
		
		std::string getPass() const;
		std::string getUser() const;
		std::string getNick() const;
		//setters
		void setNick(std::string nick);
		void setPass(std::string pass);
		void setIp(struct in_addr sin_addr);
		void setPort(unsigned short sin_port);
		void setFamily(short sin_family);
};

#endif /*BOT_HPP*/
