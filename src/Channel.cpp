#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel(const std::string &name) : name_(name), max_users_(0)
{
}

Channel::~Channel(void)
{
}

std::string Channel::getName(void) const
{
	return (name_);
}

std::string Channel::getModes(void) const
{
	std::string modes;
	std::set<char>::const_iterator it(modes_.begin());
	while (it != modes_.end())
	{
		modes += *it;
		++it;
	}
	return (modes);
}

std::string Channel::getTopic(void) const
{
	return (topic_);
}

std::string Channel::getKey(void) const
{
	return (key_);
}

size_t Channel::getMaxUsers(void) const
{
	return (max_users_);
}

void Channel::setMaxUsers(size_t max_users)
{
	max_users_ = max_users;
}

void Channel::setTopic(const std::string &topic)
{
	topic_ = topic;
}

void Channel::setKey(const std::string &key)
{
	key_ = key;
}

void Channel::AddMode(char mode)
{
	modes_.insert(mode);
}

bool Channel::HasMode(char mode) const
{
	return (modes_.find(mode) != modes_.end());
}

void Channel::DelMode(char mode)
{
	modes_.erase(mode);
}

void Channel::AddInvite(Client *client)
{
	invites_.push_back(client);
}

bool Channel::IsInvited(Client *client) const
{
	std::vector<Client *>::const_iterator it(invites_.begin());
	while (it != invites_.end())
	{
		if (*it == client)
			return (true);
		++it;
	}
	return (false);
}

void Channel::Write(Client *client, const std::string &message)
{
	if (Membership::Get(client, this) == 0)
		return (client->WriteErr(ERR_CANNOTSENDTOCHAN(client->getNick(),
					name_)));
	std::vector<Membership *>::iterator it(Server::memberships.begin());
	while (it != Server::memberships.end())
	{
		if ((*it)->getChannel() == this && (*it)->getClient() != client)
			(*it)->getClient()->Write(client->getPrefix(), message);
		++it;
	}
}

void Channel::Exit(void)
{
	std::vector<Channel *>::iterator it(Server::channels.begin());
	while (it != Server::channels.end())
	{
		delete (*it);
		++it;
	}
}

bool Channel::IsValidName(const std::string &name)
{
	if (name.size() <= 1)
		return (false);
	if (name[0] != '#' && name[0] != '&' && name[0] != '+')
		return (false);
	if (name.size() > MAX_CHANNEL_LEN)
		return (false);
	return (true);
}

size_t Channel::MemberCount(const Channel *channel)
{
	size_t count(0);
	std::vector<Membership *>::iterator it(Server::memberships.begin());
	while (it != Server::memberships.end())
	{
		if ((*it)->getChannel() == channel)
			++count;
		++it;
	}
	return (count);
}

Channel *Channel::Search(const std::string &name)
{
	std::vector<Channel *>::iterator it(Server::channels.begin());
	while (it != Server::channels.end())
	{
		if ((*it)->getName() == name)
			return (*it);
		++it;
	}
	return (0);
}

void Channel::Mode(Client *client, std::vector<std::string> &params,
	Channel *target)
{
	if (target->getName()[0] == '+')
		return (client->WriteErr(ERR_NOCHANMODES(client->getNick(),
					target->getName())));
	if (params.size() <= 1)
		return (client->WriteRpl(RPL_CHANNELMODEIS(client->getNick(),
					target->getName(), target->getModes())));
	Membership *member(Membership::Get(client, target));
	if (!member)
		return (client->WriteErr(ERR_NOTONCHANNEL(client->getNick(),
					target->getName())));
	long l(0);
	Client *client_target(0);
	Membership *target_member(0);
	bool is_op(member->HasMode('o'));
	std::string the_modes("");
	std::string the_args("");
	for (size_t i = 1; i < params.size(); i++)
	{
		bool set(false);
		std::string mode(params[i]);
		if (mode[0] == '+')
		{
			set = true;
			mode = mode.substr(1);
		}
		else if (mode[0] == '-')
			mode = mode.substr(1);
		if (!is_op)
		{
			client->WriteErr(ERR_CHANOPRIVSNEEDED(client->getNick(),
					target->getName()));
			break ;
		}
		switch (mode[0])
		{
		case 'o':
			if (i + 1 >= params.size())
			{
				client->WriteErr(ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
				break ;
			}
			client_target = Client::Search(params[i + 1]);
			if (!client_target)
			{
				client->WriteErr(ERR_NOSUCHNICK(client->getNick(), params[i
						+ 1]));
				break ;
			}
			target_member = Membership::Get(client_target, target);
			if (!target_member)
			{
				client->WriteErr(ERR_NOTONCHANNEL(client->getNick(),
						client_target->getNick()));
				break ;
			}
			set ? target_member->AddMode('o') : target_member->DelMode('o');
			the_modes += (set ? std::string("+") : std::string("-")) + "o";
			the_args += " " + client_target->getNick();
			break ;
		case 'i':
			set ? target->AddMode('i') : target->DelMode('i');
			the_modes += (set ? std::string("+") : std::string("-")) + "i";
			break ;
		case 't':
			set ? target->AddMode('t') : target->DelMode('t');
			the_modes += (set ? std::string("+") : std::string("-")) + "t";
			break ;
		case 'k':
			if (!set)
			{
				target->DelMode('k');
				the_modes += "-k";
				break ;
			}
			if (i + 1 >= params.size())
			{
				client->WriteErr(ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
				break ;
			}
			if (params[i + 1].empty() || params[i
				+ 1].find(' ') != std::string::npos)
			{
				client->WriteErr(ERR_INVALIDMODEPARAM(client->getNick(),
						target->getName(), "k"));
				break ;
			}
			target->DelMode('k');
			target->setKey(params[i + 1]);
			target->AddMode('k');
			the_modes += "+k";
			the_args = params[i + 1];
			i++;
			break ;
		case 'l':
			if (!set)
			{
				target->DelMode('l');
				the_modes += "-l";
				break ;
			}
			if (i + 1 >= params.size())
			{
				client->WriteErr(ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
				break ;
			}
			l = std::atol(params[i + 1].c_str());
			if (l <= 0 || l >= 0xFFFF)
			{
				client->WriteErr(ERR_INVALIDMODEPARAM(client->getNick(),
						target->getName(), "l"));
				break ;
			}
			target->DelMode('l');
			target->setMaxUsers(l);
			target->AddMode('l');
			the_modes += "+l";
			the_args = params[i + 1];
			i++;
			break ;
		default:
			client->WriteErr(ERR_UNKNOWNMODE(client->getNick(), mode[0],
					target->getName()));
			break ;
		}
	}
	if (!the_modes.empty())
	{
		client->Write("MODE " + target->getName() + " " + the_modes + the_args);
		target->Write(client, "MODE " + target->getName() + " " + the_modes
			+ the_args);
	}
}

bool Channel::Join(Client *client, const std::string &name)
{
	if (!IsValidName(name))
	{
		client->WriteErr(ERR_NOSUCHCHANNEL(client->getNick(), name));
		return (false);
	}
	Channel *channel(Channel::Search(name));
	if (channel)
	{
		if (Membership::Get(client, channel))
			return (false);
	}
	else
	{
		channel = new Channel(name);
		Server::channels.push_back(channel);
	}
	Server::memberships.push_back(new Membership(client, channel));
	return (true);
}

void Channel::PartAll(Client *client)
{
	std::vector<Membership *>::iterator it(Server::memberships.begin());
	while (it != Server::memberships.end())
	{
		if ((*it)->getClient() == client)
		{
			Channel *channel((*it)->getChannel());
			Part(client, channel->getName(), "");
			continue ;
		}
		++it;
	}
}

void Channel::Part(Client *client, const std::string &name,
	const std::string &reason)
{
	Channel *channel(Channel::Search(name));
	if (!channel)
		return (client->WriteErr(ERR_NOSUCHCHANNEL(client->getNick(), name)));
	Membership *membership(Membership::Get(client, channel));
	if (membership == 0)
		return (client->WriteErr(ERR_NOTONCHANNEL(client->getNick(), name)));
	client->Write("PART " + name + " :" + reason);
	channel->Write(client, "PART " + channel->getName() + " :" + reason);
	Log::Info() << "User " << client->getNick() << " left channel " << name << " (" << reason << ")\n";
	Membership::Remove(client, channel);
}

void Channel::Kick(Client *client, const std::string &nick,
	const std::string &channel, const std::string &reason)
{
	Client *target(Client::Search(nick));
	if (!target)
		return (client->WriteErr(ERR_NOSUCHNICK(client->getNick(), nick)));
	Channel *chan(Channel::Search(channel));
	if (!chan)
		return (client->WriteErr(ERR_NOSUCHCHANNEL(client->getNick(),
					channel)));
	Membership *member(Membership::Get(client, chan));
	if (!member)
		return (client->WriteErr(ERR_NOTONCHANNEL(client->getNick(), channel)));
	Membership *target_member(Membership::Get(target, chan));
	if (!target_member)
		return (client->WriteErr(ERR_USERNOTINCHANNEL(client->getNick(),
					target->getNick(), channel)));
	if (!member->HasMode('o'))
		return (client->WriteErr(ERR_CHANOPPRIVTOOLOW(client->getNick(),
					channel)));
	Membership::Remove(target, chan);
	client->Write("KICK " + channel + " " + nick + " :" + reason);
	chan->Write(client, "KICK " + channel + " " + nick + " :" + reason);
	Log::Info() << "User " << nick << " was kicked from channel " << channel << " by " << client->getNick() << " (" << reason << ")\n";
}
