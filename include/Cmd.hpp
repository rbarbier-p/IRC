#ifndef CMD_HPP
# define CMD_HPP

# include "Parser.hpp"
# include "Server.hpp"
# include <map>
# include <strings.h>
# include <vector>

class Cmd
{
  public:
	static void Init(void);
	static std::map<std::string, void (*)(Client *,
		std::vector<std::string>)> commands;

  private:
	static void Invite(Client *client, std::vector<std::string> params);
	static void Join(Client *client, std::vector<std::string> params);
	static void Kick(Client *client, std::vector<std::string> params);
	static void Mode(Client *client, std::vector<std::string> params);
	static void Nick(Client *client, std::vector<std::string> params);
	static void Part(Client *client, std::vector<std::string> params);
	static void Pass(Client *client, std::vector<std::string> params);
	static void Privmsg(Client *client, std::vector<std::string> params);
	static void Quit(Client *client, std::vector<std::string> params);
	static void Topic(Client *client, std::vector<std::string> params);
	static void User(Client *client, std::vector<std::string> params);
};

#endif
