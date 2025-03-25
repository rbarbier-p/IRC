#include "Server.hpp"

int	main(int argc, char **argv)
{
	Server *server(new Server(argc, argv));
	server->Run();
	delete (server);
	return (EXIT_SUCCESS);
}
