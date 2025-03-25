#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include <cstdlib>
# include <ctime>
# include <netinet/in.h>
# include <poll.h>
# include <sstream>
# include <string>
# include <unistd.h>
# include <vector>

class Client
{
  public:
	Client(int fd, short events);
	~Client(void);
	int getFd(void) const;
	bool isRegistered(void) const;
	short getEvents(void) const;
	time_t getLastTime(void) const;
	std::string getNick(void) const;
	std::string getUser(void) const;
	std::string getPassword(void) const;
	std::string getReadBuffer(void) const;
	std::string getWriteBuffer(void) const;
	std::string getPrefix(void) const;
	void setFd(int fd);
	void setLastTime(time_t last_time);
	void setRegistered(bool registered);
	void setReadBuffer(std::string buffer);
	void setWriteBuffer(std::string buffer);
	void setNick(std::string nick);
	void setUser(std::string user);
	void setPassword(std::string password);
	void setEvents(short events);
	void unsetEvent(short event);
	void unsetReadBuffer(void);
	void unsetWriteBuffer(void);
	void Write(const std::string &message);
	void Write(const std::string prefix, const std::string &message);
	void WriteErr(const std::string &message);
	void WriteRpl(const std::string &message);
	static void Exit(void);
	static bool IsValidNick(const std::string &nick);
	static Client *Search(const std::string &nick);
	static void Mode(Client *client, std::vector<std::string> &params,
		Client *target);
	static void Destroy(Client *client);

  private:
	std::string rbuffer_;
	std::string wbuffer_;
	pollfd poll_;
	std::string user_;
	std::string nick_;
	std::string password_;
	time_t last_time_;
	bool registered_;
};

#endif /* CLIENT_HPP */
