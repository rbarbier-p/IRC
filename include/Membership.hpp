#ifndef MEMBERSHIP_HPP
# define MEMBERSHIP_HPP

# include "Channel.hpp"
# include "Client.hpp"
# include <cstddef>
# include <set>
# include <string>
# include <vector>

class	Client;
class	Channel;

class Membership
{
  public:
	Membership(Client *client, Channel *channel);
	~Membership(void);
	Client *getClient(void);
	Channel *getChannel(void);
	void AddMode(char mode);
	bool HasMode(char mode) const;
	void DelMode(char mode);
	static void Exit(void);
	static Membership *Get(Client *client, Channel *channel);
	static void Remove(Client *client, Channel *channel);

  private:
	Client *client_;
	Channel *channel_;
	std::set<char> modes_;
};

#endif /* MEMBERSHIP_HPP */
