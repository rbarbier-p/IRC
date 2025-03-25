#include "Bot.hpp"

int main(int ac, char *av[])
{
	Bot bender;
	if (ac != 4)
	{
		std::cout << "Usage: " << av[0] << " <ip> <port> <password> \n";
		return (1);
	}
	bender.run();
	return (0);
}
