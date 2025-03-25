#include "Bot.hpp"

int main(int ac, char *av[])
{
	if (ac != 4)
	{
		std::cout << "Usage: " << av[0] << " <ip> <port> <password> \n";
		return (1);
	}
	return (0);
}
