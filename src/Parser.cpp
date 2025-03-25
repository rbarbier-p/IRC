#include "Parser.hpp"

Parser::Parser(void)
{
}

void Parser::Request(Client *client, const std::string &request)
{
	if (!Parse(request))
	{
		Log::Info() << "Connection " << client->getFd() << ": Parse error: prefix without command!?";
		client->Write("ERROR :Prefix without command\n");
		return ;
	}
	if (!ValidateCommand(client))
		return ;
	HandleRequest(client);
}

void Parser::TrimString(std::string &str)
{
	size_t	start;
	size_t	end;

	start = str.find_first_not_of(" \t\r\n");
	end = str.find_last_not_of(" \t\r\n");
	if (start == std::string::npos || end == std::string::npos)
		str.clear();
	else
		str = str.substr(start, end - start + 1);
}

bool Parser::Parse(std::string request)
{
	size_t	space_pos;

	TrimString(request);
	space_pos = request.find(' ');
	if (request[0] == ':')
	{
		if (space_pos == std::string::npos)
			return (false);
		prefix_ = request.substr(1, space_pos - 1);
		request = request.substr(space_pos + 1);
	}
	space_pos = request.find(' ');
	if (space_pos != std::string::npos)
	{
		command_ = request.substr(0, space_pos);
		request = request.substr(space_pos + 1);
	}
	else
	{
		command_ = request;
		return (true);
	}
	ParseParams(request);
	return (true);
}

void Parser::ParseParams(std::string args)
{
	size_t	space_pos;

	TrimString(args);
	while (!args.empty() && params_.size() < MAX_ARGS)
	{
		if (args[0] == ':')
		{
			params_.push_back(args.substr(1));
			break ;
		}
		else
		{
			space_pos = args.find(' ');
			if (space_pos != std::string::npos)
			{
				params_.push_back(args.substr(0, space_pos));
				args = args.substr(space_pos + 1);
				TrimString(args);
			}
			else
			{
				params_.push_back(args);
				break ;
			}
		}
	}
}

bool Parser::ValidateCommand(Client *client)
{
	if (Cmd::commands.find(command_) == Cmd::commands.end())
	{
		Log::Info() << "Connection " << client->getFd() << ": Unknown command: " << command_;
		client->WriteErr(ERR_UNKNOWNCOMMAND(client->getNick(), command_));
		return (false);
	}
	return (true);
}

void Parser::HandleRequest(Client *client)
{
	Cmd::commands[command_](client, params_);
}
