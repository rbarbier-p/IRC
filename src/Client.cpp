#include "Client.hpp"

Client::Client(int fd, short events) : last_time_(time(0)), registered_(false)
{
	poll_.fd = fd;
	poll_.events = events;
}

Client::~Client(void)
{
}

std::string Client::getReadBuffer(void) const
{
	return (this->rbuffer_);
}

std::string Client::getWriteBuffer(void) const
{
	return (this->wbuffer_);
}

std::string Client::getNick(void) const
{
	return (this->nick_);
}

void Client::setNick(std::string nick)
{
	this->nick_ = nick;
}

std::string Client::getUser(void) const
{
	return (this->user_);
}

void Client::setUser(std::string user)
{
	this->user_ = user;
}

std::string Client::getPassword(void) const
{
	return (this->password_);
}

void Client::setPassword(std::string password)
{
	this->password_ = password;
}

void Client::setReadBuffer(std::string buffer)
{
	this->rbuffer_ += buffer;
}

void Client::setWriteBuffer(std::string buffer)
{
	this->wbuffer_ = buffer;
}

int Client::getFd(void) const
{
	return (this->poll_.fd);
}

void Client::setFd(int fd)
{
	this->poll_.fd = fd;
}

short Client::getEvents(void) const
{
	return (this->poll_.events);
}

void Client::setEvents(short events)
{
	this->poll_.events |= events;
}

time_t Client::getLastTime(void) const
{
	return (this->last_time_);
}

void Client::setLastTime(time_t last_time)
{
	this->last_time_ = last_time;
}

bool Client::isRegistered(void) const
{
	return (this->registered_);
}

void Client::setRegistered(bool registered)
{
	this->registered_ = registered;
}

void Client::unsetEvent(short event)
{
	this->poll_.events &= ~event;
}

void Client::unsetReadBuffer(void)
{
	this->rbuffer_.clear();
}

void Client::unsetWriteBuffer(void)
{
	this->wbuffer_.clear();
}

void Client::Exit(void)
{
	std::vector<Client *>::iterator it(Server::clients.begin());
	while (it != Server::clients.end())
	{
		delete (*it);
		++it;
	}
}

bool Client::IsValidNick(const std::string &nick)
{
	if (nick.size() > MAX_NICK_LEN)
		return (false);
	if (nick.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\`_^{|}-") != std::string::npos)
		return (false);
	return (true);
}

Client *Client::Search(const std::string &nick)
{
	std::vector<Client *>::iterator it(Server::clients.begin());
	while (it != Server::clients.end())
	{
		if ((*it)->getNick() == nick)
			return (*it);
		++it;
	}
	return (0);
}

void Client::Mode(Client *client, std::vector<std::string> &params,
	Client *target)
{
	if (target != client)
		return (client->WriteErr(ERR_USERSDONTMATCH(client->getNick())));
	if (params.size() == 1)
		return (client->Write(RPL_UMODEIS(client->getNick(), "")));
}

void Client::Destroy(Client *client)
{
	std::vector<Client *>::iterator it(Server::clients.begin());
	while (it != Server::clients.end())
	{
		if (*it == client)
		{
			Log::Info() << "Shutting down connection " << (*it)->getFd() << " ...";
			close((*it)->getFd());
			delete (*it);
			Server::clients.erase(it);
			break ;
		}
		++it;
	}
}

std::string Client::getPrefix(void) const
{
	return (nick_ + "!" + user_ + "@unknown");
}

void Client::Write(const std::string &message)
{
	this->wbuffer_ += ":" + getPrefix() + " " + message + '\n';
}

void Client::Write(const std::string prefix, const std::string &message)
{
	this->wbuffer_ += ":" + prefix + " " + message + '\n';
}

void Client::WriteErr(const std::string &message)
{
	this->wbuffer_ += ": " + message + '\n';
}

void Client::WriteRpl(const std::string &message)
{
	this->wbuffer_ += ": " + message + '\n';
}
