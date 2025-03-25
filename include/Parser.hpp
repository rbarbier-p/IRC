#ifndef PARSER_HPP
# define PARSER_HPP

# include "Client.hpp"
# include "Cmd.hpp"
# include "Log.hpp"
# include "Server.hpp"
# include <cstdlib>
# include <cstring>
# include <iostream>
# include <string>
# include <vector>

# define MAX_ARGS 15

class	Client;

class Parser
{
  private:
	std::string prefix_;
	std::string command_;
	std::vector<std::string> params_;

  public:
	Parser(void);
	void Request(Client *client, const std::string &str);

  private:
	void TrimString(std::string &str);
	bool Parse(std::string request);
	void ParseParams(std::string args);
	bool ValidateCommand(Client *client);
	void HandleRequest(Client *client);
};

#endif
