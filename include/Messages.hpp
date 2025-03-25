#ifndef MESSAGES_HPP
# define MESSAGES_HPP

# define RPL_WELCOME(nick, user, host) "001 " + (nick) + " :Welcome to the Internet Relay Network " + (nick) + "!" + (user) + "@" + (host)
# define RPL_YOURHOST(nick, servername, version) "002 " + (nick) + " :Your host is " + (servername) + ", running version " + (version)
# define RPL_CREATED(nick, date) "003 " + (nick) + " :This server was created " + (date)
# define RPL_MYINFO(nick, servername, version, channel_modes) "004 " + (nick) + " " + (servername) + " " + (version) + " " + (channel_modes)

# define RPL_UMODEIS(nick, modes) "221 " + (nick) + " +" + (modes)
# define RPL_CHANNELMODEIS(nick, channel, modes) "324 " + (nick) + " " + (channel) + " +" + (modes)
# define RPL_NOTOPIC(nick, target) "331 " + (nick) + " " + (target) + " :No topic is set"
# define RPL_TOPIC(nick, channel, topic) "332 " + (nick) + " " + (channel) + " :" + (topic)
# define RPL_INVITING(nick, target, channel) "341 " + (nick) + " " + (target) + " " + (channel)

# define ERR_NOSUCHNICK(nick, channel) "401 " + (nick) + " " + (channel) + " :No such nick or channel name"
# define ERR_NOSUCHCHANNEL(nick, target) "403 " + (nick) + " " + (target) + " :No such channel"
# define ERR_CANNOTSENDTOCHAN(nick, target) "404 " + (nick) + " " + (target) + " :Cannot send to channel"
# define ERR_NORECIPIENT(nick, target) "411 " + (nick) + " :No recipient given " + (target)
# define ERR_NOTEXTTOSEND(nick) "412 " + (nick) + " :No text to send"
# define ERR_UNKNOWNCOMMAND(nick, target) "421 " + (nick) + " " + (target) + " :Unknown command"
# define ERR_NICKNAMETOOLONG(nick, target) "432 " + (nick) + " " + (target) + " :Nickname too long"
# define ERR_ERRONEUSNICKNAME(nick, target) "432 " + (nick) + " " + (target) + " :Erroneous nickname"
# define ERR_NICKNAMEINUSE(nick, target) "433 " + (nick) + " " + (target) + " :Nickname already in use"
# define ERR_NOTONCHANNEL(nick, target) "442 " + (nick) + " " + (target) + " :You are not on that channel"
# define ERR_USERNOTINCHANNEL(nick, target, param) "441 " + (nick) + " " + (target) + " " + (param) + " :They aren't on that channel"
# define ERR_NOTREGISTERED(nick) "451 " + (nick) + " :Connection not registered"
# define ERR_NEEDMOREPARAMS(nick, target) "461 " + ((nick).empty() ? "*" : nick) + " " + (target) + " :Syntax error"
# define ERR_ALREADYREGISTRED(nick) "462 " + (nick) + " :Connection already registered"
# define ERR_CHANNELISFULL(nick, target) "471 " + (nick) + " " + (target) + " :Cannot join channel (+l) -- Channel is full, try later"
# define ERR_UNKNOWNMODE(nick, target, param) "472 " + (nick) + " " + (param) + " :is unknown mode char for " + (target)
# define ERR_INVITEONLYCHAN(nick, target) "473 " + (nick) + " " + (target) + " :Cannot join channel (+i) -- Invited users only"
# define ERR_BADCHANNELKEY(nick, target) "475 " + (nick) + " " + (target) + " :Cannot join channel (+k) -- Wrong channel key"
# define ERR_NOCHANMODES(nick, target) "477 " + (nick) + " " + (target) + " :Channel doesn't support modes"
# define ERR_INVALIDMODEPARAM(nick, target, param) "696 " + (nick) + " " + (target) + " " + (param) + " * :Invalid mode parameter"
# define ERR_CHANOPPRIVTOOLOW(nick, target) "482 " + (nick) + " " + (target) + " :Your privileges are too low"
# define ERR_CHANOPRIVSNEEDED(nick, target) "482 " + (nick) + " " + (target) + " :You are not channel operator"
# define ERR_USERSDONTMATCH(nick) "502 " + (nick) + " :Can't set/get mode for other users"

#endif /* MESSAGES_HPP */
