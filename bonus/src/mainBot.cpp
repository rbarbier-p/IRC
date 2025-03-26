#include "Bot.hpp"

static struct in_addr ip_parse(char *av)
{
	struct in_addr ip;
	ip.s_addr = inet_addr(av);
	if (ip.s_addr == 0xFFFFFFFF)
	{
		std::cerr << "illegal ip number " << av << "!\n";
		exit(EXIT_FAILURE);
	}
	return (ip);
}

static unsigned short port_parse(char *av)
{
	char *endptr;
	int port(strtol(av, &endptr, 10));
	if (port > 0 && port < 0xFFFF && *endptr == '\0')
		return (static_cast<unsigned short>(port));
	else
	{
		std::cerr << "illegal port number " << av << "!\n";
		exit(EXIT_FAILURE);
	}
	return (0);
}

static void parse(Bot &bot, char *av[])
{
	bot.setIp(ip_parse(av[1]));
	bot.setPort(htons(port_parse(av[2])));
	bot.setFamily(AF_INET);
	bot.setPass(av[3]);
}

int main(int ac, char *av[])
{
	if (ac != 4)
	{
		std::cout << "Usage: " << av[0] << " <ip> <port> <password> \n";
		return (1);
	}
	Bot bender;
	parse(bender, av);
	bender.run();
	return (0);
}
