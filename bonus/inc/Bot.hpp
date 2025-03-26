#ifndef BOT_HPP
# define BOT_HPP

# include <cerrno>
# include <cstring>
# include <fstream>
# include <unistd.h>
# include <stdlib.h>
# include <iostream>
# include <sys/types.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include "Timer.hpp"

# define NICK "Bender" //comentar a rene que pasa si el bot esta siendo usando
# define USER "botsito 0 * :bender"

class Bot
{
	//mensaje para dei del futuro :
	//
	//el servidor hace un socket luego un bind a la ip que queremos 

	//mensaje para dei del futuro :
	//
	//el servidor hace un socket luego un bind a la ip que queremos 
	//luego se queda escuchando desde el puerto habilitado
	//si nosotros queremos conectarnos al server debemos crear un socket
	//y luego utilizar la funcion connect
	//pasarle nuestro socket la ip que conectamos y el size de la struct 
	//y ya en principio estaria hecha la conexion.//luego se queda escuchando desde el puerto habilitado
	//si nosotros queremos conectarnos al server debemos crear un socket
	//y luego utilizar la funcion connect
	//pasarle nuestro socket la ip que conectamos y el size de la struct 
	//y ya en principio estaria hecha la conexion.
	private :
		struct sockaddr_in 	addr_;
		std::string			pass_;
		std::string			nick_;
		const std::string	user_;

		int initConnection(void);	
		void listenServer(int sock);	
		//necesita una estructura sockaddr_in necesito setear esa estructura 
		//conectarme al puerto y a la ip que me pasen por parametro.
		//necesito un socket
		//sin_family -> familia de direcciones se usa AF_INET para ipv4
		//sin_port -> puerto
		//sin_addr -> ipv4 
		//sin_zero[8] -> no se usa debe ser 0
	public :
		Bot(void);
		~Bot(void);

		void	run(void);
		
		struct sockaddr_in getAddr(void) const;
		std::string getPass() const;
		std::string getUser() const;
		std::string getNick() const;
		//setters
		void setNick(std::string nick);
		void setPass(std::string pass);
		void setIp(struct in_addr sin_addr);
		void setPort(unsigned short sin_port);
		void setFamily(short sin_family);
};

#endif /*BOT_HPP*/
