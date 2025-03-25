#include "Cmd.hpp"
#include "Server.hpp"

std::map<std::string, void (*)(Client *,
	std::vector<std::string>)> Cmd::commands;

void Cmd::Init(void)
{
	commands["INVITE"] = Invite;
	commands["JOIN"] = Join;
	commands["KICK"] = Kick;
	commands["MODE"] = Mode;
	commands["NICK"] = Nick;
	commands["PART"] = Part;
	commands["PASS"] = Pass;
	commands["PRIVMSG"] = Privmsg;
	commands["QUIT"] = Quit;
	commands["TOPIC"] = Topic;
	commands["USER"] = User;
}

static bool	ValidateRegister(Client *client)
{
	if (!client->isRegistered())
	{
		client->WriteErr(ERR_NOTREGISTERED(client->getNick()));
		return (false);
	}
	return (true);
}

static bool	ValidateParams(Client *client, int min, int max, int argc,
		std::string const &cmd)
{
	if (argc < min || (max != -1 && argc > max))
	{
		client->WriteErr(ERR_NEEDMOREPARAMS(client->getNick(), cmd));
		return (false);
	}
	return (true);
}

static bool	JoinAllowed(Client *client, Channel *chan, std::string key)
{
	bool is_invited(chan->IsInvited(client));
	if (chan->HasMode('i') && !is_invited)
	{
		client->WriteErr(ERR_INVITEONLYCHAN(client->getNick(),
				chan->getName()));
		return (false);
	}
	if (chan->HasMode('k') && chan->getKey() != key)
	{
		client->WriteErr(ERR_BADCHANNELKEY(client->getNick(), chan->getName()));
		return (false);
	}
	if (chan->HasMode('l') && chan->getMaxUsers() <= Channel::MemberCount(chan))
	{
		client->WriteErr(ERR_CHANNELISFULL(client->getNick(), chan->getName()));
		return (false);
	}
	return (true);
}

static void	LoginUser(Client *client)
{
	if (!Server::password.empty() && client->getPassword() != Server::password)
	{
		Log::Err() << "User \"" << client->getUser() << "\" rejected (connection " << client->getFd() << "): Bad server password!";
		Client::Destroy(client);
		return ;
	}
	client->setRegistered(true);
	Log::Info() << "User \"" << client->getNick() << "\" registered (connection " << client->getFd() << ").";
	client->WriteRpl(RPL_WELCOME(client->getNick(), client->getUser(),
			"unknown"));
	client->WriteRpl(RPL_YOURHOST(client->getNick(), "*", "irc-0.0.1"));
	client->WriteRpl(RPL_CREATED(client->getNick(), Server::getTime()));
	client->WriteRpl(RPL_MYINFO(client->getNick(), "*", "irc-0.0.1", "iklot"));
}

void Cmd::Invite(Client *client, std::vector<std::string> params)
{
	if (!ValidateRegister(client))
		return ;
	if (!ValidateParams(client, 2, 2, params.size(), "INVITE"))
		return ;
	Client *target(Client::Search(params[0]));
	if (!target)
	{
		client->WriteErr(ERR_NOSUCHNICK(client->getNick(), params[0]));
		return ;
	}
	Channel *chan(Channel::Search(params[1]));
	if (chan)
	{
		Membership *member(Membership::Get(client, chan));
		if (!member)
		{
			client->WriteErr(ERR_NOTONCHANNEL(client->getNick(), params[1]));
			return ;
		}
		if (chan->HasMode('i') && !member->HasMode('o'))
		{
			client->WriteErr(ERR_CHANOPRIVSNEEDED(client->getNick(),
					params[1]));
			return ;
		}
		else
			chan->AddInvite(target);
	}
	Log::Info() << "User " << client->getNick() << " invites " << params[0] << " to " << params[1];
	target->Write("INVITE " + params[0] + " " + params[1]);
	target->WriteRpl(RPL_INVITING(target->getNick(), params[0], params[1]));
}

void Cmd::Join(Client *client, std::vector<std::string> params)
{
	if (!ValidateRegister(client))
		return ;
	if (!ValidateParams(client, 1, 2, params.size(), "JOIN"))
		return ;
	if (params[0].empty())
	{
		client->WriteErr(ERR_NEEDMOREPARAMS(client->getNick(), "JOIN"));
		return ;
	}
	if (params.size() == 1 && params[0] == "0")
	{
		Channel::PartAll(client);
		return ;
	}
	bool op(false);
	std::string key;
	std::string channame;
	std::stringstream key_ss;
	std::stringstream chan_ss(params[0]);
	if (params.size() > 1)
	{
		key_ss.str(params[1]);
		key_ss.clear();
		std::getline(key_ss, key, ',');
	}
	while (std::getline(chan_ss, channame, ','))
	{
		Channel *chan(Channel::Search(channame));
		Membership *is_member(chan ? Membership::Get(client, chan) : 0);
		if (is_member)
			continue ;
		if (chan)
		{
			if (!JoinAllowed(client, chan, key))
				continue ;
		}
		if (!chan && channame[0] != '+')
			op = true;
		if (!Channel::Join(client, channame))
			continue ;
		if (!chan)
		{
			chan = Channel::Search(channame);
			is_member = Membership::Get(client, chan);
		}
		if (op)
			is_member->AddMode('o');
		chan->Write(client, "JOIN :" + chan->getName());
		client->Write("JOIN :" + chan->getName());
		if (params.size() > 1)
			std::getline(key_ss, key, ',');
	}
}

void Cmd::Kick(Client *client, std::vector<std::string> params)
{
	if (!ValidateRegister(client))
		return ;
	if (!ValidateParams(client, 2, 3, params.size(), "KICK"))
		return ;
	unsigned int channel_count(0);
	unsigned int nick_count(0);
	std::string item;
	std::stringstream ss(params[0]);
	while (std::getline(ss, item, ','))
		channel_count++;
	ss.str(params[1]);
	ss.clear();
	while (std::getline(ss, item, ','))
		nick_count++;
	std::string current_nick;
	std::string current_channel;
	std::string reason(params.size() == 3 ? params[2] : client->getNick());
	if (channel_count == 1)
	{
		ss.str(params[1]);
		ss.clear();
		while (std::getline(ss, current_nick, ','))
		{
			Channel::Kick(client, current_nick, params[0], reason);
			nick_count--;
		}
	}
	else if (channel_count == nick_count)
	{
		ss.str(params[0]);
		ss.clear();
		std::stringstream nick_ss(params[1]);
		nick_ss.clear();
		while (std::getline(ss, current_channel, ',') && std::getline(nick_ss,
				current_nick, ','))
		{
			Channel::Kick(client, current_nick, current_channel, reason);
			channel_count--;
			nick_count--;
		}
	}
	else
		client->WriteErr(ERR_NEEDMOREPARAMS(client->getNick(), "KICK"));
}

void Cmd::Mode(Client *client, std::vector<std::string> params)
{
	if (!ValidateRegister(client))
		return ;
	if (!ValidateParams(client, 1, -1, params.size(), "MODE"))
		return ;
	Client *cl(0);
	Channel *chan(0);
	bool is_valid_nick(Client::IsValidNick(params[0]));
	bool is_valid_chan(Channel::IsValidName(params[0]));
	if (is_valid_nick)
		cl = Client::Search(params[0]);
	if (is_valid_chan)
		chan = Channel::Search(params[0]);
	if (cl)
		return (Client::Mode(client, params, cl));
	if (chan)
		return (Channel::Mode(client, params, chan));
	if (is_valid_nick)
		client->WriteErr(ERR_NOSUCHNICK(client->getNick(), params[0]));
	else
		client->WriteErr(ERR_NOSUCHCHANNEL(client->getNick(), params[0]));
}

void Cmd::Part(Client *client, std::vector<std::string> params)
{
	if (!ValidateRegister(client))
		return ;
	if (!ValidateParams(client, 1, 2, params.size(), "PART"))
		return ;
	if (params[0].empty())
		return (client->WriteErr(ERR_NEEDMOREPARAMS(client->getNick(),
					"PART")));
	std::string chan;
	std::stringstream ss(params[0]);
	while (std::getline(ss, chan, ','))
		Channel::Part(client, chan, params.size() > 1 ? params[1] : "");
}

void Cmd::Quit(Client *client, std::vector<std::string> params)
{
	if (!ValidateRegister(client))
		return ;
	if (!ValidateParams(client, 0, 1, params.size(), "QUIT"))
		return ;
	if (params.size() == 1)
		Log::Info() << client->getNick() << ": " << params[0];
	Client::Destroy(client);
}

void Cmd::Privmsg(Client *client, std::vector<std::string> params)
{
	if (!ValidateRegister(client))
		return ;
	if (params.size() == 0)
		return (client->WriteErr(ERR_NORECIPIENT(client->getNick(),
					"PRIVMSG")));
	else if (params.size() == 1)
		return (client->WriteErr(ERR_NOTEXTTOSEND(client->getNick())));
	else if (!ValidateParams(client, 2, 2, params.size(), "PRIVMSG"))
		return ;
	else if (params[0].empty())
		return (client->WriteErr(ERR_NOTEXTTOSEND(client->getNick())));
	std::string target;
	std::stringstream ss(params[0]);
	while (std::getline(ss, target, ','))
	{
		Client *dest(0);
		Channel *chan(0);
		if (target.find('!') != std::string::npos)
			dest = Client::Search(target);
		else
		{
			std::string nick(target.substr(0, target.find('!')));
			dest = Client::Search(nick);
		}
		if (dest)
			dest->Write("PRIVMSG " + client->getNick() + " :" + params[1]);
		else if ((chan = Channel::Search(target)))
			chan->Write(client, "PRIVMSG " + target + " :" + params[1]);
		else
			client->WriteErr(ERR_NOSUCHNICK(client->getNick(), target));
	}
}

void Cmd::Pass(Client *client, std::vector<std::string> params)
{
	if (!ValidateParams(client, 1, 1, params.size(), "PASS"))
		return ;
	if (client->getPassword().empty())
	{
		Log::Info() << "Connection " << client->getFd() << ": got PASS command ...";
		client->setPassword(params[0]);
	}
	else
		client->WriteErr(ERR_ALREADYREGISTRED(client->getNick()));
}

void Cmd::Nick(Client *client, std::vector<std::string> params)
{
	if (!ValidateParams(client, 1, 1, params.size(), "NICK"))
		return ;
	if (strcasecmp(client->getNick().c_str(), params[0].c_str()) != 0)
	{
		if (!Client::IsValidNick(params[0]))
		{
			if (params[0].size() > MAX_NICK_LEN)
				client->WriteErr(ERR_NICKNAMETOOLONG(client->getNick(),
						params[0]));
			else
				client->WriteErr(ERR_ERRONEUSNICKNAME(client->getNick(),
						params[0]));
			return ;
		}
		if (Client::Search(params[0]))
		{
			client->WriteErr(ERR_NICKNAMEINUSE(client->getNick(), params[0]));
			return ;
		}
	}
	if (!client->isRegistered())
	{
		Log::Info() << "Connection " << client->getFd() << ": got valid NICK command ...";
		client->setNick(params[0]);
		if (!client->getUser().empty())
			return (LoginUser(client));
	}
	else
	{
		Log::Info() << "Connection " << client->getFd() << ": changed nickname to " << params[0];
		client->Write("NICK :" + params[0]);
		client->setNick(params[0]);
	}
}

void Cmd::Topic(Client *client, std::vector<std::string> params)
{
	if (!ValidateRegister(client))
		return ;
	if (!ValidateParams(client, 1, 2, params.size(), "TOPIC"))
		return ;
	Channel *chan(Channel::Search(params[0]));
	if (!chan)
		return (client->WriteErr(ERR_NOSUCHCHANNEL(client->getNick(),
					params[0])));
	Membership *member(Membership::Get(client, chan));
	if (!member)
		return (client->WriteErr(ERR_NOTONCHANNEL(client->getNick(),
					params[0])));
	if (params.size() == 1)
	{
		std::string topic(chan->getTopic());
		if (topic.empty())
			client->WriteRpl(RPL_NOTOPIC(client->getNick(), chan->getName()));
		else
			client->WriteRpl(RPL_TOPIC(client->getNick(), chan->getName(),
					topic));
		return ;
	}
	if (chan->HasMode('t') && !member->HasMode('o'))
		return (client->WriteErr(ERR_CHANOPRIVSNEEDED(client->getNick(),
					params[0])));
	if (params[1] != chan->getTopic())
		chan->Write(client, "TOPIC " + params[0] + " :" + params[1]);
	chan->setTopic(params[1]);
	client->Write("TOPIC " + params[0] + " :" + params[1]);
}

void Cmd::User(Client *client, std::vector<std::string> params)
{
	if (client->isRegistered())
		return (client->WriteErr(ERR_ALREADYREGISTRED(client->getNick())));
	if (!ValidateParams(client, 4, 4, params.size(), "USER"))
		return ;
	if (!client->getPassword().empty() || !client->getNick().empty())
	{
		if (params[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-@._") != std::string::npos)
		{
			Client::Destroy(client);
			return ;
		}
		client->setUser(params[0]);
		Log::Info() << "Connection " << client->getFd() << ": got valid USER command ...";
		if (!client->getNick().empty())
			return (LoginUser(client));
	}
	else
		client->WriteErr(ERR_NOTREGISTERED(client->getNick()));
}
